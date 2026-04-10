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
#include "a96l41x_timer1.h"

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
* @brief		Initialize timer1 peripheral
* @param   mode		This parameter contains the mode of timer.
*
*					- T1_COUNTER_MODE						= 0
*					- T1_CAPTURE_MODE						= 1
*					- T1_PPG_ONESHOT_MODE 					= 2
*					- T1_PPG_REPEAT_MODE 					= 3
* @param   freq	This parameter contains the selection of frequency.
*
*					- TIMER1_DIV2048 						= 0
*					- TIMER1_DIV512 						= 1
*					- TIMER1_DIV64 							= 2
*					- TIMER1_DIV8 							= 3
*					- TIMER1_DIV4							= 4
*					- TIMER1_DIV2							= 5
*					- TIMER1_DIV1							= 6
*					- TIMER1_EC2							= 7
* @return		None
*/
void Timer1_Initial(uint8_t mode, uint8_t freq)
{
	uint8_t temp_reg, temp_reg2;
	
	temp_reg = T1CRH;
	/* Set timer mode & frequency source */
	temp_reg &= ~(0x3 << 4);
	temp_reg |= ((mode << 4) | (0x1 << 0));
	
	temp_reg2 = T1CRL;
	temp_reg2 &= ~((0x7 << 5) | (0x1 << 0));
	if(mode >= T1_PPG_ONESHOT_MODE)
		temp_reg2 |= (freq << 5);
	else
		temp_reg2 |= ((freq << 5) | (0x1 << 0));
	
	T1CRH = temp_reg;
	T1CRL = temp_reg2;
}

/**
* @brief	 Set Timer1 Counter mode operation
* @param   match_count		This parameter contains the value of timer match count(0~0xFFFF).
* @return		None
*/
void Timer1_SetMatchCounter(uint16_t match_count) 
{
	T1ADR = match_count;
}

/**
* @brief	 Set Timer2 PPG Polarity
* @param   polarity		This parameter contains the value of polarity.
*
*					- T1_START_HIGH 						= 0
*					- T1_START_LOW 							= 1
* @return		None
*/
void Timer1_SetPPGPolarity(uint8_t polarity) 
{
	uint8_t temp_reg;
	
	temp_reg = T1CRL;
	temp_reg &= ~(0x04); 
	temp_reg |= (polarity << 2);
	
	T1CRL = temp_reg;
}

/**
* @brief	 Set Timer1 PPG mode duty counter
* @param   count		This parameter contains the value of timer duty count(0~0xFFFF).
* @return		None
*/
void Timer1_SetPPGDutyCounter(uint16_t count) 
{
	T1BDR = count;
}

/**
* @brief	 Set Timer1 PPG mode period counter
* @param   count		This parameter contains the value of timer period count(1~0xFFFF).
* @return		None
*/
void Timer1_SetPPGPeriodCounter(uint16_t count) 
{
	T1ADR = count;
}


/**
* @brief	 Get Timer1 Capture mode operation
* @param   None
* @return		The value of timer capture data(16bit)
*/
uint16_t Timer1_GetCaptureValue(void) 
{
	return T1BDR;
}

/**
* @brief	 Clear the value of Timer1 counter data 
* @param   None
* @return		None
*/
void Timer1_ClearCountData(void) 
{
	T1CRH |= 0x01;
}

/**
* @brief		Start Timer1 operation
* @param   None
* @return		None
*/
void Timer1_Start(void) 
{
	T1CRH |= 0x80;
}

/**
* @brief		Stop Timer1 operation
* @param   None
* @return		None
*/
void Timer1_Stop(void) 
{
	T1CRH &= ~0x80;
}

/**
* @brief		Configure Timer1 interrupt
* @param   enable		This parameter contains the enable of timer interrupt
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void Timer1_ConfigureInterrupt(uint8_t enable) 
{
	if(enable)
		IE2 |= 0x02;
	else
		IE2 &= ~0x02;
}

/**
* @brief		Claer Timer1 interrupt
* @param   None
* @return		None
*/
void Timer1_ClearInterruptFlag(void) 
{
	T1CRL &= ~0x10;
}

/* --------------------------------- End Of File ------------------------------ */