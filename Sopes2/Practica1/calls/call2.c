#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#define SYS_uptime 550 
int main() {
    long uptime_seconds = syscall(SYS_uptime);
    if (uptime_seconds == -1) {
        perror("syscall");
        return 1;
    }
    long hours = uptime_seconds / 3600;
    long minutes = (uptime_seconds % 3600) / 60;
    long seconds = uptime_seconds % 60;
    printf("System uptime: %ld hours, %ld minutes, %ld seconds\n", hours, minutes, seconds);
    return 0;
}
