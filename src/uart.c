#include "uart.h"

#include "stm32g0xx.h"

#include <stdint.h>

#include "systick.h"

void uart_init() 
{
    #if NUCLEO

    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    GPIOA->MODER &= ~GPIO_MODER_MODE2_Msk;
    GPIOA->MODER |= GPIO_MODER_MODE2_1;
    GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_0;

    GPIOA->MODER &= ~GPIO_MODER_MODE3_Msk;
    GPIOA->MODER |= GPIO_MODER_MODE3_1;
    GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3_0;

    RCC->APBENR1 |= RCC_APBENR1_USART2EN;
    COMPORT->BRR |= 139;
    COMPORT->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    #else
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

    GPIOB->MODER &= ~GPIO_MODER_MODE6_Msk;
    GPIOB->MODER |= GPIO_MODER_MODE6_1;
    GPIOB->OTYPER |= GPIO_OTYPER_OT6;

    RCC->APBENR2 |= RCC_APBENR2_USART1EN;
    
    COMPORT->CR3 |= USART_CR3_HDSEL;
    COMPORT->BRR |= 139;
    COMPORT->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    #endif
}

void uart_deinit()
{
    #if NUCLEO

    GPIOB->MODER &= ~GPIO_MODER_MODE7_Msk;
    GPIOB->MODER &= ~GPIO_MODER_MODE6_Msk;
    RCC->IOPENR &= ~RCC_IOPENR_GPIOBEN;

    COMPORT->CR1 = 0;
    COMPORT->BRR = 0;
    RCC->APBENR1 &= ~RCC_APBENR1_USART2EN;

    #else
    GPIOB->MODER &= ~GPIO_MODER_MODE6_Msk;
    RCC->IOPENR &= ~RCC_IOPENR_GPIOBEN;

    COMPORT->CR1 = 0;
    COMPORT->CR3 = 0;
    COMPORT->BRR = 0;
    RCC->APBENR2 &= ~RCC_APBENR2_USART1EN;
    #endif
}

void uart_tx_byte(uint8_t byte)
{
    COMPORT->CR1 &= ~USART_CR1_RE;
    while(!(COMPORT->ISR & USART_ISR_TXE_TXFNF));
    COMPORT->TDR = byte;
    while (!(COMPORT->ISR & USART_ISR_TC));
    COMPORT->CR1 |= USART_CR1_RE;
}

void uart_tx_string(uint8_t *string)
{
    COMPORT->CR1 &= ~USART_CR1_RE;
    uint16_t length = 0;
    while (string[length] != '\0')
    {
        while(!(COMPORT->ISR & USART_ISR_TXE_TXFNF));
        COMPORT->TDR = string[length];
        length++;
    }
    while (!(COMPORT->ISR & USART_ISR_TC));
    COMPORT->CR1 |= USART_CR1_RE;
}

uart_ret_t uart_rx_byte(uint8_t *byte, uint32_t timeout)
{
    uint8_t status;
    status = uart_rx_wait_available(timeout);
    if (status != U_OK)
    {
        return status;
    }
    *byte = (uint8_t)COMPORT->RDR; 
    return U_OK;
}

uart_ret_t uart_rx_string(uint8_t *string, uint32_t length, uint32_t timeout)
{
    uint32_t index = 0;
    uint8_t status;
    while(index < length)
    {   
        status = uart_rx_byte(&string[index], timeout);
        if (status == U_OK)
        {
            index ++;
        }
        else
        {
            return status;
        }
    }
    return U_OK;
}

uart_ret_t uart_rx_wait_available(uint32_t timeout)
{
    uint32_t start_tick = systick_ms;
    while(!( COMPORT->ISR & USART_ISR_RXNE_RXFNE ))
    {
        if ((timeout > 0) && (systick_ms - start_tick > timeout))
        {
            return U_TIMEOUT;
        }
    }
    return U_OK;
}