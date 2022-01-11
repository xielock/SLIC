cmake_minimum_required(VERSION 3.2)
add_definitions(-std=c++11)
PROJECT(try)#一定要与#7中的名字一致,好像test是保留字，不能用
      set(CMAKE_THREAD_LIBS_INIT "-lpthread")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
      set(CMAKE_HAVE_THREADS_LIBRARY 1)
      set(CMAKE_USE_WIN32_THREADS_INIT 0)
      set(CMAKE_USE_PTHREADS_INIT 1)
      set(THREADS_PREFER_PTHREAD_FLAG ON)

#补.添加opencv库
find_package(OpenCV 3.2 REQUIRED)#注意对应版本
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

#3.head file path，头文件目录，所有.h文件放在include文件夹下
INCLUDE_DIRECTORIES(include)

#4.source directory，源文件目录，DIR_SRCS代表目录下所有源文件，所有.cpp文件放在src文件夹下
AUX_SOURCE_DIRECTORY(src DIR_SRCS)

#5.set environment variable，设置环境变量，编译用到的源文件全部都要放到这里，否则编译能够通过，但是执行的时候会出现各种问题，比如"symbol lookup error xxxxx , undefined symbol"
#设定环境变量TEST_MATH的值为环境变量DIR_SRCS的值？？？
SET(TEST_MATH ${DIR_SRCS})

#6.add executable file，添加要编译的可执行文件
ADD_EXECUTABLE(${PROJECT_NAME} ${TEST_MATH})

#7.add link library，添加可执行文件所需要的库，比如我们用到了libm.so（命名规则：lib+name+.so），就添加该库的名称
target_link_libraries (try ${OpenCV_LIBRARIES})#记着改
