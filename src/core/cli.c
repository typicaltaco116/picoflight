// Primary Header
#include <picoflight/core/cli.h>

// Local Headers
#include <picoflight/core/config.h>

// Standard Headers
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

static const char* _path[] = {
    "get",
    "set",
    "echo",
};
typedef enum {
    PATH_GET,
    PATH_SET,
    PATH_ECHO,
} path_e;
static const uint32_t _pathEntries = sizeof(_path) / sizeof(_path[0]);


// If value is found return path index. Else return -1.
static int checkPath(const char *token)
{
    int exitCode = -1;
    for (uint32_t i = 0; i < _pathEntries; ++i) {
        if (strcmp(token, _path[i]) == 0)
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

// WIP
static int executeGet(char **argv, uint32_t argc)
{
    return 0;
}

// WIP
// set <type> <parameter> <val> 
static int executeSet(char **argv, uint32_t argc)
{
    if (argc != 4)
        return 1;

    const char *typeStr = argv[1];
    const char *nameStr = argv[2];
    const char *valStr = argv[3];

    if (strcmp(typeStr, "int") == 0) {
    }
    else if (strcmp(typeStr, "float") == 0) {
    }
    else if (strcmp(typeStr, "string") == 0) {
    }
    else
        return 1;

    return 0;
}

static int executeEcho(char **argv, uint32_t argc)
{
    for (uint32_t i = 1; i < argc; ++i) {
        puts(argv[i]);
    }
    return 0;
}

void cliExecuteString(const char *str)
{
    uint32_t numTokens;
    char **tokens = tokenize(str, ' ', &numTokens);

    int32_t pathIdx = checkPath(tokens[0]);
    if (pathIdx == -1) {
        freeTokens(tokens, numTokens);
        return;
    }

    switch (pathIdx) {
        case PATH_GET:  executeGet(tokens, numTokens); break;
        case PATH_SET:  executeSet(tokens, numTokens); break;
        case PATH_ECHO: executeEcho(tokens, numTokens); break;
        default: break;
    }

    freeTokens(tokens, numTokens);
}
