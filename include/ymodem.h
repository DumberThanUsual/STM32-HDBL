#ifndef __YMODEM_H_
#define __YMODEM_H_

#include <stdint.h>

#define PACKET_DATA_SIZE_128    ((uint16_t)128)
#define PACKET_DATA_SIZE_1024   ((uint16_t)1024)

#define PACKET_HEADER_SIZE      ((uint16_t)3)
#define PACKET_FOOTER_SIZE      ((uint16_t)2)

#define PACKET_START_INDEX      ((uint16_t)0)
#define PACKET_NUMBER_INDEX     ((uint16_t)1)
#define PACKET_XNUM_INDEX       ((uint16_t)2)
#define PACKET_DATA_INDEX       ((uint16_t)3)

#define PACKET_PART_TIMEOUT     ((uint32_t)100)
#define ENTRY_TIMEOUT           ((uint32_t)100)

#define SOH                     ((uint8_t)0x01)  // 128-byte data packet 
#define STX                     ((uint8_t)0x02)  // 1024-byte data packet 
#define EOT                     ((uint8_t)0x04)  // end of transmission 
#define ACK                     ((uint8_t)0x06)  // acknowledge 
#define NAK                     ((uint8_t)0x15)  // negative acknowledge 
#define CAN                     ((uint32_t)0x18) // cancel transfer
#define CRC16                   ((uint8_t)0x43)  // request 16-bit CRC 
#define NEGATIVE_BYTE           ((uint8_t)0xFF)

typedef enum 
{
    Y_OK = 0,
    Y_ACCEPT,
    Y_ERROR,
    Y_REPEATED,
    Y_ABORTED,
    Y_ABORT,
    Y_TIMEOUT,
    Y_COMPLETE,
    Y_FINISH
} ymodem_status_t;

struct ymodem_packet
{
    uint16_t size;
    uint8_t start;
    uint8_t num;
    uint8_t xnum;
    __attribute__((aligned(4))) uint8_t data[PACKET_DATA_SIZE_1024];
    uint8_t crc0;
    uint8_t crc1;
};

struct ymodem_state
{
    ymodem_status_t status;
    struct ymodem_packet packet;
    uint8_t packets_received;
    uint32_t flash_addr;
};

ymodem_status_t ymodem();

#endif