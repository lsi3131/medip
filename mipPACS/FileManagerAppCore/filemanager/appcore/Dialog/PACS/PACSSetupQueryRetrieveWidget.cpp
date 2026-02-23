#include "stdafx.h"
#include "PACSSetupQueryRetrieveWidget.h"
#include "PACSRegisterDialog.h"
#include "filemanager/appcore/AppCoreContext.h"
#include <qmessagebox>
#include <qtablewidget>
#include <qpushbutton>
#include <QLineEdit>
#include <qfiledialog>

namespace fm
{

	//=====================================================
	//			PACSSetupQueryRetrieveWidget
	//=====================================================
	PACSSetupQueryRetrieveWidget::PACSSetupQueryRetrieveWidget(QWidget* parent) :
		QWidget(parent),
		m_pContext(nullptr),
		m_pConfig(nullptr)
	{
		setupUi(this);

		InitLayout();

		connect(m_btnEcho, &QPushButton::clicked, this, &PACSSetupQueryRetrieveWidget::onEcho);
		connect(m_btnAdd, &QPushButton::clicked, this, &PACSSetupQueryRetrieveWidget::onAdd);
		connect(m_btnEdit, &QPushButton::clicked, this, &PACSSetupQueryRetrieveWidget::onEdit);
		connect(m_btnDelete, &QPushButton::clicked, this, &PACSSetupQueryRetrieveWidget::onDelete);
		connect(m_btnOpenRepositoryPath, &QPushButton::clicked, this, &PACSSetupQueryRetrieveWidget::onOpenRepositoryPath);

		connect(m_tablePACSList, &QTableWidget::doubleClicked,
			this, &PACSSetupQueryRetrieveWidget::onPACSListDoubleClicked);

		connect(m_tablePACSList, &QTableWidget::currentItemChanged,
			this, &PACSSetupQueryRetrieveWidget::onPACSListItemChanged);

		/* TODO : Echo 진행 시 상태 확인을 GUI로 보여주도록 구현할 것 */
		m_btnEcho->hide();
	}

	PACSSetupQueryRetrieveWidget::~PACSSetupQueryRetrieveWidget()
	{
		if (m_pDefaultInterface)
		{
			delete m_pDefaultInterface;
			m_pDefaultInterface = nullptr;
		}
	}

	void PACSSetupQueryRetrieveWidget::Init(AppCoreContext* pContext)
	{
		m_pContext = pContext;
		m_pConfig = m_pContext->GetConfigManager()->GetPACSConfig();

		m_pDefaultInterface = new DefaultPACSSetupInputInterface(m_pConfig, this);
		m_pInputInterface = m_pDefaultInterface;

		PACSConfig::Listener listener = m_pConfig->GetListener();
		m_editListenerPort->setText(QString::number(listener.Port));
		m_editListenerAETitle->setText(QString::fromStdWString(listener.ApplicationEntity));

		m_editRepositoryPath->setText(QString::fromStdWString(m_pConfig->GetRepositoryDirectoryPath()));

		SetupDataModelByConfig();
	}

	bool PACSSetupQueryRetrieveWidget::Save()
	{
		PACSConfig::Listener listener;
		listener.Port = m_editListenerPort->text().toInt();
		listener.ApplicationEntity = m_editListenerAETitle->text().toStdWString();

		m_pConfig->ModifyListener(listener);

		std::wstring repositoryDirectoryPath = m_editRepositoryPath->text().toStdWString();
		m_pConfig->ModifyRepositoryDirectoryPath(repositoryDirectoryPath);

		/* SCP Network 정보 초기화 */
		DicomListenerInfo listenerInfo(
			listener.ApplicationEntity,
			listener.Port,
			m_pConfig->GetTimeout()
		);

		m_pContext->GetDicomNetworkManager()->SetListenerInfo(listenerInfo);

		for (auto& host : m_dataModel.GetAddedHostList())
		{
			m_pConfig->AddHost(host);
		}

		for (auto& host : m_dataModel.GetModifiedHostList())
		{
			m_pConfig->ModifyHost(host);
		}

		for (auto& host : m_dataModel.GetDeletedHostList())
		{
			m_pConfig->DeleteHost(host.Name);
		}

		/* Data Model 초기화 */
		SetupDataModelByConfig();

		return true;
	}

