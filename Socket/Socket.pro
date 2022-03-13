TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \
        userSocket.c

HEADERS +=

LIBS += -pthread
