#pragma once

namespace MedipLicense
{
	/*********************************	Defines	*************************************************/
#ifndef _DEBUG
#define QSETTING_USE
#endif
#define COMPANY_NAME						tr("Medicalip")
#define SAVEDKEY_NAME						tr("activekey")
#define ETC_ITEM_NAME						tr("ETC")
#define PREVIOUSLY_USED_MASTERKEY_LIST		tr("usedMasterkeyList")
#define MAX_SERVER_WAITTING_TIME 10000		// 10초

//	#define SERVICE_NAME_MEDIP_DEFAULT			("Medip Default")				// 기본(academy, research, pro)
//	#define SERVICE_NAME_MEDIP_CUSTOMIZE		("Medip Customize")				// 기능별 구성

//	#define SERVICE_ID_MEDIP_DEFAULT			("0001_medip_default")			// 기본(academy, research, pro)
//	#define SERVICE_ID_MEDIP_CUSTOMIZE			("0003_medip_customize")		// 기능별 구성
//	#define SERVICE_ID_MEDIP_DEFAULT_TEST		("0005_medip_test_default")		// 기본_test(academy, research, pro)

	// 라이센스 확장성 대비 변경 사항 반영.
#define SERVICE_ID_MEDIP					("0007_medip")					// MEDIP 서비스(light, res, pro, MDBOX, demo 포함)
#define SERVICE_ID_MEDIP_AI					("0010_MEDIP_AI")				// MEDIP AI 서비스
#define SERVICE_ID_MEDIP_DEEPCATCH			("0004_deepcatch")				// (function level : wholebody demo, wholebody, abdomnen, chest)
#define SERVICE_ID_MEDIP_COVID19			("0002_medip_covid19")			// 무료버전
#define SERVICE_ID_TISEPX					("0008_tisepx")					// tisepx client 버전에 대한 서비스.

#define SERVICE_ID_MEDIP_TEST				("1007_test_medip")				// (function level : academy, research, pro)
#define SERVICE_ID_MEDIP_DEEPCATCH_TEST		("1004_test_deepcatch")			// (function level : abdominen, chest, ...)
#define SERVICE_ID_MEDIP_COVID19_TEST		("1002_test_medip_covid19")		// (function level : abdominen, chest, ...)

// (function level : academy, research, pro)
#define PRODUCT_NAME_MEDIP					("MEDIP")
#define PRODUCT_NAME_MEDIP_PRO				("MEDIP Pro")
#define PRODUCT_NAME_MEDIP_RESEARCH			("MEDIP Research")
#define PRODUCT_NAME_MEDIP_ACADEMY			("MEDIP Academy")
#define PRODUCT_NAME_MEDIP_MDBOX			("MEDIP MDBox")

#define PRODUCT_NAME_MEDIP_AI				("MEDIP AI")

// (function level : wholebody demo, wholebody, abdomnen, chest)
#define PRODUCT_NAME_DEEPCATCH					("DeepCatch")
#define PRODUCT_NAME_DEEPCATCH_V2				("DeepCatch V2")

#define PRODUCT_NAME_MEDIP_COVID19						("COVID19")

#define PRODUCT_NAME_TISEPX								("TiSepX")

#define PRODUCT_NAME_XR_ANATOMY							("MDBOX")
#define SERVICE_ID_XR_ANATOMY							("0012_XR_ANATOMY")

//	#define AUTHORITY_LEVEL_MEDIP_RESEARCH		("medip_00003")
//	#define AUTHORITY_LEVEL_MEDIP_PRO			("medip_00004")
//	#define AUTHORITY_LEVEL_MEDIP_ACADEMY		("medip_00005")

//	#define AUTHORITY_LEVEL_MEDIP_RESEARCH_DEMO	("medip_10003")
//	#define AUTHORITY_LEVEL_MEDIP_PRO_DEMO		("medip_10004")
//	#define AUTHORITY_LEVEL_MEDIP_ACADEMY_DEMO	("medip_10005")

//	#define AUTHORITY_LEVEL_MEDIP_RESEARCH_TEST	("test_medip_00003")
//	#define AUTHORITY_LEVEL_MEDIP_PRO_TEST		("test_medip_00004")
//	#define AUTHORITY_LEVEL_MEDIP_ACADEMY_TEST	("test_medip_00005")

#define MEDIPVERSIONINFO_FILENAME			tr("medipVersionInfo.dat")
#define HWSERIAL_EDT_FILENAME				tr("HWserial.edt")
#define HWSERIAL_DDT_FILENAME				tr("HWserial.ddt")
#define MEDIPINFO_EDT_FILENAME				tr("medipInfo.edt")
#define MEDIPINFO_DDT_FILENAME				tr("medipInfo.ddt")
#define MASTERKEY_EDT_FILENAME				tr("MasterKey.edt")
#define ACTIVELICENSE_EDT_FILENAME			tr("ActiveLicense.edt")

#define LICENSE_TYPE_NONE					("None")
#define LICENSE_TYPE_PERMANENT				("Permanent")
#define LICENSE_TYPE_SUBSCIPTION			("Subscription")
#define LICENSE_TYPE_TEMPORARY				("Temporary")
#define LICENSE_TYPE_TRIAL					("Trial")

#define DIRECTORY_NAME_FUNCTION_LEVEL ("FunctionLevel")
#define FILE_NAME_FUNCTION_LEVEL_HEADER ("FunctionLevel.h")
#define FILE_NAME_FUNCTION_LEVEL_CPP ("FunctionLevel.cpp")

#define CONST_AUTH_METHOD ("Authentication Method")
#define CONST_PRODUCT_TYPE ("Product Type")
#define CONST_LICENSE_TYPE ("License Type")
#define CONST_FUNCTION_LIST ("Function List")

#define CONST_LOGIN_ID ("Login ID")
#define CONST_LOGIN_PW ("Login PW")

#define CONST_PERIOD_STARTDATE ("Perioid Start Date")
#define CONST_PERIOD_ENDDATE ("Period End Date")
#define CONST_TRIAL_VERSION ("Trial Version")


/*********************************	Enumerations	*************************************************/
	enum LICENSE_AUTH_METHOD {
		LAM_ONLINE,
		LAM_OFFLINE,
	};

