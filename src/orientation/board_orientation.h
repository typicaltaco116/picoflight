#pragma once

#include "src/orientation/types.h"

void board_orientation_SetOffsets(euler_t angles, euler_t rates);

void board_orientation_Transform(euler_t *angles, rates_t *rates);
