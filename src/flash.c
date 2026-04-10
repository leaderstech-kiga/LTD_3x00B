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





/**********************************************************************
 * @brief		SPI_Receive
 * @param   	none
 * @return	data
 **********************************************************************/
uint8_t SPI_Receive(void)
{
	USTCR2 |= (DRIE);
	while(!(USTST & (DRE)));
	USTDR = 0xFF; //Dummy data
	while(( USTST & RXC)!= RXC);		// Wait Data in
	return USTDR;
}



/**********************************************************************
 * @brief		SPI_Send
 * @param   	data
 * @return	None
 **********************************************************************/
 void SPI_Send(uint8_t value)
{
	USTCR2 |= (DRIE);
	while(!(USTST & (DRE)));
		USTDR = value;
	
	 while((USTST & RXC) != RXC);   // 전송 완료까지 대기 (추가)
   (void)USTDR;                    // 수신 버퍼 더미 읽기로 RXC 클리어 (추가)
	
}


#if 0
/**********************************************************************
 * @brief		SPI_Transfer
 * @param   	data
 * @return	None
 **********************************************************************/
uint8_t SPI_Transfer(uint8_t value)
{
	uint16_t wait;

	wait= 0x1000;
	USTCR2 |= (DRIE);
	while((!(USTST & (DRE)))&&(wait--));
	wait= 0x1000;
	USTDR = value;
	while((( USTST & RXC)!= RXC)&&(wait--));		// Wait Data in
	
	    /* 타임아웃 체크: 루프 탈출 후 RXC가 실제로 세트됐는지 확인 */
    if((USTST & RXC) != RXC){
        return 0xFF;    // 타임아웃 시 오류값 반환 (추가)
		}
	return USTDR;
}
#endif

/**********************************************************************
 * @brief		SPI_Transfer
 * @param   	data
 * @return	None
 **********************************************************************/
uint8_t SPI_Transfer(uint8_t value)
{
	uint16_t wait;

	wait= 0x1000;
	//USTCR2 |= (DRIE);
	while((!(USTST & (DRE)))&&(wait--));
	wait= 0x1000;
	USTDR = value;
	while((( USTST & RXC)!= RXC)&&(wait--));		// Wait Data in

	    /* 타임아웃 체크: RXC가 실제로 세트됐는지 확인 */
    if((USTST & RXC) != RXC)
        return 0xFF;    // 타임아웃 시 오류값 반환

    return USTDR;
		
}



#if 0

/**********************************************************************
 * @brief		Get_Identification
 * @param   	None
 * @return	None
 **********************************************************************/
void Get_Identification(uint8_t * id)
{
	SLAVESELECT;
	SPI_Transfer(COMMAND_RDID);
	id[0] = SPI_Transfer(DUMMY);
	id[1] = SPI_Transfer(DUMMY);
	id[2] = SPI_Transfer(DUMMY);
	SLAVEDESELECT;
}

uint8_t GetStatus(void)
{
	uint8_t status;
	SLAVESELECT;
	SPI_Transfer(COMMAND_RDSR);
	status = SPI_Transfer(DUMMY);
	SLAVEDESELECT;
	return status;
}

void SetAddress(unsigned long address)
{
	SPI_Transfer( (uint8_t) (address >> 16) );
	SPI_Transfer( (uint8_t) (address >> 8) );
	SPI_Transfer( (uint8_t) address );
}

void Read(unsigned long  address, char * value, uint16_t  length)
{
	uint16_t i;
	
	SLAVESELECT;
	SPI_Transfer(COMMAND_READ);
	SetAddress(address);
	for(i = 0;i < length;i ++)
	{
		*(value + i) = SPI_Transfer(DUMMY);
	}
	SLAVEDESELECT;
	//while(GetStatus() & STATUS_WIP);
}



void FastRead(unsigned long address, char * value, uint16_t length)
{
	uint16_t i;

	SLAVESELECT;
	SPI_Transfer(COMMAND_READ);
	SetAddress(address);
	SPI_Transfer(DUMMY);
	for(i = 0;i < length;i ++)
	{
		*(value + i) = SPI_Transfer(DUMMY);
	}
	SLAVEDESELECT;
	//while(GetStatus() & STATUS_WIP);
}
#endif 



/**********************************************************************
 * @brief		Get_Identification
 * @param   	None
 * @return	None
 **********************************************************************/
void Get_Identification(uint8_t * id)
{
	uint8_t i;
	
	SLAVESELECT;
	for(i=0;i<10;i++);
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

#if 0
void Read_n(unsigned long  address, char * value, uint16_t  length)
{
	uint16_t i;
	
	SLAVESELECT;
	SPI_Transfer(COMMAND_READ);
	SetAddress(address);
	for(i = 0;i < length;i ++)
	{
		*(value + i) = SPI_Receive();
	}
	SLAVEDESELECT;
	//while(GetStatus() & STATUS_WIP);
}
#endif


void Read_v(unsigned long  address, char * value)
{
	
	SLAVESELECT;
	
	SPI_Transfer(COMMAND_READ);
	SPI_Transfer( (uint8_t) (address >> 16) );
	SPI_Transfer( (uint8_t) (address >> 8) );
	SPI_Transfer( (uint8_t) address );
	*(value) = SPI_Transfer(DUMMY);
	
	SLAVEDESELECT;	
}


void Read(unsigned long  address)
{
	
	SLAVESELECT;

	SPI_Transfer(COMMAND_READ);			// command_read
	SetAddress(address);
	//*(value) = SPI_Receive();
}


void Read_c( char * value)
{

	*(value) = SPI_Receive();
}



void FastRead(unsigned long address, char * value, uint16_t length)
{
	uint8_t i;

	SLAVESELECT;
	for(i=0;i<10;i++);

	SPI_Send(COMMAND_READ);
	SetAddress(address);
	SPI_Send(DUMMY);
	for(i = 0;i < length;i ++)
	{
		*(value + i) = SPI_Receive();
	}
	SLAVEDESELECT;
}


void Read_(unsigned long  address, char * value)
{
	//uint16_t i;
	
	SLAVESELECT;
	SPI_Transfer(COMMAND_READ);
	SetAddress(address);
	*(value) = SPI_Receive();
	SLAVEDESELECT;
}



