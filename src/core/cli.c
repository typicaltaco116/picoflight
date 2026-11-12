// Primary Header
#include <picoflight/core/cli.h>

// Local Headers
#include <picoflight/core/config.h>
#include <picoflight/core/shell.h>
#include <picoflight/core/utilities.h>

// Pico-SDK Headers
#include <pico/stdlib.h>

// Standard Headers
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define CLI_SHELL_EXIT -5
#define CLI_COMMAND_NOT_FOUND -6

static const char* _path[] = {
    "get", // Gets a specific parameter.
    "set", // Stages a specific parameter.
    "echo",
    "reset", // sets all config parameters to default values.
    "exit"
};
typedef enum {
    PATH_GET,
    PATH_SET,
    PATH_ECHO,
    PATH_RESET,
    PATH_EXIT
} path_e;
static const uint32_t _pathEntries = sizeof(_path) / sizeof(_path[0]);

typedef int32_t (*executablePtr_t)(char **argv, int32_t argc);

executablePtr_t _pathExecutables[] = {
    shellExecuteGet,
    shellExecuteSet,
    shellExecuteEcho,
    shellExecuteReset,
};

// If value is found return path index. Else return -1.
static int checkPath(const char *token)
{
    int exitCode = -1;
    for (uint32_t i = 0; i < _pathEntries; ++i) {
        if (exactStrcmp(token, _path[i]))
            exitCode = i;
    }
    return exitCode;
}

static uint32_t getNumTokens(const char *str, uint32_t length, char delimiter)
{
    uint32_t numDelimiters = 0;

    if (length == 0)
        return 0;

    for (uint32_t i = 0; i < length; ++i) {
        if (str[i] == delimiter)
            numDelimiters++;

        if (i == 0)
            continue;

        // Remove duplicate delimiters
        if (str[i] == delimiter && str[i - 1] == delimiter)
            numDelimiters--;
    }

    // Remove Leading and trailing delimiters.
    if (str[length - 1] == delimiter)
        numDelimiters--;
    if (str[0] == delimiter)
        numDelimiters--;

    uint32_t numTokens = numDelimiters + 1;
    return numTokens;
}

// Returns a heap allocated array of tokens.
static char** tokenize(const char *str, char delimiter, uint32_t *numTokensRtr)
{
    uint32_t length = strlen(str);
    if (length == 0)
        return NULL;

    uint32_t numTokens = getNumTokens(str, length, delimiter);
    *numTokensRtr = numTokens;

    char **tokens = malloc(sizeof(char*) * numTokens);

    // Skip leading
    uint32_t startIdx = 0;
    while (startIdx < length && str[startIdx] == delimiter)
        startIdx++;

    uint32_t start = 0;
    uint32_t tokenLength;
    for (uint32_t i = 0; i < numTokens; ++i) {
        while (start < length && str[start] == delimiter)
            start++;

        tokenLength = 0;
        while (start + tokenLength < length && str[start + tokenLength] != delimiter)
            tokenLength++;

        char *token = malloc(sizeof(char) * (tokenLength + 1));
        memcpy(token, str + start, tokenLength);
        token[tokenLength] = '\0'; // Null terminate tokens.
        tokens[i] = token;
        
        start = start + tokenLength;
    }

    return tokens;
}

static void freeTokens(char **tokens, uint32_t numTokens)
{
    for (uint32_t i = 0; i < numTokens; ++i) {
        free(tokens[i]);
    }
    free(tokens);
}

int32_t cliExecuteString(const char *str)
{
    uint32_t numTokens;
    char **tokens = tokenize(str, ' ', &numTokens);

    int32_t pathIdx = checkPath(tokens[0]);
    if (pathIdx == -1) {
        freeTokens(tokens, numTokens);
        return CLI_COMMAND_NOT_FOUND;
    }

    // Call function pointer.
    int32_t exitCode;
    if (pathIdx == PATH_EXIT) {
        exitCode = CLI_SHELL_EXIT;
    }
    else
        exitCode = _pathExecutables[pathIdx](tokens, numTokens);

    freeTokens(tokens, numTokens);

    return exitCode;
}

#define DEL_CHAR 0x7F
static void blockingReadlineStripped(char *buffer, uint32_t bufferSize)
{
    uint32_t idx = 0;

    while (idx < bufferSize - 1) {
        int c = stdio_getchar();
        stdio_putchar(c);
        if (c == PICO_ERROR_TIMEOUT || (char)c == '\r') {
            printf("\n");
            break;
        }
        else if ((char)c == DEL_CHAR) {
            if (idx > 0) {
                printf("\b");
                idx--;
            }
        }
        else {
            buffer[idx++] = (char)c;
        }
    }

    buffer[idx] = '\0'; // null terminate
}

void cliProcessInput(void)
{
    bool keepGoing = true;

    while (keepGoing) {
        printf("PicoFlightCLI: ");
        char line[128];
        blockingReadlineStripped(line, sizeof(line));
        int32_t exitCode = cliExecuteString(line);
        if (exitCode == CLI_COMMAND_NOT_FOUND) {
            printf("Command not found...\n");
        }
        else if (exitCode == CLI_SHELL_EXIT)
            keepGoing = false;
    }
}
