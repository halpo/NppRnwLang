#ifndef DEPARSE_WM_MSG_H_
#define DEPARSE_WM_MSG_H_
#include <string>
using std::string;
namespace LexerPlugin{
string deparseMsg(int);
inline string ctos(char c){
  char a[2]={c,0};
  string s(reinterpret_cast<char*>(a));
  return s;
}
}
#endif // DEPARSE_WM_MSG_H_

