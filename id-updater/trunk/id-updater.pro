TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release
QT += network xml

FORMS	= idupdater.ui
HEADERS = idupdater.h   InstallChecker.h   ProcessStarter.h   ScheduledUpdateTask.h
SOURCES = idupdater.cpp InstallChecker.cpp ProcessStarter.cpp ScheduledUpdateTask.cpp
SOURCES += main.cpp
INCLUDEPATH += ../../ ../../smartcard++
HEADERS += ../../smartcard++/DynamicLibrary.h  precompiled.h
SOURCES += ../../smartcard++/DynamicLibrary.cpp

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
RESOURCES= idupdater.qrc
win32 {
   RC_FILE = idupdater.rc
}

TRANSLATIONS  = idupdater_et.ts


