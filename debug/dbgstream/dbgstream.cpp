/*
** file: dbgstream.cpp -- debugging stream
*/
#include "dbgstream.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <fstream>

#ifdef DBG_TIMESTAMP
extern "C" {
#include <time.h>
};
#endif /* DBG_TIMESTAMP */

using namespace std;

dbgbuf::~dbgbuf()
{
    flushMsg();
}

void dbgbuf::flushMsg()
{
	if (msg.length() > 0) {
#if DBG_TIMESTAMP
		char tbuf[64];
		time_t t = time(0);
		struct tm tm;
		tm = *localtime(&t);
		strftime(tbuf,sizeof(tbuf),"%b %d %R:%S",&tm);

		OutputDebugStringA(tbuf);
		OutputDebugStringA(":");
		if (tee) {
			(*tee) << tbuf << ": ";
		}

#endif /* DBG_TIMESTAMP */
		OutputDebugStringA(msg.c_str());
		OutputDebugStringA("\n");
		if (tee) {
			(*tee) << msg << endl << flush;
		}

		msg.erase();	// erase message buffer
	}
}

std::ostream *dbgbuf::setTee(std::ostream *_tee)
{
    std::ostream *otee = tee; 
    tee = _tee; return otee; 
}

int dbgbuf::overflow(int c)
{
#ifdef DEBUG  ///  only prints if DEBUG build
    if (c == '\n') {
		flushMsg();
    } else {
		msg += c;
    }
    return c == -1 ? -1 : ' ';
#else  /// Otherwise acts as a null sink
  return c;
#endif
}

#ifdef TEST_MAIN
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	dbgstream dbg;

	dbg << "Hello, World." << endl;

	return 0;
}
#else
dbgstream dbg;
#endif /* TEST_MAIN */
