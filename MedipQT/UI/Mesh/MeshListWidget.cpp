#include "stdafx.h"
#include "MeshListWidget.h"
#include "windowManager.h"
#include "ProductManager.h"
#include "ResourceManager.h"
#include "StringManager.h"
#include "StyleManager.h"
#include "Omniverse/OmniverseContext.h"
#include "Omniverse/Usd/mipUsdStage.h"
#include "MeshControl.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CManipulator.h"
#include "MeshEdit/CMeshManipulator.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CMeshDlgManager.h"
#include "Windows/Main/MainMeshWidget.h"
#include "MEVolumeView.h"
#include "FileManager.h"
#include "DrawcutDlg.h"
#include "DataContext.h"
#include "Actions/Omniverse/ActionReceiveFromOmniverse.h"
#include "Actions/Omniverse/ActionSendToOmniverse.h"

#define TEMP_CODE

static std::vector<MeshActionInfo> MESH_ACTION_INFO_LIST =
{
	{eMeshActionType::Export, "Export as...", MFL_Common_MeshEditing_FileExport, nullptr,
		{
			{eMeshActionType::Export_STL, "Stereolithography (.stl, current layer)", MFL_Common_MeshEditing_FileExport, &MeshListWidget::slot_OnExportSTLFile, {}},
			{eMeshActionType::Export_Obj, "Object (.obj, current layer)", MFL_Common_MeshEditing_FileExport, &MeshListWidget::slot_OnExportOBJFile, {}},
			{eMeshActionType::Export_VTK, "Visualization Toolkit (.vtk, current layer)", MFL_Common_MeshEditing_FileExport, &MeshListWidget::slot_OnExportVTKFile, {}},
			{eMeshActionType::Export_3MF, "3D Manufacturing Format (.3mf, current layer)", MFL_Common_MeshEditing_FileExport, &MeshListWidget::slot_OnExport3MFFile, {}},
			{eMeshActionType::Export_USD, "USD Format (.usd, current layer)", MFL_DEV_NvidiaOmniverse, &MeshListWidget::slot_ExportUSD, {}},
		}
	},
	{eMeshActionType::Transform, "Transform...", MFL_Common_MeshEditing_Transform, nullptr,
		{
			{eMeshActionType::Transform_Image_Plus, "Image Matrix (+)", MFL_Common_MeshEditing_Transform, &MeshListWidget::slot_ImageMatrixPlus, {}},
			{eMeshActionType::Transform_Image_Minus, "Image Matrix (-)", MFL_Common_MeshEditing_Transform, &MeshListWidget::slot_ImageMatrixMinus, {}},
		}
	},
	{eMeshActionType::Boolean, "Boolean action", MFL_Common_MeshEditing_Boolean, nullptr,
		{
			{eMeshActionType::Boolean_Difference, "Difference", MFL_Common_MeshEditing_Boolean, &MeshListWidget::slot_OnDiff, {}},
			{eMeshActionType::Boolean_Intersection, "Intersection", MFL_Common_MeshEditing_Boolean, &MeshListWidget::slot_OnIntersect, {}},
			{eMeshActionType::Boolean_Union, "Union", MFL_Common_MeshEditing_Boolean, &MeshListWidget::slot_OnUnion, {}},
		}
	},
	{eMeshActionType::Duplicate, "Duplicate", MFL_Common_MeshEditing_Duplicate , &MeshListWidget::slot_OnDuplicate, {}},
	{eMeshActionType::Attatch, "Attatch", MFL_Common_MeshEditing_Attach , &MeshListWidget::slot_Attach, {}},
	{eMeshActionType::Mesh2Mask, "Mesh to Mask", MFL_Common_MeshEditing_Mesh2Mask , &MeshListWidget::slot_Mesh2Mask, {}},
	{eMeshActionType::Solid, "Solid", MFL_Common_MeshEditing_Solid , &MeshListWidget::slot_Solid, {}},
	{eMeshActionType::Hollow, "Hollow", MFL_Common_MeshEditing_Hollow , &MeshListWidget::slot_Hollow, {}},
	{eMeshActionType::Remesh, "Remesh", MFL_Common_MeshEditing_Remesh , &MeshListWidget::slot_Remesh, {}},
	{eMeshActionType::Reduction, "Reduction", MFL_Common_MeshEditing_Reduce , &MeshListWidget::slot_Reduction, {}},
	{eMeshActionType::Smooth, "Smooth", MFL_Common_MeshEditing_Smooth , &MeshListWidget::slot_Smooth, {}},
	{eMeshActionType::Subdivision, "Subdivision", MFL_Common_MeshEditing_Subdivision , &MeshListWidget::slot_SubDivision, {}},
	{eMeshActionType::IslandFilter, "Island-Filter", MFL_Common_MeshEditing_IslandFilter , &MeshListWidget::slot_IslandFilter, {}},
	{eMeshActionType::SendToOmniverse, "Send to Omniverse", MFL_DEV_NvidiaOmniverse , &MeshListWidget::slot_SendToOmniverse, {}},
	{eMeshActionType::ReceiveFromOmniverse, "Receive From Omniverse", MFL_DEV_NvidiaOmniverse , &MeshListWidget::slot_ReceiveFromOmniverse, {}},
#if SUPPORT_MESHOFFSET == 1
	{eMeshActionType::MeshOffset, "Mesh Offset", MFL_MESH_EDITING_MESHOFFSET },
#endif
};

MeshListWidget::MeshListWidget(
	std::shared_ptr<MeshListColumnList> pColumnList,
	WindowManager* pWinManager,
	ActionManager* pActionManager,
	ProductManager* pProductManager,
	DataContext* pDataContext,
	CMeshModelViewManager* pMeshModelViewManager,
	CPlaneManiplator* pPlaneManipulator,
	CMeshManipulator* pMeshManipulator,
	ShortcutManager* pShortcutManager,
	QWidget* parent) :
	QTreeWidget(parent),
	m_pColumnList(pColumnList),
	m_pWinManager(pWinManager),
	m_pActionManager(pActionManager),
	m_pProductManager(pProductManager),
	m_pDataContext(pDataContext),
	m_pOmniverse(nullptr),
	m_pMeshModelViewManager(pMeshModelViewManager),
	m_pPlaneManipulator(pPlaneManipulator),
	m_pMeshManipulator(pMeshManipulator),
	m_pShortcutManager(pShortcutManager),
	m_isEditMode(false)
{
	setContextMenuPolicy(Qt::DefaultContextMenu);
	setAnimated(true);

	setRootIsDecorated(false);
	//setAcceptDrops(true);
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragDrop);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	m_brushes = { QBrush(QColor(48, 48, 48, 255)) , QBrush(QColor(200, 200, 200, 255)) };
	m_visibleIcons = { RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE), RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE) };

	if (m_pDataContext)
	{
		m_pOmniverse = m_pDataContext->GetOmniverseContext();
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetCurrentMesh();
		m_pPlaneManipulator->UpdatePosition(mesh);
		connect(m_pDataContext, &DataContext::sig_meshInfoChanged, this, &MeshListWidget::slot_meshInfoChanged);
	}

	connect(m_pOmniverse->GetStage(), &OmniverseStage::sig_StageDataChanged, this, &MeshListWidget::slot_OmniverseStageChanged);
	connect(m_pOmniverse->GetStage(), &OmniverseStage::sig_StagePresetChanged, this, &MeshListWidget::slot_OmniverseStagePresetChanged);
}

