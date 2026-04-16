/***************************************************************************//**
*   @file       Main.c
*   @brief      A96L41x Main  function file 
*   @author     Application Team, ABOV Semiconductor Co., Ltd.
*   @version    V1.00
*   @date       29. MAY. 2020
*
* Copyright(C) 2020, ABOV Semiconductor
* All rights reserved.
*
*   @mainpage   Project : A96L41x (20TSSOP) Timer2 Usage
*   - Introduction: This is a guide program for using A96L41x Timer2.
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
#include "Intrins.h"
#include "delay.h"     
#include "a96l41x_gpio.h"
#include "a96l41x_clock.h"
#include "a96l41x_timer0.h"
#include "a96l41x_timer1.h"

#include "main.h"
#include "flash.h"
#include "audio.h"
//#include "sound_fire1.h"


unsigned char write_data[10];
unsigned char read_data[10];



/**********************************************************************
 * @brief		Audio_Initial
 * @param   	None
 * @return	None
 **********************************************************************/
void Audio_Initial(void)
{

	if(SystemStatus==0)
		return;
	
	/* Timer0 PPG port (Alternative function) */
	Port_SetAlterFunctionpin(PORT1, PIN1, 0x1);			// PWM
	
    	/* Timer0 initialize @ 4MHz */
	Timer0_Initial(T0_PPG_REPEAT_MODE, TIMER0_DIV1);	
	 /* Timer0 PPG Polarity Start Low */
	Timer0_SetPPGPolarity(T0_START_HIGH);	
	 /* Timer0 PPG Period Counter*/ 
  	Timer0_SetPPGPeriodCounter(124);				// 125(31.2us, 32khz)  	
	Timer0_SetPPGDutyCounter(50);
	
   	/* Enable timer0 match INT */   
 	Timer0_ConfigureInterrupt(FALSE);
	/* Timer0 start */
	//Timer0_Start();	


	
    	/* Timer1 initialize @ 4MHz */
	Timer1_Initial(T1_PPG_REPEAT_MODE, TIMER1_DIV1);	
	 /* Timer1 PPG Polarity Start Low */
	Timer1_SetPPGPolarity(T1_START_HIGH);	
	 /* Timer1 PPG Period Counter*/ 
	Timer1_SetPPGPeriodCounter(499);				//500(125us,8khz)

	Timer1_SetPPGDutyCounter(100);					
	
   	/* Enable timer1 match INT */   
 	Timer1_ConfigureInterrupt(TRUE);
	/* Timer1 start */
	//Timer1_Start();	

}




void TIMER1_Int(void) interrupt T1_MATCH_VECT
{
	uint8_t duty;

	Timer1_ClearInterruptFlag();


	if(Audio_start==0)
	{	
		AUDIO_ON;
		Read(Audio_addr);		// SPI 읽기 명령만 시작
	
		Timer0_SetPPGDutyCounter(0);
		Timer0_Start();
		Audio_start=1;
		// ← 이 시점엔 아직 데이터가 PWM에 반영 안 됨
	}
	else
	{

		if(Audio_length < Audio_max_length)
		{
			Read_c(read_data);	// 다음 인터럽트에서 데이터 수신
			duty = read_data[0];	// 8bit → 0~127 변환
			duty =duty>>1;
			if(duty >= 124)
				duty= 124;
			 Timer0_SetPPGDutyCounter(duty);
			 Audio_length++;
			 Audio_addr++;
		}
		else	// Audio_length >= Audio_max_length 도달 시
		{
			SLAVEDESELECT;
			AUDIO_OFF;
			Timer0_Stop();
			Audio_start = 0;
			Audio_length = 0;
		}
		
	}
		
	
}



void Read_Audio_Length(uint16_t Audio_st_address)
{
	unsigned long temp;

	Audio_addr=Audio_st_address + 40;
	Read_v(Audio_addr, read_data);
	Audio_max_length = read_data[0];          // Low byte

	Audio_addr=Audio_st_address + 41;
	Read_v(Audio_addr, read_data);	
	temp= read_data[0];
	temp = (temp << 8)&0xff00;     // High byte
	Audio_max_length |=temp;

}


/* --------------------------------- End Of File ------------------------------ */
