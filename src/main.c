#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "src/core/init.h"

int main(void) 
{
    stdio_init_all();

    while (getc(stdin) != '\r');

    core_Init();

    printf("Done...");

    return 0;
}
