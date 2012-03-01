/* This file is part of RnwLang.
 * A plugin written for Notepad++
 * Copyright (C)2012 Andrew Redd
 * Licensed under the GPL version 3 or newer
 */
 
#ifndef RNWLANG_RNWDEBUG_H_
#define RNWLANG_RNWDEBUG_H_

#ifdef DEBUG
#ifndef _DEBUG
#define _DEBUG
#endif

#include "dbgstream.h"
#include "deparse_wm_msg.h"
#include <iomanip>
using std::endl;
using std::hex;
using std::dec;

namespace RnwLang{
}
#define rnwmsg "RnwLang:MSG:"
#define rnwerr "RnwLang:ERROR:"

#define _dbg  dbg << rnwmsg
#define _dbgwf  dbg << rnwmsg << __func__ << ":"
#define _debugenter _dbgwf << ":entering"
#define _debugenter_ _debugenter << endl
#define _debugleave _dbgwf << ":leaving"
#define _debugleave_ _debugleave << endl
#define _dbgm(x) _dbgwf << x << endl
#define _dbgwfm(x) _dbgwf << x << endl

#else
#define _dbg
#define _dbgwf  
#define _debugenter
#define _debugenter_
#define _debugleave
#define _debugleave_
#define _dbgm(x) 
#define _dbgwfm(x)
#endif  // DEBUG
#endif  // RNWLANG_RNWDEBUG_H_
