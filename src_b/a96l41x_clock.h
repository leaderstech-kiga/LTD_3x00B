/**
*   @file       a96l41x_clock.c
*   @brief      A96L41x System clock peripheral function 
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
#ifndef __CLOCK_H_
#define __CLOCK_H_
/*******************************************************************************
* Included File
*******************************************************************************/
#include "A96L41x.h"
#include "typedef.h"


/*******************************************************************************
* Public Macro
*******************************************************************************/
#define SYSTEM_CLOCK		4000000

/*******************************************************************************
* Public Typedef
*******************************************************************************/
enum SystemClk { 
	HFI4_DIV8 = 0,
	HFI4_DIV4,
	HFI4_DIV2,
	HFI4_DIV1,
	LFI
};

enum OSCSel { 
    HFIRC = 0,
    LFIRC
};

enum HfircDiv { 
    IRC4_DIV8 = 0,
    IRC4_DIV4,
    IRC4_DIV2,
    IRC4_DIV1,
};

enum OSCEnable { 
    OSC_DIS = 0,
    OSC_EN
};


/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void Clock_Initial(uint8_t clock_sel);
void Clock_ConfigureOSC(uint8_t osc_sel, uint8_t hsirc_div, uint8_t osc_enable);
uint32_t Clock_GetSystemFreq(void);
#endif  /* End of __CLOCK_H_ */
/* --------------------------------- End Of File ------------------------------ */
