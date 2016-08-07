/******************************************************************************
* Get calibrated temperature readings from internal temerature sensor.
* Print them to UART serial console (launchpad standard pin 1.0 1.1) when
* any key is pressed on keyboard.
*
* At some point it really should do more if it's just going to be a serial
* temperature sensor... but it is going to switch to I2C with USI to connnect
* to a Raspberry Pi and give various ADC readings on demand.
*
* MSP430G2452
*******************************************************************************/
/**
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>
#include <ti/mcu/msp430/Grace.h>
/*
 * ======== Grace related declaration ========
 */


/*
 *  ======== main ========
 */



//------------------------------------------------------------------------------
// Hardware-related definitions
//------------------------------------------------------------------------------
#define UART_TXD   0x02                     // TXD on P1.1 (Timer0_A.OUT0)
#define UART_RXD   0x04                     // RXD on P1.2 (Timer0_A.CCI1A)

//------------------------------------------------------------------------------
// Conditions for 9600 Baud SW UART, SMCLK = 8/8MHz
//------------------------------------------------------------------------------
#define UART_TBIT_DIV_2     (1000000 / (9600 * 2))
#define UART_TBIT           (1000000 / 9600)

//------------------------------------------------------------------------------
// Global variables used for full-duplex UART communication
//------------------------------------------------------------------------------
unsigned int txData;                        // UART internal variable for TX
unsigned char rxBuffer;                     // Received UART character
char SLV_Data = 0;                     // Variable for transmitted data
char SLV_Addr = 0x90;                  // Address is 0x48<<1 for R/W
int I2C_State = 0;                     // State variable

//------------------------------------------------------------------------------
// Function prototypes
//------------------------------------------------------------------------------
void TimerA_UART_init(void);
void TimerA_UART_tx(unsigned char byte);
void TimerA_UART_print(char *string);
void UART_print_temp(long temp);
unsigned verify_info_chk(const unsigned * const begin, const unsigned * const end);
void const *find_tag(const unsigned * const begin, const unsigned * const end, const unsigned tag);

typedef struct {
    unsigned adc_gain;                                  // ADC gain
    unsigned adc_offet;                                 // ADC offset
    unsigned ref15;                                     // ADC value of 1.5 volt input when using 1.5 volt reference
    unsigned t3015;                                     // ADC value of 30C when using 1.5 volt reference
    unsigned t8515;                                     // ADC value of 85C when using 1.5 volt reference
    unsigned ref25;                                     // ADC value of 2.5 volt input when using 2.5 volt reference
    unsigned t3025;                                     // ADC value of 30C when using 2.5 volt reference
    unsigned t8525;                                     // ADC value of 85C when using 2.5 volt reference
} TCAL;

//------------------------------------------------------------------------------
// main()
//------------------------------------------------------------------------------
int main(void)
{
	Grace_init();                    // Activate Grace-generated configuration
    WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer
    if (CALBC1_8MHZ==0xFF)					// If calibration constants erased
    {
      while(1);                             // do not load, trap CPU!!
    }
    unsigned adc;
    int dcc, dkc, dfc;                                  // Calibrated temperatures

    const unsigned * const info_seg_a = (unsigned *)0x10C0;     // Address of info segement A
    const unsigned * const info_seg_a_end = info_seg_a + 32;    // 32 words in each segment

    P1OUT = 0x00;                           // Initialize all GPIO
    P1SEL = UART_TXD + UART_RXD;            // Timer function for TXD/RXD pins
    P1DIR = 0xFF & ~UART_RXD;               // Set all pins but RXD to output
    P2OUT = 0x00;
    P2SEL = 0x00;
    P2DIR = 0xFF;

    __enable_interrupt();

    const TCAL * const cal = (TCAL *)(verify_info_chk(info_seg_a, info_seg_a_end) \
		 ? 0 \
		 : find_tag(info_seg_a, info_seg_a_end, 0x08));
	const long cc_scale  = cal ? 3604480L / (cal->t8515 - cal->t3015) : 0;
	const long cf_scale  = cal ? 6488064L / (cal->t8515 - cal->t3015) : 0;
	const long cc_offset = cal ? 1998848L - (cal->t3015 * cc_scale) : 0;
	const long cf_offset = cal ? 5668864L - (cal->t3015 * cf_scale) : 0;
	const long ck_offset = cc_offset + 17901158L;

    TimerA_UART_tx(12);
    TimerA_UART_print("G2xx2 TimerA UART\r\n");
    TimerA_UART_print("READY.\r\n");

	// Wait for incoming character
	__bis_SR_register(LPM0_bits);

	while(1)
	{
        ADC10CTL0 |= (ENC | ADC10SC);                   // Begin ADC conversion
		__bis_SR_register(CPUOFF + GIE);        // LPM0 with interrupts enabled

        adc = ADC10MEM;                                 // Read ADC
                                                        //
        dcc = ((cc_scale * adc) + cc_offset) >> 16;     // C calibrated
        dkc = ((cc_scale * adc) + ck_offset) >> 16;     // K calibrated
        dfc = ((cf_scale * adc) + cf_offset) >> 16;     // F calibrated

		// Echo temperature
		TimerA_UART_print("The temperature is currently: ");
		UART_print_temp(dfc);
		TimerA_UART_print("F, ");
		UART_print_temp(dcc);
		TimerA_UART_print("C, ");
		UART_print_temp(dkc);
		TimerA_UART_print("K\r\n");
		__bis_SR_register(LPM0_bits);

    }
}

