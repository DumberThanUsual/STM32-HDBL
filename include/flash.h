#ifndef __FLASH_H_
#define __FLASH_H_

#include <stdint.h>

#define APPLICATION_ADDRESS     (uint32_t)0x08008000

typedef enum
{
    F_OK,
    F_BUSY,
    F_ERROR,
    F_UNSUPPORTED
} flash_status;

flash_status flash_unlock();
flash_status flash_lock();
flash_status flash_write(uint32_t, uint64_t);
flash_status flash_write_batch(uint32_t, uint32_t *, uint32_t);
flash_status flash_erase();

#endif