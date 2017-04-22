#-------------------------------------------------
#
# Project created by QtCreator 2016-04-30T16:25:33
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AUVAutoControl
TEMPLATE = app

INCLUDEPATH += ../ThirdParty/opencv/include/
INCLUDEPATH += ../ThirdParty/opencv/include/opencv/
INCLUDEPATH += ../ThirdParty/opencv/include/opencv2/
INCLUDEPATH += ../ThirdParty/Devices/FireGrab/Lib
INCLUDEPATH += ../ThirdParty/Devices/Vimba_1.4/VimbaImageTransform/Include
INCLUDEPATH += ../CustomizeFuntions/
INCLUDEPATH += ../VisionClass/

LIBS += ../ThirdParty/opencv/x86/vc10/lib/opencv_core249d.lib
LIBS += ../ThirdParty/opencv/x86/vc10/lib/opencv_highgui249d.lib
LIBS += ../ThirdParty/opencv/x86/vc10/lib/opencv_imgproc249d.lib
LIBS += ../ThirdParty/opencv/x86/vc10/lib/opencv_photo249d.lib
LIBS += ../ThirdParty/opencv/x86/vc10/lib/opencv_calib3d249d.lib
LIBS += ../ThirdParty/opencv/x86/vc10/lib/opencv_nonfree249d.lib
LIBS += ../ThirdParty/opencv/x86/vc10/lib/opencv_legacy249d.lib
LIBS += ../ThirdParty/opencv/x86/vc10/lib/opencv_gpu249d.lib
LIBS += ../ThirdParty/opencv/x86/vc10/lib/opencv_objdetect249d.lib
LIBS += ../ThirdParty/Devices/FireGrab/Lib32/FGCamera.lib
LIBS += ../ThirdParty/Devices/Vimba_1.4/VimbaImageTransform/Lib/Win32/VimbaImageTransform.lib


SOURCES += main.cpp\
        auvautocontrol.cpp \
    ../VisionClass/nccamera.cpp \
    ../VisionClass/ncdownmodel.cpp \
    ../VisionClass/ncfwdmodel.cpp \
    ../VisionClass/ncimgpro_basic.cpp \
    ../VisionClass/ncimgpro_process.cpp \
    ../VisionClass/ncinit.cpp \
    ../VisionClass/ncmodel.cpp \
    ../VisionClass/ncvision_analyse.cpp \
    ../VisionClass/ncvision_basic.cpp \
    ../VisionClass/ncvision_tmppro.cpp \
    ../CustomizeFuntions/GeneralImageProcess/contours_fun.cpp \
    ../CustomizeFuntions/GeneralImageProcess/findline_fun.cpp \
    ../CustomizeFuntions/GeneralImageProcess/imageprocessing_fun.cpp \
    ../CustomizeFuntions/SpecialImageProcess/findfirstdoor.cpp \
    ../CustomizeFuntions/SpecialImageProcess/torpedoboard_fun.cpp \
    ../CustomizeFuntions/SupportFunctions/ports_fun.cpp \
    ../CustomizeFuntions/SupportFunctions/string_fun.cpp \
    ../CustomizeFuntions/SupportFunctions/xmls_fun.cpp \
    ../CustomizeFuntions/GeneralImageProcess/draw_fun.cpp \
    ../VisionClass/ncvinforeplay.cpp \
    ../VisionClass/ncvision_record.cpp \
    ../VisionClass/ncvision_coutinfo.cpp \
    ../VisionClass/ncvision_draw.cpp

HEADERS  += auvautocontrol.h \
    global.h \
    ../VisionClass/nccamera.h \
    ../VisionClass/ncdownmodel.h \
    ../VisionClass/ncfwdmodel.h \
    ../VisionClass/ncheader.h \
    ../VisionClass/ncimgpro.h \
    ../VisionClass/ncmodel.h \
    ../VisionClass/ncvision.h \
    ../CustomizeFuntions/CustomizeStructs/imageprocessheader.h \
    ../CustomizeFuntions/GeneralImageProcess/contours_fun.h \
    ../CustomizeFuntions/GeneralImageProcess/findline_fun.h \
    ../CustomizeFuntions/GeneralImageProcess/imageprocessing_fun.h \
    ../CustomizeFuntions/SpecialImageProcess/findfirstdoor.h \
    ../CustomizeFuntions/SpecialImageProcess/torpedoboard_fun.h \
    ../CustomizeFuntions/SupportFunctions/ports_fun.h \
    ../CustomizeFuntions/SupportFunctions/string_fun.h \
    ../CustomizeFuntions/SupportFunctions/xmls_fun.h \
    thruster.h \
    ../CustomizeFuntions/GeneralImageProcess/draw_fun.h \
    ../VisionClass/ncvinforeplay.h \
    mission_param.h

RC_FILE  += nacit.rc
FORMS    += auvautocontrol.ui


