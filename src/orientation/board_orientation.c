#include "board_orientation.h"

#include "src/orientation/types.h"

static euler_t _angles_offset = 0.0f;
static rates_t _rates_offset = 0.0f;

void board_orientation_SetOffsets(euler_t angles, euler_t rates)
{
    _angles_offset = angles;
    _rates_offset = rates;
}


void board_orientation_Transform(euler_t *angles, rates_t *rates)
{
    *angles += _angles_offset;
    *rates += _rates_offset;
}
