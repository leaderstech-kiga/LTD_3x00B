/**
*   @file       a96l41x_dataflash.h
*   @brief      a96l41x dataflash control function 
*   @author     Application Team, ABOV Semiconductor Co., Ltd.
*   @version    V1.00
*   @date       18. DEC. 2020
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
#ifndef __DATAFLASH_H_
#define __DATAFLASH_H_
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
//	Device : A96L41x
//	DataFlash : 256 byte (FFh)
//	PageBuf : 32 byte (20h)
//=======================================================//

//-----------------------------------------------------------------------------
// DATAFLASH Control Constants Definition
#define DATAFLASH_ID                     0x69
#define DATAFLASH_IDXOR                  0x5B
#define DATAFLASH_CLR_PAGEBUF            0x01

#define DATAFLASH_START_ADDR	            0x3000
#define DATAFLASH_END_ADDR		            0x30FF
#define DATAFLASH_BUFFER_ADDR            0x7000
#define SECTOR_SIZE_BYTE	            32

#define DFMBUSY                       7
#define DF_WT    0x7B
#define DF_ER    0xD5

//-----------------------------------------------------------------------------
//  DATAFLASH Erase/Write Code Constanst Definition
#define DATAFLASH_SECTOR_WRITE           0x9C
#define DATAFLASH_ERASE                  0x75
#define DF_SEC_ER                    0x02
#define DF_SEC_WT                    0x04

//-----------------------------------------------------------------------------
// DATAFLASH Procedure Good/Fail Constant Definition
#define DATAFLASH_PGM_GOOD               0x00
#define DATAFLASH_PGM_FAIL               0x09
#define DATAFLASH_PGM_ONGOING            0x9CB4

//----------------------------------------------------------------------------
// Internal Function Definition




/*******************************************************************************
* Public Typedef
*******************************************************************************/

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void DATAFLASH_Entry(void);
void DATAFLASH_Exit(void);
void DATAFLASH_PageEr(unsigned long, unsigned short);
void DATAFLASH_PageWt(unsigned long, unsigned short, unsigned char *);
void DATAFLASH_ByteWt(unsigned long, unsigned short, unsigned char);
unsigned char Buffer_clear(unsigned char, unsigned char *);

#endif //_DATAFLASH_HEADER_

/* --------------------------------- End Of File ------------------------------ */
