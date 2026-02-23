#include "stdafx.h"
#include "VisualPrintMeshListWidget.h"
#include "VisualPrintMeshInfoWidget.h"
#include "VisualPrintMeshSmoothWidget.h"
#include "WindowManager.h"
#include "ResourceManager.h"
#include "StringManager.h"
#include "ActionManager.h"
#include "MEVolumeView.h"
#include "DrawcutDlg.h"
#include "Main/MainMeshWidget.h"

/////////////////////////////////////////////////////////////////////////////////////
// Visual Print Mesh List Widget Class Member Functions - Start
///////////////////////////////////////////////////////////////////////////////////////
VisualPrintMeshListWidget::VisualPrintMeshListWidget(DataContext* pDataContext, QWidget* parent /*= nullptr*/) : 
	QTreeWidget(parent), 
	m_pDataContext(pDataContext)
{
	setContextMenuPolicy(Qt::DefaultContextMenu);
	setAnimated(true);

	actRename = new QAction(STRING_MANAGER->getString(STR_RENAME), this);
	actRename->setShortcut(QKeySequence(Qt::Key_F2));
	actRename->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(actRename, &QAction::triggered, this, &VisualPrintMeshListWidget::OnRename);
	addAction(actRename);

	editMode = false;
	//	setRootIsDecorated(false);
	setRootIsDecorated(false);
	setAcceptDrops(true);
	//	setDragEnabled(true);
	setDragEnabled(false);
	setDragDropMode(QAbstractItemView::DragDrop);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
}

VisualPrintMeshListWidget::~VisualPrintMeshListWidget()
{
}

QSize VisualPrintMeshListWidget::sizeForRow()
{
	return QSize(sizeHintForRow(0), sizeHintForRow(0));
}

void VisualPrintMeshListWidget::updateSubwidget(muint8 UID)
{
	//	if (UID >= m_pDataContext->m_MeshData.GetMeshCount()) return;
	if (!m_pDataContext || UID >= m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount()) return;

	QTreeWidgetItem* item = topLevelItem(UID);

	if (item)
	{
		if (item->child(0))
		{
			VisualPrintMeshInfoWidget* w = dynamic_cast<VisualPrintMeshInfoWidget*>(itemWidget(item->child(0), 0));
			if (w)
				w->Update();
		}
	}

}

void VisualPrintMeshListWidget::changeSubUID(muint8 newUID)
{
	//	if (newUID >= m_pDataContext->m_MeshData.GetMeshCount()) return;
	if (!m_pDataContext || newUID >= m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount()) return;

	QTreeWidgetItem* item = topLevelItem(newUID);

	if (item)
	{
		if (item->child(0))
		{
			VisualPrintMeshInfoWidget* w = dynamic_cast<VisualPrintMeshInfoWidget*>(itemWidget(item->child(0), 0));
			if (w)
				w->ChangeUID(newUID);
		}
	}
}

// bCreate = true : Create일 때, bCreate = false : Update 일 때
void VisualPrintMeshListWidget::update(bool bCreate)
{
	if (!m_pDataContext)
	{
		return;
	}

	clear();

	static QIcon icons[] = { RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE), RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE) };
	static const QBrush selBrush = QBrush(QColor(200, 200, 200, 255));
	//	muint32 item_cnt = m_pDataContext->m_MeshData.GetMeshCount();
	muint32 item_cnt = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();
	QList<QTreeWidgetItem*> items;

	setItemsExpandable(false);

	MeshInfo* info = nullptr;

	for (int i = 0; i < item_cnt; ++i)
	{
		//		info = m_pDataContext->m_MeshData.GetMeshInfo(i);
		info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(i);

		if (info)
		{
			COLOR col = info->color;
			QTreeWidgetItem* item = new QTreeWidgetItem(this);

			item->setText(M_COL_SUB, "+");
			item->setBackground(M_COL_COLOR, QBrush(QColor(col.r, col.g, col.b, 255)));
			item->setIcon(M_COL_SHOW, icons[info->show]);
			//			item->setText(M_COL_NAME, m_pDataContext->m_MeshData.GetMeshName(i));
			item->setText(M_COL_NAME, m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(i));

			insertTopLevelItem(0, item);

			QTreeWidgetItem* sub = new QTreeWidgetItem(item);
			sub->setFirstColumnSpanned(true);
			item->addChild(sub);
			item->setExpanded(false);
			//			setItemWidget(sub, 0, new VisualPrintMeshInfoWidget(i, this)); //ownership to treemesh
			setItemWidget(sub, 0, new VisualPrintMeshSmoothWidget(m_pDataContext, info->uid, bCreate, this));
			//			m_pVisualPrintMeshSmoothWidget = (VisualPrintMeshSmoothWidget*)sub;
		}
	}

	if (item_cnt > 0)
	{
		//		QTreeWidgetItem*item = topLevelItem(m_pDataContext->m_MeshData.GetCurrentMeshIndex());
		QTreeWidgetItem* item = topLevelItem(m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshIndex());

		if (nullptr == item)
		{
			//			m_pDataContext->m_MeshData.SetCurrentMeshIndex(0);
			m_pDataContext->m_VisualPrinting_MeshData.SetCurrentMeshIndex(0);
			item = topLevelItem(0);
		}


		if (item)
		{
			item->setBackground(M_COL_SUB, selBrush);
			item->setBackground(M_COL_SHOW, selBrush);
			item->setBackground(M_COL_NAME, selBrush);

			setItemSelected(item, true);

			setCurrentItem(item);
		}
	}
}

