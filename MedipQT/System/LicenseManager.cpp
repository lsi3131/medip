#include "stdafx.h"
#include "LicenseManager.h"

#include "LoginDlg.h"
#include "OfflineLicenseRegDlg.h"
#include "windowManager.h"

#include "styleManager.h"
#include "stringManager.h"
#include "UpdateManager.h"
#include "Network/Network.h"

#include "System/ProductManager.h"
#include "defineMipEncoder.h"
#include "medipmipencoder.h"
#include "Licensedefine.h"

#include "../Network/SSENetwork.h"

const QString LicenseManager::LICENSE_PC_CODE_FILENAME = tr("pccode.dat");
const QString LicenseManager::LICENSE_CONFIG_FILENAME = tr("config_license.dat");
const QString LicenseManager::LICENSE_RESULT_INFO_FILENAME = tr("licenseResultInfo.dat");

const QString LicenseManager::LICENSE_ACCOUNT_STATE_INFO_PURCHASEPOPUP_FILENAME = tr("accountStateInfo.dat");
const QString LicenseManager::LICENSE_FILTER_NOTICE_LIST_FILENAME = tr("filterNotice.dat");

const QString LicenseManager::LICENSE_LOGIN_TEST_SERVER_URL = tr("https://medicalip.net/licence/login");
const QString LicenseManager::LICENSE_LICENSE_TEST_SERVER_URL = tr("https://medicalip.net/licence/chkLicence");

// sse
const QString LicenseManager::LICENSE_SSE_SUBSCRIBE_TEST_SERVER_URL = tr("https://medicalip.net/sse/subscribe");
const QString LicenseManager::LICENSE_SSE_UNSUBSCRIBE_TEST_SERVER_URL = tr("https://medicalip.net/sse/unsubscribe");
const QString LicenseManager::LICENSE_SSE_ADDNOTIFICATIONHISTORY_TEST_SERVER_URL = tr("https://medicalip.net/sse/addNotificationHistory_test");

const QString LicenseManager::LICENSE_SERVER_URL_HOSTNAME_GET_URL = tr("http://medicalip.net/notice/licence/getURL");
const QString LicenseManager::NOTICE_SERVER_NOTICE_GETNOTICELIST_URL = tr("http://medicalip.net/notice/notice/getNotice");

const QString LicenseManager::LICENSE_SERVER_LOGIN_PATH = tr("licence/login");
const QString LicenseManager::LICENSE_SERVER_LICENSE_PATH = tr("licence/chkLicence");

// sse
const QString LicenseManager::LICENSE_SERVER_SSE_SUBSCRIBE_PATH = tr("sse/subscribe");
const QString LicenseManager::LICENSE_SERVER_SSE_UNSUBSCRIBE_PATH = tr("sse/unsubscribe");
const QString LicenseManager::LICENSE_SERVER_SSE_ADDNOTIFICATIONHISTORY_PATH = tr("sse/addNotificationHistory_test");

const QString LicenseManager::LICENSE_SERVER_FUNCTIONUSABLECOUNT_URL = tr("https://medicalip.net/licence/chkFunctionUsableCount");
const QString LicenseManager::LICENSE_SERVER_DECREASE_FUNCTIONUSABLECOUNT_URL = tr("https://medicalip.net/licence/decreaseFunctionUsableCount");

const QString LicenseManager::LICENSE_SERVER_FUNCTIONUSABLECOUNT_PATH = tr("licence/chkFunctionUsableCount");
const QString LicenseManager::LICENSE_SERVER_DECREASE_FUNCTIONUSABLECOUNT_PATH = tr("licence/decreaseFunctionUsableCount");

const QString LicenseManager::ENVIRONMENT_PARAM = tr("DEV");		// DEV or PRODUCTION

const QStringList LicenseManager::STRLIST_FUNCTIONPACK_NAME = {
	"Import",
	"Export",
	"Rendering",
	"Volume Operation",
	"Layer Operation",
	"Segmentation",
	"Annotation",
	"Measurement",
	"Report",
	"Mesh Editing",
	"VR",
	"Visual Printing",
	"Radiomics Pack",
	"DeepDraw Pack(MEDIP AI)",
	"DeepDraw Research",
	"DeepDraw Training",
	"DeepCatch Module",
	"DeepCatch V2 Module",
	"TiSepX Module",
};
const QVector<QStringList> LicenseManager::VECTOR_FUNCTIONS_NAME = {
	QStringList{ "DICOM support", "MIP(medip default format) support", "MIPD(deepcatch format) support", "MIPA(medip AI format) support", "NIfTI, Raw support (Mask)", "NIfTI (HU)","Txt coordinate support", "PACS Download" /*"Mesh Object(STL, OBJ) (Check Mesh Editing function)", "Image File(bmp, png, jpg) (Check Report function)", "Customized Preset 2D/3D (Check Histogram function)"*/ },
	QStringList{ "MIP(medip default format) support", "MIPD(deepcatch format) support", "MIPA(medip AI format) support", "MIP, MIPD file uploader", "NIfTI(Whole HU) support", "Mask Export(raw, nii, txt) (HU, Mask)", "Json Export", "PACS Upload" },
	QStringList{ "Window basic function", "3D volume viewer", "Mesh tab list", "2D/3D histogram basic" },
	QStringList{ "Filtration", "Z-Isotropification", "Volume Flipping", "Volume Downscaling", "Volume Cropping" },
	QStringList{ "General function", "3D hole filling", "2D hole filling", "Layer boolean function", "Mask split region", "Inverse", "Move mask", "Flip mask", "Image dilation", "Image erosion", "Image Calculator", "Component" },
	QStringList{ "2D view manual selection function", "3D view manual selection function", "3D view manual split function", "Threshold basic", "Region growing", "DrawCut", "Multi DrawCut(MDCut)", "Working region",  "Working region tab list" },
	QStringList{ "Text", "Arrow", "Angle", "Length(mm)", "Path animation" },
	QStringList{ "General function(Rendering Tab)", "Projection function", "Axises coordinating", "Line profile(mm)" },
	QStringList{ "General function(Roport Tab, Editing)", "Image Management" },
	QStringList{ "General Function", "Smooth", "Reduce", "Remesh", "Solid", "Hollow", "Brush(smooth, push, pull, move)", "Measurement", "Cutting", "Subdivision", "Boolean", "IslandFilter", "Duplicate", "Attatch", "Mesh2Mask", "File import(stl, obj, vtk)", "File export(stl, obj, vtk)" },
	QStringList{ "MEDIP VR(patient case)", "MDBOX-XR" },
	QStringList{ "Visual Printing Service (Charge required)", "Basic Mesh Editing(smooth, reduce)", "Preview" },
	QStringList{ "Radiomics" },
	QStringList{ "Predict", "Credit" },
	QStringList{ "Select weight(add weight)" },
	QStringList{ "Custom Trainning Tool(comming soon)" },
	QStringList{ "DeepCatch Credit" },
	QStringList{ "DeepCatch V2 Credit" },
	QStringList{ "TiSepX Credit" },
};
const QVector<QVector<QStringList>>	LicenseManager::VECTOR_FUNCTIONS_SUB_NAME = {
	{ QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{} /*QStringList{}, QStringList{}, QStringList{}*/ },
	{ QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{} },
	{ QStringList{ "Boundary Viewer", "Crossection Viewer" }, QStringList{ "Clip Mode(clipping option)", "Mix Mode", "3D View Shader Quality Low", "3D View Shader Quality Middle", "3D View Shader Quality High" }, QStringList{}, QStringList{ "Preset customizing", "Preset file import", "Preset file export" } },
	{ QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{} },
	{ QStringList{}, QStringList{}, QStringList{}, QStringList{ "merge", "First=First-Second" }, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{} },
	{ QStringList{}, QStringList{}, QStringList{}, QStringList{ "GMM", "Within selected layer", "Within brush", "Show preview" }, QStringList{ "26 connectivity", "6 connectivity", "mode (HU range)" }, QStringList{ "adjust lambda" }, QStringList{}, QStringList{}, QStringList{} },
	{ QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{} },
	{ QStringList{}, QStringList{}, QStringList{}, QStringList{} },
	{ QStringList{ "Report import(html)", "Report export(pdf, html)", "Print preview", "Editing Function" }, QStringList{ "Capture", "Image import(png, bmp, jpg)", "Image export(png, bmp, jpg)" } },
	{ QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{}, QStringList{} },
	{ QStringList{}, QStringList{} },
	{ QStringList{}, QStringList{}, QStringList{} },
	{ QStringList{} },
	{ QStringList{}, QStringList{} },
	{ QStringList{} },
	{ QStringList{} },
	{ QStringList{} },
	{ QStringList{} },
	{ QStringList{} },
};

typedef enum {
	eLicensePassFlag,
	eAuthenticatedId,
	eLicenseResultFlag,
	eLicenseResultServiceId,
	eLicenseResultAuthorityId,
	eLicenseResultAccessToken,
	eFunctionListFirst,
} eLicenseResultSequence;

