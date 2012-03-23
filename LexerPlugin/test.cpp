/*! @file test.cpp
  Test the LexerPlugin
 */

#include "LexerPlugin.h"
#include "plugindebug.h"
void aboutDlg()
{
  ::MessageBox(Plugin.nppHandle(),
    TEXT("Text LexerPlugin\n")
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

