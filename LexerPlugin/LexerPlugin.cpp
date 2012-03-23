// This file is part of RnwLang.
// A plugin written for Notepad++

// Copyright (C)2008-2010 Kyle Fleming ( garthex@gmail.com )

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

#include "LexerPlugin.h"
#include "PluginDebug.h"

#define EXT_LEXER_DECL __declspec( dllexport ) __stdcall

using namespace std;

namespace LexerPlugin
{


  // PluginInfo Class as a Singeton
  PluginInfo Plugin = PluginInfo::MakePlugin();


MenuItem::MenuItem(
                    generic_string name /// Item Name
                   ,PFUNCPLUGINCMD func /// = NULL  function to execute
                   ,int cmdID           /// = NULL
                   ,bool i2c            /// = false
                   ,ShortcutKey * Key   /// = NULL
){
  _pFunc=(func);
  _cmdID=(cmdID);
  _init2Check=(i2c);
  _pShKey=(Key);
	_tcsncpy(_itemName, name.c_str(), nbChar);
}


FuncItem * PluginInfo::getMenuItems(){
  return reinterpret_cast<FuncItem*>(&MenuItems[0]);
}
int PluginInfo::numMenuItems(){
  return MenuItems.size();
}
void PluginInfo::setInfo(NppData notpadPlusData){ 
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc << endl;
  #endif
  nppData = notpadPlusData; 
}
HWND PluginInfo::nppHandle(){
  return nppData._nppHandle;
}
HWND PluginInfo::CurrScintillaHandle(){
  return nppData._scintillaMainHandle;
}
void PluginInfo::setPluginHandle(HWND pHandle){
  _pHandle=pHandle;
}
HWND PluginInfo::pluginHandle(){
  return _pHandle;
}



unsigned int SendScintillaMSG(int msg, unsigned int wParam = 0, int lParam = 0){
  HWND hwndScintilla = Plugin.CurrScintillaHandle();
	int (*fn)(void*,int,int,int);
	fn = (int (__cdecl *)(void *,int,int,int))SendMessage(
		hwndScintilla,SCI_GETDIRECTFUNCTION,0,0);
	void * ptr;
	ptr = (void *)(SendMessage(hwndScintilla,SCI_GETDIRECTPOINTER,0,0));
  return fn(ptr, msg, wParam, lParam);
}


///Style Helper Function
unsigned int SetEOLFilledLine(int line, LexAccessor& styler){
  #ifdef DEBUG
  dbg << rnwmsg << __func__ << "(" << line << ",...):Entering"  << endl;
  #endif
	unsigned int lineend = SendScintillaMSG(SCI_GETLINEENDPOSITION, line, 1);
	unsigned int result  = SetEOLFilledAt(lineend);
  #ifdef DEBUG
  dbg << rnwmsg << __func__ << ":lineend=" << lineend << endl;
  dbg << rnwmsg << __func__ << ":Leaving("  << result << ")" << endl;
  #endif
  return result;
}
unsigned int SetEOLFilledAt(unsigned int pos){
	return SendScintillaMSG(SCI_STYLESETEOLFILLED, pos);
}
unsigned int SetEOLFilledAll(LexAccessor& styler){
  for(int i=0; i<styler.Length();i++)
    SetEOLFilledAt(i);
  return 0;
}
unsigned int CheckEOLFilled(LexAccessor& styler, bool showHits /* =false */){
  #ifdef DEBUG
  dbg << rnwmsg << __func__ << endl;
  for(int i=0; i<styler.Length();i++){
    int isfill = SetEOLFilledAt(i);
    if(isfill != showHits)
      dbg << rnwmsg << __func__ << ":At " << i << " isfill=" << isfill << endl;
  }
  dbg << rnwmsg << __func__ << ":leaving" << endl;
  #endif
  return 0;
}
}
using namespace LexerPlugin;


extern "C" { /// All the interface stuff

//! Main for dll call
/// 
/// NOTE: as of 1/10/2012 Notepad++ version 5.9.3 this function is not called when loading. 
BOOL APIENTRY DllMain(  HANDLE hModule,      /// module handle, NOT USED
                        DWORD reasonForCall, /// Why the call 
                        LPVOID               /// lpReserved NOT USED
){
	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
    #ifdef DEBUG
    dbg << rnwmsg << "DllMain:DLL_PROCESS_ATTACH" << endl;
    #endif
    Plugin.setPluginHandle( static_cast<HWND>(hModule));
		break;
	case DLL_THREAD_ATTACH:
    #ifdef DEBUG
    dbg << rnwmsg << "DllMain:DLL_THREAD_ATTACH" << endl;
    #endif
		break;
	case DLL_THREAD_DETACH:
    #ifdef DEBUG
    dbg << rnwmsg << "DllMain:DLL_THREAD_DETACH" << endl;
    #endif
		break;
	case DLL_PROCESS_DETACH:
    #ifdef DEBUG
    dbg << rnwmsg << ("DllMain:DLL_PROCESS_DETACH") << endl;
    #endif
		break;
	}
	return TRUE;
}

#ifdef UNICODE
__declspec(dllexport) BOOL isUnicode() {
	return TRUE;
}
#endif

__declspec(dllexport) const TCHAR * getName() { 
  _debugenter_;
  return Plugin.getName(); 
}
void setInfo(NppData notpadPlusData){ 
  _debugenter_;
  Plugin.setInfo(notpadPlusData); 
}
__declspec(dllexport) FuncItem * getFuncsArray(int *nbF){
  _debugenter_;
	*nbF = Plugin.numMenuItems();
	return Plugin.getMenuItems();
}
__declspec(dllexport) void beNotified(SCNotification * /*notifyCode*/) {}
__declspec(dllexport) LRESULT messageProc(
    UINT Message   /// Message ID
  , WPARAM         /// wParam, NOT USED
  , LPARAM         /// lParam, NOT USED
) {
  switch(Message){
  case WM_MOVE:
  case WM_MOVING:
  case WM_SIZING:
  case WM_SIZE:
    break;
  default:
    #ifdef DEBUG
    dbg << rnwmsg << "Message received " << deparseMsg(Message)  << endl;
    #endif
    break;
  }
  return TRUE; 
}
EXT_LEXER_DECL int  GetLexerCount() { 
  #ifdef DEBUG
  dbg << rnwmsg << thisfunc << ":#lexers=" << Plugin.NLexers() <<  endl;
  #endif
  return Plugin.NLexers(); 
}
EXT_LEXER_DECL void GetLexerName(unsigned int index, char *name, int buflength){
  #ifdef DEBUG
  dbg << rnwmsg << thisfunc << endl;
  #endif
	*name = 0;
	if (buflength > 0) {
		strncpy(name,Plugin.getLexer(index).getName(), buflength);
	}
}
EXT_LEXER_DECL void GetLexerStatusText(unsigned int index, TCHAR *desc, int buflength){
  #ifdef DEBUG
  dbg << rnwmsg << thisfunc 
      << "("  << index
      << ")"  << endl;
  #endif
	if (buflength > 0) {
    _tcsncpy(desc, Plugin.getLexer(index).getStatus(), buflength);
	}
}

__declspec( dllexport ) LexerFactoryFunction GetLexerFactory(unsigned int index) {
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc << " index=" << index << endl;
  #endif
	// if (index == 0)
		return Plugin.getLexer(index).getFactory();
	// else
		// return 0;
}
} // End extern "C"