QDataStream& operator<<(QDataStream& out, const sFunctionLevelListInfo& rhs)
{
	out << rhs.strFunctionStartDttm;
	out << rhs.strFunctionEndDttm;
	out << rhs.nFunctionUsableCount;
	return out;
}
QDataStream& operator >> (QDataStream& in, sFunctionLevelListInfo& rhs)
{
	in >> rhs.strFunctionStartDttm;
	in >> rhs.strFunctionEndDttm;
	in >> rhs.nFunctionUsableCount;
	return in;
}

QDataStream& operator<<(QDataStream& out, const sLoginResultInfo& rhs)
{
	out << rhs.nResultCode;
	out << rhs.strAccessToken;
	out << rhs.strServiceId;
	out << rhs.strServiceStartDttm;
	out << rhs.strServiceEndDttm;
	out << rhs.strServiceAuthorityLevelId;
	out << rhs.strIsTrial;
	out << rhs.functionList;
	out << rhs.functionLevelListMap;
	return out;
}
QDataStream& operator >> (QDataStream& in, sLoginResultInfo& rhs)
{
	in >> rhs.nResultCode;
	in >> rhs.strAccessToken;
	in >> rhs.strServiceId;
	in >> rhs.strServiceStartDttm;
	in >> rhs.strServiceEndDttm;
	in >> rhs.strServiceAuthorityLevelId;
	in >> rhs.strIsTrial;
	in >> rhs.functionList;
	in >> rhs.functionLevelListMap;
	return in;
}

LicenseManager::LicenseManager()
{
	m_strProductType = PRODUCT_NAME_MEDIP;
	m_strLicenseType = LICENSE_TYPE_NONE;
	m_nAuthenticationMethod = LAM_ONLINE;

	strDecryptedID = tr("");
	strDecryptedPWD = tr("");
	authenticatedID = tr("");

	m_strServiceID = tr("");
	m_strServiceAuthorityLevelId = tr("");

	m_strCompanyName = COMPANY_NAME;
	m_strProductName = tr("");

	m_bLoadFail_MedipVersionInfoFile = false;
	bAutoLogin = false;
	m_bTrialVer = false;

	m_strMedipVersionInfoFilePath = "";
	m_bLoadFail_MedipVersionInfoFile = loadMedipVersionInfo();

	// 라이센스 서버 호스트 네임 get.
	getLicenseServerUrlHostNameFromNoticeServer();

	m_SSENet = nullptr;
	if (IsOnline())
	{
		m_SSENet = new SSENetwork();
	}

	SetProductManager(PRODUCT_MANAGER);
	m_pMedipLicense = Medip_License::GetInstance();
}

LicenseManager::~LicenseManager()
{
	SAFE_DELETE(m_SSENet);
}

LICENSE_AUTH_METHOD& LicenseManager::getSelectedMode()
{
	return m_nAuthenticationMethod;
}

void LicenseManager::setSelectedMode(LICENSE_AUTH_METHOD val)
{
	m_nAuthenticationMethod = val;
}

QString& LicenseManager::getProductType()
{
	return m_strProductType;
}

void LicenseManager::setProductType(QString val)
{
	m_strProductType = val;
}

QString& LicenseManager::getLicenseType()
{
	return m_strLicenseType;
}

void LicenseManager::setLicenseType(QString val)
{
	m_strLicenseType = val;
}

bool LicenseManager::IsLoadFail_MedipVersionInfoFile() const
{
	return m_bLoadFail_MedipVersionInfoFile;
}

QString& LicenseManager::getConfigPath()
{
	return m_strConfigPath;
}

bool& LicenseManager::getAutoLogin()
{
	return bAutoLogin;
}

QString& LicenseManager::getStrDecryptedID()
{
	return strDecryptedID;
}

QString& LicenseManager::getStrDecryptedPWD()
{
	return strDecryptedPWD;
}

QString& LicenseManager::getAuthenticatedID()
{
	return authenticatedID;
}
void LicenseManager::setAuthenticatedID(QString val)
{
	authenticatedID = val;
}

QString& LicenseManager::getServiceID()
{
	return m_strServiceID;
}

void LicenseManager::setServiceID(QString val)
{
	m_strServiceID = val;
}

QString& LicenseManager::getServiceAuthorityLevelId()
{
	return m_strServiceAuthorityLevelId;
}

bool LicenseManager::getTrialVer() const
{
	return m_bTrialVer;
}

void LicenseManager::setTrialVer(bool val)
{
	m_bTrialVer = val;
}

QString& LicenseManager::getCompanyName()
{
	return m_strCompanyName;
}

void LicenseManager::SetProductName(const QString& value)
{
	m_strProductName = value;
}

QString& LicenseManager::getProductName()
{
	return m_strProductName;
}

QString& LicenseManager::getLicenseServerUrlHostName()
{
	return m_strLicenseServerUrlHostName;
}

sLoginResultInfo& LicenseManager::getLastLoginResultInfo()
{
	return m_LastLoginResultInfo;
}

void LicenseManager::setLastLoginResultInfo(sLoginResultInfo val)
{
	m_LastLoginResultInfo = val;
}

bool LicenseManager::initLicenseMng()
{
	sInitLicenseInfo info;
	info.eLicenseMethod = getSelectedMode();
	info.companyName = m_strCompanyName;
	info.productName = m_strProductName;
	info.productSubVersionName = GetSubProductVersionName();
	info.configPath = STRING_MANAGER->LocalLicenseDirPath;
	m_pMedipLicense->initLicense(info);

#ifndef DEV_USE_APPDATA_PATH
	m_strConfigPath = QCoreApplication::applicationDirPath() + tr("/") + LICENSE_DIRNAME;
#else
	m_strConfigPath = STRING_MANAGER->LocalLicenseDirPath;
	if (!m_strConfigPath.isEmpty())
	{
		QDir configPath(m_strConfigPath);
		if (!configPath.exists())
		{
			configPath.mkpath(".");
		}
	}
#endif

	return true;
}

bool LicenseManager::loadMedipVersionInfo()
{
	m_strMedipVersionInfoFilePath = STRING_MANAGER->ProgramPathLicenseDirPath + tr("/") + MEDIPVERSIONINFO_FILENAME;
	QFile file(m_strMedipVersionInfoFilePath);

	QDataStream dataStream(&file);

	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QMap<QString, QString> mapMedipInfo;
	dataStream >> mapMedipInfo;
	file.close();

	m_strProductType = mapMedipInfo[QString(CONST_PRODUCT_TYPE)];
	m_strLicenseType = mapMedipInfo[QString(CONST_LICENSE_TYPE)];
	m_nAuthenticationMethod = (LICENSE_AUTH_METHOD)(mapMedipInfo[QString(CONST_AUTH_METHOD)]).toInt();

	// 업데이트 테스트용.
	bool bUpdateTest = false;
	QMap<QString, QString>::const_iterator iterMap = mapMedipInfo.find("test");
	if (mapMedipInfo.size() >= 3 && iterMap != mapMedipInfo.end() && iterMap.key() == "test")
	{
		QString test = iterMap.value();
		if (!test.compare(QString("test")))
			bUpdateTest = true;
	}

	if (!m_strProductType.compare(PRODUCT_NAME_MEDIP))
	{
		m_strProductName = PRODUCT_NAME_MEDIP;
		m_strServiceID = SERVICE_ID_MEDIP;
		if (bUpdateTest)
			m_strServiceID = SERVICE_ID_MEDIP_TEST;
	}
	else if (!m_strProductType.compare(PRODUCT_NAME_DEEPCATCH) || !m_strProductType.compare(PRODUCT_NAME_DEEPCATCH_V2))
	{
		m_strProductName = PRODUCT_NAME_DEEPCATCH;
		m_strServiceID = SERVICE_ID_MEDIP_DEEPCATCH;
		if (bUpdateTest)
			m_strServiceID = SERVICE_ID_MEDIP_DEEPCATCH_TEST;

	}
	else if (!m_strProductType.compare(PRODUCT_NAME_MEDIP_COVID19))
	{
		m_strProductName = PRODUCT_NAME_MEDIP_COVID19;
		m_strServiceID = SERVICE_ID_MEDIP_COVID19;
		if (bUpdateTest)
			m_strServiceID = SERVICE_ID_MEDIP_COVID19_TEST;
	}
	else if (!m_strProductType.compare(PRODUCT_NAME_MEDIP_AI))
	{
		m_strProductName = PRODUCT_NAME_MEDIP_AI;
		m_strServiceID = SERVICE_ID_MEDIP_AI;
	}

	return true;
}

