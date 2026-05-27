/**
*   @file       a96l41x_flash.c
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

/*******************************************************************************
* Included File
*******************************************************************************/
#include	"a96l41x_flash.h"	

/*******************************************************************************
* Private Pre-processor Definition & Macro
*******************************************************************************/

/*******************************************************************************
* Private Typedef
*******************************************************************************/

/*******************************************************************************
* Private Variable
*******************************************************************************/
// User ID1/2/3 global value
unsigned char User_ID1;
unsigned char User_ID2;
unsigned char User_ID3;

unsigned char *PageBufData;
unsigned int PageBufDataSize;
unsigned char FlashMode = 0;
/*******************************************************************************
* Private Function Prototype
*******************************************************************************/

/*******************************************************************************
* Public Function
*******************************************************************************/
/**
* @brief	Safety code operation of flash memory initialization.
* @param    None
* @return	None
*/
void Init_Flash()
{
	//Set LVR over than 2.00V
	LVRCR = 0x04;//LVR enable, 2.40V
		
	//Init User_ID
	User_ID1 = 0x00;
	User_ID2 = 0x00;
	User_ID3 = 0x00;

	// Set Flash Sector Address as Dummy address
	FSADRH = (FLASH_ADDR_DUMMY >> 16) & 0x0f;	//Flash Sector Address High Register
	FSADRM = (FLASH_ADDR_DUMMY >> 8) & 0xff;	//Flash Sector Address Middle Register
	FSADRL = (FLASH_ADDR_DUMMY >> 0) & 0xff;	//Flash Sector Address Low Register
}

/**
* @brief	Update Flash buffer for writing flash memory.
* @param    buf 	This parameter contains the buffer of writing Flash memory.
* @param    buf_size 	This parameter contains the size of data.
* @return	None
*/
void Update_Flash_Buf(unsigned char *buf, unsigned int buf_size)
{
	if (FlashMode == ERASE_MODE)
	{
		FlashMode = WRITE_MODE;
	}	
	else
	{
		FlashMode = ERASE_MODE;
	}	
	//Flash Buffer Update
	User_ID1 = Flash_User_ID1;
	PageBufData = buf;
	PageBufDataSize = buf_size;
}

/**
* @brief	Safety code(Check User ID1 and set User ID2) operation of flash memory.
* @param    None
* @return	None
*/
void Check_User_ID1()
{
	if(User_ID1 == Flash_User_ID1)
	{
		User_ID2 = Flash_User_ID2;
	}
}

/**
* @brief	Safety code(Check User ID2 and set User ID3) operation of flash memory.
* @param    None
* @return	None
*/
void Check_User_ID2()
{
	if(User_ID2 == Flash_User_ID2)
	{
		User_ID3 = Flash_User_ID3;
	}
}

/**
* @brief	Update flash memory for writing flash memory.
* @param    key 	This parameter contains the address of Flash memory.
*
*					- safety code for operation of flash memory
* @return	None
*/
void Update_Flash(unsigned int key)
{
	unsigned char i = 0;
	unsigned int s = 0;
	unsigned char ret = 0;

	if(key == FLASH_KEY)
	{
			s = PageBufDataSize;
			while(PageBufDataSize > 0)
			{
				if(PageBufDataSize < PAGE_BUFFER_SIZE)
				{
					s = PageBufDataSize;
					PageBufDataSize = 0;
				}
				else
				{
					s = PAGE_BUFFER_SIZE;
					PageBufDataSize -= PAGE_BUFFER_SIZE;
				}
				if (FlashMode == ERASE_MODE)
				{
					ret = Erase_Flash(FLASH_KEY, FLASH_ADDR_USER + (PAGE_BUFFER_SIZE*i));
					if(ret == 1 || ret == 2)
					{
						//error;
						while(1);
					}
				}
				else if (FlashMode == WRITE_MODE)
				{
					ret = Write_Flash(FLASH_KEY, FLASH_ADDR_USER + (PAGE_BUFFER_SIZE*i), 
								s, PageBufData + (PAGE_BUFFER_SIZE*i));
					if(ret == 1 || ret == 4)
					{
						//error;
						while(1);
	
					}
				}
				i++;
			}
	}
}



