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


