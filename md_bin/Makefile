# Makefile for Markdown Lexer Port

# use specifically TDM version of minGW
CC = C:/MinGW64/bin/g++
RC = windres --target pe-i386
# the target MUST be pe-i386 for Notepad++ plugins
DEL = rm -f
all: plugin
.PHONY: all unicode plugin clean echo debug objects

SRC = ../md_src
PLUGIN = LexerMd.dll
BIN_DIR = .

LEXERS = LexMarkdown.o
PLUGIN_OBJECTS = LexerMd.o $(LEXERS)
$(PLUGIN_OBJECTS): $(SRC)/LexerMd.h

include ../LexerPlugin/LexerPlugin.mk
include ../LexerPlugin/test.mk