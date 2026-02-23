#include "stdafx.h"
#include "MeshTab.h"
#include "ProductManager.h"
#include "StyleManager.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "LicenseManager.h"
#include "ResourceManager.h"
#include "ShortcutManager.h"
#include "FileManager.h"
#include "Omniverse/OmniverseContext.h"
#include "UI/Omniverse/OmniverseStageListWidget.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CMeshManipulator.h"

#include "DataContext.h"
#include "Actions/FileWork/Mesh/WorkLoadUsd.h"

MeshTab::MeshTab(ProductManager* pProductManager, QWidget* parent) :
	MeshTab(
		WIN_MANAGER,
		ACTION_MANAGER,
		pProductManager,
		MESH_MODELVIEW_MANAGER,
		MESH_MANIPULATOR,
		SHORTCUT_MANAGER,
		parent)
{
}

MeshTab::MeshTab(
	WindowManager* pWinManager,
	ActionManager* pActionManager,
	ProductManager* pProductManager,
	CMeshModelViewManager* pMeshModelViewManager,
	CMeshManipulator* pMeshManipulator,
	ShortcutManager* pShorcutManager,
	QWidget* parent) :
	m_treeMeshList(nullptr),
	m_treeMeshHeader(nullptr),
	m_pWinManager(pWinManager),
	m_pActionManager(pActionManager),
	m_pProductManager(pProductManager),
	m_pMeshModelViewManager(pMeshModelViewManager),
	m_pMeshManipulator(pMeshManipulator),
	m_pShorcutManager(pShorcutManager),
	m_pDataContext(nullptr),
	CollapseWidget(QString(), parent)
{
	ICON_NEW_LIST = { RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_NEW), RESOURCE_MANAGER->getIcon(ICON_ROILIST_NEW), RESOURCE_MANAGER->getIcon(ICON_ROILIST_NEW_HOVER) };
	ICON_DIFF_LIST = { RESOURCE_MANAGER->getIcon(ICON_NON_DIFF), RESOURCE_MANAGER->getIcon(ICON_DIFF), RESOURCE_MANAGER->getIcon(ICON_DIFF_HOVER) };
	ICON_UNION_LIST = { RESOURCE_MANAGER->getIcon(ICON_NON_UNION), RESOURCE_MANAGER->getIcon(ICON_UNION), RESOURCE_MANAGER->getIcon(ICON_UNION_HOVER) };
	ICON_INTERSECT_LIST = { RESOURCE_MANAGER->getIcon(ICON_NON_INTERSECT), RESOURCE_MANAGER->getIcon(ICON_INTERSECT), RESOURCE_MANAGER->getIcon(ICON_INTERSECT_HOVER) };
	ICON_DELETE_LIST = { RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_DEL), RESOURCE_MANAGER->getIcon(ICON_ROILIST_DEL), RESOURCE_MANAGER->getIcon(ICON_ROILIST_DEL_HOVER) };

	m_pColumnList_Header = MeshListColumnList::New(m_pProductManager, MeshListColumnList::Header);
	m_pColumnList_Widget = MeshListColumnList::New(m_pProductManager, MeshListColumnList::Widget);
}

MeshTab::~MeshTab()
{

}

