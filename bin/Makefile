# Makefile for Notepad++ plugins
.SUFFIXES: .cpp .cxx
CC = g++
RC = windres --target pe-i386
DEL = rm -f
all: plugin

#CONTROL Variables
# DEBUG=1
# UNICODE=1

SRC = ../src

PLUGIN = RnwLexer_plugin.dll
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
	# ScintillaWinL.o \
	# ScintillaBaseL.o \
# ScintillaBaseL.o: ScintillaBase.cxx Platform.h \
 # ILexer.h Scintilla.h SciLexer.h PropSetSimple.h \
 # SplitVector.h Partitioning.h RunStyles.h \
 # ContractionState.h CellBuffer.h CallTip.h \
 # KeyMap.h Indicator.h XPM.h LineMarker.h \
 # Style.h ViewStyle.h AutoComplete.h \
 # CharClassify.h Decoration.h Document.h \
 # Selection.h PositionCache.h Editor.h \
 # ScintillaBase.h LexAccessor.h Accessor.h \
 # LexerModule.h Catalogue.h
	# $(CC) $(CXXFLAGS) -D SCI_LEXER -c $< -o $@

# ScintillaWinL.o: ScintillaWin.cxx Platform.h \
 # ILexer.h Scintilla.h SplitVector.h \
 # Partitioning.h RunStyles.h ContractionState.h \
 # CellBuffer.h CallTip.h KeyMap.h Indicator.h \
 # XPM.h LineMarker.h Style.h AutoComplete.h \
 # ViewStyle.h CharClassify.h Decoration.h \
 # Document.h Selection.h PositionCache.h \
 # Editor.h ScintillaBase.h UniConversion.h \
 # LexAccessor.h Accessor.h \
 # LexerModule.h Catalogue.h
	# $(CC) $(CXXFLAGS) -D SCI_LEXER -c $< -o $@
###  End Scintilla objects ###

PLUGIN_DEF = $(SRC)/Plugin.def
PLUGIN_OBJECTS = \
	RnwLang.o \
	RnwRes.o \
	RnwLex.o \
	dbgstream.o
$(PLUGIN_OBJECTS): $(SRC)/RnwLang.h
OBJECTS = $(PLUGIN_OBJECTS) $(LOBJS)

vpath %.h $(SRC) $(SRC)/headers\
		$(SCINTILLA_SRC)/src \
		$(SCINTILLA_SRC)/include \
		$(SCINTILLA_SRC)/lexlib
vpath %.cpp $(SRC)
vpath %.cxx $(SCINTILLA_SRC)/src \
		  $(SCINTILLA_SRC)/lexlib \
		  $(SCINTILLA_SRC)/lexers \
		  $(SCINTILLA_SRC)/win32
vpath %.rc $(SRC) $(SCINTILLA_SRC)/win32

vpath %.h $(SRC)/dbgstream
vpath %.cpp $(SRC)/dbgstream

LDFLAGS=-shared -Wl,--enable-runtime-pseudo-reloc-v2 -mwindows -Wl,--add-stdcall-alias
LIBS=-lstdc++ -limm32 -lole32 -luuid
# Add -MMD to get dependencies
INCLUDEDIRS = -I $(SCINTILLA_SRC)/include \
              -I $(SCINTILLA_SRC)/src \
		    -I $(SCINTILLA_SRC)/lexlib \
		    -I $(SRC)/headers \
		    -I $(SRC)/dbgstream

#CPP_FLAGS = -m32 -fpermissive -fpic
CXXBASEFLAGS = -m32 -Wall -Wno-missing-braces -Wno-char-subscripts \
	-Wno-strict-overflow -pedantic $(INCLUDEDIRS) -fno-rtti  \
	-march=i686 -mthreads

# DEBUG
ifdef DEBUG
CXXFLAGS=-DDEBUG -g -O0 $(CXXBASEFLAGS)
else
CXXFLAGS=-DNDEBUG -Os $(CXXBASEFLAGS)
STRIPFLAG=-s
endif

#UNICODE
ifdef UNICODE
CXXFLAGS+=-DUNICODE
endif

# exported variables
export DEBUG
export UNICODE

##  RULES  ####################################################################
#goals
.PHONY: all unicode plugin clean echo debug objects
plugin: unicode
unicode: $(PLUGIN)
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
	cd ..; $(MAKE) --win32 clean
	
echo:
	@echo SRC_DIR= $(SRC)
	@echo OBJECTS= $(OBJECTS)
	g++ --version
	@echo $(DEBUG)

#Pass-off rules
# ../%:
	# cd .. && $(MAKE) --win32 $*
	
#noted rule using dllwrap for definition file.
RnwLangLib.dll: $(OBJECTS)
	dlltool -l $@ -mi386 $(OBJECTS) --no-export-all-symbols --add-stdcall-underscore 
dbgtest.exe: ../src/dbgstream/dbgstream.cpp
	$(CC) -m32 -mwindows -o $@ $(STRIPFLAG) $< $(CXXFLAGS) -DTEST_MAIN
