#include <msp430.h>
#include "_Grace.h"

/* USER CODE START (section: InterruptVectors_init_c_prologue) */
/* User defined includes, defines, global variables and functions */
/* USER CODE END (section: InterruptVectors_init_c_prologue) */

/*
 *  ======== InterruptVectors_graceInit ========
 */
void InterruptVectors_graceInit(void)
{
}


/* Interrupt Function Prototypes */


/*
 *  ======== ADC10 Interrupt Service Routine ========
 */
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR_HOOK(void)
{
    /* USER CODE START (section: ADC10_ISR_HOOK) */
    /* replace this comment with your code */
    /* USER CODE END (section: ADC10_ISR_HOOK) */
}

/*
 *  ======== NMI_VECTOR Interrupt Service Routine ========
 *
 *  You can check which interrupt flag was triggered here.
 *  Copy and paste the following function as desired.
 *
    if (IFG1 & NMIIFG) {

    }
    else if (IFG1 & OFIFG) {

    }
    else if (IFG1 & ACCVIFG) {

    }
 *
 */
#pragma vector=NMI_VECTOR
__interrupt void NMI_ISR_HOOK(void)
{
    /* USER CODE START (section: NMI_ISR_HOOK) */
    /* replace this comment with your code */
    /* USER CODE END (section: NMI_ISR_HOOK) */
}
