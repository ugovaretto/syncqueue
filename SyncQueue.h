#pragma once
//Author: Ugo Varetto

//! \file SyncQueue.h
//! \brief Synchronized queue
//!
//! Implementation of a synchronized queue.

#include <deque>
#include <mutex>
#include <condition_variable>

//! Synchronized queue:
//! @c Pop() waits for data
template<typename T>
class SyncQueue {
public:
    //! Push data to back of the queue; if a temporary (rvalue ref)
    //! is passed then the data is moved into the internal `std::deque`
    //! instance.
    void Push(T&& e) {
        std::lock_guard<std::mutex> guard(mutex_);
        queue_.push_back(std::forward< T >(e));
        cond_.notify_one(); //notify
    }
    //! Push data to back of the queue.
    void Push(const T& e) {
        std::lock_guard< std::mutex > guard(mutex_);
        queue_.push_back(e);
        cond_.notify_one(); //notify
    }
    //! Push data to front of queue.
    //! Used to add a high piority message, normally to signal
    //! end of operations
    void PushFront(const T& e) {
        std::lock_guard< std::mutex > guard(mutex_);
        queue_.push_front(e);
        cond_.notify_one(); //notify
    }
    //! Push data to front of queue; supports move from temporary
    //! Used to add a high piority message, normally to signal
    //! end of operations
    void PushFront(T&& e) {
        std::lock_guard< std::mutex > guard(mutex_);
        queue_.push_front(std::forward< T >(e));
        cond_.notify_one(); //notify
    }
    //! Add elementes in [begin, end) interval to queue
    //! in a single atomic operation.
    template<typename FwdT>
    void Buffer(FwdT begin, FwdT end) {
        std::lock_guard< std::mutex > guard(mutex_);
        while(begin++ != end) queue_.push_back(*begin);
        cond_.notify_one();
    }
    //! Return and remove element in front of queue.
    //! Waits indefinitely for an element to be available.
    T Pop() {
        std::unique_lock< std::mutex > lock(mutex_);
        //stop and wait for notification if condition is false;
        //continue otherwise
        cond_.wait(lock, [this] { return !queue_.empty() || done_; });
        if(done_) return T();
        T e(std::move(queue_.front()));
        queue_.pop_front();
        return e;
    }
    //! Empty ?
    //! This is intended to be used \em only when data access happens
    //! from inside a pre-existing loop
    bool Empty() const {
        std::lock_guard< std::mutex > lg(mutex_);
        const bool e = queue_.empty();
        return e;
    }
    //! Notify end of operations: will set end of operations flag to true
    //! and notify condition variable
    void Stop() {
        done_ = true;
        cond_.notify_one(); //notify
    }
    //! Reset: set end of operations flag to true: allow reuse of current
    //! queue instance
    void Reset() {
        done_ = false;
    }
    //! End of operations requested ?
    bool Done() const {
        return done_;
    }
    //! Invoke Done()
    bool operator!() const {
        return Done();
    }
    size_t Size() const {
        std::lock_guard< std::mutex > lg(mutex_);
        const size_t e = queue_.size();
        return e;
    }
private:
    std::deque<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    bool done_ = false;
};
