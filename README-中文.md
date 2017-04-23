该仓库中内容是笔者为将于2017年7月于美国圣地亚哥举行的Robosub竞赛的NACIT团队而写的顶层软件框架，当前目的是实现获取传感器数据、进行图像处理、判断并发送控制信号等功能之间互不影响、且能互相传输数据的功能，主要实现手法是通过socket网络编程实现进程间通讯。
# 一、编写该工程的原因
业界控制机器人，普遍使用ROS系统。笔者原本也想直接使用ROS系统为团队构建框架，然而收到同行的反馈，说ROS系统在他们和他们的同行们使用的时候存在很多bug，所以不如写自己的系统那么稳定。笔者对ROS本身不作评价，但依旧按着自己的思路与ROS实现进程间通信的原理，自己尝试性的写了该软件框架，经调试已经达到笔者想要的效果，希望能够被团队后辈使用。同时此处提供源代码与相应的说明文档，希望各路大神批评指正。
# 二、文件说明
- NCStrategy Framework：NACIT团队的Robosub2017竞赛的软件框架测试源码
	- CustomizeFunctions：笔者自定义函数集源码
		- CustomizeStructs：自定义结构体
		- GeneralImageProcess：通用图像处理函数
		- SupportFunctions：其他支持函数
	- NCFunctions：笔者专为NACIT团队而写的类与函数集源码
		- NCClient：客户端及其派生类源码
		- NCServer：服务器端源码
		- NCStage：任务阶段类的源码
	- NCStrategy：笔者为该软件框架写的基础调试界面实现源码
	- IPCClients：各客户端文件的实现源码
	- IPCServer：服务器端文件的实现源码
- NCStrategy说明文档 by 琦少：该软件框架的中文说明文档
- README.md：该仓库英文说明
- README-中文：该仓库中文说明
# 三、CMake该项目
笔者的工作环境：

- 操作系统：Ubuntu 14.04 LTS
- 编译条件：
	- 已编译且安装OpenCV
	- 已安装CMake

笔者曾为该项目而专门写过CMake，且写了博客公布在国内CSDN与国外GitHub上。地址如下：
CSDN：http://blog.csdn.net/ajianyingxiaoqinghan/article/details/70338459
GitHub：https://github.com/upcAutoLang/Blog/issues/12
关于在Ubuntu 14.04下OpenCV的安装，笔者写的教程如下：
CSDN：http://blog.csdn.net/ajianyingxiaoqinghan/article/details/62424132
GitHub：https://github.com/upcAutoLang/Blog/issues/1

进入终端，在NCStrategy Framework路径下输入如下指令：
```bash
mkdir build
cd ./build
cmake ../
make
```
即可编译该工程。
生成文件路径：/NCStrategy Framework/build/bin
生成库文件路径：/NCStrategy Framework/build/lib

