#include "flash.h"

#include "stm32g0xx.h"

#include <stdint.h>

#include "main.h"

flash_status flash_unlock()
{
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
    return F_OK;
}

flash_status flash_lock()
{
    if (FLASH->SR & FLASH_SR_BSY1)
    {
        return F_BUSY;
    }
    FLASH->CR |= FLASH_CR_LOCK;
    return F_OK;
}

flash_status flash_write(uint32_t address, uint64_t data)
{
    if (FLASH->SR & FLASH_SR_BSY1)
    {
        return F_BUSY;
    }
    FLASH->SR &= ~(FLASH_SR_PROGERR | FLASH_SR_SIZERR | FLASH_SR_PGAERR | FLASH_SR_PGSERR | FLASH_SR_MISERR_Msk | FLASH_SR_WRPERR);
    if (FLASH->SR & FLASH_SR_CFGBSY)
    {
        return F_BUSY;
    }
    FLASH->CR |= FLASH_CR_PG | FLASH_CR_EOPIE;

    ///TODO: Check if in programming address range

    *(uint32_t *)address = (uint32_t)data;
    __ISB();
    *(uint32_t *)(address + 4U) = (uint32_t)(data >> 32U);

    while (FLASH->SR & FLASH_SR_CFGBSY);
    if (!(FLASH->SR & FLASH_SR_EOP))
    {
        return F_ERROR;
    }
    FLASH->SR |= FLASH_SR_EOP;
    FLASH->CR &= ~FLASH_CR_PG;
    return F_OK;
}


//length: size of data in 64-bit double words
flash_status flash_write_batch(uint32_t address, uint32_t *data, uint32_t length)
{
    uint32_t i = 0;

    if (FLASH->SR & FLASH_SR_BSY1)
    {
        return F_BUSY;
    }
    FLASH->SR &= ~(FLASH_SR_PROGERR | FLASH_SR_SIZERR | FLASH_SR_PGAERR | FLASH_SR_PGSERR | FLASH_SR_MISERR_Msk | FLASH_SR_WRPERR);
    if (FLASH->SR & FLASH_SR_CFGBSY)
    {
        return F_BUSY;
    }
    FLASH->CR |= FLASH_CR_PG | FLASH_CR_EOPIE;

    for (i = 0; i < length; i += 2)
    {
        *(uint32_t *)address = (uint32_t)data[i];
        __ISB();
        *(uint32_t *)(address + 4U) = (uint32_t)(data[i + 1]);

        while (FLASH->SR & FLASH_SR_CFGBSY);
        if (!(FLASH->SR & FLASH_SR_EOP))
        {
            FLASH->CR &= ~FLASH_CR_PG;
            return F_ERROR;
        }
        FLASH->SR |= FLASH_SR_EOP;
        address += 8U;
    }
    
    FLASH->CR &= ~FLASH_CR_PG;
    return F_OK;
}

flash_status flash_erase_page(uint8_t page)
{
    if (FLASH->SR & FLASH_SR_BSY1)
    {
        return F_BUSY;
    }
    FLASH->SR &= ~(FLASH_SR_PROGERR | FLASH_SR_SIZERR | FLASH_SR_PGAERR | FLASH_SR_PGSERR | FLASH_SR_MISERR_Msk | FLASH_SR_WRPERR);
    if (FLASH->SR & FLASH_SR_CFGBSY)
    {
        return F_BUSY;
    }
    FLASH->CR |= ((page << FLASH_CR_PNB_Pos) & FLASH_CR_PNB_Msk) | FLASH_CR_PER;
    FLASH->CR |= FLASH_CR_STRT;

    while (FLASH->SR & FLASH_SR_CFGBSY);
    FLASH->CR = 0;
    return F_OK;
}

flash_status flash_erase()
{
    uint8_t ret = F_OK;
    for(uint8_t i = 16; i < 63 && ret == F_OK; i ++) 
    {
        ret = flash_erase_page(i);
    }

    return ret;
}