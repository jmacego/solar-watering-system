/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430.h>

/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/Grace.h>

/*
 *  ======== main ========
 */
int main(void)
{
    Grace_init();                   // Activate Grace-generated configuration
    
    // >>>>> Fill-in user code here <<<<<
    /* ADC Start Conversion - Software trigger */
ADC10CTL0 |= ADC10SC;/* ADC Start Conversion - Software trigger */
ADC10CTL0 |= ADC10SC;

/* Loop until ADC10IFG is set indicating ADC conversion complete */
while ((ADC10CTL0 & ADC10IFG) == 0);

/* Read ADC conversion result from ADC10MEM */
ADC_Conversion_Result = ADC10MEM;

    return (0);
}
