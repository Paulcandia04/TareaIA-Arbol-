QT += core gui widgets

CONFIG += c++17
DESTDIR = release
TARGET = TareaIA
TEMPLATE = app

# Archivos de cabecera (.hpp) en la carpeta include
INCLUDEPATH += include

HEADERS += \
    include/Application.hpp \
    include/Parser.hpp \
    include/Tableau.hpp \
    include/Formula.hpp \
    include/AtomRegistry.hpp \
    include/Expansion.hpp \
    include/TreeRenderer.hpp

# Archivos de código fuente (.cpp) en la carpeta src
SOURCES += \
    src/main.cpp \
    src/Application.cpp \
    src/Parser.cpp \
    src/Tableau.cpp \
    src/Formula.cpp \
    src/AtomRegistry.cpp \
    src/Expansion.cpp \
    src/TreeRenderer.cpp

# Archivo de recursos
RESOURCES += \
    resources.qrc