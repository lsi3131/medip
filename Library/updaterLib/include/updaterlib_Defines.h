
#ifndef _UPDATERLIBDEFINES_H_
#define _UPDATERLIBDEFINES_H_


/*********************************	Defines	*************************************************/
#define MAX_SERVER_WAITTING_TIME 10000
#define FTP_FUNCTION_RETRY_COUNT 3

#define AI_DLL_10_0_VER "99999"
#define AI_DLL_10_2_VER "99999.10.2"
#define AI_DLL_11_0_VER "99999.11.0"

/*********************************	Enumerations	*************************************************/

typedef enum {
	// update request
	eUIETMedipUpdateRequestEvent,
	eUIETUpdaterUpdateRequestEvent,
	eUIETLauncherUpdateRequestEvent,
	eUIETLauncherUpdaterUpdateRequestEvent,

	// download request
	eUIETMedipDownloadRequestEvent,
	eUIETUpdaterDownloadRequestEvent,
	eUIETLauncherDownloadRequestEvent,
	eUIETLauncherUpdaterDownloadRequestEvent,

	// download cancel request
	eUIETMedipDownloadCancelRequestEvent,

	// update available check
	eUIETMedipUpdateAvailableCheckEvent,
	eUIETUpdaterUpdateAvailableCheckEvent,
	eUIETLauncherUpdateAvailableCheckEvent,
	eUIETLauncherUpdaterUpdateAvailableCheckEvent,

	// get url
	eUIETLicenseServerUrlGetEvent,

} eUIEventType;

typedef enum {
	eCBETUpdateRequestEvent,

	eCBETDownloadRequestEvent,
	eCBETProgressBarUpdateEvent,
	eCBETDownloadStartEvent,

} eCallBackEventType;

typedef enum {
	eURRTSuccess,
	eURRTNetworkError,
	eURRTJsonInvalid,
	eURRTNoDownloadList,
	eURRTNeedToRedirect,
	eURRTNetworkTimerOut,

} eUpdateReqResultType;

/*
typedef enum {
	eDRRTSuccess,
	eDRRTFail,

} eDownloadReqResultType;
*/

typedef enum {
	eFDFTSuccess,
	eFDFTLoginFail,
	eFDFTFTPDownloadFail,
	eFDFTVerifyFail,
	eFDFTFTPListFail,
} eFtpDownloadFailType;

typedef enum {
	eULASNormal,					// ai 구독, 구독 해제 직후가 아닌 상태.
	eULASAiUpdate,					// ai 구독하고 첫번째 업데이트 실행시 ai 업데이트 리스트로 업데이트를 완료한 상태.
	eULASNoneAiUpdate,				// ai 구독해제하고 첫번째 업데이트 실행시 none ai 업데이트 리스트로 업데이트를 완료한 상태.
} eUpdateListAppliedeState;

typedef enum {
	eADISNormal,					// ai 구독, 구독 해제 직후가 아닌 상태.
	eADISInstall,					// ai 구독하고 첫번째 업데이트 실행시 ai 업데이트 리스트로 업데이트를 완료한 상태.
	eADISUninstall,					// ai 구독해제하고 첫번째 업데이트 실행시 none ai 업데이트 리스트로 업데이트를 완료한 상태.
} eAiDllInstallState;

typedef enum {
	eUFTDirectory,					// Directory
	eUFTFile,						// File
} eUpdateFileType;

typedef enum {
	eUMNoneInstaller,				// none installer (설치폴더로 다운받은 파일 복사하여 설치.)
	eUMPatchInstaller,				// patch installer (패치 인스톨러 실행하여 설치.)
	eUMFullInstaller,				// full installer (풀 인스톨러 실행하여 설치.)
} eUpdateMethod;

/*********************************	Structs	***********************************************************/
typedef struct {
	QString version = "";
	QString downloadUrl = "";
	QString changeLog = "";
	bool	bFile = false;			// false : Directory, true : File
	QString serviceId = "";
	QString serviceFunctionLevelId = "";
	bool	bDelete = false;
	unsigned char updateMethod = eUMNoneInstaller;
} sDownloadedJsonInfo;

typedef struct {
	tstring strDownloadRemoteFilePath;
	tstring strDownloadLocalFilePath;
	tstring strDownloadLocalDirectoryPath;
	long fileSize;
	tstring strDeletedLocalFilePath;
	bool bDelete;
} sDownloadedFileInfo;

typedef struct {
	eUpdateMethod updateMethod = eUMNoneInstaller;
	QString sourceFilePath = "";
	QString destinationPath = "";
	QString deletedFilePath = "";
	bool bDelete = false;
	QString serviceID = "";
} sBatchFileWriteInfo;

typedef struct _tagBASEEVENT
{
	quint16	nType;

	_tagBASEEVENT()				{}
	virtual ~_tagBASEEVENT()	{}

} BaseEvent, *LPBaseEvent;

// update request
typedef struct _tagUPDATEREQUESTEVENT : public BaseEvent 
{
	QString url = "";
	QString version = "";
	QString serviceID = "";
	QString accessToken = "";

	bool bAiLocalState = false;
	bool bAiDllLocalState = false;
	bool bAiDll_11LocalState = false;
	bool bMEDIP = false;

	_tagUPDATEREQUESTEVENT()			{}
	virtual ~_tagUPDATEREQUESTEVENT()	{}

} UpdateRequestEvent, *LPUpdateRequestEvent;

// download request
typedef struct _tagDOWNLOADREQUESTEVENT : public BaseEvent 
{
	QString destinationPath = "";
	QString strConfigPath = "";

	_tagDOWNLOADREQUESTEVENT() {}
	virtual ~_tagDOWNLOADREQUESTEVENT() {}
} DownloadRequestEvent, *LPDownloadRequestEvent;

// get url
typedef struct _tagGETURLEVENT : public BaseEvent 
{
	QString url = "";
	QString environmentParam = "";
	QString resultUrl = "";

	_tagGETURLEVENT() {}
	virtual ~_tagGETURLEVENT() {}
} GetUrlEvent, *LPGetUrlEvent;

typedef struct {
	quint16	nEvent;		// event type
	quint16	nType;		// process type
	void*	pRetData;	// return data
} CallBackEvent, *LPCallBackEvent;

typedef struct {
	quint16 nResult;
} BaseResult, *LPBaseResult;

typedef struct : public BaseResult {
	QString url = "";
	bool updateAvailable = false;
	QString latestVersion = "";
	quint16 updateListAppliedeState;
	quint16 aiDllInstallState;
	quint16 aiDll_11InstallState;
	QString releaseNote = "";
} UpdateRequestResult, *LPUpdateRequestResult;

typedef struct : public BaseResult {
//	QVector<QString> downloadedFileNamesVec;
	QVector<sDownloadedJsonInfo> jsonDataVector;
	QString downloadedRootPath = "";
	QString destinationPath = "";
} DownloadRequestResult, *LPDownloadRequestResult;

typedef struct : public BaseResult {
	qint64 nAccumulationSize;
	qint64 nTotalSize;
} ProgressBarUpdateResult, *LPProgressBarUpdateResult;

typedef struct : public BaseResult {

} DownloadStartResult, *LPDownloadStartResult;



/*********************************	Global Variables	*************************************************/
//extern CWinThread *g_pThreadInterLockControl;



#endif	// _UPDATERLIBDEFINES_H_

