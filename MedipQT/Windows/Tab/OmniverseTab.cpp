#include "stdafx.h"
#include "OmniverseTab.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "StyleManager.h"
#include "LicenseManager.h"
#include "ProductManager.h"
#include "ResourceManager.h"
#include "Dialogs/Omniverse/OmniverseLoginDialog.h"
#include "Dialogs/Omniverse/OmniverseFileViewDialog.h"
#include "Dialogs/Omniverse/OmniversePresetDialog.h"
#include "Dialogs/Omniverse/OmniverseLiveSessionDialog.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/Usd/mipUsdPresetManager.h"
#include "UI/Omniverse/OmniverseMeshListWidget.h"
#include "UI/Omniverse/OmniversePropertyWidget.h"
#include "UI/Omniverse/OmniverseStageListWidget.h"

//#define OMNIVERSE_PROPERTY
//#define OMNIVERSE_STAGE
//#define OMNIVERSE_LIVE_SESSION_LOG
//#define OMNIVERSE_LIVE_SESSION_MERGE

OmniverseTab::OmniverseTab(DataContext* pContext, QWidget* parent /*= NULL*/) :
	CollapseWidget(QString(), parent),
	m_pDataContext(pContext),
	m_omniverseStageListWidget(nullptr),
	m_btnEndAndMergeLiveSession(nullptr),
	m_textEditLiveSessionLog(nullptr)
{
	int nRow = 0;
	m_pOmniverse = m_pDataContext->GetOmniverseContext();
	m_pOmniStage = m_pOmniverse->GetStage();
	m_pOmniConnector = m_pOmniverse->GetConnector();

	m_meshListWidget = new OmniverseMeshListWidget(this);
	m_meshListWidget->Init(m_pOmniverse);


	m_labelServer = new QLabel("Server : ", this);
	m_editServer = new QLineEdit("", this);

	m_labelUserName = new QLabel("User Name : ", this);
	m_editUserName = new QLineEdit("", this);

	m_labelCurrentFile = new QLabel("Working : ", this);
	m_editCurrentFile = new QLineEdit("", this);

	m_btnLogin = new QPushButton("Login", this);
	m_btnLogOut = new QPushButton("Log out", this);
	m_btnCreate = new QPushButton("Create", this);
	m_btnOpen = new QPushButton("Open", this);
	m_btnClose = new QPushButton("Close", this);

	m_btnLiveSessionMode = new QPushButton("Live", this);
	m_btnLeaveLiveSession = new QPushButton("Leave Session", this);
	m_labelCurrentLiveSession = new QLabel("Session : ");
	m_editCurrentLiveSession = new QLineEdit(this);

	m_cboPresetMaterials = new QComboBox(this);
	m_btnPresetEdit = new QPushButton("Edit", this);

	m_labelServer->setStyleSheet(STYLE_MANAGER->labelNormal);
	m_editServer->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	m_editServer->setReadOnly(true);

	m_labelUserName->setStyleSheet(STYLE_MANAGER->labelNormal);
	m_editUserName->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	m_editUserName->setReadOnly(true);

	m_labelCurrentFile->setStyleSheet(STYLE_MANAGER->labelNormal);
	m_editCurrentFile->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	m_editCurrentFile->setReadOnly(true);

	m_btnLogin->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnLogOut->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnCreate->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnOpen->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnClose->setStyleSheet(STYLE_MANAGER->buttonBehind);

	m_btnLiveSessionMode->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnLeaveLiveSession->setStyleSheet(STYLE_MANAGER->buttonBehind);

	m_labelCurrentLiveSession->setStyleSheet(STYLE_MANAGER->labelNormal);
	m_editCurrentLiveSession->setStyleSheet(STYLE_MANAGER->editBoxNormal);
	m_editCurrentLiveSession->setReadOnly(true);

	m_cboPresetMaterials->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
	//m_cboPresetMaterials->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum));
	m_btnPresetEdit->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnPresetEdit->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum));

	m_meshListWidget->setVisible(false);

	addWidget(m_labelServer, nRow);
	addWidget(m_editServer, nRow);
	addWidget(m_labelUserName, nRow);
	addWidget(m_editUserName, nRow++);

	addWidget(m_labelCurrentFile, nRow);
	addWidget(m_editCurrentFile, nRow++);

	addWidget(m_btnLogin, nRow);
	addWidget(m_btnLogOut, nRow);
	addWidget(m_btnCreate, nRow);
	addWidget(m_btnOpen, nRow);
	addWidget(m_btnClose, nRow++);

	addWidget(m_btnLiveSessionMode, nRow);
	addWidget(m_btnLeaveLiveSession, nRow);

#ifdef OMNIVERSE_LIVE_SESSION_MERGE
	m_btnEndAndMergeLiveSession = new QPushButton("End And Merge", this);
	m_btnEndAndMergeLiveSession->setStyleSheet(STYLE_MANAGER->buttonBehind);
	addWidget(m_btnEndAndMergeLiveSession, nRow);
	connect(m_btnEndAndMergeLiveSession, &QPushButton::clicked, this, &OmniverseTab::slot_EndAndMergeLiveSession);