MeshListWidget::~MeshListWidget()
{
}

bool MeshListWidget::Init()
{
	header()->setMinimumSectionSize(20);

	setColumnCount(m_pColumnList->GetColumnCount());
	for (auto& pCol : m_pColumnList->GetList())
	{
		if (pCol->HasColumnWidth())
		{
			setColumnWidth(pCol->GetIndex(), pCol->GetColumnWidth());
		}
	}

	setHeaderHidden(true);
	setStyleSheet(STYLE_MANAGER->treeBasicList);
	setAutoFillBackground(true);
	setContentsMargins(0, 0, 0, 0);

	return true;
}

QSize MeshListWidget::sizeForRow()
{
	return QSize(sizeHintForRow(0), sizeHintForRow(0));
}

void MeshListWidget::UpdateSubwidget(muint8 UID)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (UID >= m_pDataContext->m_MeshData.GetMeshCount())
	{
		return;
	}
}

void MeshListWidget::ChangeSubUID(muint8 newUID)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (newUID >= m_pDataContext->m_MeshData.GetMeshCount())
	{
		return;
	}
}

void MeshListWidget::Update()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	clear();

	muint32 meshCount = m_pDataContext->m_MeshData.GetMeshCount();

	setItemsExpandable(false);

	MeshInfo* pInfo = nullptr;

	for (int i = 0; i < meshCount; ++i)
	{
		pInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (pInfo)
		{
			QTreeWidgetItem* item = new QTreeWidgetItem(this);

			setItemByMeshInfo(item, *pInfo);

			insertTopLevelItem(0, item);

			QTreeWidgetItem* subItem = new QTreeWidgetItem(item);
			subItem->setFirstColumnSpanned(true);
			item->addChild(subItem);
			item->setExpanded(false);
		}
	}

	if (meshCount > 0)
	{
		QTreeWidgetItem* item = topLevelItem(m_pDataContext->m_MeshData.GetCurrentMeshIndex());

		if (nullptr == item)
		{
			m_pDataContext->m_MeshData.SetCurrentMeshIndex(0);
			item = topLevelItem(0);
		}

		if (item)
		{
			setItemBackgroundColor(item, getBrush_Selected());
			setItemSelected(item, true);
			setCurrentItem(item);
		}
	}
}

void MeshListWidget::updateSize(bool expand)
{
	const static QSize minSize = minimumSizeHint();

	if (expand)
	{
		setFixedHeight(sizeForRow().height() * (MESH_MAX + 1));
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else
	{
		setMinimumHeight(minSize.height());
		setMaximumHeight(sizeForRow().height() * (MESH_MAX + 1));
		setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
}

void MeshListWidget::SetAllColumnVisible(bool show)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	bool changed = false;
	for (int i = 0; i < m_pDataContext->m_MeshData.GetMeshCount(); i++)
	{
		MeshInfo* pInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (pInfo)
		{
			if (show != pInfo->show)
			{
				changed = true;
				pInfo->show = show;

				QTreeWidgetItem* item = topLevelItem(i);
				setItemByMeshInfo(item, *pInfo);
			}
		}
	}

	if (changed)
	{
		m_pActionManager->SendActionProcessFinished(ACTION_PROCESSING::ACTP_MESH_LAYER_VISIBILITY_CHANGED);
		m_pWinManager->renderLater_All();
	}
}

void MeshListWidget::SetAllColumnLock(eOmniverseStatus status)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	bool changed = false;
	for (int i = 0; i < m_pDataContext->m_MeshData.GetMeshCount(); i++)
	{
		MeshInfo* pInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (pInfo)
		{
			if (status != pInfo->GetOmniverseStatus())
			{
				changed = true;
				pInfo->SetOmniverseStatus(status);

				QTreeWidgetItem* item = topLevelItem(i);
				setItemByMeshInfo(item, *pInfo);
			}
		}
	}

	if (changed)
	{
		m_pActionManager->SendActionProcessFinished(ACTION_PROCESSING::ACTP_MESH_LAYER_LOCK_CHANGED);
		m_pWinManager->renderLater_All();
	}
}

void MeshListWidget::SelectionUpdate(int index_dest, bool b_close_dlg)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (b_close_dlg)
	{
		m_pWinManager->rejectMEViewDlg(m_pDataContext->m_MeshData.GetCurrentMeshIndex());
	}

	m_pDataContext->m_MeshData.ClearMeshInfoSelectMode();

	// Check 0 selected
	int count = 0;
	for (int i = 0; i < topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = topLevelItem(i);

		if (!item->isSelected())
		{
			count++;
		}
	}

	if (count != topLevelItemCount())
	{
		for (int i = 0; i < topLevelItemCount(); i++)
		{
			QTreeWidgetItem* item = topLevelItem(i);

			bool isSelected = item->isSelected();

			setItemBackgroundColor(item, isSelected ? getBrush_Selected() : getBrush_Unselected());

			m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(i, isSelected);

			item->setSelected(isSelected);
			setItemSelected(item, isSelected);
		}

		m_pWinManager->renderLater_All();
	}
	else
	{
		QTreeWidgetItem* item = topLevelItem(index_dest);

		item->setSelected(true);
		setItemSelected(item, true);

		m_pWinManager->updatePckModel(index_dest);
	}

	if (currentItem() == nullptr)
	{
		setCurrentItem(topLevelItem(index_dest));
	}

	//20210810_byPHS_레이어찍을시용량변경
	if (m_pWinManager->mainMeshWidget != nullptr)
	{
		MEVolumeView* view = m_pWinManager->mainMeshWidget->getMainView();
		view->updateGeometryCount(index_dest);
		view->update();
	}
}

