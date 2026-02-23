#include "stdafx.h"
#include "VisualPrintMeshTab.h"
#include "WindowManager.h"
#include "StringManager.h"
#include "StyleManager.h"
#include "WindowManager.h"
#include "ProductManager.h"
#include "ResourceManager.h"
#include "Renderer/Renderer.h"
#include "Main/MainSegmentWidget.h"
#include "DataContext.h"

VisualPrintMeshTab::VisualPrintMeshTab(QWidget* parent)
	:CollapseWidget(QString(), parent),
	m_pDataContext(nullptr)
{

}

VisualPrintMeshTab::~VisualPrintMeshTab()
{

}

void VisualPrintMeshTab::Init(DataContext* pDataContext)
{
	m_pDataContext = pDataContext;

	int nRow = 0;

	QString strVal;

	m_bExpand = false;
	//	m_bExpand = false;
	/*if (!WIN_MANAGER->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_expandMesh, strVal))
	m_bExpand = false;
	else if (0 >= strVal.toInt())
	m_bExpand = false;
	else
	m_bExpand = true;*/

	//	m_treeMesh = new MeshListWidget(this);
	m_treeMesh = new VisualPrintMeshListWidget(m_pDataContext, this);
	m_treeMesh->setColumnCount(M_COL_COUNT);
	m_treeMesh->setColumnWidth(M_COL_SUB, 20);
	m_treeMesh->setColumnWidth(M_COL_COLOR, 20);
	m_treeMesh->setColumnWidth(M_COL_SHOW, 20);
	m_treeMesh->setHeaderHidden(true);
	m_treeMesh->setStyleSheet(STYLE_MANAGER->treeBasicList);
	m_treeMesh->setAutoFillBackground(true);
	m_treeMesh->setContentsMargins(0, 0, 0, 0);

	connect(m_treeMesh, &QTreeWidget::itemClicked, this, &VisualPrintMeshTab::slot_OnColumnClick);
	connect(m_treeMesh, &QTreeWidget::itemDoubleClicked, this, &VisualPrintMeshTab::slot_OnColumnDoubleClick);
	connect(m_treeMesh, &QTreeWidget::itemChanged, this, &VisualPrintMeshTab::slot_OnColumnChanged);

	m_treeHeader = new QTreeWidget(this);
	m_treeHeader->setColumnCount(M_COL_COUNT + 1);
	m_treeHeader->setColumnWidth(M_COL_SUB, 20);
	m_treeHeader->setColumnWidth(M_COL_COLOR, 20);
	m_treeHeader->setColumnWidth(M_COL_SHOW, 20);
	m_treeHeader->setColumnWidth(M_COL_COUNT, 20);
	m_treeHeader->setHeaderHidden(true);
	m_treeHeader->setSelectionMode(QTreeWidget::NoSelection);
	m_treeHeader->setStyleSheet(STYLE_MANAGER->treeHeader);
	m_treeHeader->setAutoFillBackground(true);
	m_treeHeader->setHidden(true);	//show after load
	m_treeHeader->setContentsMargins(0, 0, 0, 0);
	m_treeHeader->setFocusPolicy(Qt::NoFocus);

	connect(m_treeHeader, &QTreeWidget::itemClicked, this, &VisualPrintMeshTab::slot_OnHeaderClick);

	addWidget(m_treeHeader, nRow, 0, QMargins(0, 0, 0, 0), Qt::AlignTop, true);

	getLayout(nRow)->setSpacing(0);
	getLayout(nRow)->setMargin(0);

	addWidget(m_treeMesh, nRow++, 0, QMargins(0, 0, 0, 0), Qt::AlignTop);

	m_treeMesh->resize(500, 500);

	QMenu* clipMenu = new QMenu(this);

	QAction* actLoad = new QAction("Load File", this);
	connect(actLoad, &QAction::triggered, this, &VisualPrintMeshTab::slot_LoadMesh);

	clipMenu->addAction(actLoad);

	//	QMenu *subMenu = new QMenu("Create Meshes...", this);
	//	clipMenu->addMenu(subMenu);

	//	actLoad = new QAction("Sphere", this);
	//	connect(actLoad, &QAction::triggered, this, &VisualPrintMeshTab::slot_OnSphere);
	//	subMenu->addAction(actLoad);

	//	actLoad = new QAction("Cube", this);
	//	connect(actLoad, &QAction::triggered, this, &VisualPrintMeshTab::slot_OnCube);
	//	subMenu->addAction(actLoad);

	//	actLoad = new QAction("Cylinder", this);
	//	connect(actLoad, &QAction::triggered, this, &VisualPrintMeshTab::slot_OnCylinder);
	//	subMenu->addAction(actLoad);

	/*m_btnDiff = new QPushButton(this);
	m_btnDiff->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_DIFF));
	m_btnDiff->setFixedSize(QSize(24, 24));
	m_btnDiff->setIconSize(QSize(24, 24));
	m_btnDiff->setToolTip("Boolean Difference");
	m_btnDiff->setCheckable(true);
	m_btnDiff->setChecked(false);
	m_btnDiff->setMouseTracking(true);
	m_btnDiff->installEventFilter(this);*/

	//	connect(m_btnDiff, &QPushButton::clicked, this, &VisualPrintMeshTab::slot_OnDiff);

	/*m_btnUnion = new QPushButton(this);
	m_btnUnion->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_UNION));
	m_btnUnion->setFixedSize(QSize(24, 24));
	m_btnUnion->setIconSize(QSize(24, 24));
	m_btnUnion->setToolTip("Boolean Union");
	m_btnUnion->setCheckable(true);
	m_btnUnion->setChecked(false);
	m_btnUnion->setMouseTracking(true);
	m_btnUnion->installEventFilter(this);*/

	//	connect(m_btnUnion, &QPushButton::clicked, this, &VisualPrintMeshTab::slot_OnUnion);

	/*m_btnIntersect = new QPushButton(this);
	m_btnIntersect->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_INTERSECT));
	m_btnIntersect->setFixedSize(QSize(24, 24));
	m_btnIntersect->setIconSize(QSize(24, 24));
	m_btnIntersect->setToolTip("Boolean Intersection");
	m_btnIntersect->setCheckable(true);
	m_btnIntersect->setChecked(false);
	m_btnIntersect->setMouseTracking(true);
	m_btnIntersect->installEventFilter(this);*/

	//	connect(m_btnIntersect, &QPushButton::clicked, this, &VisualPrintMeshTab::slot_OnIntersect);


	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	/*m_btnLoad = new QToolButton(this);
	m_btnLoad->setPopupMode(QToolButton::InstantPopup);
	m_btnLoad->setMenu(clipMenu);
	m_btnLoad->setIcon(RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_NEW));
	m_btnLoad->setFixedSize(QSize(34, 34));
	m_btnLoad->setIconSize(QSize(34, 34));
	m_btnLoad->setCheckable(true);
	m_btnLoad->setChecked(false);
	m_btnLoad->setToolTip("Add mesh");
	m_btnLoad->setMouseTracking(true);
	m_btnLoad->installEventFilter(this);*/

	/*m_btnDel = new QPushButton(this);
	m_btnDel->setIcon(RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_DEL));
	m_btnDel->setFixedSize(QSize(34, 34));
	m_btnDel->setIconSize(QSize(34, 34));
	m_btnDel->setToolTip("Delete mesh");
	m_btnDel->setCheckable(true);
	m_btnDel->setChecked(false);
	m_btnDel->setMouseTracking(true);
	m_btnDel->installEventFilter(this);

	connect(m_btnDel, &QPushButton::clicked, this, &VisualPrintMeshTab::slot_DeleteMesh);*/


	//	addWidget(m_btnDiff, nRow, 0, QMargins(), Qt::AlignLeft);
	//	addWidget(m_btnUnion, nRow, 0, QMargins(), Qt::AlignLeft);
	//	addWidget(m_btnIntersect, nRow, 0, QMargins(), Qt::AlignLeft);
	//	addWidget(emptyBox0, nRow);
	//	addWidget(m_btnLoad, nRow);
	//	addWidget(m_btnDel, nRow++);


	emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	addWidget(emptyBox0, nRow++, 0);

	m_HoverWidget = NULL;

	QString title = QString("Visual Print Mesh List");

	setTitle(title);

	setOpenWidget(false);
}

