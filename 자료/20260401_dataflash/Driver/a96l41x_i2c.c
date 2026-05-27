/**
*   @file       a96l41x_i2c.c
*   @brief      A96L41x i2c peripheral function 
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
#include "a96l41x_i2c.h"

/*******************************************************************************
* Private Pre-processor Definition & Macro
*******************************************************************************/

/*******************************************************************************
* Private Typedef
*******************************************************************************/

/*******************************************************************************
* Private Variable
*******************************************************************************/
uint8_t i2c_mode[I2C_MAX_CHANNEL] = {0, };

uint8_t *receiveBuffer0;

uint8_t temp_receiveBuffer0[I2C_MAX_BUFFER_SIZE];

uint8_t receiveCount[I2C_MAX_CHANNEL] = {0, };
uint8_t receiveNum[I2C_MAX_CHANNEL] = {0, };

uint8_t xdata transmitBuffer0[I2C_MAX_BUFFER_SIZE];

uint8_t transmitCount[I2C_MAX_CHANNEL] = {0, };
uint8_t transmitNum[I2C_MAX_CHANNEL] = {0, };
/*******************************************************************************
* Private Function Prototype
*******************************************************************************/
static void I2C_Start(uint8_t ch, uint8_t enable);
static void I2C_Stop(uint8_t ch, uint8_t enable);
static void I2C_Send7bitAddress(uint8_t ch, uint8_t dev_addr, uint8_t direction);
static void I2C_SlaveProcessData(uint8_t ch);
static void I2C_MasterProcessData(uint8_t ch);
static void I2C_GeneralCallProcess(uint8_t ch);
/*******************************************************************************
* Public Function
*******************************************************************************/

/**
* @brief		Initialize i2c peripheral
* @param   ch		This parameter contains the channel of i2c peripheral.
* @param   speed			This parameter contains the buadrate of i2c.
* @param   addr	This parameter contains the slave address of i2c(address[7:1]bit)
* @param   ack	This parameter contains the enable of acknowledge signal. 
*
*					- I2C_ACK_DISABLE = 0
*					- I2C_ACK_ENABLE = 1
* @return		None
*/
void I2C_Initial(uint8_t ch, uint32_t speed, uint8_t addr, uint8_t ack)
{
	uint32_t period = 0;
	uint8_t temp_reg;
	
	receiveNum[ch] = 0;
	transmitNum[ch] = 0;
	
	if(ch == I2C_CH0)
	{
		temp_reg = I2CCR;
		temp_reg &= ~(1 << 6);
		temp_reg |= (1 << 6);
		
		temp_reg &= ~(0x1 << 3);
		temp_reg |= (ack << 3);
	
		I2CCR = temp_reg;
		
		I2CSAR0 = addr;
		
		period = ((Clock_GetSystemFreq() / speed) - 4) / 4;
			
		if(period & 0x1)
		{
			I2CSCHR = (period+1) >> 1;
		}
		else
		{
			I2CSCHR = period >> 1;
		}
		
		I2CSCLR = period >> 1;
		
		/* SDAn output hold timing */
		I2CSDHR = period >> 2;
			
		I2CSR = 0x00;
		
	}
	
}

