#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <queue>
#include <chrono>

namespace common {

template<typename T> class IPushable
{
public:
    virtual void Push(const T& item) = 0;
    virtual void PushFront(const T& item) = 0;

    virtual ~IPushable(){}
};

template<typename T>
class MessageQueue: public IPushable<T>
{
public:
    MessageQueue(){}
    ~MessageQueue()
    {
    }

    void Push(const T& item)
    {
        std::unique_lock<std::mutex> lock(mtx);
        queueMessages.push_back(item);
        condVar.notify_all();
    }

    void PushFront(const T& item)
    {
        std::unique_lock<std::mutex> lock(mtx);
        queueMessages.push_front(item);
        condVar.notify_all();
    }

    bool IsEmpty(void)
    {
        std::lock_guard<std::mutex> lock(mtx);
        return queueMessages.empty();
    }
    
    void Pop(T* item)
    {
        if(!item){
            throw std::invalid_argument("Argument can't be NULL");
        }
        std::lock_guard<std::mutex> lock(mtx);
        if (!queueMessages.empty()) {
            *item = queueMessages.front();
            queueMessages.pop_front();
        } else {
            throw std::runtime_error("Can't pop queue");
        }
    }
    
    /**
     * @brief Wait - Call this function to wait, until some items will be added into the queue. 
     * Current process/thread will be stopped. 
     * @param timeout_ms - You can set timeout to wake up process/thread for checking current status of deque
     * @param item - Last element of queue will be moved into this pointer. 
     *        If "item" is empty function throws std::invalid_argument exception
     */
    void Wait(long timeout_ms, T *item)
    {
        if(!item){
            throw std::invalid_argument("Argument can't be NULL");
        }
        std::unique_lock<std::mutex> lock(mtx);
        while(queueMessages.empty()) {
            if (timeout_ms) {
                condVar.wait_for(lock, std::chrono::milliseconds(timeout_ms));
            } else {
                condVar.wait(lock);
            }
        }
        *item = queueMessages.front();
        queueMessages.pop_front();
    }
    
    /**
     * @brief WaitAndReturnAll - Call this function to wait, until some item/tems will be added into the queue. 
     *        Current process/thread will be stopped.
     * @param timeout_ms - Timeout to wake up process/thread for checking current status of deque
     * @param deq - All working deque will be moved into this pointer
     *        If "deq" is empty function throws std::invalid_argument exception
     */
    void WaitAndReturnAll(long timeout_ms, std::deque<T> *deq)
    {
        if(!deq){
            throw std::invalid_argument("Argument can't be NULL");
        }
        std::unique_lock<std::mutex> lock(mtx);
        while(queueMessages.empty()) {
            condVar.wait_for(lock, std::chrono::milliseconds(timeout_ms));
        }
        deq->clear();
        queueMessages.swap(*deq);
    }
    
    /**
     * @brief Clear - Function remove all elements from working deque
     */
    void Clear() {
        std::lock_guard<std::mutex> lock(mtx);
        queueMessages.clear();
    }

private:
    std::deque<T> queueMessages;
    std::mutex mtx;
    std::condition_variable condVar;
};

} // namespace utilities

#endif // MESSAGE_QUEUE_H
