#pragma once

// Local Headers
#include <picoflight/stability/pid.h>

// Will set the value of gains if the values are valid in flash memory.
// Otherwise leave gains untouched.
pidGains_t configGetIfPIDGains(const char *prefix, pidGains_t defaultValue);

pidIntegralLimits_t configGetIfPIDBounds(
        const char *prefix,
        pidIntegralLimits_t defaultValue
);