#endif

	addWidget(m_labelCurrentLiveSession, nRow);
	addWidget(m_editCurrentLiveSession, nRow++);

#ifdef OMNIVERSE_LIVE_SESSION_LOG
	m_textEditLiveSessionLog = new QTextEdit(this);
	addWidget(m_textEditLiveSessionLog, nRow++);
#endif

	createFrameLine(nRow++);

	addWidget(new QLabel("Preset"), nRow++);
	addWidget(m_cboPresetMaterials, nRow);
	addWidget(m_btnPresetEdit, nRow++);

#ifdef OMNIVERSE_STAGE
	createFrameLine(nRow++);

	QLabel* labelOmnverseStage = new QLabel("Stage", this);
	addWidget(labelOmnverseStage, nRow++);

	m_omniverseStageListWidget = new OmniverseStageListWidget(this);
	m_omniverseStageListWidget->Init(m_pDataContext, m_pOmniverse->GetUsdStage());
	addWidget(m_omniverseStageListWidget, nRow++);
#endif

#ifdef OMNIVERSE_PROPERTY
	m_propertyWidget = new OmniversePropertyWidget(this);
	m_propertyWidget->Init(m_pDataContext);

	createFrameLine(nRow++);

	addWidget(new QLabel("Property"), nRow++);
	addWidget(m_propertyWidget, nRow++);
#endif

	QWidget* emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	addWidget(emptyBox, nRow++, 0);

	QString title = "Omniverse";
	setTitle(title);

	setOpenWidget(false);

	connect(m_btnLogin, &QPushButton::clicked, this, &OmniverseTab::slot_Login);
	connect(m_btnLogOut, &QPushButton::clicked, this, &OmniverseTab::slot_LogOut);
	connect(m_btnCreate, &QPushButton::clicked, this, &OmniverseTab::slot_Create);
	connect(m_btnOpen, &QPushButton::clicked, this, &OmniverseTab::slot_Open);
	connect(m_btnClose, &QPushButton::clicked, this, &OmniverseTab::slot_Close);
	connect(m_btnLiveSessionMode, &QPushButton::clicked, this, &OmniverseTab::slot_LiveSessionMode);
	connect(m_btnLeaveLiveSession, &QPushButton::clicked, this, &OmniverseTab::slot_LeaveLiveSession);

	connect(m_btnPresetEdit, &QPushButton::clicked, this, &OmniverseTab::slot_PresetEdit);

	connect(m_cboPresetMaterials, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_PresetMaterialChanged(int)));
	connect(m_pOmniStage, &OmniverseStage::sig_recvLiveSessionMessage, this, &OmniverseTab::slot_HandleChannelMessage);
	connect(m_pOmniStage, &OmniverseStage::sig_StageDataChanged, this, &OmniverseTab::slot_StageChanged);
	connect(m_pOmniStage, &OmniverseStage::sig_StagePresetChanged, this, &OmniverseTab::slot_StagePresetChanged);

	updateControl();
	updatePresetMaterials();
}

void OmniverseTab::updateControl()
{
	if (m_pOmniverse->IsConnect())
	{
		m_editServer->setText(QString::fromStdString(m_pOmniConnector->IPAddress()));
		m_editUserName->setText(QString::fromStdString(m_pOmniConnector->GetUserName()));
	}
	else
	{
		m_editServer->setText("");
		m_editUserName->setText("");
	}

	m_btnLiveSessionMode->setEnabled((m_pOmniStage->IsLiveSessionMode() == false) && m_pOmniverse->IsConnect() && m_pOmniStage->IsOpen());

	m_btnLeaveLiveSession->setEnabled(m_pOmniStage->IsLiveSessionMode() && m_pOmniverse->IsConnect() && m_pOmniStage->IsOpen());
	if (m_btnEndAndMergeLiveSession)
	{
		m_btnEndAndMergeLiveSession->setEnabled(m_pOmniStage->IsLiveSessionMode() && m_pOmniverse->IsConnect() && m_pOmniStage->IsOpen());
	}
	m_editCurrentLiveSession->setText(m_pOmniStage->GetLiveSessionName().c_str());

	m_btnLogOut->setEnabled(m_pOmniverse->IsConnect());
	m_btnOpen->setEnabled(m_pOmniverse->IsConnect() && (m_pOmniStage->IsOpen() == false));
	m_btnCreate->setEnabled(m_pOmniverse->IsConnect() && (m_pOmniStage->IsOpen() == false));
	m_btnClose->setEnabled(m_pOmniverse->IsConnect() && m_pOmniStage->IsOpen());
	m_cboPresetMaterials->setEnabled(m_pOmniverse->IsConnect());
	m_btnPresetEdit->setEnabled(m_pOmniverse->IsConnect());

	QString url = QString::fromStdString(m_pOmniStage->GetStageUrl());
	m_editCurrentFile->setText(url);
}