void MeshTab::Init(DataContext* pDataContext, OmniverseContext* pOmniverse, CPlaneManiplator* pPlaneManipulator)
{
	m_pDataContext = pDataContext;

	int nRow = 0;
	QString strVal;

	m_treeMeshList = new MeshListWidget(
		m_pColumnList_Widget,
		m_pWinManager,
		m_pActionManager,
		m_pProductManager,
		pDataContext,
		m_pMeshModelViewManager,
		pPlaneManipulator,
		m_pMeshManipulator,
		m_pShorcutManager,
		this);
	m_treeMeshList->Init();


	m_treeMeshHeader = new MeshListHeaderWidget(
		m_pColumnList_Header,
		m_pWinManager,
		pDataContext, 
		this);
	m_treeMeshHeader->Init();

	addWidget(m_treeMeshHeader, nRow, 0, QMargins(0, 0, 0, 0), Qt::AlignTop, true);

	getLayout(nRow)->setSpacing(0);
	getLayout(nRow)->setMargin(0);

	addWidget(m_treeMeshList, nRow++, 0, QMargins(0, 0, 0, 0), Qt::AlignTop);

	QMenu* clipMenu = new QMenu(this);

	QAction* actLoad = new QAction("Load File", this);
	clipMenu->addAction(actLoad);

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_GeneralFunction) == eAVAILABLE_STATE::CREATE)
	{
		QMenu* subMenu = new QMenu("Create Meshes...", this);
		clipMenu->addMenu(subMenu);

		QAction* actCreate_Sphere = new QAction("Sphere", this);
		subMenu->addAction(actCreate_Sphere);

		QAction* actCreate_Cube = new QAction("Cube", this);
		subMenu->addAction(actCreate_Cube);

		QAction* actCreate_Cylinder = new QAction("Cylinder", this);
		subMenu->addAction(actCreate_Cylinder);

		connect(actCreate_Sphere, &QAction::triggered, this, &MeshTab::slot_OnSphere);
		connect(actCreate_Cube, &QAction::triggered, this, &MeshTab::slot_OnCube);
		connect(actCreate_Cylinder, &QAction::triggered, this, &MeshTab::slot_OnCylinder);
	}

	m_btnDiff = new QPushButton(this);
	m_btnDiff->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_DIFF));
	m_btnDiff->setFixedSize(QSize(24, 24));
	m_btnDiff->setIconSize(QSize(24, 24));
	m_btnDiff->setToolTip("Boolean Difference");
	m_btnDiff->setCheckable(true);
	m_btnDiff->setChecked(false);
	m_btnDiff->setMouseTracking(true);
	m_btnDiff->installEventFilter(this);

	m_btnUnion = new QPushButton(this);
	m_btnUnion->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_UNION));
	m_btnUnion->setFixedSize(QSize(24, 24));
	m_btnUnion->setIconSize(QSize(24, 24));
	m_btnUnion->setToolTip("Boolean Union");
	m_btnUnion->setCheckable(true);
	m_btnUnion->setChecked(false);
	m_btnUnion->setMouseTracking(true);
	m_btnUnion->installEventFilter(this);


	m_btnIntersect = new QPushButton(this);
	m_btnIntersect->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_INTERSECT));
	m_btnIntersect->setFixedSize(QSize(24, 24));
	m_btnIntersect->setIconSize(QSize(24, 24));
	m_btnIntersect->setToolTip("Boolean Intersection");
	m_btnIntersect->setCheckable(true);
	m_btnIntersect->setChecked(false);
	m_btnIntersect->setMouseTracking(true);
	m_btnIntersect->installEventFilter(this);

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_btnLoad = new QToolButton(this);
	m_btnLoad->setPopupMode(QToolButton::InstantPopup);
	m_btnLoad->setMenu(clipMenu);
	m_btnLoad->setIcon(RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_NEW));
	m_btnLoad->setFixedSize(QSize(34, 34));
	m_btnLoad->setIconSize(QSize(34, 34));
	m_btnLoad->setCheckable(true);
	m_btnLoad->setChecked(false);
	m_btnLoad->setToolTip("Add mesh");
	m_btnLoad->setMouseTracking(true);
	m_btnLoad->installEventFilter(this);

	m_btnDel = new QPushButton(this);
	m_btnDel->setIcon(RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_DEL));
	m_btnDel->setFixedSize(QSize(34, 34));
	m_btnDel->setIconSize(QSize(34, 34));
	m_btnDel->setToolTip("Delete mesh");
	m_btnDel->setCheckable(true);
	m_btnDel->setChecked(false);
	m_btnDel->setMouseTracking(true);
	m_btnDel->installEventFilter(this);

	addWidget(m_btnDiff, nRow, 0, QMargins(), Qt::AlignLeft);
	addWidget(m_btnUnion, nRow, 0, QMargins(), Qt::AlignLeft);
	addWidget(m_btnIntersect, nRow, 0, QMargins(), Qt::AlignLeft);
	addWidget(emptyBox0, nRow);
	addWidget(m_btnLoad, nRow);
	addWidget(m_btnDel, nRow++);

	emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	addWidget(emptyBox0, nRow++, 0);

	m_HoverWidget = nullptr;

	QString title = QString("Mesh List");

	setTitle(title);

	setOpenWidget(false);

	connect(m_treeMeshList, &MeshListWidget::itemClicked, this, &MeshTab::slot_MeshListItemClick);
	connect(m_treeMeshList, &MeshListWidget::itemDoubleClicked, this, &MeshTab::slot_MeshListItemDoubleClick);
	connect(m_treeMeshList, &MeshListWidget::itemChanged, this, &MeshTab::slot_MeshListItemChanged);
	connect(m_treeMeshList, &MeshListWidget::sig_columnStateChanged, this, &MeshTab::slot_MeshListColumnStateChanged);

	connect(m_treeMeshHeader, &MeshListHeaderWidget::itemClicked, this, &MeshTab::slot_MeshHeaderClick);
	connect(m_treeMeshHeader, &MeshListHeaderWidget::sig_columnStateChanged, this, &MeshTab::slot_MeshHeaderColumnStateChanged);
	connect(m_treeMeshHeader, &MeshListHeaderWidget::sig_expandChanged, this, &MeshTab::slot_MeshHeaderExpanedChanged);

	connect(actLoad, &QAction::triggered, this, &MeshTab::slot_LoadMesh);

	connect(m_btnDel, &QPushButton::clicked, this, &MeshTab::slot_DeleteMesh);
	connect(m_btnIntersect, &QPushButton::clicked, this, &MeshTab::slot_OnIntersect);
	connect(m_btnUnion, &QPushButton::clicked, this, &MeshTab::slot_OnUnion);
	connect(m_btnDiff, &QPushButton::clicked, this, &MeshTab::slot_OnDiff);
}

