/**
*   @file       a96l41x_timer2.h
*   @brief      A96L41x timer2 peripheral function 
*   @author     Application Team, ABOV Semiconductor Co., Ltd.
*   @version    V1.00
*   @date       29. MAY. 2020
*
* Copyright(C) 2020, ABOV Semiconductor
* All rights reserved.
*
********************************************************************************
* ABOV Disclaimer
*
*IMPORTANT NOTICE - PLEASE READ CAREFULLY
*ABOV Semiconductor ("ABOV") reserves the right to make changes, corrections, enhancements, 
*modifications, and improvements to ABOV products and/or to this document at any time without notice. 
*ABOV does not give warranties as to the accuracy or completeness of the information included herein.
*Purchasers should obtain the latest relevant information of ABOV products before placing orders. 
*Purchasers are entirely responsible for the choice, selection, and use of ABOV products and 
*ABOV assumes no liability for application assistance or the design of purchasers' products. No license, 
*express or implied, to any intellectual property rights is granted by ABOV herein. 
*ABOV disclaims all express and implied warranties and shall not be responsible or
*liable for any injuries or damages related to use of ABOV products in such unauthorized applications. 
*ABOV and the ABOV logo are trademarks of ABOV.
*All other product or service names are the property of their respective owners. 
*Information in this document supersedes and replaces the information previously
*supplied in any former versions of this document.
*2020 ABOV Semiconductor  All rights reserved
*
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER2_H_
#define __TIMER2_H_
/*******************************************************************************
* Included File
*******************************************************************************/
#include "A96L41x.h"
#include "typedef.h"


/*******************************************************************************
* Public Macro
*******************************************************************************/

/*******************************************************************************
* Public Typedef
*******************************************************************************/
enum Timer2_mode { 
    T2_COUNTER_MODE = 0,
		T2_CAPTURE_MODE,
		T2_PPG_ONESHOT_MODE,
		T2_PPG_REPEAT_MODE,
};

enum Timer2_PPG_polarity { 
    T2_START_HIGH = 0,
		T2_START_LOW,
};

enum Timer2_freq_div { 
    TIMER2_DIV2048 = 0,
		TIMER2_DIV512,
		TIMER2_DIV64,
		TIMER2_DIV8,
		TIMER2_DIV4,
		TIMER2_DIV2,  
		TIMER2_DIV1,
    TIMER2_EC2,
};

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void Timer2_Initial(uint8_t mode, uint8_t freq);
void Timer2_SetExternalClockEdge(uint8_t edge);
void Timer2_SetMatchCounter(uint16_t match_count);
void Timer2_SetPPGPolarity(uint8_t polarity);
void Timer2_SetPPGDutyCounter(uint16_t count);
void Timer2_SetPPGPeriodCounter(uint16_t count);
uint16_t Timer2_GetCaptureValue(void);
void Timer2_ClearCountData(void);
void Timer2_Start(void);
void Timer2_Stop(void);
void Timer2_ConfigureInterrupt(uint8_t enable);
void Timer2_ClearInterruptFlag(void);

#endif  /* End of __TIMER2_H_ */
/* --------------------------------- End Of File ------------------------------ */