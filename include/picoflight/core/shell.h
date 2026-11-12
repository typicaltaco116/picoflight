#pragma once

// Standard Headers
#include <stdint.h>


// ALL OF THESE NEED TO BE PROPERLY ADDED TO THE CLI PATH IN CLI.H

// get <type> <name>
int32_t shellExecuteGet(char **argv, int32_t argc);

// set <type> <name> <value>
int32_t shellExecuteSet(char **argv, int32_t argc);

// echo <args>
int32_t shellExecuteEcho(char **argv, int32_t argc);

// reset
int32_t shellExecuteReset(char **argv, int32_t argc);
