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

/*******************************************************************************
* Included File
*******************************************************************************/
#include "a96l41x_bit.h"
#include "a96l41x.h"

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
* @brief		Initialize BIT peripheral
* @param   clock_div	This parameter contains the selection of clock.
*
*					- BIT_DIV4096 			= 0
*					- BIT_DIV1024 			= 1
*					- BIT_DIV128 			= 2
*					- BIT_DIV16 			= 3
* @param   period_sel	This parameter contains the period of BIT.
*
*					- BIT_X2 			= 0
*					- BIT_X4 			= 1
*					- BIT_X8 			= 2
*					- BIT_X16			= 3
*					- BIT_X32			= 4
*					- BIT_X64			= 5
*					- BIT_X128			= 6
*					- BIT_X256			= 7
* @return			None
*/
void BIT_Initial(uint8_t clock_div, uint8_t period_sel)
{
	uint8_t temp_reg;
	
	temp_reg = BITCR;
	temp_reg &= ~((0x7 << 5) | (0x7 << 0));
	temp_reg |= ((clock_div << 5) | (period_sel << 0) | (1 << 3));
	
	BITCR = temp_reg;
}


/**
* @brief		Config BIT interrupt
* @param   enable	This parameter contains the enable of interrupt.
*
*					- FALSE = 0
*					- TRUE = 1
* @return			None
*/
void BIT_Interrupt_config(uint8_t enable)
{
	if(enable)
	{
			IE3 |=0x10;
	}
	else
	{
			IE3 &= ~(0x10);
	}
}

/**
* @brief		Get the BIT interrupt status.
* @param   		None
* @return		Value of status(interrupt occur : 1, not occur : 0)
*/
uint8_t BIT_GetInterruptStatus(void)
{
	return ((BITCR & 0x80) >> 7);
}

/**
* @brief		Clear the BIT interrupt status.
* @param   		None
* @return		None
*/
void BIT_ClearInterruptStatus(void)
{
	BITCR &= ~(0x80);
}

/**
* @brief		Get the value of BIT count.
* @param   		None
* @return		Value of BIT count(0 ~ 0xFF)
*/
uint8_t BIT_GetCountValue(void)
{
	return BITCNT;
}

/**
* @brief		Clear the value of BIT count.
* @param   		None
* @return		None
*/
void BIT_ClearCountValue(void)
{
	BITCR |= 0x08;
}

/* --------------------------------- End Of File ------------------------------ */