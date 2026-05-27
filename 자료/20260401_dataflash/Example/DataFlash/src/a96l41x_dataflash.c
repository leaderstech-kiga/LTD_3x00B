/**
*   @file       a96l41x_dataflash.c
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

/*******************************************************************************
* Included File
*******************************************************************************/
#include	"a96l41x_dataflash.h"	

/*******************************************************************************
* Private Pre-processor Definition & Macro
*******************************************************************************/

/*******************************************************************************
* Private Typedef
*******************************************************************************/

/*******************************************************************************
* Private Variable
*******************************************************************************/
unsigned char DATAFLASH_id_reg;
unsigned short DATAFLASH_OPR_FLAG;
unsigned long dataflash_id;
unsigned short dataflash_addr = 0;
unsigned char dataflash_len;
/*******************************************************************************
* Private Function Prototype
*******************************************************************************/

/*******************************************************************************
* Public Function
*******************************************************************************/
/**
* @brief	entry code operation of dataflash.
* @param    None
* @return	None
*/
//-----------------------------------------------------------------------------
void DATAFLASH_Entry(void) {
	DATAFLASH_id_reg = DATAFLASH_ID ^ DATAFLASH_IDXOR;
}
/**
* @brief	exit code operation of dataflash.
* @param    None
* @return	None
*/
void DATAFLASH_Exit(void) {
	DATAFLASH_id_reg = 0;
	DATAFLASH_OPR_FLAG = 0;
	dataflash_id = 0;
	dataflash_addr &= 0x00ff;
	DFSADRH = 0;
	DFIDR = 0;
}

/**
* @brief	Erase operation of dataflash.
* @param    u32UserId 	This parameter contains the UserID of DataFlash.
*
*					- safety code for operation of DataFlash
* @param    u16Addr 	This parameter contains the address of DataFlash.
*
*					- address range : 256 bytes (0x3000 ~ 0x30FF)
* @return	None
*/
void DATAFLASH_PageEr(unsigned long u32UserId, unsigned short u16Addr) {
	unsigned char result;
	
	DATAFLASH_Entry();
	result = Buffer_clear(DATAFLASH_ERASE, 0);
	DFIDR = DATAFLASH_id_reg^DATAFLASH_IDXOR;                                     // Identification Value
	if((u16Addr<0x3000)||(u16Addr>0x30E0)) DATAFLASH_Exit();
	else if((u32UserId == 0xA901358F)&&(result!=DATAFLASH_PGM_FAIL)) {         // Ex) 0xA901358F, The user ID may be changed by programmer
		DFSADRH = (unsigned char)(u16Addr>>8);
		DFSADRL = (unsigned char)(u16Addr>>0);
		DFMCR = DF_SEC_ER;
		_nop_();
	  	_nop_();
	  	_nop_();
	  	DATAFLASH_Exit();
	  	DATAFLASH_OPR_FLAG = DATAFLASH_PGM_ONGOING;
	}
	else {
		DATAFLASH_Exit();
	}
}


/**
* @brief	Wrtie operation of flash.
* @param    u32UserId 	This parameter contains the UserID of DataFlash.
*
*					- safety code for operation of DataFlash
* @param    u16Addr 	This parameter contains the address of DataFlash.
*
*					- address range : 256 bytes (0x3000 ~ 0x30FF)
* @param    u8Buf 	This parameter contains the data of DataFlash.
* @return	None
*/
void DATAFLASH_PageWt(unsigned long u32UserId, unsigned short u16Addr, unsigned char *u8Buf) {
	unsigned char result;
	
	DATAFLASH_Entry();
	result = Buffer_clear(DATAFLASH_SECTOR_WRITE, u8Buf);
	DFIDR = DATAFLASH_id_reg^DATAFLASH_IDXOR;                                     // Identification Value
	if((u16Addr<0x3000)||(u16Addr>0x30E0)) DATAFLASH_Exit();
	else if((u32UserId == 0x4F17DC86)&&(result!=DATAFLASH_PGM_FAIL)) {         // Ex) 0x4F17DC86, The user ID may be changed by programmer
		DFSADRH = (unsigned char)(u16Addr>>8);
		DFSADRL = (unsigned char)(u16Addr>>0);
		DFMCR = DF_SEC_WT;
		_nop_();
	 	_nop_();
	  	_nop_();
	  	DATAFLASH_Exit();
	  	DATAFLASH_OPR_FLAG = DATAFLASH_PGM_ONGOING;
	}
	else {
		DATAFLASH_Exit();
	}
}


