#include "stdafx.h"
#include "OmniverseLiveSessionDialog.h"
#include "System/StyleManager.h"
#include "ui_OmniverseLiveSessionDialog.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/LiveSession/OmniverseLiveSession.h"

OmniverseLiveSessionDialog::OmniverseLiveSessionDialog(OmniverseContext* pOmniverse, QWidget* parent) :
	m_pOmniverse(pOmniverse)
{
	m_ui = new Ui::OmniverseLiveSessionDialog();
	m_ui->setupUi(this);

	m_pOmniStage = pOmniverse->GetStage();

	setStyleSheet(STYLE_MANAGER->mainFrame);

	m_ui->m_rdoMode_Join->setStyleSheet(STYLE_MANAGER->m_Radiobtn);
	m_ui->m_rdoMode_Create->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	m_ui->m_btnJoinLiveSession->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_ui->m_btnJoinCancel->setStyleSheet(STYLE_MANAGER->buttonBehind);

	m_ui->m_cboJoinSessionList->setStyleSheet(STYLE_MANAGER->comboBoxNormal);

	m_ui->m_btnCreateKeepLocalSource->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_ui->m_btnCreateFetchFromOmniverse->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_ui->m_btnCreateCancel->setStyleSheet(STYLE_MANAGER->buttonBehind);

	//m_ui->m_listJoinLiveSessionUserList->setStyleSheet(STYLE_MANAGER->ListWidget);

	connect(m_ui->m_rdoMode_Join, &QRadioButton::clicked, this, &OmniverseLiveSessionDialog::slot_modeChanged);
	connect(m_ui->m_rdoMode_Create, &QRadioButton::clicked, this, &OmniverseLiveSessionDialog::slot_modeChanged);

	connect(m_ui->m_btnJoinLiveSession, &QPushButton::clicked, this, &OmniverseLiveSessionDialog::slot_JoinLiveSession);
	connect(m_ui->m_btnJoinCancel, &QPushButton::clicked, this, &OmniverseLiveSessionDialog::slot_Cancel);

	connect(m_ui->m_cboJoinSessionList, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_SessionChanged(int)));

	connect(m_ui->m_btnCreateKeepLocalSource, &QPushButton::clicked, this, &OmniverseLiveSessionDialog::slot_CreateNewSession_KeepLocalSource);
	connect(m_ui->m_btnCreateFetchFromOmniverse, &QPushButton::clicked, this, &OmniverseLiveSessionDialog::slot_CreateNewSession_FetchFromOmniverse);
	connect(m_ui->m_btnCreateCancel, &QPushButton::clicked, this, &OmniverseLiveSessionDialog::slot_Cancel);

	connect(m_pOmniStage, &OmniverseStage::sig_recvLiveSessionMessage, this, &OmniverseLiveSessionDialog::slot_HandleChannelMessage);

	/* TODO : 구현 완료될 때까지 hide */
#if 1
	m_ui->m_btnCreateFetchFromOmniverse->setVisible(false);
#endif

	updateControls();
}

OmniverseLiveSessionDialog::~OmniverseLiveSessionDialog()
{
	delete m_ui;
}

void OmniverseLiveSessionDialog::updateControls()
{
	OmniverseLiveSession* pSession = m_pOmniStage->GetLiveSession();

	if (isJoinSessionMode())
	{
		m_ui->m_containerMode->setCurrentIndex(eContainerIndex::Join);

		m_ui->m_cboJoinSessionList->clear();
		for (const std::string& sessionName : pSession->GetSessionList())
		{
			m_ui->m_cboJoinSessionList->addItem(sessionName.c_str());
		}

		updateSessionUserList();
	}
	else if (isCreateNewSessionMode())
	{
		m_ui->m_containerMode->setCurrentIndex(eContainerIndex::CreateNew);
	}
}

bool OmniverseLiveSessionDialog::isJoinSessionMode() const
{
	return m_ui->m_rdoMode_Join->isChecked();
}

bool OmniverseLiveSessionDialog::isCreateNewSessionMode() const
{
	return m_ui->m_rdoMode_Create->isChecked();
}

void OmniverseLiveSessionDialog::updateSessionUserList()
{
	m_ui->m_listJoinLiveSessionUserList->clear();
	for (auto& user : m_peerUserList)
	{
		QString userText = QString("%1 - %2(ID=%3)")
			.arg(user.GetUserName().c_str())
			.arg(user.GetApp().c_str())
			.arg(user.GetUserID().c_str());

		m_ui->m_listJoinLiveSessionUserList->addItem(userText);
	}
}

void OmniverseLiveSessionDialog::slot_modeChanged()
{
	if (isJoinSessionMode())
	{
		//
	}
	else if (isCreateNewSessionMode())
	{
		m_pOmniStage->CloseLiveChannel();
	}

	updateControls();
}

void OmniverseLiveSessionDialog::slot_JoinLiveSession()
{
	QString sessionName = m_ui->m_cboJoinSessionList->currentText();
	if (sessionName.isEmpty())
	{
		return;
	}

	if (m_pOmniStage->JoinLiveSession(sessionName.toStdString()) == false)
	{
		return;
	}

	accept();
}

void OmniverseLiveSessionDialog::slot_CreateNewSession_KeepLocalSource()
{
	QString sessionName = m_ui->m_editCreateSessionName->text();
	if (sessionName.isEmpty())
	{
		return;
	}

	if (m_pOmniStage->CreateLiveSession(sessionName.toStdString()) == false)
	{
		return;
	}

	accept();
}

void OmniverseLiveSessionDialog::slot_CreateNewSession_FetchFromOmniverse()
{
	/* TODO : Keep Local, Fetch from Omniverse 구분 */
	QString sessionName = m_ui->m_editCreateSessionName->text();
	if (sessionName.isEmpty())
	{
		return;
	}

	if (m_pOmniStage->CreateLiveSession(sessionName.toStdString()) == false)
	{
		return;
	}

	accept();
}

void OmniverseLiveSessionDialog::slot_Cancel()
{
	reject();
}

void OmniverseLiveSessionDialog::slot_SessionChanged(int index)
{
	QString sessionName = m_ui->m_cboJoinSessionList->currentText();
	if (sessionName.isEmpty())
	{
		return;
	}
	
	m_peerUserList.clear();
	updateSessionUserList();

	m_pOmniStage->OpenLiveChannel(sessionName.toStdString());
}

void OmniverseLiveSessionDialog::slot_HandleChannelMessage(OmniverseLiveChannelMessagePtr pMessage, std::set<OmniversePeerUser> peerUserList)
{
	m_peerUserList = peerUserList;

	updateSessionUserList();
}

