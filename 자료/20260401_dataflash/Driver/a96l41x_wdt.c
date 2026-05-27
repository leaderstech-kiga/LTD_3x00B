/**
*   @file       a96l41x_wdt.c
*   @brief      A96L41X WDT file
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

#include "a96l41x_wdt.h"
#include "a96l41x_bit.h"
/* Private Pre-processor Definition & Macro ----------------------------------*/
/* Private Typedef -----------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/
/* Private Function Prototype ------------------------------------------------*/

/**
* @brief		Initialize Watch Dog Timer peripheral
* @param    	rst_on	This parameter contains the enable of watch dog timer reset.
*
*					- WDT_FREE_RUN	 = 0
*					- WDT_RST_ON     = 1
* @param    	wdtck	This parameter contains the selection of Clock.
*
*					- WDT_BIT_OVER  	= 0
*					- WDT_LFIRC			= 1
*					- WDT_WDTRC			= 2
* @return		None
*/
void WDT_Initial(uint8_t rst_on, uint8_t wdtck)
{
	uint8_t temp_reg;
	/* Set timer mode & frequency source */
	temp_reg = WDTCR;
	temp_reg &= ~((1 << 6) | (1 << 1));
	temp_reg |= ((rst_on << 6) | (wdtck << 1));
	
	WDTCR = temp_reg;
}

/**
* @brief		Set Watch Dog Timer data operation
* @param		count	This parameter contains the value of watch dog timer count(0~0xFF).
* @return		None
*/
void WDT_SetDATA(uint8_t count) 
{
	WDTDR = count;
}

/**
* @brief		Set Watch Dog Timer 500msec operation
* @param		None	
* @return		None
*/
void WDT_Set_500msec_Reset(void) 
{
	BIT_Initial(BIT_DIV4096, BIT_X4);
	
	WDTCR = ((WDT_RST_ON << 6) | (WDT_BIT_OVER << 1));

	WDTDR = 124;
}

/**
* @brief		Set Watch Dog Timer 1sec operation
* @param		None
* @return		None
*/
void WDT_Set_1sec_Reset(void) 
{
	BIT_Initial(BIT_DIV4096, BIT_X8);

	WDTCR = ((WDT_RST_ON << 6) | (WDT_BIT_OVER << 1));

	WDTDR = 124;
}

/**
* @brief		Set Watch Dog Timer 2sec operation
* @param		None
* @return		None
*/
void WDT_Set_2sec_Reset(void) 
{
	BIT_Initial(BIT_DIV4096, BIT_X16);

	WDTCR = ((WDT_RST_ON << 6) | (WDT_BIT_OVER << 1));

	WDTDR = 124;
}

/**
* @brief		Set Watch Dog Timer 4sec operation
* @param		None
* @return		None
*/
void WDT_Set_4sec_Reset(void) 
{
	BIT_Initial(BIT_DIV4096, BIT_X32);

	WDTCR = ((WDT_RST_ON << 6) | (WDT_BIT_OVER << 1));

	WDTDR = 124;
}

/**
* @brief		Set Watch Dog Timer 4sec operation with WDTRC(about 5kHz)
* @param		None
* @return		None
*/
void WDT_Set_4sec_Reset_WDTRCEN(void) 
{
	WDTCR = ((WDT_RST_ON << 6) | (WDT_WDTRC << 1));

	WDTDR = 80;
}

/**
* @brief		Get the value of WDT counter data
* @param		None
* @return		The value of watch dog timer data
*/
uint8_t WDT_GetCountValue(void) 
{
	return WDTCNT;
}

/**
* @brief		Clear the value of WDT counter data 
* @param		None
* @return		None
*/
void WDT_ClearCountData(void) 
{
	WDTCR |= 0x20;
}

/**
* @brief		Start Watch Dog Timer operation
* @param		None
* @return		None
*/
void WDT_Start(void) 
{
	WDTCR |= 0x80;
}

/**
* @brief		Stop Watch Dog Timer operation
* @param		None
* @return		None
*/
void WDT_Stop(void) 
{
	WDTCR &= ~0x80;
}

/**
* @brief		Configure Watch Dog Timer interrupt
* @param		enable		This parameter contains the enable of watch dog timer interrupt
*
*					- WDT_INT_DIS = 0
*					- WDT_INT_EN  = 1
* @return		None
*/
void WDT_ConfigureInterrrupt(uint8_t enable) 
{
	if(enable)
			IE3 |= 0x08;
	else
			IE3 &= ~0x08;
}

/* --------------------------------- End Of File ------------------------------ */