void MeshTab::Update(bool isRefresh, int selectIndex, bool isClear, bool isCloseDialog)
{
	if (m_treeMeshList)
	{
		if (isRefresh)
		{
			m_treeMeshList->Update();
		}
		else
		{
			m_treeMeshList->SelectionRefresh(selectIndex, isClear, isCloseDialog);
		}

		if (m_treeMeshHeader->isHidden())
		{
			QSize size = m_treeMeshList->sizeForRow();

			//m_treeHeader->setFixedHeight(size.height() + 5);
			m_treeMeshHeader->setFixedHeight(25); // 201103 허 건 대리(size 값이 다른경우 발생으로 인한 상수 변경)
			m_treeMeshHeader->setHidden(false);
			m_treeMeshList->setStyleSheet(STYLE_MANAGER->treeDivList);
		}

		m_treeMeshHeader->ResetColumnAll();
	}
}

void MeshTab::UpdateSubWidget(muint8 UID)
{
	if (m_treeMeshList)
	{
		m_treeMeshList->UpdateSubwidget(UID);
	}
}

void MeshTab::ChangeSubUID(muint8 newUID)
{
	if (m_treeMeshList)
	{
		m_treeMeshList->ChangeSubUID(newUID);
	}
}

void MeshTab::Clear()
{
	if (m_treeMeshList)
	{
		m_treeMeshList->clear();
	}
}

void MeshTab::SetTabType(MAINTAB_TYPE type)
{
	if ((MAINTAB_MESH_EDITING != type) != m_btnLoad->isHidden())
	{
		m_btnDiff->setHidden(true);
		m_btnIntersect->setHidden(true);
		m_btnUnion->setHidden(true);
	}
	else if (MAINTAB_MESH_EDITING == type)
	{
		m_btnDiff->setHidden(false);
		m_btnIntersect->setHidden(false);
		m_btnUnion->setHidden(false);
	}
}

void MeshTab::ClearSelection()
{
	m_treeMeshList->clearSelection();
}