void VisualPrintMeshListWidget::updateSize(bool expand)
{
	const static QSize minSize = minimumSizeHint();

	if (expand)
	{
		setFixedHeight(sizeForRow().height() * (MESH_MAX + 1));
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else
	{
		//		setMinimumHeight(minSize.height());
		//		setMinimumHeight(630);
		//		setMinimumHeight(sizeForRow().height()*(MESH_MAX + 1));
		//		setMinimumHeight(670);
		//		setMinimumHeight(650);
		setMinimumHeight(500);
		setMaximumHeight(std::abs(sizeForRow().height()) * (MESH_MAX + 1));
		setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
}

void VisualPrintMeshListWidget::updateColumn(MESH_COLUMN_AKA aka, bool res)
{
	if (!m_pDataContext)
	{
		return;
	}

	static QIcon icons[] = { RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE), RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE) };

	if (aka == M_COL_SHOW)
	{
		for (int i = 0; i < m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount(); i++)
		{
			MeshInfo* info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(i);

			if (info)
			{
				if (res != info->show)
				{
					info->show = res;
					QTreeWidgetItem* item = topLevelItem(i);

					if (item)
						item->setIcon(M_COL_SHOW, icons[res]);
				}
			}
		}

		WIN_MANAGER->renderLater_3DView();
	}
}

void VisualPrintMeshListWidget::selectionUpdate(int index_dest)
{
	if (!m_pDataContext)
	{
		return;
	}

	static const QBrush selBrush[] = { QBrush(QColor(48, 48, 48, 255)) , QBrush(QColor(200, 200, 200, 255)) };

	for (int n = 0; n < topLevelItemCount(); n++)
	{
		QTreeWidgetItem* it = topLevelItem(n);
		bool selected = (index_dest == n || it->isSelected());

		//	if (selected)
		//		setCurrentItem(it);

		it->setBackground(M_COL_SUB, selBrush[selected]);
		it->setBackground(M_COL_SHOW, selBrush[selected]);
		it->setBackground(M_COL_NAME, selBrush[selected]);

		//WIN_MANAGER->vt_pckIDVisualPrint[n] = selected;
		m_pDataContext->m_VisualPrinting_MeshData.SetMeshInfoModeSelectMode(n, selected);

		//	if (it->isSelected() != selected)
		{
			it->setSelected(selected);
			setItemSelected(it, selected);
		}
	}

	if (currentItem() == NULL)
	{
		setCurrentItem(topLevelItem(index_dest));
	}
}

void VisualPrintMeshListWidget::selectionRefresh(int index_dest)
{
	if (!m_pDataContext)
	{
		return;
	}

	static QIcon icons[] = { RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE), RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE) };
	static const QBrush selBrush = QBrush(QColor(200, 200, 200, 255));
	muint32 item_cnt = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();
	QTreeWidgetItem* item = topLevelItem(index_dest);
	MeshInfo* info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(index_dest);
	COLOR col;
	if (NULL == info)
		return;

	if (NULL == item)
	{
		setItemsExpandable(false);
		item = new QTreeWidgetItem(this);

		insertTopLevelItem(index_dest, item);

		QTreeWidgetItem* sub = new QTreeWidgetItem(item);
		sub->setFirstColumnSpanned(true);
		item->addChild(sub);
		item->setExpanded(false);
		setItemWidget(sub, 0, new VisualPrintMeshInfoWidget(m_pDataContext, index_dest, this));
	}


	col = info->color;
	item->setText(M_COL_SUB, item->isExpanded() ? "-" : "+");
	item->setBackground(M_COL_COLOR, QBrush(QColor(col.r, col.g, col.b, 255)));
	item->setIcon(M_COL_SHOW, icons[info->show]);
	item->setText(M_COL_NAME, m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(index_dest));

	if (item_cnt > 0)
	{
		clearSelection();

		if (item)
		{
			setCurrentItem(item);
			item->setBackground(M_COL_SUB, selBrush);
			item->setBackground(M_COL_SHOW, selBrush);
			item->setBackground(M_COL_NAME, selBrush);
			item->setSelected(true);
			setItemSelected(item, true);
			scrollToItem(item);
			selectionUpdate(index_dest);
		}
		m_pDataContext->m_VisualPrinting_MeshData.SetCurrentMeshIndex(index_dest);
	}
}

void VisualPrintMeshListWidget::setTabType(MAINTAB_TYPE type)
{
	Tabtype = type;
}

VisualPrintMeshSmoothWidget* VisualPrintMeshListWidget::GetVisualPrintMeshSmoothWidget()
{
	return m_pVisualPrintMeshSmoothWidget;
}

void	VisualPrintMeshListWidget::setSmoothWidgets(int _val_smooth, int _val_reduce)
{
	if (!m_pDataContext)
	{
		return;
	}

	int n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();

	if (n_mesh < 1)
	{
		return;
	}

	for (int n = 0; n < topLevelItemCount(); n++)
	{
		QTreeWidgetItem* item = topLevelItem(n);

		if (item->child(0))
		{
			VisualPrintMeshSmoothWidget* w = dynamic_cast<VisualPrintMeshSmoothWidget*>(itemWidget(item->child(0), 0));

			if (w)
			{
				w->setParams(_val_smooth, _val_reduce);
			}
		}
	}
}

// 201019 허 건대리
void VisualPrintMeshListWidget::setSmoothWidgetsSmoothVal(int _val_smooth)
{
	if (!m_pDataContext)
	{
		return;
	}

	int n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();

	if (n_mesh < 1)
	{
		return;
	}

	for (int n = 0; n < topLevelItemCount(); n++)
	{
		QTreeWidgetItem* item = topLevelItem(n);

		if (item->child(0))
		{
			VisualPrintMeshSmoothWidget* w = dynamic_cast<VisualPrintMeshSmoothWidget*>(itemWidget(item->child(0), 0));

			if (w)
			{
				w->setSmoothVal(_val_smooth);
			}
		}
	}
}

