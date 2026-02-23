#include "svn_version.h"
/////////////////////////////// Ãß°¡
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define VERSION_MAJOR               1
#define VERSION_MINOR_TEN           0
#define VERSION_MINOR_ONE           0
#define VERSION_BUG_FIX             SVN_REVISION

#if SVN_LOCAL_MODIFICATIONS
#define VERSION_MODIFIER "M"
#else
#define VERSION_MODIFIER
#endif
#define VER_FILE_VERSION            VERSION_MAJOR, VERSION_MINOR_TEN, VERSION_MINOR_ONE, VERSION_BUG_FIX
#define VER_FILE_VERSION_STR        STRINGIZE(VERSION_MAJOR)        \
                                    "." STRINGIZE(VERSION_MINOR_TEN)    \
                                    "." STRINGIZE(VERSION_MINOR_ONE) \
                                    "." STRINGIZE(VERSION_BUG_FIX)    

#define VER_PRODUCT_VERSION_STR        STRINGIZE(VERSION_MAJOR)        \
                                    "." STRINGIZE(VERSION_MINOR_TEN)    \
                                    "." STRINGIZE(VERSION_MINOR_ONE)


#define VER_FILE_VERSION_STR_NO_BUILD        STRINGIZE(VERSION_MAJOR)        \
                                    "." STRINGIZE(VERSION_MINOR_TEN)    \
                                    "." STRINGIZE(VERSION_MINOR_ONE)

#define VER_COMPANYNAME_STR         "Medical IP"
#define VER_FILEDESCRIPTION_STR     "MEDIP FileManager Network module dll"
#define VER_INTERNALNAME_STR        "FileManager Network Module"
#define VER_LEGALCOPYRIGHT_STR      "Copyright (c) Medical IP Corp."
#define VER_LEGALTRADEMARKS1_STR    "All Rights Reserved"
#define VER_LEGALTRADEMARKS2_STR    VER_LEGALTRADEMARKS1_STR
#define VER_ORIGINALFILENAME_STR    "FileManagerNet.dll"

#define VER_PRODUCTNAME_STR             "MEDIP FileManager"
