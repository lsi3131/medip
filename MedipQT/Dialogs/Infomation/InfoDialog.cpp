#include "stdafx.h"
#include "InfoDialog.h"
#include "MedipQT.h"

#include "stringManager.h"
#include "resourceManager.h"
#include "styleManager.h"
#include "windowManager.h"
#include "LicenseManager.h"
#include "OpenSourceLicenseDlg.h"
#include "ModuleInfoDialog.h"

InfoDialog::InfoDialog(QWidget* parent) :
	QDialog(parent),
	m_rValue(0)
{
	m_pWinManager = WIN_MANAGER;
	m_pLicense = LICENSE_DATA;
	m_pStrManager = STRING_MANAGER;

	setWindowTitle(m_pStrManager->getString(STR_INFO_DIALOG));
	setStyleSheet("background-color: rgba(255, 255, 255, 255); color: rgba(0,0,0,255);");

	QVBoxLayout* verticalLayout = new QVBoxLayout(this);
	QVBoxLayout* subVbox = new QVBoxLayout;
	QHBoxLayout* subHbox = new QHBoxLayout;
	QHBoxLayout* subHbox2 = new QHBoxLayout;

	verticalLayout->addLayout(subVbox);
	verticalLayout->addLayout(subHbox);
	verticalLayout->addLayout(subHbox2);

	QLabel* Logo = new QLabel(this);
	QLabel* desc = new QLabel(this);

	subVbox->addWidget(Logo);
	subVbox->addWidget(desc);

	QImage* WaterImage = RESOURCE_MANAGER->getWaterMark();

	if (WaterImage != NULL)
		Logo->setPixmap(QPixmap::fromImage(*WaterImage));

	QString strExpiredDate = "";
	m_pLicense->getExpiredDateDisplayString(strExpiredDate);
	QString strProgramEdition = m_pStrManager->getString(STR_PROGRAM_EDITION);
	QString alphaBeta;
#if defined ALPHA_VERSION
	alphaBeta = "<strong> Alpha</strong>";
#elif defined BETA_VERSION
	alphaBeta = "<strong> Beta</strong>";
#endif

	desc->setText(strProgramEdition + alphaBeta + strExpiredDate + tr("<br>") +
		m_pStrManager->getString(STR_PROGRAM_VERSION) + tr("<br>") +
		m_pStrManager->getString(STR_PROGRAM_COPYRIGHT));
	desc->setTextInteractionFlags(Qt::TextBrowserInteraction);
	connect(desc, &QLabel::linkActivated, this, &InfoDialog::slot_MailLinkClick);

	QHBoxLayout* subHbox_CommonLink = new QHBoxLayout;
	QWidget* subHbox_CommonLink_Widget = new QWidget;
	subHbox_CommonLink_Widget->setLayout(subHbox_CommonLink);
	subHbox_CommonLink->setContentsMargins(0, 0, 0, 0);
	subHbox_CommonLink->setAlignment(Qt::AlignLeft);
	subVbox->addWidget(subHbox_CommonLink_Widget);

	QLabel* labelpenSource = new QLabel(this);
	labelpenSource->setText(m_pStrManager->getString(STR_OPENSOURCE));
	labelpenSource->setStyleSheet(STYLE_MANAGER->labelAddr);
	labelpenSource->setTextInteractionFlags(Qt::TextBrowserInteraction);
	subHbox_CommonLink->addWidget(labelpenSource);
	connect(labelpenSource, &QLabel::linkActivated, this, &InfoDialog::slot_OpenSourceLinkClick);

	QLabel* labelModuleInfo = new QLabel(this);
	labelModuleInfo->setText(m_pStrManager->getString(STR_MODULE_INFO));
	labelModuleInfo->setStyleSheet(STYLE_MANAGER->labelAddr);
	labelModuleInfo->setTextInteractionFlags(Qt::TextBrowserInteraction);
	subHbox_CommonLink->addWidget(labelModuleInfo);
	connect(labelModuleInfo, &QLabel::linkActivated, this, &InfoDialog::slot_ModuleInfoClick);

	QLabel* desc2 = new QLabel(this);
	subVbox->addWidget(desc2);
	desc2->setText(m_pStrManager->getString(STR_MEDICALIP_ADDR));
	desc2->setStyleSheet(STYLE_MANAGER->labelAddr);
	desc2->setTextInteractionFlags(Qt::TextBrowserInteraction);
	connect(desc2, &QLabel::linkActivated, this, &InfoDialog::slot_LinkClick);
	connect(this, &QDialog::finished, this, &InfoDialog::slot_finish);

	QTextEdit* content = new QTextEdit(this);
	content->setContentsMargins(QMargins(0, 0, 0, 0));
	content->setText(m_pStrManager->getString(STR_PROGRAM_DESC));
	content->setReadOnly(true);
	subVbox->addWidget(content);

	if (m_pWinManager->IsLicensePass())
	{
		QLabel* passedMsg = new QLabel(this);
		subHbox->addWidget(passedMsg);
		passedMsg->setText(m_pStrManager->getString(STR_PASSED_MSG));

		QString authenticatedID = m_pLicense->getAuthenticatedID();
		QString strOnOfflineMode = m_pLicense->getSelectedMode() == LAM_ONLINE ? QString("Online") : QString("Offline");
		QString strTrial = m_pLicense->getTrialVer() ? tr(" trial") : tr("");
		QLabel* pLabelLoginID = new QLabel(QString("Authenticated ID(%1%2) : %3").arg(strOnOfflineMode).arg(strTrial).arg(authenticatedID));
		QPushButton* pBtnlogOut = new QPushButton("Log out");
		pBtnlogOut->setFlat(true);
		subHbox2->addWidget(pLabelLoginID);
		subHbox2->addWidget(pBtnlogOut);
		connect(pBtnlogOut, &QPushButton::clicked, this, &InfoDialog::slot_logOut);
	}

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	int w = minimumSizeHint().width();
	int h = sizeHint().height();

	resize(w, h);
}