/**
 * Verify checksum of calibration constants
 */
unsigned verify_info_chk(const unsigned * const begin, const unsigned * const end)
{
    const unsigned *p = begin + 1;                      // Begin at word after checksum
    unsigned chk = 0;                                   // Init checksum
    while(p < end) chk ^= *p++;                         // XOR all words in segment
    return chk + *begin;                                // Add checksum - result will be zero if checksum is valid
}

void const *find_tag(const unsigned * const begin, const unsigned * const end, const unsigned tag)
{
    const unsigned *p = begin + 1;                      // Begin at word after checksum
    do {                                                //
        const unsigned d = *p++;                        // Get a word
        if((d & 0xFF) == tag) return (void *)p;         // Return pointer if tag matches
        p += (d >> 9);                                  // Point to next tag
    } while(p < end);                                   // While still within segment
    return 0;                                           // Not found, return NULL pointer
}



/**
 * Send digits to UART
 */
void UART_print_temp(long temp)
{
	if (temp < 0)
	{
		TimerA_UART_print("-");
	}
	if (temp >= 100 || temp <= -100)
	{
		TimerA_UART_tx(0x30+((temp/100)%10));
	}
	if (temp >= 10 || temp <= -10)
	{
		TimerA_UART_tx(0x30+((temp/10)%10));
	}
	TimerA_UART_tx(0x30+(temp%10));
}
//------------------------------------------------------------------------------
// Function configures Timer_A for full-duplex UART operation
//------------------------------------------------------------------------------
void TimerA_UART_init(void)
{
    TACCTL0 = OUT;                          // Set TXD Idle as Mark = '1'
    TACCTL1 = SCS + CM1 + CAP + CCIE;       // Sync, Neg Edge, Capture, Int
    TACTL = TASSEL_2 + MC_2;                // SMCLK, start in continuous mode
}
//------------------------------------------------------------------------------
// Outputs one byte using the Timer_A UART
//------------------------------------------------------------------------------
void TimerA_UART_tx(unsigned char byte)
{
    while (TACCTL0 & CCIE);                 // Ensure last char got TX'd
    TACCR0 = TAR;                           // Current state of TA counter
    TACCR0 += UART_TBIT;                    // One bit time till first bit
    TACCTL0 = OUTMOD0 + CCIE;               // Set TXD on EQU0, Int
    txData = byte;                          // Load global variable
    txData |= 0x100;                        // Add mark stop bit to TXData
    txData <<= 1;                           // Add space start bit
}

//------------------------------------------------------------------------------
// Prints a string over using the Timer_A UART
//------------------------------------------------------------------------------
void TimerA_UART_print(char *string)
{
    while (*string) {
        TimerA_UART_tx(*string++);
    }
}