void VisualPrintMeshTab::Update(bool refresh, int selIndex)
{
	if (m_treeMesh)
	{
		if (refresh)
			m_treeMesh->update(false);
		else
			m_treeMesh->selectionRefresh(selIndex);

		if (m_treeHeader->isHidden())
		{
			QSize size = m_treeMesh->sizeForRow();

			//m_treeHeader->setFixedHeight(size.height() + 5);
			m_treeHeader->setFixedHeight(25);
			m_treeHeader->setHidden(false);
			m_treeMesh->setStyleSheet(STYLE_MANAGER->treeDivList);
		}

		UpdateHeader();
	}
}

void VisualPrintMeshTab::UpdateHeader(MESH_COLUMN_AKA type/*=M_COL_COUNT*/)
{
	static const QIcon icons[] = { RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE), RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE) };
	static const QBrush brushes[] = { QColor(0, 0, 0, 0) , QColor(48, 48, 48) };
	static const QString stres[] = { QString(QChar(0x25B2)),QString(QChar(0x25BC)) };
	QTreeWidgetItem* item = NULL;

	if (M_COL_COUNT == type)
	{
		m_treeHeader->clear();
		item = new QTreeWidgetItem(m_treeHeader);

		m_treeHeader->insertTopLevelItem(0, item);
	}
	else
		item = m_treeHeader->topLevelItem(0);

	if (item)
	{
		bool res = m_pDataContext->volume_data.visibleCheckAll(true);

		item->setIcon(M_COL_SHOW, icons[res]);
		item->setBackground(M_COL_SHOW, brushes[res]);
		item->setForeground(M_COL_SHOW, brushes[res]);
		item->setData(M_COL_SHOW, Qt::UserRole + 1, QVariant(res));

		if (m_bExpand)
		{
			m_treeMesh->updateSize(m_bExpand);
			updateHeight();
			updateGeometry();
		}

		item->setText(M_COL_COUNT, stres[m_bExpand]);
	}
}

