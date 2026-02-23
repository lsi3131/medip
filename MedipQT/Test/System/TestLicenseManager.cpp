#include "stdafx.h"
#include "Test/test_pch.h"
#include "System/LicenseManager.h"
#include "System/ProductManager.h"
#include "StringManager.h"

class TestLicenseManager : public ::testing::Test
{
public:
	void SetUp() override
	{
		m_pLicenseManager = new LicenseManager();
		m_pLicenseManager->SetProductManager(&m_productManager);

		m_testDirPath = GetUnitTestDataDirectory("/License").c_str();
		if (m_tempMasterKeyFileName.isEmpty() == false)
		{
			QFile::remove(GetMasterKeyFilePath());
		}
	}
	void TearDown() override
	{
		delete m_pLicenseManager;
	}

	void PrepareOfflineLicense(sInitLicenseInfo licenseInfo)
	{
		m_medipLicense.initLicense(licenseInfo);
		m_pLicenseManager->SetMedipLicense(&m_medipLicense);
	}

	void PrepareOfflineLicenseAndActivateKey(sAuthenticationInfo authInfo, sInitLicenseInfo licenseInfo)
	{
		m_medipLicense.initLicense(licenseInfo);
		m_pLicenseManager->SetMedipLicense(&m_medipLicense);

		m_pLicenseManager->CreateOfflineMasterKey(authInfo, m_testDirPath, &m_tempMasterKeyFileName);
		m_pLicenseManager->ActivateOfflineLicense(authInfo, GetMasterKeyFilePath());
	}

	QString GetMasterKeyFilePath()
	{
		return m_testDirPath + "/" + m_tempMasterKeyFileName;
	}

protected:
	QString m_testDirPath;
	QString m_tempMasterKeyFileName;
	LicenseManager* m_pLicenseManager;
	ProductManager m_productManager;
	Medip_License m_medipLicense;
};

TEST_F(TestLicenseManager, TestFileShouldExist)
{
	EXPECT_TRUE(QFile::exists(m_testDirPath));
}
TEST_F(TestLicenseManager, TestDefaultValue)
{
	QString url = m_pLicenseManager->getUrl(eLSPTLogin);
	QString serviceID = m_pLicenseManager->getServiceID();
	QString productType = m_pLicenseManager->getProductType();
	EXPECT_STREQ("MEDIP", m_pLicenseManager->getProductName().toStdString().c_str());
	EXPECT_STREQ("https://medicalip.net/licence/login", url.toStdString().c_str());
	EXPECT_STREQ("0007_medip", serviceID.toStdString().c_str());
	EXPECT_STREQ("MEDIP", productType.toStdString().c_str());

	sAuthenticationInfo outAuthInfo;

	Medip_License::GetInstance()->getLoginInfo(outAuthInfo);
	EXPECT_STREQ("", outAuthInfo.strFunctionList.toStdString().c_str());
}

TEST_F(TestLicenseManager, TestInitLicenseWithOnline)
{
	/*
		Lambda context에서 진행
	*/

	QString companyName = "Medicalip";
	sInitLicenseInfo info;
	info.eLicenseMethod = LICENSE_AUTH_METHOD::LAM_ONLINE;
	info.companyName = "medipcalip";
	info.productName = "MEDIP";
	info.productSubVersionName = "";
	info.configPath = STRING_MANAGER->m_strAppDataLocalPath;

	Medip_License::GetInstance()->initLicense(info);

	QString id = "m_pro@medicalip.com";
	QString pwd = "1234";
	QString serviceID = m_pLicenseManager->getServiceID();
	QString url = m_pLicenseManager->getUrl(eLSPTLogin);

	Medip_License::GetInstance()->loginCheck(id, pwd, serviceID, url);

	sAuthenticationInfo outAuthInfo;

	Medip_License::GetInstance()->getLoginInfo(outAuthInfo);
	EXPECT_STREQ("", outAuthInfo.strFunctionList.toStdString().c_str());
}

TEST_F(TestLicenseManager, TestDefault_Offline_MEDIP_Pro)
{
	sLoginResultInfo loginInfo = {};
	loginInfo.strServiceId = SERVICE_ID_MEDIP;
	loginInfo.functionList.push_back(MFL_Product_MEDIP_MEDIPPro);

	m_pLicenseManager->setSelectedMode(MedipLicense::LICENSE_AUTH_METHOD::LAM_OFFLINE);
	m_pLicenseManager->setLastLoginResultInfo(loginInfo);
	EXPECT_TRUE(m_pLicenseManager->productTypeSetting());
	EXPECT_TRUE(m_pLicenseManager->IsOffline());
}

TEST_F(TestLicenseManager, Test_WithoutInitializeCreateOfflineMasterKey_ReturnFalse)
{
	sAuthenticationInfo authInfo = {};
	//m_pLicenseManager->ActivateOfflineLicense

	EXPECT_FALSE(m_pLicenseManager->CreateOfflineMasterKey(authInfo, m_testDirPath, &m_tempMasterKeyFileName));
}

