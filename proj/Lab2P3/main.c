/**
 * ******************************************************************************
 * @file    : main.c
 * @brief   : 
 * @details : Implementation of LED control over UART
 * 
 * @author : Melie Momah
 * @date : 9/9/2026
 * ******************************************************************************
*/
#include "uart.h"
#include "leds.h"
#include "peripherals/hw_gpio.h"
#include "peripherals/hw_iomux.h"
#include "m0p/mspm0g350x.h"
#include <string.h>
#include <stdio.h>

int main(void){
	
	UART0_init();
	UART1_init();
	LED2_init();

  UART1_put("\r\nEnter a sentence or LED command (0-3): ");
	//control loop
	while (1) {
				UART1_put("\r\nEnter a sentence: ");
		
				char c = UART1_getchar();	//getting character from phone over bluetooth
				
				if (c == '0'){
					LED2_set(OFF);
					UART1_put("OFF\r\n");
				}
				else if (c == '1'){
					LED2_set(OFF);
					LED2_set(RED);	//turn LED2 red
					UART1_put("RED\r\n");
				}
				else if (c == '2'){
					LED2_set(OFF);
					LED2_set(BLUE);  //turn LED2 blue
					UART1_put("BLUE\r\n");
				}
				else if (c == '3'){
					LED2_set(OFF);
					LED2_set(GREEN); //turn LED3 green
					UART1_put("GREEN\r\n");
				}
				else{
					UART1_put("Invalid Input\r\n"); //invalid input
					}
    }
	
		
		
	
		
		
		
		

	
	
	
}	