#ifndef WORKING_THREAD_H
#define WORKING_THREAD_H

#include <memory>
#include <thread>
#include <atomic>

#include "message_queue.h"

namespace common {

class IRunnable
{
public:
    virtual void Run() = 0;
    virtual ~IRunnable(){}
};

template <typename F> class TaskWrapper: public IRunnable
{
    F fn;
public:
    TaskWrapper(const F& f): fn(f){}
    virtual void Run()
    {
        fn();
    }

};

/**
 * @brief Function creates runable class for you function to add it into the task's queue.
 */
template <typename F>
std::shared_ptr<IRunnable> WrapTask(const F& f)
{
    return std::shared_ptr<IRunnable>(new TaskWrapper<F>(f));
}

/**
 * @brief The WorkingThread class. It is the thread safe class for running functions from pool.
 * Class creates the queue and starts own thread to running functions from the queue.
 * To start working you need to call function Run() at once.
 * Class does not have piority for jobs and runs functions sequentially
 */
class WorkingThread
{
    WorkingThread(const WorkingThread&);
    WorkingThread& operator=(const WorkingThread&);
public:
    WorkingThread();
    ~WorkingThread();
    
    /**
     * @brief Run - creates working thread to getting tasks from queue and calling it;
     */
    void Run();
    
    /**
     * @brief EnqueueTask - add task into the queue
     * @param task - runnable class to add into the queue.
     */
    void EnqueueTask(std::shared_ptr<IRunnable> task);

    /**
     * @brief EnqueueTaskAfterTimeOut - add task into the main task queue after some time
     * @param task - runnable class to add into the queue.
     * @param ms - time in milliseconds to wait before task will be added to main queue
     */
    void EnqueueTaskAfterTimeOut(std::shared_ptr<IRunnable> task, unsigned int ms);
    
    /**
     * @brief EnqueueTaskFront - add task into beginning of queue
     * @param task - runnable class to add into the queue.
     */
    void EnqueueTaskFront(std::shared_ptr<IRunnable> task);
    
    /**
     * @brief Stop - stop working thread.
     * Function does not stop thread immediately.
     * If there are some tasks in queue - working thread runs all tasks from queue and then stops working.
     * To stop working immediately call ClearQueueTasks before function Stop
     */
    void Stop();
    
    /**
     * @brief TasksQueueEmpty
     * @return true if task's queue is empty or false in otherwise
     */
    bool TasksQueueEmpty();
    
    /**
     * @brief IsRunning returns information about working thread status
     * @return true if working thread is running or false in otherwise
     */
    bool IsRunning();
    
    /**
     * @brief ClearQueueTasks removes all jobs from queue
     */
    void ClearQueueTasks();
    
    /**
     * @brief SetMeLowPriority sets working thread priority.
     * This function should be called from working thread. You need to add this function into the working thread by EnqueueTask function.
     * In otherwise it sets priority for current thread in your application
     * @param priority can be from 1 to 19. 19 - maximum slowly
     */
    static void SetMeLowPriority(int priority);
private:
    void DefferedTasksHandler();
private:
    static void* Runner(void* context);
    int Work(void);
    std::atomic<bool> isRunning;
    MessageQueue<std::shared_ptr<IRunnable>> taskQueue;
    std::thread workThr;
    std::thread defferedTasksThr;
    std::condition_variable defferedTasksCnd;
    std::mutex defferedTasksMtx;
    struct DefferedTask {
        DefferedTask(std::shared_ptr<IRunnable> task, const unsigned int millisec) : taskToRun(task), ms(millisec) {}
        std::shared_ptr<IRunnable> taskToRun;
        unsigned int ms = 0;
        bool operator<(const DefferedTask &rhs) const {
            return std::tie(ms) < std::tie(rhs.ms);
        }
    };
    std::vector<DefferedTask> defferedTasksList;
    unsigned int updateDefferedTasksMS = 500;
};

} // namespace common



#endif // WORKING_THREAD_H
