#include "futex_wrapper.h"

void FutexWait(void* uaddr, int excepted) {
    syscall(SYS_futex, uaddr, (FUTEX_WAIT | FUTEX_PRIVATE_FLAG),
                   excepted, nullptr, nullptr, 0);
}

void FutexWake(void* uaddr) {
    syscall(SYS_futex, uaddr, (FUTEX_WAKE | FUTEX_PRIVATE_FLAG),
                   1, nullptr, nullptr, 0);
}