void MeshListWidget::SelectionRefresh(int selectIndex, bool b_clear, bool b_close_dlg)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	muint32 meshCount = m_pDataContext->m_MeshData.GetMeshCount();
	QTreeWidgetItem* item = topLevelItem(selectIndex);
	MeshInfo* pSelectMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(selectIndex);
	if (nullptr == pSelectMeshInfo)
	{
		return;
	}

	if (nullptr == item)
	{
		setItemsExpandable(false);
		item = new QTreeWidgetItem(this);

		insertTopLevelItem(selectIndex, item);
		QTreeWidgetItem* sub = new QTreeWidgetItem(item);
		sub->setFirstColumnSpanned(true);
		item->addChild(sub);
		item->setExpanded(false);
	}

	setItemByMeshInfo(item, *pSelectMeshInfo);

	if (meshCount > 0)
	{
		if (b_clear)
		{
			clearSelection();
		}

		if (item)
		{
			setCurrentItem(item);
			setItemBackgroundColor(item, getBrush_Selected());

			item->setSelected(true);
			setItemSelected(item, true);
			scrollToItem(item);
			SelectionUpdate(selectIndex, b_close_dlg);
		}
		m_pDataContext->m_MeshData.SetCurrentMeshIndex(selectIndex);
	}
}


void MeshListWidget::contextMenuEvent(QContextMenuEvent* e)
{
	if (m_isEditMode)
	{
		return;
	}

	std::shared_ptr<QMenu> pMenu = Get_QMenu();
	pMenu->setStyleSheet(STYLE_MANAGER->m_MenuMaskROI);
	pMenu->exec(e->globalPos());
}

void MeshListWidget::dropEvent(QDropEvent* e)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (e->source() != this && !(e->possibleActions() & Qt::MoveAction) || !m_pActionManager->isActionFinished())
	{
		return;
	}

	QTreeWidgetItem* target = itemAt(e->pos());

	if (target)
	{
		mint32 index_dest = indexOfTopLevelItem(target);
		mint32 index_source = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		if (index_dest < 0)
		{
			index_dest = 0;
		}
		else if (index_dest >= m_pDataContext->m_MeshData.GetMeshCount())
		{
			index_dest = m_pDataContext->m_MeshData.GetMeshCount() - 1;
		}

		if (index_dest == index_source || (index_source >= m_pDataContext->m_MeshData.GetMeshCount()))
		{
			return;
		}

		m_pActionManager->action_MeshList_UID_change(m_pDataContext, index_source, index_dest);

		m_pMeshManipulator->indexChange(index_source, index_dest);
	}
}

void MeshListWidget::mousePressEvent(QMouseEvent* e)
{
	QTreeWidget::mousePressEvent(e);

	if (m_pDataContext == nullptr)
	{
		return;
	}

	QTreeWidgetItem* item = itemAt(e->pos());

	if (item == nullptr || (Qt::NoModifier == e->modifiers() && (e->button() != Qt::RightButton)))
	{
		mint32 index = indexOfTopLevelItem(item);

		if (index >= 0 && index < topLevelItemCount())
		{
			SelectionRefresh(index);
			m_pDataContext->m_MeshData.SetCurrentMeshIndex(index);
		}
		else
		{
			SelectionRefresh(m_pDataContext->m_MeshData.GetCurrentMeshIndex());
		}

		m_pWinManager->updatePckModel(m_pDataContext->m_MeshData.GetCurrentMeshIndex());

		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetCurrentMesh();

		m_pPlaneManipulator->UpdatePosition(mesh);

		return;
	}

	mint32 index = indexOfTopLevelItem(item);

	if (index >= 0 && index < topLevelItemCount())
	{
		m_pDataContext->m_MeshData.SetCurrentMeshIndex(index);
		SelectionUpdate(index);
	}
	else
	{
		SelectionUpdate(m_pDataContext->m_MeshData.GetCurrentMeshIndex());
	}
}

void MeshListWidget::mouseReleaseEvent(QMouseEvent* e)
{
	QTreeWidgetItem* item = itemAt(e->pos());

	if (selectedItems().size() <= 1)
	{
		QTreeWidget::mouseReleaseEvent(e);
	}
}

void MeshListWidget::keyReleaseEvent(QKeyEvent* e)
{
	if (Qt::Key_Enter == e->key())
	{
		m_isEditMode = false;
	}
	else if (Qt::Key_Delete == e->key())
	{
		if (!m_isEditMode)
		{
			slot_DeleteMeshes();
			return;
		}
	}

	QTreeWidget::keyReleaseEvent(e);
}

void MeshListWidget::keyPressEvent(QKeyEvent* e)
{
	if (Qt::Key_Down == e->key())
	{
		e->ignore();
		return;
	}
	else if (Qt::Key_Up == e->key())
	{
		e->ignore();
		return;
	}

	if (!m_isEditMode)
	{
		if (Qt::Key_Right == e->key())
		{
			e->ignore();
			return;
		}
		else if (Qt::Key_Left == e->key())
		{
			e->ignore();
			return;
		}
	}

	QTreeWidget::keyPressEvent(e);
}

QList<muint32> MeshListWidget::getSeletedIndexList() const
{
	QList<muint32> selectIndexList;

	QList<QTreeWidgetItem*> list = selectedItems();

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
		{
			item = item->parent();
		}

		muint32 index = indexOfTopLevelItem(item);

		if (!selectIndexList.contains(index))
		{
			selectIndexList.push_back(index);
		}
	}

	return selectIndexList;
}

QList<QTreeWidgetItem*> MeshListWidget::getSelectedTargetItem()
{
	QList<QTreeWidgetItem*> selectedItemList = selectedItems();
	QList<QTreeWidgetItem*> targetItemList;

	for (int i = 0; i < selectedItemList.size(); i++)
	{
		QTreeWidgetItem* selectedItem = selectedItemList[i];

		if (selectedItem->childCount() == 0)
		{
			selectedItem = selectedItem->parent();
		}

		if (selectedItem)
		{
			muint32 index = indexOfTopLevelItem(selectedItem);

			if (!targetItemList.contains(selectedItem))
			{
				targetItemList.push_back(selectedItem);
			}
		}
	}
	return targetItemList;
}


void MeshListWidget::slot_OnExport3MFFile()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	mip::MeshTopology* m = m_pDataContext->m_MeshData.GetCurrentMesh();
	mint32 layerUID = m_pDataContext->m_MeshData.GetCurrentMeshLUID();

	if (nullptr == m)
	{
		QMessageBox::warning(this, QString("Export error"), QString("The mesh to extract does not exist."));
		return;
	}

	if (!m_pWinManager->IsLicensePass())
	{
		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> Exportlist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
		{
			item = item->parent();
		}

		muint32 index = indexOfTopLevelItem(item);

		if (!Exportlist.contains(item))
		{
			Exportlist.push_back(item);
		}
	}

	MeshInfo* pInfo = nullptr;

	if (Exportlist.size() > 0)
	{
		m_pWinManager->exportList.clear();
		m_pActionManager->SetAfterThread(THREAD_EXPORT_MESH_FILES);

		for (int i = 0; i < Exportlist.size(); i++)
		{
			int index = indexOfTopLevelItem(Exportlist[i]);
			pInfo = m_pDataContext->m_MeshData.GetMeshInfo(index);

			if (pInfo)
			{
				m_pWinManager->exportList.push_back(index);
			}
		}
	}
	else
	{
		return;
	}

	ExportMeshLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_3MF),
		EX_FILES_3MF,
		tr("3MF File(*.3mf;*.3MF)")
	);
}