/**
* @brief	Erase operation of flash memory.
* @param    key 	This parameter contains the address of Flash memory.
*
*					- safety code for operation of flash memory
* @param    sector_address 	This parameter contains the address of Flash memory.
*
*					- address range : 16kbytes(0x0 ~ 0x3FFF)
* @return	status(0 : OK, 1/2 : Failed)
*/
unsigned char Erase_Flash(unsigned int key, unsigned long sector_address)
{
	unsigned char code *rom_addr = 0x0000;
	unsigned char xdata *x_addr=0x8000;		// Page buffer address
	unsigned char i = 0;
	unsigned char tmp = 0;
	unsigned char ret = 0;
	unsigned long addr = 0;
	
	//Disable EA
	tmp = IE;
	IE &= (~0x80);

	if(key == FLASH_KEY)
	{
		key ^= FLASH_XOR;

		if(sector_address < (unsigned long)FLASH_SIZE)
		{
	
			// erase
			FMCR=0x01;									// Page buffer clear
			_nop_( );	//Dummy instruction, This instruction must be needed.
			_nop_( );	//Dummy instruction, This instruction must be needed.
			_nop_( );	//Dummy instruction, This instruction must be needed.
		
			//write data to pagebuffer
			for(i=0; i<PAGE_BUFFER_SIZE; i++)	x_addr[i]= 0x00;

			// Set Flash Sector Address as used area
			FSADRH = (sector_address >> 16) & 0xFF;
			FSADRM = (sector_address >> 8) & 0xFF;
			FSADRL = (sector_address) & 0xFF;

			// Set Flash Identification Register
			FIDR = Flash_ID;//auto clear
			
			// Check User_ID1/2/3
			if(User_ID1 == Flash_User_ID1 
				&& User_ID2 == Flash_User_ID2 
				&& User_ID3 == Flash_User_ID3)
				{
			// Check valid of Flash Sector Address range
				addr = ((unsigned long)FSADRH << 16) 
					+ ((unsigned long)FSADRM << 8) 
					+ ((unsigned long)FSADRL << 0);
				if( FLASH_ADDR_MIN <= addr &&  addr <= FLASH_ADDR_MAX)
				{
					if((key ^ FLASH_XOR) == FLASH_KEY)
					{
						// Flase Erase
						FMCR = Flash_Erase;	// Start Erase.
						_nop_();_nop_();_nop_(); 	//Dummy instruction, This instruction must be needed.

						while(FMCR & 0x80);		// Flash Mode Busy Bit. This bit will be used for only debugger.
						_nop_();_nop_();_nop_(); 	//Dummy instruction, This instruction must be needed.
					}
					
					Flash_Exit();			            // Set flash to normal mode.
					//verify data
					for(i = 0; i < PAGE_BUFFER_SIZE; i++)
					{
						if(rom_addr[sector_address + i] != 0x00)
						{
							ret = 2;
						}
					}
				}
			}
				
			//Initialize
			User_ID1 = 0x00;
			User_ID2 = 0x00;
			User_ID3 = 0x00;
			
			// Set Flash Sector Address as Dummy address
			FSADRH = (FLASH_ADDR_DUMMY >> 16) & 0x0f;	//Flash Sector Address High Register
			FSADRM = (FLASH_ADDR_DUMMY >> 8) & 0xff;		//Flash Sector Address Middle Register
			FSADRL = (FLASH_ADDR_DUMMY >> 0) & 0xff;		//Flash Sector Address Low Register
		}
	}
	IE = tmp;
	return ret;
}