/**
* @brief	Byte Write operation of dataflash.
* @param    u32UserId 	This parameter contains the UserID of DataFlash.
*
*					- safety code for operation of DataFlash
* @param    u16Addr 	This parameter contains the address of DataFlash.
*
*					- address range : 256 bytes (0x3000 ~ 0x30FF)
* @param    u8Byte 	This parameter contains the data of DataFlash.
* @return	NONE
*/
void DATAFLASH_ByteWt(unsigned long u32UserId, unsigned short u16Addr, unsigned char u8Byte) {
	unsigned char xdata *pagebuffer=0x7000;
	
	DATAFLASH_Entry();
	
	DFMCR = DATAFLASH_CLR_PAGEBUF;                                             // Clear sector buffer
	_nop_();
	_nop_();
	_nop_();
	
	DFIDR = DATAFLASH_id_reg^DATAFLASH_IDXOR;                                     // Identification Value
	if((u16Addr<0x3000)||(u16Addr>0x30FF)) DATAFLASH_Exit();
	else if(u32UserId == 0xA39B71D6) {                                      // Ex) 0xA39B71D6, The user ID may be changed by programmer
		pagebuffer[(unsigned char)(u16Addr&0x0f)] = u8Byte;		
		DFSADRH = (unsigned char)(u16Addr>>8);
		DFSADRL = (unsigned char)(u16Addr>>0);
		DFMCR = DF_SEC_WT;
 		_nop_();
 	  	_nop_();
 	  	_nop_();
	  	DATAFLASH_Exit();
	  	DATAFLASH_OPR_FLAG = DATAFLASH_PGM_ONGOING;
	}
	else {
		DATAFLASH_Exit();
	}
}


/**
* @brief	Buffer clear operation of dataflash.
* @param    u8FncSel 	This parameter contains the mode function select of DataFlash.
*					- Sector erase, Sector write
* @param    u8Buf 	This parameter contains the data of DataFlash.
* @return	DATAFLASH_PGM_GOOD
*/
unsigned char Buffer_clear(unsigned char u8FncSel, unsigned char *u8Buf) {
	unsigned char i;
	unsigned char xdata *pagebuffer=0x7000;
	
	DFMCR = DATAFLASH_CLR_PAGEBUF;                                             // Clear sector buffer
	_nop_();
	_nop_();
	_nop_();
	for(i=0 ; i < SECTOR_SIZE_BYTE ; i++) {
		if(u8FncSel == DATAFLASH_SECTOR_WRITE) pagebuffer[i] = *u8Buf++;         // To sector write
		else if(u8FncSel == DATAFLASH_ERASE) pagebuffer[i] = 0x00;               // To sector erase
		else return DATAFLASH_PGM_FAIL;
	}
	return DATAFLASH_PGM_GOOD;
}


/**
* @brief	len Read operation of dataflash.
*
* @param    u16Addr 	This parameter contains the address of DataFlash.
*
*					- address range : 256 bytes (0x3000 ~ 0x30FF)
* @param    u8Byte 	This parameter contains the data of DataFlash.
* @return	NONE
*/
unsigned char  DATAFLASH_PageRd(unsigned short u16Addr, unsigned char *u8Buf, unsigned char u8Len) {
      unsigned char i;
	unsigned char  xdata *pagebuffer;
	
	DATAFLASH_Entry();
	
	DFMCR = DATAFLASH_CLR_PAGEBUF;                                             // Clear sector buffer
	_nop_();
	_nop_();
	_nop_();
	
	DFIDR = DATAFLASH_id_reg^DATAFLASH_IDXOR;                                     // Identification Value
	if((u16Addr<0x3000)||(u16Addr>0x30FF)) 
	{
		DATAFLASH_Exit();
		return DATAFLASH_PGM_FAIL;
	}
	for(i=0; i<u8Len; i++)
	      u8Buf[i]=pagebuffer[(unsigned char)(u16Addr++)];
	
	DATAFLASH_Exit();
	return DATAFLASH_PGM_GOOD;
}

/* --------------------------------- End Of File ------------------------------ */
