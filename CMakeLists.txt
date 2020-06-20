cmake_minimum_required(VERSION 3.15)
project(sonarLibrary)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-lpthread -lws2_32 -lwsock32 -Werror -Wall")

add_library(SonarLibrary SHARED api/record/sonarRecordAPI.cpp sonar/record/SonarRecord.cpp sonar/comm/SonarSocket.cpp network/WindowsSocketObject.cpp api/communication/sonarCommunicationAPI.cpp)
add_executable(sonarLibTest sample/main.cpp lib/libSonarLibrary.cpp)


find_package(Threads)
target_link_libraries (SonarLibrary ${CMAKE_THREAD_LIBS_INIT})

find_library(WSOCK32_LIBRARY wsock32)
find_library(WS2_32_LIBRARY ws2_32)
target_link_libraries(SonarLibrary wsock32 ws2_32)