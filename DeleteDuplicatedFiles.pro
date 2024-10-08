QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = DeleteDuplicatedFiles
DESTDIR = bin

MOC_DIR =     .build
UI_DIR =      .build
RCC_DIR =     .build
OBJECTS_DIR = .build

SOURCES += src/duplicatefsmodel.cpp \
           src/fsitem.cpp \
           src/main.cpp \
           src/mainwindow.cpp

HEADERS += src/duplicatefsmodel.h \
           src/fsitem.h \
           src/mainwindow.h

FORMS += src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:QMAKE_POST_LINK += windeployqt $$OUT_PWD/$$DESTDIR

RESOURCES += resources.qrc
