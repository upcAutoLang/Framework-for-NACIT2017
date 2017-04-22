# CMake最低版本要求
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


# 添加源文件中的#include路径
INCLUDE_DIRECTORIES(./)
INCLUDE_DIRECTORIES(../)
INCLUDE_DIRECTORIES(../NCClient)
INCLUDE_DIRECTORIES(../NCStage)
INCLUDE_DIRECTORIES(../NCStage/Basic)
INCLUDE_DIRECTORIES(../../CustomizeFunctions/SupportFunctions)
INCLUDE_DIRECTORIES(../../CustomizeFunctions/CustomizeStructs)
INCLUDE_DIRECTORIES(../../CustomizeFunctions/GeneralImageProcess)

# 查找某个路径下的所有源文件，并将源文件列表存储到某个变量中 
AUX_SOURCE_DIRECTORY(./ LIB_SERVER_SOURCE)
SET(LIB_SERVER_SOURCE 
	${LIB_SERVER_SOURCE}
	../NCClient/ncclient_macros.h
	../NCStage/Basic/ncstage.cpp
	../../CustomizeFunctions/SupportFunctions/string_fun.cpp
	../../CustomizeFunctions/SupportFunctions/sys_fun.cpp)

# 生成库文件
ADD_LIBRARY(ncserver SHARED ${LIB_SERVER_SOURCE})
