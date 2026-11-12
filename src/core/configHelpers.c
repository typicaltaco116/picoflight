// Primary Header
#include <picoflight/core/configHelpers.h>

// Local Headers
#include <picoflight/stability/PID.h>
#include <picoflight/core/config.h>
#include <picoflight/core/utilities.h>

// Standard Headers
#include <string.h>

#include <stdio.h>
PID_gains_t configGetIfPIDGains(const char *prefix, PID_gains_t val)
{
    char pName[32];
    char iName[32];
    char dName[32];
    char allName[32];
    strcpy(pName, prefix); strcat(pName, "_P");
    strcpy(iName, prefix); strcat(iName, "_I");
    strcpy(dName, prefix); strcat(dName, "_D");
    strcpy(allName, prefix); strcat(allName, "_ALL");

    configFetch();

    if (configCheckExists(pName)) {
        PID_gains_t gains;
        gains.p     = safeCastFloat(configGetEntry(pName));
        gains.i     = safeCastFloat(configGetEntry(iName));
        gains.d     = safeCastFloat(configGetEntry(dName));
        gains.all   = safeCastFloat(configGetEntry(allName));
        return gains;
    }
    else {
        configAddNewEntry(pName, safeCastInt(val.p));
        configAddNewEntry(iName, safeCastInt(val.i));
        configAddNewEntry(dName, safeCastInt(val.d));
        configAddNewEntry(allName, safeCastInt(val.all));
        configCommit();
        return val;
    }
}

PID_integral_bounds_t configGetIfPIDBounds(
        const char *prefix,
        PID_integral_bounds_t val)
{
    char minName[32];
    char maxName[32];
    strcpy(minName, prefix); strcat(minName, "_MIN");
    strcpy(maxName, prefix); strcat(maxName, "_MAX");

    configFetch();

    if (configCheckExists(minName)) {
        PID_integral_bounds_t bounds;
        bounds.iMin = safeCastFloat(configGetEntry(minName));
        bounds.iMax = safeCastFloat(configGetEntry(maxName));
        return bounds;
    }
    else {
        configAddNewEntry(minName, safeCastInt(val.iMin));
        configAddNewEntry(maxName, safeCastInt(val.iMax));
        configCommit();
        return val;
    }
}
