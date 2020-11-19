#ifndef FUTEX_WRAPPER_H
#define FUTEX_WRAPPER_H

#include <linux/futex.h>
#include <syscall.h>
#include <unistd.h>

void FutexWait(void* uaddr, int expected);

void FutexWake(void* uaddr);

#endif // FUTEX_WRAPPER_H
