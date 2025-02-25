#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buffer[128];
    FILE *fp;
    printf(" Getting uname -rs:\n");
    fp = popen("uname -rs", "r");
    if (fp == NULL) {
        perror("Error al ejecutar uname -rs");
        return 1;
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    pclose(fp);
    printf("\n");
    printf("Results of 'dmesg | grep \"Welcome\"':\n");
    fp = popen("dmesg | grep \"Welcome\"", "r");
    if (fp == NULL) {
        perror("Error executingdmesg | grep \"Welcome\"");
        return 1;
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    pclose(fp);
    printf("\n");
    printf("Results of 'dmesg | grep \"201900647\"':\n");
    fp = popen("dmesg | grep \"201900647\"", "r");
    if (fp == NULL) {
        perror(" Error executing dmesg | grep \"201900647\"");
        return 1;
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    pclose(fp);

    return 0;
}
