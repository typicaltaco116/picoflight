// Primary Header
#include <picoflight/core/shell.h>

// Local Headers
#include <picoflight/core/cli.h>
#include <picoflight/core/config.h>
#include <picoflight/core/utilities.h>

// Standard Headers
#include <stdint.h>
#include <stdio.h>

int32_t shellExecuteGet(char **argv, int32_t argc)
{
    if (argc != 3)
        return 1;

    const char *typeStr = argv[1];
    const char *nameStr = argv[2];

    if (exactStrcmp(typeStr, "int")) {
        int value = *(int*)configGetParam(nameStr);
        printf("%s = %d\n", nameStr, value);
    }
    else if (exactStrcmp(typeStr, "float")) {
    }
    else if (exactStrcmp(typeStr, "string")) {
    }
    else {
        return 1;
    }
} 

int32_t shellExecuteSet(char **argv, int32_t argc)
{
    return 0; // placeholder
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

int32_t shellExecuteWrite(char **argv, int32_t argc)
{
    return configWriteAll();
} 

int32_t shellExecuteReset(char **argv, int32_t argc)
{
    return 0; // placeholder.
}
