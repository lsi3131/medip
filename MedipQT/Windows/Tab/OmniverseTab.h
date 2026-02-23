#pragma once

#include "CollapseWidget.h"
#include "Omniverse/LiveSession/OmniverseLiveChannelMessage.h"
#include "Omniverse/LiveSession/OmniversePeerUser.h"
#include <QPushButton>

class OmniverseContext;
class OmniverseStage;
class OmniverseConnector;
class DataContext;
class OmniverseMeshListWidget;
class OmniversePropertyWidget;
class OmniverseStageListWidget;

class OmniverseTab : public CollapseWidget
{
	Q_OBJECT

public:
	OmniverseTab(DataContext* pContext, QWidget* parent = NULL);

private:
	void updateControl();
	void updatePresetMaterials();

private slots:
	void slot_Login();
	void slot_LogOut();
	void slot_Create();
	void slot_Open();
	void slot_Close();
	void slot_LiveSessionMode();
	void slot_LeaveLiveSession();
	void slot_EndAndMergeLiveSession();

	void slot_PresetEdit();

	void slot_PresetMaterialChanged(int index);
	void slot_HandleChannelMessage(OmniverseLiveChannelMessagePtr pMessage, std::set<OmniversePeerUser> peerUserList);
	void slot_StageChanged();
	void slot_StagePresetChanged();

private:
	DataContext* m_pDataContext;
	OmniverseContext* m_pOmniverse;
	OmniverseStage* m_pOmniStage;
	OmniverseConnector* m_pOmniConnector;
	OmniverseMeshListWidget* m_meshListWidget;
	OmniversePropertyWidget* m_propertyWidget;
	OmniverseStageListWidget* m_omniverseStageListWidget;

	QLabel* m_labelServer;
	QLineEdit* m_editServer;

	QLabel* m_labelUserName;
	QLineEdit* m_editUserName;

	QLabel* m_labelCurrentFile;
	QLineEdit* m_editCurrentFile;

	QPushButton* m_btnLogin;
	QPushButton* m_btnLogOut;
	QPushButton* m_btnCreate;
	QPushButton* m_btnOpen;
	QPushButton* m_btnClose;
	
	QPushButton* m_btnLiveSessionMode;
	QPushButton* m_btnLeaveLiveSession;
	QPushButton* m_btnEndAndMergeLiveSession;
	QCheckBox* m_chkLiveMode;
	QLabel* m_labelCurrentLiveSession;
	QLineEdit* m_editCurrentLiveSession;

	QTextEdit* m_textEditLiveSessionLog;

	QComboBox* m_cboPresetMaterials;
	QPushButton* m_btnPresetEdit;
};

