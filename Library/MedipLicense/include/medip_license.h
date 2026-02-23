#ifndef MEDIP_LICENSE_H
#define MEDIP_LICENSE_H

#include "medip_license_global.h"
#include "Licensedefine.h"
#include <memory>

using namespace MedipLicense;

class AuthenticationStrategy;

MEDIP_LICENSE_EXPORT QString MakeFunctionListToString(QVector<qulonglong> functionList);

class MEDIP_LICENSE_EXPORT Medip_License : public QObject
{
	Q_OBJECT
public:
	static Medip_License* GetInstance();
private:
	static Medip_License* m_pInstance;

public:
	Medip_License();
	virtual ~Medip_License();

public:
	void setStrategyMethod(LICENSE_AUTH_METHOD eType);
	void setProductInfo(QString strCompanyName, QString strProductName);
	void initLicense(sInitLicenseInfo& info);
	void requestAuthentication(void);

	// common
	bool TryLogin(sLoginResultInfo* pOut, const QString& id, const QString& pwd, const QString& serviceId, const QString url = tr(""));
	void loginCheck(QString& id, QString& pwd, QString& serviceId, QString url = tr(""));
	void licenseCheck(QString& serviceId, QString& serviceAuthorityLevelId, QString url = tr(""));
	bool autoLoginCheck(sAutoLoginInfo& info, sLoginResultInfo& resultInfo);

	// online
	bool getAuthenticatedAccessToken(QString& strAccessToken);

	// offline
	bool OfflineLicenseDialog(
		const QString& functionLevelDirPath, 
		const QString& licenseDirPath, 
		const std::vector<int>& creditFunctionLevelIdList,
		const QString& product,
		const QString& company,
		const QString& productVersion,
		const WidgetStyleSheetInfo& info,
		QWidget* parent = nullptr);
	void getLoginInfo(sAuthenticationInfo& sInfo);
	void setLoginInfo(const sAuthenticationInfo& sInfo);
	QString getInfoData(const QString& strNicName);

	bool makeMasterKeyInServer(const sAuthenticationInfo& sInfo);
	bool _makeMasterKeyInServer(const sAuthenticationInfo& sInfo);		// TiSepX Client Lib용 Master Key File 생성시 사용되는 인터페이스.
	bool MakeMasterKeyInServerWithDirectoryPath(const sAuthenticationInfo& sInfo, QString directoryPath, QString* pOutFilePath);

	bool ActivateInClient(const sAuthenticationInfo& sInfo, const QString& strMasterFile);
	bool IsActivate() const;
	bool updateFunctionInfo(const QMap<qulonglong, int>& functionInfoMap);

	// etc
	void encryptMedipString(QString& sInputString, QString& sOutputString);
	void decryptMedipString(QString& sInputString, QString& sOutputString);

	//3.0 전용 API 
public:
	void InitializeLicenseInfo(const sInitLicenseInfo& info);
	bool MakeMasterKeyInServerWithName(const sAuthenticationInfo& sInfo, const std::wstring& keyName, std::wstring* pOutSavedFilePath);
	bool MakeActivateKey(const sAuthenticationInfo& sInfo, const std::wstring& masterKeyFileFile, const OfflineLicenseInfo& info, std::wstring* pOutSavedFilePath);
	QString GetPCCode(const QString& nickName);

	const AuthenticationStrategy* GetCurrentStrategy_const() const;
	AuthenticationStrategy* GetCurrentStrategy();

	bool Login(const sInitLicenseInfo& licenseInfo, const LoginLicenseInfo& loginInfo, MedipLicense::sLoginResultInfo* pOutLoginResultInfo);
	bool IsOfflineActive(const OfflineLicenseInfo& info) const;

signals:
	void loginFinished(const sLoginResultInfo& info);
	void licenseCheckFinished(const sLoginResultInfo& info);

private:
	std::shared_ptr<AuthenticationStrategy> m_pStrategyMethod = nullptr;

	std::shared_ptr<AuthenticationStrategy> m_pOnline = nullptr;
	std::shared_ptr<AuthenticationStrategy> m_pOffline = nullptr;
};

#endif // MEDIP_LICENSE_H
