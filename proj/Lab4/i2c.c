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
#include "oled.h" 
#include "peripherals/hw_gpio.h"
#include "peripherals/hw_iomux.h"
#include "peripherals/hw_i2c.h"
#include "m0p/mspm0g350x.h"
#include <string.h>


/**
 * @brief Initialize UART0
*/
void I2C1_init(uint16_t targetAddress){
		if ((I2C1 -> GPRCM.PWREN & I2C_PWREN_ENABLE_MASK) != I2C_PWREN_ENABLE_ENABLE) {
			
        //If not powered, reset the peripheral first
        I2C1 -> GPRCM.RSTCTL |= (I2C_RSTCTL_KEY_UNLOCK_W |I2C_RSTCTL_RESETASSERT_ASSERT);
        
        // Enable power to the peripheral
        I2C1 ->GPRCM.PWREN |= (I2C_PWREN_KEY_UNLOCK_W | I2C_PWREN_ENABLE_ENABLE);
    }
		
		//SCL Pin
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM15] |=  IOMUX_PINCM15_PF_I2C1_SCL;  	//Setting "Peripheral Function Selection" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM15] |= (IOMUX_PINCM_PC_CONNECTED | 0x04);			//Setting "Peripheral is connected" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM15] |= IOMUX_PINCM_INENA_ENABLE;			//enabling scl as an input
		
		//SDA Pin
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM16] |=  IOMUX_PINCM16_PF_I2C1_SDA;		//Setting "Peripheral Function selection" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM16] |= (IOMUX_PINCM_PC_CONNECTED | 0x04)	;		//Setting "Peripheral is connected" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM16] |= IOMUX_PINCM_INENA_ENABLE;			//enabling SDA as an input
		
		I2C1-> CLKSEL = I2C_CLKSEL_BUSCLK_SEL_ENABLE;			//ENABLING busclk
		I2C1 -> CLKDIV  = I2C_CLKDIV_RATIO_DIV_BY_1;				//setting clock division ratio
		
		I2C1 -> GFCTL &= ~I2C_GFCTL_AGFEN_ENABLE; //disabling analog glitch suppression
		
		I2C1 -> MASTER.MCTR = 0x00000000; //disabling controller control register
		
		I2C1 -> MASTER.MTPR = 0x1F;  //setting the timer period value
		
		I2C1 -> MASTER.MFIFOCTL |= I2C_MFIFOCTL_RXTRIG_LEVEL_1; //SETTINg rx fifo to trigger 
		
		I2C1 -> MASTER.MFIFOCTL |= I2C_MFIFOCTL_TXTRIG_EMPTY;
		
		I2C1 -> MASTER.MCR &= ~I2C_MCR_CLKSTRETCH_MASK;
		
		//Setting the target address
		I2C1 -> MASTER.MSA  |= (targetAddress << 1);
		
		//Enabling the I2C
		I2C1 -> MASTER.MCR |= I2C_MCR_ACTIVE_ENABLE;
		
	}		

	/**
 * @brief Sends a single character byte over I2C1
 * @param[in] ch - Byte to send
*/
void I2C1_putchar(unsigned char ch){
	
		
		 while (!((I2C1->MASTER.MFIFOSR & I2C_MFIFOSR_TXFIFOCNT_MASK) >=  I2C_MFIFOSR_TXFIFOCNT_MINIMUM)){
				// Do Nothing until FIFO is empty :)
		}
       //wait until FIFO has space
    
		I2C1 -> MASTER.MTXDATA |= ch;
	
}


/**
 * @brief Send full character string over I2C until limit is reached
 * @param[in] data - String pointer to data to send
 * @param[in] data_size - Amount of bytes to transmit
*/
void I2C1_put(unsigned char *data, uint16_t data_size){
			uint16_t offset = 0;

			/* Config of the MSA and MCTR registers */
			I2C1->MASTER.MSA &= ~I2C_MSA_DIR_RECEIVE; // Trasmit Mode
			I2C1->MASTER.MCTR |= ((uint32_t)data_size << (uint32_t)I2C_MCTR_MBLEN_OFS) & I2C_MCTR_MBLEN_MAXIMUM; // Setting Length to "data_size"
			I2C1->MASTER.MCTR |= I2C_MCTR_BURSTRUN_ENABLE; // Burst Run Enabled
			I2C1->MASTER.MCTR |= I2C_MCTR_START_ENABLE; // Start Condition Enabled
			I2C1->MASTER.MCTR |= I2C_MCTR_STOP_ENABLE ; // Stop Condtion Enabled

			
			for (uint16_t fset = 0;fset < data_size; fset++){
				I2C1_putchar(data[offset]);
			}

			while (!(I2C1->MASTER.MSR & I2C_MSR_IDLE_MASK)){
			}

			I2C1->MASTER.MCTR &= ~I2C_MCTR_BURSTRUN_ENABLE; // Disabling the Module after TX
}