/**
* @brief	Wrtie operation of flash memory.
* @param    key 	This parameter contains the address of Flash memory.
*
*					- safety code for operation of flash memory
* @param    sector_address 	This parameter contains the address of Flash memory.
*
*					- address range : 32kbytes(0x0 ~ 0x3FFF)
* @param    buf_size 	This parameter contains the size of writing Flash data.
* @param    buf 	This parameter contains the buffer of writing Flash data
* @return	status(0 : OK, 1/4 : Failed)
*/
unsigned char Write_Flash(unsigned int key, unsigned long sector_address, unsigned int buf_size, unsigned char *buf)
{
	unsigned char *pdt;
	unsigned char code *rom_addr=0x0000;
	unsigned char xdata *x_addr=0x8000;		// Page buffer address
	unsigned char i = 0;
	unsigned char tmp = 0;
	unsigned char ret = 0;
	unsigned long addr = 0;

	//Disable EA
	tmp = IE;
	IE &= (~0x80);

	if(key == FLASH_KEY)
	{
		key ^= FLASH_XOR;

		if(sector_address < (unsigned long)FLASH_SIZE)
		{			
			// write
			FMCR=0x01;									// Page buffer clear
			_nop_( );	//Dummy instruction, This instruction must be needed.
			_nop_( );	//Dummy instruction, This instruction must be needed.
			_nop_( );	//Dummy instruction, This instruction must be needed.

			//write data to pagebuffer
			pdt = buf;
			for(i=0; i<buf_size; i++)	
			{
				x_addr[i]= *pdt;
				pdt++;
			}
			
			
			
			// Set Flash Sector Address as used area
			FSADRH = (sector_address >> 16) & 0xFF;
			FSADRM = (sector_address >> 8) & 0xFF;
			FSADRL = (sector_address) & 0xFF;
			
			// Set Flash Identification Register
			FIDR = Flash_ID;//auto clear
			
			// Check User_ID1/2/3
			if(User_ID1 == Flash_User_ID1 
				&& User_ID2 == Flash_User_ID2 
				&& User_ID3 == Flash_User_ID3)
			{
			// Check valid of Flash Sector Address range
				addr = ((unsigned long)FSADRH << 16) 
					+ ((unsigned long)FSADRM << 8) 
					+ ((unsigned long)FSADRL << 0);
				if( FLASH_ADDR_MIN <= addr &&  addr <= FLASH_ADDR_MAX)
				{
					if((key ^ FLASH_XOR) == FLASH_KEY)
					{
						// Flase Write
						FMCR = Flash_Write;	        // Start program.
						_nop_();_nop_();_nop_(); 	//Dummy instruction, This instruction must be needed.
						
						while(FMCR & 0x80);		// Flash Mode Busy Bit. This bit will be used for only debugger.
						_nop_();_nop_();_nop_(); 	//Dummy instruction, This instruction must be needed.
					}

					Flash_Exit();			            // Set flash to normal mode.
					//verify data
					pdt = buf;
					for(i = 0; i < PAGE_BUFFER_SIZE; i++)
					{
						if(rom_addr[sector_address + i] != *pdt)
						{
							ret = 4;
						}
						
						pdt++;
					}
				}
			}
			//Initialize
			User_ID1 = 0x00;
			User_ID2 = 0x00;
			User_ID3 = 0x00;
			
			// Set Flash Sector Address as Dummy address
			FSADRH = (FLASH_ADDR_DUMMY >> 16) & 0x0f;	//Flash Sector Address High Register
			FSADRM = (FLASH_ADDR_DUMMY >> 8) & 0xff;		//Flash Sector Address Middle Register
			FSADRL = (FLASH_ADDR_DUMMY >> 0) & 0xff;		//Flash Sector Address Low Register
		}
	}
	IE = tmp;
	return ret;
}


/**
* @brief	Read operation of flash memory.
* @param    sector_address 	This parameter contains the address of Flash memory.
*
*					- address range : 16kbytes(0x0 ~ 0x3FFF)
* @param    buf_size 	This parameter contains the size of reading Flash data.
* @param    buf 	This parameter contains the data to read flash memory.
* @return	NONE
*/
void Read_Flash(unsigned long sector_address, unsigned int buf_size, unsigned char *buf)
{
	unsigned char code *rom_addr=0x0000;
	unsigned int i = 0;

	for(i = 0; i < buf_size; i++)
	{
		buf[i] = rom_addr[sector_address + i];
	}
}

/* --------------------------------- End Of File ------------------------------ */