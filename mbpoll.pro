TARGET = mbpoll
TEMPLATE = app

SOURCES += src/mbpoll.c \
    3rdparty/libmodbus/modbus.c \
    3rdparty/libmodbus/modbus-data.c \
    3rdparty/libmodbus/modbus-tcp.c \
    3rdparty/libmodbus/modbus-rtu.c 

HEADERS  += src/config.h \
    3rdparty/libmodbus/modbus.h \

INCLUDEPATH += 3rdparty/libmodbus 

unix:SOURCES +=
unix:LIBS += -lsysio
unix:DEFINES += _TTY_POSIX_

win32:SOURCES += 3rdparty/wingetopt/getopt.c
win32:INCLUDEPATH += 3rdparty/wingetopt
win32:DEFINES += _TTY_WIN_  WINVER=0x0501
win32:LIBS += -lsetupapi -lwsock32 -lws2_32

















