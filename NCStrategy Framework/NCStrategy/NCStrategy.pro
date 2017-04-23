#-------------------------------------------------
#
# Project created by QtCreator 2017-04-03T15:26:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NCStrategy
TEMPLATE = app

INCLUDEPATH += \
    ../CustomizeFunctions \
    ../CustomizeFunctions/GeneralImageProcess \
    ../CustomizeFunctions/SupportFunctions \
    ../NCFunctions \
    ../NCFunctions/NCClient \
    ../NCFunctions/NCServer \
    ../NCFunctions/NCStage \
    ../NCFunctions/NCStage/Basic \

SOURCES += main.cpp\
        ncstrategymainwin.cpp \
    ../CustomizeFunctions/SupportFunctions/string_fun.cpp \
    ../CustomizeFunctions/SupportFunctions/sys_fun.cpp \
    ../NCFunctions/NCServer/ncserver_dataproc.cpp \
    ../NCFunctions/NCServer/ncserver_link.cpp \
    ../NCFunctions/NCServer/ncserver_record.cpp \
    ../NCFunctions/NCServer/ncserver_stage.cpp \
    ../NCFunctions/NCServer/ncserver_strategy.cpp \
    ../NCFunctions/NCStage/Basic/ncstage.cpp \
    ../NCFunctions/NCStage/Basic/ncstage_basic.cpp \
    ../NCFunctions/NCStage/ncstage_test.cpp \
    ../NCFunctions/NCClient/ncclient_surface.cpp \
    ../NCFunctions/NCClient/ncclient.cpp \
    client_thread.cpp

HEADERS  += ncstrategymainwin.h \
    ../CustomizeFunctions/SupportFunctions/string_fun.h \
    ../CustomizeFunctions/SupportFunctions/sys_fun.h \
    ../NCFunctions/NCClient/ncclient_macros.h \
    ../NCFunctions/NCServer/ncserver.h \
    ../NCFunctions/NCServer/ncserver_macros.h \
    ../NCFunctions/NCStage/Basic/ncstage.h \
    ../NCFunctions/NCStage/Basic/ncstage_basic.h \
    ../NCFunctions/NCStage/Basic/ncstage_macro.h \
    ../NCFunctions/NCStage/ncstage_test.h \
    ../NCFunctions/NCClient/ncclient_surface.h \
    ../NCFunctions/NCClient/ncclient.h \
    client_thread.h

FORMS    += ncstrategymainwin.ui
