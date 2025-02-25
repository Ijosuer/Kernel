#ifndef _SYSCALLS_USAC_H_
#define _SYSCALLS_USAC_H_
#include <linux/linkage.h>
#include <linux/types.h>
#include <linux/kernel.h>

// Definición de las syscalls
asmlinkage long sys_my_encrypt(const char __user *input_path, const char __user *output_path, int num_threads, const char __user *key_path);
asmlinkage long sys_my_decrypt(const char __user *input_path, const char __user *output_path, int num_threads, const char __user *key_path);

#endif /* _LINUX_SYSCALLS_USAC_H */