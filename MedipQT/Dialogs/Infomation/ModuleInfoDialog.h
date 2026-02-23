#pragma once

#include <QDialog>
#include "ui_ModuleInfoDialog.h"

class ModuleInfoDialog : public QDialog, public Ui::ModuleInfoDialog
{
public:
	ModuleInfoDialog(QWidget* parent);

private:
	void updateInternalModuleList();
	void updateAIWeightList();
};
