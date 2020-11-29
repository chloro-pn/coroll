#include "task.h"
#include "id_alloc.h"
#include "schedule.h"
#include "futex_wrapper.h"
#include <cassert>

void MyWork(fcontext_transfer_t t) {
    Task* self = static_cast<Task*>(t.data);
    int state = self->state_.load();
    assert(state == RUNNABLE);
    self->prev_context_ = t.prev_context;
    self->func_(self->arg_);
    self->state_.store(OVER);
    Schedule::instance().ScheToMain(self);
}

void DoNothing(TaskArg arg) {

}

Task::Task():sche_(nullptr),
    id_(0),
    state_(RUNNABLE),
    func_(DoNothing),
    context_(nullptr),
    prev_context_(nullptr) {

}

Task::Task(Schedule* sche, FuncType func, void* arg) : sche_(sche),
                                       id_(0),
                                       context_(nullptr),
                                       prev_context_(nullptr),
                                       state_(RUNNABLE),
                                       func_(func) {
    id_ = IdAlloc::instance().GetId();
    context_ = fcontext_create(MyWork);
    arg_ = {arg, this};
}

Task::Task(Task&& other) {
    sche_ = other.sche_;
    id_ = other.id_;
    other.id_ = 0;
    context_ = other.context_;
    other.context_ = nullptr;
    prev_context_ = other.prev_context_;
    other.prev_context_ = nullptr;
    state_.store(other.state_.load());
    func_ = std::move(other.func_);
    arg_ = {other.arg_.arg, this};
}

Task& Task::operator=(Task &&other) {
    sche_ = other.sche_;
    id_ = other.id_;
    other.id_ = 0;
    context_ = other.context_;
    other.context_ = nullptr;
    prev_context_ = other.prev_context_;
    other.prev_context_ = nullptr;
    state_.store(other.state_.load());
    func_ = std::move(other.func_);
    arg_ = {other.arg_.arg, this};
    return *this;
}

Task::~Task() {
    if(context_ == nullptr) {
        return;
    }
    fcontext_destroy(context_);
}

// 必须在协程内部调用
void Task::Suspend() {
    if(state_ != RUNNABLE) {
        return;
    }
    state_ = SUSPEND;
    sche_->ScheToMain(this);
}

void Task::Resume() {
    int state = state_.load();
    if(state != SUSPEND) {
        return;
    }
    state_.store(RUNNABLE);
    sche_->WakeUp();
}
