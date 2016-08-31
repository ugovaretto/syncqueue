#pragma once
//Author: Ugo Varetto

//! \file SyncValue.h
//! \brief Synchronized value access
//!



#include <deque>
#include <mutex>
#include <condition_variable>

//! Synchronized value:
//! @c Get() waits for data
template<typename T>
class SyncValue {
public:
    //! Put
    void Put(T&& v) {
        std::lock_guard<std::mutex> guard(mutex_);
        value_ = std::move(v); //MUST HAVE AN OVERLOADED operator=(T&&)
        empty_ = false;
        cond_.notify_one(); //notify
    }
    //! Return and remove element in front of queue.
    //! Waits indefinitely for an element to be available.
    T Get() {
        std::unique_lock<std::mutex> lock(mutex_);
        //stop and wait for notification if condition is false;
        //continue otherwise
        cond_.wait(lock, [this] { return !empty_; });
        T e(std::move(value_));
        empty_ = true;
        return e;
    }
    void Finish() {
        done_ = true;
        Put(T());
    }
    //! Empty ?
    bool Empty() const {
        return empty_;
    }
    bool Done() const {
        return done_;
    }
    void Reset() { done_ = false; }
    bool operator!() const { return Done(); }
private:
    T value_;
    bool empty_ = true;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    bool done_ = false;
};
