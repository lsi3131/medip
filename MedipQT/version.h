#pragma once
#include "define.h"
#include "svn_version.h"
#include <string>

/////////////////////////////////////
//#define MACRO_VER // 매크로 버전
//#define ONLY_FOR_FTP_UPLOAD_VER // 마스크저장x, mip만 저장 기능만 살려둠(fileopen, savefile, savemeshfile 에서 라이센스 체크 안함)

// product version
//#define DEV_VER //개발 버전. 내부 사용

//#define DEEP_CATCH_VER // DEEPCATCH 버전, 체성분분석
//#define COVID19_VER // 코로나 버전, 현재 항상 기간제한 라이센스 들어감

/////////////////////////////// 추가
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#if defined(DEEP_CATCH_VER)
#define VERSION_MAJOR               1	// UI 변경
#define VERSION_MINOR_TEN           2	// 학습강화, 설치버전 생성
#define VERSION_MINOR_ONE           1	// 알고리즘 변경
#define VERSION_BUG_FIX             SVN_REVISION
//#define VERSION_BUILD				SVN_REVISION

#elif defined(COVID19_VER)
#define VERSION_MAJOR               1	// UI 변경
#define VERSION_MINOR_TEN           2	// 학습강화
#define VERSION_MINOR_ONE           1	// 알고리즘 변경
#define VERSION_BUG_FIX             SVN_REVISION
//#define VERSION_BUILD				SVN_REVISION

#else
#define VERSION_MAJOR               2
#define VERSION_MINOR_TEN           4	// dll 추가, 주요 기능추가
#define VERSION_MINOR_ONE           0	// dll 변경, 주요 기능 변경
#define VERSION_BUG_FIX             SVN_REVISION	// 버그 픽스
//#define VERSION_BUILD				SVN_REVISION

#endif


#if SVN_LOCAL_MODIFICATIONS
#define VERSION_MODIFIER "M"
#else
#define VERSION_MODIFIER
#endif

#define VER_FILE_VERSION            VERSION_MAJOR, VERSION_MINOR_TEN, VERSION_MINOR_ONE, VERSION_BUG_FIX

#define VER_FILE_VERSION_STR        STRINGIZE(VERSION_MAJOR)        \
                                    "." STRINGIZE(VERSION_MINOR_TEN)    \
                                    "." STRINGIZE(VERSION_MINOR_ONE) \
                                    "." STRINGIZE(VERSION_BUG_FIX)    \
									//"." STRINGIZE(VERSION_BUILD) 

#define VER_PRODUCT_VERSION_STR        STRINGIZE(VERSION_MAJOR)        \
                                    "." STRINGIZE(VERSION_MINOR_TEN)    \
                                    "." STRINGIZE(VERSION_MINOR_ONE)


#define VER_FILE_VERSION_STR_NO_BUILD        STRINGIZE(VERSION_MAJOR)        \
                                    "." STRINGIZE(VERSION_MINOR_TEN)    \
                                    "." STRINGIZE(VERSION_MINOR_ONE) \
                                    //"." STRINGIZE(VERSION_BUG_FIX)    \


//////////////////////////////
// #if defined(PRO_VER) || defined(RESEARCH_VER)
// #define VER_FILEVERSION             2.0.0.0
// #define VER_FILEVERSION_STR         "2.0.0,0\0"
// 
// #define VER_PRODUCTVERSION          2.0.0.0
// 
// #define VER_PRODUCTVERSION_STR      "2.0.0,0\0"
// 
// #elif defined(NOT_USE_ON_DEEP_CATCH_VER) || defined(COVID19_VER)
// #define VER_FILEVERSION             1.0.0.0
// #define VER_FILEVERSION_STR         "1.0.0,0\0"
// 
// #define VER_PRODUCTVERSION          1.0.0.0
// 
// #define VER_PRODUCTVERSION_STR      "1.0.0,0\0"     1
// #endif

#if defined(DEEP_CATCH_VER)
#define VER_FILEDESCRIPTION_STR     "DeepCatch software"
#define VER_INTERNALNAME_STR        "DeepCatch"
#define VER_PRODUCTNAME_STR			"DeepCatch"
#else
#define VER_FILEDESCRIPTION_STR     "MEDIP software"
#define VER_INTERNALNAME_STR        "MEDIP"
#define VER_PRODUCTNAME_STR			"MEDIP"
#endif

#define VER_COMPANYNAME_STR         "Medical IP"
#define VER_LEGALCOPYRIGHT_STR      "Copyright (c) Medical IP Corp."
#define VER_LEGALTRADEMARKS1_STR    "All Rights Reserved"
#define VER_LEGALTRADEMARKS2_STR    VER_LEGALTRADEMARKS1_STR


// #ifdef PRO_VER
// #define VER_PRODUCTNAME_STR				"Medip Pro"
// #define VER_PRODUCTNAME_STR_UPPERCASE   "MEDIP PRO"
// #define VER_ORIGINALFILENAME_STR		"Medip Pro.exe"
// 
// #elif defined RESEARCH_VER
// #define VER_PRODUCTNAME_STR				"Medip Research"
// #define VER_PRODUCTNAME_STR_UPPERCASE   "MEDIP RESEARCH"
// #define VER_ORIGINALFILENAME_STR		"Medip Research.exe"
// 
// #elif defined COVID19_VER
// #define VER_PRODUCTNAME_STR				"Medip Covid19"
// #define VER_PRODUCTNAME_STR_UPPERCASE   "MEDIP COVID19"
// #define VER_ORIGINALFILENAME_STR		"Medip COVID19.exe"
// 
// #elif defined  DEEP_CATCH_VER
// #define VER_PRODUCTNAME_STR				"Medip DeepCatch"
// #define VER_PRODUCTNAME_STR_UPPERCASE	"MEDIP DEEPCATCH"
// #define VER_ORIGINALFILENAME_STR		"Medip DeepCatch.exe"
// 
// #elif defined  ACADEMY_VER
// #define VER_PRODUCTNAME_STR				"Medip Academy"
// #define VER_PRODUCTNAME_STR_UPPERCASE   "MEDIP ACADEMY"
// #define VER_ORIGINALFILENAME_STR		"Medip Academy.exe"
// 
// #else
// #define VER_PRODUCTNAME_STR_UPPERCASE        "MEDIP Lite"
// #endif

#ifdef DEV_VER
#define VER_ADD_STR		" - DEV"
#else
#define VER_ADD_STR		""

#endif


// SNUH 한정판 (오백록교수)
// #define VER_PRODUCTNAME_STR         "Medip SNUH 2019 Lite"
// #define VER_PRODUCTNAME_STR2        "Medip SNUH 2019 Education"
// #define VER_PRODUCTNAME_STR3		"Medip SNUH PRO"

#define VER_COMPANYDOMAIN_STR       "www.medicalip.com"