// 라이센스 popup
bool LicenseManager::licenseDlgPopup()
{
	bool bResult = true;
	// 오프라인이고 activekey가 없으면
	if (!IsOnline() && !IsActiveKey())
	{
		// 오프라인 라이센스 등록 팝업 출력.
		std::vector<int> creditFunctionLevelIdList = {
			MFL_Common_VisualPrinting_VisualPrintingservice,
			MFL_DeepCatch_DeepCatchModule_PredictUsableCount_Credit,
			MFL_DeepCatchV2_DeepCatchV2Module_PredictUsableCount_Credit,
			MFL_Common_AI_PredictUsableCount_Credit,
			MFL_TiSepX_TiSepXClient_PredictUsableCount_Credit,
		};

		QString productSubVersion = GetSubProductVersionName();

		MedipLicense::WidgetStyleSheetInfo styleSheetInfo;
		styleSheetInfo.Frame = STYLE_MANAGER->mainFrame;
		styleSheetInfo.TreeList = STYLE_MANAGER->treeWithHeaderList;
		styleSheetInfo.Button = STYLE_MANAGER->buttonBehind;
		styleSheetInfo.ComboBox = STYLE_MANAGER->comboBoxNormal;
		styleSheetInfo.SpinBox = STYLE_MANAGER->spinbox;
		styleSheetInfo.LineEdit = STYLE_MANAGER->editBoxNormal;

		bool activate = m_pMedipLicense->OfflineLicenseDialog(
			STRING_MANAGER->ProgramPathFunctionLevelDirPath,
			STRING_MANAGER->LocalLicenseDirPath,
			creditFunctionLevelIdList,
			m_strProductName,
			m_strCompanyName,
			productSubVersion,
			styleSheetInfo
		);

		//bool activate = false;
		if (activate)
		{
			LoginDlg dlg;
			dlg.resize(QSize(420, 360));
			if (dlg.exec() != QDialog::Accepted)
			{
				bResult = false;
			}
		}
		else
		{
			bResult = false;
		}
	}
	// 온라인이거나
	// 오프라인이면 activekey가 있거나
	else
	{
		// 자동 로그인인 경우는 바로 접속.
		qint16 result = IsAutologinSuccess();
		if (result == -1)
		{
			// id pwd 입력 팝업 출력.
			LoginDlg dlg;
			dlg.resize(QSize(420, 360));
			if (dlg.exec() != QDialog::Accepted)
			{
				bResult = false;
			}
		}
		else if (result == -2)
		{
			bResult = false;
		}
	}
	return bResult;
}

qint16 LicenseManager::IsAutologinSuccess()
{
	qint16 result = -1;
	QString SavedID = ("");
	QString SavedPWD = ("");
	QString strLoginUrl = tr("");
	QString strLicenseUrl = tr("");
	bAutoLogin = false;
	QVector<eLicenseConfigElementType> keyVector{ eLCETLoginAuto, eLCETLoginAutoID , eLCETLoginAutoPWD };
	QMap<muint8, QString> mapLicenseInfo;
	bool res = getConfig_License(keyVector, mapLicenseInfo);

	if (res)
	{
		QMap<muint8, QString>::const_iterator find_iter;
		if ((find_iter = mapLicenseInfo.find(eLCETLoginAuto)) != mapLicenseInfo.end() && find_iter.key() == eLCETLoginAuto)
			bAutoLogin = QVariant(find_iter.value()).toBool();
		if ((find_iter = mapLicenseInfo.find(eLCETLoginAutoID)) != mapLicenseInfo.end() && find_iter.key() == eLCETLoginAutoID)
			SavedID = find_iter.value();
		if ((find_iter = mapLicenseInfo.find(eLCETLoginAutoPWD)) != mapLicenseInfo.end() && find_iter.key() == eLCETLoginAutoPWD)
			SavedPWD = find_iter.value();
	}

	if (bAutoLogin)
	{
		// decryption
		strDecryptedID = tr("");
		strDecryptedPWD = tr("");
		if (!SavedID.isEmpty())
			m_pMedipLicense->decryptMedipString(SavedID, strDecryptedID);
		if (!SavedPWD.isEmpty())
			m_pMedipLicense->decryptMedipString(SavedPWD, strDecryptedPWD);

		// 자동 인증 과정 진행.
		if (getSelectedMode() == LAM_OFFLINE)
		{
			sAuthenticationInfo sInfo;
			sInfo.strID = strDecryptedID;
			sInfo.strPWD = strDecryptedPWD;
			sInfo.strProductType = getProductType();
			m_pMedipLicense->setLoginInfo(sInfo);
		}
		else if (getSelectedMode() == LAM_ONLINE)
		{
			strLoginUrl = getUrl(eLSPTLogin);
			strLicenseUrl = getUrl(eLSPTChkLicence);
		}

		sAutoLoginInfo info{ strLoginUrl, strLicenseUrl, strDecryptedID, strDecryptedPWD, m_strServiceID, m_strServiceAuthorityLevelId };
		sLoginResultInfo resultInfo;
		result = m_pMedipLicense->autoLoginCheck(info, resultInfo) ? 0 : -1;
		if (!result)	// auto login 성공시 id 저장.
		{
			m_LastLoginResultInfo = resultInfo;
			WIN_MANAGER->SetLicensePass(true);
			setAuthenticatedID(strDecryptedID);
			if (!resultInfo.strIsTrial.isEmpty())
				setTrialVer(QVariant(resultInfo.strIsTrial).toBool());

			// 자동 로그인 성공 후 SSE Subscribe.
			if (!sseSubscribe())
			{
				// sse subscribe 메시지를 못 받고 time out 시 처리.
			//	sseUnSubscribe();
				QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_WARN), "SSE Subscribe fail.!!!");
				result = -2;
			}

		}
		// 자동 로그인 실패시(오프라인 계정에 대한 내용이 바뀌거나 온라인이면 계정 정보가 삭제 및 변경 될시) 자동 로그인 저장 정보 삭제.
		// network time out 되는 경우도 추가.
		else
		{
			QVector<eLicenseConfigElementType> keyVector{ eLCETLoginAuto, eLCETLoginAutoID,  eLCETLoginAutoPWD };
			QStringList valueList;
			valueList << QVariant(false).toString() << "" << "";
			setConfig_License(keyVector, valueList);

			bAutoLogin = false;
			strDecryptedID = tr("");
			strDecryptedPWD = tr("");

			// 자동 로그인 실패 후 메시지 표시.
		//	QMessageBox::warning(nullptr, "Auto login/license", loginResultMessage(resultInfo.nResultCode));
			QMessageBox::warning(nullptr, "Auto login", "Auto login failed. please try login again.");
		}

	}
	return result;
}

bool LicenseManager::IsActiveKey()
{
	return m_pMedipLicense->IsActivate();
}

bool LicenseManager::IsOnline()
{
	bool bResult = false;
	switch (getSelectedMode())
	{
	case LAM_ONLINE:
		bResult = true;
		break;
	case LAM_OFFLINE:
		bResult = false;
		break;
	}
	return bResult;
}

bool LicenseManager::IsOffline()
{
	return !IsOnline();
}

bool LicenseManager::IsValidateSystemDate()
{
	bool bIsValid = true;
	QString strlastExcutedTime = ("");
#if 0
	bool res = WIN_MANAGER->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_lastExecutedTime, strlastExcutedTime);
#else
	QString strDecryptedCurDate = "";
	bool res = getConfig_License(eLCETLastExecutedTime, strlastExcutedTime);
#endif
	if (res && !strlastExcutedTime.isEmpty())
	{
		m_pMedipLicense->decryptMedipString(strlastExcutedTime, strDecryptedCurDate);
		QDate lastExcutedTime = QDate::fromString(strDecryptedCurDate, ("yyyyMMdd"));
		QDate curDate = QDate::currentDate();
		if (lastExcutedTime.daysTo(curDate) < 0)
			bIsValid = false;
	}
	return bIsValid;
}

void LicenseManager::saveSystemDate()
{
	// system time config에 저장.
#if 0
	WIN_MANAGER->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_lastExecutedTime, QDate::currentDate().toString("yyyyMMdd"));
#else
	QString strEncryptedCurDate = "";
	m_pMedipLicense->encryptMedipString(QDate::currentDate().toString("yyyyMMdd"), strEncryptedCurDate);
	setConfig_License(eLCETLastExecutedTime, strEncryptedCurDate);
#endif
}

void LicenseManager::setConfig_License(eLicenseConfigElementType savedType, QString& value)
{
	m_mapLicenseInfo.clear();

	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_CONFIG_FILENAME;

	QFile file_read(configFilePath);
	QDataStream dataStream_read(&file_read);
	if (file_read.open(QIODevice::ReadOnly))
	{
		dataStream_read >> m_mapLicenseInfo;
		file_read.close();
	}

	// lastExcutedTime, ID, Pwd 저장.
	m_mapLicenseInfo[savedType] = value;

	QFile file_write(configFilePath);
	QDataStream dataStream_write(&file_write);
	if (file_write.open(QIODevice::WriteOnly))
	{
		dataStream_write << m_mapLicenseInfo;
		file_write.close();
	}
}

