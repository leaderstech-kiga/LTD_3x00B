/**
*   @file       a96l41x_dataflash.h
*   @brief      A96L41x DataFlash control function header.
*
*   Copied from vendor sample (자료/20260401_dataflash/Driver) and committed
*   into src/ so the project links it directly. The 256-byte DataFlash window
*   at 0x3000..0x30FF is used to persist per-unit calibration data
*   (baseline_offset + gain_Q8) across power cycles. See calib.c for the
*   higher-level wrapper.
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
/* Device description ---------------------------------------------------------
 *   Device   : A96L41x
 *   DataFlash: 256 byte (0x3000 - 0x30FF)
 *   PageBuf  : 32 byte at 0x7000 (XDATA)
 *----------------------------------------------------------------------------*/

/* DataFlash control constants */
#define DATAFLASH_ID                     0x69
#define DATAFLASH_IDXOR                  0x5B
#define DATAFLASH_CLR_PAGEBUF            0x01

#define DATAFLASH_START_ADDR             0x3000
#define DATAFLASH_END_ADDR               0x30FF
#define DATAFLASH_BUFFER_ADDR            0x7000
#define SECTOR_SIZE_BYTE                 32

#define DFMBUSY                          7
#define DF_WT                            0x7B
#define DF_ER                            0xD5

/* DataFlash erase / write opcodes */
#define DATAFLASH_SECTOR_WRITE           0x9C
#define DATAFLASH_ERASE                  0x75
#define DF_SEC_ER                        0x02
#define DF_SEC_WT                        0x04

/* Procedure status */
#define DATAFLASH_PGM_GOOD               0x00
#define DATAFLASH_PGM_FAIL               0x09
#define DATAFLASH_PGM_ONGOING            0x9CB4

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void DATAFLASH_Entry(void);
void DATAFLASH_Exit(void);
void DATAFLASH_PageEr(unsigned long u32UserId, unsigned short u16Addr);
void DATAFLASH_PageWt(unsigned long u32UserId, unsigned short u16Addr, unsigned char *u8Buf);
void DATAFLASH_ByteWt(unsigned long u32UserId, unsigned short u16Addr, unsigned char u8Byte);
unsigned char Buffer_clear(unsigned char u8FncSel, unsigned char *u8Buf);

#endif /* __DATAFLASH_H_ */

/* --------------------------------- End Of File ------------------------------ */
