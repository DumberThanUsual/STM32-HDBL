#include "crc.h"

#include "stm32g0xx.h"

#include <stdint.h>

#include "uart.h"
#include "systick.h"

uint16_t crc16(uint8_t *ptr, uint16_t count)
{
    uint16_t crc = 0;

    while (count-- > 0)
    {
        crc = crc ^ ((uint16_t) (*ptr++ << 8));
        for(uint8_t i = 0; i < 8; i++)
        {
            if(crc & 0x8000)
            {
                crc = (crc << 1) ^ (1 << 12 | 1 << 5 | 1);
            }
            else
            {
                crc = crc << 1;
            }
        }
    }

    return crc;
}