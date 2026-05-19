#include "SBUS.h"

#include <stdint.h>
#include <stdbool.h>
#include "hardware/uart.h"
#include "hardware/gpio.h"

#include "src/RC/RC.h"

#define SBUS_BAUD       100000
#define SBUS_DATA_BITS  8
#define SBUS_STOP_BITS  2
#define SBUS_PARITY     UART_PARITY_EVEN

#define SBUS_HEADER     0x0F
#define SBUS_FOOTER     0x00
#define SBUS_PAYLOAD_SIZE 23


// SBUS Protocol
// 25 Byte frames
// [0] SBUS Header 0x0F
// [22:1] 16 servo channels with 11 bits per channel
// [23][7] digital channel 17
// [23][6] digital channel 18
// [23][5] frame lost (0x20)
// [23][4] failsafe active (0x10)
// [23][3:0] unused
// [24] SBUS end Byte

static uart_inst_t *_uart_instance;
static bool _failsafe = true;


void SBUS_InitBus(uart_inst_t *uart, uint32_t rx)
{
    _uart_instance = uart;

    gpio_set_function(rx, UART_FUNCSEL_NUM(_uart_instance, rx));
    gpio_set_inover(rx, GPIO_OVERRIDE_INVERT);

    uart_init(_uart_instance, SBUS_BAUD);
    uart_set_format(_uart_instance, SBUS_DATA_BITS, SBUS_STOP_BITS, SBUS_PARITY);
}


// MIGHT BE USEFUL TO MOVE THE WHILE LOOP TO THE OUTSIDE OF THIS FUNCTION SO THE 
// CALLER HAS CONTROL OVER THE PARSING TIMEFRAME.
static bool parse(uint8_t **ret_ptr)
{
    typedef enum {
        IDLE,
        READ_CHANNEL_DATA,
        READ_FOOTER,
    } parser_state_machine_e;
    static parser_state_machine_e state = IDLE;
    static uint32_t packet_byte_count;
    static uint8_t buffer[SBUS_PAYLOAD_SIZE] = {0};
    bool packet_complete = false;

    *ret_ptr = buffer;

    while (uart_is_readable(_uart_instance) && !packet_complete) {
        uint8_t data = uart_getc(_uart_instance);

        switch (state) {
            default:
            case IDLE:
                packet_complete = false;

                if (data == SBUS_HEADER) {
                    state = READ_CHANNEL_DATA;
                    packet_byte_count = 0;
                }
                break;

            case READ_CHANNEL_DATA:
                buffer[packet_byte_count++] = data;

                if (packet_byte_count >= SBUS_PAYLOAD_SIZE)
                    state = READ_FOOTER;
                break;

            case READ_FOOTER:
                if (data == SBUS_FOOTER) {
                    state = IDLE;
                    packet_complete = true;
                }
                break;
        }
    }

    return packet_complete;
}


typedef uint16_t SBUS_raw_t[19];

static bool GetRaw(SBUS_raw_t raw_ch)
{
    uint8_t *packet;

    if (parse(&packet)) {
        raw_ch[1] = (((uint16_t)packet[1] << 8) | ((uint16_t)packet[0])) & 0x07FF;
        raw_ch[2] = (((uint16_t)packet[2] << 5) | ((uint16_t)packet[1] >> 3)) & 0x07FF;
        raw_ch[3] = (((uint16_t)packet[4] << 10) | ((uint16_t)packet[3] << 2) | ((uint16_t)packet[2] >> 6)) & 0x07FF;
        raw_ch[4] = (((uint16_t)packet[5] << 7) | ((uint16_t)packet[4] >> 1)) & 0x07FF;
        raw_ch[5] = (((uint16_t)packet[6] << 4) | ((uint16_t)packet[5] >> 4)) & 0x07FF;
        raw_ch[6] = (((uint16_t)packet[8] << 9) | ((uint16_t)packet[7] << 1) | ((uint16_t)packet[6] >> 7)) & 0x07FF;
        raw_ch[7] = (((uint16_t)packet[9] << 6) | ((uint16_t)packet[8] >> 2)) & 0x07FF;
        raw_ch[8] = (((uint16_t)packet[10] << 3) | ((uint16_t)packet[9] >> 5)) & 0x07FF;

        raw_ch[9] = (((uint16_t)packet[12] << 8) | ((uint16_t)packet[11])) & 0x07FF;
        raw_ch[10] = (((uint16_t)packet[13] << 5) | ((uint16_t)packet[12] >> 3)) & 0x07FF;
        raw_ch[11] = (((uint16_t)packet[15] << 10) | ((uint16_t)packet[14] << 2) | ((uint16_t)packet[13] >> 6)) & 0x07FF;
        raw_ch[12] = (((uint16_t)packet[16] << 7) | ((uint16_t)packet[15] >> 1)) & 0x07FF;
        raw_ch[13] = (((uint16_t)packet[17] << 4) | ((uint16_t)packet[16] >> 4)) & 0x07FF;
        raw_ch[14] = (((uint16_t)packet[19] << 9) | ((uint16_t)packet[18] << 1) | ((uint16_t)packet[17] >> 7)) & 0x07FF;
        raw_ch[15] = (((uint16_t)packet[20] << 6) | ((uint16_t)packet[19] >> 2)) & 0x07FF;
        raw_ch[16] = (((uint16_t)packet[21] << 3) | ((uint16_t)packet[20] >> 5)) & 0x07FF;

        raw_ch[17] = (bool)(packet[22] & (0x1 << 7));
        raw_ch[18] = (bool)(packet[22] & (0x1 << 6));

        _failsafe = packet[22] & (0x1 << 4);

        return true;
    }

    return false;
}

bool SBUS_GetChannels(RC_t rc)
{
    SBUS_raw_t raw_ch = {0};

    if (GetRaw(raw_ch)) {
        for (uint32_t i = 0; i <= 16; ++i) {
            rc[i] = raw_ch[i] / 2048.0f; // normalize values to [0, 1]
        }

        rc[17] = (raw_ch[17]) ? 1.0f : 0.0f;
        rc[18] = (raw_ch[18]) ? 1.0f : 0.0f;

        return true;
    }

    return false;
}


bool SBUS_IsFailsafe(void)
{
    return _failsafe;
}
