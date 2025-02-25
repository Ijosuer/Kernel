#include <linux/export.h>
#include <linux/mm.h>
#include <linux/mm_inline.h>
#include <linux/utsname.h>
#include <linux/mman.h>
#include <linux/reboot.h>
#include <linux/prctl.h>
#include <linux/highuid.h>
#include <linux/fs.h>
#include <linux/kmod.h>
#include <linux/ksm.h>
#include <linux/perf_event.h>
#include <linux/resource.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/capability.h>
#include <linux/device.h>
#include <linux/key.h>
#include <linux/times.h>
#include <linux/posix-timers.h>
#include <linux/security.h>
#include <linux/random.h>
#include <linux/suspend.h>
#include <linux/tty.h>
#include <linux/signal.h>
#include <linux/cn_proc.h>
#include <linux/getcpu.h>
#include <linux/task_io_accounting_ops.h>
#include <linux/seccomp.h>
#include <linux/cpu.h>
#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/fs_struct.h>
#include <linux/file.h>
#include <linux/mount.h>
#include <linux/gfp.h>
#include <linux/syscore_ops.h>
#include <linux/version.h>
#include <linux/ctype.h>
#include <linux/syscall_user_dispatch.h>
#include <linux/compat.h>
#include <linux/syscalls.h>
#include <linux/kprobes.h>
#include <linux/user_namespace.h>
#include <linux/time_namespace.h>
#include <linux/binfmts.h>
#include <linux/sched.h>
#include <linux/sched/autogroup.h>
#include <linux/sched/loadavg.h>
#include <linux/sched/stat.h>
#include <linux/sched/mm.h>
#include <linux/sched/coredump.h>
#include <linux/sched/task.h>
#include <linux/sched/cputime.h>
#include <linux/rcupdate.h>
#include <linux/uidgid.h>
#include <linux/cred.h>
#include <linux/nospec.h>
#include <linux/kmsg_dump.h>
#include <generated/utsrelease.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <asm/unistd.h>
#include "uid16.h"
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/ktime.h>
#include <linux/kern_levels.h>
#include <linux/printk.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/syscalls_usac.h>
#include <linux/delay.h>


#include <linux/swap.h>
#include <linux/vmstat.h>
#include <asm/page.h>

// Estructura para manejar fragmentos de datos
typedef struct {
    unsigned char *data;      // Puntero a los datos
    size_t size;             // Tamaño de los datos
    unsigned char *key;      // Clave para la operación XOR
    size_t key_size;         // Tamaño de la clave
    size_t start;            // Índice de inicio para el fragmento
    size_t end;              // Índice final para el fragmento
} Fragment;

// Estructura para pasar argumentos a los hilos
struct task_args {
    Fragment fragment;       // Fragmento a procesar
    struct completion done;  // Señal de finalización
};

// Función que realiza la operación XOR en el fragmento de datos
int xor_operation(void *arg) {
    struct task_args *targs = (struct task_args *)arg;
    Fragment *fragment = &targs->fragment;
    size_t i;

    // Realiza la operación XOR en el rango especificado
    for (i = fragment->start; i < fragment->end; i++) {
        fragment->data[i] ^= fragment->key[i % fragment->key_size];
    }

    // Completa la tarea
    complete(&targs->done);
    return 0;
}

