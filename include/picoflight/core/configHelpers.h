#pragma once

// Local Headers
#include <picoflight/stability/PID.h>

// Will set the value of gains if the values are valid in flash memory.
// Otherwise leave gains untouched.
PIDGains_t configGetIfPIDGains(const char *prefix, PIDGains_t defaultValue);

PIDIntegralLimits_t configGetIfPIDBounds(
        const char *prefix,
        PIDIntegralLimits_t defaultValue
);
