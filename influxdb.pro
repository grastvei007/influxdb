#-------------------------------------------------
#
# Project created by QtCreator 2018-09-13T18:29:16
#
#-------------------------------------------------

QT       += network widgets

TEMPLATE = lib

release: TARGET = influxdb
debug: TARGET = influxdbd

DESTDIR = $$(DEV_LIBS)

release: BUILDDIR = build/release
debug:   BUILDDIR = build/debug

OBJECTS_DIR = $$BUILDDIR/.obj
MOC_DIR = $$BUILDDIR/.moc
RCC_DIR = $$BUILDDIR/.qrc
UI_DIR = $$BUILDDIR/.ui

DEFINES += INFLUXDB_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


if(debug){
LIBS += -L$$(DEV_LIBS)
}
else{
LIBS += -L$$(DEV_LIBS)
}

INCLUDEPATH += $$(JUNE_ROOT)



SOURCES += \
        influxdb.cpp

HEADERS += \
        influxdb.h \
        influxdb_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS +=
