/**
*   @file       a96L41x_adc.h
*   @brief      A96L41x ADC peripheral function 
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
#ifndef __ADC_H_
#define __ADC_H_
/*******************************************************************************
* Included File
*******************************************************************************/
#include "A96L41x.h"
#include "a96l41x_clock.h"
#include "typedef.h"
#include "Intrins.h"

/*******************************************************************************
* Public Macro
*******************************************************************************/
#define ADC_MAX_BUFFER_SIZE		3

#define ADC_CLK_4M						0
#define ADC_CLK_2M						1
#define ADC_CLK_1M						2
#define ADC_CLK_0_5M					3

#define ADC_SW_TRIG						0
#define ADC_T0_TRIG						1

#define ADC_INTERNAL_REF	0
#define ADC_LDO_REF			1

#define ADC_MSB		0
#define ADC_LSB		1

/*******************************************************************************
* Public Typedef
*******************************************************************************/
enum  adc_channel{ 
	ADC_CH0		 	= 0,
	ADC_CH1,
	ADC_CH2,
	ADC_CH3,
	ADC_CH4,
	ADC_CH5,
	ADC_CH6,
	ADC_OP0OUT		= 9,
	ADC_OP1OUT,
	ADC_VBGR		= 15,
};

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void ADC_Initial(uint8_t clock_sel, uint8_t trigger_sel, uint8_t ref_voltage, uint8_t align_sel);
void ADC_Enable(uint8_t enable);
void ADC_SelectChannel(uint8_t channel);

void ADC_StartSoftwareTrigger(void);
uint8_t ADC_GetConversionStatus(void);

void ADC_GetDataWithInterrupt(uint16_t *adc_data, uint8_t count);
void ADC_GetDataWithPolling(uint16_t *adc_data, uint8_t count);

void ADC_ConfigureInterrupt(uint8_t enable);
void ADC_ClearInterruptStatus(void);

#endif  /* End of __ADC_H_ */
/* --------------------------------- End Of File ------------------------------ */
