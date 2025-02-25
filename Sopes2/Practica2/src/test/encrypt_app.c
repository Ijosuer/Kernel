
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/syscall.h>

// Definición de la syscall
#define SYS_my_encrypt 552

// Función para mostrar el uso del programa
void mostrar_uso()
{
    printf("Uso: encrypt_app -p <archivo_entrada> -o <archivo_salida> -j <hilos> -k <archivo_llave>\n");
}

// Función principal
int main(int argc, char *argv[])
{
    char *entrada = NULL;
    char *salida = NULL;
    char *llave = NULL;
    int hilos = 0;
    int opt;

    printf("\nEncriptando archivo 🐧 ...\n");

    // Procesa los argumentos
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
    int resultado = syscall(SYS_my_encrypt, entrada, salida, hilos, llave);
    // Manejo de errores
    if (resultado < 0)
    {
        perror("Error en la syscall my_encrypt :c");
        return errno;
    }

    // Mensaje de éxito
    printf("\n Archivo encriptado exitosamente 🐧\n\n");
    return 0;
}