void MeshListWidget::slot_OnExport3MFFilePatientCoordinate()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	mip::MeshTopology* m = m_pDataContext->m_MeshData.GetCurrentMesh();
	mint32 layerUID = m_pDataContext->m_MeshData.GetCurrentMeshLUID();

	if (nullptr == m)
	{
		QMessageBox::warning(this, QString("Export error"), QString("The mesh to extract does not exist."));
		return;
	}

	if (!m_pWinManager->IsLicensePass())
	{
		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> Exportlist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
		{
			item = item->parent();
		}

		muint32 index = indexOfTopLevelItem(item);

		if (!Exportlist.contains(item))
		{
			Exportlist.push_back(item);
		}
	}

	MeshInfo* pInfo = nullptr;

	if (Exportlist.size() > 0)
	{
		m_pWinManager->exportList.clear();
		m_pActionManager->SetAfterThread(THREAD_EXPORT_MESH_FILES);

		for (int i = 0; i < Exportlist.size(); i++)
		{
			int index = indexOfTopLevelItem(Exportlist[i]);
			pInfo = m_pDataContext->m_MeshData.GetMeshInfo(index);

			if (pInfo)
			{
				m_pWinManager->exportList.push_back(index);
			}
		}
	}
	else
	{
		return;
	}

	ExportMeshLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_3MF),
		EX_FILES_3MF,
		tr("3MF File(*.3mf;*.3MF)"),
		true
	);
}

void MeshListWidget::slot_ExportUSD()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ExportMeshLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_USD),
		EX_FILES_USD,
		tr("USD File(*.usd;*.USD)"),
		true
	);
}

void MeshListWidget::slot_OnExportOBJFile()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ExportMeshLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_OBJ),
		EX_FILES_OBJ,
		tr("Object File(*.obj;*.OBJ)")
	);
}

void MeshListWidget::slot_OnExportOBJFilePatientCoordinate()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ExportMeshLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_OBJ),
		EX_FILES_OBJ,
		tr("Object File(*.obj;*.OBJ)"),
		true
	);
}

void MeshListWidget::slot_OnExportVTKFile()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ExportMeshLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_VTK),
		EX_FILES_VTK,
		tr("VTK File(*.vtk;*.VTK)")
	);
}

void MeshListWidget::slot_OnExportVTKFilePatientCoordinate()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ExportMeshLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_VTK),
		EX_FILES_VTK,
		tr("VTK File(*.vtk;*.VTK)"),
		true
	);
}


void MeshListWidget::slot_ImageMatrixPlus()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	mip::MeshTopology* pCurrentMesh = m_pDataContext->m_MeshData.GetCurrentMesh();

	if (pCurrentMesh)
	{
		mip::MATRIX44 imageMatrix;
		if (getImageMatrix(imageMatrix))
		{
			m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_TRANSFORM, MESH_WORK_TRANSFORM);

			int nMesh = m_pDataContext->m_MeshData.GetMeshCount();

			for (int idx = 0; idx < nMesh; ++idx)
			{
				MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(idx);

				//if (m_pWinManager->vt_pckID[idx])
				if (pMeshInfo && pMeshInfo->selected)
				{
					mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetMesh(idx);

					if (pMesh)
					{
						pMesh->addRotate(imageMatrix.getQuaternion());
						pMesh->addTranslate(imageMatrix.getOrigin());

						m_pMeshManipulator->AddTransformAxis(idx, imageMatrix);

						m_pMeshModelViewManager->UpdatePivotPoint();
					}
				}
			}
		}
		else
		{
			QMessageBox::warning(this, QString("Matrix error"), QString("Image orientation is not correct."));
		}
	}
}

void MeshListWidget::slot_ImageMatrixMinus()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	mip::MeshTopology* pCurrentMesh = m_pDataContext->m_MeshData.GetCurrentMesh();

	if (pCurrentMesh)
	{
		mip::MATRIX44 imageMatrix;
		if (getImageMatrix(imageMatrix))
		{
			imageMatrix.inverse();

			m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_TRANSFORM, MESH_WORK_TRANSFORM);

			int nMesh = m_pDataContext->m_MeshData.GetMeshCount();

			for (int idx = 0; idx < nMesh; ++idx)
			{
				MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(idx);

				//if (m_pWinManager->vt_pckID[idx])
				if (pMeshInfo && pMeshInfo->selected)
				{
					mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetMesh(idx);

					if (pMesh)
					{
						pMesh->addRotate(imageMatrix.getQuaternion());
						pMesh->addTranslate(imageMatrix.getOrigin());

						m_pMeshManipulator->AddTransformAxis(idx, imageMatrix);

						m_pMeshModelViewManager->UpdatePivotPoint();
					}
				}
			}
		}
		else
		{
			QMessageBox::warning(this, QString("Matrix error"), QString("Image orientation is not correct."));
		}
	}
}

bool MeshListWidget::getImageMatrix(mip::MATRIX44& imageMatrix)
{
	if (m_pDataContext == nullptr)
	{
		return false;
	}

	bool bSuceess = false;

	imageMatrix.identity();

	double spaceZ = m_pDataContext->volume_data.getSpaceZ();

	const int nSlice = m_pDataContext->volume_data.getCZ();

	float xdir[3];
	float ydir[3];

	m_pDataContext->volume_data.getImgOrientation(true, xdir);
	m_pDataContext->volume_data.getImgOrientation(false, ydir);

	float zdir[3];
	vtkMath::Cross(xdir, ydir, zdir);

	if (zdir[0] != 0.f || zdir[1] != 0.f || zdir[2] != 0.f)
	{
		double position[3];

		position[0] = (double)m_pDataContext->volume_data.getImgPosX() * 0.1f;
		position[1] = (double)m_pDataContext->volume_data.getImgPosY() * 0.1f;
		position[2] = (double)m_pDataContext->volume_data.getImgPosZ() * 0.1f;

		for (int i = 0; i < 3; i++)
		{
			imageMatrix.m[i][0] = (double)xdir[i];
			imageMatrix.m[i][1] = (double)ydir[i];
			imageMatrix.m[i][2] = (double)zdir[i];
			imageMatrix.m[i][3] = position[i];
		}

		imageMatrix.m[3][3] = 1.0;

		//patientMat.m[0][3] -= zdir[0] * space[2] * (nSlice - 1);
		//patientMat.m[1][3] -= zdir[1] * space[2] * (nSlice - 1);
		imageMatrix.m[2][3] -= zdir[2] * spaceZ * (nSlice - 1);

		imageMatrix.transpose();

		bSuceess = true;
	}

	return bSuceess;
}