MeshListWidget* MeshTab::GetMeshList() const
{
	return m_treeMeshList;
}

MeshListHeaderWidget* MeshTab::GetMeshHeader() const
{
	return m_treeMeshHeader;
}

bool MeshTab::eventFilter(QObject* watched, QEvent* e)
{
	if (watched == nullptr)
	{
		return QWidget::eventFilter(watched, e);
	}

	if (!watched->inherits("QPushButton") && !watched->inherits("QToolButton"))
	{
		return QWidget::eventFilter(watched, e);
	}

	QEvent::Type eventType = e->type();

	if (eventType == QEvent::HoverLeave ||
		eventType == QEvent::Leave ||
		eventType == QEvent::Show)
	{
		if (m_HoverWidget)
		{
			if (!m_HoverWidget->isChecked())
			{
				m_HoverWidget->setIcon(m_LeaveIcon);
			}
		}
	}

	bool isHovered = false;
	QWidget* w = dynamic_cast<QWidget*>(watched);
	{
		QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(e);
		if (mouseEvent)
		{
			if (w)
			{
				if (w->rect().contains(mouseEvent->pos()))
				{
					isHovered = true;
				}
			}
		}
	}
	{
		QHoverEvent* hoverEvent = dynamic_cast<QHoverEvent*>(e);
		if (hoverEvent)
		{
			if (w)
			{
				if (w->rect().contains(hoverEvent->pos()))
				{
					isHovered = true;
				}
			}
		}
	}
	{
		QWheelEvent* wheelEvent = dynamic_cast<QWheelEvent*>(e);
		if (wheelEvent)
		{
			if (w)
			{
				if (w->rect().contains(wheelEvent->pos()))
				{
					isHovered = true;
				}
			}
		}
	}

	if (isHovered)
	{
		if (watched == m_btnLoad && !m_btnLoad->isChecked())
		{
			setButtonStyle(m_btnLoad, ICON_NEW_LIST, eMeshTabButtonType::Hovered);
		}
		else if (watched == m_btnDel && !m_btnDel->isChecked())
		{
			setButtonStyle(m_btnDel, ICON_DELETE_LIST, eMeshTabButtonType::Hovered);
		}
		else if (watched == m_btnUnion && !m_btnUnion->isChecked())
		{
			setButtonStyle(m_btnUnion, ICON_UNION_LIST, eMeshTabButtonType::Hovered);
		}
		else if (watched == m_btnIntersect && !m_btnIntersect->isChecked())
		{
			setButtonStyle(m_btnIntersect, ICON_INTERSECT_LIST, eMeshTabButtonType::Hovered);
		}
		else if (watched == m_btnDiff && !m_btnDiff->isChecked())
		{
			setButtonStyle(m_btnDiff, ICON_DIFF_LIST, eMeshTabButtonType::Hovered);
		}
	}

	return QWidget::eventFilter(watched, e);
}

void MeshTab::setButtonStyle(QAbstractButton* button, const std::array<QIcon, 3>& iconList, eMeshTabButtonType type)
{
	switch (type)
	{
	case eMeshTabButtonType::Normal:
		button->setChecked(false);
		break;
	case eMeshTabButtonType::Selected:
		button->setChecked(true);
		break;
	case eMeshTabButtonType::Hovered:
		m_HoverWidget = button;
		m_LeaveIcon = iconList[(int)eMeshTabButtonType::Normal];
		break;
	default:
		qCritical() << "invalid button style type : " << (int)type;
		Q_ASSERT(false);
	}
	button->setIcon(iconList[(int)type]);
}

QList<muint32> MeshTab::getSelectedIndexList() const
{
	QList<QTreeWidgetItem*> selectItemList = m_treeMeshList->selectedItems();
	QList<muint32> selectedIndexList;

	for (int i = 0; i < selectItemList.size(); i++)
	{
		QTreeWidgetItem* item = selectItemList.at(i);

		if (item->childCount() == 0)
		{
			item = item->parent();
		}

		muint32 index = m_treeMeshList->indexOfTopLevelItem(item);

		if (!selectedIndexList.contains(index))
		{
			selectedIndexList.push_back(index);
		}
	}
	return selectedIndexList;
}

