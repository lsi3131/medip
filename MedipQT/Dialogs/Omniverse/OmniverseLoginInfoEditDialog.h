#pragma once

#include <QDialog>

class OmniverseConfig;

namespace Ui
{
	class OmniverseLoginInfoEditDialog;
};

class OmniverseLoginInfoEditDialog : public QDialog
{
	Q_OBJECT

public:
	OmniverseLoginInfoEditDialog(OmniverseConfig* pConfig, QWidget* parent);

private slots:
	void slot_Add();
	void slot_Edit();
	void slot_Delete();
	void slot_Cancel();

private:
	Ui::OmniverseLoginInfoEditDialog* ui;
	OmniverseConfig* m_pConfig;
};
