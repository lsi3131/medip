#include "stdafx.h"
#include "LoginDlg.h"

#include "stringManager.h"
#include "LicenseManager.h"
#include "styleManager.h"
#include "windowManager.h"
#include "resourceManager.h"


LoginDlg::LoginDlg(QWidget *parent)
	: QDialog(parent, Qt::WindowCloseButtonHint)
{		
	const QIcon winIcon = QIcon::fromTheme("Medip", RESOURCE_MANAGER->getIcon(ICON_APP));
	setWindowIcon(winIcon);

	QString strProductName = LICENSE_DATA->getProductName();
	QString strOnOffline = LICENSE_DATA->getSelectedMode() == LAM_OFFLINE ? tr("(Offline Authentication)") : tr("(Online Authentication)");
	setWindowTitle(strProductName + " - " +strOnOffline);

	setStyleSheet(STYLE_MANAGER->mainFrame);

	QString SavedID = ("");
	QString SavedPWD = ("");
	bool bIDSaveCheck = false;

	QVector<eLicenseConfigElementType> keyVector{ eLCETLoginIDSaveCheck, eLCETLoginID };
	QMap<muint8, QString> mapLicenseInfo;
	bool res = LICENSE_DATA->getConfig_License(keyVector, mapLicenseInfo);
	if (res)
	{
		QMap<muint8, QString>::const_iterator find_iter;
		if ((find_iter = mapLicenseInfo.find(eLCETLoginIDSaveCheck)) != mapLicenseInfo.end() && find_iter.key() == eLCETLoginIDSaveCheck)
			bIDSaveCheck = QVariant(find_iter.value()).toBool();
		if ((find_iter = mapLicenseInfo.find(eLCETLoginID)) != mapLicenseInfo.end() && find_iter.key() == eLCETLoginID)
			SavedID = find_iter.value();
	}

	// decryption
	QString strDecryptedID = tr("");
	QString strDecryptedPWD = tr("");
	if (!SavedID.isEmpty())
		Medip_License::GetInstance()->decryptMedipString(SavedID, strDecryptedID);
	if (!SavedPWD.isEmpty())
		Medip_License::GetInstance()->decryptMedipString(SavedPWD, strDecryptedPWD);
	
	QBoxLayout *pVerMain = new QVBoxLayout();
	pVerMain->setMargin(50);
	pVerMain->setSpacing(15);

	//pMainLayout->addLayout(pVerMain);
		
	QLabel* pLabelLogin = new QLabel(this);
	pLabelLogin->setText("Login");
	pLabelLogin->setStyleSheet("QLabel{font-size:42px;}");

	m_lineEdit_id  = new QLineEdit();
	m_lineEdit_id->setPlaceholderText(STRING_MANAGER->getString(STR_LOGIN_ID));
	if (LICENSE_DATA->getAutoLogin())
		m_lineEdit_id->setText(LICENSE_DATA->getStrDecryptedID());
	else if (bIDSaveCheck)
		m_lineEdit_id->setText(strDecryptedID);
	m_lineEdit_id->setStyleSheet(STYLE_MANAGER->m_editBoxLogin);
	m_lineEdit_id->setContextMenuPolicy(Qt::NoContextMenu);

	m_lineEdit_pwd = new QLineEdit();
	m_lineEdit_pwd->setPlaceholderText(STRING_MANAGER->getString(STR_LOGIN_PWD));
	m_lineEdit_pwd->setEchoMode(QLineEdit::Password);
	m_lineEdit_pwd->setText(LICENSE_DATA->getStrDecryptedPWD());
	m_lineEdit_pwd->setStyleSheet(STYLE_MANAGER->m_editBoxLogin);
	m_lineEdit_pwd->setContextMenuPolicy(Qt::NoContextMenu);

	QPushButton *pBtnLogin = new QPushButton(STRING_MANAGER->getString(STR_LOGIN));
	pBtnLogin->setStyleSheet(STYLE_MANAGER->buttonBehind + QString("QPushButton{font-size:15px;}"));
	pBtnLogin->setFixedHeight(32);	
	
	chkBox_idsave = new QCheckBox(STRING_MANAGER->getString(STR_LOGIN_IDSAVE));
	chkBox_idsave->setChecked(bIDSaveCheck);
	chkBox_idsave->setStyleSheet(STYLE_MANAGER->m_Checkbox);

	chkBox_autologin = new QCheckBox(STRING_MANAGER->getString(STR_LOGIN_AUTO));
	chkBox_autologin->setChecked(LICENSE_DATA->getAutoLogin());
	chkBox_autologin->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	

	// main layout 에 추가
	pVerMain->addWidget(pLabelLogin, 1, Qt::AlignCenter| Qt::AlignHCenter);
	pVerMain->addWidget(m_lineEdit_id, 1);
	pVerMain->addWidget(m_lineEdit_pwd, 1);

	pVerMain->addWidget(pBtnLogin, 1);

	QHBoxLayout *hLayoutAutoLoginCheck = new QHBoxLayout();
	pVerMain->addLayout(hLayoutAutoLoginCheck);

	//hLayoutAutoLoginCheck->addWidget(chkBox_idsave, 1);
	hLayoutAutoLoginCheck->addWidget(chkBox_autologin, 1);
	hLayoutAutoLoginCheck->addWidget(chkBox_idsave, 1);

	setLayout(pVerMain);
		
	connect(chkBox_idsave, SIGNAL(clicked()), this, SLOT(idSave()));
	connect(chkBox_autologin, SIGNAL(clicked()), this, SLOT(autologin()));
	connect(pBtnLogin, SIGNAL(clicked()), this, SLOT(loginExecute()));

	connect(Medip_License::GetInstance(), SIGNAL(loginFinished(sLoginResultInfo)), this, SLOT(onloginFinished(sLoginResultInfo)));
	connect(Medip_License::GetInstance(), SIGNAL(licenseCheckFinished(sLoginResultInfo)), this, SLOT(onlicenseCheckFinished(sLoginResultInfo)));

}

