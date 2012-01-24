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

#ifndef RNWLANG_SRC_RnwLang_H
#define RNWLANG_SRC_RnwLang_H

//{ Includes Section
  #include <Tchar.h>
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501
  #endif
  

  //{ C RunTime Header Files
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
  //}
  //{ STL Headers
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
  //}
  #ifdef UNICODE
    #include <wchar.h>
  #endif
  //{ Notepad++
  //}
  //{ Scintilla includes
  #include "Scintilla.h"
  #include "ILexer.h"
  #include "PropSetSimple.h"
  #include "WordList.h"
  #include "LexerBase.h"
  #include "LexAccessor.h"
  #include "Accessor.h"
  #include "LexerModule.h"
  #include "StyleContext.h"
  #include "CharacterSet.h"
  //}
  //{ RnwLang includes
  #include <cassert>
  #include <algorithm>
  #include "unidef.h"
  #include "lexers.h"
  
  #define thisfunc __PRETTY_FUNCTION__
  #ifdef UNICODE
    #define generic_strncpy_s wcsncpy_s
  #else
    #define generic_strncpy_s strncpy_s
  #endif
  //}
//} end includes/defines section
using std::vector;
using std::string;

namespace RnwLang
{
  namespace Lexers{ namespace Rnw{
	class LexerRnw : public LexerBase {
  private:
    PropSetSimple R_props, TeX_props, Rnw_props;
	public:
		LexerRnw();
		~LexerRnw();
    SCI_METHOD int  WordListSet(int n, const char *wl);
		SCI_METHOD void Lex(unsigned int startPos, int length, int initStyle, IDocument *pAccess);
    SCI_METHOD void Fold(unsigned int startPos, int length, int initStyle, IDocument *pAccess);
		static ILexer* LexerFactory();
    void Style(unsigned int startPos, int length, int initStyle, IDocument* pAccess, bool fold);
    void FoldByLine( int startLine , int endLine , int initStyle , IDocument* pAccess );
 	};
  }} // namespace Lexers::Rnw
  #ifdef DEBUG
  static const string rnwmsg = "RnwLang:MSG:";
  static const string rnwerr = "RnwLang:ERROR:";
  #endif
}

#endif  // RNWLANG_SRC_RNWLANG_H

