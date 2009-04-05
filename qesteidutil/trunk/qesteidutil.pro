# -------------------------------------------------
# Project created by QtCreator 2009-02-18T18:52:12
# -------------------------------------------------
TARGET = qesteidutil
TEMPLATE = app
QT += webkit \
    network
SOURCES += main.cpp \
    mainwindow.cpp \
    cardlib/PCSCManager.cpp \
    cardlib/CardBase.cpp \
    cardlib/SCError.cpp \
    cardlib/DynamicLibrary.cpp \
    cardlib/CTAPIManager.cpp \
    cardlib/EstEidCard.cpp \
    jsextender.cpp \
    jsesteidcard.cpp \
    jscertdata.cpp \
    jscardmanager.cpp \
    cardlib/SmartCardManager.cpp
HEADERS += mainwindow.h \
    cardlib/PCSCManager.h \
    cardlib/CardBase.h \
    cardlib/ManagerInterface.h \
    cardlib/SCError.h \
    cardlib/DynamicLibrary.h \
    cardlib/CTAPIManager.h \
    cardlib/EstEidCard.h \
    jsextender.h \
    jsesteidcard.h \
    jscertdata.h \
    jscardmanager.h \
    cardlib/SmartCardManager.h
INCLUDEPATH += /usr/include/PCSC
FORMS += 
RESOURCES += qesteidutil.qrc