// 201019 허 건대리
void VisualPrintMeshListWidget::setSmoothWidgetsReduceVal(int _val_reduce)
{
	if (!m_pDataContext)
	{
		return;
	}

	int n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();

	if (n_mesh < 1)
	{
		return;
	}

	for (int n = 0; n < topLevelItemCount(); n++)
	{
		QTreeWidgetItem* item = topLevelItem(n);

		if (item->child(0))
		{
			VisualPrintMeshSmoothWidget* w = dynamic_cast<VisualPrintMeshSmoothWidget*>(itemWidget(item->child(0), 0));

			if (w)
			{
				w->setReduceVal(_val_reduce);
			}
		}
	}
}

void VisualPrintMeshListWidget::setEnable(bool _b_enable)
{
	this->setEnabled(_b_enable);
}

void VisualPrintMeshListWidget::contextMenuEvent(QContextMenuEvent* event)
{
	if (editMode) return;


	/*QMenu menu(this);

	QMenu *subMenu = menu.addMenu("Export as...");

	menu.setStyleSheet(STYLE_MANAGER->m_MenuMaskROI);

	QAction* actExport = subMenu->addAction("Stereolithography (.stl, current layer)");
	connect(actExport, &QAction::triggered, this, &VisualPrintMeshListWidget::OnExportSTLFile);
	actExport = subMenu->addAction("Object (.obj, current layer)");
	connect(actExport, &QAction::triggered, this, &VisualPrintMeshListWidget::OnExportOBJFile);
	actExport = subMenu->addAction("Visualization Toolkit (.vtk, current layer)");
	connect(actExport, &QAction::triggered, this, &VisualPrintMeshListWidget::OnExportVTKFile);

	subMenu = menu.addMenu("Boolean action");

	QAction* actBoolean = subMenu->addAction("Difference");
	connect(actBoolean, &QAction::triggered, this, &VisualPrintMeshListWidget::slot_OnDiff);

	actBoolean = subMenu->addAction("Intersection");
	connect(actBoolean, &QAction::triggered, this, &VisualPrintMeshListWidget::slot_OnIntersect);

	actBoolean = subMenu->addAction("Union");
	connect(actBoolean, &QAction::triggered, this, &VisualPrintMeshListWidget::slot_OnUnion);

	QAction *actRemesh = menu.addAction("Solid");
	connect(actRemesh, &QAction::triggered, this, &VisualPrintMeshListWidget::slot_Solid);
	actRemesh = menu.addAction("Hollow");
	connect(actRemesh, &QAction::triggered, this, &VisualPrintMeshListWidget::slot_Hollow);
	actRemesh = menu.addAction("Remesh");
	connect(actRemesh, &QAction::triggered, this, &VisualPrintMeshListWidget::slot_Remesh);
	actRemesh = menu.addAction("Reduction");
	connect(actRemesh, &QAction::triggered, this, &VisualPrintMeshListWidget::slot_Reduction);
	actRemesh = menu.addAction("Smooth");
	connect(actRemesh, &QAction::triggered, this, &VisualPrintMeshListWidget::slot_Smooth);

	menu.exec(event->globalPos());*/
}

void VisualPrintMeshListWidget::dropEvent(QDropEvent* event)
{
	// 201008 허 건 대리 주석처리
	// Mesh-list 오작동 및 버그생성 방지
	//	if (event->source() != this && !(event->possibleActions() & Qt::MoveAction)) return;
	//
	//	QTreeWidgetItem* target = itemAt(event->pos());
	//
	//	if (target)
	//	{
	//		mint32 index_dest = indexOfTopLevelItem(target);
	//		mint32 index_source = m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshIndex();
	//
	//		if (index_dest < 0)
	//		{
	//			index_dest = 0;
	//		}
	////		else if (index_dest >= m_pDataContext->m_MeshData.GetMeshCount())
	//		else if (index_dest >= m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount())
	//		{
	////			index_dest = m_pDataContext->m_MeshData.GetMeshCount() - 1;
	//			index_dest = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount() - 1;
	//		}
	//
	//		if (index_dest == index_source)
	//			return;
	//
	//		ACTION_MANAGER->action_MeshList_UID_change(index_source, index_dest);
	//	}
}

void VisualPrintMeshListWidget::mousePressEvent(QMouseEvent* event)
{
	QTreeWidget::mousePressEvent(event);

	if (!m_pDataContext)
	{
		return;
	}

	QTreeWidgetItem* item = itemAt(event->pos());

	if (item == NULL || (Qt::NoModifier == event->modifiers() && (event->button() != Qt::RightButton)))
	{
		mint32 index = indexOfTopLevelItem(item);

		if (index >= 0 && index < topLevelItemCount())
		{
			selectionRefresh(index);
			m_pDataContext->m_VisualPrinting_MeshData.SetCurrentMeshIndex(index);
		}
		else
		{
			selectionRefresh(m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshIndex());
		}

		// update pck model index
		WIN_MANAGER->updateVisualPrintPckModel(index);

		return;
	}

	mint32 index = indexOfTopLevelItem(item);

	if (index >= 0 && index < topLevelItemCount())
	{
		m_pDataContext->m_VisualPrinting_MeshData.SetCurrentMeshIndex(index);
		selectionUpdate(index);
	}
	else
	{
		selectionUpdate(m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshIndex());
	}
}

