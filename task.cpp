#include "task.h"
#include "id_alloc.h"
#include "schedule.h"
#include "futex_wrapper.h"
#include <cassert>

// 不需要显式切换到schedule，通过uc_link。
void MyWork(void* arg) {
    Task* self = static_cast<Task*>(arg);
    assert(self->state_ == TaskState::RUNNABLE);
    self->func_(self->arg_);
    self->state_ = TaskState::OVER;
}

void DoNothing(TaskArg arg) {

}

Task::Task():id_(0), stack_(nullptr), state_(TaskState::RUNNABLE), func_(DoNothing) {

}

Task::Task(FuncType func, void* arg) : id_(0),
                                       stack_(nullptr),
                                       state_(TaskState::RUNNABLE),
                                       func_(func) {
    id_ = IdAlloc::instance().GetId();
    stack_ = new char[128 * 1024];
    getcontext(&ctx_);
    ctx_.uc_stack.ss_flags = 0;
    ctx_.uc_stack.ss_sp = stack_;
    ctx_.uc_stack.ss_size = 128 * 1024;
    ctx_.uc_link = &Schedule::instance().GetUContext();
    makecontext(&ctx_, (void(*)(void))MyWork, 1, this);
    arg_ = {arg, this};
}

Task::Task(Task&& other) {
    id_ = other.id_;
    other.id_ = 0;
    ctx_ = other.ctx_;
    makecontext(&ctx_, (void(*)(void))MyWork, 1, this);
    stack_ = other.stack_;
    other.stack_ = nullptr;
    state_ = other.state_;
    func_ = std::move(other.func_);
    arg_ = {other.arg_.arg, this};
}

Task& Task::operator=(Task &&other) {
    id_ = other.id_;
    other.id_ = 0;
    ctx_ = other.ctx_;
    makecontext(&ctx_, (void(*)(void))MyWork, 1, this);
    delete stack_;
    stack_ = other.stack_;
    other.stack_ = nullptr;
    state_ = other.state_;
    func_ = std::move(other.func_);
    arg_ = {other.arg_.arg, this};
}

Task::~Task() {
    delete stack_;
}

// 必须在协程内部调用
void Task::Suspend() {
    if(state_ != TaskState::RUNNABLE) {
        return;
    }
    state_ = TaskState::SUSPEND;
    Schedule::instance().ScheToMain(id_);
}

void Task::MakeRunnable() {
    if(state_ != TaskState::SUSPEND) {
        return;
    }
    state_ = TaskState::RUNNABLE;
    FutexWake();
}
