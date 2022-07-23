QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatdialog.cpp \
    ckernel.cpp \
    logindialog.cpp \
    main.cpp \
    mediator/INetMediator.cpp \
    mediator/TcpClientMediator.cpp \
    mychatdialog.cpp \
    net/TcpClientNet.cpp \
    useritem.cpp

INCLUDEPATH +=./net
INCLUDEPATH +=./mediator
QT += core5compat
LIBS +=-lws2_32

HEADERS += \
    chatdialog.h \
    ckernel.h \
    logindialog.h \
    mediator/INetMediator.h \
    mediator/TcpClientMediator.h \
    mychatdialog.h \
    net/INet.h \
    net/PackDef.h \
    net/TcpClientNet.h \
    useritem.h


FORMS += \
    chatdialog.ui \
    logindialog.ui \
    mychatdialog.ui \
    useritem.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DISTFILES += \
    ../build-MyChat-Desktop_Qt_6_3_0_MSVC2019_64bit-Release/release/MyChat.exe \
    ../build-MyChat-Desktop_Qt_6_3_0_MSVC2019_64bit-Release/release/MyChat.exe \
    resource/register.png \
    tx/0.png \
    tx/1.png \
    tx/10.png \
    tx/11.png \
    tx/12.png \
    tx/13.png \
    tx/14.png \
    tx/15.png \
    tx/16.png \
    tx/17.png \
    tx/18.png \
    tx/19.png \
    tx/2.png \
    tx/20.png \
    tx/21.png \
    tx/22.png \
    tx/23.png \
    tx/24.png \
    tx/25.png \
    tx/26.png \
    tx/27.png \
    tx/28.png \
    tx/29.png \
    tx/3.png \
    tx/30.png \
    tx/31.png \
    tx/32.png \
    tx/33.png \
    tx/34.png \
    tx/35.png \
    tx/4.png \
    tx/5.png \
    tx/6.png \
    tx/7.png \
    tx/8.png \
    tx/9.png
