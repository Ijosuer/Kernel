#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

#define SYS_getlogskernelss 551 
#define LOG_BUFFER_SIZE 1024

int main() {
    char buffer[LOG_BUFFER_SIZE]; 
    long result;
    memset(buffer, 0, LOG_BUFFER_SIZE);
    result = syscall(SYS_getlogskernelss, buffer, LOG_BUFFER_SIZE);
    ssize_t log_size = syscall(SYS_getlogskernelss, buffer, LOG_BUFFER_SIZE);
    if (result < 0) {
        perror("Error getting kernel logs");
        return 1;  
    }
    printf("Last 5 log messages:\n%.*s\n", (int)log_size, buffer);
    return 0;
}



