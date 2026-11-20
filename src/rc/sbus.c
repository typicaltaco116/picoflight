// Primary Header
#include <picoflight/rc/sbus.h>

// Local Headers
#include <picoflight/rc/rc.h>
#include <picoflight/core/utilities.h>

// Pico-SDK Headers
#include <hardware/uart.h>
#include <hardware/gpio.h>

// Standard Headers
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SBUS_BAUD       100000
#define SBUS_DATA_BITS  8
#define SBUS_STOP_BITS  2
#define SBUS_PARITY     UART_PARITY_EVEN

#define SBUS_HEADER     0x0F
#define SBUS_FOOTER     0x00
#define SBUS_PAYLOAD_SIZE 23

#define SBUS_FRAME_CHAN17   0x1
#define SBUS_FRAME_CHAN18   0x2
#define SBUS_FRAME_LOST     0x4
#define SBUS_FRAME_FAILSAFE 0x8

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
typedef struct {
    unsigned chan1  : 11;
    unsigned chan2  : 11;
    unsigned chan3  : 11;
    unsigned chan4  : 11;
    unsigned chan5  : 11;
    unsigned chan6  : 11;
    unsigned chan7  : 11;
    unsigned chan8  : 11;
    unsigned chan9  : 11;
    unsigned chan10 : 11;
    unsigned chan11 : 11;
    unsigned chan12 : 11;
    unsigned chan13 : 11;
    unsigned chan14 : 11;
    unsigned chan15 : 11;
    unsigned chan16 : 11;
    uint8_t flags;
} __attribute__((__packed__)) sbusChannels_t;

static uart_inst_t *_uartInst;
static bool _failsafe = true;
static uint8_t _buffer[64];
static uint32_t _packetByteCount = 0;


void sbusInitBus(uart_inst_t *uart, uint32_t rx)
{
    _uartInst = uart;

    gpio_set_function(rx, UART_FUNCSEL_NUM(_uart_instance, rx));
    gpio_set_inover(rx, GPIO_OVERRIDE_INVERT);

    uart_init(_uartInst, SBUS_BAUD);
    uart_set_format(_uartInst, SBUS_DATA_BITS, SBUS_STOP_BITS, SBUS_PARITY);
}


static bool parse(void)
{
    typedef enum {
        IDLE,
        READ_CHANNEL_DATA,
        READ_FOOTER,
    } parser_state_machine_e;
    static parser_state_machine_e state = IDLE;
    bool packet_complete = false;

    while (uart_is_readable(_uartInst) && !packet_complete) {
        uint8_t data = uart_getc(_uartInst);

        switch (state) {
            default:
            case IDLE:
                packet_complete = false;

                if (data == SBUS_HEADER) {
                    state = READ_CHANNEL_DATA;
                    _packetByteCount = 0;
                }
                break;

            case READ_CHANNEL_DATA:
                _buffer[_packetByteCount++] = data;

                if (_packetByteCount >= SBUS_PAYLOAD_SIZE)
                    state = READ_FOOTER;
                break;

            case READ_FOOTER:
                state = IDLE;
                if (data == SBUS_FOOTER)
                    packet_complete = true;
                break;
        }
    }
    return packet_complete;
}


bool sbusGetChannels(rc_t rc)
{
    if (parse()) {
        sbusChannels_t data;
        memcpy(&data, _buffer, 23);

        rc[1]   = mapf((float)data.chan1, 0, 2048, -1, 1);
        rc[2]   = mapf((float)data.chan2, 0, 2048, -1, 1);
        rc[3]   = mapf((float)data.chan3, 0, 2048, -1, 1);
        rc[4]   = mapf((float)data.chan4, 0, 2048, -1, 1);
        rc[5]   = mapf((float)data.chan5, 0, 2048, -1, 1);
        rc[6]   = mapf((float)data.chan6, 0, 2048, -1, 1);
        rc[7]   = mapf((float)data.chan7, 0, 2048, -1, 1);
        rc[8]   = mapf((float)data.chan8, 0, 2048, -1, 1);
        rc[9]   = mapf((float)data.chan9, 0, 2048, -1, 1);
        rc[10]  = mapf((float)data.chan10, 0, 2048, -1, 1);
        rc[11]  = mapf((float)data.chan11, 0, 2048, -1, 1);
        rc[12]  = mapf((float)data.chan12, 0, 2048, -1, 1);
        rc[13]  = mapf((float)data.chan13, 0, 2048, -1, 1);
        rc[14]  = mapf((float)data.chan14, 0, 2048, -1, 1);
        rc[15]  = mapf((float)data.chan15, 0, 2048, -1, 1);
        rc[16]  = mapf((float)data.chan16, 0, 2048, -1, 1);
        rc[17]  = (data.flags & SBUS_FRAME_CHAN17) ? 1.0f : -1.0f;
        rc[18]  = (data.flags & SBUS_FRAME_CHAN18) ? 1.0f : -1.0f;

        _failsafe  = (bool)(data.flags & SBUS_FRAME_FAILSAFE);

        return true;
    }

    return false;
}


bool sbusGetIsFailsafe(void)
{
    return _failsafe;
}
