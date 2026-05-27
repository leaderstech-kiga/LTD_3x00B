/**
*   @file       a96l41x_pwr.c
*   @brief      A96l41x power mode function 
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

/*******************************************************************************
* Included File
*******************************************************************************/
#include "a96l41x_pwr.h"

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
* @brief		Enter Idle mode for power function.
* @param 	None
* @return		None
*/
void PWR_EnterIdle(void)
{
	PCON = PWR_IDLE_MODE;
	
	/* Must execute NOP */ 
	NOP;
	NOP;
	NOP;
}


/**
* @brief		Enter Stop mode for power function.
* @param 	None
* @return		None
*/
void PWR_EnterStop(void)
{
	PCON = PWR_STOP_MODE;
	
	/* Must execute NOP */ 
	NOP;
	NOP;
	NOP;
}

/**
* @brief		Get status of Reset status.
* @param   None
* @return		Status of Reset flag.
*/
uint8_t PWR_GetResetStatus(void)
{
	return (RSTFR & 0xF8);
}

/**
* @brief		Clear status of Reset status.
* @param   flag 		status of Reset flag
*
*					- POR_FLAG			= 0x80
*					- EXT_RESET_FLAG	= 0x40
*					- WDT_RESET_FLAG 	= 0x20
*					- OCD_RESET_FLAG 	= 0x10
*					- LVR_RESET_FLAG 	= 0x08
* @return		None
*/
void PWR_ClearResetStatus(uint8_t flag)
{
	RSTFR = ~flag;
}

/**
* @brief		Configure low voltage reset function.
* @param   select_level		This parameter contains the selection of LVR level.
*
*	- 		LVR_1_60		 	= 0
*	-		LVR_2_20		 	= 1
*	-		LVR_2_40		 	= 2
*	-		LVR_2_70		 	= 3
* @param   enable   This parameter contains the enable of this function. 
* 					- FALSE = 0
*					- TRUE = 1
* @return		None
*/
void PWR_ConfigureLVR(uint8_t select_level, uint8_t enable)
{
	uint8_t temp_reg;
	
	temp_reg = LVRCR;
	temp_reg &= ~(0x07);
	
	if(enable)
		temp_reg |= ((select_level << 1) | (0 << 0));
	else
		temp_reg |= ((select_level << 1) | (1 << 0));

	LVRIDR = 0x59;	
	LVRCR = temp_reg;
}

/* --------------------------------- End Of File ------------------------------ */