	typedef enum {
		ePTIMedip,
		ePTIMedip_AI,
		ePTIMedip_Deepcatch,
		ePTIMedip_Covid19,
		ePTITisepX,
		ePTIXrAnatomy,
	} eProductTypeIndex;

	typedef enum {
		eFLTCIColumnFunctionName,
		eFLTCIColumnUsableCount,
		//		eFLTCIColumnFunctionDesc,
		eFLTCIColumnMax,
	} eFunctionListTreeColumnIndex;

	typedef enum {
		eLRCSuccess,				// 성공
		eLRCIdMismatch,				// id
		eLRCPwdMismatch,			// pwd mismatch
		eLRCUnauthorizedEmail,		// unauthorizedEmail
		eLRCWithdrewUser,			// withdrewUser

		eLRCProductTypeMismatch,	// product type mismatch
		eLRCLicenseTypeMismatch,	// license type mismatch
		eLRCPeriodMismatch,			// 유효 기간 만료.
		eLRCLoadLicenseFail,		// load license fail

		eLRCRedirectNeed,			// redirect need case
		eLRCNetworkError,			// network error
		eLRCJsonDataInvalid,		// json data invalid
		eLRCUnauthorizedAccess,		// Unauthorized access

		eLRCNotRemainUsableCountError,		// notRemainUsableCountError
		eLRCRejectUsableCountError,			// rejectUsableCountError
		eLRCLimitUsableCountError,			// limitUsableCountError

		eLRCNetworkTimeOut,

	} eLicenseResultCode;


	typedef enum {
		eLSPTLogin,
		eLSPTChkLicence,
		eLSPTGetVersion,
		eLSPTChkFunctionUsableCount,
		eLSPTDecreaseFunctionUsableCount,

		// SSE control
		eLSPTSSESubscribe,
		eLSPTSSESubscribe_test,
		eLSPTSSEUnSubscribe,
		eLSPTSSEAddNotificationHistory,

	} eLicenseServerPathType;

