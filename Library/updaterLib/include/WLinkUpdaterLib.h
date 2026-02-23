#ifndef _WLINKUPDATERLIB_
#define _WLINKUPDATERLIB_


#include "updaterLib.h"


#pragma message( " " )
#pragma message( "<>-- updaterLib Library -------------------------------------------" )
#ifdef _DEBUG
#pragma message( "updaterLibd.lib [Debug Lib] 링크" )
#pragma comment(lib, "updaterLibd.lib")
#else
#pragma message( "updaterLib.lib [Release Lib] 링크" )
#pragma comment(lib, "updaterLib.lib")
#endif
#pragma message( "<>-- updaterLib Library -------------------------------------------" )
#pragma message( " " )



#endif// _WLINKUPDATERLIB_