void VisualPrintMeshListWidget::mouseReleaseEvent(QMouseEvent* e)
{
	QTreeWidgetItem* item = itemAt(e->pos());

	if (selectedItems().size() <= 1)
		QTreeWidget::mouseReleaseEvent(e);

	if (item == NULL)
	{
		if (m_pDataContext && selectedItems().size() <= 1)
		{
			selectionRefresh(m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshIndex());
			return;
		}
	}
}

void VisualPrintMeshListWidget::keyReleaseEvent(QKeyEvent* e)
{
	if (Qt::Key_F2 == e->key())
	{
		this->clearFocus();
		return;
	}

	// 201008 허건 대리 기존소스코드 주석처리 
	//if (Qt::Key_Enter == e->key())
	//{
	//	editMode = false;
	//}
	//else if (Qt::Key_Delete == e->key())
	//{
	//	if (!editMode)
	//	{
	//		slot_DeleteMeshes();
	//		return;
	//	}
	//}

	QTreeWidget::keyReleaseEvent(e);
}

void VisualPrintMeshListWidget::keyPressEvent(QKeyEvent* e)
{
	if (Qt::Key_F2 == e->key())
	{

		this->clearFocus();
		return;
	}

	// 201008 허건 대리 기존코드 주석처리
	//if (Qt::Key_Down == e->key())
	//{
	//	e->ignore();
	//	return;
	//}
	//else if (Qt::Key_Up == e->key())
	//{
	//	e->ignore();
	//	return;
	//}

	//if (!editMode)
	//{
	//	if (Qt::Key_Right == e->key())
	//	{
	//		e->ignore();
	//		return;
	//	}
	//	else if (Qt::Key_Left == e->key())
	//	{
	//		e->ignore();
	//		return;
	//	}
	//}

	QTreeWidget::keyPressEvent(e);
}

