#pragma once

#ifndef LICENSE_MANAGER_H
#define LICENSE_MANAGER_H

#include "define.h"
#include "MedipType.h"

class SSENetwork;
class ProductManager;
class Medip_License;

#define LICENSE_DATA LicenseManager::GetLicenseInstance()

class LicenseManager : public QObject
{
	Q_OBJECT
public:
	static const QString LICENSE_PC_CODE_FILENAME;
	static const QString LICENSE_CONFIG_FILENAME;
	static const QString LICENSE_RESULT_INFO_FILENAME;

	static const QString LICENSE_ACCOUNT_STATE_INFO_PURCHASEPOPUP_FILENAME;
	static const QString LICENSE_FILTER_NOTICE_LIST_FILENAME;

	static const QString LICENSE_LOGIN_TEST_SERVER_URL;
	static const QString LICENSE_LICENSE_TEST_SERVER_URL;

	// sse
	static const QString LICENSE_SSE_SUBSCRIBE_TEST_SERVER_URL;
	static const QString LICENSE_SSE_UNSUBSCRIBE_TEST_SERVER_URL;
	static const QString LICENSE_SSE_ADDNOTIFICATIONHISTORY_TEST_SERVER_URL;

	static const QString LICENSE_SERVER_URL_HOSTNAME_GET_URL;
	static const QString NOTICE_SERVER_NOTICE_GETNOTICELIST_URL;

	static const QString LICENSE_SERVER_LOGIN_PATH;
	static const QString LICENSE_SERVER_LICENSE_PATH;

	// sse
	static const QString LICENSE_SERVER_SSE_SUBSCRIBE_PATH;
	static const QString LICENSE_SERVER_SSE_UNSUBSCRIBE_PATH;
	static const QString LICENSE_SERVER_SSE_ADDNOTIFICATIONHISTORY_PATH;

	static const QString LICENSE_SERVER_FUNCTIONUSABLECOUNT_URL;
	static const QString LICENSE_SERVER_FUNCTIONUSABLECOUNT_PATH;
	static const QString LICENSE_SERVER_DECREASE_FUNCTIONUSABLECOUNT_URL;
	static const QString LICENSE_SERVER_DECREASE_FUNCTIONUSABLECOUNT_PATH;

	static const QString ENVIRONMENT_PARAM;

	static const QStringList STRLIST_FUNCTIONPACK_NAME;
	static const QVector<QStringList> VECTOR_FUNCTIONS_NAME;
	static const QVector<QVector<QStringList>>	 VECTOR_FUNCTIONS_SUB_NAME;

public:
	static LicenseManager* GetLicenseInstance()
	{
		static LicenseManager Licenseinstance;
		return &Licenseinstance;
	}

public:
	LicenseManager();
	virtual ~LicenseManager();

public:
	bool initLicenseMng();
	// 상수 정의.

	// public 함수.
public:
	LICENSE_AUTH_METHOD& getSelectedMode();
	void setSelectedMode(LICENSE_AUTH_METHOD val);

	QString& getProductType();
	void setProductType(QString val);

	QString& getLicenseType();
	void setLicenseType(QString val);

	bool IsLoadFail_MedipVersionInfoFile() const;

	QString& getConfigPath();

	bool& getAutoLogin();
	QString& getStrDecryptedID();
	QString& getStrDecryptedPWD();

	QString& getAuthenticatedID();
	void setAuthenticatedID(QString val);

	QString& getServiceID();
	void setServiceID(QString val);

	QString& getServiceAuthorityLevelId();

	bool getTrialVer() const;
	void setTrialVer(bool val);

	sLoginResultInfo& getLastLoginResultInfo();
	void setLastLoginResultInfo(sLoginResultInfo val);

	QString& getCompanyName();

	void SetProductName(const QString& value);
	QString& getProductName();

	QString& getLicenseServerUrlHostName();

	bool loadMedipVersionInfo();

	// 라이센스 popup
	bool licenseDlgPopup();

	// offline "ActiveLicense.edt"(라이센스 키 파일)이 있는지 없는지 체크해주는 함수.
	bool IsActiveKey();
	bool IsOnline();
	bool IsOffline();
	bool IsValidateSystemDate();
	void saveSystemDate();

