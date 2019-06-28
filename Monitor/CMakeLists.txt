#Make 最低版本号要求
cmake_minimum_required(VERSION 2.8)
#项目信息
project(mergeMonitor)

#include路径
include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/../Base
)

# 查找当前目录下的所有源文件
# 并将名称保存到 LIB_SRC 变量
aux_source_directory(. LIB_SRC)

#添加可执行文件

add_executable(mergeMonitor ${LIB_SRC})

# 设置可执行文件生成路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../) 

find_library(LIB_BASE base  ${PROJECT_SOURCE_DIR}/../libs)
 
#生成debug版本
SET(CMAKE_BUILD_TYPE "DEBUG") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -std=c++11  -fPIC -Wno-deprecated  -Wunused-value  -Wno-write-strings -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


#其他系统库
TARGET_LINK_LIBRARIES(mergeMonitor ${LIB_BASE}
                        glog
                        curl
                        pthread
                      )