void MeshListWidget::setItemByMeshInfo(QTreeWidgetItem* outItem, const MeshInfo& info)
{
	COLOR color = info.color;

	outItem->setText(M_COL_SUB, outItem->isExpanded() ? "-" : "+");
	outItem->setBackground(M_COL_COLOR, QBrush(QColor(color.r, color.g, color.b, 255)));
	outItem->setIcon(M_COL_SHOW, RESOURCE_MANAGER->GetIcon_ListVisible(info.show));
	outItem->setText(M_COL_NAME, info.GetName());
	outItem->setText(M_COL_MATERIAL, getConnectedMaterialNameByMeshInfo(info));
	outItem->setIcon(M_COL_LOCK, RESOURCE_MANAGER->GetIcon_ListOmniverseLock(info.GetOmniverseStatus()));
}

QString MeshListWidget::getConnectedMaterialNameByMeshInfo(const MeshInfo& info)
{
	QString materialName = "None";
	if (m_pOmniverse->GetStage()->IsOpen() == false)
	{
		return materialName;
	}
	mipUSDMeshPtr pUsdMesh = m_pOmniverse->GetUsdStage()->CreateMeshByName(info.GetName().toStdString());
	if (pUsdMesh == nullptr)
	{
		return materialName;
	}

	mipUSDMaterialPtr pUsdMaterial = pUsdMesh->GetMaterial();
	if (pUsdMaterial == nullptr)
	{
		return materialName;
	}
	materialName = QString::fromStdString(pUsdMaterial->GetName());

	return materialName;
}

QVector<int> MeshListWidget::getExportIndexList() const
{
	QVector<int> indexList;

	QList<QTreeWidgetItem*> selectItemList = selectedItems();
	QList<QTreeWidgetItem*> exportItemList;

	for (int i = 0; i < selectItemList.size(); i++)
	{
		QTreeWidgetItem* item = selectItemList.at(i);

		if (item->childCount() == 0)
		{
			item = item->parent();
		}

		muint32 index = indexOfTopLevelItem(item);

		if (!exportItemList.contains(item))
		{
			exportItemList.push_back(item);
		}
	}

	MeshInfo* pMeshInfo = nullptr;

	for (int i = 0; i < exportItemList.size(); i++)
	{
		int index = indexOfTopLevelItem(exportItemList[i]);
		pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(index);

		if (pMeshInfo)
		{
			indexList.push_back(index);
		}
	}

	return indexList;
}

void MeshListWidget::setItemBackgroundColor(QTreeWidgetItem* item, const QBrush& brush)
{
	for (auto& pCol : m_pColumnList->GetList())
	{
		if (pCol->GetKey() != M_COL_COLOR)
		{
			item->setBackground(pCol->GetIndex(), brush);
		}
	}
}

QBrush MeshListWidget::getBrush_Unselected() const
{
	return m_brushes[0];
}

QBrush MeshListWidget::getBrush_Selected() const
{
	return m_brushes[1];
}

QIcon MeshListWidget::getIcon_Invisible() const
{
	return m_visibleIcons[0];
}

QIcon MeshListWidget::getIcon_Visible() const
{
	return m_visibleIcons[1];
}

void MeshListWidget::ExportMeshLayerToFile(QString caption, EXPORT_FILES exportFileType, QString filter, bool bPatientCoordinate)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	mip::MeshTopology* pMesh = m_pDataContext->m_MeshData.GetCurrentMesh();
	mint32 layerUID = m_pDataContext->m_MeshData.GetCurrentMeshLUID();

	if (nullptr == pMesh)
	{
		QMessageBox::warning(this, QString("Export error"), QString("The mesh to extract does not exist."));
		return;
	}

	if (!m_pWinManager->IsLicensePass())
	{
		QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
		return;
	}

	QList<QTreeWidgetItem*> exportItemList = GetExportItemList();

	QString lastestDirPath;
	bool latest = m_pWinManager->lastestPathGet(lastestDirPath, true);
	if (!latest)
	{
		lastestDirPath = "";
	}

	bool exportResult = false;
	if (exportItemList.size() > 1)
	{
		exportResult = ExportMultiFile(caption, lastestDirPath, exportItemList, exportFileType, bPatientCoordinate);
	}
	else if (exportItemList.size() == 1)
	{
		exportResult = ExportSingleFile(caption, lastestDirPath, exportFileType, filter, bPatientCoordinate);
	}

	if (exportResult == false)
	{
		QMessageBox::warning(nullptr,
			caption,
			STRING_MANAGER->getString(STR_FAILED_TO) + caption);
	}
}

QList<QTreeWidgetItem*> MeshListWidget::GetExportItemList()
{
	QList<QTreeWidgetItem*>& selectedItemList = selectedItems();
	QList<QTreeWidgetItem*> exportItemList;

	for (int i = 0; i < selectedItemList.size(); i++)
	{
		QTreeWidgetItem* item = selectedItemList.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 meshIndex = indexOfTopLevelItem(item);

		if (!exportItemList.contains(item))
			exportItemList.push_back(item);
	}

	return exportItemList;
}

bool MeshListWidget::ExportMultiFile(QString caption, QString lastestDirPath, QList<QTreeWidgetItem*> exportItemList, EXPORT_FILES exportFileType, bool bPatientCoordinate)
{
	if (m_pDataContext == nullptr)
	{
		return false;
	}

	int meshIndex = 0;
	MeshInfo* meshInfo = nullptr;
	QString filepath = "";

	m_pActionManager->SetAfterThread(THREAD_EXPORT_MESH_FILES);
	m_pWinManager->exportList = getExportIndexList();

	meshIndex = indexOfTopLevelItem(exportItemList[0]);
	meshInfo = m_pDataContext->m_MeshData.GetMeshInfo(meshIndex);

	if (exportFileType == EX_FILES_USD)
	{
		QString filter = tr("USD File(*.usd;*.USD)");
		filepath = ExportFileDialog(
			this, caption,
			m_pDataContext->m_MeshData.GetCurrentMeshName(),
			lastestDirPath,
			filter,
			QFileDialog::ShowDirsOnly
		);

		if (filepath.isEmpty())
		{
			/* Skip */
			return true;
		}
	}
	else
	{
		QString dirpath = ExportDirectoryDialog(
			this, caption, lastestDirPath
		);

		if (dirpath.isEmpty())
		{
			/* Skip */
			return true;
		}

		m_pWinManager->exportPath = dirpath;

		QString fileName = m_pDataContext->m_MeshData.GetMeshName(meshIndex);
		filepath = dirpath + "/" + QString("%1.").arg(fileName);

		if (exportFileType != EX_FILES_STL && exportFileType != EX_FILES_3MF)
		{
			m_pWinManager->exportList.takeFirst();
		}
	}

	return m_pWinManager->SaveMeshFiles(filepath, meshIndex, meshInfo->uid, exportFileType, bPatientCoordinate);
}

