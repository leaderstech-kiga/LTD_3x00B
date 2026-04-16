/***************************************************************************//**
*   @file       Main.c
*   @brief      A96L41x Main  function file 
*   @author     Application Team, ABOV Semiconductor Co., Ltd.
*   @version    V1.00
*   @date       29. MAY. 2020
*
* Copyright(C) 2019, ABOV Semiconductor
* All rights reserved.
*
*   @mainpage   Project : A96L41x (20TSSOP) 
*   - Introduction: This is a guide program for using A96L41x .
*
*   @section    Compiler Version
*   - uVision C51 Compiler ( Version 5.13 )
*
*   @section    Configure Option
*   - Code Read Protection :                Disable
*   - Code Write Protection :               Disable
*   - Vector Area (00H~FFH) Protection :    Disable
*   - Select RESETB pin :                   Disable
*
*   @section    Clock Setting
*   - HFIRC OSC/1 (Internal 4Mhz) 
*
*   @section    Project Program Version : V1.00
*   @section    Modify Infomation
*   - V1.00 :    First Start.    
*
*   @section    Caution
*   - Nothing special.
*
*   @section Common
*   - ABOV Semiconductor Co.,Ltd.
*   - URL : <http://www.abovsemi.com>
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



/* Includes ------------------------------------------------------------------*/
#include    "Intrins.h"
#include    "delay.h"     //
#include "a96l41x_gpio.h"
#include "a96l41x_clock.h"

#include "a96l41x_adc.h"
#include "a96l41x_opamp.h"
#include "a96l41x_usart.h"

#include "a96l41x_usart_spi.h"
#include "a96l41x_timer0.h"
#include "a96l41x_timer1.h"
#include "a96l41x_timer2.h"


#include "main.h"
#include "audio.h"
#include "flash.h"


/* Private Pre-processor Definition & Macro ----------------------------------*/
/* Private Typedef -----------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////Start  오디오 변수//////////// 
// Audio Data : 8000bps , 8bit 
uint16_t Audio_length;
uint16_t Audio_addr;
uint8_t Audio_start_address;
uint16_t Audio_max_length;
/* [BUG-06 FIX] Duplicate declaration removed - SystemStatus already declared at line 168 */
/* BEFORE: uint8_t SystemStatus; */
/* uint8_t SystemStatus; */

uint8_t EM_AL_Status = 0;
uint8_t Audio_start =0;

uint16_t  Audio1_Address  = 0x0;
uint16_t  Audio1_length  = 10801;
uint8_t  Audio1_runtime  = 1;
uint16_t  Audio2_Address  = 0x2baa;
uint16_t  Audio2_length  = 21478;
uint8_t  Audio2_runtime  = 1;
uint16_t  Audio3_Address  = 0x8108;
uint16_t  Audio3_length  = 24324;
uint8_t  Audio3_runtime  = 1;
////////////////End  오디오 변수//////////// 

////////////////Start  Debug용 uart text//////////// 
uint8_t Dust_MODE[10] = "Dust_MODE:";
uint8_t Visu_MODE[10] = "Visu_MODE:";
uint8_t Temp_MODE[10] = "Temp_MODE:";

uint8_t ADC[4] = "ADC:";
uint8_t CHR[4] = "CHR:";
uint8_t INF[4] = "INF:";

uint8_t Dust[5] = "Dust:";
uint8_t Dust_ori[7] = "Dust_O:";
uint8_t Dust_avr[7] = "Dust_A:";
uint8_t Temp[5] = "Temp:";
uint8_t Bat[4] = "Bat:";
uint8_t Tab[4] = "    ";
uint8_t End[4] = "\n\r";
uint8_t int_val ;
uint16_t tmp ;

uint8_t opamp1[5] = "OPA1:";
uint8_t opamp2[5] = "OPA2:";

uint8_t Avr[9] = "Average: ";

uint8_t Dust_Si_Off[7] = "Si_Off:";
uint8_t Dust_Si_On[6] = "Si_On:";
uint8_t Dust_Si_Deta[6] = "Si_Da:";

uint8_t Dust_Du_Off[7] = "Du_Off:";
uint8_t Dust_Du_On[6] = "Du_On:";
uint8_t Dust_Du_Deta[6] = "Si_Da:";
////////////////End  Debug용 uart text//////////// 

////////////////Start  시스템 상태 함수//////////// 
uint8_t deviceID[3];
uint8_t SystemStatus;


////////////////End  시스템 상태 함수//////////// 


/* Public Variable -----------------------------------------------------------*/
/* Public Function -----------------------------------------------------------*/

/**********************************************************************
 * @brief		Main program
 * @param   	None
 * @return	    None
 **********************************************************************/
void Main(void)
{
	

	while(1)
	{
	}

}

