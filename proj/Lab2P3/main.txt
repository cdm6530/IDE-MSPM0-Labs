/**
 * ******************************************************************************
 * @file    : uart.c
 * @brief   : UART module header file
 * @details : UART initialization and interaction
 * 
 * @author : Melie Momah
 * @date : 9/9/2026
 * ******************************************************************************
*/
#include "uart.h"
#include "peripherals/hw_gpio.h"
#include "peripherals/hw_iomux.h"
#include "m0p/mspm0g350x.h"
#include <string.h>


/**
 * @brief Initialize UART0
*/
void UART0_init(void){
		if ((UART0 -> GPRCM.PWREN & UART_PWREN_ENABLE_MASK) != UART_PWREN_ENABLE_ENABLE) {
			
        //If not powered, reset the peripheral first
        UART0->GPRCM.RSTCTL |= (UART_RSTCTL_KEY_UNLOCK_W |UART_RSTCTL_RESETASSERT_ASSERT);
        
        // Enable power to the peripheral
        UART0->GPRCM.PWREN |= (UART_PWREN_KEY_UNLOCK_W | UART_PWREN_ENABLE_ENABLE);
    }
		
		//Rx (Receive Pin)
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM22] |=  IOMUX_PINCM22_PF_UART0_RX;  	//Setting "Peripheral Function Selection" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM22] |= IOMUX_PINCM_PC_CONNECTED;			//Setting "Peripheral is connected" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM22] |= IOMUX_PINCM_INENA_ENABLE;			//Enabling Rx pin as an input
		
		//Tx (Transmit Pin)
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM21] |=  IOMUX_PINCM21_PF_UART0_TX;		//Setting "Peripheral Function selection" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM21] |= IOMUX_PINCM_PC_CONNECTED	;		//Setting "Peripheral is connected" bit
		
		
		
		UART0 -> CTL0 &= ~UART_CTL0_ENABLE_ENABLE;//clearing ENABLE bit in CTL0 register to disable UART
		
		UART0 -> CLKSEL |= UART_CLKSEL_BUSCLK_SEL_ENABLE;			//ENABLING busclk
		
		UART0 -> CLKDIV  |= UART_CLKDIV_RATIO_DIV_BY_1;	//setting clock division ratio
		
		UART0 -> CTL0 &= ~UART_CTL0_HSE_MASK;//oversampling rate
		
		UART0 -> CTL0 |= (UART_CTL0_TXE_ENABLE | UART_CTL0_RXE_ENABLE|UART_CTL0_FEN_ENABLE);
		
		UART0 -> IBRD =  0x00D0; //setting integer part of Baud Rate 
		UART0 -> FBRD =  0x15; //setting fractional part of Baud Rate
		
		UART0 -> LCRH |= UART_LCRH_WLEN_DATABIT8 ; // Setting UART to use 8 Data Bits, 1 stop bit, and no Parity bit
		
		UART0 -> LCRH &= ~UART_LCRH_STP2_ENABLE;
		
		UART0 -> LCRH &= ~UART_LCRH_PEN_MASK;
		
		UART0 -> CTL0 |= UART_CTL0_ENABLE_ENABLE; // enabling UART
}	