/**
* @brief		Enables or disables the I2C peripheral.
* @param   ch		This parameter contains the channel of I2C peripheral.
* @param   enable   This parameter contains the enable of this function. 
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void I2C_Enable(uint8_t ch, uint8_t enable)
{
	if(ch == I2C_CH0)
	{
		if(enable == TRUE)
		{
			I2CCR |= (1 << 6);
		}
		else
		{
			I2CCR &= ~(1 << 6);
		}
	}
}


/**
* @brief		Enables or disables the I2C general call function.
* @param   ch		This parameter contains the channel of I2C peripheral.
* @param   enable   This parameter contains the enable of this function. 
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void I2C_ConfigureGerneralCall(uint8_t ch, uint8_t enable)
{
	if(ch == I2C_CH0)
	{
		if(enable == TRUE)
		{
			I2CSAR0 |= 0x1;
		}
		else
		{
			I2CSAR0 &= ~0x1;
		}
	}
}

/**
* @brief		Enables or disables the I2C Start condition.
* @param   ch		This parameter contains the channel of I2C peripheral.
* @param   enable   This parameter contains the enable of this function. 
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void I2C_Start(uint8_t ch, uint8_t enable)
{
	if(ch == I2C_CH0)
	{
		if(enable == TRUE)
		{
			I2CCR |= 0x1;
		}
		else
		{
			I2CCR &= ~0x1;
		}
	}
}

/**
* @brief		Enables or disables the I2C Stop condition.
* @param   ch		This parameter contains the channel of I2C peripheral.
* @param   enable   This parameter contains the enable of this function. 
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void I2C_Stop(uint8_t ch, uint8_t enable)
{
	if(ch == I2C_CH0)
	{
		if(enable == TRUE)
		{
			I2CCR |= 0x2;
		}
		else
		{
			I2CCR &= ~0x2;
		}
	}
}

/**
* @brief		Enables or disables the I2C Software reset
* @param   ch		This parameter contains the channel of I2C peripheral.
* @param   enable   This parameter contains the enable of this function. 
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void I2C_SoftwareReset(uint8_t ch, uint8_t enable)
{
	if(ch == I2C_CH0)
	{
		if(enable == TRUE)
		{
			I2CCR |= 0x20;
		}
		else
		{
			I2CCR &= ~0x20;
		}
	}
}


/**
* @brief		Configure the I2C Acknowledge signal.
* @param   ch		This parameter contains the channel of I2C peripheral.
* @param   enable   This parameter contains the enable of this function. 
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void I2C_ConfigureAcknowledge(uint8_t ch, uint8_t enable)
{
	if(ch == I2C_CH0)
	{
		if(enable == TRUE)
		{
			I2CCR |= 0x8;
		}
		else
		{
			I2CCR &= ~0x8;
		}
	}
}


/**
* @brief		Write multi bytes by writing as i2c master.
* @param   ch		This parameter contains the channel of I2C peripheral.
* @param   dev_addr			This parameter contains the slave address of target.
* @param   *write_data			This parameter contains the write data of i2c(pointer variable)
* @param   write_len			This parameter contains the number of write length.
* @param   *read_data			This parameter contains the read data of i2c(pointer variable)
* @param   read_len			This parameter contains the number of read length.
* @return		None
*/
void I2C_MasterTransferData(uint8_t ch, uint8_t dev_addr, uint8_t *write_data, uint8_t write_len, uint8_t *read_data, uint8_t read_len)
{
	uint8_t i;
	
	if(ch == I2C_CH0)
	{
		for(i = 0; i < I2C_MAX_BUFFER_SIZE; i++)
			transmitBuffer0[i] = write_data[i];
		
		receiveBuffer0 =  (uint8_t*)read_data;
	}

	transmitCount[ch] = write_len;
	receiveCount[ch] = read_len;
	
	i2c_mode[ch] = I2C_BUSY;
	
	if(write_len != 0)
		I2C_Send7bitAddress(ch, dev_addr, I2C_WRITE_MODE);
	else
		I2C_Send7bitAddress(ch, dev_addr, I2C_READ_MODE);
	
	while(i2c_mode[ch] == I2C_BUSY)
	{
		//timeout
	}

}

/**
* @brief		Send slave address as i2c master.
* @param   ch		This parameter contains the channel of I2C peripheral.
* @param   dev_addr			This parameter contains the slave address of target.
* @param   direction			This parameter contains the mode of i2c(read or write).
*
*					- I2C_WRITE_MODE 	= 0
*					- I2C_READ_MODE 	= 1
* @return		None
*/
void I2C_Send7bitAddress(uint8_t ch, uint8_t dev_addr, uint8_t direction)
{
	
	if(ch == I2C_CH0)
	{
		if(I2CSR == BUSY) 
			I2C_Initial(ch, I2C_SPEED, 0x00, I2C_ACK_ENABLE);
		
		I2CDR = dev_addr | direction;
	}
	
	I2C_Start(ch, TRUE);
	
}

/**
* @brief		Response specific bytes as i2c master.(custom user)
* @param   ch		This parameter contains the channel of I2C peripheral.
* @return		None
*/
static void I2C_MasterProcessData(uint8_t ch)
{
	uint8_t i2c_status;
	
	if(ch == I2C_CH0)
	{
		i2c_status = I2CSR;
	}
	
	if(i2c_status & RXACK)
	{
		// Master Tx mode
		if(i2c_status & TMODE)
		{
			if(transmitCount[ch])
			{
				if(ch == I2C_CH0)
					I2CDR = transmitBuffer0[transmitNum[ch]];	
				
				transmitCount[ch] -= 1;
				transmitNum[ch] += 1;
			}
			else
			{
				if(receiveCount[ch])
				{
					//Restart for Rx
					I2C_Send7bitAddress(ch, I2C_DEVICE_ADDRESS, I2C_READ_MODE);
				}
				else
				{
					// TxLength 0 Stop
					transmitNum[ch] = 0;
					I2C_Stop(ch, TRUE);
				}
			}
		}
		// Master Rx mode
		else
		{
			if(i2c_status&GCALL)
			{
				// Address ACK is received
				if(receiveCount[ch] == 1)
				{
					I2C_ConfigureAcknowledge(ch, FALSE);	// NoAck generate
				}
			}
			else
			{
				receiveCount[ch] -= 1;
				
				if(receiveCount[ch] > 1)
				{
					if(ch == I2C_CH0)
						receiveBuffer0[receiveNum[ch]] = I2CDR;
					
					receiveNum[ch] += 1;
				}
				else if(receiveCount[ch] == 1)
				{
					if(ch == I2C_CH0)
						receiveBuffer0[receiveNum[ch]] = I2CDR;
					
					receiveNum[ch] += 1;
					
					I2C_ConfigureAcknowledge(ch, FALSE);	// NoAck generate
				}
				else
				{
					receiveNum[ch] = 0;
					I2C_Stop(ch, TRUE);
				}
				
			}
		}
	}
	else
	{
		// NoAck data is received
		if(receiveCount[ch] == 1)
		{
			receiveCount[ch] = 0;
			
			if(ch == I2C_CH0)
				receiveBuffer0[receiveNum[ch]] = I2CDR;
		}
		
		// NoAck Stop
		receiveNum[ch] = 0;
		I2C_Stop(ch, TRUE);
		
	}

}

