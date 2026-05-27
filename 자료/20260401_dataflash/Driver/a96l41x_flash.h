/**
*   @file       a96l41x_flash.h
*   @brief      a96l41x flash control function 
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
#include "intrins.h"
#include "delay.h"
#include "typedef.h"
/*******************************************************************************
* Public Macro
*******************************************************************************/
//Device Description=====================================//
//	Device : A96L416
//	Flash : 16 Kbyte (3FFFh)
//	PageBuf : 32 byte (20h)
//=======================================================//

//Device Dependent=======================================
#define FLASH_SIZE (0x003FFF)
#define PAGE_BUFFER_SIZE (0x20)

//Modify to use==========================================
//Flash Address for used
#define FLASH_ADDR_USER 0x002000

//Flash Address Min/Max value for permitted area
#define FLASH_ADDR_MIN 0x002000
#define FLASH_ADDR_MAX 0x00201F

//Check the power by LVR/LVI and do not execute under unstable or low power.
//Set LVR/LVI over than 2.00V
#define FLASH_LVR 0x04	//LVR 2.40V(2.17V~2.47V)

//Flash Dummy Address value, not used area
#define FLASH_ADDR_DUMMY 0x03FF00
//=====================================================

//safety code for operation of flash memory
#define FLASH_KEY 0xAA55
#define FLASH_XOR 0x9C75

#define ERASE_MODE	0x66
#define WRITE_MODE	0xCC

#define Flash_Exit()	FMCR = 0x00	                // Set flash to normal mode.

//Flash Identification value
#define Flash_ID 0xa5

//Flash Mode Control value
#define Flash_PageBuffer_Reset 0x01
#define Flash_Erase 0x02
#define Flash_Write 0x03

//Flash User ID1/2/3 value, used password
#define Flash_User_ID1 0x55
#define Flash_User_ID2 0xaa
#define Flash_User_ID3 0x5a



/*******************************************************************************
* Public Typedef
*******************************************************************************/

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void Init_Flash();
void Update_Flash_Buf(unsigned char *buf, unsigned int size);
void Update_Flash(unsigned int key);
void Check_User_ID1(void);
void Check_User_ID2(void);
unsigned char Erase_Flash(unsigned int key, unsigned long sector_address);
unsigned char Write_Flash(unsigned int key, unsigned long sector_address, unsigned int size, unsigned char *buf);
void Read_Flash(unsigned long sector_address, unsigned int size, unsigned char *buf);

#endif //_FLASH_HEADER_

/* --------------------------------- End Of File ------------------------------ */