void OmniverseTab::updatePresetMaterials()
{
	m_cboPresetMaterials->blockSignals(true);
	m_cboPresetMaterials->clear();
	m_cboPresetMaterials->addItem("Default");

	std::vector<mipUsdPresetPtr> presetList = m_pDataContext->GetOmniversePresetManager()->GetPresetList();
	for (auto pPreset : presetList)
	{
		std::string name = pPreset->GetPresetName();
		QVariant data = qVariantFromValue<void*>(pPreset.get());
		m_cboPresetMaterials->addItem(name.c_str(), data);
	}
	m_cboPresetMaterials->setCurrentIndex(0);
	m_cboPresetMaterials->blockSignals(false);
}

void OmniverseTab::slot_Login()
{
	OmniverseLoginDialog dlg(m_pOmniverse, this);
	int result = dlg.exec();
	if (result == QDialog::Accepted)
	{
		updateControl();
	}
}

void OmniverseTab::slot_LogOut()
{
	QString message = "are you sure you want to log out?";
	int result = QMessageBox::warning(this, "warning", message, QMessageBox::Ok | QMessageBox::Cancel);
	if (result == QMessageBox::Ok)
	{
		m_pOmniverse->Disconnect();
		updateControl();
	}
}

void OmniverseTab::slot_Create()
{
	OmniverseFileViewDialog dlg(m_pOmniverse, OmniverseFileViewDialog::Create, this);
	int result = dlg.exec();
	if (result == QDialog::Accepted)
	{
		std::string filePath = dlg.GetFilePath();

		if (m_pOmniverse->CreateOmniverseStage(filePath) == false)
		{
			qWarning() << "fail to create omniverse model";
			return;
		}
		QString url = QString::fromStdString(m_pOmniStage->GetStageUrl());
		qInfo() << "create url finished : " << url;

		m_pOmniStage->AddMeshList(m_pDataContext->m_MeshData);
		m_pOmniStage->UpdateStage();
	}
}

void OmniverseTab::slot_Open()
{
	OmniverseFileViewDialog dlg(m_pOmniverse, OmniverseFileViewDialog::Open, this);
	int result = dlg.exec();
	if (result == QDialog::Accepted)
	{
		std::string filePath = dlg.GetFilePath();
		if (m_pOmniverse->OpenOmniverseStage(filePath) == false)
		{
			qWarning() << "fail to open omniverse model";
			return;
		}
		QString url = QString::fromStdString(m_pOmniStage->GetStageUrl());
		qInfo() << "open url finished : " << url;

		m_pOmniStage->UpdateStage();
	}
}

void OmniverseTab::slot_Close()
{
	QString message = "are you sure you want to close?";
	int result = QMessageBox::warning(this, "warning", message, QMessageBox::Ok | QMessageBox::Cancel);

	if (result == QMessageBox::Ok)
	{
		if (m_pOmniStage->IsOpen())
		{
			m_pOmniStage->Close();
		}
	}
}

void OmniverseTab::slot_LiveSessionMode()
{
	if (m_pOmniverse->IsConnect() == false)
	{
		return;
	}

	OmniverseLiveSessionDialog dlg(m_pOmniverse, this);
	dlg.exec();

	updateControl();
}

void OmniverseTab::slot_LeaveLiveSession()
{
	OmniverseLiveChannel* pLiveChannel = m_pOmniStage->GetLiveChannel();
	if (pLiveChannel == nullptr)
	{
		return;
	}

	QString message = QString("you are about to leave '%1' session").arg(pLiveChannel->GetSessionName().c_str());
	int result = QMessageBox::warning(this, "warning", message, QMessageBox::Ok | QMessageBox::Cancel);
	if (result == QMessageBox::Ok)
	{
		m_pOmniStage->LeaveLiveSession();
		updateControl();
	}
}

void OmniverseTab::slot_EndAndMergeLiveSession()
{
	updateControl();
}

void OmniverseTab::slot_PresetEdit()
{
	OmniversePresetDialog dlg(this);
	dlg.Init(m_pDataContext);
	dlg.exec();

	updatePresetMaterials();
}

void OmniverseTab::slot_PresetMaterialChanged(int index)
{
	if (index >= 0)
	{
		QVariant data = m_cboPresetMaterials->currentData();
		if (data.isNull())
		{
			m_pOmniStage->ClearPreset();
		}
		else
		{
			mipUsdPreset* pPreset = (mipUsdPreset*)data.value<void*>();
			if (pPreset)
			{
				m_pOmniStage->UpdatePreset(*pPreset);
			}
		}
	}
}

void OmniverseTab::slot_HandleChannelMessage(OmniverseLiveChannelMessagePtr pMessage, std::set<OmniversePeerUser> peerUserList)
{
	if (m_textEditLiveSessionLog)
	{
		QString message = QString("type : %1, from  : %2 - %3").arg(pMessage->GetType_Text().c_str()).arg(pMessage->GetUserName().c_str()).arg(pMessage->GetAppName().c_str());
		QString logMessage = m_textEditLiveSessionLog->document()->toPlainText() + "\n" + message;
		m_textEditLiveSessionLog->setText(logMessage);
	}
}

void OmniverseTab::slot_StageChanged()
{
	updateControl();

	if (m_omniverseStageListWidget)
	{
		m_omniverseStageListWidget->Update();
	}
}

void OmniverseTab::slot_StagePresetChanged()
{
	//
}