//호출되지 않음
//void VisualPrintMeshListWidget::OnExportOBJFile()
//{
//	if (!m_pDataContext)
//	{
//		return;
//	}
//
//	//	mip::MeshCore *m = m_pDataContext->m_MeshData.GetCurrentMesh();
//	mip::MeshCore* m = m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMesh();
//	//	mint32 layerUID = m_pDataContext->m_MeshData.GetCurrentMeshLUID();
//	mint32 layerUID = m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshLUID();
//
//	if (nullptr == m)
//	{
//		QMessageBox::warning(this, QString("Export error"), QString("The mesh to extract does not exist."));
//		return;
//	}
//
//	if (!WIN_MANAGER->getlicensePass())
//	{
//		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
//		return;
//	}
//
//	QList<QTreeWidgetItem*>& list = selectedItems();
//	QList<QTreeWidgetItem*> Exportlist;
//
//	for (int i = 0; i < list.size(); i++)
//	{
//		QTreeWidgetItem* item = list.at(i);
//
//		if (item->childCount() == 0)
//			item = item->parent();
//
//		muint32 index = indexOfTopLevelItem(item);
//
//		if (!Exportlist.contains(item))
//			Exportlist.push_back(item);
//	}
//
//	MeshInfo* info = NULL;
//
//	if (Exportlist.size() > 1)
//	{
//		WIN_MANAGER->exportList.clear();
//		ACTION_MANAGER->m_eAfterThread = THREAD_EXPORT_MESH_FILES;
//	}
//	else
//	{
//		info = m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshInfo();
//	}
//
//	QFileDialog dlg(this);
//
//	dlg.setFileMode(QFileDialog::DirectoryOnly);
//	QString strOBJ;
//	bool latest = WIN_MANAGER->lastestPathGet(strOBJ, true);
//	if (!latest)
//		strOBJ = "";
//	if (THREAD_EXPORT_MESH_FILES == ACTION_MANAGER->m_eAfterThread)
//	{
//		QString filePath = dlg.getExistingDirectory(this,
//			"Export Mesh File",
//			strOBJ.isEmpty() ? QDir::homePath() : strOBJ); // Options options = 0)
//
//		QFileInfo Finfo(filePath);
//
//		if (Finfo.isDir())
//		{
//			WIN_MANAGER->exportPath = filePath;
//
//			strOBJ = filePath;
//			int index;
//			for (int i = 1; i < Exportlist.size(); i++)
//			{
//				index = indexOfTopLevelItem(Exportlist[i]);
//				info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(index);
//
//				if (info)
//				{
//					WIN_MANAGER->exportList.push_back(index);
//				}
//			}
//			index = indexOfTopLevelItem(Exportlist[0]);
//			info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(index);
//
//			strOBJ.append("/");
//			strOBJ.append(QString("%1.").arg(m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(index)));
//
//			bool r = WIN_MANAGER->SaveMeshFiles(strOBJ, index, info->uid, EX_FILES_OBJ);
//
//			if (r == false)
//			{
//				QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_OBJ),
//					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_OBJ));
//			}
//		}
//	}
//	else if (info)
//	{
//		if (latest)
//		{
//			//			strOBJ += "/" + m_pDataContext->m_MeshData.GetCurrentMeshName();
//			strOBJ += "/" + m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshName();
//		}
//
//		const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
//		QString selectedFilter;
//		QString fileName = QFileDialog::getSaveFileName(this,
//			"Export Mesh File",
//			strOBJ.isEmpty() ? QDir::homePath() : strOBJ, // const QString & dir = QString(),
//			tr("Object File(*.obj;*.OBJ)"), //const QString & filter = QString()
//			&selectedFilter, // QString * selectedFilter = 0,
//			options); // Options options = 0)
//
//		if (!fileName.isEmpty())
//		{
//			bool r = WIN_MANAGER->SaveMeshFiles(fileName, m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshIndex(), info->uid, EX_FILES_OBJ, false);
//
//			if (r == false)
//			{
//				QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_OBJ),
//					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_OBJ));
//			}
//		}
//	}
//}
//
////호출되지 않음
//void VisualPrintMeshListWidget::OnExportVTKFile()
//{
//	if (!m_pDataContext)
//	{
//		return;
//	}
//
//	//	mip::MeshCore *m = m_pDataContext->m_MeshData.GetCurrentMesh();
//	mip::MeshCore* m = m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMesh();
//	//	mint32 layerUID = m_pDataContext->m_MeshData.GetCurrentMeshLUID();
//	mint32 layerUID = m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshLUID();
//
//	if (nullptr == m)
//	{
//		QMessageBox::warning(this, QString("Export error"), QString("The mesh to extract does not exist."));
//		return;
//	}
//
//	if (!WIN_MANAGER->getlicensePass())
//	{
//		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
//		return;
//	}
//
//	QList<QTreeWidgetItem*>& list = selectedItems();
//	QList<QTreeWidgetItem*> Exportlist;
//
//	for (int i = 0; i < list.size(); i++)
//	{
//		QTreeWidgetItem* item = list.at(i);
//
//		if (item->childCount() == 0)
//			item = item->parent();
//
//		muint32 index = indexOfTopLevelItem(item);
//
//		if (!Exportlist.contains(item))
//			Exportlist.push_back(item);
//	}
//
//	MeshInfo* info = NULL;
//
//	if (Exportlist.size() > 1)
//	{
//		WIN_MANAGER->exportList.clear();
//		ACTION_MANAGER->m_eAfterThread = THREAD_EXPORT_MESH_FILES;
//	}
//	else
//	{
//		info = m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshInfo();
//	}
//
//	QFileDialog dlg(this);
//
//	dlg.setFileMode(QFileDialog::DirectoryOnly);
//	QString strVTK;
//	bool latest = WIN_MANAGER->lastestPathGet(strVTK, true);
//	if (!latest)
//		strVTK = "";
//	if (THREAD_EXPORT_MESH_FILES == ACTION_MANAGER->m_eAfterThread)
//	{
//		QString filePath = dlg.getExistingDirectory(this,
//			"Export Mesh File",
//			strVTK.isEmpty() ? QDir::homePath() : strVTK); // Options options = 0)
//
//		QFileInfo Finfo(filePath);
//
//		if (Finfo.isDir())
//		{
//			WIN_MANAGER->exportPath = filePath;
//
//			strVTK = filePath;
//			int index;
//			for (int i = 1; i < Exportlist.size(); i++)
//			{
//				index = indexOfTopLevelItem(Exportlist[i]);
//				info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(index);
//
//				if (info)
//				{
//					WIN_MANAGER->exportList.push_back(index);
//				}
//			}
//			index = indexOfTopLevelItem(Exportlist[0]);
//			info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(index);
//
//			strVTK.append("/");
//			strVTK.append(QString("%1.").arg(m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(index)));
//
//			bool r = WIN_MANAGER->SaveMeshFiles(strVTK, index, info->uid, EX_FILES_VTK);
//
//			if (r == false)
//			{
//				QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_VTK),
//					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_VTK));
//			}
//		}
//	}
//	else if (info)
//	{
//		if (latest)
//		{
//			//			strVTK += "/" + m_pDataContext->m_MeshData.GetCurrentMeshName();
//			strVTK += "/" + m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshName();
//		}
//
//		const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
//		QString selectedFilter;
//		QString fileName = QFileDialog::getSaveFileName(this,
//			"Export Mesh File",
//			strVTK.isEmpty() ? QDir::homePath() : strVTK, // const QString & dir = QString(),
//			tr("VTK File(*.vtk;*.VTK)"), //const QString & filter = QString()
//			&selectedFilter, // QString * selectedFilter = 0,
//			options); // Options options = 0)
//
//		if (!fileName.isEmpty())
//		{
//			bool r = WIN_MANAGER->SaveMeshFiles(fileName, m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshIndex(), info->uid, EX_FILES_VTK, false);
//
//			if (r == false)
//			{
//				QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_VTK),
//					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_VTK));
//			}
//		}
//	}
//}

void VisualPrintMeshListWidget::slot_ShiftPosition()
{
	if (!m_pDataContext)
	{
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();

	if (list.size() != 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT) +
			QString("shift position"));
		return;
	}

	muint32 index = 0;
	QTreeWidgetItem* item = list.at(0);
	if (item->childCount() == 0)
		item = item->parent();

	index = indexOfTopLevelItem(item);

	//if (WIN_MANAGER->mainMeshWidget)
	{
		//MEVolumeView* view = WIN_MANAGER->mainMeshWidget->getMainView();
		//if (view)
		ACTION_MANAGER->action_MeshList_align_change(m_pDataContext, index);
	}
}

void VisualPrintMeshListWidget::OnRename()
{
	QList<QTreeWidgetItem*>& list = selectedItems();

	if (list.size() != 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT) +
			STRING_MANAGER->getString(STR_RENAME));
		return;
	}

	muint32 index = 0;
	QTreeWidgetItem* item = list.at(0);
	if (item->childCount() == 0)
		item = item->parent();

	index = indexOfTopLevelItem(item);

	editMode = true;
	WIN_MANAGER->setMoveFocus(false);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	editItem(item, M_COL_NAME);
}

