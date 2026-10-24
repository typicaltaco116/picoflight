#pragma once

#include <picoflight/orientation/types.h>

void boardOrientationSetOffsets(euler_t angles, euler_t rates);

void boardOrientationTransform(euler_t *angles, rates_t *rates);