bool MeshListWidget::ExportSingleFile(QString caption, QString lastestDirPath, EXPORT_FILES exportFileType, QString filter, bool bPatientCoordinate)
{
	if (m_pDataContext == nullptr)
	{
		return false;
	}

	m_pWinManager->exportList.clear();

	QString filePath = ExportFileDialog(
		this, caption,
		m_pDataContext->m_MeshData.GetCurrentMeshName(),
		lastestDirPath,
		filter,
		QFileDialog::ShowDirsOnly
	);

	if (filePath.isEmpty())
	{
		/* Skip */
		return true;
	}
	MeshInfo* meshInfo = m_pDataContext->m_MeshData.GetCurrentMeshInfo();

	int index = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
	m_pWinManager->exportList.push_back(index);

	return m_pWinManager->SaveMeshFiles(filePath, index, meshInfo->uid, exportFileType, bPatientCoordinate);
}

void MeshListWidget::slot_ShiftPosition()
{
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
	{
		item = item->parent();
	}

	index = indexOfTopLevelItem(item);

	if (m_pDataContext)
	{
		m_pActionManager->action_MeshList_align_change(m_pDataContext, index);
	}
}

void MeshListWidget::rename()
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

	m_isEditMode = true;
	m_pWinManager->setMoveFocus(false);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	editItem(item, M_COL_NAME);
}

void MeshListWidget::UpdateItemByLayerIndex(int layerIndex)
{
	MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(layerIndex);
	if (nullptr == info)
	{
		return;
	}

	QTreeWidgetItem* item = topLevelItem(layerIndex);
	if (nullptr == item)
	{
		return;
	}

	setItemByMeshInfo(item, *info);
}

void MeshListWidget::UpdateItemList()
{
	for (int i = 0; i < topLevelItemCount(); ++i)
	{
		QTreeWidgetItem* item = topLevelItem(i);
		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (item && info)
		{
			setItemByMeshInfo(item, *info);
		}
	}
}

void MeshListWidget::ToggleShowState()
{
	QList<QTreeWidgetItem*> itemList = getSelectedTargetItem();

	bool isChanged = false;

	for (int i = 0; i < itemList.size(); i++)
	{
		QTreeWidgetItem* item = itemList.at(i);
		muint32 index = indexOfTopLevelItem(item);
		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(index);
		if (info)
		{
			info->show = !info->show;
			setItemByMeshInfo(item, *info);
			isChanged = true;
		}
	}

	if (isChanged)
	{
		const MeshListColumn* pCol = m_pColumnList->GetByKey(M_COL_SHOW);
		if (pCol)
		{
			emit sig_columnStateChanged(pCol->GetIndex());
		}
	}
}

void MeshListWidget::ToggleOmnivserStatusState()
{
	QList<QTreeWidgetItem*> targetItemList = getSelectedTargetItem();

	bool isChanged = false;

	for (int i = 0; i < targetItemList.size(); i++)
	{
		QTreeWidgetItem* item = targetItemList.at(i);
		muint32 index = indexOfTopLevelItem(item);
		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(index);
		if (info)
		{
			if (info->GetOmniverseStatus() == eOmniverseStatus::Unlock)
			{
				info->SetOmniverseStatus(eOmniverseStatus::Lock);
			}
			else if (info->GetOmniverseStatus() == eOmniverseStatus::Lock)
			{
				info->SetOmniverseStatus(eOmniverseStatus::Unlock);
			}
			setItemByMeshInfo(item, *info);
			isChanged = true;
		}
	}

	if (isChanged)
	{
		const MeshListColumn* pCol = m_pColumnList->GetByKey(M_COL_LOCK);
		if (pCol)
		{
			emit sig_columnStateChanged(pCol->GetIndex());
		}
	}
}

bool MeshListWidget::IsEditMode() const
{
	return m_isEditMode;
}

void MeshListWidget::SetEditMode(bool value)
{
	m_isEditMode = value;
}

std::vector<MeshActionInfo> MeshListWidget::GetActionInfoList() const
{
	std::vector<MeshActionInfo> info = MESH_ACTION_INFO_LIST;

	return info;
}

std::shared_ptr<QMenu> MeshListWidget::Get_QMenu()
{
	std::vector<QAction*> actionList;

	std::shared_ptr<QMenu> pMenu = std::make_shared<QMenu>(this);

	for (const MeshActionInfo& info : MESH_ACTION_INFO_LIST)
	{
		if (m_pProductManager->IsAvailableFunctionLevel((eMEDIP_FUNCTION_LEVEL)info.FunctionLevel) == eAVAILABLE_STATE::CREATE)
		{
			if (info.Children.empty())
			{
				QAction* action = pMenu->addAction(info.Text);
				connect(action, &QAction::triggered, this, info.SlotFunction);
			}
			else
			{
				QMenu* subMenu = pMenu->addMenu(info.Text);
				for (const MeshActionInfo& subChildInfo : info.Children)
				{
					QAction* action = subMenu->addAction(subChildInfo.Text);
					connect(action, &QAction::triggered, this, subChildInfo.SlotFunction);
				}
			}
		}
	}

	return pMenu;
}

bool MeshListWidget::ItemClicked(QTreeWidgetItem* item, int column)
{
	if ((m_pDataContext == nullptr) || (item == nullptr))
	{
		return false;
	}

	int count = selectedItems().count();

	if (count > 1)
	{
		qWarning() << "item selected count : " << count;
		return false;
	}

	const MeshListColumn* pColumn = m_pColumnList->GetByIndex(column);
	if (pColumn == nullptr)
	{
		return false;
	}

	muint32 index = indexOfTopLevelItem(item);

	if (M_COL_SHOW == pColumn->GetKey())
	{
		ToggleShowState();
	}
	else if (M_COL_LOCK == pColumn->GetKey())
	{
		ToggleOmnivserStatusState();
	}
	else if (M_COL_SUB == pColumn->GetKey())
	{
		bool isExpanded = item->isExpanded();

		item->setExpanded(!isExpanded);

		if (!isExpanded)
		{
			item->setText(M_COL_SUB, QString("-"));
		}
		else
		{
			item->setText(M_COL_SUB, QString("+"));
		}
	}

	m_pDataContext->m_MeshData.SetCurrentMeshIndex(index);
	emit m_pDataContext->sig_meshSelectChanged();

	return true;
}