void LicenseManager::setConfig_License(QVector<eLicenseConfigElementType>& savedTypes, QStringList& values)
{
	m_mapLicenseInfo.clear();

	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_CONFIG_FILENAME;

	QFile file_read(configFilePath);
	QDataStream dataStream_read(&file_read);
	if (file_read.open(QIODevice::ReadOnly))
	{
		dataStream_read >> m_mapLicenseInfo;
		file_read.close();
	}

	// lastExcutedTime, ID, Pwd 저장.
	for (int i = 0; i < savedTypes.size(); i++)
	{
		eLicenseConfigElementType savedType = savedTypes.at(i);
		if (i < values.size())
			m_mapLicenseInfo[savedType] = values.at(i);
	}

	QFile file_write(configFilePath);
	QDataStream dataStream_write(&file_write);
	if (file_write.open(QIODevice::WriteOnly))
	{
		dataStream_write << m_mapLicenseInfo;
		file_write.close();
	}
}

void LicenseManager::setConfig_License(QMap<muint8, QString>& mapLicenseInfo)
{
	m_mapLicenseInfo.clear();

	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_CONFIG_FILENAME;

	QFile file_read(configFilePath);
	QDataStream dataStream_read(&file_read);
	if (file_read.open(QIODevice::ReadOnly))
	{
		dataStream_read >> m_mapLicenseInfo;
		file_read.close();
	}

	for (QMap<muint8, QString>::const_iterator mapIter = mapLicenseInfo.constBegin(); mapIter != mapLicenseInfo.constEnd(); mapIter++)
		m_mapLicenseInfo[mapIter.key()] = mapIter.value();

	QFile file_write(configFilePath);
	QDataStream dataStream_write(&file_write);
	if (file_write.open(QIODevice::WriteOnly))
	{
		dataStream_write << m_mapLicenseInfo;
		file_write.close();
	}
}

bool LicenseManager::getConfig_License(eLicenseConfigElementType savedType, QString& value)
{
	bool bResult = false;
	m_mapLicenseInfo.clear();

	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_CONFIG_FILENAME;

	QFile file_read(configFilePath);
	QDataStream dataStream(&file_read);
	if (file_read.open(QIODevice::ReadOnly))
	{
		dataStream >> m_mapLicenseInfo;
		file_read.close();
#if 0
		value = m_mapLicenseInfo[savedType];
#else
		QMap<muint8, QString>::const_iterator find_iter;
		if ((find_iter = m_mapLicenseInfo.find(savedType)) != m_mapLicenseInfo.end())
		{
			value = find_iter.value();
			bResult = true;
		}
#endif
	}

	return bResult;
}

bool LicenseManager::getConfig_License(QVector<eLicenseConfigElementType>& savedTypes, QStringList& values)
{
	bool bResult = false;
	m_mapLicenseInfo.clear();

	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_CONFIG_FILENAME;

	QFile file_read(configFilePath);
	QDataStream dataStream(&file_read);
	if (file_read.open(QIODevice::ReadOnly))
	{
		dataStream >> m_mapLicenseInfo;
		file_read.close();
#if 0
		value = m_mapLicenseInfo[savedType];
#else
		for (int i = 0; i < savedTypes.size(); i++)
		{
			eLicenseConfigElementType savedType = savedTypes.at(i);
			QMap<muint8, QString>::const_iterator find_iter;
			if ((find_iter = m_mapLicenseInfo.find(savedType)) != m_mapLicenseInfo.end())
			{
				values << find_iter.value();
				bResult = true;
			}
		}
#endif
	}

	return bResult;
}

bool LicenseManager::getConfig_License(QVector<eLicenseConfigElementType>& savedTypes, QMap<muint8, QString>& mapLicenseInfo)
{
	bool bResult = false;
	m_mapLicenseInfo.clear();

	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_CONFIG_FILENAME;

	QFile file_read(configFilePath);
	QDataStream dataStream(&file_read);
	if (file_read.open(QIODevice::ReadOnly))
	{
		dataStream >> m_mapLicenseInfo;
		file_read.close();

		for (int i = 0; i < savedTypes.size(); i++)
		{
			eLicenseConfigElementType savedType = savedTypes.at(i);
			QMap<muint8, QString>::const_iterator find_iter;
			if ((find_iter = m_mapLicenseInfo.find(savedType)) != m_mapLicenseInfo.end())
			{
				mapLicenseInfo[savedType] = find_iter.value();
				bResult = true;
			}
		}

	}

	return bResult;
}

void LicenseManager::progressBegin()
{
	if (pProgressDlg != NULL)
	{
		pProgressDlg->deleteLater();
		pProgressDlg = NULL;
	}

	pProgressDlg = new QProgressDialog((QWidget*)WIN_MANAGER->mainWindow);
	pProgressDlg->setWindowFlags(Qt::WindowCloseButtonHint);
	pProgressDlg->setStyleSheet(STYLE_MANAGER->mainFrame);
	pProgressDlg->setLabelText("Auto Login");
	pProgressDlg->setCancelButtonText(STRING_MANAGER->getString(STR_CANCEL));
	pProgressDlg->setRange(0, 100);
	pProgressDlg->setWindowTitle(STRING_MANAGER->getString(STR_WORK));
	//	pProgressDlg->setWindowModality(Qt::WindowModal);
	//	pProgressDlg->setMinimumDuration(500);
	pProgressDlg->setMinimumDuration(0);
	pProgressDlg->setValue(50);
	pProgressDlg->exec();
}

void LicenseManager::progressUpdate(qint64 value)
{
	if (pProgressDlg)
		pProgressDlg->setValue(value);
}

void LicenseManager::progressEnd()
{
	if (pProgressDlg)
	{
		pProgressDlg->hide();
		pProgressDlg->done(QDialog::Accepted);
		pProgressDlg->deleteLater();
		pProgressDlg = NULL;
	}
}

// 인증된 로그인 정보를 저장하여 updater에 의해 재실행되면 로드하여 사용. 
void LicenseManager::licenseResultInfoFileSave(/*QStringList &strListLicenseResultInfo*/)
{
#if 0
	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_RESULT_INFO_FILENAME;
	QFile file(configFilePath);
	QTextStream textStream(&file);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		for (int i = 0; i < strListLicenseResultInfo.size(); i++)
			textStream << strListLicenseResultInfo.at(i) << tr("\n");
		file.close();
	}
#else
	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_RESULT_INFO_FILENAME;
	QFile file(configFilePath);
	QDataStream dataStream(&file);
	if (file.open(QIODevice::WriteOnly))
	{
		dataStream << WIN_MANAGER->IsLicensePass();
		dataStream << LICENSE_DATA->getAuthenticatedID();
		dataStream << LICENSE_DATA->getLastLoginResultInfo();
		file.close();
	}
#endif
}

void LicenseManager::licenseResultInfoApply()
{
#if 0
	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_RESULT_INFO_FILENAME;
	QFile file(configFilePath);
	QTextStream textStream(&file);
	QStringList strListLicenseResultInfo;
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QString line;
		while (textStream.readLineInto(&line))
			strListLicenseResultInfo << line;
		file.close();
	}
	QString strLicensePass = tr("");
	QString strAuthenticatedID = tr("");
	if ((eLicensePassFlag + 1) <= strListLicenseResultInfo.size())
		strLicensePass = strListLicenseResultInfo.at(eLicensePassFlag);
	if ((eAuthenticatedId + 1) <= strListLicenseResultInfo.size())
		strAuthenticatedID = strListLicenseResultInfo.at(eAuthenticatedId);

	if (!strLicensePass.isEmpty())
		WIN_MANAGER->SetLicensePass(QVariant(strLicensePass).toBool());
	if (!strAuthenticatedID.isEmpty())
		setAuthenticatedID(strAuthenticatedID);

	// 라이센스 결과 반환값 로드.
	// eLicenseResultFlag
	// eLicenseResultServiceId
	// eLicenseResultAuthorityId
	// eFunctionListFirst
	sLoginResultInfo lastLoginResultInfo;
	QString strLicenseResultFlag = tr("");
	QString strLicenseResultServiceId = tr("");
	QString strLicenseResultAuthorityId = tr("");
	QString strLicenseResultAccessToken = tr("");
	QString strTemp = tr("");
	if ((eLicenseResultFlag + 1) <= strListLicenseResultInfo.size())
		strLicenseResultFlag = strListLicenseResultInfo.at(eLicenseResultFlag);
	if ((eLicenseResultServiceId + 1) <= strListLicenseResultInfo.size())
		strLicenseResultServiceId = strListLicenseResultInfo.at(eLicenseResultServiceId);
	if ((eLicenseResultAuthorityId + 1) <= strListLicenseResultInfo.size())
		strLicenseResultAuthorityId = strListLicenseResultInfo.at(eLicenseResultAuthorityId);
	if ((eLicenseResultAccessToken + 1) <= strListLicenseResultInfo.size())
		strLicenseResultAccessToken = strListLicenseResultInfo.at(eLicenseResultAccessToken);

	if (!strLicenseResultFlag.isEmpty())
		lastLoginResultInfo.nResultCode = QVariant(strLicenseResultFlag).toInt();
	if (!strLicenseResultServiceId.isEmpty())
		lastLoginResultInfo.strServiceId = strLicenseResultServiceId;
	if (!strLicenseResultAuthorityId.isEmpty())
		lastLoginResultInfo.strServiceAuthorityLevelId = strLicenseResultAuthorityId;
	if (!strLicenseResultAccessToken.isEmpty())
		lastLoginResultInfo.strAccessToken = strLicenseResultAccessToken;

	for (int i = eFunctionListFirst; i < strListLicenseResultInfo.size(); i++)
	{
		strTemp = strListLicenseResultInfo.at(i);
		lastLoginResultInfo.functionList.push_back(strTemp.toULongLong());
	}
	setLastLoginResultInfo(lastLoginResultInfo);

	// 적용 후 파일 삭제.
	if (QFile::exists(configFilePath))
		QFile::remove(configFilePath);