//void VisualPrintMeshListWidget::slot_DeleteMeshes()
//{
//	if (!m_pDataContext)
//	{
//		return;
//	}
//
//	QList<QTreeWidgetItem*>& list = selectedItems();
//	QList<QTreeWidgetItem*> Dellist;
//
//	for (int i = 0; i < list.size(); i++)
//	{
//		QTreeWidgetItem* item = list.at(i);
//
//		if (item->childCount() == 0)
//			item = item->parent();
//
//		muint32 index = indexOfTopLevelItem(item);
//
//		if (!Dellist.contains(item))
//			Dellist.push_back(item);
//	}
//
//	if (Dellist.size() > m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount())
//	{
//		//		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), QString("Only %1 or fewer layers can be deleted.").
//		//			arg(m_pDataContext->m_MeshData.GetMeshCount()));
//		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), QString("Only %1 or fewer layers can be deleted.").
//			arg(m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount()));
//		return;
//	}
//
//	if (Dellist.size() == 1)
//	{
//		mint8 index = indexOfTopLevelItem(Dellist.at(0));
//
//		MeshInfo* info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(index);
//
//		if (info)
//		{
//			mint32 lUID = info->uid;
//
//			ACTION_MANAGER->action_MeshList_delete(m_pDataContext, index, lUID);
//		}
//	}
//	else if (Dellist.size() > 1)
//	{
//		std::vector<mint8> indeces;
//		for (int cnt = 0; cnt < Dellist.size(); cnt++)
//		{
//			indeces.push_back(indexOfTopLevelItem(Dellist[cnt]));
//		}
//
//		ACTION_MANAGER->action_MeshList_delete(m_pDataContext, indeces);
//	}
//}

