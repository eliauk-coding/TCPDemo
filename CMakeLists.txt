# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.5)

# 项目信息
project (TCPDemo C)

#设置与操作系统相关属性
if(UNIX)
  set(OS "linux")
  set(OS_LIBS pthread)
endif()

message("OS is ${OS}")

file(GLOB SOURCES src/*.c)

file(GLOB HEADERS src/*.h)

# 指定生成目标
add_executable(TCPDemo ${SOURCES})

#添加链接库
target_link_libraries(TCPDemo ${OS_LIBS})