#else
	QString preConfigFilePath = m_strConfigPath + tr("/") + "licenseinfo.dat";
	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_RESULT_INFO_FILENAME;
	if (QFile::exists(configFilePath))
	{
		QFile file(configFilePath);
		QDataStream dataStream(&file);
		if (file.open(QIODevice::ReadOnly))
		{
			bool blicense_pass;
			QString authenticatedID;
			sLoginResultInfo lastLoginResultInfo;
			dataStream >> blicense_pass;
			dataStream >> authenticatedID;
			dataStream >> lastLoginResultInfo;

			WIN_MANAGER->SetLicensePass(blicense_pass);
			setAuthenticatedID(authenticatedID);
			setLastLoginResultInfo(lastLoginResultInfo);
			file.close();
		}
		// 적용 후 파일 삭제.
		QFile::remove(configFilePath);
	}
	else if (QFile::exists(preConfigFilePath))
	{
		QFile file(preConfigFilePath);
		QTextStream textStream(&file);
		QStringList strListLicenseResultInfo;
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QString line;
			while (textStream.readLineInto(&line))
				strListLicenseResultInfo << line;
			file.close();
		}
		QString strLicensePass = tr("");
		QString strAuthenticatedID = tr("");
		if ((eLicensePassFlag + 1) <= strListLicenseResultInfo.size())
			strLicensePass = strListLicenseResultInfo.at(eLicensePassFlag);
		if ((eAuthenticatedId + 1) <= strListLicenseResultInfo.size())
			strAuthenticatedID = strListLicenseResultInfo.at(eAuthenticatedId);

		if (!strLicensePass.isEmpty())
			WIN_MANAGER->SetLicensePass(QVariant(strLicensePass).toBool());
		if (!strAuthenticatedID.isEmpty())
			setAuthenticatedID(strAuthenticatedID);

		// 라이센스 결과 반환값 로드.
		// eLicenseResultFlag
		// eLicenseResultServiceId
		// eLicenseResultAuthorityId
		// eFunctionListFirst
		sLoginResultInfo lastLoginResultInfo;
		QString strLicenseResultFlag = tr("");
		QString strLicenseResultServiceId = tr("");
		QString strLicenseResultAuthorityId = tr("");
		QString strLicenseResultAccessToken = tr("");
		QString strTemp = tr("");
		if ((eLicenseResultFlag + 1) <= strListLicenseResultInfo.size())
			strLicenseResultFlag = strListLicenseResultInfo.at(eLicenseResultFlag);
		if ((eLicenseResultServiceId + 1) <= strListLicenseResultInfo.size())
			strLicenseResultServiceId = strListLicenseResultInfo.at(eLicenseResultServiceId);
		if ((eLicenseResultAuthorityId + 1) <= strListLicenseResultInfo.size())
			strLicenseResultAuthorityId = strListLicenseResultInfo.at(eLicenseResultAuthorityId);
		if ((eLicenseResultAccessToken + 1) <= strListLicenseResultInfo.size())
			strLicenseResultAccessToken = strListLicenseResultInfo.at(eLicenseResultAccessToken);

		if (!strLicenseResultFlag.isEmpty())
			lastLoginResultInfo.nResultCode = QVariant(strLicenseResultFlag).toInt();
		if (!strLicenseResultServiceId.isEmpty())
			lastLoginResultInfo.strServiceId = strLicenseResultServiceId;
		if (!strLicenseResultAuthorityId.isEmpty())
			lastLoginResultInfo.strServiceAuthorityLevelId = strLicenseResultAuthorityId;
		if (!strLicenseResultAccessToken.isEmpty())
			lastLoginResultInfo.strAccessToken = strLicenseResultAccessToken;

		for (int i = eFunctionListFirst; i < strListLicenseResultInfo.size(); i++)
		{
			strTemp = strListLicenseResultInfo.at(i);
			lastLoginResultInfo.functionList.push_back(strTemp.toULongLong());
		}
		setLastLoginResultInfo(lastLoginResultInfo);

		// 적용 후 파일 삭제.
		QFile::remove(preConfigFilePath);
	}
#endif

}

bool LicenseManager::isSubscribeFunctionLevel(eMEDIP_FUNCTION_LEVEL functionLevelId)
{
	bool bResult = false;
	QVector<qulonglong>& functionList = m_LastLoginResultInfo.functionList;
	for (int i = 0; i < functionList.size(); i++)
	{
		// 사용자가 구독한 function list에서 parameter로 온 functionlevelid가 존재하면 구독중으로 판단. 
		if (functionList.at(i) == functionLevelId)
		{
			bResult = true;
			break;
		}
	}
	return bResult;
}

// product type function level이면 true, 아니면 false
bool LicenseManager::isSubscribeProductTypeFunctionLevel(eMEDIP_FUNCTION_LEVEL functionLevelId)
{
	bool bResult = false;
	if ((100000 < functionLevelId && functionLevelId < 200000) || (10100000 < functionLevelId && functionLevelId < 10200000))
	{
		bResult = true;
	}
	return bResult;
}

bool LicenseManager::isSubscribeMedipProductTypeFunctionLevel(eMEDIP_FUNCTION_LEVEL functionLevelId)
{
	bool bResult = false;
	if ((MFL_Product_MEDIP_MEDIPLight <= functionLevelId) && (functionLevelId <= MFL_Product_MEDIP_MEDIPMDBox))
		bResult = true;
	else if (functionLevelId == MFL_Product_MEDIP_MEDIPFull_DEMOMEDIPFull)
		bResult = true;

	return bResult;
}

// demo 포함 paid 버전 프로덕트 레벨 get.
eMEDIP_FUNCTION_LEVEL LicenseManager::paidProductTypeFunctionLevel()
{
	eMEDIP_FUNCTION_LEVEL result = (eMEDIP_FUNCTION_LEVEL)0;
	QVector<qulonglong>& functionList = m_LastLoginResultInfo.functionList;
	for (int i = 0; i < functionList.size(); i++)
	{
		if ((MFL_Product_MEDIP_MEDIPResearch <= functionList.at(i)) && (functionList.at(i) <= MFL_Product_MEDIP_MEDIPMDBox))
		{
			result = (eMEDIP_FUNCTION_LEVEL)functionList.at(i);
			break;
		}
		else if (functionList.at(i) == MFL_Product_MEDIP_MEDIPFull_DEMOMEDIPFull)
		{
			result = (eMEDIP_FUNCTION_LEVEL)functionList.at(i);
			break;
		}
		else if (functionList.at(i) == MFL_Product_DeepCatch_DeepCatchV2)
		{
			result = (eMEDIP_FUNCTION_LEVEL)functionList.at(i);
			break;
		}
	}
	return result;
}

bool LicenseManager::isSubscribeProductTypeOnlyMedipLight()
{
	bool bResult = false;
	bool bMedipLight = false;
	int count = 0;
	QVector<qulonglong>& functionList = m_LastLoginResultInfo.functionList;
	for (int i = 0; i < functionList.size(); i++)
	{
		if (functionList.at(i) == MFL_Product_MEDIP_MEDIPLight)
			bMedipLight = true;
		if (isSubscribeMedipProductTypeFunctionLevel((eMEDIP_FUNCTION_LEVEL)functionList.at(i)))
			count++;
	}
	bResult = bMedipLight && (count == 1);
	return bResult;
}

bool LicenseManager::isFreeProductTypeFunctionLevel()
{
	// medipt light 버전이면 무료.
	if (isSubscribeProductTypeOnlyMedipLight())
		return true;

	// coivd 19이면 무료.
	bool bDeepCatch_v1 = false;
	bool bDeepCatch_v2 = false;

	QVector<qulonglong>& functionList = m_LastLoginResultInfo.functionList;
	for (int i = 0; i < functionList.size(); i++)
	{
		if (functionList.at(i) == MFL_Product_DeepCatch)
			bDeepCatch_v1 = true;

		if (functionList.at(i) == MFL_Product_DeepCatch_DeepCatchV2)
			bDeepCatch_v2 = true;

		if (functionList.at(i) == MFL_Product_COVID19)
			return true;
	}

	if (bDeepCatch_v1 && !bDeepCatch_v2)
		return true;

	return false;
}

