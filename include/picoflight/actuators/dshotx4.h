#pragma once

// PICO-sdk Headers
#include <hardware/pio.h>

// Standard Headers
#include <stdint.h>

typedef struct {
    PIO pio;
    uint8_t pinBase;
    uint8_t pinCount;
    uint8_t sm;
    uint32_t speed;
    uint8_t offset;
} dshotx4_t;

dshotx4_t* dshotx4Construct(
    uint8_t pinBase,
    uint8_t pinCount,
    uint32_t speed,
    PIO pio,
    uint8_t sm
);

void dshotx4Free(dshotx4_t *obj);

void dshotx4SendRawThrottles(dshotx4_t *obj, uint16_t *data);

void dshotx4SendThrottles(dshotx4_t *obj, uint16_t *data);

typedef enum {
	DSHOT_CMD_MOTOR_STOP = 0,
	DSHOT_CMD_BEACON1,
	DSHOT_CMD_BEACON2,
	DSHOT_CMD_BEACON3,
	DSHOT_CMD_BEACON4,
	DSHOT_CMD_BEACON5,
	DSHOT_CMD_ESC_INFO, // V2 INCLUDES SETTINGS
	DSHOT_CMD_SPIN_DIRECTION_1,
	DSHOT_CMD_SPIN_DIRECTION_2,
	DSHOT_CMD_3D_MODE_OFF,
	DSHOT_CMD_3D_MODE_ON,
	DSHOT_CMD_SETTINGS_REQUEST, // CURRENTLY NOT IMPLEMENTED
	DSHOT_CMD_SAVE_SETTINGS,
	DSHOT_CMD_EXTENDED_TELEMETRY_ENABLE,
	DSHOT_CMD_EXTENDED_TELEMETRY_DISABLE,
	DSHOT_CMD_SPIN_DIRECTION_NORMAL = 20,
	DSHOT_CMD_SPIN_DIRECTION_REVERSED = 21,
	DSHOT_CMD_LED0_ON, // BLHELI32 ONLY
	DSHOT_CMD_LED1_ON, // BLHELI32 ONLY
	DSHOT_CMD_LED2_ON, // BLHELI32 ONLY
	DSHOT_CMD_LED3_ON, // BLHELI32 ONLY
	DSHOT_CMD_LED0_OFF, // BLHELI32 ONLY
	DSHOT_CMD_LED1_OFF, // BLHELI32 ONLY
	DSHOT_CMD_LED2_OFF, // BLHELI32 ONLY
	DSHOT_CMD_LED3_OFF, // BLHELI32 ONLY
	DSHOT_CMD_AUDIO_STREAM_MODE_ON_OFF = 30, // KISS AUDIO STREAM MODE ON/OFF
	DSHOT_CMD_SILENT_MODE_ON_OFF = 31, // KISS SILENT MODE ON/OFF
	DSHOT_CMD_MAX = 47
} dshotCommands_e;

void dshotx4SendCommand(dshotx4_t *obj, dshotCommands_e cmd);