	typedef enum {
		eLCETLastExecutedTime,	// last 접속 시간.(validation check 용도)
		eLCETLoginIDSaveCheck,	// id save 플레그.
		eLCETLoginID,			// id save시 저장할 ID.
		eLCETLoginPWD,			// id save시 저장할 pwd.
		eLCETLoginAuto,			// 자동 로그인 플레그.
		eLCETLoginAutoID,		// 자동 로그인 아이디.
		eLCETLoginAutoPWD,		// 자동 로그인 패스워드.
		eLCETLoginOffline,		// Offline 로그인
		eLCETLastLoginProductType		//마지막 Login Product 정보
	//	eLCETLicensedResult,	// 라이센스 성공/실패 플레그.
	//	eLCETLicensedID,		// 라이센스 성공한 ID.
	//	eLCETLicensedPWD,		// 라이센스 성공한 PWD.
	} eLicenseConfigElementType;

	/*********************************	Structs	***********************************************************/
	typedef struct {
		LICENSE_AUTH_METHOD eLicenseMethod;
		QString companyName = "";
		QString productName = "";
		QString productSubVersionName;
		QString configPath = "";
	} sInitLicenseInfo;

	typedef struct {
		qulonglong functionID;
		QString functionName;
		int usableCount;
		QString strTooltip;
	} sFunctionLevelComponent;

	typedef struct {
		sFunctionLevelComponent functionComponent;
		QVector<sFunctionLevelComponent> subFunctionData;
	} sFunctionData;

	typedef struct {
		qulonglong functionID;
		QString functionName;
	} sProductTypeFunctionData;

	struct sAuthenticationInfo
	{
		QString strPCcode = "";				// server master key에 저장.

		QString strID = "";
		QString strPWD = "";
		QString strProductType = "";
		QString strLicenseType = "";
		QString strFunctionList;
		QString strPeriodStartDate = "";
		QString strPeriodEndDate = "";
		QString strIsTrial = "";

		QString strNicName = "";			// nic name
		QMap<qulonglong, int> usableCountMap;
	};

	typedef struct {
		QString strLoginUrl;
		QString strLicenseUrl;
		QString strId;
		QString strPwd;
		QString strServiceId;
		QString strServiceAuthorityLevelId;
	} sAutoLoginInfo;

	// 나머지 function level list 정보(필요시 추가하여 사용.)
	typedef struct {
		QString strFunctionStartDttm = "";
		QString strFunctionEndDttm = "";
		int nFunctionUsableCount = 0;
	} sFunctionLevelListInfo;

	typedef struct {
		//	bool bResult;
		qint16 nResultCode;
		QString strAccessToken;
		QString strServiceId;
		QString strLicenseType;
		QString strServiceStartDttm;
		QString strServiceEndDttm;
		QString strServiceAuthorityLevelId;
		QString strIsTrial = "";
		QString strProductType;
		QVector<qulonglong> functionList;										// function level list중에 function level id정보만
		QMap<qulonglong, sFunctionLevelListInfo> functionLevelListMap;			// 나머지 function level list 정보 map.
	} sLoginResultInfo;

	struct FunctionLevelModule
	{
		int FunctionLevelID;
		QString FunctionLevelName;
		QString Comment;
		QString Category1;
		QString Category2;
		QString Category3;
		QString ProductType;
	};

	struct OfflineLicenseInfo
	{
		bool IsOfflineUseRegistry;
		std::wstring ActivateKeyFilePath;
		std::string ProductName;
	};

	struct OnlineLicenseInfo
	{
		std::wstring URL;
		std::wstring LicenseURL;
		std::wstring ServiceID;
		std::wstring ServiceAuthorityLevelID;
		bool IsTestService;
	};

	struct LoginLicenseInfo
	{
		LICENSE_AUTH_METHOD LicenseAuthMethod;
		std::wstring ID;
		std::wstring Password;
		std::string ProductType;
		std::string ProductName;
		bool IsAutoLogin;

		OfflineLicenseInfo Offline;
		OnlineLicenseInfo Online;
	};

	struct WidgetStyleSheetInfo
	{
		QString Frame;
		QString ComboBox;
		QString Button;
		QString SpinBox;
		QString LineEdit;
		QString CheckBox;
		QString TreeList;
	};
}