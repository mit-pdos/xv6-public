#include "steady_clock.h"

unsigned long long steady_clock_now()
{
    static const unsigned long tsc_frequency_khz = XV6_TSC_FREQUENCY;
    unsigned long long cycles = 0;
    asm("rdtsc" : "=A" (cycles));
    return cycles * 1000 / tsc_frequency_khz;
}
