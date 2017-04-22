# CMake最低版本要求
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# 添加依赖库路径
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)

# 添加源文件中的#include路径
INCLUDE_DIRECTORIES(./)
INCLUDE_DIRECTORIES(../NCFunctions)
INCLUDE_DIRECTORIES(../NCFunctions/NCServer)
INCLUDE_DIRECTORIES(../NCFunctions/NCClient)
INCLUDE_DIRECTORIES(../NCFunctions/NCStage)
INCLUDE_DIRECTORIES(../NCFunctions/NCStage/Basic)
INCLUDE_DIRECTORIES(../CustomizeFunctions)
INCLUDE_DIRECTORIES(../CustomizeFunctions/CustomizeStructs)
INCLUDE_DIRECTORIES(../CustomizeFunctions/GeneralImageProcess)
INCLUDE_DIRECTORIES(../CustomizeFunctions/SupportFunctions)

# 设置路径变量
SET(LIBS_SERVER
	ncserver
	ncstage
	ncstage_basic
	ncstage_test
)

# 生成二进制可执行文件
ADD_EXECUTABLE(IPCServer
	./main.cpp)
# 链接库文件，以这些库文件为基础生成目标文件
TARGET_LINK_LIBRARIES(IPCServer
	${LIBS_SERVER})
