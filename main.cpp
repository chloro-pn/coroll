#include "schedule.h"
#include "task.h"
#include <thread>
#include "futex_wrapper.h"
#include <iostream>

void func(TaskArg arg) {
    int num = *(int*)arg.arg;
    std::cout << "hello world, my arg is " << num << std::endl;
    sleep(1);
    Schedule::instance().CreateTask(func, new int(num + 1), arg.self->GetTaskId());
    std::cout << "after create task" << std::endl;
}

int main() {
    Schedule::instance().CreateTask(func, new int(0), 0);
    Schedule::instance().Run();
    return 0;
}
