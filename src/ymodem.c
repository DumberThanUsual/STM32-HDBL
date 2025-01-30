#include "ymodem.h"

#include "stm32g071xx.h"

#include <stdint.h>

#include "crc.h"
#include "uart.h"
#include "flash.h"
#include "main.h"
#include "systick.h"

uint8_t num_header_packets = 0;

void receive_packet_start(struct ymodem_state *state)
{
    struct ymodem_packet *packet = &(state->packet);

    uart_ret_t uart_ret;

    if (state->packets_received == 0)
    {
        uart_ret = uart_rx_byte(&(packet->start), ENTRY_TIMEOUT);
    }
    else
    {
        uart_ret = uart_rx_byte(&(packet->start), PACKET_PART_TIMEOUT);
    }

    switch (uart_ret)
    {
    case U_TIMEOUT:
        state->status = Y_TIMEOUT;
        return;
    case U_ERROR:
        state->status = Y_ERROR;
        return;
    default:
        break;
    }

    switch(packet->start)
    {
    case SOH:
        packet->size = PACKET_DATA_SIZE_128;
        break;
    case STX:
        packet->size = PACKET_DATA_SIZE_1024;
        break;
    case EOT:
        state->status = Y_COMPLETE;
        return;
    case CAN:
        state->status = Y_ABORTED;
        return;
    default:
        break;
    }
}

void receive_packet_body(struct ymodem_state *state)
{
    struct ymodem_packet *packet = &(state->packet);
    uart_ret_t uart_ret;

    uart_ret = uart_rx_byte(&packet->num, PACKET_PART_TIMEOUT);
    if (uart_ret != U_OK) {
        state->status = Y_ERROR;
        return;
    }

    uart_ret = uart_rx_byte(&packet->xnum, PACKET_PART_TIMEOUT);
    if (uart_ret != U_OK) {
        state->status = Y_ERROR;
        return;
    }

    uart_ret = uart_rx_string(packet->data, packet->size, PACKET_PART_TIMEOUT);
    if (uart_ret != U_OK) {
        state->status = Y_ERROR;
        return;
    }

    uart_ret = uart_rx_byte(&packet->crc0, PACKET_PART_TIMEOUT);
    if (uart_ret != U_OK) {
        state->status = Y_ERROR;
        return;
    }

    uart_ret = uart_rx_byte(&packet->crc1, PACKET_PART_TIMEOUT);
    if (uart_ret != U_OK) {
        state->status = Y_ERROR;
        return;
    }
}

void check_packet_cmp(struct ymodem_state *state)
{
    struct ymodem_packet *packet = &(state->packet);

    if (packet->num != (255 - packet->xnum))
        state->status = Y_ERROR;
}

void check_packet_crc(struct ymodem_state *state)
{
    struct ymodem_packet *packet = &(state->packet);

    uint16_t crc;

    crc = crc16(packet->data, packet->size);
    if (crc != (packet->crc1 | packet->crc0 << 8))
        state->status = Y_ERROR;
}

void check_packet_num(struct ymodem_state *state)
{
    struct ymodem_packet *packet = &(state->packet);

    if (packet->num != state->packets_received)
    {
        if (packet->num == state->packets_received - 1)
        {
            state->status = Y_REPEATED;
        } else
        {
            state->status = Y_ABORT;
        }
    }
}

void receive_packet(struct ymodem_state *state)
{
    receive_packet_start(state);
    if (state->status != Y_OK)
        return;
    
    receive_packet_body(state);
    if (state->status != Y_OK)
        return;

    check_packet_cmp(state);
    if (state->status != Y_OK)
        return;

    check_packet_crc(state);
    if (state->status != Y_OK)
        return;
}

void process_header_packet(struct ymodem_state *state)
{
    struct ymodem_packet *packet = &(state->packet);
    num_header_packets ++;
    
    if (packet->data[0] == 0) {
        state->status = Y_FINISH;
        return;
    }

    flash_erase();
    state->status = Y_ACCEPT;
}

void process_data_packet(struct ymodem_state *state)
{
    struct ymodem_packet *packet = &(state->packet);

    uint32_t ramsource = (uint32_t)&packet->data[0];

    if (flash_write_batch(state->flash_addr, (uint32_t *)ramsource, packet->size/4) == F_OK)
    {
        state->flash_addr += packet->size;
        state->status = Y_OK;
    }
    else
    {
        state->status = Y_ABORT;
    }
}

void process_packet(struct ymodem_state *state)
{
    if (state->status != Y_OK)
        return;

    if (state->packets_received == 0)
        process_header_packet(state);
    else
        process_data_packet(state);
}

void respond_packet(struct ymodem_state *state)
{
    switch (state->status)
    {
    case Y_OK:
        state->packets_received ++;
        uart_tx_byte(ACK);
        return;
    case Y_ACCEPT:
        state->packets_received ++;
        uart_tx_byte(ACK);
        uart_tx_byte(CRC16);
        state->status = Y_OK;
        return;
    case Y_ERROR:
        uart_tx_byte(NAK);
        state->status = Y_OK;
        return;
    case Y_ABORTED:
        state->packets_received ++;
        uart_tx_byte(ACK);
        return;
    case Y_ABORT:
        state->packets_received ++;
        uart_tx_byte(CAN);
        uart_tx_byte(CAN);
        return;
    case Y_TIMEOUT:
        return;
    case Y_REPEATED:
        uart_tx_byte(ACK);
        state->status = Y_OK;
    case Y_COMPLETE:
        uart_tx_byte(ACK);
        uart_tx_byte(CRC16);
        state->packets_received = 0;
        state->status = Y_OK;
        return;
    case Y_FINISH:
        state->packets_received ++;
        uart_tx_byte(ACK);
        return;
    }
}

ymodem_status_t ymodem()
{
    struct ymodem_state state = {0};
    state.flash_addr = APPLICATION_ADDRESS;

    flash_unlock();

    uart_tx_byte(CRC16);

    while (state.status == Y_OK) 
    {
        receive_packet(&state);
        process_packet(&state);
        respond_packet(&state);
    }

    flash_lock();

    return state.status;
}