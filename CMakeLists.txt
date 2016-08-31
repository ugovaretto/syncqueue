cmake_minimum_required(VERSION 3.5)
project(synctypes)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(syncvalue-test test/SyncValueTest.cpp)
add_executable(syncqueue-test test/SyncQueueTest.cpp)
