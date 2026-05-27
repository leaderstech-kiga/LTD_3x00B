/**
*   @file       a96l41x_dataflash.c
*   @brief      A96L41x DataFlash low-level access (page erase / page write /
*               byte write). Copied verbatim from vendor sample with only
*               formatting/comments cleaned up.
*
*   Usage notes
*   -----------
*   - 256-byte DataFlash region: 0x3000 .. 0x30FF (page size = 32 bytes).
*   - PageBuf scratch (XDATA): 0x7000 .. 0x701F.
*   - Every operation requires a magic user ID parameter (different for
*     erase / page-write / byte-write). These are documented inline.
*/

#include "a96l41x_dataflash.h"

/* Internal state used to satisfy the dual-write safety sequence. */
unsigned char  DATAFLASH_id_reg;
unsigned short DATAFLASH_OPR_FLAG;
unsigned long  dataflash_id;
unsigned short dataflash_addr = 0;

/**
* @brief	Prepare DataFlash access (load XOR'd ID register).
*/
void DATAFLASH_Entry(void) {
	DATAFLASH_id_reg = DATAFLASH_ID ^ DATAFLASH_IDXOR;
}

/**
* @brief	Tear down DataFlash access (clear ID, clear DFSADRH/DFIDR).
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
* @brief	Erase one DataFlash page.
* @param	u32UserId : magic 0xA901358F.
* @param	u16Addr   : page-aligned address in 0x3000..0x30E0.
*/
void DATAFLASH_PageEr(unsigned long u32UserId, unsigned short u16Addr) {
	unsigned char result;

	DATAFLASH_Entry();
	result = Buffer_clear(DATAFLASH_ERASE, 0);
	DFIDR = DATAFLASH_id_reg ^ DATAFLASH_IDXOR;
	if ((u16Addr < 0x3000) || (u16Addr > 0x30E0)) {
		DATAFLASH_Exit();
	}
	else if ((u32UserId == 0xA901358F) && (result != DATAFLASH_PGM_FAIL)) {
		DFSADRH = (unsigned char)(u16Addr >> 8);
		DFSADRL = (unsigned char)(u16Addr >> 0);
		DFMCR   = DF_SEC_ER;
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
* @brief	Page write (32 bytes from caller buffer).
* @param	u32UserId : magic 0x4F17DC86.
* @param	u16Addr   : page-aligned address in 0x3000..0x30E0.
* @param	u8Buf     : pointer to 32-byte payload.
*/
void DATAFLASH_PageWt(unsigned long u32UserId, unsigned short u16Addr, unsigned char *u8Buf) {
	unsigned char result;

	DATAFLASH_Entry();
	result = Buffer_clear(DATAFLASH_SECTOR_WRITE, u8Buf);
	DFIDR = DATAFLASH_id_reg ^ DATAFLASH_IDXOR;
	if ((u16Addr < 0x3000) || (u16Addr > 0x30E0)) {
		DATAFLASH_Exit();
	}
	else if ((u32UserId == 0x4F17DC86) && (result != DATAFLASH_PGM_FAIL)) {
		DFSADRH = (unsigned char)(u16Addr >> 8);
		DFSADRL = (unsigned char)(u16Addr >> 0);
		DFMCR   = DF_SEC_WT;
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
* @brief	Single-byte write (used for sparse updates).
* @param	u32UserId : magic 0xA39B71D6.
*/
void DATAFLASH_ByteWt(unsigned long u32UserId, unsigned short u16Addr, unsigned char u8Byte) {
	unsigned char xdata *pagebuffer = (unsigned char xdata *)0x7000;

	DATAFLASH_Entry();

	DFMCR = DATAFLASH_CLR_PAGEBUF;
	_nop_();
	_nop_();
	_nop_();

	DFIDR = DATAFLASH_id_reg ^ DATAFLASH_IDXOR;
	if ((u16Addr < 0x3000) || (u16Addr > 0x30FF)) {
		DATAFLASH_Exit();
	}
	else if (u32UserId == 0xA39B71D6) {
		pagebuffer[(unsigned char)(u16Addr & 0x0f)] = u8Byte;
		DFSADRH = (unsigned char)(u16Addr >> 8);
		DFSADRL = (unsigned char)(u16Addr >> 0);
		DFMCR   = DF_SEC_WT;
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
* @brief	Clear the 32-byte page buffer, then fill it according to mode.
*           - DATAFLASH_SECTOR_WRITE : copy 32 bytes from u8Buf.
*           - DATAFLASH_ERASE        : fill with 0x00.
* @return	DATAFLASH_PGM_GOOD on success, DATAFLASH_PGM_FAIL on bad mode.
*/
unsigned char Buffer_clear(unsigned char u8FncSel, unsigned char *u8Buf) {
	unsigned char i;
	unsigned char xdata *pagebuffer = (unsigned char xdata *)0x7000;

	DFMCR = DATAFLASH_CLR_PAGEBUF;
	_nop_();
	_nop_();
	_nop_();
	for (i = 0; i < SECTOR_SIZE_BYTE; i++) {
		if (u8FncSel == DATAFLASH_SECTOR_WRITE) {
			pagebuffer[i] = *u8Buf++;
		}
		else if (u8FncSel == DATAFLASH_ERASE) {
			pagebuffer[i] = 0x00;
		}
		else {
			return DATAFLASH_PGM_FAIL;
		}
	}
	return DATAFLASH_PGM_GOOD;
}

/* --------------------------------- End Of File ------------------------------ */