//void VisualPrintMeshListWidget::slot_OnDiff()
//{
//	QList<QTreeWidgetItem*> list = selectedItems();
//	QList<muint32> diffList;
//
//	for (int i = 0; i < list.size(); i++)
//	{
//		QTreeWidgetItem* item = list.at(i);
//
//		if (item->childCount() == 0)
//			item = item->parent();
//
//		muint32 index = indexOfTopLevelItem(item);
//
//		if (!diffList.contains(index))
//			diffList.push_back(index);
//	}
//
//	if (diffList.size() != 2)
//	{
//		QMessageBox::warning(this, QString("Mesh Diff"), STRING_MANAGER->getString(STR_SELECT_2_WARN));
//		return;
//	}
//
//	//ACTION_MANAGER->action_Mesh_Boolean(BOT_DIFF, diffList.at(0), diffList.at(1));
//}
//
//void VisualPrintMeshListWidget::slot_OnIntersect()
//{
//	QList<QTreeWidgetItem*> list = selectedItems();
//	QList<muint32> sectList;
//
//	for (int i = 0; i < list.size(); i++)
//	{
//		QTreeWidgetItem* item = list.at(i);
//
//		if (item->childCount() == 0)
//			item = item->parent();
//
//		muint32 index = indexOfTopLevelItem(item);
//
//		if (!sectList.contains(index))
//			sectList.push_back(index);
//	}
//
//	if (sectList.size() != 2)
//	{
//		QMessageBox::warning(this, QString("Mesh Intersect"), STRING_MANAGER->getString(STR_SELECT_2_WARN));
//		return;
//	}
//
//	//ACTION_MANAGER->action_Mesh_Boolean(BOT_INTERSECT, sectList.at(0), sectList.at(1));
//}
//
//void VisualPrintMeshListWidget::slot_OnUnion()
//{
//	QList<QTreeWidgetItem*> list = selectedItems();
//	QList<muint32> unionList;
//
//	for (int i = 0; i < list.size(); i++)
//	{
//		QTreeWidgetItem* item = list.at(i);
//
//		if (item->childCount() == 0)
//			item = item->parent();
//
//		muint32 index = indexOfTopLevelItem(item);
//
//		if (!unionList.contains(index))
//			unionList.push_back(index);
//	}
//
//	if (unionList.size() != 2)
//	{
//		QMessageBox::warning(this, QString("Mesh Union"), STRING_MANAGER->getString(STR_SELECT_2_WARN));
//		return;
//	}
//
//	//ACTION_MANAGER->action_Mesh_Boolean(BOT_UNION, unionList.at(0), unionList.at(1));
//}
//
//void VisualPrintMeshListWidget::slot_Solid()
//{
//	if (!m_pDataContext)
//	{
//		return;
//	}
//
//	QList<QTreeWidgetItem*> list = selectedItems();
//	QList<muint32> SolidList;
//
//	for (int i = 0; i < list.size(); i++)
//	{
//		QTreeWidgetItem* item = list.at(i);
//
//		if (item->childCount() == 0)
//			item = item->parent();
//
//		muint32 index = indexOfTopLevelItem(item);
//
//		if (!SolidList.contains(index))
//			SolidList.push_back(index);
//	}
//
//	if (SolidList.size() != 1)
//	{
//		QMessageBox::warning(this, QString("Solid mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
//		return;
//	}
//
//	//	OffsetDlg dlg(QString("Solid Mesh [%1]").arg(m_pDataContext->m_MeshData.GetMeshName(SolidList.at(0))),
//	//		QString("Offset :"), 0, 0.01, -1.0, 1.0, this);
//	OffsetDlg dlg(QString("Solid Mesh [%1]").arg(m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(SolidList.at(0))),
//		QString("Offset :"), 0, 0.01, -1.0, 1.0, this);
//
//	dlg.exec();
//
//	if (dlg.isAccept())
//	{
//		float offset = dlg.getDoubleVal();
//
//		ACTION_MANAGER->action_Mesh_Remesh(MESH_SOLID, offset, SolidList.at(0), m_pDataContext, m_pViewer);
//	}
//}
//
//void VisualPrintMeshListWidget::slot_Hollow()
//{
//	QList<QTreeWidgetItem*> list = selectedItems();
//	QList<muint32> HollowList;
//
//	for (int i = 0; i < list.size(); i++)
//	{
//		QTreeWidgetItem* item = list.at(i);
//
//		if (item->childCount() == 0)
//			item = item->parent();
//
//		muint32 index = indexOfTopLevelItem(item);
//
//		if (!HollowList.contains(index))
//			HollowList.push_back(index);
//	}
//
//	if (HollowList.size() != 1)
//	{
//		QMessageBox::warning(this, QString("Hollow mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
//		return;
//	}
//
//	//	OffsetDlg dlg(QString("Hollow Mesh [%1]").arg(m_pDataContext->m_MeshData.GetMeshName(HollowList.at(0))),
//	//		QString("Offset :"), -0.5, 0.01, -1.0, -0.01, this);
//	OffsetDlg dlg(QString("Hollow Mesh [%1]").arg(m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(HollowList.at(0))),
//		QString("Offset :"), -0.5, 0.01, -1.0, -0.01, this);
//
//	dlg.exec();
//
//	if (dlg.isAccept())
//	{
//		float offset = dlg.getDoubleVal();
//
//		ACTION_MANAGER->action_Mesh_Remesh(m_pDataContext, MESH_HOLLOW, offset, HollowList.at(0));
//	}
//
//}
//
//void VisualPrintMeshListWidget::slot_Remesh()
//{
//	QList<QTreeWidgetItem*> list = selectedItems();
//	QList<muint32> RemeshList;
//
//	for (int i = 0; i < list.size(); i++)
//	{
//		QTreeWidgetItem* item = list.at(i);
//
//		if (item->childCount() == 0)
//			item = item->parent();
//
//		muint32 index = indexOfTopLevelItem(item);
//
//		if (!RemeshList.contains(index))
//			RemeshList.push_back(index);
//	}
//
//	if (RemeshList.size() != 1)
//	{
//		QMessageBox::warning(this, QString("Remesh mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
//		return;
//	}
//
//	//	OffsetDlg dlg(QString("Remesh Mesh [%1]").arg(m_pDataContext->m_MeshData.GetMeshName(RemeshList.at(0))),
//	//		QString("Level :"), 1, 1.0, 1, 4, this);
//	OffsetDlg dlg(QString("Remesh Mesh [%1]").arg(m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(RemeshList.at(0))),
//		QString("Level :"), 1, 1.0, 1, 4, this);
//
//	dlg.exec();
//
//	if (dlg.isAccept())
//	{
//		int offset = dlg.getIntVal();
//
//		ACTION_MANAGER->action_Mesh_Remesh(m_pDataContext, MESH_REMESH, offset, RemeshList.at(0));
//	}
//
//}
//
//void VisualPrintMeshListWidget::slot_Smooth()
//{
//	QList<QTreeWidgetItem*> list = selectedItems();
//	QList<muint32> SmoothList;
//
//	for (int i = 0; i < list.size(); i++)
//	{
//		QTreeWidgetItem* item = list.at(i);
//
//		if (item->childCount() == 0)
//			item = item->parent();
//
//		muint32 index = indexOfTopLevelItem(item);
//
//		if (!SmoothList.contains(index))
//			SmoothList.push_back(index);
//	}
//
//	if (SmoothList.size() != 1)
//	{
//		QMessageBox::warning(this, QString("Smooth mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
//		return;
//	}
//
//	//	OffsetDlg dlg(QString("Smooth Mesh [%1]").arg(m_pDataContext->m_MeshData.GetMeshName(SmoothList.at(0))),
//	//		QString("Level :"), 0.1, 0.1, 0.1, 10.f, this);
//	OffsetDlg dlg(QString("Smooth Mesh [%1]").arg(m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(SmoothList.at(0))),
//		QString("Level :"), 0.1, 0.1, 0.1, 10.f, this);
//
//	dlg.exec();
//
//	if (dlg.isAccept())
//	{
//		float offset = dlg.getDoubleVal();
//
//		ACTION_MANAGER->action_Mesh_Remesh(m_pDataContext, MESH_SMOOTH, offset, SmoothList.at(0));
//	}
//
//}
//
//void VisualPrintMeshListWidget::slot_Reduction()
//{
//	QList<QTreeWidgetItem*> list = selectedItems();
//	QList<muint32> ReducList;
//
//	for (int i = 0; i < list.size(); i++)
//	{
//		QTreeWidgetItem* item = list.at(i);
//
//		if (item->childCount() == 0)
//			item = item->parent();
//
//		muint32 index = indexOfTopLevelItem(item);
//
//		if (!ReducList.contains(index))
//			ReducList.push_back(index);
//	}
//
//	if (ReducList.size() != 1)
//	{
//		QMessageBox::warning(this, QString("Reduct mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
//		return;
//	}
//
//	//	OffsetDlg dlg(QString("Reduct Mesh [%1]").arg(m_pDataContext->m_MeshData.GetMeshName(ReducList.at(0))),
//	//		QString("Reduction :"), 1, 1.0, 1, 100, this);
//	OffsetDlg dlg(QString("Reduct Mesh [%1]").arg(m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(ReducList.at(0))),
//		QString("Reduction :"), 1, 1.0, 1, 100, this);
//
//	dlg.exec();
//
//	if (dlg.isAccept())
//	{
//		int offset = dlg.getIntVal();
//
//		ACTION_MANAGER->action_Mesh_Remesh(m_pDataContext, MESH_REDUCTION, offset, ReducList.at(0));
//	}
//}