// Función principal que procesa el archivo
int process_file(const char *input_path, const char *output_path, int num_threads, const char *key_path) {
    struct file *infile, *outfile, *keyfile;
    loff_t offset_in = 0, offset_out = 0, offset_key = 0;
    unsigned char *key, *data;
    size_t input_size, key_size;
    struct task_args *tasks;
    struct task_struct **threads;
    Fragment *fragments;
    size_t fragment_size, remainder;
    int i, ret = 0;

    // Abre el archivo de entrada
    infile = filp_open(input_path, O_RDONLY, 0);
    if (IS_ERR(infile)) {
        ret = PTR_ERR(infile);
        printk(KERN_ERR "Error abriendo el archivo de entrada: %d\n", ret);
        goto out;
    }

    // Abre el archivo de salida
    outfile = filp_open(output_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (IS_ERR(outfile)) {
        ret = PTR_ERR(outfile);
        printk(KERN_ERR "Error abriendo el archivo de salida: %d\n", ret);
        goto close_infile;
    }

    // Abre el archivo de clave
    keyfile = filp_open(key_path, O_RDONLY, 0);
    if (IS_ERR(keyfile)) {
        ret = PTR_ERR(keyfile);
        printk(KERN_ERR "Error abriendo el archivo de clave: %d\n", ret);
        goto close_outfile;
    }

    // Lee el tamaño de la clave
    key_size = i_size_read(file_inode(keyfile));
    if (key_size <= 0) {
        ret = -EINVAL;
        printk(KERN_ERR "Tamaño de clave inválido: %zu\n", key_size);
        goto close_keyfile;
    }

    key = kmalloc(key_size, GFP_KERNEL);
    if (!key) {
        ret = -ENOMEM;
        printk(KERN_ERR "Error al asignar memoria para la clave\n");
        goto close_keyfile;
    }

    // Lee la clave desde el archivo
    ret = kernel_read(keyfile, key, key_size, &offset_key);
    if (ret < 0) {
        printk(KERN_ERR "Error leyendo la clave del archivo: %d\n", ret);
        goto free_key;
    }

    // Lee el tamaño del archivo de entrada
    input_size = i_size_read(file_inode(infile));
    if (input_size <= 0) {
        ret = -EINVAL;
        printk(KERN_ERR "Tamaño de archivo de entrada inválido: %zu\n", input_size);
        goto free_key;
    }

    // Asigna memoria para los datos
    data = kmalloc(input_size, GFP_KERNEL);
    if (!data) {
        ret = -ENOMEM;
        printk(KERN_ERR "Error al asignar memoria para los datos\n");
        goto free_key;
    }

    // Lee los datos del archivo de entrada
    ret = kernel_read(infile, data, input_size, &offset_in);
    if (ret < 0) {
        printk(KERN_ERR "Error leyendo datos del archivo de entrada: %d\n", ret);
        goto free_data;
    }

    // Asigna memoria para los hilos y tareas
    threads = kmalloc(sizeof(struct task_struct *) * num_threads, GFP_KERNEL);
    tasks = kmalloc(sizeof(struct task_args) * num_threads, GFP_KERNEL);
    fragments = kmalloc(sizeof(Fragment) * num_threads, GFP_KERNEL);

    if (!threads || !tasks || !fragments) {
        ret = -ENOMEM;
        printk(KERN_ERR "Error al asignar memoria para hilos o tareas\n");
        goto free_data;
    }

    // Calcula el tamaño de los fragmentos y el resto
    fragment_size = input_size / num_threads;
    remainder = input_size % num_threads;

    // Crea y lanza los hilos
    for (i = 0; i < num_threads; i++) {
        fragments[i].data = data;
        fragments[i].size = input_size;
        fragments[i].key = key;
        fragments[i].key_size = key_size;
        fragments[i].start = i * fragment_size;
        fragments[i].end = (i == num_threads - 1) ? (i + 1) * fragment_size + remainder : (i + 1) * fragment_size;

        tasks[i].fragment = fragments[i];
        init_completion(&tasks[i].done);

        // Inicia el hilo
        threads[i] = kthread_run(xor_operation, &tasks[i], "xor_thread_%d", i);
        if (IS_ERR(threads[i])) {
            ret = PTR_ERR(threads[i]);
            printk(KERN_ERR "Error creando el hilo %d: %d\n", i, ret);
            goto free_all;
        }
    }

    // Espera a que todos los hilos terminen
    for (i = 0; i < num_threads; i++) {
        wait_for_completion(&tasks[i].done);
        printk(KERN_INFO "Hilo %d ha terminado\n", i);
    }

    // Escribe los datos procesados en el archivo de salida
    ret = kernel_write(outfile, data, input_size, &offset_out);
    if (ret < 0) {
        printk(KERN_ERR "Error escribiendo en el archivo de salida: %d\n", ret);
        goto free_all;
    }

free_all:
    kfree(threads);
    kfree(tasks);
    kfree(fragments);

free_data:
    kfree(data);

free_key:
    kfree(key);

close_keyfile:
    filp_close(keyfile, NULL);

close_outfile:
    filp_close(outfile, NULL);

close_infile:
    filp_close(infile, NULL);

out:
    return ret;
}

// Definición de la syscall para encriptar
SYSCALL_DEFINE4(my_encrypt, const char __user *, input_path, const char __user *, output_path, int, num_threads, const char __user *, key_path) {
    char *k_input_path, *k_output_path, *k_key_path;
    int ret;

    printk(KERN_INFO "my_encrypt ejecutando\n");

    // Copia las rutas del espacio del usuario
    k_input_path = strndup_user(input_path, PATH_MAX);
    k_output_path = strndup_user(output_path, PATH_MAX);
    k_key_path = strndup_user(key_path, PATH_MAX);

    if (IS_ERR(k_input_path) || IS_ERR(k_output_path) || IS_ERR(k_key_path)) {
        ret = -EFAULT;
        printk(KERN_ERR "Error con rutas en el espacio del usuario\n");
        goto out_free;
    }

    // Procesa el archivo
    ret = process_file(k_input_path, k_output_path, num_threads, k_key_path);
    if (ret < 0) {
        printk(KERN_ERR "Error en process_file: %d\n", ret);
    }

    printk(KERN_INFO "my_encrypt terminando\n");

out_free:
    kfree(k_input_path);
    kfree(k_output_path);
    kfree(k_key_path);
    return ret;
}

// Definición de la syscall para desencriptar
SYSCALL_DEFINE4(my_decrypt, const char __user *, input_path, const char __user *, output_path, int, num_threads, const char __user *, key_path) {
    char *k_input_path, *k_output_path, *k_key_path;
    int ret;

    printk(KERN_INFO "my_decrypt ejecutando\n");

    // Copia las rutas del espacio del usuario
    k_input_path = strndup_user(input_path, PATH_MAX);
    k_output_path = strndup_user(output_path, PATH_MAX);
    k_key_path = strndup_user(key_path, PATH_MAX);

    if (IS_ERR(k_input_path) || IS_ERR(k_output_path) || IS_ERR(k_key_path)) {
        ret = -EFAULT;
        printk(KERN_ERR "Error con rutas en el espacio del usuario\n");
        goto out_free;
    }

    // Procesa el archivo
    ret = process_file(k_input_path, k_output_path, num_threads, k_key_path);
    if (ret < 0) {
        printk(KERN_ERR "Error en process_file: %d\n", ret);
    }

    printk(KERN_INFO "my_decrypt terminando\n");

out_free:
    kfree(k_input_path);
    kfree(k_output_path);
    kfree(k_key_path);
    return ret;
}


// Llamada al sistema para obtener la información de memoria
SYSCALL_DEFINE1(my_meminfo, struct memory_info __user *, info)
{
    struct memory_info k_info;
    struct sysinfo si;

    // Validar que el puntero info no sea NULL
    if (!info)
        return -EINVAL;  // Error por argumento inválido

    si_meminfo(&si);
    
    k_info.free_memory = si.freeram * PAGE_SIZE;
    k_info.used_memory = (si.totalram - si.freeram) * PAGE_SIZE;
    k_info.cached_memory = global_node_page_state(NR_FILE_PAGES) * PAGE_SIZE;

    // Copiar la información de memoria desde el espacio kernel al espacio usuario
    if (copy_to_user(info, &k_info, sizeof(struct memory_info)))
        return -EFAULT;  // Error de fallo de acceso

    return 0;  // Éxito
}

// Llamada al sistema para obtener la información de swap
SYSCALL_DEFINE1(my_swapinfo, struct swap_info __user *, info)
{
    struct swap_info k_info;
    struct sysinfo si;

    // Validar que el puntero info no sea NULL
    if (!info)
        return -EINVAL;  // Error por argumento inválido

    si_swapinfo(&si);
    
    k_info.free_pages = si.freeswap;
    k_info.used_pages = si.totalswap - si.freeswap;

    // Copiar la información de swap desde el espacio kernel al espacio usuario
    if (copy_to_user(info, &k_info, sizeof(struct swap_info)))
        return -EFAULT;  // Error de fallo de acceso

    return 0;  // Éxito
}

// Llamada al sistema para obtener la información de fallos de página
SYSCALL_DEFINE1(my_page_faults, struct page_fault_info __user *, info)
{
    struct page_fault_info k_info;
    
    k_info.minor_faults = global_node_page_state(PGFAULT);
    k_info.major_faults = global_node_page_state(PGMAJFAULT);

    if (copy_to_user(info, &k_info, sizeof(struct page_fault_info)))
        return -EFAULT;

    return 0;
}

// Llamada al sistema para obtener la información de páginas activas/inactivas
SYSCALL_DEFINE1(my_page_status, struct page_status_info __user *, info)
{
    struct page_status_info k_info;
    
    k_info.active_pages = global_node_page_state(NR_ACTIVE_ANON) +
                         global_node_page_state(NR_ACTIVE_FILE);
    k_info.inactive_pages = global_node_page_state(NR_INACTIVE_ANON) +
                           global_node_page_state(NR_INACTIVE_FILE);

    if (copy_to_user(info, &k_info, sizeof(struct page_status_info)))
        return -EFAULT;

    return 0;
}

SYSCALL_DEFINE1(my_top_processes, struct top_processes __user *, info)
{
    struct top_processes k_info;
    struct task_struct *task;
    struct process_memory_info *temp_processes;
    int count = 0;
    int i, j;

    // Allocar memoria para el array temporal
    temp_processes = kmalloc(sizeof(struct process_memory_info) * 1024, GFP_KERNEL);
    if (!temp_processes)
        return -ENOMEM;

    // Recolectar información de todos los procesos
    rcu_read_lock();
    for_each_process(task) {
        if (count >= 1024)
            break;
            
        if (task->mm) {  // Solo procesos con espacio de memoria
            temp_processes[count].pid = task->pid;
            strncpy(temp_processes[count].comm, task->comm, TASK_COMM_LEN - 1);
            temp_processes[count].comm[TASK_COMM_LEN - 1] = '\0';
            temp_processes[count].rss = get_mm_rss(task->mm) * PAGE_SIZE;
            count++;
        }
    }
    rcu_read_unlock();

    // Ordenar por uso de memoria (bubble sort)
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (temp_processes[j].rss < temp_processes[j + 1].rss) {
                struct process_memory_info temp = temp_processes[j];
                temp_processes[j] = temp_processes[j + 1];
                temp_processes[j + 1] = temp;
            }
        }
    }

    // Copiar los primeros 5 procesos
    memset(&k_info, 0, sizeof(struct top_processes));
    for (i = 0; i < 5 && i < count; i++) {
        k_info.processes[i] = temp_processes[i];
    }

    // Liberar memoria temporal
    kfree(temp_processes);

    if (copy_to_user(info, &k_info, sizeof(struct top_processes)))
        return -EFAULT;

    return 0;
}
