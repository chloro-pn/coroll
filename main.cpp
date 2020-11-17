#include "schedule.h"
#include "task.h"
#include <thread>
#include "futex_wrapper.h"
#include <iostream>

Task* global_task = nullptr;

void func2(TaskArg arg) {
    std::cout << "task : " << arg.self->GetTaskId() << std::endl;
    std::cout << "suspend" << std::endl;
    global_task = arg.self;
    arg.self->Suspend();
    std::cout << "be waked " << std::endl;
}

void func(TaskArg arg) {
    std::cout << "hello world" << std::endl;
    Schedule::instance().CreateTask(func2, nullptr, arg.self->GetTaskId());
    std::cout << "after create task" << std::endl;
}

int main() {
    std::thread th([]() {
        sleep(2);
        global_task->MakeRunnable();
    });
    Schedule::instance().CreateTask(func, nullptr, 0);
    Schedule::instance().Run();
    th.join();
    return 0;
}