/**
 * @brief Initialize UART1
*/
void UART1_init(void){
		if ((UART1 -> GPRCM.PWREN & UART_PWREN_ENABLE_MASK) != UART_PWREN_ENABLE_ENABLE) {
			
        //If not powered, reset the peripheral first
        UART1->GPRCM.RSTCTL |= (UART_RSTCTL_KEY_UNLOCK_W |UART_RSTCTL_RESETASSERT_ASSERT);
        
        // Enable power to the peripheral
        UART1->GPRCM.PWREN |= (UART_PWREN_KEY_UNLOCK_W | UART_PWREN_ENABLE_ENABLE);
    }
		
		//Rx (Receive Pin)
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM20] |=  IOMUX_PINCM20_PF_UART1_RX;  	//Setting "Peripheral Function Selection" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM20] |= IOMUX_PINCM_PC_CONNECTED;			//Setting "Peripheral is connected" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM20] |= IOMUX_PINCM_INENA_ENABLE;			//Enabling Rx pin as an input
		
		//Tx (Transmit Pin)
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM19] |=  IOMUX_PINCM19_PF_UART1_TX;		//Setting "Peripheral Function selection" bit
		IOMUX -> SECCFG.PINCM[IOMUX_PINCM19] |= IOMUX_PINCM_PC_CONNECTED	;		//Setting "Peripheral is connected" bit
		
		
		
		UART1-> CTL0 &= ~UART_CTL0_ENABLE_ENABLE;//clearing ENABLE bit in CTL0 register to disable UART
		
		UART1 -> CLKSEL |= UART_CLKSEL_BUSCLK_SEL_ENABLE;			//ENABLING busclk
		
		UART1 -> CLKDIV  |= UART_CLKDIV_RATIO_DIV_BY_1;	//setting clock division ratio
		
		UART1 -> CTL0 &= ~UART_CTL0_HSE_MASK;//oversampling rate
		
		UART1 -> CTL0 |= (UART_CTL0_TXE_ENABLE | UART_CTL0_RXE_ENABLE|UART_CTL0_FEN_ENABLE);
		
		UART1 -> IBRD =  0x00D0; //setting integer part of Baud Rate 
		UART1 -> FBRD =  0x15; //setting fractional part of Baud Rate
		
		UART1 -> LCRH |= UART_LCRH_WLEN_DATABIT8 ; // Setting UART to use 8 Data Bits, 1 stop bit, and no Parity bit
		
		UART1 -> LCRH &= ~UART_LCRH_STP2_ENABLE;
		
		UART1 -> LCRH &= ~UART_LCRH_PEN_MASK;
		
		UART1 -> CTL0 |= UART_CTL0_ENABLE_ENABLE; // enabling UART
}	


/**
 * @brief Put a character over UART0
 * @param[in] ch - Character to print
*/
void UART0_putchar(char ch){
	//polling until transmit register is not full
	UART0 -> TXDATA &= 0xFFFFFF00;
	
	while ((UART0 -> STAT & UART_STAT_TXFF_MASK) == UART_STAT_TXFF_SET ){
					
				}
	
	UART0 -> TXDATA = ch;

}

/**
 * @brief Put a character over UART0
 * @param[in] ch - Character to print
*/
void UART1_putchar(char ch){
	//polling until transmit register is not full
	UART1 -> TXDATA &= 0xFFFFFF00;
	
	while ((UART1 -> STAT & UART_STAT_TXFF_MASK) == UART_STAT_TXFF_SET ){
					
				}
	
	UART1 -> TXDATA = ch;

}

/**
 * @brief Retrieve a single character from UART0
*/
char UART0_getchar(void){
	//polling until the receive register is not empty and there is info to collect
	while((UART0 -> STAT & UART_STAT_RXFE_MASK) == UART_STAT_RXFE_SET){
		
	}
	
	return (UART0 -> RXDATA & UART_RXDATA_DATA_MASK);
	
}

/**
 * @brief Retrieve a single character from UART1
*/
char UART1_getchar(void){
	//polling until the receive register is not empty and there is info to collect
	while((UART1 -> STAT & UART_STAT_RXFE_MASK) == UART_STAT_RXFE_SET){
		
	}
	
	return (UART1 -> RXDATA & UART_RXDATA_DATA_MASK);
	
}


/**
 * @brief Send a full character string over UART0
* @param[in] ptr_str - Pointer to the string to print
*/
void UART0_put(char *ptr_str){
	size_t size = strlen(ptr_str); //getting size of string
	for (size_t i = 0 ; i < size; i ++){
			UART0_putchar(ptr_str[i]);
	}
}

/**
 * @brief Send a full character string over UART0
 * @param[in] ptr_str - Pointer to the string to print
*/
void UART1_put(char *ptr_str){
	size_t size = strlen(ptr_str); //getting size of string
	for (size_t i = 0 ; i < size; i ++){
			UART1_putchar(ptr_str[i]);
	}
}

int uart0_dataAvailable()
	{
			if ((UART0->STAT & UART_STAT_RXFE_MASK) != UART_STAT_RXFE_SET) {
				
        return 1;  // Data is waiting in the buffer
				
			}
		
			return 0;
	
	
	}
	
int uart1_dataAvailable()
	{
		
			if ((UART1->STAT & UART_STAT_RXFE_MASK) != UART_STAT_RXFE_SET) {
				
        return 1;  // Data is waiting in the buffer
				
			}
		
			return 0;
	
	
	}	



