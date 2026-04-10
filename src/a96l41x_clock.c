/**
*   @file       a96l41x_clock.c
*   @brief      A96L41x System clock peripheral function 
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
#include "a96l41x_clock.h"
#include "delay.h"

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
* @brief	Initialize System Clock peripheral
* @param    clock_sel	This parameter contains the selection of clock.
* @return		None
*/
void Clock_Initial(uint8_t clock_sel)
{
    uint8_t temp_reg;
	if(clock_sel <= HFI4_DIV1)
	{
		/* Internal 4Mhz RC OSC for system clock */
		SCCR	= SCCR_IRC_4MHZ;            
		/* Internal RC Oscillator Post-divider Select INT-RC/n (32/n MHz) */
		temp_reg = OSCCR;
		temp_reg  &= ~(0x3 << 3);
		temp_reg  |= (clock_sel << 3) | OSCCR_HIRCE_EN;
		OSCCR = temp_reg;
	}
	else if(clock_sel == LFI)
	{
		/* Internal 32khz RC OSC for system clock */
   		OSCCR |= OSCCR_LFIRCE_EN;
        NOP_20us_Delay(5000);
		SCCR	= SCCR_IRC_32KHZ; 
		temp_reg = OSCCR;
		temp_reg  |= OSCCR_HIRCE_DIS;
		OSCCR = temp_reg;
 	}
}

/**
* @brief	Controls clock frequency selection and oscillator operation.
* @param    osc_sel 	This parameter contains the selection of Oscillator.
*
*					- HFIRC  = 0
*					- LFIRC	 = 1
* @param    hsirc_div 	This parameter contains the Post-divider Selection of Internal RC Oscillator.
*
*					- HFIRC4_DIV8 = 0
*					- HFIRC4_DIV4 = 1 
*					- HFIRC4_DIV2 = 2
*					- HFIRC4_DIV1 = 3
* @param    osc_enable	This parameter contains the enable of oscillator.
*
*					- OSC_DIS  = 0
*					- OSC_EN   = 1
* @return		None
*/
void Clock_ConfigureOSC(uint8_t osc_sel, uint8_t hsirc_div, uint8_t osc_enable)
{
	uint8_t temp_reg;
	
	if(osc_sel == LFIRC)
	{
        if(osc_enable == OSC_DIS)
        {
			temp_reg = OSCCR;
            /* Disable Low Frequency(32kHz) Internal RC OSC for system clock */
            temp_reg &= ~(osc_enable << 7);
        }
        else if(osc_enable == OSC_EN)
        {
            /* Enable Low Frequency(32kHz) Internal RC OSC for system clock */
            OSCCR |= (0x1 << 7);          
        }
 	}
	else if(osc_sel == HFIRC)
	{
        if(osc_enable == OSC_DIS)
        {
            /* Disable High Frequency(4MHz) Internal RC OSC for system clock */
            OSCCR |= (0x1 << 2);          
        }
        else if(osc_enable == OSC_EN)
        {
			temp_reg = OSCCR;
            /* Enable High Frequency(4MHz) Internal RC OSC for system clock */
            temp_reg &= ~(osc_enable << 2);
            
            temp_reg &= ~(0x3<<3);
            temp_reg |= (hsirc_div<<3);
			
			OSCCR = temp_reg;
        }
 	}
}

/**
* @brief		Get Systemclock frequency(speed)
* @param   		None
* @return		The value of system clock(frequency)
*/
uint32_t Clock_GetSystemFreq(void)
{
	return SYSTEM_CLOCK;
}

/* --------------------------------- End Of File ------------------------------ */
