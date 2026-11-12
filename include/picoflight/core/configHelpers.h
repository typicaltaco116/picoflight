#pragma once

// Local Headers
#include <picoflight/stability/PID.h>

// Will set the value of gains if the values are valid in flash memory.
// Otherwise leave gains untouched.
PID_gains_t configGetIfPIDGains(const char *prefix, PID_gains_t defaultValue);

PID_integral_bounds_t configGetIfPIDBounds(
        const char *prefix,
        PID_integral_bounds_t defaultValue
);