void MeshTab::doBooleanWork(QPushButton* button, const std::array<QIcon, 3>& iconList, MESH_WORK_MODE mode, const QString& warningMessageTitle)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (
		(mode == MESH_WORK_INTERSECT) ||
		(mode == MESH_WORK_DIFF) ||
		(mode == MESH_WORK_UNION)
		)
	{
		setButtonStyle(button, iconList, eMeshTabButtonType::Selected);

		QList<muint32> selectedIndexList = getSelectedIndexList();

		if (selectedIndexList.size() != 2)
		{
			QMessageBox::warning(this, warningMessageTitle, STRING_MANAGER->getString(STR_SELECT_2_WARN));
		}
		else
		{
			m_pActionManager->action_Mesh_Boolean(m_pDataContext, mode, selectedIndexList.at(0), selectedIndexList.at(1));
		}

		setButtonStyle(button, iconList, eMeshTabButtonType::Normal);
	}
}

void MeshTab::slot_MeshHeaderClick(QTreeWidgetItem* item, int column)
{
	m_treeMeshHeader->ItemClicked(item, column);
}

void MeshTab::slot_MeshHeaderColumnStateChanged(int column)
{
	m_treeMeshList->UpdateItemList();

	const MeshListColumn* pCol = m_pColumnList_Header->GetByIndex(column);
	if (pCol == nullptr)
	{
		return;
	}
	if (pCol->GetKey() == M_COL_SHOW)
	{
		m_pActionManager->SendActionProcessFinished(ACTION_PROCESSING::ACTP_MESH_LAYER_VISIBILITY_CHANGED);
		m_pWinManager->renderLater_All();
	}
	else if (pCol->GetKey() == M_COL_LOCK)
	{
		m_pActionManager->SendActionProcessFinished(ACTION_PROCESSING::ACTP_MESH_LAYER_LOCK_CHANGED);
		m_pWinManager->renderLater_All();
	}
}

void MeshTab::slot_MeshHeaderExpanedChanged(bool isExpand)
{
	m_treeMeshList->updateSize(isExpand);
	updateHeight();
}

void MeshTab::slot_MeshListItemClick(QTreeWidgetItem* item, int column)
{
	m_treeMeshList->ItemClicked(item, column);
}

void MeshTab::slot_MeshListItemDoubleClick(QTreeWidgetItem* item, int column)
{
	m_treeMeshList->ItemDoubleClicked(item, column);
}

void MeshTab::slot_MeshListItemChanged(QTreeWidgetItem* item, int column)
{
	if ((m_pDataContext == nullptr) || (item == nullptr))
	{
		return;
	}

	if (item->childCount() == 0)
	{
		return;
	}

	const MeshListColumn* pColumn = m_pColumnList_Widget->GetByIndex(column);
	if (pColumn == nullptr)
	{
		return;
	}

	if (M_COL_NAME == pColumn->GetKey())
	{
		muint8 index = m_treeMeshList->indexOfTopLevelItem(item);
		QString newName = item->text(column);
		//if (treeMesh->editMode && m_pDataContext->volume_data.GetMeshName(index).compare(newName))
		if (m_pDataContext->m_MeshData.GetMeshName(index).compare(newName))
		{
			if (newName.contains(QRegularExpression(FILE_NAME_RULE)))
			{
				m_treeMeshList->blockSignals(true);
				QMessageBox::warning(this, QString("Name rule"),
					QString("A mesh name cannot contain any of the following characters : \"\\ / : *? \" < > |	\""));
				item->setText(column, m_pDataContext->m_MeshData.GetMeshName(m_treeMeshList->indexOfTopLevelItem(item)));
				m_treeMeshList->blockSignals(false);
			}
			else
			{
				m_pActionManager->action_MeshList_name_change(m_pDataContext, index, newName);
			}
		}

		item->setFlags(item->flags() & (~Qt::ItemIsEditable));
		m_treeMeshList->SetEditMode(false);
		m_pWinManager->setMoveFocus(true);
	}
}