	void PACSSetupQueryRetrieveWidget::SetInputInterface(PACSSetupInputInterface* pInputInterface)
	{
		m_pInputInterface = pInputInterface;
	}

	void PACSSetupQueryRetrieveWidget::InitLayout()
	{
		//m_mainVertLayout = new QVBoxLayout(this);
		//InitTopWidget();
		//InitCenterWidget();
		//InitBottomWidget();

		//m_mainVertLayout->addWidget(m_topLayoutWidget);
		//m_mainVertLayout->addWidget(m_centerLayoutWidget);
		//m_mainVertLayout->addWidget(m_bottomLayoutWidget);

		//=============================================
		setWindowTitle(QString::fromLocal8Bit("PACS Setting"));

		QStringList tableColumnHeader = {
			QString::fromLocal8Bit("Name"),
			QString::fromLocal8Bit("IP Address"),
			QString::fromLocal8Bit("Port"),
			QString::fromLocal8Bit("Application Entity"),
			QString::fromLocal8Bit("Protocol"),
		};

		m_tablePACSList->setColumnCount(tableColumnHeader.size());
		m_tablePACSList->setHorizontalHeaderLabels(tableColumnHeader);
		m_tablePACSList->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_tablePACSList->setAlternatingRowColors(true);
		m_tablePACSList->setSelectionMode(QAbstractItemView::SingleSelection);
		m_tablePACSList->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_tablePACSList->horizontalHeader()->setHighlightSections(false);
	}

	void PACSSetupQueryRetrieveWidget::SetupDataModelByConfig()
	{
		m_dataModel.Init(m_pConfig->GetQueryRetrieveHosts());
		UpdateByModel();
	}

	void PACSSetupQueryRetrieveWidget::UpdateByModel()
	{
		std::vector<PACSConfig::Host> hosts = m_dataModel.GetHostList();

		int newRowCount = hosts.size();
		m_tablePACSList->setRowCount(newRowCount);
		for (int i = 0; i < hosts.size(); ++i)
		{
			m_tablePACSList->setItem(i, COL_HOST_NAME, new QTableWidgetItem(QString::fromStdWString(hosts[i].Name)));
			m_tablePACSList->setItem(i, COL_IP, new QTableWidgetItem(hosts[i].IP.c_str()));
			m_tablePACSList->setItem(i, COL_PORT, new QTableWidgetItem(QString::number(hosts[i].Port)));
			m_tablePACSList->setItem(i, COL_AETITLE, new QTableWidgetItem(QString::fromStdWString(hosts[i].ApplicationEntity)));
			m_tablePACSList->setItem(i, COL_PROTOCOL, new QTableWidgetItem(EDcmProtocol_Text(hosts[i].Protocol).c_str()));
		}

		m_tablePACSList->resizeColumnsToContents();
		UpdateControl_As_CurStatus();
	}

	void PACSSetupQueryRetrieveWidget::UpdateControl_As_CurStatus()
	{
		if (m_tablePACSList->currentRow() < 0)
		{
			m_btnEdit->setEnabled(false);
			m_btnDelete->setEnabled(false);
		}
		else
		{
			m_btnEdit->setEnabled(true);
			m_btnDelete->setEnabled(true);
		}
	}

	void PACSSetupQueryRetrieveWidget::onEcho()
	{
		/* TODO : echo 기능을 통해 접속된 PACS가 동작되는지 확인 */
		//m_pContext->GetDicomNetworkManager()->Echo_Async();
	}

	void PACSSetupQueryRetrieveWidget::onAdd()
	{
		PACSConfig::Host host;
		if (m_pInputInterface->GetNewHost(&host))
		{
			m_dataModel.AddHost(host);
			UpdateByModel();
		}
	}

	void PACSSetupQueryRetrieveWidget::onEdit()
	{
		int row = m_tablePACSList->currentRow();
		if (row >= 0)
		{
			std::wstring hostName = m_tablePACSList->item(row, COL_HOST_NAME)->text().toStdWString();
			PACSConfig::Host originHost;
			if (m_dataModel.FindHostByName(&originHost, hostName))
			{
				PACSConfig::Host targetHost;
				if (m_pInputInterface->GetEdittedHost(&targetHost, originHost))
				{
					m_dataModel.EditHostByName(targetHost);
					UpdateByModel();
				}
			}
		}
	}

