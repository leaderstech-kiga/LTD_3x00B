/**
*   @file       a96l41x_opamp.h
*   @brief      A96L41x opamp peripheral function 
*   @author     Application Team, ABOV Semiconductor Co., Ltd.
*   @version    V1.01
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
#ifndef __OPAMP_H_
#define __OPAMP_H_
/*******************************************************************************
* Included File
*******************************************************************************/
#include "A96L41x.h"
#include "a96l41x_clock.h"
#include "typedef.h"
#include "Intrins.h"

/*******************************************************************************
* Public Macro
*******************************************************************************/

/*******************************************************************************
* Public Typedef
*******************************************************************************/
enum  gain_1sel	{ 
	GAIN1_DIS	= 0,
	GAIN1_X1,	
	GAIN1_X2, 
	GAIN1_X10,
	GAIN1_X15,
	GAIN1_X20,
	GAIN1_X30,		
	GAIN1_X60, 		
};
enum  gain_0sel	{ 
	GAIN0_DIS	= 0,
	GAIN0_X5,	
	GAIN0_X10, 
	GAIN0_X20,
};
enum  chp_sel	{ 
	chp_125KHz	= 0,
	chp_167KHz,
};

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void OPAMP_Initial(uint8_t gain_1sel, uint8_t gain_0sel, uint8_t chp_clk);
void OP1_Enable(uint8_t enable);
void OP0_Enable(uint8_t enable);

#endif  /* End of __OPAMP_H_ */
/* --------------------------------- End Of File ------------------------------ */
