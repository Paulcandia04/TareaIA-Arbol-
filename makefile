#   mingw32-make run
SHELL := cmd.exe
.SHELLFLAGS := /C
export PATH := C:\msys64\ucrt64\bin;$(PATH)

PROJECT := TareaIA.pro
QT_MAKEFILE := Makefile.qt

# Aquí le decimos que busque en release, no en bin
EXE := release\TareaIA.exe

.DEFAULT_GOAL := run

build:
	qmake6 "$(PROJECT)" -o "$(QT_MAKEFILE)"
	$(MAKE) -f "$(QT_MAKEFILE)"

run: build
	start "" "$(EXE)"

start:
	start "" "$(EXE)"

clean:
	if exist "$(QT_MAKEFILE)" $(MAKE) -f "$(QT_MAKEFILE)" clean

rebuild: clean
	$(MAKE) build

.PHONY: build run start clean rebuild