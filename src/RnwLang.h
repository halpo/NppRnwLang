// This file is part of RnwLang.
// A plugin written for Notepad++

// Copyright (C)2012 Andrew Redd
// Modified from GmodLua plugin according to permission in the GPL license.

// RnwLang is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// RnwLang is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with RnwLang.  If not, see <http://www.gnu.org/licenses/>.

#ifndef RnwLang_H
#define RnwLang_H

#if 1 // Includes Section

#include <cassert>
#include <algorithm>
#include "unidef.h"
  
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501
  #endif

  // C RunTime Header Files
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdarg.h>
  #include <assert.h>
  #include <ctype.h>
  #include <functional>
  #include <time.h>
  #include <sys/types.h>
  #include <sys/stat.h>

  // STL Headers
  #include <iostream>
  #include <sstream>
  #include <string>
  #include <vector>
  #include <deque>
  #include <map>
  #include <memory>
  #include <algorithm>
  #include <exception>	
  #include <string>

  // Windows Header Files
  #include <WinDef.h>


  #ifdef UNICODE
    #include <wchar.h>
  #endif

  // Notepad++
  #include "Common.h"
  #include "Window.h"
  #include "StaticDialog.h"
  #include "PluginInterface.h"

  // Scintilla includes
  #include "ILexer.h"
  #include "PropSetSimple.h"
  #include "WordList.h"
  #include "LexerBase.h"
  #include "LexAccessor.h"
  #include "Accessor.h"
  #include "LexerModule.h"
  #include "StyleContext.h"
  #include "CharacterSet.h"

  #include "dbgstream.h"
  #define thisfunc __PRETTY_FUNCTION__
#ifdef UNICODE
	#define generic_strncpy_s wcsncpy_s
#else
	#define generic_strncpy_s strncpy_s
#endif

#endif //*/ end includes/defines section
using std::vector;
using std::string;

namespace RnwLang
{
  typedef std::basic_string<TCHAR> generic_string;
  static const string rnwmsg = "RnwLang:MSG:";
  static const string rnwerr = "RnwLang:ERROR:";
	void aboutDlg();
  
  
	const int DEFAULT = 0;

	const int LUA_COMMENT = 1;
	const int CPP_COMMENT = 2;
	const int LUA_COMMENTLINE = 3;
	const int CPP_COMMENTLINE = 4;

	const int NUMBER = 5;
	const int OPERATOR = 6;
	const int IDENTIFIER = 7;

	const int STRING = 8;
	const int CHARACTER = 9;
	const int STRINGEOL = 10;
	const int LITERALSTRING = 11;

	const int WORD0 = 20;
	const int WORD1 = 21;
	const int WORD2 = 22;
	const int WORD3 = 23;
	const int WORD4 = 24;
	const int WORD5 = 25;
	const int WORD6 = 26;
	const int WORD7 = 27;
	const int WORD8 = 28;

  class MenuItem : public FuncItem{
  public:
    MenuItem( generic_string name,         /// Item Name
              PFUNCPLUGINCMD func=NULL,  /// function to execute
              int cmdID = NULL,
              bool i2c = false,
              ShortcutKey * Key = NULL);
    
  };
	class LexerRnw : public LexerBase {
  private:
    vector<MenuItem> MenuItems;
  protected:
    NppData nppData;
	public:
		LexerRnw();
		void SCI_METHOD Lex(unsigned int startPos, int length, int initStyle, IDocument *pAccess);
		void SCI_METHOD Fold(unsigned int startPos, int length, int initStyle, IDocument *pAccess);
		static ILexer *LexerFactory();
    FuncItem * getMenuItems();
    int numMenuItems();
    void setInfo(NppData notpadPlusData);
	};

  // Primary Class used as a Singeton
  extern LexerRnw Rnw;
  
	void Colourise_Doc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[], Accessor &styler);
	void Fold_Doc(unsigned int startPos, int length, int initStyle, Accessor &styler);
}

#endif  // RNWLANG_H

