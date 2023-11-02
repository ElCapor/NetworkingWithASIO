// Net Thread-Safe Queue
#pragma once
#include <mutex>
#include <mutex>
#include <mutex>
#include <mutex>
#include <mutex>

#include "NetIncludes.h"


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

    protected:
        std::mutex mx_Queue;
        // double ended queue
        std::deque<T> deq_Queue;
    };
}
