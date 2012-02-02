#include "deparse_wm_msg.h"
#include "lexers.h" // for RnwLang enum definitions
#include <map>
#include <sstream>
#define _STRINGIFY_(x) m[x]=#x;



using namespace std;
using namespace RnwLang::Lexers;

namespace{  // local functions to this file
//{ Define windows messages
int WM_NULL = 0x00;
int WM_CREATE = 0x01;
int WM_DESTROY = 0x02;
int WM_MOVE = 0x03;
int WM_SIZE = 0x05;
int WM_ACTIVATE = 0x06;
int WM_SETFOCUS = 0x07;
int WM_KILLFOCUS = 0x08;
int WM_ENABLE = 0x0A;
int WM_SETREDRAW = 0x0B;
int WM_SETTEXT = 0x0C;
int WM_GETTEXT = 0x0D;
int WM_GETTEXTLENGTH = 0x0E;
int WM_PAINT = 0x0F;
int WM_CLOSE = 0x10;
int WM_QUERYENDSESSION = 0x11;
int WM_QUIT = 0x12;
int WM_QUERYOPEN = 0x13;
int WM_ERASEBKGND = 0x14;
int WM_SYSCOLORCHANGE = 0x15;
int WM_ENDSESSION = 0x16;
int WM_SYSTEMERROR = 0x17;
int WM_SHOWWINDOW = 0x18;
int WM_CTLCOLOR = 0x19;
int WM_WININICHANGE = 0x1A;
int WM_SETTINGCHANGE = 0x1A;
int WM_DEVMODECHANGE = 0x1B;
int WM_ACTIVATEAPP = 0x1C;
int WM_FONTCHANGE = 0x1D;
int WM_TIMECHANGE = 0x1E;
int WM_CANCELMODE = 0x1F;
int WM_SETCURSOR = 0x20;
int WM_MOUSEACTIVATE = 0x21;
int WM_CHILDACTIVATE = 0x22;
int WM_QUEUESYNC = 0x23;
int WM_GETMINMAXINFO = 0x24;
int WM_PAINTICON = 0x26;
int WM_ICONERASEBKGND = 0x27;
int WM_NEXTDLGCTL = 0x28;
int WM_SPOOLERSTATUS = 0x2A;
int WM_DRAWITEM = 0x2B;
int WM_MEASUREITEM = 0x2C;
int WM_DELETEITEM = 0x2D;
int WM_VKEYTOITEM = 0x2E;
int WM_CHARTOITEM = 0x2F;

int WM_SETFONT = 0x30;
int WM_GETFONT = 0x31;
int WM_SETHOTKEY = 0x32;
int WM_GETHOTKEY = 0x33;
int WM_QUERYDRAGICON = 0x37;
int WM_COMPAREITEM = 0x39;
int WM_COMPACTING = 0x41;
int WM_WINDOWPOSCHANGING = 0x46;
int WM_WINDOWPOSCHANGED = 0x47;
int WM_POWER = 0x48;
int WM_COPYDATA = 0x4A;
int WM_CANCELJOURNAL = 0x4B;
int WM_NOTIFY = 0x4E;
int WM_INPUTLANGCHANGEREQUEST = 0x50;
int WM_INPUTLANGCHANGE = 0x51;
int WM_TCARD = 0x52;
int WM_HELP = 0x53;
int WM_USERCHANGED = 0x54;
int WM_NOTIFYFORMAT = 0x55;
int WM_CONTEXTMENU = 0x7B;
int WM_STYLECHANGING = 0x7C;
int WM_STYLECHANGED = 0x7D;
int WM_DISPLAYCHANGE = 0x7E;
int WM_GETICON = 0x7F;
int WM_SETICON = 0x80;

int WM_NCCREATE = 0x81;
int WM_NCDESTROY = 0x82;
int WM_NCCALCSIZE = 0x83;
int WM_NCHITTEST = 0x84;
int WM_NCPAINT = 0x85;
int WM_NCACTIVATE = 0x86;
int WM_GETDLGCODE = 0x87;
int WM_NCMOUSEMOVE = 0xA0;
int WM_NCLBUTTONDOWN = 0xA1;
int WM_NCLBUTTONUP = 0xA2;
int WM_NCLBUTTONDBLCLK = 0xA3;
int WM_NCRBUTTONDOWN = 0xA4;
int WM_NCRBUTTONUP = 0xA5;
int WM_NCRBUTTONDBLCLK = 0xA6;
int WM_NCMBUTTONDOWN = 0xA7;
int WM_NCMBUTTONUP = 0xA8;
int WM_NCMBUTTONDBLCLK = 0xA9;

int WM_KEYFIRST = 0x100;
int WM_KEYDOWN = 0x100;
int WM_KEYUP = 0x101;
int WM_CHAR = 0x102;
int WM_DEADCHAR = 0x103;
int WM_SYSKEYDOWN = 0x104;
int WM_SYSKEYUP = 0x105;
int WM_SYSCHAR = 0x106;
int WM_SYSDEADCHAR = 0x107;
int WM_KEYLAST = 0x108;

int WM_IME_STARTCOMPOSITION = 0x10D;
int WM_IME_ENDCOMPOSITION = 0x10E;
int WM_IME_COMPOSITION = 0x10F;
int WM_IME_KEYLAST = 0x10F;

int WM_INITDIALOG = 0x110;
int WM_COMMAND = 0x111;
int WM_SYSCOMMAND = 0x112;
int WM_TIMER = 0x113;
int WM_HSCROLL = 0x114;
int WM_VSCROLL = 0x115;
int WM_INITMENU = 0x116;
int WM_INITMENUPOPUP = 0x117;
int WM_MENUSELECT = 0x11F;
int WM_MENUCHAR = 0x120;
int WM_ENTERIDLE = 0x121;

int WM_CTLCOLORMSGBOX = 0x132;
int WM_CTLCOLOREDIT = 0x133;
int WM_CTLCOLORLISTBOX = 0x134;
int WM_CTLCOLORBTN = 0x135;
int WM_CTLCOLORDLG = 0x136;
int WM_CTLCOLORSCROLLBAR = 0x137;
int WM_CTLCOLORSTATIC = 0x138;

int WM_MOUSEFIRST = 0x200;
int WM_MOUSEMOVE = 0x200;
int WM_LBUTTONDOWN = 0x201;
int WM_LBUTTONUP = 0x202;
int WM_LBUTTONDBLCLK = 0x203;
int WM_RBUTTONDOWN = 0x204;
int WM_RBUTTONUP = 0x205;
int WM_RBUTTONDBLCLK = 0x206;
int WM_MBUTTONDOWN = 0x207;
int WM_MBUTTONUP = 0x208;
int WM_MBUTTONDBLCLK = 0x209;
int WM_MOUSEWHEEL = 0x20A;
int WM_MOUSEHWHEEL = 0x20E;

int WM_PARENTNOTIFY = 0x210;
int WM_ENTERMENULOOP = 0x211;
int WM_EXITMENULOOP = 0x212;
int WM_NEXTMENU = 0x213;
int WM_SIZING = 0x214;
int WM_CAPTURECHANGED = 0x215;
int WM_MOVING = 0x216;
int WM_POWERBROADCAST = 0x218;
int WM_DEVICECHANGE = 0x219;

int WM_MDICREATE = 0x220;
int WM_MDIDESTROY = 0x221;
int WM_MDIACTIVATE = 0x222;
int WM_MDIRESTORE = 0x223;
int WM_MDINEXT = 0x224;
int WM_MDIMAXIMIZE = 0x225;
int WM_MDITILE = 0x226;
int WM_MDICASCADE = 0x227;
int WM_MDIICONARRANGE = 0x228;
int WM_MDIGETACTIVE = 0x229;
int WM_MDISETMENU = 0x230;
int WM_ENTERSIZEMOVE = 0x231;
int WM_EXITSIZEMOVE = 0x232;
int WM_DROPFILES = 0x233;
int WM_MDIREFRESHMENU = 0x234;

int WM_IME_SETCONTEXT = 0x281;
int WM_IME_NOTIFY = 0x282;
int WM_IME_CONTROL = 0x283;
int WM_IME_COMPOSITIONFULL = 0x284;
int WM_IME_SELECT = 0x285;
int WM_IME_CHAR = 0x286;
int WM_IME_KEYDOWN = 0x290;
int WM_IME_KEYUP = 0x291;

int WM_MOUSEHOVER = 0x2A1;
int WM_NCMOUSELEAVE = 0x2A2;
int WM_MOUSELEAVE = 0x2A3;

int WM_CUT = 0x300;
int WM_COPY = 0x301;
int WM_PASTE = 0x302;
int WM_CLEAR = 0x303;
int WM_UNDO = 0x304;

int WM_RENDERFORMAT = 0x305;
int WM_RENDERALLFORMATS = 0x306;
int WM_DESTROYCLIPBOARD = 0x307;
int WM_DRAWCLIPBOARD = 0x308;
int WM_PAINTCLIPBOARD = 0x309;
int WM_VSCROLLCLIPBOARD = 0x30A;
int WM_SIZECLIPBOARD = 0x30B;
int WM_ASKCBFORMATNAME = 0x30C;
int WM_CHANGECBCHAIN = 0x30D;
int WM_HSCROLLCLIPBOARD = 0x30E;
int WM_QUERYNEWPALETTE = 0x30F;
int WM_PALETTEISCHANGING = 0x310;
int WM_PALETTECHANGED = 0x311;

int WM_HOTKEY = 0x312;
int WM_PRINT = 0x317;
int WM_PRINTCLIENT = 0x318;

int WM_HANDHELDFIRST = 0x358;
int WM_HANDHELDLAST = 0x35F;
int WM_PENWINFIRST = 0x380;
int WM_PENWINLAST = 0x38F;
int WM_COALESCE_FIRST = 0x390;
int WM_COALESCE_LAST = 0x39F;
int WM_DDE_FIRST = 0x3E0;
int WM_DDE_INITIATE = 0x3E0;
int WM_DDE_TERMINATE = 0x3E1;
int WM_DDE_ADVISE = 0x3E2;
int WM_DDE_UNADVISE = 0x3E3;
int WM_DDE_ACK = 0x3E4;
int WM_DDE_DATA = 0x3E5;
int WM_DDE_REQUEST = 0x3E6;
int WM_DDE_POKE = 0x3E7;
int WM_DDE_EXECUTE = 0x3E8;
int WM_DDE_LAST = 0x3E8;

int WM_USER = 0x400;
int WM_APP = 0x8000;
//}

typedef map<int,string> ismap;
static ismap WindowsMap()
{
  ismap m;
  _STRINGIFY_(WM_NULL)
  _STRINGIFY_(WM_CREATE)
  _STRINGIFY_(WM_DESTROY)
  _STRINGIFY_(WM_MOVE)
  _STRINGIFY_(WM_SIZE)
  _STRINGIFY_(WM_ACTIVATE)
  _STRINGIFY_(WM_SETFOCUS)
  _STRINGIFY_(WM_KILLFOCUS)
  _STRINGIFY_(WM_ENABLE)
  _STRINGIFY_(WM_SETREDRAW)
  _STRINGIFY_(WM_SETTEXT)
  _STRINGIFY_(WM_GETTEXT)
  _STRINGIFY_(WM_GETTEXTLENGTH)
  _STRINGIFY_(WM_PAINT)
  _STRINGIFY_(WM_CLOSE)
  _STRINGIFY_(WM_QUERYENDSESSION)
  _STRINGIFY_(WM_QUIT)
  _STRINGIFY_(WM_QUERYOPEN)
  _STRINGIFY_(WM_ERASEBKGND)
  _STRINGIFY_(WM_SYSCOLORCHANGE)
  _STRINGIFY_(WM_ENDSESSION)
  _STRINGIFY_(WM_SHOWWINDOW)
  _STRINGIFY_(WM_WININICHANGE)
  _STRINGIFY_(WM_DEVMODECHANGE)
  _STRINGIFY_(WM_ACTIVATEAPP)
  _STRINGIFY_(WM_FONTCHANGE)
  _STRINGIFY_(WM_TIMECHANGE)
  _STRINGIFY_(WM_CANCELMODE)
  _STRINGIFY_(WM_SETCURSOR)
  _STRINGIFY_(WM_MOUSEACTIVATE)
  _STRINGIFY_(WM_CHILDACTIVATE)
  _STRINGIFY_(WM_QUEUESYNC)
  _STRINGIFY_(WM_GETMINMAXINFO)
  _STRINGIFY_(WM_PAINTICON)
  _STRINGIFY_(WM_ICONERASEBKGND)
  _STRINGIFY_(WM_NEXTDLGCTL)
  _STRINGIFY_(WM_SPOOLERSTATUS)
  _STRINGIFY_(WM_DRAWITEM)
  _STRINGIFY_(WM_MEASUREITEM)
  _STRINGIFY_(WM_DELETEITEM)
  _STRINGIFY_(WM_VKEYTOITEM)
  _STRINGIFY_(WM_CHARTOITEM)
  _STRINGIFY_(WM_SETFONT)
  _STRINGIFY_(WM_GETFONT)
  _STRINGIFY_(WM_SETHOTKEY)
  _STRINGIFY_(WM_GETHOTKEY)
  _STRINGIFY_(WM_QUERYDRAGICON)
  _STRINGIFY_(WM_COMPAREITEM)
  _STRINGIFY_(WM_COMPACTING)
  _STRINGIFY_(WM_WINDOWPOSCHANGING)
  _STRINGIFY_(WM_WINDOWPOSCHANGED)
  _STRINGIFY_(WM_POWER)
  _STRINGIFY_(WM_COPYDATA)
  _STRINGIFY_(WM_CANCELJOURNAL)
  _STRINGIFY_(WM_NOTIFY)
  _STRINGIFY_(WM_INPUTLANGCHANGEREQUEST)
  _STRINGIFY_(WM_INPUTLANGCHANGE)
  _STRINGIFY_(WM_TCARD)
  _STRINGIFY_(WM_HELP)
  _STRINGIFY_(WM_USERCHANGED)
  _STRINGIFY_(WM_NOTIFYFORMAT)
  _STRINGIFY_(WM_CONTEXTMENU)
  _STRINGIFY_(WM_STYLECHANGING)
  _STRINGIFY_(WM_STYLECHANGED)
  _STRINGIFY_(WM_DISPLAYCHANGE)
  _STRINGIFY_(WM_GETICON)
  _STRINGIFY_(WM_SETICON)
  _STRINGIFY_(WM_NCCREATE)
  _STRINGIFY_(WM_NCDESTROY)
  _STRINGIFY_(WM_NCCALCSIZE)
  _STRINGIFY_(WM_NCHITTEST)
  _STRINGIFY_(WM_NCPAINT)
  _STRINGIFY_(WM_NCACTIVATE)
  _STRINGIFY_(WM_GETDLGCODE)
  _STRINGIFY_(WM_NCMOUSEMOVE)
  _STRINGIFY_(WM_NCLBUTTONDOWN)
  _STRINGIFY_(WM_NCLBUTTONUP)
  _STRINGIFY_(WM_NCLBUTTONDBLCLK)
  _STRINGIFY_(WM_NCRBUTTONDOWN)
  _STRINGIFY_(WM_NCRBUTTONUP)
  _STRINGIFY_(WM_NCRBUTTONDBLCLK)
  _STRINGIFY_(WM_NCMBUTTONDOWN)
  _STRINGIFY_(WM_NCMBUTTONUP)
  _STRINGIFY_(WM_NCMBUTTONDBLCLK)
  _STRINGIFY_(WM_KEYDOWN)
  _STRINGIFY_(WM_KEYFIRST)
  _STRINGIFY_(WM_KEYUP)
  _STRINGIFY_(WM_CHAR)
  _STRINGIFY_(WM_DEADCHAR)
  _STRINGIFY_(WM_SYSKEYDOWN)
  _STRINGIFY_(WM_SYSKEYUP)
  _STRINGIFY_(WM_SYSCHAR)
  _STRINGIFY_(WM_SYSDEADCHAR)
  _STRINGIFY_(WM_KEYLAST)
  _STRINGIFY_(WM_IME_STARTCOMPOSITION)
  _STRINGIFY_(WM_IME_ENDCOMPOSITION)
  _STRINGIFY_(WM_IME_COMPOSITION)
  _STRINGIFY_(WM_IME_KEYLAST)
  _STRINGIFY_(WM_INITDIALOG)
  _STRINGIFY_(WM_COMMAND)
  _STRINGIFY_(WM_SYSCOMMAND)
  _STRINGIFY_(WM_TIMER)
  _STRINGIFY_(WM_HSCROLL)
  _STRINGIFY_(WM_VSCROLL)
  _STRINGIFY_(WM_INITMENU)
  _STRINGIFY_(WM_INITMENUPOPUP)
  _STRINGIFY_(WM_MENUSELECT)
  _STRINGIFY_(WM_MENUCHAR)
  _STRINGIFY_(WM_ENTERIDLE)
  _STRINGIFY_(WM_CTLCOLORMSGBOX)
  _STRINGIFY_(WM_CTLCOLOREDIT)
  _STRINGIFY_(WM_CTLCOLORLISTBOX)
  _STRINGIFY_(WM_CTLCOLORBTN)
  _STRINGIFY_(WM_CTLCOLORDLG)
  _STRINGIFY_(WM_CTLCOLORSCROLLBAR)
  _STRINGIFY_(WM_CTLCOLORSTATIC)
  _STRINGIFY_(WM_MOUSEFIRST)
  _STRINGIFY_(WM_MOUSEMOVE)
  _STRINGIFY_(WM_LBUTTONDOWN)
  _STRINGIFY_(WM_LBUTTONUP)
  _STRINGIFY_(WM_LBUTTONDBLCLK)
  _STRINGIFY_(WM_RBUTTONDOWN)
  _STRINGIFY_(WM_RBUTTONUP)
  _STRINGIFY_(WM_RBUTTONDBLCLK)
  _STRINGIFY_(WM_MBUTTONDOWN)
  _STRINGIFY_(WM_MBUTTONUP)
  _STRINGIFY_(WM_MBUTTONDBLCLK)
  _STRINGIFY_(WM_MOUSEWHEEL)
  _STRINGIFY_(WM_PARENTNOTIFY)
  _STRINGIFY_(WM_ENTERMENULOOP)
  _STRINGIFY_(WM_EXITMENULOOP)
  _STRINGIFY_(WM_NEXTMENU)
  _STRINGIFY_(WM_SIZING)
  _STRINGIFY_(WM_CAPTURECHANGED)
  _STRINGIFY_(WM_MOVING)
  _STRINGIFY_(WM_POWERBROADCAST)
  _STRINGIFY_(WM_DEVICECHANGE)
  _STRINGIFY_(WM_MDICREATE)
  _STRINGIFY_(WM_MDIDESTROY)
  _STRINGIFY_(WM_MDIACTIVATE)
  _STRINGIFY_(WM_MDIRESTORE)
  _STRINGIFY_(WM_MDINEXT)
  _STRINGIFY_(WM_MDIMAXIMIZE)
  _STRINGIFY_(WM_MDITILE)
  _STRINGIFY_(WM_MDICASCADE)
  _STRINGIFY_(WM_MDIICONARRANGE)
  _STRINGIFY_(WM_MDIGETACTIVE)
  _STRINGIFY_(WM_MDISETMENU)
  _STRINGIFY_(WM_ENTERSIZEMOVE)
  _STRINGIFY_(WM_EXITSIZEMOVE)
  _STRINGIFY_(WM_DROPFILES)
  _STRINGIFY_(WM_MDIREFRESHMENU)
  _STRINGIFY_(WM_IME_SETCONTEXT)
  _STRINGIFY_(WM_IME_NOTIFY)
  _STRINGIFY_(WM_IME_CONTROL)
  _STRINGIFY_(WM_IME_COMPOSITIONFULL)
  _STRINGIFY_(WM_IME_SELECT)
  _STRINGIFY_(WM_IME_CHAR)
  _STRINGIFY_(WM_IME_KEYDOWN)
  _STRINGIFY_(WM_IME_KEYUP)
  _STRINGIFY_(WM_MOUSEHOVER)
  _STRINGIFY_(WM_NCMOUSELEAVE)
  _STRINGIFY_(WM_MOUSELEAVE)
  _STRINGIFY_(WM_CUT)
  _STRINGIFY_(WM_COPY)
  _STRINGIFY_(WM_PASTE)
  _STRINGIFY_(WM_CLEAR)
  _STRINGIFY_(WM_UNDO)
  _STRINGIFY_(WM_RENDERFORMAT)
  _STRINGIFY_(WM_RENDERALLFORMATS)
  _STRINGIFY_(WM_DESTROYCLIPBOARD)
  _STRINGIFY_(WM_DRAWCLIPBOARD)
  _STRINGIFY_(WM_PAINTCLIPBOARD)
  _STRINGIFY_(WM_VSCROLLCLIPBOARD)
  _STRINGIFY_(WM_SIZECLIPBOARD)
  _STRINGIFY_(WM_ASKCBFORMATNAME)
  _STRINGIFY_(WM_CHANGECBCHAIN)
  _STRINGIFY_(WM_HSCROLLCLIPBOARD)
  _STRINGIFY_(WM_QUERYNEWPALETTE)
  _STRINGIFY_(WM_PALETTEISCHANGING)
  _STRINGIFY_(WM_PALETTECHANGED)
  _STRINGIFY_(WM_HOTKEY)
  _STRINGIFY_(WM_PRINT)
  _STRINGIFY_(WM_PRINTCLIENT)
  _STRINGIFY_(WM_HANDHELDFIRST)
  _STRINGIFY_(WM_HANDHELDLAST)
  _STRINGIFY_(WM_PENWINFIRST)
  _STRINGIFY_(WM_PENWINLAST)
  
  _STRINGIFY_(WM_SYSTEMERROR)
  _STRINGIFY_(WM_CTLCOLOR)
  _STRINGIFY_(WM_SETTINGCHANGE)
  _STRINGIFY_(WM_MOUSEHWHEEL)
  _STRINGIFY_(WM_COALESCE_FIRST)
  _STRINGIFY_(WM_COALESCE_LAST)
  _STRINGIFY_(WM_DDE_FIRST)
  _STRINGIFY_(WM_DDE_INITIATE)
  _STRINGIFY_(WM_DDE_TERMINATE)
  _STRINGIFY_(WM_DDE_ADVISE)
  _STRINGIFY_(WM_DDE_UNADVISE)
  _STRINGIFY_(WM_DDE_ACK)
  _STRINGIFY_(WM_DDE_DATA)
  _STRINGIFY_(WM_DDE_REQUEST)
  _STRINGIFY_(WM_DDE_POKE)
  _STRINGIFY_(WM_DDE_EXECUTE)
  _STRINGIFY_(WM_DDE_LAST)
  _STRINGIFY_(WM_USER)
  _STRINGIFY_(WM_APP)
  return m;
}
class MsgMap : public ismap
{
  private:
    string identifier;
  public:
    MsgMap(){}
    MsgMap(string s):identifier(s){}
    string operator[](int index){
      MsgMap::iterator loc = find(index);
      if(loc == end()){
        ostringstream notfound;
        notfound << "<" << identifier << "(" << index <<") unknown>";
        return notfound.str();
      } else {
        return (*loc).second;
      }
    }
    MsgMap(ismap const &rhs):ismap(rhs){}
};
static ismap StyleMap(){
  using namespace RnwLang;
  ismap m;
  _STRINGIFY_(TEX_DEFAULT)
  _STRINGIFY_(TEX_SPECIAL)
  _STRINGIFY_(TEX_GROUP)
  _STRINGIFY_(TEX_SYMBOL)
  _STRINGIFY_(TEX_COMMAND)
  _STRINGIFY_(TEX_TEXT)
  _STRINGIFY_(TEX_RNW_SPECIAL)
  _STRINGIFY_(RNW_DEFAULT)
  _STRINGIFY_(RNW_REUSE)
  _STRINGIFY_(RNW_OPERATOR)
  _STRINGIFY_(RNW_COMMENT)
  _STRINGIFY_(RNW_NAME)
  _STRINGIFY_(RNW_KEYWORD)
  _STRINGIFY_(R_DEFAULT)
  _STRINGIFY_(R_COMMENT)
  _STRINGIFY_(R_KWORD)
  _STRINGIFY_(R_BASEKWORD)
  _STRINGIFY_(R_OTHERKWORD)
  _STRINGIFY_(R_NUMBER)
  _STRINGIFY_(R_STRING)
  _STRINGIFY_(R_STRING2)
  _STRINGIFY_(R_OPERATOR)
  _STRINGIFY_(R_IDENTIFIER)
  _STRINGIFY_(R_INFIX)
  _STRINGIFY_(R_INFIXEOL)
  return m;
}
}
#ifndef NO_DEPARSE_MSG
string RnwLang::deparseMsg(int i){
  static MsgMap wmmap = WindowsMap();
  return wmmap[i];
}
string RnwLang::deparseStyle(int i){
  static MsgMap stylemap = StyleMap();
  return stylemap[i];
}
string RnwLang::ctos(char c){
  stringstream s;
  char a[2]={c,0};
  s << reinterpret_cast<char*>(a);
  return s.str();
}
#else
string deparseMsg(int i){return string();}
#endif // NO_DEPARSE_MSG
#ifdef TEST_MAIN
#include <iostream>
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	cout << "message 3 is " << deparseMsg(3) << endl;
	return 0;
}
#endif
