#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "pico/stdlib.h"

int main()
{

    stdio_init_all();

    printf("start\n");

    for(;;)
    {
        sleep_ms(1000);
        printf("hello world\n");
    }

    return 0;
}