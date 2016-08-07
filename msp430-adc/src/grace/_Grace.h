
/*
 *  ======== _Grace.h ========
 *  DO NOT MODIFY THIS FILE - CHANGES WILL BE OVERWRITTEN
 */

#ifdef __cplusplus
extern "C" {
#endif

/* external peripheral initialization functions */
void GPIO_graceInit(void);
void BCSplus_graceInit(void);
void ADC10_graceInit(void);
void USI_graceInit(void);
void InterruptVectors_graceInit(void);
void Timer0_A3_graceInit(void);
void System_graceInit(void);
void WDTplus_graceInit(void);
void Grace_init(void);

#ifdef __cplusplus
}
#endif

