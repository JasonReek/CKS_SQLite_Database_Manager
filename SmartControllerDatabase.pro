QT += core gui sql
#QT += webenginewidgets

RC_ICONS = db.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
#CONFIG += console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    csvworker.cpp \
    dataformat.cpp \
    highlighter.cpp \
    iconlabel.cpp \
    importdialog.cpp \
    jfile.cpp \
    main.cpp \
    mainwindow.cpp \
    newtabledialog.cpp \
    popuplist.cpp \
    sqleditor.cpp \
    table.cpp

HEADERS += \
    csvworker.h \
    dataformat.h \
    highlighter.h \
    iconlabel.h \
    importdialog.h \
    jfile.h \
    mainwindow.h \
    newtabledialog.h \
    popuplist.h \
    sqleditor.h \
    table.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    database/SmartController.db

RESOURCES += \
    Resources.qrc
