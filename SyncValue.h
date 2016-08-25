// Zero Remoting Framework
// Author: Ugo Varetto

//! \file SyncQueue.h
//! \brief Synchronized queue
//!
//! Implementation of a synchronized queue.
#pragma once
//Author: Ugo Varetto
//
// This file is part of zrf - zeromq remoting framework.
//zrf is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//zrf is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with zrf.  If not, see <http://www.gnu.org/licenses/>.

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
    //! Empty ?
    bool Empty() const {
        return empty_;
    }
private:
    T value_;
    bool empty_ = true;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
};
