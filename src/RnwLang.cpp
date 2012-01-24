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
//{ Windows Header Files
#include <WinDef.h>
//}
//{ Notepad++
#include "Common.h"
#include "Window.h"
#include "StaticDialog.h"
#include "PluginInterface.h"
//}

#ifdef DEBUG
  #include "dbgstream.h"
  #include "deparse_wm_msg.h"
#endif

#define EXT_LEXER_DECL __declspec( dllexport ) __stdcall

using namespace std;
using namespace RnwLang::Lexers::Rnw;

namespace RnwLang
{
  typedef std::basic_string<TCHAR> generic_string;
	void aboutDlg();


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
    PluginInfo(PluginInfo const &);
    void operator=(PluginInfo const&);
    vector<MenuItem> MenuItems;
  protected:
    NppData nppData;
    HWND _pHandle;
  public:
    PluginInfo();
    void setInfo(NppData notpadPlusData);
    FuncItem * getMenuItems();
    int numMenuItems();
    HWND nppHandle();
    HWND pluginHandle();
    void setPluginHandle(HWND);
  };
  // PluginInfo Class as a Singeton
  PluginInfo Plugin;

  
static const generic_string PLUGIN_NAME      = _TEXT("&Rnw Lexer");
static const std::string LEXER_NAME          = "RnwLang";
static const generic_string LEXER_STATUS_TEXT= _TEXT("R/Sweave Lexer");
static const generic_string aboutMenuItem    = _TEXT("&About R/Sweave");
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
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc << endl;
  #endif
  MenuItem mi_about(_TEXT("&About LexRnwer"), &aboutDlg);
  MenuItems.push_back(mi_about);
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
void PluginInfo::setPluginHandle(HWND pHandle){
  _pHandle=pHandle;
}
HWND PluginInfo::pluginHandle(){
  return _pHandle;
}
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
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc << endl;
  #endif
  return PLUGIN_NAME.c_str(); 
}
void setInfo(NppData notpadPlusData){ 
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc << endl;
  #endif
  Plugin.setInfo(notpadPlusData); 
}
__declspec(dllexport) FuncItem * getFuncsArray(int *nbF){
  #ifdef DEBUG
  dbg << rnwmsg << thisfunc << endl;
  #endif
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
  dbg << rnwmsg << thisfunc << endl;
  #endif
  return 1; 
}
EXT_LEXER_DECL void GetLexerName(unsigned int /*index*/, char *name, int buflength){
  #ifdef DEBUG
  dbg << rnwmsg << thisfunc << endl;
  #endif
	*name = 0;
	if (buflength > 0) {
		strncpy(name, LEXER_NAME.c_str(), buflength);
	}
}
EXT_LEXER_DECL void GetLexerStatusText(unsigned int /*Index*/, TCHAR *desc, int buflength){
  #ifdef DEBUG
  dbg << rnwmsg << thisfunc << endl;
  #endif
	if (buflength > 0) {
    _tcsncpy(desc, LEXER_STATUS_TEXT.c_str(), buflength);
	}
}

__declspec( dllexport ) LexerFactoryFunction GetLexerFactory(unsigned int index) {
  #ifdef DEBUG
  dbg << rnwmsg << "in " << thisfunc << " index=" << index << endl;
  #endif
	if (index == 0)
		return &LexerRnw::LexerFactory;
	else
		return 0;
}
} // End extern "C"

