/**
 * ******************************************************************************
 * @file    : main.c
 * @brief   : 
 * @details : Implementation of UART Echo
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

int main(void){
	
	UART0_init(); 
	
	char buffer[10 + 1];
	
	while (1) {
        UART0_put("\r\nEnter a sentence: ");

        int count = 0;
        while (count < 10) {
            char c = UART0_getchar();

            // Check for Enter key (Carriage Return or Line Feed)
            if (c == '\r' || c == '\n') {
                break;
            }

            // Echo character back to terminal live
            UART0_putchar(c);

            // Store character in buffer
            buffer[count++] = c;
        }

        // Null-terminate the string
        buffer[count] = '\0';

        // Print message on a new line
        UART0_put("\r\nYou entered: ");
        UART0_put(buffer);
    }

	
}