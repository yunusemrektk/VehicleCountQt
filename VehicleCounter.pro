QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += C:\opencv-4.4.0\build\install\include

LIBS += C:\opencv-4.4.0\build\bin\libopencv_core440.dll
LIBS += C:\opencv-4.4.0\build\bin\libopencv_highgui440.dll
LIBS += C:\opencv-4.4.0\build\bin\libopencv_imgcodecs440.dll
LIBS += C:\opencv-4.4.0\build\bin\libopencv_imgproc440.dll
LIBS += C:\opencv-4.4.0\build\bin\libopencv_features2d440.dll
LIBS += C:\opencv-4.4.0\build\bin\libopencv_calib3d440.dll
LIBS += C:\opencv-4.4.0\build\bin\libopencv_video440.dll
LIBS += C:\opencv-4.4.0\build\bin\libopencv_videoio440.dll
LIBS += C:\opencv-4.4.0\build\bin\libopencv_tracking440.dll
LIBS += C:\opencv-4.4.0\build\bin\openh264-1.8.0-win32.dll

#openh264-1.6.0-win32msvc.dll
SOURCES += \
    blob.cpp \
    main.cpp \
    mainwindow.cpp \
    qlabelmy.cpp

HEADERS += \
    blob.h \
    mainwindow.h \
    qlabelmy.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
