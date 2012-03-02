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
  //{ Windows Header Files
    #include <WinDef.h>
  //}
  //{ Notepad++
    #include "Common.h"
    #include "Window.h"
    #include "StaticDialog.h"
    #include "PluginInterface.h"
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

namespace LexerPlugin
{
  //typedef std::basic_string<TCHAR> generic_string;
  class LexerInfo{
   public:
    string name;
    generic_string status;
    LexerFactoryFunction factory;
    LexerInfo(string n, generic_string s, LexerFactoryFunction f):name(n), status(s), factory(f){}
    const char* getName() const {return name.c_str();}
    const TCHAR* getStatus() const {return status.c_str();}
    LexerFactoryFunction getFactory() const {return factory;}
  };
  class MenuItem : public FuncItem{
  public:
    MenuItem( generic_string name,         /// Item Name
              PFUNCPLUGINCMD func=NULL,  /// function to execute
              int cmdID = NULL,
              bool i2c = false,
              ShortcutKey * Key = NULL);
  };
  class PluginInfo{
  private:
    generic_string _name;   // = _TEXT("&Rnw Lexer");
    NppData nppData;
    HWND _pHandle;
    vector<LexerInfo> Lexers;
    vector<MenuItem> MenuItems;
    PluginInfo(generic_string name):_name(name){};
    void addLexer(LexerInfo l){Lexers.push_back(l);}
    void addMenuItem(MenuItem MI){MenuItems.push_back(MI);}
  protected:
  public:
    void setInfo(NppData notpadPlusData);
    FuncItem * getMenuItems();
    const TCHAR* getName() const {return _name.c_str();}
    int numMenuItems();
    HWND nppHandle();
    HWND pluginHandle();
    HWND CurrScintillaHandle();
    void setPluginHandle(HWND);
    int NLexers(){return Lexers.size();}
    LexerInfo getLexer(int index){return Lexers[index];}
    static const generic_string PLUGIN_NAME;
    static const generic_string aboutMenuItem;
    static PluginInfo& MakePlugin();
  };
  extern PluginInfo Plugin;

  
 
  //Styler Functions
  unsigned int SetEOLFilledLine(int line, LexAccessor& styler);
  unsigned int SetEOLFilledAt(unsigned int pos);
  unsigned int SetEOLFilledAll(LexAccessor& styler);
  unsigned int CheckEOLFilled(LexAccessor& styler, bool showHits = false);
}
#endif  // RNWLANG_SRC_RNWLANG_H

