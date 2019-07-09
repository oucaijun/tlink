#-------------------------------------------------
#
# Project created by QtCreator 2018-12-27T23:03:46
#
#-------------------------------------------------

TARGET = tlink
CONFIG += c++11
OBJECTS_DIR = bin
INCLUDEPATH += src/
INCLUDEPATH += src/zlog/

SOURCES += src/main.cpp \
    src/zlog/logmain.cpp \
    src/zlog/buf.c \
    src/zlog/category.c \
    src/zlog/category_table.c \
    src/zlog/conf.c \
    src/zlog/event.c \
    src/zlog/format.c \
    src/zlog/level.c \
    src/zlog/level_list.c \
    src/zlog/mdc.c \
    src/zlog/record.c \
    src/zlog/record_table.c \
    src/zlog/rotater.c \
    src/zlog/rule.c \
    src/zlog/spec.c \
    src/zlog/thread.c \
    src/zlog/zc_arraylist.c \
    src/zlog/zc_hashtable.c \
    src/zlog/zc_profile.c \
    src/zlog/zc_util.c \
    src/zlog/zlog-chk-conf.c \
    src/zlog/zlog.c

OTHER_FILES += \
    build.sh

HEADERS += \
    src/zlog/buf.h \
    src/zlog/category.h \
    src/zlog/category_table.h \
    src/zlog/conf.h \
    src/zlog/event.h \
    src/zlog/fmacros.h \
    src/zlog/format.h \
    src/zlog/level.h \
    src/zlog/level_list.h \
    src/zlog/logmain.h \
    src/zlog/mdc.h \
    src/zlog/record.h \
    src/zlog/record_table.h \
    src/zlog/rotater.h \
    src/zlog/rule.h \
    src/zlog/spec.h \
    src/zlog/thread.h \
    src/zlog/version.h \
    src/zlog/zc_arraylist.h \
    src/zlog/zc_defs.h \
    src/zlog/zc_hashtable.h \
    src/zlog/zc_profile.h \
    src/zlog/zc_util.h \
    src/zlog/zc_xplatform.h \
    src/zlog/zlog.h

