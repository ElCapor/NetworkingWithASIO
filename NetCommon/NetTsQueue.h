// Net Thread-Safe Queue
#pragma once
#include "NetCommon.h"


namespace net
{
    // Thread-Safe Queue implementation
    // basically we're creating a thread-safe double ended queue
    template <typename T>
    class ts_queue
    {
    public:
        ts_queue() = default;
        ts_queue(const ts_queue&) = delete;
        virtual ~ts_queue() {clear();}

    public:
        // return the front 
        const T& front()
        {
            // acquire ownership of the mutex
            std::scoped_lock lock(mx_Queue);
            return deq_Queue.front();
        }

        //return the back
        const T& back()
        {
            std::scoped_lock lock(mx_Queue);
            return deq_Queue.back();
        }

        // return the number of elements
        size_t count()
        {
            std::scoped_lock lock(mx_Queue);
            return deq_Queue.size();
        }

        // clear the queue
        void clear()
        {
            std::scoped_lock lock(mx_Queue);
            return deq_Queue.clear();
        }

        // remove the item and return it from the front
        T pop_front()
        {
            std::scoped_lock lock(mx_Queue);
            // move the reference to our object, we own it now
            auto t = std::move(deq_Queue.front());
            // the standard implementation only removes it but doesnt return it
            deq_Queue.pop_front();
            return t;
        }

        // remove the last item and return it from the back
        T pop_back()
        {
            std::scoped_lock lock(mx_Queue);
            // move the reference
            auto t = std::move(deq_Queue.back());

            deq_Queue.pop_back();

            return t;
        }

        void push_back(const T& item)
        {
            std::scoped_lock lock(mx_Queue);
            // move the reference back to the queue
            deq_Queue.emplace_back(std::move(item));

            std::unique_lock<std::mutex> unique_lock(mxblock_queue);
            cv_queue.notify_one(); // we finished modifying it , now the other threads can edit it
        }

        void push_front(const T& item)
        {
            std::scoped_lock lock(mx_Queue);
            deq_Queue.emplace_front(std::move(item));

            std::unique_lock<std::mutex> unique_lock(mxblock_queue);
            cv_queue.notify_one();
        }

        bool empty()
        {
            std::scoped_lock lock(mx_Queue);
            return deq_Queue.empty();
        }
        

        // wait for any input/output
        
        void wait()
        {
            /*
            Any thread that intends to wait on a std::condition_variable must:

            Acquire a std::unique_lock<std::mutex> on the mutex used to protect the shared variable.
            Do one of the following: 

            1 - Check the condition, in case it was already updated and notified.
            2 - Call wait, wait_for, or wait_until on the std::condition_variable (atomically releases the mutex and suspends thread execution until the condition variable is notified, a timeout expires, or a spurious wakeup occurs, then atomically acquires the mutex before returning).
            3- Check the condition and resume waiting if not satisfied. 
             */
            while (empty())
            {
                std::unique_lock<std::mutex> unique_lock(mxblock_queue);
                cv_queue.wait(unique_lock);
            }
        }
        

    protected:
        std::mutex mx_Queue;
        // double ended queue
        std::deque<T> deq_Queue;

        /*
        The condition_variable class is a synchronization primitive used with a std::mutex to block one or more threads until another thread both modifies a shared variable (the condition) and notifies the condition_variable.

        The thread that intends to modify the shared variable must:

        Acquire a std::mutex (typically via std::lock_guard).
        Modify the shared variable while the lock is owned.
        Call notify_one or notify_all on the std::condition_variable (can be done after releasing the lock).
        */
        std::condition_variable cv_queue;

        // this mutex is used to hold on push_front and push_back
        std::mutex mxblock_queue;
    };
}
