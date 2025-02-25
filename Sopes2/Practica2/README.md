# *Manual Técnico ☝️🤓* 


## 📚 Contenido    


  - 🎯 Objetivos
  - 📍 Tecnologías
  - 🛠️ Herramientas
  - 💼 Paquetes
  - 🛡️ Guía Instalación


## 🎯 Objetivos


- Comprender como funcionan los hilos en Linux.
- Comprender los conceptos de concurrencia y paralelismo.
- Aplicar conceptos de sincronización de procesos.


## 📍 Tecnologías

Estas son las tecnologías, herramientas y paquetes utilizados en el práctica:

- [**Imagen Linux Mint 22 Cinammon Edition**](https://www.linuxmint.com/download.php)
- [**Git**](https://git-scm.com/downloads) 
- [**Longterm**](https://kernel.org/)

## 🛠️ Herramientas
- [**VMware Workstation**](https://www.vmware.com/products/workstation-pro/workstation-pro-evaluation.html)
- [**Visual Studio Code:**](https://code.visualstudio.com/download)

## 💼 Paquetes

- **GCC**
- **Make**
- **build-essentia**
- **libncurses-dev**
- **bison**
- **flex**
- **libssl-dev**
- **libelf-dev**
- **fakeroot**
- **dwarves**

## 🛡️ Guía Instalación

## Paso 1: Actualizar el sistema

```bash
sudo apt update
sudo apt upgrade
```

## Paso 2: Instalar paquetes necesarios

```bash
sudo apt install build-essential libncurses-dev bison flex libssl-dev libelf-dev fakeroot dwarves
```

## Paso 3: Descargar el kernel de Linux

Visitar el sitio oficial de [Kernel.org](https://www.kernel.org/) y descargar la versión deseada (preferencia copiar link *tarball*).

![alt text](./img/image.png)

Luego que copiamos el enlace `tarball` y con el siguiente comando lo descargarmos y comprimimos:

```bash
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.6.46.tar.xz
tar -xvf linux-6.6.46.tar.xz
```

## Paso 4: Configurar el kernel

Primero ingresamos a la carpeta del kernel:

```bash
cd linux-6.6.46
```

La configuración del kernel se debe especificar en un archivo .config. Para no escribir este desde 0 vamos a copiar el archivo de configuración de su Linux actualmente instalado:

```bash
cp -v /boot/config-$(uname -r) .config
```
Sin embargo, este esta lleno de modulos y drivers que no necesitamos que pueden aumentar el tiempo de compilación. Por lo que utilizamos el comando localmodconfig que analiza los módulos del kernel cargados de su sistema y modifica el archivo .config de modo que solo estos módulos se incluyan en la compilación. Tomar en cuenta que esto causará que nuestro kernel compilado solo funcione en nuestra maquina, por lo que si quieren que sea portatil omitan este paso. Y luego de poner el comando solo skipeamos los modulos con enter  hasta que aparezca nuestra terminal de nuevo..

```bash
make localmodconfig
```

Luego tenemos que modificar el .config, ya que al copiar nuestro .config se incluyeron nuestras llaves privadas, por lo que tendremos que reliminarlas del .config.

```bash
scripts/config --disable SYSTEM_TRUSTED_KEYS
scripts/config --disable SYSTEM_REVOCATION_KEYS
scripts/config --set-str CONFIG_SYSTEM_TRUSTED_KEYS ""
scripts/config --set-str CONFIG_SYSTEM_REVOCATION_KEYS ""
```

## Paso 5: Compilar el kernel

Primero verificamos cuantos nucleos tiene nuestra maquina para poder compilar el kernel de manera mas rapida:

```bash
nproc
```

Luego de esto procedemos a compilar el kernel, el `nproc` es el numero de nucleos que tiene nuestra maquina:
```bash
fakeroot make -j$(nproc)
```
Luego de esto empezará a compilar el kernel, dicho proceso puede tardar dependiendo de la potencia de su maquina y el numero de nucleos que tenga. Pero una vez que termine tendremos nuestro kernel compilado y para verificar que todo salio bien en el proceso de compilación podemos correr el siguiente comando:

```bash
echo $?
```

Si el comando anterior retorna un 0, entonces la compilación fue exitosa.

## Paso 6: Instalar el kernel

La instalación se divide en dos partes: instalar los módulos del kernel e instalar el kernel mismo.

Para instalar los módulos del kernel:

```bash
sudo make modules_install
```

Para instalar el kernel:

```bash
sudo make install
```

Luego de haber instalar el kernel, tenemos que actualizar el GRUB para que este pueda reconocer nuestro nuevo kernel:

```bash
sudo update-grub
```

Luego reiniciamos nuestra maquina con el siguiente comando:

```bash
sudo reboot
```

Justo en el instante que la maquina empieza a reiniciarse pulsamos las teclas `Shift + Esc` para que nos aparezca el menu de GRUB y seleccionamos nuestro nuevo kernel. Aparecerá la siguiente interfaz y pulsamos la opcion de `Advanced options for Linux Mint 22 Cinammon`:

![alt text](./img/ss1.png)

Luego seleccionamos nuestro nuevo kernel que es el que tiene el numero de version que compilamos:

![alt text](./img/ss2.png)


Luego de esto ya podremos ingreso a nuestra máquina virtual con nuestro nuevo kernel compilado.

## 💻☎️ Guía Edición del Kernel y Llamadas al Sistema:

## Paso 1: Cambiar el nombre del sistema operativo

Ingresamos  a la carpeta del kernel:

```bash
cd linux-6.6.46
```
Hacemos un ls para ver los archivos que contiene la carpeta:

```bash
ls
```
![alt text](./img/ss3.png)



## Paso 2: Implementacion de llamadas al sistema

### Llamadas al sistema:
##### 1. Llamada al sistema para encriptar.
##### 2. Llamada al sistema para desencriptar.


Primero vamos a la siguiente path `linux-6.6.45/arch/x86/entry/syscalls` y editamos el archivo `syscall_64.tbl` con el siguiente comando:

```bash
sudo nano syscall_64.tbl
```

Luego de esto agregamos las siguientes linea al final del archivo:

![alt text](./img/tbl.png)

Luego de esto guardamos los cambios con `Ctrl + O` y salimos con `Ctrl + X`.

Luego de esto vamos a la siguiente path `/linux-6.6.45/include/linux` y creamos el archivo `syscalls_usac.h` con el siguiente comando:

```bash
sudo nano syscalls_usac.h
```


Luego de esto agregamos  las siguientes linea, para definir las syscall:

```c++
#ifndef _SYSCALLS_USAC_H_
#define _SYSCALLS_USAC_H_
#include <linux/linkage.h>
#include <linux/types.h>
#include <linux/kernel.h>

// Definición de las syscalls
asmlinkage long sys_my_encrypt(const char __user *input_path, const char __user *output_path, int num_threads, const char __user *key_path);
asmlinkage long sys_my_decrypt(const char __user *input_path, const char __user *output_path, int num_threads, const char __user *key_path);

#endif /* _LINUX_SYSCALLS_USAC_H */
```
Luego de esto guardamos los cambios con `Ctrl + O` y salimos con `Ctrl + X`.


Luego de esto vamos a la siguiente path `/linux-6.6.45/kernel` y creamos el archivo `syscalls_usac.c` con el siguiente comando:

```bash
sudo nano syscalls_usac.c
```

Primero agregamos las librerias necesarias al inicio del archivo:

![alt text](./img/libs.png)


Luego de esto agregamos las siguientes funciones al archivo:
- Función para encriptar

![alt text](./img/encrypt.png)

- Función para desencriptar

![alt text](./img/decrypt.png)

Realmente hay más código en el archivo, pero este es el más importante. Luego de esto guardamos los cambios con `Ctrl + O` y salimos con `Ctrl + X`.


Luego de realizar todos estos cambios en los 3 archivos `syscall_64.tbl`, `syscalls_usac.h` y `syscalls_usac.c` procedemos a compilar el kernel, si no hay errores en la compilación procedemos a instalar el kernel, actualizar el GRUB y reiniciar la maquina.

## Paso 3: Testear las llamadas al sistema

Luego de compilar el kernel y reiniciar la maquina, procedemos a testear las llamadas al sistema que implementamos.

#### Testeando la llamada al sistema para encriptar:
Primero creamos un archivo llamado `encrypt_app.c` y dentro de el ponemos el siguiente codigo:

![alt text](./img/userencrypt.png)

Luego accedemos via bash en donde se encuentra al archivo `encrypt_app.c` y corremos el siguiente comando:

```bash
gcc encrypt_app.c -o encrypt_app
```

Luego de esto corremos el siguiente comando:

```bash
sudo ./encrypt_app -p archivo_entrada.txt -o archivo_salida.txt -j 2  -k clave.txt
```

Y nos debería aparecer el siguiente mensaje en consola:

![alt text](./img/salidaencrypt.png)

Luego verificiamos el archivo de salida y debería estar encriptado asi:

![alt text](./img/archivoencr.png)
#### Testeando la llamada al sistema para desencriptar:

Primero creamos un archivo llamado `decrypt_app.c` y dentro de el ponemos el siguiente codigo:

![alt text](./img/userdecrypt.png)

Luego accedemos via bash en donde se encuentra al archivo `decrypt_app.c` y corremos el siguiente comando:

```bash
gcc decrypt_app.c -o decrypt_app
```

Luego de esto corremos el siguiente comando:

```bash
sudo ./decrypt_app -p archivo_salida.txt -o archivo_desenc.txt -j 2  -k clave.txt
```

Y nos debería aparecer el siguiente mensaje en consola:

![alt text](./img/salidadecrypt.png)

Luego verificiamos el archivo de salida y debería estar encriptado asi:

![alt text](./img/archivodsenc.png)




## 💻🐛❌ Bitácora de Errores:

Durante el proceso de implementación de las llamadas al sistema, se presentaron los siguientes errores:

- **Error 1:** 

```bash
ld: arch/x86/entry/syscall_64.o: en la función `x64_sys_call':
/home/lall0g/linux-6.6.45/./arch/x86/include/generated/asm/syscalls_64.h:554:(.text+0x1559): undefined reference to `__x64_sys_my_decrypt'
ld: /home/lall0g/linux-6.6.45/./arch/x86/include/generated/asm/syscalls_64.h:553:(.text+0x1cc4): undefined reference to `__x64_sys_my_encrypt'
ld: arch/x86/entry/syscall_64.o:(.rodata+0x1140): undefined reference to `__x64_sys_my_encrypt'
ld: arch/x86/entry/syscall_64.o:(.rodata+0x1148): undefined reference to `__x64_sys_my_decrypt'
  BTF     .btf.vmlinux.bin.o
pahole: .tmp_vmlinux.btf: Invalid argument
  LD      .tmp_vmlinux.kallsyms1
.btf.vmlinux.bin.o: file not recognized: file format not recognized
make[2]: *** [scripts/Makefile.vmlinux:37: vmlinux] Error 1
make[1]: *** [/home/lall0g/linux-6.6.45/Makefile:1164: vmlinux] Error 2
make: *** [Makefile:234: __sub-make] Error 2
```

**Solución:** Este error me dio porque en archivo `syscall_64.tbl` no se habia agregado correctamente las llamadas al sistema que se implementaron en el archivo `syscalls_usac.c`. Por lo que se soluciono agregando las llamadas al sistema en el archivo `syscall_64.tbl` y volviendo a compilar el kernel.


- **Error 2:** 

```bash
  CC      drivers/char/random.o
drivers/char/random.c: In function ‘try_to_generate_entropy’:
drivers/char/random.c:1333:1: warning: the frame size of 1152 bytes is larger than 1024 bytes [-Wframe-larger-than=]
 1333 | }
      | ^
```

**Solución:** Este error se debe a que el tamaño del marco de la pila es mayor a 1024 bytes, por lo que se soluciono reiniciando la maquina y volviendo a compilar el kernel.

- **Error 3:** 

```bash
kernel/syscalls_usac.c: In function ‘sys_my_encrypt’:
kernel/syscalls_usac.c:114:5: error: unknown type name ‘mm_segment_t’
  114 |     mm_segment_t old_fs;
      |     ^~~~~~~~~~~~
kernel/syscalls_usac.c:123:14: error: implicit declaration of function ‘get_fs’; did you mean ‘sget_fc’? [-Werror=implicit-function-declaration]
  123 |     old_fs = get_fs();
      |              ^~~~~~
      |              sget_fc
kernel/syscalls_usac.c:124:5: error: implicit declaration of function ‘set_fs’; did you mean ‘sget_fc’? [-Werror=implicit-function-declaration]
  124 |     set_fs(KERNEL_DS);
      |     ^~~~~~
      |     sget_fc
kernel/syscalls_usac.c:124:12: error: ‘KERNEL_DS’ undeclared (first use in this function); did you mean ‘KERNFS_NS’?
  124 |     set_fs(KERNEL_DS);
      |            ^~~~~~~~~
      |            KERNFS_NS
kernel/syscalls_usac.c:124:12: note: each undeclared identifier is reported only once for each function it appears in
kernel/syscalls_usac.c: In function ‘sys_my_decrypt’:
kernel/syscalls_usac.c:203:5: error: unknown type name ‘mm_segment_t’
  203 |     mm_segment_t old_fs;
      |     ^~~~~~~~~~~~
kernel/syscalls_usac.c:213:12: error: ‘KERNEL_DS’ undeclared (first use in this function); did you mean ‘KERNFS_NS’?
  213 |     set_fs(KERNEL_DS);
      |            ^~~~~~~~~
      |            KERNFS_NS
```

**Solución:**  Este error se debe a que no se incluyeron las librerias necesarias en el archivo `syscalls_usac.c`, por lo que se soluciono agregando las librerias necesarias al archivo `syscalls_usac.c` y volviendo a compilar el kernel.

## 📝📚 Referencias

- [**Linux Kernel Development**](https://www.doc-developpement-durable.org/file/Projets-informatiques/cours-&-manuels-informatiques/Linux/Linux%20Kernel%20Development,%203rd%20Edition.pdf)