/**
* @brief		Response specific bytes as i2c slave.(custom user)
* @param   ch		This parameter contains the channel of I2C peripheral.
* @return		None
*/
static void I2C_SlaveProcessData(uint8_t ch)
{

	uint8_t i2c_status;
	
	if(ch == I2C_CH0)
	{
		i2c_status = I2CSR;
	}

	if((i2c_status & GCALL))
	{
		I2C_GeneralCallProcess(ch); //USER CODE;
	}
	else
	{
		//USER CODE;
		//USER CODE;
		
		if(i2c_status & TMODE) // Slave Tx mode
		{
				if(ch == I2C_CH0)
					I2CDR = transmitBuffer0[transmitNum[ch]+1];
					
				transmitCount[ch] -= 1;
				transmitNum[ch] += 1;
					
				if(transmitCount[ch] == 1) 
				{
					i2c_mode[ch] = I2C_IDLE;
					receiveNum[ch] = 0;
					transmitNum[ch] = 0;
				}
		}
		else  // Slave Rx mode
		{
			
			if(ch == I2C_CH0)	
			{
				temp_receiveBuffer0[receiveNum[ch]] = I2CDR;
				//eco
				transmitBuffer0[receiveNum[ch]] = temp_receiveBuffer0[receiveNum[ch]];
			}
			
			receiveNum[ch] += 1;
			//eco
			transmitCount[ch] = receiveNum[ch];
		}
			//USER CODE;
			//USER CODE;
			
	}
		//END
}

/**
* @brief		Handle general call as i2c process.(custom user)
* @param   ch		This parameter contains the channel of I2C peripheral.
* @return		None
*/
static void I2C_GeneralCallProcess(uint8_t ch)
{
	if(ch == I2C_CH0)
		NOP;
	
	//USER CODE
	//USER CODE
}


/**
* @brief		Configure the enable or disable I2C interrupt.
* @param   ch		This parameter contains the channel of I2C peripheral.
* @param   enable   This parameter contains the enable of this function. 
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void I2C_ConfigureInterrupt(uint8_t ch, uint8_t enable)
{
	if(ch == I2C_CH0)
	{
		if(enable == TRUE)
		{
			IE1 |= 0x02;
			I2CCR |= 0x10;
		}
		else
		{
			I2CCR &= ~0x10;
			IE1 &= ~0x02;
		}
	}
}

/**
* @brief		Get the I2C interrupt status.
* @param   ch		This parameter contains the channel of I2C peripheral.
* @return		Value of status(interrupt flag)
*/
uint8_t I2C_GetInterruptStatus(uint8_t ch)
{
	uint8_t result;

	if(ch == I2C_CH0)
	{
		result = I2CSR;
	}
	
	return result;
}


/**
* @brief		Configure the enable or disable I2C interrupt.
* @param   ch		This parameter contains the channel of I2C peripheral.
* @return		None
*/
void I2C_InterruptHandler(uint8_t ch)
{
	uint8_t i2c_status, i2c_operation;
	
	if(ch == I2C_CH0)
	{
		i2c_status = I2CSR;
		i2c_operation = I2CCR;
	}
	
	
	if((i2c_status & MLOST) || (i2c_status & STOPD))
	{
		i2c_mode[ch] = I2C_IDLE;
		I2C_Initial(ch, I2C_SPEED, I2C_DEVICE_ADDRESS, I2C_ACK_ENABLE);
	}
	else
	{
			if(i2c_operation & 0x4)  //Master mode
			{
				I2C_MasterProcessData(ch);
			}
			else  //Slave mode
			{
				I2C_SlaveProcessData(ch);
			}
	}
	
	
	if(ch == I2C_CH0)
	{
		I2CSR = 0x00;
	}
	
}

void I2C_Int_Handler(void) interrupt I2C_VECT
{
	I2C_InterruptHandler(I2C_CH0);
}

/* --------------------------------- End Of File ------------------------------ */