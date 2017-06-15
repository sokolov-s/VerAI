#include "working_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>    /* For SYS_xxx definitions */
#include <sys/resource.h>
#include <algorithm>

using namespace common;
using namespace std;

void WorkingThread::Run()
{
    if (IsRunning()) return;
    workThr = thread(&WorkingThread::Runner, this);
    defferedTasksThr = std::thread(&WorkingThread::DefferedTasksHandler, this);
}

WorkingThread::WorkingThread()
    : isRunning(false)
{
}

WorkingThread::~WorkingThread()
{
    Stop();
    workThr.join();
    defferedTasksThr.join();
}

void WorkingThread::EnqueueTask(shared_ptr<IRunnable> task)
{
    taskQueue.Push(task);
}

void WorkingThread::EnqueueTaskAfterTimeOut(std::shared_ptr<IRunnable> task, unsigned int ms)
{
    std::unique_lock<std::mutex> locker(defferedTasksMtx);
    defferedTasksList.push_back(DefferedTask(task, ms));
    defferedTasksCnd.notify_one();
}

void WorkingThread::EnqueueTaskFront(shared_ptr<IRunnable> task)
{
    taskQueue.PushFront(task);
}

void* WorkingThread::Runner(void* context)
{
    WorkingThread* this_ = reinterpret_cast<WorkingThread*>(context);
    this_->Work();
    return NULL;
}

int WorkingThread::Work(void)
{
    this->isRunning.store(true);
    
    while (IsRunning())
    {
        deque<shared_ptr<IRunnable>> deq;
        taskQueue.WaitAndReturnAll(100, &deq);
        while(!deq.empty()) {
            shared_ptr<IRunnable> current_task(deq.front());
            deq.pop_front();
            if (current_task) {
                current_task->Run();
            }
        }
    }
    while(!taskQueue.IsEmpty()) {
        shared_ptr<IRunnable> current_task;
        taskQueue.Pop(&current_task);
        if (current_task) {
            current_task->Run();
        }
    }
    this->isRunning.store(false);
    return 0;
}

void WorkingThread::Stop()
{
    this->isRunning.store(false);
    taskQueue.Push(shared_ptr<IRunnable>());
    if(defferedTasksThr.joinable()) {
        std::unique_lock<std::mutex> locker(defferedTasksMtx);
        defferedTasksList.clear();
        defferedTasksCnd.notify_all();
    }
}

bool WorkingThread::TasksQueueEmpty()
{
    return taskQueue.IsEmpty();
}

bool WorkingThread::IsRunning()
{
    return isRunning.load();
}

void WorkingThread::ClearQueueTasks()
{
    taskQueue.Clear();
}

void WorkingThread::SetMeLowPriority(int priority)
{
    pid_t tid;
    tid = syscall(SYS_gettid);

    if (getpriority(PRIO_PROCESS, tid) != priority) {
        int err = setpriority(PRIO_PROCESS, tid, priority);
        if (err) {
            fprintf(stderr, "FAILED to setpriority\n");
        }
    }
}

void WorkingThread::DefferedTasksHandler()
{
    std::unique_lock<std::mutex> locker(defferedTasksMtx);
    unsigned int nextWait = updateDefferedTasksMS;
    bool isFirstWait = true;
    while(IsRunning()) {
        auto t1 = std::chrono::high_resolution_clock::now();
        defferedTasksCnd.wait_for(locker, std::chrono::milliseconds(nextWait));
        if(defferedTasksList.empty())
            continue;
        std::qsort(defferedTasksList.data(), defferedTasksList.size(), sizeof(DefferedTask),
                   [](const void * a, const void * b)->int {
                       return (*(DefferedTask*)a).ms - (*(DefferedTask*)b).ms;
                    });
        if(isFirstWait) {
            isFirstWait = false;
        } else {
            auto t2 = std::chrono::high_resolution_clock::now();
            auto curWait = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
            auto it = defferedTasksList.begin();
            while(it != defferedTasksList.end()) {
                if(((*it).ms - curWait.count()) <= 0) {
                    EnqueueTask((*it).taskToRun);
                    it = defferedTasksList.erase(it);
                } else {
                    (*it).ms -= curWait.count();
                    ++it;
                }
            }
        }
        if(!defferedTasksList.empty()) {
            nextWait = defferedTasksList.at(0).ms;
        } else {
            isFirstWait = true;
            nextWait = updateDefferedTasksMS;
        }
    }
}
