/*
 * clock_gettime.h
 *
 *  Created on: 08.04.2017
 *      Author: Stefan
 */

#ifndef CLOCK_GETTIME_H_
#define CLOCK_GETTIME_H_


// Provide function clock_gettime(..).
// Credit goes to its author "work.bin", as I copied this code from
// http://stackoverflow.com/questions/5404277/porting-clock-gettime-to-windows

#define BILLION                             (1E9)

static BOOL g_first_time = 1;
static LARGE_INTEGER g_counts_per_sec;

int clock_gettime(struct timespec *ct)
{
    LARGE_INTEGER count;

    if (g_first_time)
    {
        g_first_time = 0;

        if (0 == QueryPerformanceFrequency(&g_counts_per_sec))
        {
            g_counts_per_sec.QuadPart = 0;
        }
    }

    if ((NULL == ct) || (g_counts_per_sec.QuadPart <= 0) ||
            (0 == QueryPerformanceCounter(&count)))
    {
        return -1;
    }

    ct->tv_sec = count.QuadPart / g_counts_per_sec.QuadPart;
    ct->tv_nsec = ((count.QuadPart % g_counts_per_sec.QuadPart) * BILLION) / g_counts_per_sec.QuadPart;

    return 0;
}

#endif /* CLOCK_GETTIME_H_ */
