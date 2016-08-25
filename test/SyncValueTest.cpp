//
// Created by Ugo Varetto on 8/25/16.
//
#include <cstdlib>
#include <cassert>
#include <thread>
#include <future>
#include <string>
#include <iostream>

#include "../SyncValue.h"

using namespace std;

int main(int argc, char** argv) {

    SyncValue< string > v;

    auto get = async(launch::async, [&v](){
        return v.Get();
    });

    auto put = async(launch::async, [&v]() {
        v.Put(string("HELLO"));
    });

    put.get();
    assert(get.get() == "HELLO");

    cout << "PASSED" << endl;
    return EXIT_SUCCESS;
}


////other thread Gets buffer and consumes it then moves it back into syncBuffer
//shared_ptr< SyncBuffer< ByteArray > > syncBuffer;
//
////PRODUCER THREAD
//syncBuffer->Put(buffer);
//while(...) {
// buffer = syncBuffer->Get(); //get back used buffer, move operation
// Update(buffer);
// syncBuffer->Put(buffer);
// ...
//}
//syncBuffer->Finish(); //notify end of operations
//
////CONSUMER THREAD
//while(syncBuffer) {
//  sendbuffer = syncBuffer->Get(); //move into local variable
//  Send(sendbuffer); //consume
//  syncBuffer->Put(sendbuffer); //put back
//}

