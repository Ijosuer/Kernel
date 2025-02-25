#ifndef _SYSCALLS_USAC_H_
#define _SYSCALLS_USAC_H_
#include <linux/linkage.h>
#include <linux/types.h>
#include <linux/kernel.h>


// Estructura para almacenar información de memoria
struct memory_info {
    unsigned long free_memory;
    unsigned long used_memory;
    unsigned long cached_memory;
};

// Estructura para información de swap
struct swap_info {
    unsigned long free_pages;
    unsigned long used_pages;
};


// Estructura para fallos de página
struct page_fault_info {
    unsigned long minor_faults;
    unsigned long major_faults;
};

// Estructura para páginas activas/inactivas
struct page_status_info {
    unsigned long active_pages;
    unsigned long inactive_pages;
};

// Estructura para información de proceso
struct process_memory_info {
    pid_t pid;
    char comm[TASK_COMM_LEN];
    unsigned long rss;
};

// Estructura para top procesos
struct top_processes {
    struct process_memory_info processes[5];
};


// Definición de las syscalls
asmlinkage long sys_my_encrypt(const char __user *input_path, const char __user *output_path, int num_threads, const char __user *key_path);
asmlinkage long sys_my_decrypt(const char __user *input_path, const char __user *output_path, int num_threads, const char __user *key_path);

// Syscall proyecto
asmlinkage long sys_my_meminfo(struct memory_info __user *info);
asmlinkage long sys_my_swapinfo(struct swap_info __user *info);
asmlinkage long sys_my_page_faults(struct page_fault_info __user *info);
asmlinkage long sys_my_page_status(struct page_status_info __user *info);
asmlinkage long sys_my_top_processes(struct top_processes __user *info);


#endif /* _LINUX_SYSCALLS_USAC_H */