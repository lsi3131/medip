#pragma once

#include <qdialog>
#include "FileManager/config/PACSConfig.h"
#include "filemanager/export.h"
#include "ui_SettingDialog.h"

class QTableWidget;
class QTableWidgetItem;
class QLineEdit;
class QPushButton;
class QWidget;
class QTabWidget;

namespace fm
{
	class AppCoreContext;
	class PACSSetupQueryRetrieveWidget;
	class PACSSetupStoreWidget;

	class FM_CORE_EXPORT SettingDialog : public QDialog, public Ui::SettingDialog
	{
		Q_OBJECT

	public:
		SettingDialog(AppCoreContext* pContext, QWidget* parent = nullptr);
		virtual ~SettingDialog();

	public slots:
		void onSave();
		void onCancel();
	};

}

