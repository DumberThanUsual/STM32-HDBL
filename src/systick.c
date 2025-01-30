#include "systick.h"

#include "stm32g071xx.h"

#include <stdint.h>

volatile uint32_t systick_ms = 0;

void systick_init()
{
    SysTick_Config(SYSCLK/1000);
}

void systick_deinit()
{
    SysTick->CTRL  = 0;
}

void  SysTick_Handler(void)
{
    systick_ms ++;
}

void delay(uint32_t ms)
{
    uint32_t start_tick = systick_ms;
    while (start_tick + ms > systick_ms);
}