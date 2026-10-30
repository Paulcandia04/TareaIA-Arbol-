QT += widgets

TEMPLATE = app
TARGET = TareaIA

CONFIG += c++17 release
CONFIG -= debug debug_and_release console app_bundle

INCLUDEPATH += include

DESTDIR = bin
OBJECTS_DIR = bin/obj
MOC_DIR = bin/moc
RCC_DIR = bin/rcc
UI_DIR = bin/ui

SOURCES += \
    src/main.cpp \
    src/Formula.cpp \
    src/AtomRegistry.cpp \
    src/Parser.cpp \
    src/Expansion.cpp \
    src/Tableau.cpp \
    src/Application.cpp \
    src/TreeRenderer.cpp

HEADERS += \
    include/Formula.hpp \
    include/AtomRegistry.hpp \
    include/Parser.hpp \
    include/Expansion.hpp \
    include/Tableau.hpp \
    include/Application.hpp \
    include/TreeRenderer.hpp