// Primary Header
#include <picoflight/core/configHelpers.h>

// Local Headers
#include <picoflight/stability/PID.h>
#include <picoflight/core/config.h>
#include <picoflight/core/utilities.h>

// Standard Headers
#include <string.h>

PID_gains_t configGetIfPIDGains(const char *prefix, PID_gains_t val)
{
    char paramPrefix[64];
    strcpy(paramPrefix, prefix);
    strcat(paramPrefix, "_");

    configFetch();

    if (configCheckExists(strcat(paramPrefix, "P"))) {
        PID_gains_t gains;
        gains.p = safeCastFloat(configGetEntry(strcat(paramPrefix,"P")));
        gains.i = safeCastFloat(configGetEntry(strcat(paramPrefix,"I")));
        gains.d = safeCastFloat(configGetEntry(strcat(paramPrefix,"D")));
        gains.all = safeCastFloat(configGetEntry(strcat(paramPrefix,"ALL")));
        return gains;
    }
    else {
        configAddNewEntry(strcat(paramPrefix,"P"), safeCastInt(val.p));
        configAddNewEntry(strcat(paramPrefix,"I"), safeCastInt(val.i));
        configAddNewEntry(strcat(paramPrefix,"D"), safeCastInt(val.d));
        configAddNewEntry(strcat(paramPrefix,"ALL"), safeCastInt(val.all));
        configCommit();
        return val;
    }
}

PID_integral_bounds_t configGetIfPIDBounds(
        const char *prefix,
        PID_integral_bounds_t val)
{
    char paramPrefix[64];
    strcpy(paramPrefix, prefix);
    strcat(paramPrefix, "_");

    configFetch();

    if (configCheckExists(strcat(paramPrefix, "MIN"))) {
        PID_integral_bounds_t bounds;
        bounds.iMin = safeCastFloat(configGetEntry(strcat(paramPrefix,"MIN")));
        bounds.iMax = safeCastFloat(configGetEntry(strcat(paramPrefix,"MAX")));
        return bounds;
    }
    else {
        configAddNewEntry(strcat(paramPrefix,"MIN"), safeCastInt(val.iMin));
        configAddNewEntry(strcat(paramPrefix,"MAX"), safeCastInt(val.iMax));
        configCommit();
        return val;
    }
}
