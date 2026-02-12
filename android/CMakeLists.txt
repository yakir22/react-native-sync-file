cmake_minimum_required(VERSION 3.13)
project(sync-file)

set(CMAKE_CXX_STANDARD 17)

find_package(ReactAndroid REQUIRED CONFIG)

add_library(sync-file SHARED
  cpp/jni-adapter.cpp
  ../cpp/SyncFileBridge.cpp
)

target_include_directories(sync-file PRIVATE
  ../cpp
)

target_link_libraries(sync-file
  ReactAndroid::jsi
  ReactAndroid::reactnativejni
  android
  log
)
