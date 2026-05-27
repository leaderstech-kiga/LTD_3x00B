/**
*   @file       a96l41x_bit.c
*   @brief      A96L41x BIT(Basic Interval Timer) peripheral function 
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
#ifndef __BIT_H_
#define __BIT_H_
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
enum BITClk { 
    BIT_DIV4096 = 0,
		BIT_DIV1024,
		BIT_DIV128,
		BIT_DIV16,
};

enum BITPeriod { 
    BIT_X2 = 0,
		BIT_X4,
		BIT_X8,
		BIT_X16,
		BIT_X32,
		BIT_X64,
		BIT_X128,
		BIT_X256,
};

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void BIT_Initial(uint8_t clock_div, uint8_t period_sel);
void BIT_Interrupt_config(uint8_t enable);
uint8_t BIT_GetInterruptStatus(void);
void BIT_ClearInterruptStatus(void);
uint8_t BIT_GetCountValue(void);
void BIT_ClearCountValue(void);

#endif  /* End of __BIT_H_ */
/* --------------------------------- End Of File ------------------------------ */