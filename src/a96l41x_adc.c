/**
* @file       a96l41x_adc.c
* @brief      A96L41x ADC peripheral function 
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
#include "a96l41x_adc.h"

/*******************************************************************************
* Private Pre-processor Definition & Macro
*******************************************************************************/

/*******************************************************************************
* Private Typedef
*******************************************************************************/

/*******************************************************************************
* Private Variable
*******************************************************************************/
uint8_t adc_count = 0;
uint16_t *adc_buff;
uint8_t buff_cnt = 0;
/*******************************************************************************
* Private Function Prototype
*******************************************************************************/

/*******************************************************************************
* Public Function
*******************************************************************************/

/**
* @brief		Initialize ADC peripheral
* @param   clock_sel			This parameter contains the selection of ADC converter clock (fx <= 8MHz).
*
*			- ADC_CLK_4M	 	= 0
*			- ADC_CLK_2M		= 1
*			- ADC_CLK_1M 		= 2
*			- ADC_CLK_0_5M 		= 3
* @param   trigger_sel		This parameter contains the selection of ADC trigger source.
*
*			- ADC_SW_TRIG	 	= 0
*			- ADC_T0_TRIG 		= 1
* @param   ref_voltage		This parameter contains the selection of ADC reference voltage.
*
*			- ADC_INTERNAL_REF 	= 0
*			- ADC_LDO_REF 	= 1
* @param   align_sel 		This parameter contains the selection of ADC data align. 
*
*			- ADC_MSB 			= 0
*			- ADC_LSB 			= 1
* @return		None
*/
void ADC_Initial(uint8_t clock_sel, uint8_t trigger_sel, uint8_t ref_voltage, uint8_t align_sel)
{
		/*  The A/D converter needs at least 14 us for conversion time.
        So you must set the conversion time more than 14 us.
        If the ADC conversion time is set short, the resolution is degraded.
    
        ** ADC Conversion clock = ADCLK * 28 clocks
        
		Example)
        fx=2Mhz, CKSEL = 2M,  ADC Clock source(@2Mhz) = 0.5us
        28 clock x 0.5us = 14us at 2Mhz
    */
	uint8_t temp_reg, temp_reg2;
	
		if(clock_sel == 0){
		clock_sel =1;
	}
		
	temp_reg = ADCCRH;
	temp_reg &= ~(0xFF);
	temp_reg |= ((trigger_sel << 3) | (align_sel << 2) | (clock_sel << 0));
	
	temp_reg2 = ADCCRL;
	temp_reg2 &= ~(0xFF);
	temp_reg2 |= ((1 << 7) | (ref_voltage << 5));  //ADC Enable + ref_Voltage
	
	ADCCRH = temp_reg;
	ADCCRL = temp_reg2;
	
	/* If the ADC clock is more than 4MHz, ADC module is not initialized. 
	   To meet the conversion time spec value, adc clock is under 2MHz recommended.
	*/
	/*
	while( (clock_sel >= 1) )
	{
		NOP;  // NOTE : clock_sel <= 2Mhz
	}
*/

		
}

/**
* @brief		Enables or disables the ADC peripheral.
* @param   enable   This parameter contains the enable of this function. 
*
*			- FALSE 	= 0
*			- TRUE 		= 1
* @return		None
*/
void ADC_Enable(uint8_t enable)
{
	if(enable == TRUE)
	{
		ADCCRL |= (1 << 7);
	}
	else
	{
		ADCCRL &= ~(1 << 7);
	}
}

/**
* @brief		Enables the ADC Start conversion(For software trigger).
* @param   		None
* @return		None
*/
void ADC_StartSoftwareTrigger()
{
	ADCCRL |= (1 << 6);
}

/**
* @brief		Select the ADC Converter Input Channel.
* @param   channel		This parameter contains the channel of ADC input.
*
*			- ADC_CH0 	= 0
*			- ADC_CH1 	= 1
*			- ADC_CH2 	= 2
*			- ADC_CH3 	= 3
*			- ADC_CH4 	= 4
*			- ADC_CH5 	= 5
*			- ADC_CH6 	= 6
*			- ADC_OP0OUT 	= 9
*			- ADC_OP1OUT	= 10
*			- ADC_VBGR 	= 15
* @return		None
*/
void ADC_SelectChannel(uint8_t channel)
{
	uint8_t temp_reg;
	
	temp_reg = ADCCRL;
	temp_reg &= ~(0xF << 0);
	temp_reg |= channel;
	
	ADCCRL = temp_reg;
}

/**
* @brief		Get status of ADC conversion.
* @param   None
* @return		Status of ADC conversion flag.
*/
uint8_t ADC_GetConversionStatus()
{
	return (ADCCRL & 0x10);
}

/**
* @brief		Get data of ADC conversion for ADC interrupt.
* @param   adc_data		This parameter contains the data of conversion ADC.
* @param   count		This parameter contains the number of count.
* @return		None
*/
void ADC_GetDataWithInterrupt(uint16_t *adc_data, uint8_t count)
{
	adc_count = count;
	adc_buff = (uint16_t*)adc_data;
	buff_cnt = 0;
	
	ADC_ConfigureInterrupt(TRUE);

	if( ((ADCCRH & 0x08) == 0x00)) //SW trigger
	{
		ADC_StartSoftwareTrigger();
	}
	
	while(0 < adc_count);  //adc interrupt subroutine execute

}

/**
* @brief		Get data of ADC conversion for ADC polling.
* @param   adc_data		This parameter contains the data of conversion ADC.
* @param   count		This parameter contains the number of count.
* @return		None
*/
void ADC_GetDataWithPolling(uint16_t *adc_data, uint8_t count)
{
	uint8_t i;
	
	for(i = 0; i < count; i++)
	{
		if( ((ADCCRH & 0x08) == 0x00) ) //SW trigger
			ADC_StartSoftwareTrigger();
			
		while(!(ADC_GetConversionStatus()));
			
		adc_data[i] = ADCDR;
	}
}

/**
* @brief		Configure the enable or disable ADC interrupt.
* @param   enable   This parameter contains the enable of this function. 
*
*			- FALSE = 0
*			- TRUE 	= 1
* @return		None
*/
void ADC_ConfigureInterrupt(uint8_t enable)
{
	if(enable == TRUE)
	{
		IE1  |= 0x01; 
	}
	else
	{
		ADCCRH &= ~0x80;
		IE1 &= ~0x01;
	}
}

/**
* @brief		Clear the ADC interrupt status.
* @param   		None
* @return		None
*/
void ADC_ClearInterruptStatus()
{
	ADCCRH &= ~0x80;
}


void ADC_Int_Handler(void) interrupt ADC_VECT
{
	if(((ADCCRH & 0x08) == 0x00) && (adc_count != 0)) //SW trigger
		ADC_StartSoftwareTrigger();
	
	adc_count--;
	adc_buff[buff_cnt++] = ADCDR;
	
	if(adc_count == 0)
		ADC_ConfigureInterrupt(FALSE);
}

/* --------------------------------- End Of File ------------------------------ */
