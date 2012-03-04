# Makefile for Notepad++ plugins
.SUFFIXES: .cpp .cxx

# use specifically TDM version of minGW
CC = C:/MinGW64/bin/g++
RC = windres --target pe-i386  
# the target MUST be pe-i386 for Notepad++ plugins
DEL = rm -f
all: plugin
.PHONY: all unicode plugin clean echo debug objects

SRC = ../src
LEXERPLUGIN_SRC = ../LexerPlugin
DEBUG_SRC = ../debug
PLUGIN = LexRnw.dll
SCINTILLA_BASE = ../Scintilla

LEXERS = \
	LexRnw.o \
	LexR.o   \
	LexTeX.o
$(LEXERS):lexers.h
PLUGIN_OBJECTS = $(LEXERS)

include ../LexerPlugin/LexerPlugin.mk