//호출되지 않음
//void VisualPrintMeshListWidget::OnExportSTLFile()
//{
//	//	mip::MeshCore *m = m_pDataContext->m_MeshData.GetCurrentMesh();
//	mip::MeshCore* m = m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMesh();
//	//	mint32 layerUID = m_pDataContext->m_MeshData.GetCurrentMeshLUID();
//	mint32 layerUID = m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshLUID();
//
//	if (nullptr == m)
//	{
//		QMessageBox::warning(this, QString("Export error"), QString("The mesh to extract does not exist."));
//		return;
//	}
//
//	if (!WIN_MANAGER->getlicensePass())
//	{
//		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
//		return;
//	}
//
//	QList<QTreeWidgetItem*>& list = selectedItems();
//	QList<QTreeWidgetItem*> Exportlist;
//
//	for (int i = 0; i < list.size(); i++)
//	{
//		QTreeWidgetItem* item = list.at(i);
//
//		if (item->childCount() == 0)
//			item = item->parent();
//
//		muint32 index = indexOfTopLevelItem(item);
//
//		if (!Exportlist.contains(item))
//			Exportlist.push_back(item);
//	}
//
//	MeshInfo* info = NULL;
//
//	if (Exportlist.size() > 1)
//	{
//		WIN_MANAGER->exportList.clear();
//		ACTION_MANAGER->m_eAfterThread = THREAD_EXPORT_MESH_FILES;
//	}
//	else
//	{
//		//		info = m_pDataContext->m_MeshData.GetCurrentMeshInfo();
//		info = m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshInfo();
//	}
//
//	QFileDialog dlg(this);
//
//	dlg.setFileMode(QFileDialog::DirectoryOnly);
//	QString strSTL;
//	bool latest = WIN_MANAGER->lastestPathGet(strSTL, true);
//	if (!latest)
//		strSTL = "";
//	if (THREAD_EXPORT_MESH_FILES == ACTION_MANAGER->m_eAfterThread)
//	{
//		QString filePath = dlg.getExistingDirectory(this,
//			"Export Mesh File",
//			strSTL.isEmpty() ? QDir::homePath() : strSTL); // Options options = 0)
//
//		QFileInfo Finfo(filePath);
//
//		if (Finfo.isDir())
//		{
//			WIN_MANAGER->exportPath = filePath;
//
//			strSTL = filePath;
//			int index;
//			for (int i = 1; i < Exportlist.size(); i++)
//			{
//				index = indexOfTopLevelItem(Exportlist[i]);
//				info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(index);
//
//				if (info)
//				{
//					WIN_MANAGER->exportList.push_back(index);
//				}
//			}
//			index = indexOfTopLevelItem(Exportlist[0]);
//			info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(index);
//
//			strSTL.append("/");
//			strSTL.append(QString("%1.").arg(m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(index)));
//
//			bool r = WIN_MANAGER->SaveMeshFiles(strSTL, index, info->uid, EX_FILES_STL);
//
//			if (r == false)
//			{
//				QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_STL),
//					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_STL));
//			}
//		}
//	}
//	else if (info)
//	{
//		if (latest)
//		{
//			//			strSTL += "/" + m_pDataContext->m_MeshData.GetCurrentMeshName();
//			strSTL += "/" + m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshName();
//		}
//
//		const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
//		QString selectedFilter;
//		QString fileName = QFileDialog::getSaveFileName(this,
//			"Export Mesh File",
//			strSTL.isEmpty() ? QDir::homePath() : strSTL, // const QString & dir = QString(),
//			tr("STL File(*.stl;*.STL)"), //const QString & filter = QString()
//			&selectedFilter, // QString * selectedFilter = 0,
//			options); // Options options = 0)
//
//		if (!fileName.isEmpty())
//		{
//			bool r = WIN_MANAGER->SaveMeshFiles(fileName, m_pDataContext->m_VisualPrinting_MeshData.GetCurrentMeshIndex(), info->uid, EX_FILES_STL, false);
//
//			if (r == false)
//			{
//				QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_STL),
//					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_STL));
//			}
//		}
//	}
//
//#if 0
//	QString strMesh;
//	bool latest = WIN_MANAGER->lastestPathGet(strMesh, true);
//
//	if (!latest)
//		strMesh = "";
//	else
//		strMesh.append(QString("/%1").arg(m_pDataContext->m_MeshData.GetCurrentMeshName()));
//
//	const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
//	QString selectedFilter;
//	QString fileName = QFileDialog::getSaveFileName(this,
//		"Export Mesh File",
//		strMesh.isEmpty() ? QDir::homePath() : strMesh, // const QString & dir = QString(),
//		tr("STL File(*.stl;*.STL)"), //const QString & filter = QString()
//		&selectedFilter, // QString * selectedFilter = 0,
//		options); // Options options = 0)
//
//	if (!fileName.isEmpty())
//	{
//		QString ext = fileName.section('.', -1);
//
//		if (ext.compare(QString("stl"), Qt::CaseInsensitive) == 0)
//		{
//			mip::model::SaveSTLFile(fileName.toLocal8Bit().constData(),
//				m, layerUID != -1 ? 10.0f : 1.0f, true, false);
//		}
//
//	}
//#endif
//}
///////////////////////////////////////////////////////////////////////////////////////////
// Visual Print MeshListWidget Class Member Functions - End
///////////////////////////////////////////////////////////////////////////////////////////