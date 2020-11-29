#include "schedule.h"
#include "task.h"
#include <time.h>
#include "futex_wrapper.h"
#include <cstdio>
#include <thread>

struct timespec t1;
struct timespec t2;

void func2(TaskArg arg) {
    clock_gettime(CLOCK_MONOTONIC, &t2);
    printf("%ld\n", t2.tv_nsec - t1.tv_nsec);
}

void func(TaskArg arg) {
    clock_gettime(CLOCK_MONOTONIC, &t1);
    printf("func call.\n");
    Schedule::instance().CreateTask(func2, nullptr, arg.self->GetTaskId());
    arg.self->Suspend();
    printf("after create task\n");
}

int main() {
    uint64_t id = Schedule::instance().CreateTask(func, new int(0), 0);
    std::thread th([id]() {
        sleep(1);
        Schedule::instance().Resume(id);
    });
    Schedule::instance().Run();
    th.join();
    return 0;
}
