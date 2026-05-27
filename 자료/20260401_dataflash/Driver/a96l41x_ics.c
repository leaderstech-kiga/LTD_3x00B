/**
*   @file       a96l41x_ics.c
*   @brief      A96L41x ics peripheral function 
*   @author     Application Team, ABOV Semiconductor Co., Ltd.
*   @version    V1.01
*   @date       18. DEC. 2020
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

/*******************************************************************************
* Included File
*******************************************************************************/
#include "a96l41x_ics.h"

/*******************************************************************************
* Private Pre-processor Definition & Macro
*******************************************************************************/

/*******************************************************************************
* Private Typedef
*******************************************************************************/

/*******************************************************************************
* Private Variable
*******************************************************************************/

/*******************************************************************************
* Private Function Prototype
*******************************************************************************/

/*******************************************************************************
* Public Function
*******************************************************************************/

/**
* @brief		Initialize ICS peripheral
* @param   ICSx_select		This parameter contains the select of ICSx.
*
*    					- Disable_ICS 					= 0
*					- Enable_ICS0 					= 5
*					- Enable_ICS1 					= A
* @param   current_select	This parameter contains the selection of Current.
*
*					- ICS_49 						= 0
*					- ICS_65 						= 1
*					- ICS_80 						= 2
*					- ICS_96 						= 3
*					- ICS_111 						= 4
*					- ICS_127 						= 5
*					- ICS_142 						= 6
*					- ICS_158 						= 7
*					- ICS_173 						= 8
*					- ICS_188 						= 9
*					- ICS_203 						= A
*					- ICS_218 						= B
*					- ICS_232 						= C
*					- ICS_246 						= D
*					- ICS_260 						= E
*					- ICS_274 						= F
* @return		None
*/
void ICS_Initial(uint8_t ICSx_select, uint8_t current_select)
{
	uint8_t temp_reg;
	if(Enable_ICS0 == ICSx_select)
	{
		temp_reg = (current_select << 0);
		ICSDR0 = temp_reg;
		ICSCR = Enable_ICS0;
	}
	else if(Enable_ICS1 == ICSx_select)
	{
		temp_reg = (current_select << 0);
		ICSDR1 = temp_reg;
		ICSCR = Enable_ICS1;
	}
	else
	{
		ICSCR = Disable_ICS;
	}
}

/* --------------------------------- End Of File ------------------------------ */
