This repository is the Source Code of software frame, which is written by myself and for NACIT team that will participate in Robosub Competition, which is held on San Diego of California in July, 2017.
NACIT product will archive a series of functions, like retrieving sensors' data, image processing, judging and sending control signals and so on. This frame's aim is making these functions work modular and transport data with each other. The main way to archive this is IPC (InterProcess Communication) by using socket.
# Why I write this project?
In robot industry, people use ROS as their operating system as normal. Actually, I wanted to build the software frame for my team by using ROS directly. But I get feedback from my colleagues who say there are so many bugs when they and their colleagues use ROS, so my colleagues suggest that maybe it's more stable if I write the frame by myself than ROS.
I can't judge ROS because I didn't use ROS, but I still learn the IPC theory like ROS does, and merge it with my own thoughts then try to write this software frame. After my debugging, I think this frame already has the effect that I imagined before, so I hope this frame will do something useful for the next generation of my team. 
Besides, I will produce source codes and introducing document, and hope someone amazing in GitHub, like you, give me some suggestion.
# Files Introduction
- NCStrategy Framework：Source Codes of NACIT's software frame for Robosub in 2017(version 0 and have been debugged)
	- CustomizeFunctions：A set of customizes functions by myself
		- CustomizeStructs：Structs customized by myself
		- GeneralImageProcess：General Image Processing Functions customized by myself
		- SupportFunctions：Other Supported Functions customized by myself
	- NCFunctions：Source Codes of classes and functions, which are just for NACIT team and written by myself
		- NCClient：Source Codes of clients' basic and derived classes
		- NCServer：Source Codes of Server class
		- NCStage：Source Codes of Mission & Stage class
	- NCStrategy：Source Codes of software frame's UI(User Interface)
	- IPCClients：Source Codes of building clients
	- IPCServer：Source Codes of building server
- NCStrategy说明文档 by 琦少：the Introduce document of this software frame (Written in Chinese)
- README.md：the Introduce of this repository (Written in English)
- README-中文：the Introduce of this repository (Written in Chinese)

# Project's cmake
My working environment:

- Operating System: Ubuntu 14.04 LTS
- Conditions before your cmake command:
	- have already done OpenCV's make & make install 
	- have alread done CMake's make & make install

I used to write a series of CMakeLists.txt specially, then I wrote and published the blog of the cmake tutorial of this project in CSDN and GitHub website. Here are the websites:
CSDN：http://blog.csdn.net/ajianyingxiaoqinghan/article/details/70338459
GitHub：https://github.com/upcAutoLang/Blog/issues/12

Besides, I also wrote the tutorial blog of how to install OpenCV 2.4.9 in Ubuntu 14.04. Here are the websites:
CSDN：http://blog.csdn.net/ajianyingxiaoqinghan/article/details/62424132
GitHub：https://github.com/upcAutoLang/Blog/issues/1

Open a terminal and enter in the path of folder named "NCStrategy Framework", then input commands like below:
```bash
mkdir build
cd ./build
cmake ../
make
```
Then you will build this project.
The path of binary files: /NCStrategy Framework/build/bin
The path of library files: /NCStrategy Framework/build/lib


