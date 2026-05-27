/**
* @file       a96l41x_opamp.c
* @brief      A96L41x OPAMP peripheral function 
* @author     Application Team, ABOV Semiconductor Co., Ltd.
* @version    V1.00
* @date       29. MAY. 2020
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
#include "a96l41x_opamp.h"

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
* @brief		Initialize OPAMP peripheral
* @param   gain_1sel			This parameter contains the selection of OP1 gain select .
*
*			- GAIN1_DIS	 	= 0
*			- X1			= 1
*			- X2 			= 2
*			- X10 			= 3
*			- X15 			= 4
*			- X20 			= 5
*			- X30 			= 6
*			- X60 			= 7
* @param   gain_0sel		This parameter contains the selection of OP0 gain select.
*
*			- GAIN0_DIS	 	= 0
*			- X5			= 1
*			- X10 			= 2
*			- X20 			= 3
* @param   chp_clk		This parameter contains the selection of OPAMP chopper clock select.
*
*			- 125KHz	 	= 0
*			- 167KHz		= 1
* @return		None
*/
void OPAMP_Initial(uint8_t gain_1sel, uint8_t gain_0sel, uint8_t chp_clk)
{
	uint8_t temp_reg, temp_reg2;
	
	temp_reg = AMPCR1;
	temp_reg &= ~(0xFF);
	temp_reg |= ((gain_1sel << 4) | (gain_0sel << 0));
	
	AMPCR1 = temp_reg;
	
	temp_reg2 = CHPCR;
	temp_reg2 &= ~(0xFF);
	temp_reg2 |= (chp_clk << 0);
	
	CHPCR = temp_reg2;
	
}

/**
* @brief		Enables or disables the OP1 peripheral.
* @param   enable   This parameter contains the enable of this function. 
*
*			- FALSE 	= 0
*			- TRUE 		= 1
* @return		None
*/
void OP1_Enable(uint8_t enable)
{
	if(enable == TRUE)
	{
		AMPCR1 |= (1 << 7);
	}
	else
	{
		AMPCR1 &= ~(1 << 7);
	}
}
/**
* @brief		Enables or disables the OP0 peripheral.
* @param   enable   This parameter contains the enable of this function. 
*
*			- FALSE 	= 0
*			- TRUE 		= 1
* @return		None
*/
void OP0_Enable(uint8_t enable)
{
	if(enable == TRUE)
	{
		AMPCR1 |= (1 << 3);
	}
	else
	{
		AMPCR1 &= ~(1 << 3);
	}
}

/* --------------------------------- End Of File ------------------------------ */
