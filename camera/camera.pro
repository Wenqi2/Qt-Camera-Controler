#-------------------------------------------------
#
# Project created by QtCreator 2023-07-18T09:12:26
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = camera
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += link_pkgconfig


PKGCONFIG += gstreamer-1.0 \
        glib-2.0 \
        gobject-2.0 \
        gio-2.0 \
        gstreamer-video-1.0 \
        gstreamer-audio-1.0 \

 LIBS += -lglib-2.0 \
        -lgobject-2.0 \
        -lgstreamer-1.0 \          # <gst/gst.h>
        -lgstvideo-1.0 \             # <gst/video/videooverlay.h>
        -L/usr/lib/x86_64-linux-gnu/gstreamer-1.0 \
        -lgstautodetect
        -lQt5Core
linux {
    INCLUDEPATH += \
        /usr/include/gstreamer-1.0 \
        /usr/include/glib-2.0/ \
        /usr/lib/x86_64-linux-gnu/glib-2.0/include/ \
        /usr/include/qt
}

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        video_capture.cpp \
        image_capture.cpp \
    imgmanager.cpp \
    cameracontroller.cpp \
    cameraimage.cpp \
    camerarepository.cpp

HEADERS += \
        mainwindow.h \
    video_capture.h \
    image_capture.h \
    imgmanager.h \
    cameracontroller.h \
    cameraimage.h \
    camerarepository.h



FORMS += \
        mainwindow.ui \
    video_capture.ui \
    image_capture.ui \
    imgmanager.ui

DISTFILES +=
