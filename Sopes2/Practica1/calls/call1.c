#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/kernel.h>
#include <time.h>
#define SYS_reaaltime 549
int main()
{
    long seconds_since_epoch = syscall(SYS_reaaltime);
    if (seconds_since_epoch == -1)
    {
        perror("syscall");
        return 1;
    }
    printf("Epoch: %ld\n", seconds_since_epoch);
    // Convertir segundos desde epoch a estructura de tiempo
    time_t raw_time = (time_t)seconds_since_epoch;
    struct tm *time_info = localtime(&raw_time);
    if (time_info == NULL)
    {
        perror("localtime");
        return 1;
    }
    printf("Current time: %02d:%02d:%02d\n",
           time_info->tm_hour,
           time_info->tm_min,
           time_info->tm_sec);
    return 0;
}