	void PACSSetupQueryRetrieveWidget::onDelete()
	{
		int row = m_tablePACSList->currentRow();
		if (row >= 0)
		{
			if (m_pInputInterface->CheckDeleteHost())
			{
				std::wstring hostName = m_tablePACSList->item(row, COL_HOST_NAME)->text().toStdWString();
				m_dataModel.Delete(hostName);
				UpdateByModel();
			}
		}
	}

	void PACSSetupQueryRetrieveWidget::onOpenRepositoryPath()
	{
		QString dirpath = QFileDialog::getExistingDirectory(this);
		if (!dirpath.isEmpty())
		{
			m_editRepositoryPath->setText(dirpath);
		}
	}

	void PACSSetupQueryRetrieveWidget::onPACSListDoubleClicked()
	{
		onEdit();
	}

	void PACSSetupQueryRetrieveWidget::onPACSListItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
	{
		UpdateControl_As_CurStatus();
	}

	//void PACSSetupQueryRetrieveWidget::InitTopWidget()
	//{
	//	/* Top Widget*/
	//	m_topLayoutWidget = new QWidget(this);
	//	QHBoxLayout* topWidgetLayout = new QHBoxLayout(m_topLayoutWidget);

	//	/* > Listener Widget */
	//	QWidget* listenerPortWidget = new QWidget(m_topLayoutWidget);
	//	listenerPortWidget->setObjectName(QStringLiteral("widget_2"));
	//	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	//	sizePolicy.setHorizontalStretch(2);
	//	sizePolicy.setVerticalStretch(0);
	//	sizePolicy.setHeightForWidth(listenerPortWidget->sizePolicy().hasHeightForWidth());
	//	listenerPortWidget->setSizePolicy(sizePolicy);
	//	QHBoxLayout* listenPortLayout = new QHBoxLayout(listenerPortWidget);

	//	QLabel* label = new QLabel(listenerPortWidget);
	//	QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
	//	sizePolicy1.setHorizontalStretch(0);
	//	sizePolicy1.setVerticalStretch(0);
	//	sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
	//	label->setSizePolicy(sizePolicy1);
	//	label->setText("Port : ");

	//	listenPortLayout->addWidget(label);

	//	m_editListenerPort = new QLineEdit(listenerPortWidget);
	//	QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
	//	sizePolicy2.setHorizontalStretch(0);
	//	sizePolicy2.setVerticalStretch(0);
	//	sizePolicy2.setHeightForWidth(m_editListenerPort->sizePolicy().hasHeightForWidth());
	//	m_editListenerPort->setSizePolicy(sizePolicy2);

	//	listenPortLayout->addWidget(m_editListenerPort);

	//	QSpacerItem* listernPortSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	//	listenPortLayout->addItem(listernPortSpacer);

	//	topWidgetLayout->addWidget(listenerPortWidget);

	//	/* > Application Entity Title */
	//	QWidget* appEntityTitleWidget = new QWidget();
	//	QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
	//	sizePolicy3.setHorizontalStretch(4);
	//	sizePolicy3.setVerticalStretch(0);
	//	sizePolicy3.setHeightForWidth(appEntityTitleWidget->sizePolicy().hasHeightForWidth());
	//	appEntityTitleWidget->setSizePolicy(sizePolicy3);

	//	QHBoxLayout* appEntityTitleLayout = new QHBoxLayout(appEntityTitleWidget);

	//	QSpacerItem* appEntityTitleSpacer_1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	//	QSpacerItem* appEntityTitleSpacer_2 = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	//	QLabel* appEntityTitleLabel = new QLabel(appEntityTitleWidget);
	//	appEntityTitleLabel->setText("Application Entity Title : ");

	//	m_editListenerAETitle = new QLineEdit(appEntityTitleWidget);
	//	sizePolicy2.setHeightForWidth(m_editListenerAETitle->sizePolicy().hasHeightForWidth());
	//	m_editListenerAETitle->setSizePolicy(sizePolicy2);

	//	appEntityTitleLayout->addItem(appEntityTitleSpacer_1);
	//	appEntityTitleLayout->addWidget(appEntityTitleLabel);
	//	appEntityTitleLayout->addWidget(m_editListenerAETitle);
	//	appEntityTitleLayout->addItem(appEntityTitleSpacer_2);

