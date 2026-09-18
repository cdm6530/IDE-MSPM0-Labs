/**
 * ******************************************************************************
 * @file    : main.c
 * @brief   : 
 * @details : Implementation of chatroom over UART
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
#include <stdio.h>

int main(void){

    UART0_init();
    UART1_init();

    UART0_put("Chatroom has booted up \r\n");
    UART1_put("Chatroom has booted up \r\n");

    char inputb[80]; // PC buffer
    char inputc[80]; // Phone buffer

    UART0_put("PC> ");
    UART1_put("Phone> ");

    int buffsize = 0;
    int buffsize1 = 0;  

	
		//control  loop
    while (1) {
        if (uart0_dataAvailable()) {
            char ch1 = UART0_getchar();
						//checking for new line character
            if (ch1 == '\n' || ch1 == '\r') {
                inputb[buffsize] = '\0';
								
								//clearing what phone has entered so far with carriage returns
                for (int i = 0; i < buffsize1; i++) {
                    UART1_putchar(0x7F);
                    UART1_putchar(' ');
                    UART1_putchar(0x7F);
                }

                UART0_put("\r\nPC> ");

                UART1_put("\r\nPC> ");
                UART1_put(inputb);	//showing PCs message on phone.
								
                inputc[buffsize1] = '\0';
                buffsize = 0;
            }
						//checking for backspace
            else if (ch1 == '\b' || ch1 == 0x7F) {
                if (buffsize > 0) {
                    UART0_putchar('\b');
                    UART0_putchar(' ');
                    UART0_putchar('\b');
                    buffsize--;
                }
            }
            else {
							//non-special character
                if (buffsize < 80) {
                    inputb[buffsize++] = ch1;
                    UART0_putchar(ch1);
                }
            }
        }
        if (uart1_dataAvailable()) {
            char ch2 = UART1_getchar();

            if (ch2 == '\n' || ch2 == '\r') { 
                inputc[buffsize1] = '\0';

                // Clear whatever the PC had typed so far
                for (int i = 0; i < buffsize; i++) {
                    UART0_putchar(0x7F);
                    UART0_putchar(' ');
                    UART0_putchar(0x7F);
                }


                UART1_put("\r\nPhone> ");
								UART1_put(inputc);

                UART0_put("\r\nPhone> ");
                
								
                inputb[buffsize] = '\0';
                UART0_put(inputb);  //displaying phones message on Putty
                buffsize1 = 0;
            }
            else if (ch2 == '\b' || ch2 == 0x7F) {
							//checking for backspace
                if (buffsize1 > 0) {
                    UART1_putchar('\b');
                    UART1_putchar(' ');
                    UART1_putchar('\b');
                    buffsize1--;
                }
            }
            else {
							//non special character
                if (buffsize1 < 80) {
                    inputc[buffsize1++] = ch2;
                    UART1_putchar(ch2);
                }
            }
        }
    }
}