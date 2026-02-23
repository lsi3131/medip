#ifndef _WLINKMEDIPLICENSE_
#define _WLINKMEDIPLICENSE_

#include "medip_license.h"

#pragma message( " " )
#pragma message( "<>-- MedipLicense Library -------------------------------------------" )
#ifdef _DEBUG
#pragma message( "MedipLicensed.lib [Debug Lib] 링크" )
#pragma comment(lib, "MedipLicensed.lib")
#else
#pragma message( "MedipLicense.lib [Release Lib] 링크" )
#pragma comment(lib, "MedipLicense.lib")
#endif
#pragma message( "<>-- MedipLicense Library -------------------------------------------" )
#pragma message( " " )


#endif// _WLINKMEDIPLICENSE_