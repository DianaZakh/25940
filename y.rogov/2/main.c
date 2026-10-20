#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

extern char *tzname[];

int main()
{
    time_t now;
    time(&now);

    putenv("TZ=America/Los_Angeles");
    struct tm *pst = localtime(&now);

    printf("CURRENT TIME: %d/%d/%02d %d:%02d %s\n",
            pst->tm_mday, pst->tm_mon + 1,
            1900 + pst->tm_year, pst->tm_hour,
            pst->tm_min, tzname[pst->tm_isdst]);

    if (pst->tm_isdst) {
        now -= 60 * 60 * 1;
        pst = localtime(&now);
        pst->tm_isdst = 0;
    }

    printf("PST TIME: %d/%d/%02d %d:%02d %s\n",
            pst->tm_mday, pst->tm_mon + 1,
            1900 + pst->tm_year, pst->tm_hour,
            pst->tm_min, tzname[pst->tm_isdst]);

    exit(0);
}