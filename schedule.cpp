#include "schedule.h"
#include "futex_wrapper.h"
#include <cassert>

Schedule& Schedule::instance() {
    static Schedule obj;
    return obj;
}

#define FLAG_INIT

Schedule::Schedule():flag_(FLAG_INIT) {
    getcontext(&schedule_);
}

void Schedule::ScheTo(uint64_t id) {
    auto it = tasks_.find(id);
    assert(it != tasks_.end());
    assert(it->second.state_ == RUNNABLE);
    swapcontext(&schedule_, &it->second.ctx_);
}

void Schedule::ScheToMain(uint64_t from_id) {
    auto it = tasks_.find(from_id);
    assert(it != tasks_.end());
    assert(it->second.state_ == SUSPEND);
    swapcontext(&it->second.ctx_, &schedule_);
}

void Schedule::CreateTask(Task::FuncType func, void* arg, uint64_t from_id) {
    Task tmp(func, arg);
    assert(tasks_.find(tmp.GetTaskId()) == tasks_.end());
    if(from_id != 0) {
        tmp_tasks_.push_back(std::move(tmp));
        auto it = tasks_.find(from_id);
        assert(it != tasks_.end());
        assert(it->second.state_ == RUNNABLE);
        swapcontext(&it->second.ctx_, &schedule_);
    }
    else {
        tasks_[tmp.GetTaskId()] = std::move(tmp);
    }
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
                    ScheTo(it->second.GetTaskId());
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
