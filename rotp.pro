#-------------------------------------------------
#
# Project created by QtCreator 2014-06-10T10:59:30
#
#-------------------------------------------------

QT       += core gui

unix{
INCLUDEPATH += /usr/include/opencv2
LIBS += `pkg-config opencv --libs`
}

win32{
INCLUDEPATH += C:\\opencv-2.4.9\\opencv\\build\\include
LIBS += -LC:\\opencv-2.4.9\\mybuild\\lib\\Debug \
            -lopencv_calib3d249d \
            -lopencv_contrib249d \
            -lopencv_core249d \
            -lopencv_features2d249d \
            -lopencv_flann249d \
            -lopencv_gpu249d \
            -lopencv_highgui249d \
            -lopencv_imgproc249d \
            -lopencv_legacy249d \
            -lopencv_ml249d \
            -lopencv_objdetect249d \
            -lopencv_ts249d \
            -lopencv_video249d

}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rotp
TEMPLATE = app

SOURCES += main.cpp\
    rotimage.cpp \
    rotspinbox.cpp

HEADERS  += \
    rotimage.h \
    rotspinbox.h