#ifdef TEST_OFFLINE_LICENSE
sInitLicenseInfo CreateTestOfflineLicenseInfo(QString configPath)
{
	sInitLicenseInfo licenseInfo = {};
	licenseInfo.eLicenseMethod = LICENSE_AUTH_METHOD::LAM_OFFLINE;
	licenseInfo.configPath = configPath;
	return licenseInfo;
}

sAuthenticationInfo CreateTestAuth(QString id, QString pwd, QVector<qulonglong> functionList = QVector<qulonglong>(), QString startDate = QString(), QString endDate = QString())
{
	sAuthenticationInfo info;
	info.strID = id;
	info.strPWD = pwd;
	info.strFunctionList = MakeFunctionListToString(functionList);
	info.strPeriodStartDate = startDate;
	info.strPeriodEndDate = endDate;
	info.strProductType = "";

	return info;
}

TEST_F(TestLicenseManager, WhenInitConfig_CreateOfflineMasterKey_Sucess)
{
	sAuthenticationInfo authInfo = {};
	sInitLicenseInfo licenseInfo = CreateTestOfflineLicenseInfo(m_testDirPath);
	m_medipLicense.initLicense(licenseInfo);

	m_pLicenseManager->SetMedipLicense(&m_medipLicense);

	EXPECT_TRUE(m_pLicenseManager->CreateOfflineMasterKey(authInfo, m_testDirPath, &m_tempMasterKeyFileName));
	EXPECT_TRUE(QFile::exists(GetMasterKeyFilePath()));
}

TEST_F(TestLicenseManager, WhenMasterKeyNotExist_ActivateOfflineLicesne_ReturnFalse)
{
	sAuthenticationInfo authInfo = {};
	EXPECT_FALSE(m_pLicenseManager->ActivateOfflineLicense(authInfo, "invalid/file/path"));
}

TEST_F(TestLicenseManager, Test_CreateAndActiveMasterKey)
{
	sAuthenticationInfo authInfo = {};
	sInitLicenseInfo licenseInfo = CreateTestOfflineLicenseInfo(m_testDirPath);
	m_medipLicense.initLicense(licenseInfo);

	m_pLicenseManager->SetMedipLicense(&m_medipLicense);

	EXPECT_TRUE(m_pLicenseManager->CreateOfflineMasterKey(authInfo, m_testDirPath, &m_tempMasterKeyFileName));
	EXPECT_TRUE(m_pLicenseManager->ActivateOfflineLicense(authInfo, GetMasterKeyFilePath()));
}

TEST_F(TestLicenseManager, When_ID_PWD_Invalid_ActivateShouldFail)
{
	sAuthenticationInfo masterKeyAuthInfo = CreateTestAuth("id", "pwd");
	sAuthenticationInfo activateKeyAuthInfo = CreateTestAuth("id2", "pwd2");

	sInitLicenseInfo licenseInfo = CreateTestOfflineLicenseInfo(m_testDirPath);
	PrepareOfflineLicense(licenseInfo);

	m_pLicenseManager->CreateOfflineMasterKey(masterKeyAuthInfo, m_testDirPath, &m_tempMasterKeyFileName);
	EXPECT_TRUE(m_pLicenseManager->ActivateOfflineLicense(masterKeyAuthInfo, GetMasterKeyFilePath()));
}

TEST_F(TestLicenseManager, When_FunctionList_Invalid_ActivateShouldFail)
{
	sAuthenticationInfo masterKeyAuthInfo = CreateTestAuth("id", "pwd", { 1,2,3 });
	sAuthenticationInfo activateKeyAuthInfo = CreateTestAuth("id", "pwd", { 2,3,4 });

	sInitLicenseInfo licenseInfo = CreateTestOfflineLicenseInfo(m_testDirPath);
	PrepareOfflineLicense(licenseInfo);

	m_pLicenseManager->CreateOfflineMasterKey(masterKeyAuthInfo, m_testDirPath, &m_tempMasterKeyFileName);
	EXPECT_FALSE(m_pLicenseManager->ActivateOfflineLicense(activateKeyAuthInfo, GetMasterKeyFilePath()));
}

TEST_F(TestLicenseManager, When_StartEndDate_Invalid_ActivateShouldFail)
{
	sAuthenticationInfo masterKeyAuthInfo = CreateTestAuth("id", "pwd", { 1,2,3 }, "20200101", "20300101");
	sAuthenticationInfo activateKeyAuthInfo = CreateTestAuth("id", "pwd", { 1,2,3 }, "20220101", "20320101");

	sInitLicenseInfo licenseInfo = CreateTestOfflineLicenseInfo(m_testDirPath);
	PrepareOfflineLicense(licenseInfo);

	m_pLicenseManager->CreateOfflineMasterKey(masterKeyAuthInfo, m_testDirPath, &m_tempMasterKeyFileName);
	EXPECT_FALSE(m_pLicenseManager->ActivateOfflineLicense(activateKeyAuthInfo, GetMasterKeyFilePath()));
}


