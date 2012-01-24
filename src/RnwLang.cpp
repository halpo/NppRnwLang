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

#include "RnwLang.h"

#define EXT_LEXER_DECL __declspec( dllexport ) __stdcall

using namespace std;
using namespace RnwLang::Lexers::Rnw;

namespace RnwLang
{
static const generic_string PLUGIN_NAME      = TEXT("&Rnw Lexer");
static const std::string LEXER_NAME          = "RnwLang";
static const generic_string LEXER_STATUS_TEXT= TEXT("R/Sweave Lexer");
static const generic_string aboutMenuItem    = TEXT("&About R/Sweave");
void aboutDlg()
{
  ::MessageBox(Plugin.nppHandle(),
    TEXT("R/Sweave Syntax Plugin\n")
    TEXT("http://github.com/halpo/NppRnwLang\n\n")
    TEXT("               Author: Andrew Redd\n")
    TEXT("                        (aka halpo)"),
    TEXT("<- About ->"),
    MB_OK);
}
RnwLang::MenuItem::MenuItem(
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

PluginInfo::PluginInfo(){
  dbg << rnwmsg << "in " << thisfunc << endl;
  MenuItem mi_about(TEXT("&About LexRnwer"), &aboutDlg);
  MenuItems.push_back(mi_about);
}
FuncItem * PluginInfo::getMenuItems(){
  return reinterpret_cast<FuncItem*>(&MenuItems[0]);
}
int PluginInfo::numMenuItems(){
  return MenuItems.size();
}
void PluginInfo::setInfo(NppData notpadPlusData){ 
  dbg << rnwmsg << "in " << thisfunc << endl;
  nppData = notpadPlusData; 
}
HWND PluginInfo::nppHandle(){
  return nppData._nppHandle;
}
void PluginInfo::setPluginHandle(HWND pHandle){
  _pHandle=pHandle;
}
HWND PluginInfo::pluginHandle(){
  return _pHandle;
}

PluginInfo Plugin;
}
using namespace RnwLang;


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
    dbg << rnwmsg << "DllMain:DLL_PROCESS_ATTACH" << endl;
    Plugin.setPluginHandle( static_cast<HWND>(hModule));
		break;
	case DLL_THREAD_ATTACH:
    dbg << rnwmsg << "DllMain:DLL_THREAD_ATTACH" << endl;
		break;
	case DLL_THREAD_DETACH:
    dbg << rnwmsg << "DllMain:DLL_THREAD_DETACH" << endl;
		break;
	case DLL_PROCESS_DETACH:
    dbg << rnwmsg << ("DllMain:DLL_PROCESS_DETACH") << endl;
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
  dbg << rnwmsg << "in " << thisfunc << endl;
  return PLUGIN_NAME.c_str(); 
}
void setInfo(NppData notpadPlusData){ 
  dbg << rnwmsg << "in " << thisfunc << endl;
  Plugin.setInfo(notpadPlusData); 
}
__declspec(dllexport) FuncItem * getFuncsArray(int *nbF){
  dbg << rnwmsg << thisfunc << endl;
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
    dbg << rnwmsg << "Message received " << deparseMsg(Message)  << endl;
  }
  return TRUE; 
}
EXT_LEXER_DECL int  GetLexerCount() { 
  dbg << rnwmsg << thisfunc << endl;
  return 1; 
}
EXT_LEXER_DECL void GetLexerName(unsigned int /*index*/, char *name, int buflength){
  dbg << rnwmsg << thisfunc << endl;
	*name = 0;
	if (buflength > 0) {
		strncpy(name, LEXER_NAME.c_str(), buflength);
	}
}
EXT_LEXER_DECL void GetLexerStatusText(unsigned int /*Index*/, TCHAR *desc, int buflength){
  dbg << rnwmsg << thisfunc << endl;
	if (buflength > 0) {
    _tcsncpy(desc, LEXER_STATUS_TEXT.c_str(), buflength);
	}
}

__declspec( dllexport ) LexerFactoryFunction GetLexerFactory(unsigned int index) {
  dbg << rnwmsg << "in " << thisfunc << " index=" << index << endl;
	if (index == 0)
		return &LexerRnw::LexerFactory;
	else
		return 0;
}
} // End extern "C"

