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


/* MX25L1006E SPI flash command opcodes (only the two issued by this project). */
#define COMMAND_READ    0x03                  /* used by Read() */
#define COMMAND_RDID    0x9F                  /* used by Get_Identification() */
#define DUMMY           0xFF

#define ADDR_START      44                    /* audio data starts 44 bytes into the flash region */

/*******************************************************************************
* Public Typedef
*******************************************************************************/


/*******************************************************************************
* Exported Public Variable
*******************************************************************************/
/* SPI timeout flag - set by SPI_Transfer / SPI_Receive on timeout,
 * cleared on success. Callers can check this after an SPI operation. */
extern volatile uint8_t SPI_Timeout;


/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void Get_Identification(uint8_t * id);
void Read(unsigned long address);
void Read_c(char * value);


#endif

/* --------------------------------- End Of File ------------------------------ */
