#ifndef UNIDEF_H_
#ifdef UNICODE 

  #define _tcslen     wcslen
  #define _tcscpy     wcscpy
  #define _tcscpy_s   wcscpy_s
  #define _tcsncpy    wcsncpy
  #define _tcsncpy_s  wcsncpy_s
  #define _tcscat     wcscat
  #define _tcscat_s   wcscat_s
  #define _tcsupr     wcsupr
  #define _tcsupr_s   wcsupr_s
  #define _tcslwr     wcslwr
  #define _tcslwr_s   wcslwr_s

  #define _stprintf_s swprintf_s
  #define _stprintf   swprintf
  #define _tprintf    wprintf

  #define _vstprintf_s    vswprintf_s
  #define _vstprintf      vswprintf

  #define _tscanf     wscanf
  
  #include <cwctype>
  #define _tisspace   iswspace
  
  //#define TCHAR wchar_t

#else

  #define _tcslen     strlen
  #define _tcscpy     strcpy
  #define _tcscpy_s   strcpy_s
  #define _tcsncpy    strncpy
  #define _tcsncpy_s  strncpy_s
  #define _tcscat     strcat
  #define _tcscat_s   strcat_s
  #define _tcsupr     strupr
  #define _tcsupr_s   strupr_s
  #define _tcslwr     strlwr
  #define _tcslwr_s   strlwr_s

  #define _stprintf_s sprintf_s
  #define _stprintf   sprintf
  #define _tprintf    printf

  #define _vstprintf_s    vsprintf_s
  #define _vstprintf      vsprintf

  #define _tscanf     scanf

  #define TCHAR char

  #include <cctype>  // for character checking
  #define _tisspace   isspace
  
  
#endif  // UNICODE
#endif  // UNIDEF_H_
