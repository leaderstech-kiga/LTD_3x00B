/**
*   @file       a96l41x_timer2.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_H_
#define __FLASH_H_
/*******************************************************************************
* Included File
*******************************************************************************/
#include "A96L41x.h"
#include "typedef.h"


/*******************************************************************************
* Public Macro
*******************************************************************************/
#define FLASH_ON		P00 = 1
#define FLASH_OFF		P00 = 0

/*
#define SLAVESELECT 		P20 = 0
#define SLAVEDESELECT 	P20 = 1
*/
#define SLAVESELECT 		P13 = 0
#define SLAVEDESELECT 	P13 = 1

#define COMMAND_WREN  	0x06
#define COMMAND_WRDI  	0x04
#define COMMAND_RDSR  	0x05
#define COMMAND_WRSR  0x01
#define COMMAND_READ  	0x03
#define COMMAND_FREAD 0x0B
#define COMMAND_WRITE 0x02
#define COMMAND_RDID  	0x9F
#define COMMAND_SE    	0x20
#define COMMAND_BE    	0x52
#define COMMAND_CE    	0x60

#define STATUS_WIP    	0x01
#define STATUS_WEL    	0x02
#define STATUS_BP0    	0x04
#define STATUS_BP1    	0x08
#define STATUS_BP2    	0x10
#define STATUS_BP3    	0x20
#define STATUS_RES    	0x40
#define STATUS_SWRD   	0x80

#define DUMMY         		0xFF

#define MAX_BUFF_SIZE	127
//#define DEF_AUDIO_LEN	24452

#define ADDR_START		44				//44

/*******************************************************************************
* Public Typedef
*******************************************************************************/


/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void Get_Identification(uint8_t * id);


void Read_v(unsigned long  address, char * value);
void Read(unsigned long  address);
void Read_c( char * value);



#endif

/* --------------------------------- End Of File ------------------------------ */
