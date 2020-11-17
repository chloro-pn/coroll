#include "futex_wrapper.h"

int* uaddr = new int(0);

void FutexWait() {
    syscall(SYS_futex, uaddr, (FUTEX_WAIT | FUTEX_PRIVATE_FLAG),
                   0, nullptr, nullptr, 0);
}

void FutexWake() {
    syscall(SYS_futex, uaddr, (FUTEX_WAKE | FUTEX_PRIVATE_FLAG),
                   1, NULL, NULL, 0);
}