quint16 LicenseManager::getLicenseServerUrlHostNameFromNoticeServer()
{
	// get url
	quint16 nResult;
	GetUrlEvent getUrlEvent;
	getUrlEvent.nType = eUIETLicenseServerUrlGetEvent;
	getUrlEvent.url = LICENSE_SERVER_URL_HOSTNAME_GET_URL;
	getUrlEvent.environmentParam = ENVIRONMENT_PARAM;
	getUrlEvent.resultUrl = tr("");
	nResult = updaterLib::getInstance()->getUrlForLicenseServer(&getUrlEvent);
	m_strLicenseServerUrlHostName = getUrlEvent.resultUrl;
	// 실패 후 메시지 표시.
//	 if (nResult != eURRTSuccess)
//		 QMessageBox::warning(nullptr, "get url", UPDATE_DATA->updateReqResultMessage(nResult));

	return nResult;
}

QString LicenseManager::getUrl(eLicenseServerPathType type)
{
	QString strResultURL = tr("");
	QString strUrl = tr("");
	QString strUrlPath = tr("");
	switch (type)
	{
	case eLSPTLogin:
	{
		strUrl = LICENSE_LOGIN_TEST_SERVER_URL;
		strUrlPath = LICENSE_SERVER_LOGIN_PATH;
	}
	break;
	case eLSPTChkLicence:
	{
		strUrl = LICENSE_LICENSE_TEST_SERVER_URL;
		strUrlPath = LICENSE_SERVER_LICENSE_PATH;
	}
	break;
	case eLSPTGetVersion:
	{
		strUrl = UpdateManager::MEDIP_NOTICE_SERVER_URL;
		strUrlPath = UpdateManager::LICENSE_SERVER_GETVERSION_PATH;
	}
	break;
	case eLSPTChkFunctionUsableCount:
	{
		strUrl = LICENSE_SERVER_FUNCTIONUSABLECOUNT_URL;
		strUrlPath = LICENSE_SERVER_FUNCTIONUSABLECOUNT_PATH;
	}
	break;
	case eLSPTDecreaseFunctionUsableCount:
	{
		strUrl = LICENSE_SERVER_DECREASE_FUNCTIONUSABLECOUNT_URL;
		strUrlPath = LICENSE_SERVER_DECREASE_FUNCTIONUSABLECOUNT_PATH;
	}
	break;
	case eLSPTSSESubscribe:
	{
		strUrl = LICENSE_SSE_SUBSCRIBE_TEST_SERVER_URL;
		strUrlPath = LICENSE_SERVER_SSE_SUBSCRIBE_PATH;
	}
	break;
	case eLSPTSSESubscribe_test:
	{
		strUrl = "https://medicalip.net/sse/subscribe_test";
		strUrlPath = "sse/subscribe_test";
	}
	break;
	case eLSPTSSEUnSubscribe:
	{
		strUrl = LICENSE_SSE_UNSUBSCRIBE_TEST_SERVER_URL;
		strUrlPath = LICENSE_SERVER_SSE_UNSUBSCRIBE_PATH;
	}
	break;
	case eLSPTSSEAddNotificationHistory:
	{
		strUrl = LICENSE_SSE_ADDNOTIFICATIONHISTORY_TEST_SERVER_URL;
		strUrlPath = LICENSE_SERVER_SSE_ADDNOTIFICATIONHISTORY_PATH;
	}
	break;
	}

	if (!m_strLicenseServerUrlHostName.isEmpty())
		strResultURL = m_strLicenseServerUrlHostName + strUrlPath;
	else
		strResultURL = strUrl;

	return strResultURL;
}

int LicenseManager::chkFunctionUsableCount(eMEDIP_FUNCTION_LEVEL functionLevelId, eLicenseServerPathType usableCountType)
{
	int resultCount = -1;
	if (IsOnline())
		resultCount = network::chkFunctionUsableCount(getUrl(usableCountType), functionLevelId);
	else
	{
		switch (usableCountType)
		{
		case eLSPTChkFunctionUsableCount:
		{
			// 현재 오프라인 라이센스 파일에 암호화되어 기록된 function info를 얻어온다.
			sAuthenticationInfo sInfo;
			m_pMedipLicense->getLoginInfo(sInfo);
			QMap<qulonglong, int>& functionMap = sInfo.usableCountMap;
			QMap<qulonglong, int>::const_iterator iterMap = functionMap.find(functionLevelId);
			if (iterMap != functionMap.end())
				resultCount = iterMap.value();
		}
		break;
		case eLSPTDecreaseFunctionUsableCount:
		{
			// 현재 오프라인 라이센스 파일에 암호화되어 기록된 function info를 얻어온다.
			int curFunctionLevelId_UsableCount = 0;
			sAuthenticationInfo sInfo;
			m_pMedipLicense->getLoginInfo(sInfo);
			QMap<qulonglong, int>& functionMap = sInfo.usableCountMap;
			QMap<qulonglong, int>::const_iterator iterMap = functionMap.find(functionLevelId);
			if (iterMap != functionMap.end())
			{
				curFunctionLevelId_UsableCount = iterMap.value();
				functionMap.insert(functionLevelId, --curFunctionLevelId_UsableCount);
				if (m_pMedipLicense->updateFunctionInfo(functionMap))
					resultCount = curFunctionLevelId_UsableCount;
			}
		}
		break;
		}
	}
	return resultCount;
}

int LicenseManager::chkFunctionUsableCount(eMEDIP_FUNCTION_LEVEL functionLevelId, eLicenseServerPathType usableCountType, sFuncUsableCountResult& outData)
{
	int resultCount = -1;
	if (IsOnline())
	{
#ifdef LOCALHOST_API_TEST
		QString url = "";
		if (usableCountType == eLSPTChkFunctionUsableCount)
			url = "http://localhost:8080/licence/chkFunctionUsableCount";
		else if (usableCountType == eLSPTDecreaseFunctionUsableCount)
			url = "http://localhost:8080/licence/decreaseFunctionUsableCount";
		resultCount = network::chkFunctionUsableCount(url, functionLevelId, outData);
#else
		resultCount = network::chkFunctionUsableCount(getUrl(usableCountType), functionLevelId, outData);
#endif
	}
	else
	{
		switch (usableCountType)
		{
		case eLSPTChkFunctionUsableCount:
		{
			// 현재 오프라인 라이센스 파일에 암호화되어 기록된 function info를 얻어온다.
			sAuthenticationInfo sInfo;
			m_pMedipLicense->getLoginInfo(sInfo);
			QMap<qulonglong, int>& functionMap = sInfo.usableCountMap;
			QMap<qulonglong, int>::const_iterator iterMap = functionMap.find(functionLevelId);
			if (iterMap != functionMap.end())
				resultCount = iterMap.value();
		}
		break;
		case eLSPTDecreaseFunctionUsableCount:
		{
			// 현재 오프라인 라이센스 파일에 암호화되어 기록된 function info를 얻어온다.
			int curFunctionLevelId_UsableCount = 0;
			sAuthenticationInfo sInfo;
			m_pMedipLicense->getLoginInfo(sInfo);
			QMap<qulonglong, int>& functionMap = sInfo.usableCountMap;
			QMap<qulonglong, int>::const_iterator iterMap = functionMap.find(functionLevelId);
			if (iterMap != functionMap.end())
			{
				curFunctionLevelId_UsableCount = iterMap.value();
				functionMap.insert(functionLevelId, --curFunctionLevelId_UsableCount);
				if (m_pMedipLicense->updateFunctionInfo(functionMap))
					resultCount = curFunctionLevelId_UsableCount;
			}
		}
		break;
		}
	}
	return resultCount;
}

QString LicenseManager::loginResultMessage(qint16 resultCode)
{
	QString strResult = tr("");
	switch (resultCode)
	{
	case eLRCSuccess:
	{
		strResult = "Success";
	}
	break;
	case eLRCIdMismatch:
	{
		strResult = STRING_MANAGER->getString(ERR_DU_0001);
	}
	break;
	case eLRCPwdMismatch:
	{
		strResult = STRING_MANAGER->getString(ERR_DU_0007);
	}
	break;
	case eLRCUnauthorizedEmail:
	{
		strResult = "Unauthorized Email.";
	}
	break;
	case eLRCWithdrewUser:
	{
		strResult = "Withdrew User.";
	}
	break;
	case eLRCProductTypeMismatch:
	{
		strResult = "Product type mismatched.";
	}
	break;
	case eLRCPeriodMismatch:
	{
		strResult = "Usable period expired.";
	}
	break;
	case eLRCLoadLicenseFail:
	{
		strResult = "License load failed.";
	}
	break;
	case eLRCRedirectNeed:
	{
		strResult = "Network request need to be redirected.";
	}
	break;
	case eLRCNetworkError:
	{
		strResult = "Network error occurred.";
	}
	break;
	case eLRCJsonDataInvalid:
	{
		strResult = "Received json data is invalid.";
	}
	break;
	case eLRCUnauthorizedAccess:
	{
		strResult = "Unauthorized Access.";
	}
	break;
	case eLRCNotRemainUsableCountError:
	{
		strResult = "Not Remained Function Usable Count.";
	}
	break;
	case eLRCRejectUsableCountError:
	{
		strResult = "Rejected Function Usable Count.";
	}
	break;
	case eLRCLimitUsableCountError:
	{
		strResult = "Limited Function Usable Count.";
	}
	break;
	case eLRCNetworkTimeOut:
	{
		strResult = "Network time out.";
	}
	break;
	}
	return strResult;
}