	//	topWidgetLayout->addWidget(appEntityTitleWidget);
	//}
	//void PACSSetupQueryRetrieveWidget::InitCenterWidget()
	//{
	//	/* > Center */
	//	m_centerLayoutWidget = new QWidget();
	//	QVBoxLayout* centerVertLayout = new QVBoxLayout(m_centerLayoutWidget);
	//	centerVertLayout->setContentsMargins(6, 0, 6, 0);

	//	/* >> PACS Label */
	//	QWidget* pacsLocationLabelWidget = new QWidget();
	//	QHBoxLayout* pacsLocationLabelLayout = new QHBoxLayout();
	//	pacsLocationLabelWidget->setLayout(pacsLocationLabelLayout);
	//	pacsLocationLabelWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//	pacsLocationLabelLayout->setAlignment(Qt::AlignLeft);
	//	pacsLocationLabelLayout->setContentsMargins(6, 0, 6, 0);

	//	QLabel* labelArrow = new QLabel();
	//	labelArrow->setObjectName(QStringLiteral("m_labelArrow"));

	//	QLabel* pacsLocationLabel = new QLabel();
	//	pacsLocationLabel->setText("PACS Location");

	//	pacsLocationLabelLayout->addWidget(labelArrow);
	//	pacsLocationLabelLayout->addWidget(pacsLocationLabel);

	//	/* >> PACS Table */
	//	m_tablePACSList = new QTableWidget(m_centerLayoutWidget);

	//	centerVertLayout->addWidget(pacsLocationLabelWidget);
	//	centerVertLayout->addWidget(m_tablePACSList);
	//}
	//void PACSSetupQueryRetrieveWidget::InitBottomWidget()
	//{
	//	/* Bottom Button List */
	//	m_bottomLayoutWidget = new QWidget(m_centerLayoutWidget);
	//	QHBoxLayout* bottomButtonListHorzLayout = new QHBoxLayout(m_bottomLayoutWidget);
	//	bottomButtonListHorzLayout->setAlignment(Qt::AlignLeft);

	//	/* > Edit Button List*/
	//	QWidget* editButtonListWidget = new QWidget(m_bottomLayoutWidget);
	//	editButtonListWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//	QHBoxLayout* editButtonListLayout = new QHBoxLayout(editButtonListWidget);

	//	m_btnEcho = new QPushButton();
	//	m_btnEcho->setObjectName(QStringLiteral("m_btnPopUpEcho"));

	//	m_btnAdd = new QPushButton();
	//	m_btnAdd->setObjectName(QStringLiteral("m_btnPopUpAdd"));

	//	m_btnEdit = new QPushButton();
	//	m_btnEdit->setObjectName(QStringLiteral("m_btnPopUpEdit"));

	//	m_btnDelete = new QPushButton();
	//	m_btnDelete->setObjectName(QStringLiteral("m_btnPopUpDelete"));

	//	editButtonListLayout->addWidget(m_btnEcho);
	//	editButtonListLayout->addWidget(m_btnAdd);
	//	editButtonListLayout->addWidget(m_btnEdit);
	//	editButtonListLayout->addWidget(m_btnDelete);

	//	/* > Spacer */
	//	QSpacerItem* bottomButtonListHorzSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	//	/* > SaveCancel Button List */
	//	QWidget* saveCancelButtonWidget = new QWidget(this);
	//	QHBoxLayout* saveCancelButtonHorzLayout = new QHBoxLayout(saveCancelButtonWidget);
	//	saveCancelButtonWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	//	m_btnSave = new QPushButton(saveCancelButtonWidget);
	//	m_btnSave->setObjectName(QStringLiteral("m_btnPopUpSave"));

	//	m_btnCancel = new QPushButton(saveCancelButtonWidget);
	//	m_btnCancel->setObjectName(QStringLiteral("m_btnPopUpCancel"));

	//	saveCancelButtonHorzLayout->addWidget(m_btnSave);
	//	saveCancelButtonHorzLayout->addWidget(m_btnCancel);

	//	bottomButtonListHorzLayout->addWidget(editButtonListWidget);
	//	bottomButtonListHorzLayout->addItem(bottomButtonListHorzSpacer);
	//	bottomButtonListHorzLayout->addWidget(saveCancelButtonWidget);
	//}
}

