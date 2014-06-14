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
INCLUDEPATH += C://opencv-2.4.5//mybuild//install//include
LIBS += C://opencv-2.4.5//mybuild//bin//*.dll
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