void MeshTab::slot_MeshListColumnStateChanged(int column)
{
	const MeshListColumn* pCol = m_pColumnList_Widget->GetByIndex(column);
	if (pCol == nullptr)
	{
		return;
	}
	if (pCol->GetKey() == M_COL_SHOW)
	{
		m_treeMeshHeader->UpdateColumn(M_COL_SHOW);
		m_pActionManager->SendActionProcessFinished(ACTION_PROCESSING::ACTP_MESH_LAYER_VISIBILITY_CHANGED);
		m_pWinManager->renderLater_All();
	}
	else if (pCol->GetKey() == M_COL_LOCK)
	{
		m_treeMeshHeader->UpdateColumn(M_COL_LOCK);
		m_pActionManager->SendActionProcessFinished(ACTION_PROCESSING::ACTP_MESH_LAYER_LOCK_CHANGED);
		m_pWinManager->renderLater_All();
	}
}

void MeshTab::slot_LoadMesh()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	setButtonStyle(m_btnLoad, ICON_NEW_LIST, eMeshTabButtonType::Selected);

	QString filters = QString("STL File(*.stl;*.STL);;VTK File(*.vtk;*.VTK);;OBJ File(*.obj;*OBJ)");;
	filters += QString(";;3MF File(*.3mf;*.3MF)");

#ifdef USD_IMPORT
	filters += QString(";;USD File(*.usd;*.USD)");
#endif

	QString lastestFilePath;
	bool latest = m_pWinManager->lastestPathGet(lastestFilePath, true);
	QStringList importFilePathList = ImportFileListDialog(
		this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN),
		latest ? lastestFilePath : QDir::homePath(),
		filters
	);

	if (!importFilePathList.isEmpty())
	{
		//20201112_byPHS - Dlg reject
		m_pWinManager->rejectMEViewDlg();

		// 201103 허 건 대리 
		// 메쉬 최대개수 32개
		int n_tot_mesh = m_pDataContext->m_MeshData.GetMeshCount() + importFilePathList.length();

		if (importFilePathList.length() > 0 && n_tot_mesh <= 32)
		{
			slot_AddMeshes(importFilePathList);
		}
		else
		{
			QMessageBox::warning(this, "Warning", "Maximum Number of mesh is 32.");
		}
	}

	setButtonStyle(m_btnLoad, ICON_NEW_LIST, eMeshTabButtonType::Normal);
}

void MeshTab::slot_OnUnion()
{
	doBooleanWork(m_btnUnion, ICON_UNION_LIST, MESH_WORK_UNION, "Mesh Union");
}

void MeshTab::slot_OnDiff()
{
	doBooleanWork(m_btnDiff, ICON_DIFF_LIST, MESH_WORK_DIFF, "Mesh Diff");
}

void MeshTab::slot_OnIntersect()
{
	doBooleanWork(m_btnIntersect, ICON_INTERSECT_LIST, MESH_WORK_INTERSECT, "Mesh Intersect");
}

void MeshTab::slot_OnSphere()
{
	setButtonStyle(m_btnLoad, ICON_NEW_LIST, eMeshTabButtonType::Selected);

	m_pActionManager->action_MeshList_create_polygon(m_pDataContext, eMeshPrimitiveType::Sphere);

	setButtonStyle(m_btnLoad, ICON_NEW_LIST, eMeshTabButtonType::Normal);
}

void MeshTab::slot_OnCube()
{
	setButtonStyle(m_btnLoad, ICON_NEW_LIST, eMeshTabButtonType::Selected);

	m_pActionManager->action_MeshList_create_polygon(m_pDataContext, eMeshPrimitiveType::Cube);

	setButtonStyle(m_btnLoad, ICON_NEW_LIST, eMeshTabButtonType::Normal);
}

