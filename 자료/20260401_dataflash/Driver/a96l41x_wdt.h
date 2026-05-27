/**
*   @file       a96l41x_wdt.h
*   @brief      A96L41x WDT file
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

#ifndef __WDT_H_
#define __WDT_H_
/*******************************************************************************
* Included File
*******************************************************************************/
#include    "typedef.h"  
#include "A96L41x.h"
/*******************************************************************************
* Public Macro
*******************************************************************************/

/*******************************************************************************
* Public Typedef
*******************************************************************************/
enum WDT_rst { 
	WDT_FREE_RUN = 0,
	WDT_RST_ON
};

enum WDT_clk { 
	WDT_BIT_OVER = 0,
	WDT_LFIRC,
	WDT_WDTRC,
};

enum WDT_interrupt { 
	WDT_INT_DIS = 0,
	WDT_INT_EN
};

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void WDT_Initial(uint8_t rst_on, uint8_t wdtck);
void WDT_SetDATA(uint8_t count) ;
void WDT_Set_500msec_Reset(void);
void WDT_Set_1sec_Reset(void);
void WDT_Set_2sec_Reset(void);
void WDT_Set_4sec_Reset(void);
void WDT_Set_4sec_Reset_WDTRCEN(void);
uint8_t WDT_GetCountValue(void);
void WDT_ClearCountData(void);
void WDT_Start(void);
void WDT_Stop(void);
void WDT_ConfigureInterrrupt(uint8_t enable);

#endif  // __WDT_H_
/* --------------------------------- End Of File ------------------------------ */
