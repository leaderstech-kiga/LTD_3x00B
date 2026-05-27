/***************************************************************************//**
*   @file       Main.c
*   @brief      A96L41x Main  function file 
*   @author     Application Team, ABOV Semiconductor Co., Ltd.
*   @version    V1.00
*   @date       18. DEC. 2020
*
*   @mainpage   Project : A96L41x (20TSSOP) DataFlash Usage
*   - Introduction: This is a guide program for using A96L41x DataFlash.
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
*   - HFIRC OSC/1 (Internal 4MHz) 
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
/* Includes ------------------------------------------------------------------*/
#include "intrins.h"
#include "delay.h"
#include "a96l41x_dataflash.h"
#include "a96l41x_gpio.h"
#include "a96l41x_clock.h"

/* Private Pre-processor Definition & Macro ----------------------------------*/
/* Private Typedef -----------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/
unsigned char dataflash_data[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
											 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};

unsigned char dataflash_rdata[32];
/* Private Function Prototype ------------------------------------------------*/
/* External Variable----------------------------------------------------------*/
extern unsigned short dataflash_addr;
extern unsigned short DATAFLASH_OPR_FLAG;
extern unsigned long dataflash_id;
extern unsigned char dataflash_len;


/**********************************************************************
 * @brief		Main program
 * @param   	None
 * @return	    None
 **********************************************************************/
void main(void)
{
	uint8_t i, DF_WT_FLAG = DF_WT;
	/* Disable INT. during peripheral setting */
	GLOBAL_INTERRUPT_DIS();     
    
    /* Port intialize */
	Port_Initial();		        
    /* Clock initialize */
	Clock_Initial(HFI4_DIV1);   
    /* System Stabilize Delay */
	NOP_20us_Delay(25000);        // 0.5sec
	
	Port_SetOutputLowpin(PORT1, PIN0);
	
	/* DataFlash initialize */
	DATAFLASH_Exit();
    
    /* Enable INT */
	GLOBAL_INTERRUPT_EN();
	


	/* Flash erese/write */
	NOP; // check flash
	
  /* Infinite loop */
	while(1)
	{
		if(DATAFLASH_OPR_FLAG) 
		{
			if((DFMCR&(0x01<<DFMBUSY)) == 0) DATAFLASH_OPR_FLAG = 0;
		}

		if(DATAFLASH_OPR_FLAG == 0) 
		{
			if(DF_WT_FLAG == DF_WT) 
			{
				dataflash_id = 0xA901358F;
				dataflash_addr = 0x3000;
				DATAFLASH_PageEr(dataflash_id, dataflash_addr);
				DF_WT_FLAG = DF_ER;
		       }
			else if(DF_WT_FLAG == DF_ER)
			{
#if 1 											 // If 0/1, Byte/Page Write
				dataflash_id = 0x4F17DC86;
				dataflash_addr= 0x3000;
				for(i=0 ; i<32 ; i++) {
				  dataflash_data[i] += 1;
				}
				DATAFLASH_PageWt(dataflash_id, dataflash_addr, dataflash_data);
				DF_WT_FLAG = DF_RD;

				dataflash_addr += 0x0020;
				dataflash_addr &= 0x00F0;
#else
				dataflash_id = 0xA39B71D6;
				dataflash_addr |= 0x3000;
				if((dataflash_addr&0x000f) == 0) {
				  for(i=0 ; i<16 ; i++) dataflash_data[i] += 1;
				}
				DATAFLASH_ByteWt(dataflash_id, dataflash_addr, dataflash_data[(unsigned char)(dataflash_addr&0x000f)]);

				dataflash_addr++;
				dataflash_addr &= 0x007f;

				if((dataflash_addr&0x000f) == 0) DF_WT_FLAG = DF_RD;
#endif
		   	}
			else if(DF_WT_FLAG == DF_RD)
			{
				dataflash_addr = 0x3000;
				for(i=0 ; i<32 ; i++) {
				  dataflash_rdata[i]=0;
				}
				dataflash_len=32;
				DATAFLASH_PageRd(dataflash_addr, dataflash_rdata, dataflash_len);
				DF_WT_FLAG = DF_WT;
			}

	     	}
	}
}

/* --------------------------------- End Of File ------------------------------ */
