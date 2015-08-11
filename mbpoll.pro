TARGET = mbpoll
TEMPLATE = app
CONFIG += console
QT -= core gui
OBJECTS_DIR = obj

SOURCES += src/mbpoll.c \
    3rdparty/modbus/modbus.c \
    3rdparty/modbus/modbus-data.c \
    3rdparty/modbus/modbus-tcp.c \
    3rdparty/modbus/modbus-rtu.c \
    3rdparty/sysio/delay.c \
    3rdparty/sysio/serial.c

HEADERS  += src/config.h \
    3rdparty/modbus/modbus.h \

INCLUDEPATH += 3rdparty 3rdparty/modbus

distclean_dirs  = $$OBJECTS_DIR/

unix {
  SOURCES +=
  DEFINES += _TTY_POSIX_
  QMAKE_CFLAGS += -Wno-unused-parameter
  INSTALLBASE = /usr/local
  INSTALLBIN  = $$INSTALLBASE/bin
  QMAKE_DEL_FILE = rm -fr
}

win32 {
  SOURCES += 3rdparty/getopt/getopt.c
  INCLUDEPATH += 3rdparty/getopt
  DEFINES += _TTY_WIN_  WINVER=0x0501 STATIC_GETOPT 
  DEFINES += _WINSOCK_DEPRECATED_NO_WARNINGS
  DEFINES += _CRT_SECURE_NO_WARNINGS
  LIBS += -lsetupapi -lwsock32 -lws2_32
  INSTALLBASE = bin
  INSTALLBIN  = $$INSTALLBASE
  distclean_dirs ~= s,/,\\,g
  QMAKE_DEL_FILE = DEL /S /Q
}

QMAKE_DISTCLEAN +=  $$distclean_dirs

CONFIG(debug, debug|release) {
  DEFINES += DEBUG
}

target.path = $$INSTALLBIN
INSTALLS += target