bool MeshListWidget::ItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	if ((m_pDataContext == nullptr) || (item == nullptr))
	{
		return false;
	}

	const MeshListColumn* pColumn = m_pColumnList->GetByIndex(column);
	if (pColumn == nullptr)
	{
		return false;
	}

	if (pColumn->GetKey() != M_COL_NAME)
	{
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));
	}

	int count = selectedItems().count();
	if (count > 1)
	{
		qWarning() << "item selected count : " << count;
		return false;
	}

	muint32 index = indexOfTopLevelItem(item);
	MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(index);

	if (pMeshInfo)
	{
		if (M_COL_COLOR == pColumn->GetKey())
		{
			QColorDialog dlg;
			QColor col = QColor(pMeshInfo->color.r, pMeshInfo->color.g, pMeshInfo->color.b);
			dlg.setCurrentColor(col);
			dlg.setStyleSheet("background-color : rgba(48,48,48,255); color : white");

			if (dlg.exec() == QDialog::Accepted)
			{
				if (col != dlg.selectedColor())
				{
					m_pActionManager->action_MeshList_color_change(m_pDataContext, index, dlg.selectedColor());
				}
			}
		}
		else if (M_COL_NAME == pColumn->GetKey())
		{
			SetEditMode(true);
			m_pWinManager->setMoveFocus(false);
			item->setFlags(item->flags() | Qt::ItemIsEditable);
			editItem(item, M_COL_NAME);
		}

		m_pDataContext->m_MeshData.SetCurrentMeshIndex(index);
		emit m_pDataContext->sig_meshSelectChanged();
	}

	return true;
}

void MeshListWidget::slot_DeleteMeshes()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = selectedItems();
	QList<QTreeWidgetItem*> Dellist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!Dellist.contains(item))
			Dellist.push_back(item);
	}

	if (Dellist.size() > m_pDataContext->m_MeshData.GetMeshCount())
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), QString("Only %1 or fewer layers can be deleted.").
			arg(m_pDataContext->m_MeshData.GetMeshCount()));
		return;
	}

	if (Dellist.size() == 1)
	{
		mint8 index = indexOfTopLevelItem(Dellist.at(0));

		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(index);

		if (info)
		{
			mint32 lUID = info->uid;

			m_pActionManager->action_MeshList_delete(m_pDataContext, index, lUID);
		}
	}
	else if (Dellist.size() > 1)
	{
		std::vector<mint8> indeces;
		for (int cnt = 0; cnt < Dellist.size(); cnt++)
		{
			indeces.push_back(indexOfTopLevelItem(Dellist[cnt]));
		}

		m_pActionManager->action_MeshList_delete(m_pDataContext, indeces);
	}
}

void MeshListWidget::slot_OnDiff()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*> list = selectedItems();
	QList<muint32> diffList;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!diffList.contains(index))
			diffList.push_back(index);
	}

	if (diffList.size() != 2)
	{
		QMessageBox::warning(this, QString("Mesh Diff"), STRING_MANAGER->getString(STR_SELECT_2_WARN));
		return;
	}

	m_pActionManager->action_Mesh_Boolean(m_pDataContext, MESH_WORK_DIFF, diffList.at(0), diffList.at(1));
}

void MeshListWidget::slot_OnIntersect()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*> list = selectedItems();
	QList<muint32> sectList;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
		{
			item = item->parent();
		}

		muint32 index = indexOfTopLevelItem(item);

		if (!sectList.contains(index))
		{
			sectList.push_back(index);
		}
	}

	if (sectList.size() != 2)
	{
		QMessageBox::warning(this, QString("Mesh Intersect"), STRING_MANAGER->getString(STR_SELECT_2_WARN));
		return;
	}

	m_pActionManager->action_Mesh_Boolean(m_pDataContext, MESH_WORK_INTERSECT, sectList.at(0), sectList.at(1));
}

void MeshListWidget::slot_OnUnion()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*> list = selectedItems();
	QList<muint32> unionList;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!unionList.contains(index))
			unionList.push_back(index);
	}

	if (unionList.size() != 2)
	{
		QMessageBox::warning(this, QString("Mesh Union"), STRING_MANAGER->getString(STR_SELECT_2_WARN));
		return;
	}

	m_pActionManager->action_Mesh_Boolean(m_pDataContext, MESH_WORK_UNION, unionList.at(0), unionList.at(1));
}

/*
@brief
@return
*/
void MeshListWidget::slot_Attach()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	QList<muint32> selected_list = getSeletedIndexList();

	if (selected_list.size() != 2)
	{
		QMessageBox::warning(this, QString("Attatch mesh"), STRING_MANAGER->getString(STR_SELECT_2_WARN));
		return;
	}

	m_pActionManager->action_Mesh_Attach(m_pDataContext, selected_list);
}

/*
@brief
@return
*/
void MeshListWidget::slot_Mesh2Mask()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	QList<muint32> selected_list = getSeletedIndexList();

	if (selected_list.empty())
	{
		QMessageBox::warning(this, QString("Select mesh"), STRING_MANAGER->getString(STR_SELECT_2_WARN));
		return;
	}

	m_pActionManager->action_Mesh_MeshToMask(selected_list, m_pDataContext);
}

/*
@brief
@return
*/
void MeshListWidget::slot_OnDuplicate()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	QList<muint32> selected_list = getSeletedIndexList();

	if (selected_list.size() < 1)
	{
		QMessageBox::warning(this, QString("Solid mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
		return;
	}

	MEVolumeView* view = m_pWinManager->mainMeshWidget->getMainView();
	view->setEnabled(false);

	m_pActionManager->action_Mesh_Duplicate(m_pDataContext, selected_list);
}

void MeshListWidget::slot_Solid()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	QList<QTreeWidgetItem*> list = selectedItems();
	QList<muint32> SolidList;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
		{
			item = item->parent();
		}

		muint32 index = indexOfTopLevelItem(item);

		if (!SolidList.contains(index))
		{
			SolidList.push_back(index);
		}
	}

	//20201008_byPHS_meshlist 0개일때 팅김 수정
	if (SolidList.size() < 1)
	{
		QMessageBox::warning(this, QString("Solid mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
		return;
	}

#ifndef TEMP_CODE
	if (SolidList.size() != 1)
	{
		QMessageBox::warning(this, QString("Solid mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
		return;
	}

	OffsetDlg dlg(QString("Solid Mesh [%1]").arg(m_pDataContext->m_MeshData.GetMeshName(SolidList.at(0))),
		QString("Offset :"), 0, 0.01, -1.0, 1.0, this);

	dlg.exec();

	if (dlg.isAccept())
	{
		float offset = dlg.getDoubleVal();

		m_pActionManager->action_Mesh_Remesh(MESH_SOLID, offset, SolidList.at(0));
	}
#else
	//220121 허 건 과장
	m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_SOLID, MESH_WORK_NONE);

	m_pWinManager->showMeshDlg(MESH_WORK_SOLID);
#endif
	}

void MeshListWidget::slot_Hollow()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	QList<QTreeWidgetItem*> list = selectedItems();
	QList<muint32> HollowList;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!HollowList.contains(index))
			HollowList.push_back(index);
	}

	//20201008_byPHS_meshlist 0개일때 팅김 수정
	if (HollowList.size() < 1)
	{
		QMessageBox::warning(this, QString("Hollow mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
		return;
	}

	//220121 허 건 과장
	m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_HOLLOW, MESH_WORK_NONE);

	m_pWinManager->showMeshDlg(MESH_WORK_HOLLOW);
}

