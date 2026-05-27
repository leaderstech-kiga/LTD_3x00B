/**
*   @file       a96l41x_ics.h
*   @brief      A96L41x ics peripheral function 
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
#ifndef __ICS_H_
#define __ICS_H_
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
enum ICSx_select { 
    Disable_ICS = 0,
	Enable_ICS0	= 5,
	Enable_ICS1	= 0x0A,
};

enum current_select { 
    ICS_49  = 0,
	ICS_65, 
	ICS_80,
	ICS_96, 
	ICS_111,
	ICS_127,
	ICS_142,
    ICS_158,
	ICS_173,
	ICS_188,
	ICS_203,
	ICS_218,
	ICS_232,
	ICS_246,
	ICS_260,
	ICS_274,
};

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void ICS_Initial(uint8_t ICSx_select, uint8_t current_select);
#endif  /* End of __ICS_H_ */
/* --------------------------------- End Of File ------------------------------ */
