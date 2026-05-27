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

#include "main.h"
#include "flash.h"
#include "audio.h"


/* SPI timeout flag. Set by SPI_Transfer / SPI_Receive when their wait
 * loops time out so callers can distinguish a real 0xFF data byte from
 * an error. volatile because it may be set inside TIMER1_Int audio ISR
 * (via Read_c) and consumed by main-thread code. */
volatile uint8_t SPI_Timeout = 0;



/**********************************************************************
 * @brief   SPI_Receive - clock out a 0xFF dummy and return the byte the
 *          slave shifted back on MISO. Bounded wait loops with timeout
 *          guards; sets SPI_Timeout on failure.
 *          Used from TIMER1_Int audio streaming ISR - must not hang.
 * @return  received byte, or 0xFF if timeout (check SPI_Timeout flag).
 **********************************************************************/
uint8_t SPI_Receive(void)
{
	uint16_t wait;

	USTCR2 |= (DRIE);

	wait = 0x1000;
	while((!(USTST & (DRE))) && (--wait));
	if ((USTST & DRE) == 0) {
		SPI_Timeout = 1;
		return 0xFF;
	}

	USTDR = 0xFF;                            /* dummy byte to clock in MISO */

	wait = 0x1000;
	while(((USTST & RXC) != RXC) && (--wait));
	if ((USTST & RXC) != RXC) {
		SPI_Timeout = 1;
		return 0xFF;
	}

	SPI_Timeout = 0;
	return USTDR;
}



/**********************************************************************
 * @brief   SPI_Transfer - clock out `value` on MOSI and return the byte
 *          shifted in on MISO. Sets SPI_Timeout on wait-loop timeout.
 * @param   value - byte to send
 * @return  received byte, or 0xFF if timeout (check SPI_Timeout flag).
 **********************************************************************/
uint8_t SPI_Transfer(uint8_t value)
{
	uint16_t wait;

	wait= 0x1000;
	while((!(USTST & (DRE)))&&(wait--));

	if ((USTST & DRE) == 0) {
		SPI_Timeout = 1;
		return 0xFF;
	}

	wait= 0x1000;
	USTDR = value;
	while((( USTST & RXC)!= RXC)&&(wait--));

	if((USTST & RXC) != RXC) {
		SPI_Timeout = 1;
		return 0xFF;
	}

	SPI_Timeout = 0;
	return USTDR;
}



/**********************************************************************
 * @brief		Get_Identification - read JEDEC ID (3 bytes)
 **********************************************************************/
void Get_Identification(uint8_t * id)
{
	SLAVESELECT;
	NOP_4us_Delay();                           /* ~4 us CS-setup delay */
	SPI_Transfer(COMMAND_RDID);
	id[0] = SPI_Transfer(DUMMY);
	id[1] = SPI_Transfer(DUMMY);
	id[2] = SPI_Transfer(DUMMY);
	SLAVEDESELECT;
}


void SetAddress(unsigned long address)
{
	SPI_Transfer( (uint8_t) (address >> 16) );
	SPI_Transfer( (uint8_t) (address >> 8) );
	SPI_Transfer( (uint8_t) address );
}


void Read(unsigned long  address)
{
	SLAVESELECT;

	SPI_Transfer(COMMAND_READ);                /* command_read */
	SetAddress(address);
	/* CS stays low - subsequent Read_c() calls clock out streaming bytes */
}


void Read_c( char * value)
{
	*(value) = SPI_Receive();
}



