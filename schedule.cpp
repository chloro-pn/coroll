#include "schedule.h"
#include "futex_wrapper.h"
#include <cassert>

Schedule& Schedule::instance() {
    static Schedule obj;
    return obj;
}

#define FLAG_INIT 0

Schedule::Schedule():flag_(FLAG_INIT) {

}

void Schedule::ScheTo(Task* task) {
    fcontext_transfer_t t = jump_fcontext(task->context_->context, task);
    task->context_->context = t.prev_context;
}

void Schedule::ScheToMain(Task* task) {
    assert(task->state_ == SUSPEND || task->state_ == OVER);
    fcontext_transfer_t t = jump_fcontext(task->prev_context_, nullptr);
    task->prev_context_ = t.prev_context;
}

uint64_t Schedule::CreateTask(Task::FuncType func, void* arg, uint64_t from_id) {
    Task tmp(this, func, arg);
    uint64_t result = tmp.GetTaskId();
    assert(tasks_.find(tmp.GetTaskId()) == tasks_.end());
    if(from_id != 0) {
        tmp_tasks_.push_back(std::move(tmp));
    }
    else {
        tasks_[tmp.GetTaskId()] = std::move(tmp);
    }
    return result;
}

// should be thread-safe.
bool Schedule::Resume(uint64_t id) {
    auto it = tasks_.find(id);
    if(it == tasks_.end()) {
        return false;
    }
    it->second.Resume();
}

void Schedule::Run() {
    while(true) {
        for(auto it = tasks_.begin(); it != tasks_.end();) {
            int state = it->second.state_.load();
            if(state == OVER) {
                it = tasks_.erase(it);
            }
            else {
                if(state == RUNNABLE) {
                    ScheTo(&it->second);
                    //从这个协程切换回调度器
                    if(it->second.state_ == OVER) {
                        it = tasks_.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
                else {
                    ++it;
                }
            }
        }
        if(tmp_tasks_.empty() == false) {
            for(auto& each : tmp_tasks_) {
                uint64_t id = each.GetTaskId();
                tasks_[id] = std::move(each);
            }
            tmp_tasks_.clear();
        }
        else {
            if(tasks_.empty() == true) {
                break;
            }
            //TODO:WAIT_FROM_THERE.
            Wait();
        }
    }
}

void Schedule::WakeUp() {
    flag_.fetch_add(1);
    FutexWake(&flag_);
}

void Schedule::Wait() {
    static int last_state_{FLAG_INIT};
    FutexWait(&flag_, last_state_);
    last_state_ = flag_.load();
}
