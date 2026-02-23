#pragma once

#include "ui_PACSSetupQueryRetrieveWidget.h"
#include "FileManager/config/PACSConfig.h"
#include "FileManager/appcore/Dialog/PACS/PACSSetupQueryRetrieveModel.h"
#include "FileManager/appcore/Dialog/PACS/PACSSetupInputInterface.h"
#include "filemanager/export.h"

class QTableWidget;
class QTableWidgetItem;
class QLineEdit;
class QPushButton;
class QWidget;
class QVBoxLayout;

namespace fm
{
	class AppCoreContext;

	class FM_CORE_EXPORT PACSSetupQueryRetrieveWidget : public QWidget, public Ui::PACSSetupQueryRetrieveWidget
	{
		Q_OBJECT

	public:
		enum EColumn
		{
			COL_HOST_NAME = 0,
			COL_IP,
			COL_PORT,
			COL_AETITLE,
			COL_PROTOCOL
		};

	public:
		PACSSetupQueryRetrieveWidget(QWidget* parent = nullptr);
		virtual ~PACSSetupQueryRetrieveWidget();

	public:
		void Init(AppCoreContext* pContext);
		bool Save();

		void SetInputInterface(PACSSetupInputInterface* pInputInterface);
		PACSSetupQueryRetrieveModel* GetDataModel()
		{
			return &m_dataModel;
		}

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

		void onOpenRepositoryPath();

		void onPACSListDoubleClicked();
		void onPACSListItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

	private:
		void InitTopWidget();
		void InitCenterWidget();
		void InitBottomWidget();

	protected:
		AppCoreContext* m_pContext;
		PACSSetupQueryRetrieveModel m_dataModel;
		fm::PACSConfig* m_pConfig;

		//QVBoxLayout* m_mainVertLayout;
		//QWidget* m_topLayoutWidget;
		//QWidget* m_centerLayoutWidget;
		//QWidget* m_bottomLayoutWidget;

		//QLineEdit* m_editListenerAETitle;
		//QLineEdit* m_editListenerPort;
		//QTableWidget* m_tablePACSList;
		//QPushButton* m_btnEcho;
		//QPushButton* m_btnAdd;
		//QPushButton* m_btnEdit;
		//QPushButton* m_btnDelete;
		//QPushButton* m_btnSave;
		//QPushButton* m_btnCancel;
		PACSSetupInputInterface* m_pInputInterface;
		DefaultPACSSetupInputInterface* m_pDefaultInterface;
	};

}