	void setConfig_License(eLicenseConfigElementType savedType, QString& value);
	void setConfig_License(QVector<eLicenseConfigElementType>& savedTypes, QStringList& values);
	void setConfig_License(QMap<muint8, QString>& mapLicenseInfo);
	bool getConfig_License(eLicenseConfigElementType savedType, QString& value);
	bool getConfig_License(QVector<eLicenseConfigElementType>& savedTypes, QStringList& values);
	bool getConfig_License(QVector<eLicenseConfigElementType>& savedTypes, QMap<muint8, QString>& mapLicenseInfo);

	qint16 IsAutologinSuccess();

	void progressBegin();
	void progressUpdate(qint64 value);
	void progressEnd();

	void licenseResultInfoFileSave(/*QStringList &strListLicenseResultInfo*/);
	void licenseResultInfoApply();

	bool isSubscribeFunctionLevel(eMEDIP_FUNCTION_LEVEL functionLevelId);
	bool isSubscribeProductTypeFunctionLevel(eMEDIP_FUNCTION_LEVEL functionLevelId);
	bool isSubscribeMedipProductTypeFunctionLevel(eMEDIP_FUNCTION_LEVEL functionLevelId);

	bool isSubscribeProductTypeOnlyMedipLight();
	eMEDIP_FUNCTION_LEVEL paidProductTypeFunctionLevel();
	bool isFreeProductTypeFunctionLevel();

	quint16 getLicenseServerUrlHostNameFromNoticeServer();

	QString getUrl(eLicenseServerPathType type);

	int chkFunctionUsableCount(eMEDIP_FUNCTION_LEVEL functionLevelId, eLicenseServerPathType usableCountType);
	int chkFunctionUsableCount(eMEDIP_FUNCTION_LEVEL functionLevelId, eLicenseServerPathType usableCountType, sFuncUsableCountResult& outData);

	QString loginResultMessage(qint16 resultCode);

	bool productTypeSetting();

	void checkProductTypeStateChange(); 			// MEDIP product type일때 light만 남은 경우메시지 처리.
	void getFilterNoticeList(QStringList& _filterNoticeList);	// filtering할 공지 사항 Subject 스트링 정보를 로드.
	void setFilterNoticeList(QList<sNoticeListInfo>& ShowedNoticeListVec);	// filtering할 공지 사항 Subject 스트링 정보를 저장.

	void getProductTypeEndDateAndPeriod(QString& strEndDate, QString& strPeriod);
	void getExpiredDateDisplayString(QString& strExpiredDate);

	// SSE Control
	bool sseSubscribe();
	void sseUnSubscribe();
	void sseAddNotificationHistory();
	bool isForceLogOutCase();
	void setForceLogOutCase(bool _bForceLogOutCase);

	bool Login(const QString& id, const QString& pwd, const QString& serviceID);

	void SetProductManager(ProductManager* pProductManager);
	ProductManager* GetProdcutManager() const;

	void SetMedipLicense(Medip_License* pMedipLicense);
	Medip_License* GetMedipLicense() const;

	bool ActivateOfflineLicense(sAuthenticationInfo authInfo, QString masterKeyFilePath);
	bool CreateOfflineMasterKey(sAuthenticationInfo authInfo, QString masterKeyDirPathPath, QString* pOutMasterKeyName);

	QString GetSubProductVersionName() const;

private:
	ProductManager* m_pProductManager;
	Medip_License* m_pMedipLicense;

	QString m_strProductType;
	QString m_strLicenseType;
	LICENSE_AUTH_METHOD m_nAuthenticationMethod;

	// lastExcutedTime, ID, Pwd를 binary로 저장용 map선언.
	QMap<muint8, QString> m_mapLicenseInfo;
	QMap<QString, bool> m_mapAccountStateInfo_purchasePopup;

	bool m_bLoadFail_MedipVersionInfoFile;

	QString m_strConfigPath;
	QString m_strMedipVersionInfoFilePath;

	bool bAutoLogin;
	QString strDecryptedID;
	QString strDecryptedPWD;

	QString authenticatedID;

	QProgressDialog* pProgressDlg = NULL;

	QString m_strServiceID;
	QString m_strServiceAuthorityLevelId;

	QString m_strCompanyName;
	QString m_strProductName;
	bool m_bTrialVer;
	sLoginResultInfo m_LastLoginResultInfo;

	QString m_strLicenseServerUrlHostName;

	SSENetwork* m_SSENet;
};
#endif