bool LicenseManager::productTypeSetting()
{
	/* Product Type Setting */
	QString strLicensedServiceId = getLastLoginResultInfo().strServiceId;
	QVector<qulonglong>& functionList = getLastLoginResultInfo().functionList;

	bool bMedip = !strLicensedServiceId.compare(SERVICE_ID_MEDIP);
	bool bDeepCatch = !strLicensedServiceId.compare(SERVICE_ID_MEDIP_DEEPCATCH);
	bool bCovid19 = !strLicensedServiceId.compare(SERVICE_ID_MEDIP_COVID19);
	bool bMEDIP_AI = !strLicensedServiceId.compare(SERVICE_ID_MEDIP_AI);

	// update test용
	bool bMedipTest = !strLicensedServiceId.compare(SERVICE_ID_MEDIP_TEST);
	bool bDeepCatchTest = !strLicensedServiceId.compare(SERVICE_ID_MEDIP_DEEPCATCH_TEST);
	bool bCovid19Test = !strLicensedServiceId.compare(SERVICE_ID_MEDIP_COVID19_TEST);

	bool bFunctionLevel_Light = isSubscribeFunctionLevel(MFL_Product_MEDIP_MEDIPLight);
	bool bFunctionLevel_Research = isSubscribeFunctionLevel(MFL_Product_MEDIP_MEDIPResearch);
	bool bFunctionLevel_Pro = isSubscribeFunctionLevel(MFL_Product_MEDIP_MEDIPPro);
	bool bFunctionLevel_MDBox = isSubscribeFunctionLevel(MFL_Product_MEDIP_MEDIPMDBox);
	bool bFunctionLevel_DEMO_FULL = isSubscribeFunctionLevel(MFL_Product_MEDIP_MEDIPFull_DEMOMEDIPFull);
	bool bFunctionLevel_MEDIP_AI = isSubscribeFunctionLevel(MFL_Product_MEDIP_AI);

	bool bSubscribeDeepCatch = isSubscribeFunctionLevel(MFL_Product_DeepCatch);
	bool bSubscribeDeepCatch_v2 = isSubscribeFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2);
	bool bOnline = IsOnline();

	// product는 deepcatch인데 on/offline function list에는 deepcatch가 구독이 안된 경우 예외 처리.
	if ((bDeepCatch || bDeepCatchTest) && (!bSubscribeDeepCatch && !bSubscribeDeepCatch_v2))
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_0006).arg(getProductName()));
		return false;
	}

	// medip or mediptest instance 생성.
	if (bMedip || bMedipTest)
	{
		if (bFunctionLevel_DEMO_FULL)
		{
			m_pProductManager->SetProduct(MedipType::New(L"Demo", L"데모", false));
		}
		else if (bFunctionLevel_Light)
		{
			m_pProductManager->SetProduct(MedipType::New(L"Light", L"연구용", false));
		}
		else if (bFunctionLevel_Research)
		{
			m_pProductManager->SetProduct(MedipType::New(L"Research", L"연구용", false));
		}
		else if (bFunctionLevel_Pro)
		{
			m_pProductManager->SetProduct(MedipType::New(L"Pro", L"의료기기", false));
		}
		else if (bFunctionLevel_MDBox)
		{
			m_pProductManager->SetProduct(MedipType::New(L"Demo", L"연구용", false));
		}
		else
		{
			QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_0006).arg(getProductName()));
			return false;
		}

		MIP_ENCODER_SINGLTON->SetProjectMethod(MIP_ENCODER::PT_MIP); // login 완료 후로 이동해야함
	}
	// deepCatch instance 생성.
	else if ((bDeepCatch || bDeepCatchTest) && (bSubscribeDeepCatch == true && bSubscribeDeepCatch_v2 == false))
	{
		m_pProductManager->SetProduct(MedipType::New(L"DeepCatch", L"연구용", false));
		MIP_ENCODER_SINGLTON->SetProjectMethod(MIP_ENCODER::PT_MIPD); // login 완료 후로 이동해야함
	}
	else if (/*(bDeepCatch || bDeepCatchTest) &&*/ bSubscribeDeepCatch_v2)
	{
		m_pProductManager->SetProduct(MedipType::New(L"DeepCatch V2", L"연구용", false));
		MIP_ENCODER_SINGLTON->SetProjectMethod(MIP_ENCODER::PT_MIPD); // login 완료 후로 이동해야함
	}

	// 공통
	// function list 받아서 처리.
	for (int i = 0; i < functionList.size(); i++)
	{
		// function list에서 product type은 위쪽에서 인스턴스 생성을 통해 생성된다.
		//	if (!isSubscribeProductTypeFunctionLevel((eMEDIP_FUNCTION_LEVEL)functionList.at(i)))
		m_pProductManager->AddFunctionLevel((eMEDIP_FUNCTION_LEVEL)functionList.at(i), CREATE);
	}


	return true;
}

void LicenseManager::checkProductTypeStateChange()
{
	bool bResultProductTypeChangeState = false;
	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_ACCOUNT_STATE_INFO_PURCHASEPOPUP_FILENAME;
	// 계정 상태 정보 파일을 읽어 해당 계정 정보가 없으면 false이고 계정 정보가 있으면 그 값을 읽는다.
	m_mapAccountStateInfo_purchasePopup.clear();
	QFile file_read(configFilePath);
	QDataStream dataStream_read(&file_read);
	if (file_read.open(QIODevice::ReadOnly))
	{
		dataStream_read >> m_mapAccountStateInfo_purchasePopup;
		file_read.close();
	}

	if (!m_mapAccountStateInfo_purchasePopup.isEmpty())
	{
		QMap<QString, bool>::const_iterator find_iter = m_mapAccountStateInfo_purchasePopup.find(getAuthenticatedID());
		if (find_iter != m_mapAccountStateInfo_purchasePopup.end())
		{
			//	bResultProductTypeChangeState = QVariant(find_iter.value()).toBool();
			bResultProductTypeChangeState = find_iter.value();
		}
	}

	// 해당 계정의 medipProductTypeStateChange 상태가 false면
	if (!bResultProductTypeChangeState)
	{
		// MEDIP에서 가능한 product type을 체크하여 Light만 남은 경우.
		if (isSubscribeProductTypeOnlyMedipLight())
		{
			// medipProductTypeStateChange 변수값을 true로 셋팅.
			m_mapAccountStateInfo_purchasePopup[getAuthenticatedID()] = true;
			QFile file_write(configFilePath);
			QDataStream dataStream_write(&file_write);
			if (file_write.open(QIODevice::WriteOnly))
			{
				dataStream_write << m_mapAccountStateInfo_purchasePopup;
				file_write.close();
			}
			// 변동되었음을 메시지 표시.
			QMessageBox msg(nullptr);
			msg.setWindowTitle(STRING_MANAGER->getString(STR_WARN));
			msg.setTextFormat(Qt::RichText);
			msg.setText(STRING_MANAGER->getString(ERR_DU_1019));
			msg.setStandardButtons(QMessageBox::Ok);
			msg.exec();
			return;
		}
	}
	// 해당 계정의 medipProductTypeStateChange 상태가 true면
	else
	{
		// MEDIP에서 가능한 product type을 체크하여 Light외에 1개 이상이 더 있는 경우.
		if (!isSubscribeProductTypeOnlyMedipLight())
		{
			// medipProductTypeStateChange 변수값을 false로 셋팅.
			m_mapAccountStateInfo_purchasePopup[getAuthenticatedID()] = false;
			QFile file_write(configFilePath);
			QDataStream dataStream_write(&file_write);
			if (file_write.open(QIODevice::WriteOnly))
			{
				dataStream_write << m_mapAccountStateInfo_purchasePopup;
				file_write.close();
			}
		}
	}

}

// 사용자 계정의 공지 사항 Subject 스트링 정보들이 저장된 파일을 parsing된 공지는 공지사항 리스트에서 필터링.
void LicenseManager::getFilterNoticeList(QStringList& _filterNoticeList)
{
	QStringList strList_DontSeeAgainSubjects = { "", };
	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_FILTER_NOTICE_LIST_FILENAME;
	QFile file_read(configFilePath);
	QDataStream dataStream_read(&file_read);
	if (file_read.open(QIODevice::ReadOnly))
	{
		dataStream_read >> _filterNoticeList;
		file_read.close();
	}
}

