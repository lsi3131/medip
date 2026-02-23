#pragma once

#include <qdialog>
#include "FileManager/config/PACSConfig.h"
#include "FileManager/appcore/Dialog/PACS/PACSSetupStoreModel.h"
#include "FileManager/appcore/Dialog/PACS/PACSSetupInputInterface.h"
#include "filemanager/export.h"
#include "ui_PACSSetupStoreWidget.h"

class QTableWidget;
class QTableWidgetItem;
class QLineEdit;
class QPushButton;
class QWidget;

namespace fm
{
	class AppCoreContext;
	class PACSConfig;

	class FM_CORE_EXPORT PACSSetupStoreWidget : public QWidget, public Ui::PACSSetupStoreWidget
	{
		Q_OBJECT

	public:
		enum EColumn
		{
			COL_HOST_NAME = 0,
			COL_IP,
			COL_PORT,
			COL_AETITLE,
		};

	public:
		PACSSetupStoreWidget(QWidget* parent = nullptr);
		virtual ~PACSSetupStoreWidget();

	public:
		void Init(AppCoreContext* pContext);
		bool Save();

	private:
		void InitLayout();
		void SetupDataModelByConfig();
		void UpdateByModel();
		void UpdateControl_As_CurStatus();

	public slots:
		void onEcho();
		void onAdd();
		void onEdit();
		void onDelete();

		void onPACSListDoubleClicked();
		void onPACSListItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

	protected:
		AppCoreContext* m_pContext;
		PACSSetupStoreModel m_dataModel;
		PACSConfig* m_pConfig;

		PACSSetupInputInterface* m_pInputInterface;
		DefaultPACSSetupInputInterface* m_defaultInterface;
	};
}