LoginDlg::~LoginDlg()
{

}

void LoginDlg::autologin()
{
	if (chkBox_autologin->isChecked() && chkBox_idsave->isChecked())
		chkBox_idsave->setCheckState(Qt::Unchecked);
}

void LoginDlg::idSave()
{
	if (chkBox_idsave->isChecked() && chkBox_autologin->isChecked())
		chkBox_autologin->setCheckState(Qt::Unchecked);

	// id 저장
	QMap<muint8, QString> mapLicenseInfo;
	QString id = tr("");
	id = m_lineEdit_id->text();
	QString strEncryptedID = tr("");
	mapLicenseInfo[eLCETLoginIDSaveCheck] = QVariant(chkBox_idsave->isChecked()).toString();
	if (!id.isEmpty()) {
		Medip_License::GetInstance()->encryptMedipString(id, strEncryptedID);
		mapLicenseInfo[eLCETLoginID] = strEncryptedID;
	}
	LICENSE_DATA->setConfig_License(mapLicenseInfo);
}

void LoginDlg::loginExecute()
{
	QString id = tr("");
	QString pwd = tr("");
	QString url = tr("");
	id = m_lineEdit_id->text();
	pwd = m_lineEdit_pwd->text();

	idSave();

	if (LICENSE_DATA->getSelectedMode() == LAM_OFFLINE)
	{
		sAuthenticationInfo sInfo;
		sInfo.strID = id;
		sInfo.strPWD = pwd;
		sInfo.strProductType = LICENSE_DATA->getProductType();
		Medip_License::GetInstance()->setLoginInfo(sInfo);
	}
	else if (LICENSE_DATA->getSelectedMode() == LAM_ONLINE)
		url = LICENSE_DATA->getUrl(eLSPTLogin);

	Medip_License::GetInstance()->loginCheck(id, pwd, LICENSE_DATA->getServiceID(), url);
}

void LoginDlg::onloginFinished(const sLoginResultInfo &info)
{
#if 0
	switch (LICENSE_DATA->getSelectedMode())
	{
		case LAM_ONLINE:
		{
			if (!bSuccess) 
			{
				QMessageBox::warning(this, "Online login", "Online login failed(login)!");
			}
			else {
				// 리턴된 accessToken을 이용해 라이센스 체크 진행.
				if (!accessToken.isEmpty())	// accessToken 문자가 공백 문자가 아닌 경우 정상적으로 로그인이 완료된 상태.
				{
					QString url = LICENSE_DATA->getUrl(eLSPTChkLicence);
					Medip_License::GetInstance()->licenseCheck(LICENSE_DATA->getServiceID(), LICENSE_DATA->getServiceAuthorityLevelId(), url);
				}
			}
		}
		break;
	}
#else
	if (info.nResultCode == eLRCSuccess)
	{
		// 리턴된 accessToken을 이용해 라이센스 체크 진행.
		if (!info.strAccessToken.isEmpty())	// accessToken 문자가 공백 문자가 아닌 경우 정상적으로 로그인이 완료된 상태.
		{
			QString url = LICENSE_DATA->getUrl(eLSPTChkLicence);
			Medip_License::GetInstance()->licenseCheck(LICENSE_DATA->getServiceID(), LICENSE_DATA->getServiceAuthorityLevelId(), url);
		}
	}
	else
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), LICENSE_DATA->loginResultMessage(info.nResultCode)).exec();
#endif
}