InfoDialog::~InfoDialog()
{

}

void InfoDialog::slot_LinkClick()
{
	m_rValue = LINK_CLICKED;
	close();
}

void InfoDialog::slot_MailLinkClick()
{
	m_rValue = MAIL_LINK_CLICKED;
	close();
}

void InfoDialog::slot_OpenSourceLinkClick()
{
	OpenSourceLicenseDlg dlg(this);
	dlg.exec();
}

void InfoDialog::slot_ModuleInfoClick()
{
	ModuleInfoDialog dlg(this);
	dlg.exec();
}

void InfoDialog::slot_finish()
{
	if (0 != m_rValue)
	{
		setResult(m_rValue);
	}
}

void InfoDialog::slot_logOut()
{
	// 자동 로그인 해제.	
	this->done(QDialog::Accepted);

	QMessageBox dlg(this);
	QPushButton* btnYes, * btnNo;
	dlg.setWindowTitle("Log Out");
	dlg.setText("Do you want to log out now? if you, the program will restart now.");
	btnYes = dlg.addButton(QMessageBox::Yes);
	btnNo = dlg.addButton(QMessageBox::No);

	if (dlg.exec())
	{
		if (btnYes == dlg.clickedButton()) //작업 중 내용 저장하고, 재실행 => closeEvent에서 저장 안되어 있으면 저장할 건지 물어봄
		{
#if 0
			m_pLicense->setConfig_License(eLCETLoginAuto, QVariant(false).toString());
#else
			QVector<eLicenseConfigElementType> keyVector{ eLCETLoginAuto, eLCETLoginAutoID,  eLCETLoginAutoPWD };
			QStringList valueList;
			valueList << QVariant(false).toString() << "" << "";
			m_pLicense->setConfig_License(keyVector, valueList);
#endif

			m_pWinManager->mainWindow->RebootProgram();
		}
	}
}