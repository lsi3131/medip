#ifndef _WLINKFTPCLIENTDLL_
#define _WLINKFTPCLIENTDLL_


#include "FTPClient.h"


#pragma message( " " )
#pragma message( "<>-- FTP Client Library -------------------------------------------" )
#ifdef _DEBUG
#pragma message( "ftpClientLibd.lib [Debug Lib] 링크" )
#pragma comment(lib, "ftpClientLibd.lib")
#else
#pragma message( "ftpClientLib.lib [Release Lib] 링크" )
#pragma comment(lib, "ftpClientLib.lib")
#endif
#pragma message( "<>-- FTP Client Library -------------------------------------------" )
#pragma message( " " )



#endif// _WLINKFTPCLIENTDLL_