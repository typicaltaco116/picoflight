#pragma once

#include <picoflight/orientation/types.h>

void boardOrientationSetOffsets(bodyAngles_t angles, bodyAngles_t rates);

void boardOrientationTransform(bodyAngles_t *angles, bodyRates_t *rates);
