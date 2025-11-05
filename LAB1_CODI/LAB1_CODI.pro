QT       += core gui
QT += widgets charts
QT += charts printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    codecdiagramwidget.cpp \
    lab1panel.cpp \
    lab2panel.cpp \
    lab3panel.cpp \
    lab4panel.cpp \
    main.cpp \
    mainwindow.cpp \
    manualplotdialog.cpp \
    octaveparamspart2.cpp \
    octaverunner.cpp \
    octaverunnerpart3.cpp \
    qcustomplot.cpp \
    themestyles.cpp

HEADERS += \
    OctaveParams.h \
    codecdiagramwidget.h \
    lab1panel.h \
    lab2panel.h \
    lab3panel.h \
    lab4panel.h \
    mainwindow.h \
    manualplotdialog.h \
    octaveparamspart2.h \
    octaverunner.h \
    octaverunnerpart3.h \
    qcustomplot.h \
    themestyles.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