void VisualPrintMeshTab::UpdateSubWidget(muint8 UID)
{
	if (m_treeMesh)
		m_treeMesh->updateSubwidget(UID);
}

void VisualPrintMeshTab::ChangeSubUID(muint8 newUID)
{
	if (m_treeMesh)
		m_treeMesh->changeSubUID(newUID);
}

void VisualPrintMeshTab::Clear()
{
	if (m_treeMesh)
		m_treeMesh->clear();
}

void VisualPrintMeshTab::SetTabType(MAINTAB_TYPE type)
{
	if (m_treeMesh)
		m_treeMesh->setTabType(type);

	/*if ((MAINTAB_MESH_EDITING != type) != m_btnLoad->isHidden())
	{
	m_btnLoad->setHidden(MAINTAB_MESH_EDITING != type);
	m_btnDiff->setHidden(MAINTAB_MESH_EDITING != type);
	m_btnIntersect->setHidden(MAINTAB_MESH_EDITING != type);
	m_btnUnion->setHidden(MAINTAB_MESH_EDITING != type);
	}*/
}

VisualPrintMeshListWidget* VisualPrintMeshTab::GetVisualPrintMeshListWidget()
{
	return m_treeMesh;
}


//bool VisualPrintMeshTab::eventFilter(QObject *watched, QEvent *e)
//{
//	if (watched == NULL) return QWidget::eventFilter(watched, e);
//
//	static QIcon LoadIcon[] = { RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_NEW),
//		RESOURCE_MANAGER->getIcon(ICON_ROILIST_NEW_HOVER) };
//	static QIcon DelIcon[] = { RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_DEL),
//		RESOURCE_MANAGER->getIcon(ICON_ROILIST_DEL_HOVER) };
//	static QIcon uniIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_UNION),
//		RESOURCE_MANAGER->getIcon(ICON_UNION_HOVER) };
//	static QIcon diffIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_DIFF),
//		RESOURCE_MANAGER->getIcon(ICON_DIFF_HOVER) };
//	static QIcon interIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_INTERSECT),
//		RESOURCE_MANAGER->getIcon(ICON_INTERSECT_HOVER) };
//
//	if (!watched->inherits("QPushButton") && !watched->inherits("QToolButton"))
//		return QWidget::eventFilter(watched, e);
//
//	QEvent::Type _type = e->type();
//
//	int res = 0;
//
//	if (_type == QEvent::HoverLeave ||
//		_type == QEvent::Leave || _type == QEvent::Show)
//	{
//		if (m_HoverWidget)
//		{
//			if (!m_HoverWidget->isChecked())
//				m_HoverWidget->setIcon(m_LeaveIcon);
//		}
//	}
//
//	QWidget* w = dynamic_cast<QWidget*>(watched);
//	{
//		QMouseEvent *evt = dynamic_cast<QMouseEvent*>(e);
//		if (evt)
//		{
//			if (w)
//			{
//				if (w->rect().contains(evt->pos()))
//					res = 1;
//			}
//		}
//	}
//	{
//		QHoverEvent *evt = dynamic_cast<QHoverEvent*>(e);
//		if (evt)
//		{
//			if (w)
//			{
//				if (w->rect().contains(evt->pos()))
//					res = 1;
//			}
//		}
//	}
//	{
//		QWheelEvent *evt = dynamic_cast<QWheelEvent*>(e);
//		if (evt)
//		{
//			if (w)
//			{
//				if (w->rect().contains(evt->pos()))
//					res = 1;
//			}
//		}
//	}
//
//	if (res == 1)
//	{
//		if (watched == m_btnLoad && !m_btnLoad->isChecked())
//		{
//			m_btnLoad->setIcon(LoadIcon[res]);
//			m_HoverWidget = m_btnLoad;
//			m_LeaveIcon = LoadIcon[0];
//		}
//		else if (watched == m_btnDel && !m_btnDel->isChecked())
//		{
//			m_btnDel->setIcon(DelIcon[res]);
//			m_HoverWidget = m_btnDel;
//			m_LeaveIcon = DelIcon[0];
//		}
//		else if (watched == m_btnUnion && !m_btnUnion->isChecked())
//		{
//			m_btnUnion->setIcon(uniIcon[res]);
//			m_HoverWidget = m_btnUnion;
//			m_LeaveIcon = uniIcon[0];
//		}
//		else if (watched == m_btnIntersect && !m_btnIntersect->isChecked())
//		{
//			m_btnIntersect->setIcon(interIcon[res]);
//			m_HoverWidget = m_btnIntersect;
//			m_LeaveIcon = interIcon[0];
//		}
//		else if (watched == m_btnDiff && !m_btnDiff->isChecked())
//		{
//			m_btnDiff->setIcon(diffIcon[res]);
//			m_HoverWidget = m_btnDiff;
//			m_LeaveIcon = diffIcon[0];
//		}
//	}
//
//	return QWidget::eventFilter(watched, e);
//}

