#ifndef TASK_H
#define TASK_H

#include <ucontext.h>
#include <functional>

enum class TaskState {RUNNABLE, SUSPEND, OVER};

void MyWork(void* arg);

class Task;

struct TaskArg {
    void* arg;
    Task* self;
};

class Task {
public:
    using FuncType = std::function<void(TaskArg)>;

    Task();

    Task(FuncType func, void* arg);

    Task(const Task& other) = delete;
    Task& operator=(const Task& other) = delete;
    Task(Task&& other);
    Task& operator=(Task&& other);

    ~Task();

    uint64_t GetTaskId() const {
        return id_;
    }

    void Suspend();

    void MakeRunnable();

private:
    friend void MyWork(void* arg);
    friend class Schedule;
    uint64_t id_;
    ucontext_t ctx_;
    char* stack_;
    TaskState state_;
    FuncType func_;
    TaskArg arg_;
};

#endif // TASK_H