// filtering할 공지 사항 Subject 스트링 정보를 저장.
void LicenseManager::setFilterNoticeList(QList<sNoticeListInfo>& ShowedNoticeListVec)
{
	QStringList filterNoticeList;
	filterNoticeList.clear();
	QString configFilePath = m_strConfigPath + tr("/") + LICENSE_FILTER_NOTICE_LIST_FILENAME;
	QFile file_read(configFilePath);
	QDataStream dataStream_read(&file_read);
	if (file_read.open(QIODevice::ReadOnly))
	{
		dataStream_read >> filterNoticeList;
		file_read.close();
	}

	for (int i = 0; i < ShowedNoticeListVec.size(); i++)
		filterNoticeList += ShowedNoticeListVec.at(i).noticeSubject;

	QFile file_write(configFilePath);
	QDataStream dataStream_write(&file_write);
	if (file_write.open(QIODevice::WriteOnly))
	{
		dataStream_write << filterNoticeList;
		file_write.close();
	}
}

void LicenseManager::getProductTypeEndDateAndPeriod(QString& strEndDate, QString& strPeriod)
{
	if (isFreeProductTypeFunctionLevel())
	{
		// deepcatch v1, covid, MEDIP Light
		strEndDate = "";
		strPeriod = "";
	}
	else
	{
		// demo 포함 paid Product type.(deepcatch v1, covid, MEDIP Light 제외.)
		eMEDIP_FUNCTION_LEVEL curProductTypeFunctionLevelId = paidProductTypeFunctionLevel();
		if (curProductTypeFunctionLevelId != 0)	// deepcatch v1, covid, light 버전 제외.
		{
			if (IsOnline())
			{
				QMap<qulonglong, sFunctionLevelListInfo>& functionLevelListMap = getLastLoginResultInfo().functionLevelListMap;
				QMap<qulonglong, sFunctionLevelListInfo>::const_iterator iterMap = functionLevelListMap.find(curProductTypeFunctionLevelId);
				if (iterMap != functionLevelListMap.end())
				{
					sFunctionLevelListInfo functionLevelListInfo;
					functionLevelListInfo = iterMap.value();
					strEndDate = functionLevelListInfo.strFunctionEndDttm.left(8);
					QDate endDate = QDate::fromString(strEndDate, tr("yyyyMMdd"));
					//	strEndDate = endDate.toString("MMM d yyyy");
					strEndDate = QLocale("en_US").toString(endDate, "MMM d yyyy");
					QDate curDate = QDate::currentDate();
					qint64 remainedDay = curDate.daysTo(endDate);
					strPeriod = QVariant(remainedDay).toString();
				}
			}
			else
			{
				strEndDate = getLastLoginResultInfo().strServiceEndDttm;
				QDate endDate = QDate::fromString(strEndDate, tr("yyyyMMdd"));
				//	strEndDate = endDate.toString("MMM d yyyy");
				strEndDate = QLocale("en_US").toString(endDate, "MMM d yyyy");
				QDate curDate = QDate::currentDate();
				qint64 remainedDay = curDate.daysTo(endDate);
				strPeriod = QVariant(remainedDay).toString();
			}
		}
	}
}

void LicenseManager::getExpiredDateDisplayString(QString& strExpiredDate)
{
	QString strEndDate = "";
	QString strPeriod = "";
	getProductTypeEndDateAndPeriod(strEndDate, strPeriod);
	if (strEndDate.isEmpty() && strPeriod.isEmpty())
	{
		strExpiredDate = "&nbsp;(Free version, ";
		if (!getProductName().compare(PRODUCT_NAME_DEEPCATCH) || !getProductName().compare(PRODUCT_NAME_DEEPCATCH_V2))
			strExpiredDate += QString("<a href = 'mailto:contact@medicalip.com'>contact@medicalip.com</a> for more credit purchase.)");
		else
			strExpiredDate += QString("<a href = 'mailto:contact@medicalip.com'>contact@medicalip.com</a> for purchase.)");
	}
	else
		strExpiredDate = QString("&nbsp;(Expiration date : %1, %2 days left)").arg(strEndDate).arg(strPeriod);
}

// SSE Control
bool LicenseManager::sseSubscribe()
{
	QString url = tr("");
	url = LICENSE_DATA->getUrl(eLSPTSSESubscribe);
	//	url = "http://192.168.1.105:8080/sse/subscribe";
	if (m_SSENet)
	{
		m_SSENet->subscribe(QUrl(url), getLastLoginResultInfo().strAccessToken, SSENetwork::SSE_FIRST_CONNECTION);

		// subscibe 완료 메시지를 받을때까지 기다려서 받으면 다음으로 정상 진행.
		// 받지 못하면 로그인 무효 처리.
		QTimer timer;
		timer.setSingleShot(true);
		QEventLoop loop;
		connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
		connect(m_SSENet, SIGNAL(SSESubscibeComplete()), &loop, SLOT(quit()));
		timer.start(SSENetwork::SSE_SUBSCRIBE_MAX_WAITTING_TIME);
		loop.exec();

		if (!timer.isActive())
		{
			// SSESubscibeComplete 시그널을 받지 못하고 time out된 경우.
			return false;
		}

		disconnect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
		disconnect(m_SSENet, SIGNAL(SSESubscibeComplete()), &loop, SLOT(quit()));
		timer.stop();
	}
	return true;
}

void LicenseManager::sseUnSubscribe()
{
	QString url = tr("");
	url = LICENSE_DATA->getUrl(eLSPTSSEUnSubscribe);
	//	url = "http://192.168.1.105:8080/sse/unsubscribe";
	if (m_SSENet)
	{
		if (!m_SSENet->IsDuplicatedLogin())
		{
			m_SSENet->unsubscribe(QUrl(url), getLastLoginResultInfo().strAccessToken);
			//	m_SSENet->unsubscribe(QUrl(url), getAuthenticatedID(), getServiceID());
		}
	}
}

void LicenseManager::sseAddNotificationHistory()
{
	QString url = tr("");
	url = LICENSE_DATA->getUrl(eLSPTSSEAddNotificationHistory);
	//	url = "http://192.168.1.105:8080/sse/addNotificationHistory_test";
	if (m_SSENet)
	{
		//	m_SSENet->addNotificationHistory(QUrl(url), getLastLoginResultInfo().strAccessToken, SSENetwork::SSE_DUPLICATED_LOGIN);
		m_SSENet->addNotificationHistory(QUrl(url), getAuthenticatedID(), getServiceID(), SSENetwork::SSE_DUPLICATED_LOGIN);
	}
}

bool LicenseManager::isForceLogOutCase()
{
	if (m_SSENet)
	{
		return m_SSENet->isForceLogOutCase();
	}
	else
	{
		return false;
	}
}

void LicenseManager::setForceLogOutCase(bool _bForceLogOutCase)
{
	if (m_SSENet)
	{
		m_SSENet->setForceLogOutCase(_bForceLogOutCase);
	}
}

bool LicenseManager::Login(const QString& id, const QString& pwd, const QString& serviceID)
{
	sLoginResultInfo result = {};
	//if (m_pMedipLicense->TryLogin(&result, id, pwd, SERVICE_ID_MEDIP) == false)
	if (m_pMedipLicense->TryLogin(&result, id, pwd, serviceID) == false)
	{
		return false;
	}

	m_LastLoginResultInfo = result;

	return true;
}

void LicenseManager::SetProductManager(ProductManager* pProductManager)
{
	m_pProductManager = pProductManager;
}

ProductManager* LicenseManager::GetProdcutManager() const
{
	return m_pProductManager;
}

void LicenseManager::SetMedipLicense(Medip_License* pMedipLicense)
{
	m_pMedipLicense = pMedipLicense;
}

Medip_License* LicenseManager::GetMedipLicense() const
{
	return m_pMedipLicense;
}

bool LicenseManager::ActivateOfflineLicense(sAuthenticationInfo authInfo, QString masterKeyFilePath)
{
	if (m_pMedipLicense->ActivateInClient(authInfo, masterKeyFilePath) == false)
	{
		return false;
	}

	return true;
}

bool LicenseManager::CreateOfflineMasterKey(sAuthenticationInfo authInfo, QString masterKeyDirPathPath, QString* pOutMasterKeyName)
{
	authInfo.strPCcode = m_pMedipLicense->getInfoData(authInfo.strNicName);
	if (m_pMedipLicense->MakeMasterKeyInServerWithDirectoryPath(authInfo, masterKeyDirPathPath, pOutMasterKeyName) == false)
	{
		return false;
	}

	return true;
}

QString LicenseManager::GetSubProductVersionName() const
{
	QString productSubVersion;
#ifdef ALPHA_VERSION
	productSubVersion = "Alpha";
#elif defined BETA_VERSION
	productSubVersion = "Beta";
#endif 

	return productSubVersion;
}
