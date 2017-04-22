# CMake最低版本要求
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# 添加源文件中的#include路径
INCLUDE_DIRECTORIES(./)
INCLUDE_DIRECTORIES(../)

# 设置路径变量
SET(LIB_STRING_SOURCE 
	./string_fun.h
	./string_fun.cpp)
SET(LIB_SYS_SOURCE 
	./string_fun.h
	./string_fun.cpp
	./sys_fun.h
	./sys_fun.cpp)

# 生成库文件
ADD_LIBRARY(string_fun SHARED ${LIB_STRING_SOURCE})
ADD_LIBRARY(sys_fun SHARED ${LIB_SYS_SOURCE})
