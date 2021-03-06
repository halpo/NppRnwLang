# Makefile for Notepad++ plugins
.SUFFIXES: .cpp .cxx

# use specifically TDM version of minGW
CC = C:/MinGW64/bin/g++
RC = windres --target pe-i386
DEL = rm -f
all: plugin
.PHONY: all unicode plugin clean echo debug objects

SRC = ../src

PLUGIN = LexRnw.dll
SCINTILLA_SRC = ../Scintilla

### SCINTILLA OBJECTS #########
BASEOBJS = \
	AutoComplete.o \
	CallTip.o \
	CellBuffer.o \
	CharacterSet.o \
	CharClassify.o \
	ContractionState.o \
	Decoration.o \
	Document.o \
	Editor.o \
	KeyMap.o \
	Indicator.o \
	LineMarker.o \
	PerLine.o \
	PlatWin.o \
	PositionCache.o \
	PropSetSimple.o \
	RESearch.o \
	RunStyles.o \
	ScintRes.o \
	Selection.o \
	Style.o \
	UniConversion.o \
	ViewStyle.o \
	XPM.o
LOBJS = \
	Accessor.o \
	Catalogue.o \
	ExternalLexer.o \
	LexerBase.o \
	LexerModule.o \
	LexerSimple.o \
	StyleContext.o \
	WordList.o \
	$(BASEOBJS)	
###  End Scintilla objects ###

LEXERS = \
	LexR.o \
	LexTeX.o
$(LEXERS):lexers.h
PLUGIN_DEF = $(SRC)/Plugin.def
PLUGIN_OBJECTS = \
	RnwLang.o \
	RnwRes.o \
	$(LEXERS) \
	LexRnw.o
$(PLUGIN_OBJECTS): $(SRC)/RnwLang.h
OBJECTS = \
	$(LOBJS) \
	$(PLUGIN_OBJECTS)

vpath %.h   $(SRC) $(SRC)/headers\
            $(SCINTILLA_SRC)/src \
            $(SCINTILLA_SRC)/include \
            $(SCINTILLA_SRC)/lexlib \
            $(SRC)/dbgstream
vpath %.cpp $(SRC) \
            $(SRC)/dbgstream
vpath %.cxx $(SCINTILLA_SRC)/src \
            $(SCINTILLA_SRC)/lexlib \
            $(SCINTILLA_SRC)/win32
vpath %.rc $(SRC) $(SCINTILLA_SRC)/win32


LDFLAGS=-shared -Wl,--enable-runtime-pseudo-reloc-v2 -mwindows -Wl,--add-stdcall-alias
LIBS=-lstdc++ -limm32 -lole32 -luuid
# Add -MMD to get dependencies
INCLUDEDIRS = -I $(SCINTILLA_SRC)/include \
              -I $(SCINTILLA_SRC)/src \
		    -I $(SCINTILLA_SRC)/lexlib \
		    -I $(SRC)/headers \
		    -I $(SRC)/dbgstream
		    
# http://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Warning-Options.html#Warning-Options
CHECK_FLAGS  = -Wall -Wextra -Wno-missing-braces -Wno-char-subscripts \
               -Wno-strict-overflow -pedantic -Wno-unused-parameter \
               -Wno-missing-field-initializers
SCI_CXXFLAGS = -DSCI_LEXER
CXXBASEFLAGS = -m32 $(CHECK_FLAGS) $(SCI_CXXFLAGS)\
	$(INCLUDEDIRS) -fno-rtti  \
	-march=i686 -mthreads

# DEBUG
DEBUG ?= 0
ifeq ($(DEBUG),1)
CXXFLAGS=-DDEBUG -D_DEBUG -g -O0 $(CXXBASEFLAGS)
PLUGIN_OBJECTS += \
	deparse_wm_msg.o \
	dbgstream.o
else
CXXFLAGS=-DNDEBUG -Os $(CXXBASEFLAGS)
STRIPFLAG=-s
endif



#UNICODE
UNICODE ?= 1
ifeq ($(UNICODE), 1)
CXXFLAGS += -DUNICODE -D_UNICODE
endif

# exported variables
export DEBUG
export _DEBUG
export UNICODE
export _UNICODE

##  RULES  ####################################################################
#goals
.PHONY:plugin objects
plugin: $(PLUGIN)
objects:$(OBJECTS)

.cxx.o:
	$(CC) $(CXXFLAGS) -c $<
.cpp.o:
	$(CC) $(CXXFLAGS) -c $<

$(PLUGIN): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(STRIPFLAG) $^ $(CXXFLAGS) $(LIBS)

%.o:	%.rc
	$(RC) $< $@

.PHONY:clean sweep clobber
sweep:
	-$(DEL) *.dll $(PLUGIN_OBJECTS) dbgtest.exe
clean: sweep
	-$(DEL) *.exe *.o *.obj *.dll *.res *.map *.d
clobber:clean
	cd ..; $(MAKE) clean
	
echo:
	@echo SRC_DIR= $(SRC)
	@echo OBJECTS= $(OBJECTS)
	g++ --version
	@echo DEBUG= $(DEBUG)
	@echo UNICODE= $(UNICODE)
	@echo CXXFLAGS= $(CXXFLAGS)
	
	
#noted rule using dllwrap for definition file.
RnwLangLib.dll: $(OBJECTS)
	dlltool -l $@ -mi386 $(OBJECTS) --no-export-all-symbols --add-stdcall-underscore 
dbgtest.exe: ../src/dbgstream/dbgstream.cpp
	$(CC) -m32 -mwindows -o $@ $(STRIPFLAG) $< $(CXXFLAGS) -DTEST_MAIN
deparseWM_MSG.exe: deparse_wm_msg.cpp
	$(CC) -m32 -mwindows -o $@ $(STRIPFLAG) $< $(CXXFLAGS) -DTEST_MAIN
	
	
	