#pragma once

#include <qdialog>
#include "FileManager/config/PACSConfig.h"
#include "filemanager/export.h"
#include "ui_SingleInputDialog.h"

namespace fm
{
	class AppCoreContext;
	class PACSSetupQueryRetrieveWidget;
	class PACSSetupStoreWidget;

	class FM_CORE_EXPORT SingleInputDialog : public QDialog, public Ui::SingleInputDialog
	{
		Q_OBJECT

	public:
		SingleInputDialog(QWidget* parent = nullptr);
		virtual ~SingleInputDialog();

	public:
		QString GetData();

	public slots:
		void onReturnPressed();
		void onOk();
		void onCancel();

	private:
		QString m_data;
	};

}

