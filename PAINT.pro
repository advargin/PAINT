QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 6): QT += widgets

TARGET = PAINT

TEMPLATE = app

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        canvaswidget.cpp \
        newcanvasdialog.cpp \
        canvas.cpp \
        drawingtools.cpp

HEADERS += \
        mainwindow.h \
        canvaswidget.h \
        newcanvasdialog.h \
        canvas.h \
        drawingtools.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc

