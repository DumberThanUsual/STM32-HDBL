#include "main.h"

#include "stm32g0xx.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "uart.h"
#include "ymodem.h"
#include "flash.h"
#include "systick.h"

int main()
{
    pFunction user_program;
    uint32_t user_prog_address;
    
    systick_init();
    uart_init();
    
    switch(ymodem())
    {
        case Y_FINISH:
            uart_tx_string((uint8_t *)"\r\nYMODEM complete\r\n");
            break;
        case Y_TIMEOUT:
            uart_tx_string((uint8_t *)"\r\nNo response to YMODEM initiation\r\n");
            break;
        case Y_ABORT:
            uart_tx_string((uint8_t *)"\r\nYMODEM error\r\n");
            break;
        default:
            break;
    }

    uart_tx_string((uint8_t *)"Booting from 0x8008000...\r\n");

    uart_deinit();
    systick_deinit();

    user_prog_address = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4); // Load program address
    user_program = (pFunction) user_prog_address;
    __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS); // Set stack pointer
    SCB->VTOR = APPLICATION_ADDRESS; //Set Vector Table Offset Register
    user_program(); // Jump to program
}