void VisualPrintMeshTab::slot_OnHeaderClick(QTreeWidgetItem* item, int column)
{
	static const QString stres[] = { QString(QChar(0x25B2)),QString(QChar(0x25BC)) };
	static const QBrush brushes[] = { QBrush(QColor(0,0,0,0)), QBrush(QColor(48, 48, 48)) };
	static const QIcon icons[] = { RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE), RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE) };

	if (NULL == item) return;

	if (M_COL_SHOW == column)
	{
		bool res = item->data(column, Qt::UserRole + 1).toBool();
		res = !res;
		item->setIcon(M_COL_SHOW, icons[res]);
		item->setBackground(M_COL_SHOW, brushes[res]);
		item->setForeground(M_COL_SHOW, brushes[res]);
		item->setData(M_COL_SHOW, Qt::UserRole + 1, QVariant(res));

		m_treeMesh->updateColumn(M_COL_SHOW, res);
	}
	else if (M_COL_COUNT == column)
	{
		m_bExpand = !m_bExpand;
		m_treeMesh->updateSize(m_bExpand);
		item->setText(column, stres[m_bExpand]);
		updateHeight();

		WIN_MANAGER->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_expandMesh, QString::number(m_bExpand));
	}
}

void VisualPrintMeshTab::slot_OnColumnClick(QTreeWidgetItem* item, int column)
{
	if (NULL == item) return;

	int cnt = m_treeMesh->selectedItems().count();

	if (cnt > 1) return;

	static QIcon showIcons[] = { RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE), RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE) };

	muint32 index = m_treeMesh->indexOfTopLevelItem(item);

	if (item->childCount() == 0)
	{
		item = item->parent();
		index = m_treeMesh->indexOfTopLevelItem(item);
		m_treeMesh->selectionRefresh(index);
		return;
	}

	m_treeMesh->selectionRefresh(index);

	if (M_COL_SHOW == column)
	{
		MeshInfo* info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(index);

		if (info)
		{
			info->show = !info->show;
			item->setIcon(M_COL_SHOW, showIcons[info->show]);
			UpdateHeader(M_COL_SHOW);

			WIN_MANAGER->renderLater_3DView();
		}
	}
	else if (M_COL_SUB == column)
	{
		bool res = false;

		res = item->isExpanded();

		item->setExpanded(!res);

		if (!res)
			item->setText(M_COL_SUB, QString("-"));
		else
			item->setText(M_COL_SUB, QString("+"));

	}

	m_pDataContext->m_VisualPrinting_MeshData.SetCurrentMeshIndex(index);
}

