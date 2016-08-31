//
// Created by Ugo Varetto on 8/31/16.
//

#include <cassert>
#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <chrono>

#include "../SyncQueue.h"

using namespace std;

int main(int, char**) {
    SyncQueue< vector< char > > sq;
    const vector< char > in = {'1', '2', '3'};
    //push
    sq.Push(in);
    assert(sq.Pop() == in);
    //push front
    sq.PushFront(in);
    assert(sq.Pop() == in);
    //size
    sq.PushFront(in);
    sq.PushFront(in);
    assert(sq.Size() == 2);
    //push: move semantics
    vector< char > tin(in);
    sq.Push(move(tin));
    assert(tin.empty());
    assert(sq.Pop() == in);
    //push forward: move semantics
    tin = in;
    sq.PushFront(move(tin));
    assert(tin.empty());
    assert(sq.Pop() == in);
    //stop from separate thread
    //launch thread that waits on Pop then release lock from main thread
    auto task = async(launch::async, [&sq]() {
        //wait until main thread has unlocked the queue then
        //'Pop' synchronously: if 'Stop' works the method
        //should return immediately
        sq.Pop();
    });
    sq.Stop();
    //wait until Pop has been called (removed condition variable: too messy)
    this_thread::sleep_for(chrono::seconds(2));
    //check that thread has ended
    const std::future_status fs =
        task.wait_for(chrono::seconds(0));
    assert(fs == std::future_status::ready);
    //ok
    cout << "PASSED" << endl;
    return EXIT_SUCCESS;
}