void LoginDlg::onlicenseCheckFinished(const sLoginResultInfo &info)
{
#if 0
	switch (LICENSE_DATA->getSelectedMode())
	{
		case LAM_ONLINE:
		{
			if (info.bResult)
			{
				LICENSE_DATA->setAuthenticatedID(m_lineEdit_id->text());
				// 자동 로그인 정보 SAVE
				// 로그인 성공 + 자동 로그인 플레그 ON이면 
				if (IsAutoLoginCheck())
					saveAutoLoginInfo();
				WIN_MANAGER->SetLicensePass(true);

				// 로그인 결과 서버로부터 전송받은 serviceid, authoritylevelid를 멤버로 셋팅하여 product별 instance 생성시 사용.
				LICENSE_DATA->setLastLoginResultInfo(info);

				this->done(QDialog::Accepted);
			}
			else
			{
				QMessageBox::warning(this, "Online login", "Online login failed(license)!");
			}
		}
		break;
		case LAM_OFFLINE:
		{
			if (info.bResult)
			{
				// 인증된 아이디 정보 저장.
				LICENSE_DATA->setAuthenticatedID(m_lineEdit_id->text());

				// 자동 로그인 정보 SAVE
				if (IsAutoLoginCheck())
					saveAutoLoginInfo();

				WIN_MANAGER->SetLicensePass(true);

				// 인증 완료 후 trial version 상태 저장.
				sAuthenticationInfo sInfo;
				Medip_License::GetInstance()->getLoginInfo(sInfo);
				if (!sInfo.strIsTrial.isEmpty())
					LICENSE_DATA->setTrialVer(QVariant(sInfo.strIsTrial).toBool());

				// 오프라인의 경우 serviceid와 authoritylevelid 처리.
				LICENSE_DATA->setLastLoginResultInfo(info);

				this->done(QDialog::Accepted);
			}
			else
			{
				QMessageBox::warning(this, "Offline login", "Offline login failed(license)!");
			}
		}
		break;
	}
#else
	if (info.nResultCode == eLRCSuccess)
	{
		LICENSE_DATA->setAuthenticatedID(m_lineEdit_id->text());
		// 자동 로그인 정보 SAVE
		// 로그인 성공 + 자동 로그인 플레그 ON이면 
		if (IsAutoLoginCheck())
			saveAutoLoginInfo();
		WIN_MANAGER->SetLicensePass(true);

		if (!LICENSE_DATA->IsOnline())
		{
			// 인증 완료 후 trial version 상태 저장.
			sAuthenticationInfo sInfo;
			Medip_License::GetInstance()->getLoginInfo(sInfo);
			if (!sInfo.strIsTrial.isEmpty())
				LICENSE_DATA->setTrialVer(QVariant(sInfo.strIsTrial).toBool());
			if (!info.strLicenseType.isEmpty())
			{
				LICENSE_DATA->setLicenseType(info.strLicenseType);
			}
				
		}

		// 로그인 결과 서버로부터 전송받은 serviceid, authoritylevelid를 멤버로 셋팅하여 product별 instance 생성시 사용.
		LICENSE_DATA->setLastLoginResultInfo(info);

		// 로그인 성공 후 SSE Subscribe.
#ifdef DISABLE_DUPLICATE_LOGIN
		this->done(QDialog::Accepted);
#else
		if (!LICENSE_DATA->sseSubscribe())
		{
			//	LICENSE_DATA->sseUnSubscribe();
			QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), "SSE Subscribe fail.!!!");
			this->done(QDialog::Rejected);
		}
		else
			this->done(QDialog::Accepted);
#endif
	}
	else
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), LICENSE_DATA->loginResultMessage(info.nResultCode)).exec();
#endif
}

void LoginDlg::saveAutoLoginInfo()
{
	QString id = tr("");
	QString pwd = tr("");
	QMap<muint8, QString> mapLicenseInfo;
	QString strEncryptedID = tr("");
	QString strEncryptedPWD = tr("");

	id = m_lineEdit_id->text();
	pwd = m_lineEdit_pwd->text();

	mapLicenseInfo[eLCETLoginAuto] = QVariant(IsAutoLoginCheck()).toString();
	if (!id.isEmpty()) {
		Medip_License::GetInstance()->encryptMedipString(id, strEncryptedID);
		mapLicenseInfo[eLCETLoginAutoID] = strEncryptedID;
	}
	if (!pwd.isEmpty()) {
		Medip_License::GetInstance()->encryptMedipString(pwd, strEncryptedPWD);
		mapLicenseInfo[eLCETLoginAutoPWD] = strEncryptedPWD;
	}
	LICENSE_DATA->setConfig_License(mapLicenseInfo);
}
