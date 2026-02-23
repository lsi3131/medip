#pragma once

#include <QDialog>

class OmniverseContext;
class OmniverseConfig;

namespace Ui
{
	class OmniverseLoginDialog;
};

class OmniverseLoginDialog : public QDialog
{
	Q_OBJECT

public:
	OmniverseLoginDialog(OmniverseContext* pOmniverse, QWidget* parent);

private:
	QString currentServerName() const;
	void updateServerList(const QString& selectServer = "");

private slots:
	void slot_ServerChanged(int index);
	void slot_Connect();
	void slot_Edit();
	void slot_Cancel();

private:
	Ui::OmniverseLoginDialog* ui;
	OmniverseContext* m_pOmniverse;
	OmniverseConfig* m_pConfig;
};
