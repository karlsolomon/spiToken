#include <stdio.h>
#include <stdint.h>
#include "Timer.h"

int main(void)
{
    Timer_Init();
    while(1)
    {
        uint32_t tick = Timer_GetTick();
        printf("time = %d\n", tick);
        Timer_Sleep(100);
    }
    return 0;
}