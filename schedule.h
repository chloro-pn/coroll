#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <ucontext.h>
#include <cstdint>
#include <atomic>
#include <vector>
#include <unordered_map>
#include "task.h"

class Schedule {
public:
    ucontext_t& GetUContext() {
        return schedule_;
    }

    static Schedule& instance();

    Schedule();

    void ScheTo(uint64_t id);

    void ScheToMain(uint64_t from_id);

    void CreateTask(Task::FuncType func, void* arg, uint64_t from_id);

    void Run();

    void WakeUp();

    void Wait();

private:
    ucontext_t schedule_;
    std::atomic<int> flag_;
    std::unordered_map<uint64_t, Task> tasks_;
    std::vector<Task> tmp_tasks_;
};

#endif // SCHEDULE_H