void MeshTab::slot_OnCylinder()
{
	setButtonStyle(m_btnLoad, ICON_NEW_LIST, eMeshTabButtonType::Selected);

	m_pActionManager->action_MeshList_create_polygon(m_pDataContext, eMeshPrimitiveType::Cylinder);

	setButtonStyle(m_btnLoad, ICON_NEW_LIST, eMeshTabButtonType::Normal);
}

void MeshTab::slot_DeleteMesh()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	setButtonStyle(m_btnDel, ICON_DELETE_LIST, eMeshTabButtonType::Selected);

	QList<QTreeWidgetItem*>& selectItemList = m_treeMeshList->selectedItems();
	QList<QTreeWidgetItem*> deleteItemList;

	for (int i = 0; i < selectItemList.size(); i++)
	{
		QTreeWidgetItem* item = selectItemList.at(i);

		muint32 index = m_treeMeshList->indexOfTopLevelItem(item);

		if (!deleteItemList.contains(item))
		{
			deleteItemList.push_back(item);
		}
	}

	if (deleteItemList.size() > m_pDataContext->m_MeshData.GetMeshCount())
	{
		QString message = QString("Only %1 or fewer layers can be deleted.").arg(m_pDataContext->m_MeshData.GetMeshCount());
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), message);
		return;
	}

	if (deleteItemList.size() == 1)
	{
		mint8 index = m_treeMeshList->indexOfTopLevelItem(deleteItemList.at(0));
		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(index);

		if (info)
		{
			mint32 lUID = info->uid;
			m_pActionManager->action_MeshList_delete(m_pDataContext, index, lUID);
		}
	}
	else if (deleteItemList.size() > 1)
	{
		std::vector<mint8> deleteIndexList;
		for (int i = 0; i < deleteItemList.size(); i++)
		{
			deleteIndexList.push_back(m_treeMeshList->indexOfTopLevelItem(deleteItemList[i]));
		}

		m_pActionManager->action_MeshList_delete(m_pDataContext, deleteIndexList);
	}

	setButtonStyle(m_btnDel, ICON_DELETE_LIST, eMeshTabButtonType::Normal);
}

void MeshTab::slot_AddMesh(QString filename)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	m_pWinManager->loadFiles(filename);

	int nMesh = m_pDataContext->m_MeshData.GetMeshCount();
	if (nMesh > 0)
	{
		m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();
		m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(nMesh - 1, true);
		m_pDataContext->m_MeshData.SetCurrentMeshIndex(nMesh - 1);
	}
}

void MeshTab::slot_AddMeshes(QStringList filePathList)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	QString ext = filePathList[0].section('.', -1);
	//EXPORT_FILES index = (EXPORT_FILES)extList.indexOf(ext);
	//if (index == -1) return;

	if (ext.compare("stl") == 0 || ext.compare("STL") == 0)
	{
		m_pActionManager->action_FileWork_Import_STL(m_pDataContext, filePathList);
	}
	else if (ext.compare("obj") == 0 || ext.compare("OBJ") == 0)
	{
		m_pActionManager->action_FileWork_Import_OBJ(m_pDataContext, filePathList);
	}
	else if (ext.compare("vtk") == 0 || ext.compare("VTK") == 0)
	{
		m_pActionManager->action_FileWork_Import_VTK(m_pDataContext, filePathList);
	}
	else if (ext.compare("3mf") == 0 || ext.compare("3MF") == 0)
	{
		m_pActionManager->action_FileWork_Import_3MF(m_pDataContext, filePathList);
	}
#ifdef USD_IMPORT
	else if (ext.compare("usd") == 0 || ext.compare("USD") == 0)
	{
		m_pActionManager->action_FileWork_Import_USD(m_pDataContext, filePathList[0]);
		m_pWinManager->UpdateAllMeshTabList();
	}
#endif
	else
	{
		return;
	}

	int nMesh = m_pDataContext->m_MeshData.GetMeshCount();
	if (nMesh > 0)
	{
		m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();
		m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(nMesh - 1, true);
		m_pDataContext->m_MeshData.SetCurrentMeshIndex(nMesh - 1);
	}
}