void VisualPrintMeshTab::slot_OnColumnDoubleClick(QTreeWidgetItem* item, int column)
{
	if (!m_pDataContext || NULL == item) return;

	if (column != M_COL_NAME)
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));

	int cnt = m_treeMesh->selectedItems().count();

	if (cnt > 1) return;

	muint32 index = m_treeMesh->indexOfTopLevelItem(item);

	if (item->childCount() == 0)
	{
		item = item->parent();
		index = m_treeMesh->indexOfTopLevelItem(item);
		m_treeMesh->selectionRefresh(index);
		return;
	}

	MeshInfo* info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(index);

	if (info)
	{
		if (M_COL_COLOR == column)
		{
			QColorDialog dlg;
			QColor col = QColor(info->color.r, info->color.g, info->color.b);
			dlg.setCurrentColor(col);
			dlg.setStyleSheet("background-color : rgba(48,48,48,255); color : white");

			if (dlg.exec() == QDialog::Accepted)
			{
				if (col != dlg.selectedColor())
				{
					ACTION_MANAGER->action_VisualPrint_MeshList_color_change(m_pDataContext, index, dlg.selectedColor());
				}
			}
		}
		else if (M_COL_NAME == column)
		{
			m_treeMesh->editMode = true;
			WIN_MANAGER->setMoveFocus(false);
			item->setFlags(item->flags() | Qt::ItemIsEditable);
			m_treeMesh->editItem(item, M_COL_NAME);
		}

		m_treeMesh->selectionUpdate(index);
		m_pDataContext->m_VisualPrinting_MeshData.SetCurrentMeshIndex(index);
	}
}

void VisualPrintMeshTab::slot_OnColumnChanged(QTreeWidgetItem* item, int column)
{
	if (!m_pDataContext || NULL == item) return;

	if (item->childCount() == 0)return;

	if (M_COL_NAME == column)
	{
		muint8 index = m_treeMesh->indexOfTopLevelItem(item);
		QString newName = item->text(column);
		//if (m_treeMesh->editMode && m_pDataContext->volume_data.VisualPrintGetMeshName(index).compare(newName))
		if (m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(index).compare(newName))
		{
			if (newName.contains(QRegularExpression(FILE_NAME_RULE)))
			{
				m_treeMesh->blockSignals(true);
				QMessageBox::warning(this, QString("Name rule"),
					QString("A mesh name cannot contain any of the following characters : \"\\ / : *? \" < > |	\""));
				item->setTextColor(column, QColor(255, 255, 255, 255));
				item->setText(column, m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(m_treeMesh->indexOfTopLevelItem(item)));
				m_treeMesh->blockSignals(false);
			}
			else
			{
				//ACTION_MANAGER->action_MeshList_name_change(index, newName);
				ACTION_MANAGER->action_VisualPrint_MeshList_name_change(m_pDataContext, index, newName);
			}
		}

		item->setFlags(item->flags() & (~Qt::ItemIsEditable));
		//m_treeMesh->editMode = false;
		WIN_MANAGER->setMoveFocus(true);
	}

	m_treeMesh->editMode = false;
}

void VisualPrintMeshTab::slot_LoadMesh()
{
	static QIcon icons[] = { RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_NEW), RESOURCE_MANAGER->getIcon(ICON_ROILIST_NEW) };

	bool res = true;
	m_btnLoad->setChecked(res);
	m_btnLoad->setIcon(icons[res]);
	res = false;

	QString strLatest;
	bool latest = WIN_MANAGER->lastestPathGet(strLatest, true);

	// 200903 허 건 대리
	// File Multi selelted
	QFileDialog dialog(this, STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN));

	dialog.setDirectory(latest ? strLatest : QDir::homePath());
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(QString("STL File(*.stl;*.STL);;VTK File(*.vtk;*.VTK);;OBJ File(*.obj;*OBJ)"));

	if (dialog.exec())
	{
		QStringList fileNames = dialog.selectedFiles();

		// 201103 허 건 대리 
		// 메쉬 최대개수 32개
		int n_tot_mesh = m_pDataContext->m_MeshData.GetMeshCount() + fileNames.length();

		if (fileNames.length() > 0 && n_tot_mesh <= 32)
		{
			slot_AddMeshes(fileNames);
		}
		else
		{
			QMessageBox::warning(this, "Warning", "Maximum Number of mesh is 32.");
		}
	}

	m_btnLoad->setChecked(res);
	m_btnLoad->setIcon(icons[res]);
}

void VisualPrintMeshTab::slot_AddMeshes(QStringList filenames)
{
	ACTION_MANAGER->action_FileWork_Import_STL(m_pDataContext, filenames);
}

