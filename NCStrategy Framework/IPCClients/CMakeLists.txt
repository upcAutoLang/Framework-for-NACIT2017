# CMake最低版本要求
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# 系统中寻找OpenCV的安装路径
FIND_PACKAGE(OpenCV REQUIRED)

# 添加依赖库路径
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)

# 添加源文件中的#include路径
INCLUDE_DIRECTORIES(./)
INCLUDE_DIRECTORIES(./IPCImageClient)
INCLUDE_DIRECTORIES(./IPCSendClient)
INCLUDE_DIRECTORIES(./IPCSurfClient)
INCLUDE_DIRECTORIES(../NCFunctions)
INCLUDE_DIRECTORIES(../NCFunctions/NCClient)
INCLUDE_DIRECTORIES(../CustomizeFunctions)
INCLUDE_DIRECTORIES(../CustomizeFunctions/CustomizeStructs)
INCLUDE_DIRECTORIES(../CustomizeFunctions/GeneralImageProcess)
INCLUDE_DIRECTORIES(../CustomizeFunctions/SupportFunctions)

# 设置路径变量
SET(LIBS_CLIENT
	ncclient)
SET(LIBS_IMAGE
	${LIBS_CLIENT}
	ncclient_image)
SET(LIBS_SEND
	${LIBS_CLIENT}
	ncclient_send)
SET(LIBS_SURF
	${LIBS_CLIENT}
	ncclient_surf)

# 生成二进制可执行文件
ADD_EXECUTABLE(IPCImageClient 
	./IPCImageClient/ncclient_image_main.cpp)
# 链接库文件，以这些库文件为基础生成目标文件
TARGET_LINK_LIBRARIES(IPCImageClient 
	${LIBS_IMAGE})

ADD_EXECUTABLE(IPCSendClient
	./IPCSendClient/ncclient_send_main.cpp)
TARGET_LINK_LIBRARIES(IPCSendClient
	${LIBS_SEND})

ADD_EXECUTABLE(IPCSurfClient
	./IPCSurfClient/NCClient_Surf_Main.cpp)
TARGET_LINK_LIBRARIES(IPCSurfClient
	${LIBS_SURF})
