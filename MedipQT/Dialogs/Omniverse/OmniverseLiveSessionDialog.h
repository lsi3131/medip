#pragma once

#include <QDialog>
#include "Omniverse/OmniverseStage.h"

class OmniverseContext;

namespace Ui
{
	class OmniverseLiveSessionDialog;
};

class OmniverseLiveSessionDialog : public QDialog
{
	Q_OBJECT
public:
	enum eContainerIndex
	{
		Join,
		CreateNew
	};

public:
	OmniverseLiveSessionDialog(OmniverseContext* pOmniverse, QWidget* parent);
	virtual ~OmniverseLiveSessionDialog();

private:
	void updateControls();
	void updateSessionUserList();

	bool isJoinSessionMode() const;
	bool isCreateNewSessionMode() const;

private slots:
	void slot_modeChanged();
	void slot_JoinLiveSession();
	void slot_CreateNewSession_KeepLocalSource();
	void slot_CreateNewSession_FetchFromOmniverse();
	void slot_Cancel();

	void slot_SessionChanged(int index);
	void slot_HandleChannelMessage(OmniverseLiveChannelMessagePtr pMessage, std::set<OmniversePeerUser> peerUserList);

private:
	Ui::OmniverseLiveSessionDialog* m_ui;
	OmniverseContext* m_pOmniverse;
	OmniverseStage* m_pOmniStage;
	std::set<OmniversePeerUser> m_peerUserList;
};

