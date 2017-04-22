CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FIND_PACKAGE(OpenCV REQUIRED)

INCLUDE_DIRECTORIES(./)
INCLUDE_DIRECTORIES(../)
INCLUDE_DIRECTORIES(../CustomizeStructs)

SET(LIB_CONTOUR_SOURCE 
	../CustomizeStructs/imageprocessheader.h
	./contours_fun.h
	./contours_fun.cpp)
SET(LIB_IMAGEPROCESS_SOURCE 
	../CustomizeStructs/imageprocessheader.h
	./imageprocessing_fun.h
	./imageprocessing_fun.cpp)

ADD_LIBRARY(contours_fun SHARED ${LIB_CONTOUR_SOURCE})
ADD_LIBRARY(imageprocessing_fun SHARED ${LIB_IMAGEPROCESS_SOURCE})
