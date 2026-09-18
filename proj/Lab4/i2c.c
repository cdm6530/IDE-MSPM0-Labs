/**
 * ******************************************************************************
 * @file    : i2c.c
 * @brief   : UART module header file
 * @details : UART initialization and interaction
 * 
 * @author : Melie Momah
 * @date : 9/9/2026
 * ******************************************************************************
*/
#include "i2c.h"
#include "peripherals/hw_gpio.h"
#include "peripherals/hw_iomux.h"
#include "peripherals/hw_i2c.h"
#include "m0p/mspm0g350x.h"
#include <string.h>


/**
 * @brief Initialize UART0
*/
void I2C1_Init(uint16_t targetAddress){
		if ((I2C1 -> GPRCM.PWREN & I2C_PWREN_ENABLE_MASK) != I2C_PWREN_ENABLE_ENABLE) {
			
        //If not powered, reset the peripheral first
        I2C1 -> GPRCM.RSTCTL |= (I2C_RSTCTL_KEY_UNLOCK_W |I2C_RSTCTL_RESETASSERT_ASSERT);
        
        // Enable power to the peripheral
        I2C1 ->GPRCM.PWREN |= (I2C_PWREN_KEY_UNLOCK_W | I2C_PWREN_ENABLE_ENABLE);
    }
		
		//SCL Pin
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM15] |=  IOMUX_PINCM15_PF_I2C1_SCL;  	//Setting "Peripheral Function Selection" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM15] |= IOMUX_PINCM_PC_CONNECTED;			//Setting "Peripheral is connected" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM15] |= IOMUX_PINCM_INENA_ENABLE;			//enabling scl as an input
		
		//SDA Pin
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM16] |=  IOMUX_PINCM16_PF_I2C1_SDA;		//Setting "Peripheral Function selection" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM16] |= IOMUX_PINCM_PC_CONNECTED	;		//Setting "Peripheral is connected" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM16] |= IOMUX_PINCM_INENA_ENABLE;			//enabling SDA as an input
		
		I2C1-> CLKSEL |= I2C_CLKSEL_BUSCLK_SEL_ENABLE;			//ENABLING busclk
		I2C1 -> CLKDIV  |= I2C_CLKDIV_RATIO_DIV_BY_1;				//setting clock division ratio
		
		I2C1 -> GFCTL &= ~I2C_GFCTL_AGFEN_ENABLE; //disabling analog glitch suppression
		
		I2C1 -> MASTER.MCTR = 0x00000000; //disabling controller control register
		
		I2C1 -> MASTER.MTPR = 0x07;  //setting the timer period value
		
		I2C1 -> MASTER.MFIFOCTL |= I2C_MFIFOCTL_RXTRIG_LEVEL_1; //SETTINg rx fifo to trigger 
		
		I2C1 -> MASTER.MFIFOCTL |= I2C_MFIFOCTL_TXTRIG_EMPTY;
		
		I2C1 -> MASTER.MCR &= ~I2C_MCR_CLKSTRETCH_MASK;
		
		//Setting the target address
		I2C1 -> MASTER.MSA  |= targetAddress;
		
		//Enabling the I2C
		I2C1 -> MASTER.MCR |= I2C_MCR_ACTIVE_ENABLE;
		
	}		

	/**
 * @brief Sends a single character byte over I2C1
 * @param[in] ch - Byte to send
*/
void I2C1_putchar(unsigned char ch){
	
		 while ((I2C1->MASTER.MFIFOSR & I2C_MFIFOSR_TXFIFOCNT_MASK) == I2C_MFIFOSR_TXFIFOCNT_MINIMUM)
    {
       
    }
		I2C1 -> MASTER.MTXDATA = ch;
	
}


/**
 * @brief Send full character string over I2C until limit is reached
 * @param[in] data - String pointer to data to send
 * @param[in] data_size - Amount of bytes to transmit
*/
void I2C1_put(unsigned char *data, uint16_t data_size){
	
}