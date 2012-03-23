/* LexerMD.cpp
 * Markdown lexer for notepad++
 * Andrew Redd (c) 2012
 * Liscened under GPL v2 or newer
 */

#include "LexerMd.h"
using namespace LexerPlugin;
using namespace Lexers::Markdown;


namespace Lexers{
  namespace Markdown{
    SCI_METHOD void LexerMarkdown::Lex(unsigned int startPos, int length, int initStyle, IDocument *pAccess) {
      _debugenter_;
      try {
        Accessor styler(pAccess,   &props);
        ColorizeMarkdownDoc(startPos, length, initStyle, NULL, styler);
      } catch (...) {
        #ifdef DEBUG
        dbg << rnwerr << "Unhandled Exception" << endl;
        #endif
        // Should not throw into caller as may be compiled with different compiler or options
        pAccess->SetErrorStatus(SC_STATUS_FAILURE);
      }
    }
    SCI_METHOD void LexerMarkdown::Fold(unsigned int /* startPos */, int /* length */, int /* initStyle */, IDocument*/* pAccess */) {return;}
  }
}

void aboutDlg()
{
  ::MessageBox(Plugin.nppHandle(),
    TEXT("Markdown Syntax Plugin\n")
    TEXT("http://github.com/halpo/NppRnwLang\n\n")
    TEXT("Author: Andrew Redd\n")
    #ifdef DEBUG
    TEXT("\n\nBuilt:"__DATE__" "__TIME__)
    #endif
    ,
    TEXT("<- About ->"),
    MB_OK);
}
PluginInfo& PluginInfo::MakePlugin(){
  #ifdef DEBUG
  dbg << "LexerMarkdown::MakePlugin::Entering" << std::endl;
  #endif  // DEBUG
  static PluginInfo P(_TEXT("&Markdown Lexer"));
  MenuItem mi_about(_TEXT("&About LexRnwer"), &aboutDlg);
  P.addMenuItem(mi_about);
  static LexerInfo liMd("Markdown", _TEXT("Markdown"), &LexerFactory<Lexers::Markdown::LexerMarkdown>);
  P.addLexer(liMd);
  return P;
}

