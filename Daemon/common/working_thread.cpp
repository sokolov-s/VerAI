#include "working_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>    /* For SYS_xxx definitions */
#include <sys/resource.h>

using namespace common;
using namespace std;

void WorkingThread::Run()
{
    if (IsRunning()) return;
    thrWork = thread(&WorkingThread::Runner, this);
}

WorkingThread::WorkingThread()
    : isRunning(false)
{
}

WorkingThread::~WorkingThread()
{
    Stop();
    thrWork.join();
}

void WorkingThread::EnqueueTask(shared_ptr<IRunnable> task)
{
    taskQueue.Push(task);
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