TEST_F(TestLicenseManager, When_ID_PWD_Invalid_LoginReturnFalse)
{
	sAuthenticationInfo authInfo = CreateTestAuth("id", "pwd", {});
	sInitLicenseInfo licenseInfo = CreateTestOfflineLicenseInfo(m_testDirPath);

	PrepareOfflineLicenseAndActivateKey(authInfo, licenseInfo);

	EXPECT_FALSE(m_pLicenseManager->Login("invalid_id", "invalid_pwd", SERVICE_ID_MEDIP));
}

TEST_F(TestLicenseManager, Test_OfflineLogin)
{
	QString id = "id";
	QString pwd = "pwd";
	sAuthenticationInfo authInfo = CreateTestAuth(id, pwd, {});
	sInitLicenseInfo licenseInfo = CreateTestOfflineLicenseInfo(m_testDirPath);

	PrepareOfflineLicenseAndActivateKey(authInfo, licenseInfo);

	EXPECT_TRUE(m_pLicenseManager->Login(id, pwd, SERVICE_ID_MEDIP));
}

TEST_F(TestLicenseManager, Test_Offiline_FunctionListSorted)
{
	QString id = "id";
	QString pwd = "pwd";
	sAuthenticationInfo authInfo = CreateTestAuth(id, pwd, { 5,1,3, });
	sInitLicenseInfo licenseInfo = CreateTestOfflineLicenseInfo(m_testDirPath);

	PrepareOfflineLicenseAndActivateKey(authInfo, licenseInfo);

	m_pLicenseManager->Login(id, pwd, SERVICE_ID_MEDIP);

	sLoginResultInfo loginResult = m_pLicenseManager->getLastLoginResultInfo();
	EXPECT_EQ(QVector<qulonglong>({ 1,3,5 }), loginResult.functionList);
}

TEST_F(TestLicenseManager, Test_Offiline_UsableCount)
{
	QString id = "id";
	QString pwd = "pwd";
	QVector<qulonglong> functionLevelList = { 1,2,3,4,5 };
	QMap<qulonglong, int> usableCountMap = { {1, 10}, {2, 20}, {3, 30} };

	sAuthenticationInfo authInfo = CreateTestAuth(id, pwd, functionLevelList);
	authInfo.usableCountMap = usableCountMap;

	sInitLicenseInfo licenseInfo = CreateTestOfflineLicenseInfo(m_testDirPath);

	PrepareOfflineLicenseAndActivateKey(authInfo, licenseInfo);

	m_pLicenseManager->Login(id, pwd, SERVICE_ID_MEDIP);

	sLoginResultInfo loginResult = m_pLicenseManager->getLastLoginResultInfo();
	EXPECT_EQ(QVector<qulonglong>({ 1, 2, 3, 4, 5 }), loginResult.functionList);

	/* login result에는 usable count가 저장되어 있지 않음 */
	EXPECT_TRUE(loginResult.functionLevelListMap.isEmpty());

	/* MEDIP_License에서 Usable Count가 저장됨 */
	sAuthenticationInfo loginAuthInfo;
	m_medipLicense.getLoginInfo(loginAuthInfo);
	EXPECT_EQ(usableCountMap, loginAuthInfo.usableCountMap);
}

TEST_F(TestLicenseManager, Test_Offiline_ProductType)
{
	QString id = "id";
	QString pwd = "pwd";
	QVector<qulonglong> functionLevelList = { 1,2,3,4,5 };
	QMap<qulonglong, int> usableCountMap = { {1, 10}, {2, 20}, {3, 30} };

	sAuthenticationInfo authInfo = CreateTestAuth(id, pwd, functionLevelList);
	authInfo.usableCountMap = usableCountMap;
	authInfo.strProductType = PRODUCT_NAME_MEDIP_PRO;

	sInitLicenseInfo licenseInfo = CreateTestOfflineLicenseInfo(m_testDirPath);

	PrepareOfflineLicenseAndActivateKey(authInfo, licenseInfo);

	m_pLicenseManager->Login(id, pwd, SERVICE_ID_MEDIP);

	sLoginResultInfo loginResult = m_pLicenseManager->getLastLoginResultInfo();
	EXPECT_EQ(QVector<qulonglong>({ 1, 2, 3, 4, 5 }), loginResult.functionList);

	/* login result에는 usable count가 저장되어 있지 않음 */
	EXPECT_TRUE(loginResult.functionLevelListMap.isEmpty());

	/* MEDIP_License에서 Usable Count가 저장됨 */
	sAuthenticationInfo loginAuthInfo;
	m_medipLicense.getLoginInfo(loginAuthInfo);
	EXPECT_EQ(usableCountMap, loginAuthInfo.usableCountMap);
}
#endif
