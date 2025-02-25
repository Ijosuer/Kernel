#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define SYS_my_decrypt 553

void mostrar_uso()
{
    printf("Uso: decrypt_app -p <archivo_entrada> -o <archivo_salida> -j <hilos> -k <archivo_llave>\n");
}

int main(int argc, char *argv[])
{
    char *entrada = NULL;
    char *salida = NULL;
    char *llave = NULL;
    int hilos = 0;
    int opt;

    printf("\nDesencriptando archivo ;D\n");

    while ((opt = getopt(argc, argv, "p:o:j:k:")) != -1)
    {
        if (opt == 'p')
        {
            entrada = optarg;
        }
        else if (opt == 'o')
        {
            salida = optarg;
        }
        else if (opt == 'j')
        {
            hilos = atoi(optarg);
        }
        else if (opt == 'k')
        {
            llave = optarg;
        }
        else
        {
            mostrar_uso();
            return 1;
        }
    }

    // Manejo de errores
    if (!entrada || !salida || hilos <= 0 || !llave)
    {
        printf("Error: Faltan argumentos obligatorios =( \n");
        mostrar_uso();
        return 1;
    }

    if (access(entrada, F_OK) == -1)
    {
        printf("Error: El archivo de entrada no existe :s \n");
        return 1;
    }

    if (access(llave, F_OK) == -1)
    {
        printf("Error: El archivo de llave no existe :c \n");
        return 1;
    }

    if (hilos <= 0)
    {
        printf("Error: El número de hilos no es válido =( \n");
        return 1;
    }

    // Llama a la syscall
    int resultado = syscall(SYS_my_decrypt, entrada, salida, hilos, llave);
    if (resultado < 0)
    {
        perror("syscall my_decrypt falló :c");
        return errno;
    }

    printf("\nArchivo desencriptado exitosamente 🐧\n\n");
    return 0;
}
