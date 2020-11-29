#ifndef TASK_H
#define TASK_H

#include "fcontext.h"
#include <functional>
#include <atomic>

#define RUNNABLE 0
#define SUSPEND 1
#define OVER 2

//enum class TaskState {RUNNABLE, SUSPEND, OVER};

void MyWork(fcontext_transfer_t t);

class Task;
class Schedule;

struct TaskArg {
    void* arg;
    Task* self;
};

class Task {
public:
    using FuncType = std::function<void(TaskArg)>;

    Task();

    Task(Schedule* sche, FuncType func, void* arg);

    Task(const Task& other) = delete;
    Task& operator=(const Task& other) = delete;
    Task(Task&& other);
    Task& operator=(Task&& other);

    ~Task();

    uint64_t GetTaskId() const {
        return id_;
    }

    void Suspend();

    void Resume();

private:
    friend void MyWork(fcontext_transfer_t t);
    friend class Schedule;
    Schedule* sche_;
    uint64_t id_;
    fcontext_state_t* context_;
    fcontext_t prev_context_;
    std::atomic<int> state_;
    FuncType func_;
    TaskArg arg_;
};

#endif // TASK_H
