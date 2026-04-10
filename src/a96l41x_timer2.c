/**
*   @file       a96l41x_timer2.c
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

/*******************************************************************************
* Included File
*******************************************************************************/
#include "a96l41x_timer2.h"

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
* @brief		Initialize timer2 peripheral
* @param   mode		This parameter contains the mode of timer.
*
*					- T2_COUNTER_MODE						= 0
*					- T2_CAPTURE_MODE						= 1
*					- T2_PPG_ONESHOT_MODE 					= 2
*					- T2_PPG_REPEAT_MODE 					= 3
* @param   freq	This parameter contains the selection of frequency.
*
*					- TIMER2_DIV2048 						= 0
*					- TIMER2_DIV512 						= 1
*					- TIMER2_DIV64 							= 2
*					- TIMER2_DIV8 							= 3
*					- TIMER2_DIV4							= 4
*					- TIMER2_DIV2							= 5
*					- TIMER2_DIV1							= 6
*					- TIMER2_EC2							= 7
* @return		None
*/
void Timer2_Initial(uint8_t mode, uint8_t freq)
{
	uint8_t temp_reg, temp_reg2;
	
	temp_reg = T2CRH;
	/* Set timer mode & frequency source */
	temp_reg &= ~(0x3 << 4);
	temp_reg |= ((mode << 4) | (0x1 << 0));
	
	temp_reg2 = T2CRL;
	temp_reg2 &= ~((0x7 << 5) | (0x1 << 0));
	if(mode >= T2_PPG_ONESHOT_MODE)
		temp_reg2 |= (freq << 5);
	else
		temp_reg2 |= ((freq << 5) | (0x1 << 0));
	
	T2CRH = temp_reg;
	T2CRL = temp_reg2;
}

/**
* @brief	 Set Timer2 Counter mode operation
* @param   match_count		This parameter contains the value of timer match count(0~0xFFFF).
* @return		None
*/
void Timer2_SetMatchCounter(uint16_t match_count) 
{
	T2ADR = match_count;
}

/**
* @brief	 Set Timer2 PPG Polarity
* @param   polarity		This parameter contains the value of polarity.
*
*					- T2_START_HIGH 						= 0
*					- T2_START_LOW 							= 1
* @return		None
*/
void Timer2_SetPPGPolarity(uint8_t polarity) 
{
	uint8_t temp_reg;
	
	temp_reg = T2CRL;
	temp_reg &= ~(0x04); 
	temp_reg |= (polarity << 2);
	
	T2CRL = temp_reg;
}

/**
* @brief	 Set Timer2 PPG mode duty counter
* @param   count		This parameter contains the value of timer duty count(0~0xFFFF).
* @return		None
*/
void Timer2_SetPPGDutyCounter(uint16_t count) 
{
	T2BDR = count;
}

/**
* @brief	 Set Timer2 PPG mode period counter
* @param   count		This parameter contains the value of timer period count(1~0xFFFF).
* @return		None
*/
void Timer2_SetPPGPeriodCounter(uint16_t count) 
{
	T2ADR = count;
}


/**
* @brief	 Get Timer2 Capture mode operation
* @param   None
* @return		The value of timer capture data(16bit)
*/
uint16_t Timer2_GetCaptureValue(void) 
{
	return T2BDR;
}

/**
* @brief	 Clear the value of Timer2 counter data 
* @param   None
* @return		None
*/
void Timer2_ClearCountData(void) 
{
	T2CRH |= 0x01;
}

/**
* @brief		Start Timer2 operation
* @param   None
* @return		None
*/
void Timer2_Start(void) 
{
	T2CRH |= 0x80;
}

/**
* @brief		Stop Timer2 operation
* @param   None
* @return		None
*/
void Timer2_Stop(void) 
{
	T2CRH &= ~0x80;
}

/**
* @brief		Configure Timer2 interrupt
* @param   enable		This parameter contains the enable of timer interrupt
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void Timer2_ConfigureInterrupt(uint8_t enable) 
{
	if(enable)
		IE2 |= 0x04;
	else
		IE2 &= ~0x04;
}

/**
* @brief		Claer Timer2 interrupt
* @param   None
* @return		None
*/
void Timer2_ClearInterruptFlag(void) 
{
	T2CRL &= ~0x10;
}

/* --------------------------------- End Of File ------------------------------ */