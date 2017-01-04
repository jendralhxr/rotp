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
#INCLUDEPATH += C:\opencv\build\include
#LIBS += C:\opencv\build\x64\vc12\bin\*.dll
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

win32:CONFIG(release, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib/ -lopencv_core2413
else:win32:CONFIG(debug, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib/ -lopencv_core2413d
win32:CONFIG(release, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib/ -lopencv_highui2413
else:win32:CONFIG(debug, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib/ -lopencv_highui2413d
win32:CONFIG(release, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib/ -lopencv_imgproc2413
else:win32:CONFIG(debug, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib/ -lopencv_imgproc2413d


INCLUDEPATH += C:/opencv/build/include
DEPENDPATH += C:/opencv/build/include
