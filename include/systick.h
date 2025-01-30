#ifndef __SYSTICK_H_
#define __SYSTICK_H_

#include <stdint.h>

#define SYSCLK 16000000

volatile uint32_t systick_ms;

void systick_init();
void systick_deinit();
void SysTick_Handler(void);
void delay(uint32_t ms);

#endif