#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <fcontext.h>
#include <cstdint>
#include <atomic>
#include <vector>
#include <unordered_map>
#include "task.h"

class Schedule {
public:
    static Schedule& instance();

    Schedule();

    void ScheTo(Task* task);

    void ScheToMain(Task* task);

    uint64_t CreateTask(Task::FuncType func, void* arg, uint64_t from_id);

    bool Resume(uint64_t id);

    void Run();

    void WakeUp();

    void Wait();

private:
    std::atomic<int> flag_;
    std::unordered_map<uint64_t, Task> tasks_;
    std::vector<Task> tmp_tasks_;
};

#endif // SCHEDULE_H
