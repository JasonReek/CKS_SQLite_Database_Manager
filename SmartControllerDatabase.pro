QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    csv.cpp \
    dataformat.cpp \
    highlighter.cpp \
    iconlabel.cpp \
    main.cpp \
    mainwindow.cpp \
    newtabledialog.cpp \
    sqleditor.cpp \
    table.cpp

HEADERS += \
    csv.h \
    dataformat.h \
    highlighter.h \
    iconlabel.h \
    mainwindow.h \
    newtabledialog.h \
    sqleditor.h \
    table.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Resources/Icons/add_table.png \
    database/SmartController.db

RESOURCES += \
    Resources.qrc
