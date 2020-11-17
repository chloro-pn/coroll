#ifndef FUTEX_WRAPPER_H
#define FUTEX_WRAPPER_H

#include <linux/futex.h>
#include <syscall.h>
#include <unistd.h>

extern int* uaddr;

void FutexWait();

void FutexWake();

#endif // FUTEX_WRAPPER_H
