// Primary Header
#include <picoflight/core/shell.h>

// Local Headers
#include <picoflight/core/cli.h>
#include <picoflight/core/config.h>
#include <picoflight/core/utilities.h>

// Standard Headers
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int32_t shellExecuteGet(char **argv, int32_t argc)
{
    if (argc != 3)
        return 1;

    const char *typeStr = argv[1];
    const char *nameStr = argv[2];

    configFetch();

    if (exactStrcmp(typeStr, "int")) {
        if (configCheckExists(nameStr))
            printf("%s = %d\n", nameStr, (int)configGetEntry(nameStr));
        else
            printf("%s not defined\n", nameStr);
    }
    else if (exactStrcmp(typeStr, "float")) {
        if (configCheckExists(nameStr))
            printf("%s = %.03f\n", nameStr, (float)configGetEntry(nameStr));
        else
            printf("%s not defined\n", nameStr);
    }

    return 1;

} 

int32_t shellExecuteSet(char **argv, int32_t argc)
{
    if (argc != 4)
        return 1;

    const char *typeStr = argv[1];
    const char *nameStr = argv[2];
    const char *valStr  = argv[3];

    uint32_t data;
    if (exactStrcmp(typeStr, "int")) {
        data = (uint32_t)atoi(valStr);
    }
    else if (exactStrcmp(typeStr, "float")) {
        data = (uint32_t)atof(valStr);
    }
    else {
        return 1;
    }

    configFetch();

    int32_t exitCode = 0;
    if (configCheckExists(nameStr)) {
        configModifyEntry(nameStr, data);
    }
    else {
        exitCode = configAddNewEntry(nameStr, data);
    }

    configCommit();

    return exitCode;

} 

int32_t shellExecuteEcho(char **argv, int32_t argc)
{
    if (argc == 1)
        return 0;

    for (int i = 1; i < argc; ++i) {
        puts(argv[i]);
    }
    return 0;
} 

int32_t shellExecuteReset(char **argv, int32_t argc)
{
    (void)argc; (void)argv;
    configReset();
    return 0;
}