void MeshListWidget::slot_Remesh()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	QList<QTreeWidgetItem*> list = selectedItems();
	QList<muint32> RemeshList;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!RemeshList.contains(index))
			RemeshList.push_back(index);
	}

	//20201008_byPHS_meshlist 0개일때 팅김 수정
	if (RemeshList.size() < 1)
	{
		QMessageBox::warning(this, QString("Hollow mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
		return;
	}

	//220121 허 건 과장
	m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_REMESH, MESH_WORK_NONE);

	m_pWinManager->showMeshDlg(MESH_WORK_REMESH);

#ifndef TEMP_CODE
	if (RemeshList.size() != 1)
	{
		QMessageBox::warning(this, QString("Remesh mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
		return;
	}

	OffsetDlg dlg(QString("Remesh Mesh [%1]").arg(m_pDataContext->m_MeshData.GetMeshName(RemeshList.at(0))),
		QString("Level :"), 1, 1.0, 1, 4, this);

	dlg.exec();

	if (dlg.isAccept())
	{
		int offset = dlg.getIntVal();

		m_pActionManager->action_Mesh_Remesh(MESH_REMESH, offset, RemeshList.at(0));
	}
#else
	//m_pActionManager->action_Mesh_Remesh(MESH_REMESH, 1, RemeshList.at(0));
#endif
}

void MeshListWidget::slot_Smooth()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	QList<QTreeWidgetItem*> list = selectedItems();
	QList<muint32> SmoothList;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!SmoothList.contains(index))
			SmoothList.push_back(index);
	}

	//20201008_byPHS_meshlist 0개일때 팅김 수정
	if (SmoothList.size() < 1)
	{
		QMessageBox::warning(this, QString("Smooth mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
		return;
	}

	//220121 허 건 과장
	m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_SMOOTH, MESH_WORK_NONE);

	m_pWinManager->showMeshDlg(MESH_WORK_SMOOTH);

	return;
}

void MeshListWidget::slot_Reduction()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	QList<QTreeWidgetItem*> list = selectedItems();
	QList<muint32> ReducList;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!ReducList.contains(index))
			ReducList.push_back(index);
	}

	//20201008_byPHS_meshlist 0개일때 팅김 수정
	if (ReducList.size() < 1)
	{
		QMessageBox::warning(this, QString("Reduct mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
		return;
	}

	//220121 허 건 과장
	m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_REDUCTION, MESH_WORK_NONE);

	m_pWinManager->showMeshDlg(MESH_WORK_REDUCTION);
}

// 210115 허 건
void MeshListWidget::slot_SubDivision()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	QList<QTreeWidgetItem*> list = selectedItems();
	QList<muint32> divide_List;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
		{
			item = item->parent();
		}

		muint32 index = indexOfTopLevelItem(item);

		if (!divide_List.contains(index))
		{
			divide_List.push_back(index);
		}
	}

	//20201008_byPHS_meshlist 0개일때 팅김 수정
	if (divide_List.size() < 1)
	{
		QMessageBox::warning(this, QString("SubDivision mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
		return;
	}

	//220121 허 건 과장
	m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_SUBDIVISION, MESH_WORK_NONE);

	m_pWinManager->showMeshDlg(MESH_WORK_SUBDIVISION);
}

void MeshListWidget::slot_IslandFilter()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	QList<QTreeWidgetItem*> list = selectedItems();
	QList<muint32> filter_List;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
		{
			item = item->parent();
		}
		muint32 index = indexOfTopLevelItem(item);

		if (!filter_List.contains(index))
		{
			filter_List.push_back(index);
		}
	}

	if (filter_List.size() < 1)
	{
		QMessageBox::warning(this, QString("Island-filter mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
		return;
	}

	//220121 허 건 과장
	m_pActionManager->action_UndoRedo_update(m_pDataContext, MESH_WORK_ISLANDFILTER, MESH_WORK_NONE);

	m_pWinManager->showMeshDlg(MESH_WORK_ISLANDFILTER);
}

void MeshListWidget::slot_SendToOmniverse()
{
	QList<muint32> selectIndexList = getSeletedIndexList();
	if (selectIndexList.empty())
	{
		return;
	}

	ActionSendToOmniverse action(m_pDataContext, selectIndexList);
	action.Run();
}

void MeshListWidget::slot_ReceiveFromOmniverse()
{
	CMeshWorkManager* pMeshWork = m_pMeshModelViewManager->GetWorkManager();
	ActionReceiveFromOmniverse action(m_pDataContext, m_pWinManager, pMeshWork, m_pMeshManipulator, m_pShortcutManager);
	action.Run();
}


#if SUPPORT_MESHOFFSET == 1
void MeshListWidget::OnMeshOffset()
{
	QList<QTreeWidgetItem*> list = selectedItems();
	QList<muint32> SmoothList;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = indexOfTopLevelItem(item);

		if (!SmoothList.contains(index))
			SmoothList.push_back(index);
	}

	//20201008_byPHS_meshlist 0개일때 팅김 수정
	if (SmoothList.size() < 1)
	{
		QMessageBox::warning(this, QString("Smooth mesh"), STRING_MANAGER->getString(STR_ONE_SELECT));
		return;
	}

	m_pWinManager->showMeshDlg(MESH_WORK_MESHOFFSET);

	return;
}
#endif


void MeshListWidget::slot_OnExportSTLFile()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ExportMeshLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_STL),
		EX_FILES_STL,
		tr("STL File(*.stl;*.STL)")
	);
}

void MeshListWidget::slot_OnExportSTLFilePatientCoordinate()
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	if (m_pDataContext->m_MeshData.GetMeshCount() < 1)
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ExportMeshLayerToFile(
		STRING_MANAGER->getString(STR_EXPORT_STL),
		EX_FILES_STL,
		tr("STL File(*.stl;*.STL)"),
		true
	);
}


void MeshListWidget::slot_meshInfoChanged(MeshInfo* pInfo)
{
	if (m_pDataContext == nullptr)
	{
		return;
	}

	qInfo() << "mesh info chaged : " << pInfo->GetName();
	int index = 0;
	if (m_pDataContext->m_MeshData.TryGetMeshLayerIndexByInfo(&index, pInfo))
	{
		UpdateItemByLayerIndex(index);
	}
}

void MeshListWidget::slot_OmniverseStageChanged()
{
	UpdateItemList();
}

void MeshListWidget::slot_OmniverseStagePresetChanged()
{
	UpdateItemList();
}


