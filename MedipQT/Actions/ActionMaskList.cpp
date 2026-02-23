#include "stdafx.h"
#include "ActionMaskList.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "System/stringManager.h"
#include "graphics/volumedata.h"
#include "UI/MaskList.h"
#include "ActionManager.h"
#include "Actions/ActionMesh.h"
#include "Windows/Tabwindow.h"
#include "Windows/Tab/MeshTab.h"
#include "MeshEdit/CMeshWorkManager.h"


ActionMaskClearOne::ActionMaskClearOne(VOLUME_DATA* pVolumeData, muint32 index, int maskIndex, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_CLEAR_ONE;
	m_id = s_id++;
	m_list_index = index;
	m_maskIndex = maskIndex;
	MaskInfo * info = m_pVolumeData->getMaskInfo(m_list_index);
	m_maskNumber = info->uid;
	m_TAState = m_pVolumeData->getTAState(m_maskNumber);
	//	m_fillCount = m_pVolumeData->voxelCount[m_maskNumber];
	//	m_orgbox = m_pVolumeData->boundingBoxROI[m_maskNumber];
	m_fillCount = m_pVolumeData->getVoxelCount(m_maskNumber);
	m_orgbox = m_pVolumeData->getBoundingBox(m_maskNumber);
	m_first = false;
}

bool ActionMaskClearOne::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskClearOne::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
			m_pVolumeData->setVoxelCount(m_maskNumber, m_fillCount, false);
			m_pVolumeData->setBoundingBox(m_maskNumber, m_orgbox);
			m_pVolumeData->forceUpdateMaskVolume();
			//	m_pVolumeData->voxelCount[m_maskNumber] += m_fillCount;
			//	m_pVolumeData->boundingBoxROI[m_maskNumber] = m_orgbox;
			WIN_MANAGER->updatePlaneData_all();
			WIN_MANAGER->renderLater_GridView(false);
			m_pVolumeData->setTAState(m_maskNumber, m_TAState);
			WIN_MANAGER->applyVoxelToUI(m_maskNumber);
			WIN_MANAGER->setSaveState(false);
		}
	}
}

void ActionMaskClearOne::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			MaskInfo * info = m_pVolumeData->getMaskInfo(m_list_index);
			m_pVolumeData->clearMaskData(m_maskIndex == 0 ? info->mask_id : info->mask_id2, m_maskIndex);
			WIN_MANAGER->setDelMaskView(m_list_index, true);
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}
		}
		m_pVolumeData->setVoxelCount(m_maskNumber, -m_fillCount, false);
		m_pVolumeData->setBoundingBox(m_maskNumber, BoundingBoxI(), true);
		m_pVolumeData->forceUpdateMaskVolume();
		//	m_pVolumeData->voxelCount[m_maskNumber] -= m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_maskNumber].reset(true);
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(false);
		m_pVolumeData->setTAState(m_maskNumber, false);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber);
		WIN_MANAGER->setSaveState(false);
	}
}



ActionMaskListAdd::ActionMaskListAdd(VOLUME_DATA* pVolumeData, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_ADD;
	m_id = s_id++;
	m_list_index = 0;
	m_first = false;
}

bool ActionMaskListAdd::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListAdd::undo()
{
	bool r = m_pVolumeData->delMaskInfo(m_list_index);
	m_pVolumeData->setCurrentMaskIndex(0);
	MaskInfo * info = m_pVolumeData->getMaskInfo(0);
	std::vector<muint32> vecSelect;
	vecSelect.push_back(info->uid);
	m_pVolumeData->setMultiSelectUID(vecSelect);
	if (r)
	{
		WIN_MANAGER->updateUI();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionMaskListAdd::redo()
{
	if (m_first == false)
	{
		m_first = true;
		bool r = m_pVolumeData->createMaskInfo();
		if (r)
		{
			m_list_index = m_pVolumeData->getCurrentMaskInfoID();
			m_info = *m_pVolumeData->getCurrentMaskInfo();
		}
	}
	else
	{
		m_pVolumeData->insertMaskInfo(m_list_index, m_info);
	}
	WIN_MANAGER->forceUpdate2DViewData(false, true);
	WIN_MANAGER->updateUI(true, m_list_index);
	WIN_MANAGER->renderLater_GridView(false);
	WIN_MANAGER->setSaveState(false);
}





ActionMaskListCopyAdd::ActionMaskListCopyAdd(VOLUME_DATA* pVolumeData, muint32 index, QUndoCommand *parent)
	: QUndoCommand(parent),
	m_pVolumeData(pVolumeData)
{
	static int s_id = ACT_ID_MASKLIST_COPY;
	m_id = s_id++;

	//m_info = *mask;
	m_list_index = index;
	m_del_index = 0;
	m_first = false;
}

bool ActionMaskListCopyAdd::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListCopyAdd::undo()
{
	bool r = m_pVolumeData->delMaskInfo(m_del_index);
	m_pVolumeData->setCurrentMaskIndex(0);
	MaskInfo * info = m_pVolumeData->getMaskInfo(0);
	std::vector<muint32> vecSelect;
	vecSelect.push_back(info->uid);
	m_pVolumeData->setMultiSelectUID(vecSelect);
	if (r)
	{
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updateUI();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionMaskListCopyAdd::redo()
{
	//if (m_first == false)
	{
		m_first = true;
		bool r = m_pVolumeData->createMaskInfoFromCopyByIndex(m_list_index);
		if (r)
		{
			m_del_index = m_pVolumeData->getCurrentMaskInfoID();
			m_info = *m_pVolumeData->getCurrentMaskInfo();
		}
	}
	//else
	//{
	//	m_pVolumeData->insertMaskInfo(m_del_index, m_info);
	//}
	WIN_MANAGER->updateUI(true, m_del_index);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->updatePlaneData_all();
	WIN_MANAGER->renderLater_GridView(false);
	WIN_MANAGER->setSaveState(false);
}





ActionMaskListDel::ActionMaskListDel(VOLUME_DATA* pVolumeData, muint32 index, int maskIndex, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_DEL;
	m_id = s_id++;
	m_maskIndex = maskIndex;
	m_list_index = index;
	m_taseq = -1;
	m_mesh = NULL;
#ifdef MULTI_DRAWCUT_MODE
	{
		MaskInfo*info = m_pVolumeData->getMaskInfo(m_list_index);

		if (info)
		{
			DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();

			if (tab)
				m_seedchk = tab->contains(info->uid);
		}
	}
#else
	m_seedchk = false;
#endif
}

ActionMaskListDel::~ActionMaskListDel()
{
	if (m_mesh)
		SAFE_DELETE(m_mesh);
}

bool ActionMaskListDel::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListDel::undo()
{
	//기존 UID 를 차지하고 있는 마스크가 있는 경우, 해당 마스크에 대한 DATA 기록, UID/MASKID 교체 후 진행
	if (-1 != m_mvmaskIndex)
	{
		MaskInfo *chInfo = m_pVolumeData->getMaskInfo(m_info.uid, true);

		if (!chInfo)
		{
			return;
		}

		m_pVolumeData->setVoxelCount(chInfo->uid, m_pVolumeData->getVoxelCount(m_info.uid));
		m_pVolumeData->moveMaskBitData(m_maskIndex, m_mvmaskIndex,
			m_maskIndex == 0 ? chInfo->mask_id : chInfo->mask_id2, m_mvmaskID);
		chInfo->uid = m_mvmaskUID;
		chInfo->mask_id = VM_MASK7;
		chInfo->mask_id2 = m_mvmaskID;
		m_pVolumeData->setBoundingBox(chInfo->uid, m_orgbox[1]);

		WIN_MANAGER->ChangeUID(m_info.uid, chInfo->uid);

		std::swap(m_info.uid, chInfo->uid);
	}
	m_pVolumeData->setVoxelCount(m_info.uid, m_fillCount);
	m_pVolumeData->setBoundingBox(m_info.uid, m_orgbox[0]);
	bool r = m_pVolumeData->insertMaskInfo(m_list_index, m_info);
	AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();
	if ((m_taseq != -1) && tab)
		tab->SetResultFromFile(m_info.uid, m_taseq);

	m_pVolumeData->setCurrentMaskIndex(m_list_index);
	MaskInfo * info = m_pVolumeData->getMaskInfo(m_list_index);
	std::vector<muint32> vecSelect;
	vecSelect.push_back(info->uid);
	m_pVolumeData->setMultiSelectUID(vecSelect);

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
			file.close();
		}
	}

	// 211026 허 건과장
	// Mess 리스트 영향으로 인한 주석처리
	//if (m_mesh)
	//{
	//	m_mesh->undo();
	//}

	if (m_seedchk)
	{
		DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();

		if (tab)
			tab->AddSeed(m_info.uid);
	}

	if (r)
	{
		m_pVolumeData->setTAState(m_info.uid, m_TAState);
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updateUI();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionMaskListDel::redo()
{
	m_info = *m_pVolumeData->getMaskInfo(m_list_index);
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_info.uid);
	m_fillCount = m_pVolumeData->getVoxelCount(m_info.uid);

	if (WIN_MANAGER->GetTab())
	{
		AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();

		if (tab)
		{
			m_taseq = tab->existResult(m_info.uid, true);
			tab->DeleteResult(m_info.uid, true);
		}
	}

	MaskInfo * lastInfo = m_pVolumeData->getAtLastMaskInfo();
	if (lastInfo->uid >= MASK_SECOND_MAX)
	{
		m_mvmaskIndex = (lastInfo->uid - MASK_SECOND_MAX) / 8 + 1;
		m_mvmaskID = lastInfo->mask_id2;
	}
	else
	{
		m_mvmaskIndex = 0;
		m_mvmaskID = lastInfo->mask_id;
	}

	if (!(m_mvmaskIndex > m_maskIndex))
		m_mvmaskIndex = -1;
	else
	{
		m_mvmaskUID = lastInfo->uid;
		m_orgbox[1] = m_pVolumeData->getBoundingBox(lastInfo->uid);
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
			file.close();
		}
		else
		{
			// 211026 허 건과장
			// Mess 리스트 영향으로 인한 주석처리
			//if (m_info.meshConnected)
			//{
			//	mint8 mUID = m_pVolumeData->GetMeshUID(m_info.uid);

			//	if (mUID != -1)
			//		m_mesh = new ActionDelMesh(mUID, m_info.uid);
			//}


			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			m_pVolumeData->clearMaskData(m_maskIndex == 0 ? m_info.mask_id : m_info.mask_id2, m_maskIndex);
			m_TAState = m_pVolumeData->getTAState(m_info.uid);

			WIN_MANAGER->setDelMaskView(m_list_index);

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}
		}
	}
	/*WIN_MANAGER->deleteMesh(m_info.uid);*/
	// 211026 허 건과장
	// Mess 리스트 영향으로 인한 주석처리
	//if (m_mesh)
	//{
	//	m_mesh->redo();
	//}

	if (m_seedchk)
	{
		DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();

		if (tab)
			tab->DelSeed(m_info.uid);
	}

	m_pVolumeData->setTAState(m_info.uid, false);
	bool r = m_pVolumeData->delMaskInfo(m_list_index);
	//m_pVolumeData->setCurrentMaskIndex(0);
// 	MaskInfo * info = m_pVolumeData->getMaskInfo(0);
// 	std::vector<muint32> vecSelect;
// 	vecSelect.push_back(info->uid);
// 	m_pVolumeData->setMultiSelectUID(vecSelect);

	if (r)
	{
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updateUI();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->setSaveState(false);
	}
}







ActionMaskListTextChange::ActionMaskListTextChange(VOLUME_DATA* pVolumeData, MaskListWidget * maskList, muint32 data_index, QString & str, QUndoCommand *parent)
	: QUndoCommand(parent),
	m_meshIndex(-1)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_TEXT_CHANGE;
	m_id = s_id++;
	m_index = data_index;
	m_str = str;
	m_maskList = maskList;


	//MaskInfo *info = m_pVolumeData->getMaskInfo(m_index);

	//if (info)
	//	m_meshIndex = m_pVolumeData->GetMeshUID(info->uid);
}

bool ActionMaskListTextChange::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListTextChange::undo()
{
	MaskInfo * info = m_pVolumeData->getMaskInfo(m_index);

	m_pVolumeData->setMaskName(m_preStr, info->uid, true);

	m_maskList->SelectionRefresh(m_index);

	{
		AnalysisTab*tab = WIN_MANAGER->GetTab()->getAnalysisTab();

		if (tab)
			tab->UpdateName(info->uid, m_preStr);
	}

	if (-1 != m_meshIndex)
	{
		MeshTab *tab = WIN_MANAGER->GetTab()->getMeshTab();
		if (tab)
			tab->UpdateSubWidget(m_meshIndex);

		VisualPrintMeshTab *pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
		if (pVisualPrintMeshTab)
			pVisualPrintMeshTab->UpdateSubWidget(m_meshIndex);
	}

#ifdef MULTI_DRAWCUT_MODE
	{
		DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();
		if (tab)
		{
			if (tab->contains(info->uid))
				tab->UpdateSeed();
		}
	}
#endif

	WIN_MANAGER->setSaveState(false);
	//	m_maskList->update();
}

void ActionMaskListTextChange::redo()
{
	MaskInfo * info = m_pVolumeData->getMaskInfo(m_index);
	m_preStr = m_pVolumeData->getMaskName(m_index);

	m_pVolumeData->setMaskName(m_str, info->uid, true);

	m_maskList->SelectionRefresh(m_index);

	{
		AnalysisTab *tab = WIN_MANAGER->GetTab()->getAnalysisTab();

		if (tab)
			tab->UpdateName(info->uid, m_str);
	}

	if (-1 != m_meshIndex)
	{
		MeshTab *tab = WIN_MANAGER->GetTab()->getMeshTab();
		if (tab)
			tab->UpdateSubWidget(m_meshIndex);

		VisualPrintMeshTab *pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
		if (pVisualPrintMeshTab)
			pVisualPrintMeshTab->UpdateSubWidget(m_meshIndex);
	}

#ifdef MULTI_DRAWCUT_MODE
	{
		DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();
		if (tab)
		{
			if (tab->contains(info->uid))
				tab->UpdateSeed();
		}
	}
#endif

	WIN_MANAGER->setSaveState(false);
	//	m_maskList->update();
}

ActionMaskListColor::ActionMaskListColor(VOLUME_DATA* pVolumeData, MaskListWidget * maskList, muint32 data_index, COLOR color, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_COLOR;
	m_id = s_id++;
	m_index = data_index;
	m_color = color;
	m_maskList = maskList;

	//MaskInfo *info = m_pVolumeData->getMaskInfo(m_index);

	//if (info)
	//	m_meshIndex = m_pVolumeData->GetMeshUID(info->uid);
}

bool ActionMaskListColor::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListColor::undo()
{
	MaskInfo * info = m_pVolumeData->getMaskInfo(m_index);
	QTreeWidgetItem * item = m_maskList->topLevelItem(m_index);

	if (info && item)
	{
		info->color = m_preColor;
		item->setBackground(L_COL_COLOR, QBrush(QColor(m_preColor.r, m_preColor.g, m_preColor.b, 255)));

		WIN_MANAGER->forceUpdate2DViewData(false, true);
		WIN_MANAGER->renderLater_GridView(true);
		m_pVolumeData->setCurrentMaskIndex(m_index);
		MaskInfo * info = m_pVolumeData->getMaskInfo(m_index);
		std::vector<muint32> vecSelect;
		vecSelect.push_back(info->uid);
		m_pVolumeData->setMultiSelectUID(vecSelect);

		if (-1 != m_meshIndex)
		{
			MeshTab *tab = WIN_MANAGER->GetTab()->getMeshTab();
			if (tab)
				tab->UpdateSubWidget(m_meshIndex);

			VisualPrintMeshTab *pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
			if (pVisualPrintMeshTab)
				pVisualPrintMeshTab->UpdateSubWidget(m_meshIndex);
		}
#ifdef MULTI_DRAWCUT_MODE
		{
			DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();
			if (tab)
			{
				if (tab->contains(info->uid))
					tab->UpdateSeed();
			}
		}
#endif
		WIN_MANAGER->setSaveState(false);
	}


}

void ActionMaskListColor::redo()
{
	MaskInfo * info = m_pVolumeData->getMaskInfo(m_index);
	QTreeWidgetItem * item = m_maskList->topLevelItem(m_index);
	if (info && item)
	{
		m_preColor = info->color;
		info->color = m_color;
		item->setBackground(L_COL_COLOR, QBrush(QColor(m_color.r, m_color.g, m_color.b, 255)));

		WIN_MANAGER->forceUpdate2DViewData(false, true);
		WIN_MANAGER->renderLater_GridView(true);
		m_pVolumeData->setCurrentMaskIndex(m_index);
		MaskInfo * info = m_pVolumeData->getMaskInfo(m_index);
		std::vector<muint32> vecSelect;
		vecSelect.push_back(info->uid);
		m_pVolumeData->setMultiSelectUID(vecSelect);
		WIN_MANAGER->setSaveState(false);

		if (-1 != m_meshIndex)
		{
			MeshTab *tab = WIN_MANAGER->GetTab()->getMeshTab();
			if (tab)
				tab->UpdateSubWidget(m_meshIndex);

			VisualPrintMeshTab *pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
			if (pVisualPrintMeshTab)
				pVisualPrintMeshTab->UpdateSubWidget(m_meshIndex);
		}

#ifdef MULTI_DRAWCUT_MODE
		{
			DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();
			if (tab)
			{
				if (tab->contains(info->uid))
					tab->UpdateSeed();
			}
		}
#endif
	}
}






ActionMaskListMove::ActionMaskListMove(VOLUME_DATA* pVolumeData, MaskListWidget * maskList, muint32 source_index, muint32 dest_index, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_MOVE;
	m_id = s_id++;
	m_source_index = source_index;
	m_dest_index = dest_index;
	m_maskList = maskList;
}

bool ActionMaskListMove::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListMove::undo()
{
	if (m_maskList)
	{
		m_pVolumeData->moveMaskInfo(m_dest_index, m_source_index);
		m_pVolumeData->setCurrentMaskIndex(m_dest_index);
		MaskInfo * info = m_pVolumeData->getMaskInfo(m_dest_index);
		std::vector<muint32> vecSelect;
		vecSelect.push_back(info->uid);
		m_pVolumeData->setMultiSelectUID(vecSelect);
		m_maskList->UpdateByVolumeData();

		WIN_MANAGER->forceUpdate2DViewData(false, true);
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionMaskListMove::redo()
{
	if (m_maskList)
	{
		m_pVolumeData->moveMaskInfo(m_source_index, m_dest_index);
		m_pVolumeData->setCurrentMaskIndex(m_dest_index);
		MaskInfo * info = m_pVolumeData->getMaskInfo(m_dest_index);
		std::vector<muint32> vecSelect;
		vecSelect.push_back(info->uid);
		m_pVolumeData->setMultiSelectUID(vecSelect);
		m_maskList->UpdateByVolumeData();

		WIN_MANAGER->forceUpdate2DViewData(false, true);
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->setSaveState(false);
	}
}







ActionMaskListInvert::ActionMaskListInvert(VOLUME_DATA* pVolumeData, MaskInfo & maskinfo, int maskIndex, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_INVERT;
	m_id = s_id++;
	m_maskInfo = maskinfo;
	m_maskIndex = maskIndex;
	m_TAState = m_pVolumeData->getTAState(m_maskInfo.uid);
	//	m_fillCount = m_pVolumeData->voxelCount[m_maskInfo.uid];
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[m_maskInfo.uid];
	m_fillCount = m_pVolumeData->getVoxelCount(m_maskInfo.uid);
	m_orgbox[0] = m_pVolumeData->getBoundingBox(m_maskInfo.uid);
	m_orgbox[1].reset(true);
	m_first = false;
}

bool ActionMaskListInvert::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListInvert::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		mask * buff = m_pVolumeData->getMaskDataPoint(m_maskIndex);
		muint32 length = m_pVolumeData->getVolumeDataLength();

		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)buff, length);
			file.close();
		}
		m_pVolumeData->setVoxelCount(m_maskInfo.uid, m_fillCount);
		m_pVolumeData->setBoundingBox(m_maskInfo.uid, m_orgbox[0]);
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();
		//	m_pVolumeData->voxelCount[m_maskInfo.uid] = m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_maskInfo.uid] = m_orgbox[0];
		WIN_MANAGER->renderLater_GridView(false);
		m_pVolumeData->setTAState(m_maskInfo.uid, m_TAState);
		WIN_MANAGER->applyVoxelToUI(m_maskInfo.uid);
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionMaskListInvert::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		mask * buff = m_pVolumeData->getMaskDataPoint(m_maskIndex);
		muint32 length = m_pVolumeData->getVolumeDataLength();

		if (m_first == false)
		{
			m_first = true;

			QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
			QFile file(filename);
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)buff, length);
				file.close();
			}

			mask maskID = m_maskIndex == 0 ? m_maskInfo.mask_id : m_maskInfo.mask_id2;
			for (mint32 n = 0; n < length; n++)
			{
				if (buff[n] & maskID)
					buff[n] &= ~maskID;
				else
					buff[n] |= maskID;
			}
			m_pVolumeData->updateUIDBoundingBox(m_maskInfo.uid, true);
			//		m_orgbox[1] = m_pVolumeData->boundingBoxROI[m_maskInfo.uid];
			m_orgbox[1] = m_pVolumeData->getBoundingBox(m_maskInfo.uid);
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)buff, length);
				file.close();
			}
		}
		else
		{
			QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
			QFile file(filename);
			if (file.exists() && file.open(QIODevice::ReadOnly))
			{
				file.read((char*)buff, length);
				file.close();
			}
		}
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();
		//	m_pVolumeData->voxelCount[m_maskInfo.uid] = length - m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_maskInfo.uid] = m_orgbox[1];
		m_pVolumeData->setVoxelCount(m_maskInfo.uid, (length - m_fillCount));
		m_pVolumeData->setBoundingBox(m_maskInfo.uid, m_orgbox[1]);
		WIN_MANAGER->renderLater_GridView(false);
		m_pVolumeData->setTAState(m_maskInfo.uid, false);
		WIN_MANAGER->applyVoxelToUI(m_maskInfo.uid);
		WIN_MANAGER->setSaveState(false);
	}
}

ActionMaskListZFlip::ActionMaskListZFlip(VOLUME_DATA* pVolumeData, MaskInfo & maskinfo, int maskIndex, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_ZFLIP;
	m_id = s_id++;
	m_maskIndex = maskIndex;
	m_maskInfo = maskinfo;
	m_TAState = m_pVolumeData->getTAState(m_maskInfo.uid);
	m_first = false;
}

void ActionMaskListZFlip::undo()
{
	mask * buff = m_pVolumeData->getMaskDataPoint(m_maskIndex);
	muint32 dx = m_pVolumeData->getCX();
	muint32 dy = m_pVolumeData->getCY();
	muint32 dz = m_pVolumeData->getCZ();
	int zVal;
	int yVal;
	int index;
	int length = m_pVolumeData->getVolumeDataLength();
	int *flipindex;
	WIN_MANAGER->setRenderable(false);

	flipindex = new int[length];
	memset(flipindex, 0, sizeof(int)*length);
	mask maskID = m_maskIndex == 0 ? m_maskInfo.mask_id : m_maskInfo.mask_id2;
	for (int z = 0; z < dz; z++)
	{
		zVal = dz - z - 1;
		for (int y = 0; y < dy; y++)
		{
			for (int x = 0; x < dx; x++)
			{
				index = (z * dx * dy) + (y * dx) + x;
				if (buff[index] & maskID)
				{
					buff[index] &= ~maskID;
					flipindex[index] = 1;
				}
			}
		}
	}

	for (int z = 0; z < dz; z++)
	{
		zVal = dz - z - 1;

		for (int y = 0; y < dy; y++)
		{

			for (int x = 0; x < dx; x++)
			{
				index = (z * dx * dy) + (y * dx) + x;
				if (flipindex[index] == 1)
				{
					index = (zVal * dx * dy) + (y * dx) + x;
					buff[index] |= maskID;
				}
			}
		}
	}
	delete[]flipindex;
	m_pVolumeData->updateUIDBoundingBox(m_maskInfo.uid, true);
	m_pVolumeData->setTAState(m_maskInfo.uid, m_TAState);
	WIN_MANAGER->renderLater_GridView();
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->setRenderable(true);
	WIN_MANAGER->setSaveState(false);
}

void ActionMaskListZFlip::redo()
{
	mask * buff = m_pVolumeData->getMaskDataPoint(m_maskIndex);
	muint32 dx = m_pVolumeData->getCX();
	muint32 dy = m_pVolumeData->getCY();
	muint32 dz = m_pVolumeData->getCZ();
	int zVal;
	int index;
	int length = m_pVolumeData->getVolumeDataLength();
	int *flipindex;
	WIN_MANAGER->setRenderable(false);

	flipindex = new int[length];
	memset(flipindex, 0, sizeof(int)*length);
	mask maskID = m_maskIndex == 0 ? m_maskInfo.mask_id : m_maskInfo.mask_id2;
	for (int z = 0; z < dz; z++)
	{
		for (int y = 0; y < dy; y++)
		{
			for (int x = 0; x < dx; x++)
			{
				index = (z * dx * dy) + (y * dx) + x;
				if (buff[index] & maskID)
				{
					buff[index] &= ~maskID;
					flipindex[index] = 1;
				}
			}
		}
	}

	for (int z = 0; z < dz; z++)
	{
		zVal = dz - z - 1;
		for (int y = 0; y < dy; y++)
		{
			for (int x = 0; x < dx; x++)
			{
				index = (z * dx * dy) + (y * dx) + x;
				if (flipindex[index] == 1)
				{
					index = (zVal * dx * dy) + (y * dx) + x;
					buff[index] |= maskID;
				}
			}
		}
	}
	delete[]flipindex;
	m_pVolumeData->updateUIDBoundingBox(m_maskInfo.uid, true);
	m_pVolumeData->forceUpdateMaskVolume();
	m_pVolumeData->setTAState(m_maskInfo.uid, false);
	WIN_MANAGER->renderLater_GridView();
	WIN_MANAGER->setRenderable(true);
	WIN_MANAGER->setSaveState(false);
	//	WIN_MANAGER->renderLater_All();
}

bool ActionMaskListZFlip::mergeWith(const QUndoCommand * command)
{
	return false;
}






ActionMaskListYFlip::ActionMaskListYFlip(VOLUME_DATA* pVolumeData, MaskInfo & maskinfo, int maskIndex, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_YFLIP;
	m_id = s_id++;
	m_maskIndex = maskIndex;
	m_maskInfo = maskinfo;
	m_TAState = m_pVolumeData->getTAState(m_maskInfo.uid);
	m_first = false;
}

void ActionMaskListYFlip::undo()
{
	mask * buff = m_pVolumeData->getMaskDataPoint(m_maskIndex);
	muint32 dx = m_pVolumeData->getCX();
	muint32 dy = m_pVolumeData->getCY();
	muint32 dz = m_pVolumeData->getCZ();
	int zVal;
	int yVal;
	int index;
	int length = m_pVolumeData->getVolumeDataLength();
	int *flipindex;
	WIN_MANAGER->setRenderable(false);

	flipindex = new int[length];
	memset(flipindex, 0, sizeof(int)*length);
	mask maskID = m_maskIndex == 0 ? m_maskInfo.mask_id : m_maskInfo.mask_id2;
	for (int z = 0; z < dz; z++)
	{
		for (int y = 0; y < dy; y++)
		{

			for (int x = 0; x < dx; x++)
			{
				index = (z * dx * dy) + (y * dx) + x;
				if (buff[index] & maskID)
				{
					buff[index] &= ~maskID;
					flipindex[index] = 1;
				}
			}
		}
	}

	for (int z = 0; z < dz; z++)
	{
		for (int y = 0; y < dy; y++)
		{
			yVal = dy - y - 1;
			for (int x = 0; x < dx; x++)
			{
				index = (z * dx * dy) + (y * dx) + x;
				if (flipindex[index] == 1)
				{
					index = (z * dx * dy) + (yVal * dx) + x;
					buff[index] |= maskID;
				}
			}
		}
	}
	SAFE_DELETES(flipindex);
	m_pVolumeData->updateUIDBoundingBox(m_maskInfo.uid, true);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->updatePlaneData_all();
	m_pVolumeData->setTAState(m_maskInfo.uid, m_TAState);
	WIN_MANAGER->renderLater_GridView();
	WIN_MANAGER->setRenderable(true);
	WIN_MANAGER->setSaveState(false);
}

void ActionMaskListYFlip::redo()
{
	mask * buff = m_pVolumeData->getMaskDataPoint(m_maskIndex);
	muint32 dx = m_pVolumeData->getCX();
	muint32 dy = m_pVolumeData->getCY();
	muint32 dz = m_pVolumeData->getCZ();
	int yVal;
	int index;
	int length = m_pVolumeData->getVolumeDataLength();
	int *flipindex;
	WIN_MANAGER->setRenderable(false);

	flipindex = new int[length];
	memset(flipindex, 0, sizeof(int)*length);
	mask maskID = m_maskIndex == 0 ? m_maskInfo.mask_id : m_maskInfo.mask_id2;
	for (int z = 0; z < dz; z++)
	{

		for (int y = 0; y < dy; y++)
		{
			for (int x = 0; x < dx; x++)
			{
				index = (z * dx * dy) + (y * dx) + x;
				if (buff[index] & maskID)
				{
					buff[index] &= ~maskID;
					flipindex[index] = 1;
				}
			}
		}
	}

	for (int z = 0; z < dz; z++)
	{

		for (int y = 0; y < dy; y++)
		{
			yVal = dy - y - 1;
			for (int x = 0; x < dx; x++)
			{
				index = (z * dx * dy) + (y * dx) + x;
				if (flipindex[index] == 1)
				{
					index = (z * dx * dy) + (yVal * dx) + x;
					buff[index] |= maskID;
				}
			}
		}
	}
	SAFE_DELETES(flipindex);
	m_pVolumeData->updateUIDBoundingBox(m_maskInfo.uid, true);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->updatePlaneData_all();
	m_pVolumeData->setTAState(m_maskInfo.uid, false);
	WIN_MANAGER->renderLater_GridView();
	WIN_MANAGER->setRenderable(true);
	WIN_MANAGER->setSaveState(false);
	//	WIN_MANAGER->renderLater_All();
}

bool ActionMaskListYFlip::mergeWith(const QUndoCommand * command)
{
	return false;
}





ActionMaskListXFlip::ActionMaskListXFlip(VOLUME_DATA* pVolumeData, MaskInfo & maskinfo, int maskIndex, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_XFLIP;
	m_id = s_id++;
	m_maskIndex = maskIndex;
	m_maskInfo = maskinfo;
	m_TAState = m_pVolumeData->getTAState(m_maskInfo.uid);
	m_first = false;
}

void ActionMaskListXFlip::undo()
{
	mask * buff = m_pVolumeData->getMaskDataPoint(m_maskIndex);
	muint32 dx = m_pVolumeData->getCX();
	muint32 dy = m_pVolumeData->getCY();
	muint32 dz = m_pVolumeData->getCZ();
	int xVal;
	int index;
	int length = m_pVolumeData->getVolumeDataLength();
	int *flipindex;
	WIN_MANAGER->setRenderable(false);

	flipindex = new int[length];
	memset(flipindex, 0, sizeof(int)*length);
	mask maskID = m_maskIndex == 0 ? m_maskInfo.mask_id : m_maskInfo.mask_id2;
	for (int z = 0; z < dz; z++)
	{
		for (int y = 0; y < dy; y++)
		{

			for (int x = 0; x < dx; x++)
			{
				index = (z * dx * dy) + (y * dx) + x;
				if (buff[index] & maskID)
				{
					buff[index] &= ~maskID;
					flipindex[index] = 1;
				}
			}
		}
	}

	for (int z = 0; z < dz; z++)
	{
		for (int y = 0; y < dy; y++)
		{
			
			for (int x = 0; x < dx; x++)
			{
				index = (z * dx * dy) + (y * dx) + x;
				xVal = dx - x - 1;
				if (flipindex[index] == 1)
				{
					index = (z * dx * dy) + (y * dx) + xVal;
					buff[index] |= maskID;
				}
			}
		}
	}
	SAFE_DELETES(flipindex);
	m_pVolumeData->updateUIDBoundingBox(m_maskInfo.uid, true);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->updatePlaneData_all();
	m_pVolumeData->setTAState(m_maskInfo.uid, m_TAState);
	WIN_MANAGER->renderLater_GridView();
	WIN_MANAGER->setRenderable(true);
	WIN_MANAGER->setSaveState(false);
}

void ActionMaskListXFlip::redo()
{
	mask * buff = m_pVolumeData->getMaskDataPoint(m_maskIndex);
	muint32 dx = m_pVolumeData->getCX();
	muint32 dy = m_pVolumeData->getCY();
	muint32 dz = m_pVolumeData->getCZ();
	int xVal;
	int index;
	int length = m_pVolumeData->getVolumeDataLength();
	int *flipindex;
	WIN_MANAGER->setRenderable(false);

	flipindex = new int[length];
	memset(flipindex, 0, sizeof(int)*length);
	mask maskID = m_maskIndex == 0 ? m_maskInfo.mask_id : m_maskInfo.mask_id2;
	for (int z = 0; z < dz; z++)
	{

		for (int y = 0; y < dy; y++)
		{
			for (int x = 0; x < dx; x++)
			{
				index = (z * dx * dy) + (y * dx) + x;
				if (buff[index] & maskID)
				{
					buff[index] &= ~maskID;
					flipindex[index] = 1;
				}
			}
		}
	}

	for (int z = 0; z < dz; z++)
	{

		for (int y = 0; y < dy; y++)
		{
			
			for (int x = 0; x < dx; x++)
			{
				xVal = dx - x - 1;
				index = (z * dx * dy) + (y * dx) + x;
				if (flipindex[index] == 1)
				{
					index = (z * dx * dy) + (y * dx) + xVal;
					buff[index] |= maskID;
				}
			}
		}
	}
	SAFE_DELETES(flipindex);
	m_pVolumeData->updateUIDBoundingBox(m_maskInfo.uid, true);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->updatePlaneData_all();
	m_pVolumeData->setTAState(m_maskInfo.uid, false);
	WIN_MANAGER->renderLater_GridView();
	WIN_MANAGER->setRenderable(true);
	WIN_MANAGER->setSaveState(false);
	//	WIN_MANAGER->renderLater_All();
}

bool ActionMaskListXFlip::mergeWith(const QUndoCommand * command)
{
	return false;
}








ActionMaskListDifference::ActionMaskListDifference(VOLUME_DATA* pVolumeData, MaskInfo & maskinfoA, MaskInfo & maskinfoB, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_INTERSECT;
	m_id = s_id++;
	m_maskInfoA = maskinfoA;
	m_maskInfoB = maskinfoB;
	m_maskIndex = m_maskInfoA.uid >= MASK_SECOND_MAX ? (m_maskInfoA.uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	m_maskIndexB = m_maskInfoB.uid >= MASK_SECOND_MAX ? (m_maskInfoB.uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	m_TAState = m_pVolumeData->getTAState(m_maskInfoA.uid);
	m_fillCount = m_pVolumeData->getVoxelCount(maskinfoA.uid);
	m_emptyCount = 0;
	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[maskinfoA.uid];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(maskinfoA.uid);
	m_orgbox[1].reset(true);
	m_first = false;
}

bool ActionMaskListDifference::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListDifference::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		mask * buff = m_pVolumeData->getMaskDataPoint(m_maskIndex);
		muint32 length = m_pVolumeData->getVolumeDataLength();

		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)buff, length);
			file.close();
		}
		//	m_pVolumeData->voxelCount[m_maskInfoA.uid] = m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_maskInfoA.uid] = m_orgbox[0];
		m_pVolumeData->setVoxelCount(m_maskInfoA.uid, m_fillCount);
		m_pVolumeData->setBoundingBox(m_maskInfoA.uid, m_orgbox[0]);
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(false);
		m_pVolumeData->setTAState(m_maskInfoA.uid, m_TAState);
		WIN_MANAGER->applyVoxelToUI(m_maskInfoA.uid);
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionMaskListDifference::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		mask * buff = m_pVolumeData->getMaskDataPoint(m_maskIndex);
		mask * buff2;

		if (m_maskIndex == m_maskIndexB)
			buff2 = buff;
		else
			buff2 = m_pVolumeData->getMaskDataPoint(m_maskIndexB);

		muint32 length = m_pVolumeData->getVolumeDataLength();

		if (m_first == false)
		{
			m_first = true;

			QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
			QFile file(filename);
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)buff, length);
				file.close();
			}
			mask maskA = m_maskIndex == 0 ? m_maskInfoA.mask_id : m_maskInfoA.mask_id2;
			mask maskB = m_maskIndexB == 0 ? m_maskInfoB.mask_id : m_maskInfoB.mask_id2;
			m_emptyCount = 0;
			for (mint32 n = 0; n < length; n++)
			{
				if (maskA & buff[n])
				{
					if (buff2[n] & maskB)
					{
						buff[n] &= ~maskA;
						m_emptyCount++;
					}
				}
			}
			m_pVolumeData->updateUIDBoundingBox(m_maskInfoA.uid, true);
			//		m_orgbox[1] = m_pVolumeData->boundingBoxROI[m_maskInfoA.uid];
			m_orgbox[1] = m_pVolumeData->getBoundingBox(m_maskInfoA.uid);
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)buff, length);
				file.close();
			}
		}
		else
		{
			QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
			QFile file(filename);
			if (file.exists() && file.open(QIODevice::ReadOnly))
			{
				file.read((char*)buff, length);
				file.close();
			}
		}
		//	m_pVolumeData->voxelCount[m_maskInfoA.uid] -= m_emptyCount;
		//	m_pVolumeData->boundingBoxROI[m_maskInfoA.uid] = m_orgbox[1];
		m_pVolumeData->setVoxelCount(m_maskInfoA.uid, -m_emptyCount, false);
		m_pVolumeData->setBoundingBox(m_maskInfoA.uid, m_orgbox[1]);
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(false);
		m_pVolumeData->setTAState(m_maskInfoA.uid, false);
		WIN_MANAGER->applyVoxelToUI(m_maskInfoA.uid);
		WIN_MANAGER->setSaveState(false);
	}
}









ActionMaskListIntersection::ActionMaskListIntersection(VOLUME_DATA* pVolumeData, MaskInfo & maskinfoA, MaskInfo & maskinfoB, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_INTERSECTION;
	m_id = s_id++;

	m_maskInfoA = maskinfoA;
	m_maskInfoB = maskinfoB;
	m_maskIndexA = m_maskInfoA.uid >= MASK_SECOND_MAX ? (m_maskInfoA.uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	m_maskIndexB = m_maskInfoB.uid >= MASK_SECOND_MAX ? (m_maskInfoB.uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	m_TAState = m_pVolumeData->getTAState(m_maskInfoA.uid);
	//	m_fillCount = m_pVolumeData->voxelCount[maskinfoA.uid];
	m_fillCount = m_pVolumeData->getVoxelCount(maskinfoA.uid);
	m_emptyCount = 0;
	m_first = false;

	//	m_orgbox[0] = m_pVolumeData->boundingBoxROI[maskinfoA.uid];
	m_orgbox[0] = m_pVolumeData->getBoundingBox(maskinfoA.uid);
	m_orgbox[1].reset(true);
}

ActionMaskListIntersection::~ActionMaskListIntersection()
{

}

bool ActionMaskListIntersection::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListIntersection::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		mask * buff = m_pVolumeData->getMaskDataPoint(m_maskIndexA);
		muint32 length = m_pVolumeData->getVolumeDataLength();

		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)buff, length);
			file.close();
		}
		//	m_pVolumeData->voxelCount[m_maskInfoA.uid] = m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_maskInfoA.uid] = m_orgbox[0];
		m_pVolumeData->setVoxelCount(m_maskInfoA.uid, m_fillCount);
		m_pVolumeData->setBoundingBox(m_maskInfoA.uid, m_orgbox[0]);
		WIN_MANAGER->applyVoxelToUI(m_maskInfoA.uid);
		m_pVolumeData->setTAState(m_maskInfoA.uid, m_TAState);
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionMaskListIntersection::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		mask* result = m_pVolumeData->getMaskDataPoint(m_maskIndexA);
		mask* buffForScondMaskMax;

		if (m_maskIndexA == m_maskIndexB)
			buffForScondMaskMax = result;
		else
			buffForScondMaskMax = m_pVolumeData->getMaskDataPoint(m_maskIndexB);

		muint32 length = m_pVolumeData->getVolumeDataLength();
		if (m_first == false)
		{
			m_first = true;

			QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
			QFile file(filename);

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)result, length);
				file.close();
			}

			mask maskA = m_maskIndexA == 0 ? m_maskInfoA.mask_id : m_maskInfoA.mask_id2;
			mask maskB = m_maskIndexB == 0 ? m_maskInfoB.mask_id : m_maskInfoB.mask_id2;

			for (mint32 n = 0; n < length; n++)
			{
				if (maskA & result[n])
				{
					if (!(buffForScondMaskMax[n] & maskB))
					{
						result[n] &= ~maskA;
						m_emptyCount++;
					}
				}
			}

			m_pVolumeData->updateUIDBoundingBox(m_maskInfoA.uid, true);
			//	m_orgbox[1] = m_pVolumeData->boundingBoxROI[m_maskInfoA.uid];
			m_orgbox[1] = m_pVolumeData->getBoundingBox(m_maskInfoA.uid);


			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)result, length);
				file.close();
			}

		}
		else
		{
			QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
			QFile file(filename);
			if (file.exists() && file.open(QIODevice::ReadOnly))
			{
				file.read((char*)result, length);
				file.close();
			}
		}
	}
	//	m_pVolumeData->voxelCount[m_maskInfoA.uid] -= m_emptyCount;
	//	m_pVolumeData->boundingBoxROI[m_maskInfoA.uid] = m_orgbox[1];
	m_pVolumeData->setVoxelCount(m_maskInfoA.uid, -m_emptyCount, false);
	m_pVolumeData->setBoundingBox(m_maskInfoA.uid, m_orgbox[1]);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->updatePlaneData_all();
	WIN_MANAGER->updateUI();
	WIN_MANAGER->renderLater_GridView(false);
	m_pVolumeData->setTAState(m_maskInfoA.uid, false);
	WIN_MANAGER->applyVoxelToUI(m_maskInfoA.uid);
	WIN_MANAGER->setSaveState(false);
}

void ActionMaskListIntersection::Do()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		mask* result = m_pVolumeData->getMaskDataPoint(m_maskIndexA);
		mask* buffForScondMaskMax;

		if (m_maskIndexA == m_maskIndexB)
			buffForScondMaskMax = result;
		else
			buffForScondMaskMax = m_pVolumeData->getMaskDataPoint(m_maskIndexB);

		muint32 length = m_pVolumeData->getVolumeDataLength();


		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)result, length);
			file.close();
		}

	}
	//	m_pVolumeData->voxelCount[m_maskInfoA.uid] -= m_emptyCount;
	//	m_pVolumeData->boundingBoxROI[m_maskInfoA.uid] = m_orgbox[1];
	m_pVolumeData->setVoxelCount(m_maskInfoA.uid, -m_emptyCount, false);
	m_pVolumeData->setBoundingBox(m_maskInfoA.uid, m_orgbox[1]);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->updatePlaneData_all();
	WIN_MANAGER->updateUI();
	WIN_MANAGER->renderLater_GridView(false);
	m_pVolumeData->setTAState(m_maskInfoA.uid, false);
	WIN_MANAGER->applyVoxelToUI(m_maskInfoA.uid);
	WIN_MANAGER->setSaveState(false);
}







ActionMaskListMerge::ActionMaskListMerge(DataContext* pDataContext, std::vector<muint32> & list, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pDataContext = pDataContext;

	static int s_id = ACT_ID_MASKLIST_MERGE;
	m_id = s_id++;
	m_fillCount = NULL;
	m_etcbox = NULL;
	m_taseq = NULL;
	m_TAState = NULL;
	m_mesh = NULL;
	m_seed = NULL;
	memset(m_mask, 0, sizeof(m_mask));
	memset(m_orgCount, 0, sizeof(m_orgCount));
	muint32 cnt = m_pDataContext->volume_data.getMaskInfoListCnt();
	m_maskPCount[0] = m_pDataContext->volume_data.GetMaskPointCount();
	std::sort(list.begin(), list.end(), std::less<muint32>());//오름차순(0,1,2,,,,)
	std::vector<muint32> mesh_vector;

	mesh_vector.reserve(list.size() - 1);

	for (int n = 1; n < list.size(); n++)
	{
		if (cnt > list[n])
		{
			MaskInfo * info = m_pDataContext->volume_data.getMaskInfo(list[n]);

			if (info)
			{
				int maskIndex = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
				m_uid_vector.push_back(info->uid);
				m_mask[maskIndex] |= ((0 == maskIndex) ? info->mask_id : info->mask_id2);
				m_info_map.insert(list[n], *info);
				mesh_vector.push_back(info->uid);
			}
		}
	}

	if (!mesh_vector.empty())
	{
		m_mesh = new ActionConnectMeshes(m_pDataContext, mesh_vector, false);
#ifdef MULTI_DRAWCUT_MODE
		m_seed = new ActionDelDrawSeed(mesh_vector);
#endif
	}

	std::sort(m_uid_vector.begin(), m_uid_vector.end(), std::less<unsigned int>());//내림차순 정렬(2,1,0,,,,)

	m_info = m_pDataContext->volume_data.getMaskInfo(list[0]);
}

ActionMaskListMerge::~ActionMaskListMerge()
{
	SAFE_DELETES(m_fillCount);
	SAFE_DELETES(m_etcbox);
	SAFE_DELETES(m_taseq);
	SAFE_DELETES(m_TAState);
	SAFE_DELETE(m_mesh);
}

bool ActionMaskListMerge::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListMerge::undo()
{
	bool r = true;
	bool existSurface = false;
	bool TAState = m_TAState[0];
	std::sort(m_uid_vector.begin(), m_uid_vector.end(), std::less<unsigned int>());//내림차순 정렬

	int i = 0;
	for (auto f = m_uid_vector.begin(); f != m_uid_vector.end(); f++)
	{
		if (i < m_mv_id.size())
		{
			int _uid = (*f);
			auto _chInfo = m_mv_id.at(i);
			MaskInfo *chInfo = m_pDataContext->volume_data.getMaskInfo(_uid, true);

			chInfo->mask_id = _chInfo.uid >= MASK_SECOND_MAX ? VM_MASK7 : _chInfo._m;
			chInfo->mask_id2 = _chInfo.uid >= MASK_SECOND_MAX ? _chInfo._m : 0;

			WIN_MANAGER->ChangeUID(chInfo->uid, _chInfo.uid);

			m_pDataContext->volume_data.setVoxelCount(_chInfo.uid, m_pDataContext->volume_data.getVoxelCount(chInfo->uid));
			m_pDataContext->volume_data.setBoundingBox(_chInfo.uid, m_pDataContext->volume_data.getBoundingBox(chInfo->uid));
			chInfo->uid = _chInfo.uid;
		}
		i++;
	}
	i = 0;
	AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();
	for (auto f = m_info_map.begin(); f != m_info_map.end(); f++)
	{
		MaskInfo info = f.value();
		m_pDataContext->volume_data.setVoxelCount(info.uid, m_fillCount[i]);
		m_pDataContext->volume_data.setBoundingBox(info.uid, m_etcbox[i]);
		m_pDataContext->volume_data.setTAState(info.uid, m_TAState[i + 1]);
		m_pDataContext->volume_data.insertMaskInfo(f.key(), info);

		if ((m_taseq[i] != -1) && tab)
			tab->SetResultFromFile(info.uid, m_taseq[i]);

		i++;
	}

	SAFE_DELETES(m_fillCount);
	SAFE_DELETES(m_etcbox);
	SAFE_DELETES(m_taseq);
	SAFE_DELETES(m_TAState);

	m_pDataContext->volume_data.setCurrentMaskIndex(0);
	MaskInfo * info = m_pDataContext->volume_data.getMaskInfo(0);
	std::vector<muint32> vecSelect;
	vecSelect.push_back(info->uid);
	m_pDataContext->volume_data.setMultiSelectUID(vecSelect);

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			for (int i = 0; i < m_maskPCount[0]; i++)
				file.read((char*)m_pDataContext->volume_data.getMaskDataPoint(i), m_pDataContext->volume_data.getVolumeDataLength());
			file.close();
		}
	}
	if (m_mesh)
		m_mesh->undo();

	m_pDataContext->volume_data.setVoxelCount(m_info->uid, m_orgCount[0]);
	m_pDataContext->volume_data.setBoundingBox(m_info->uid, m_orgbox[0]);
	m_pDataContext->volume_data.setTAState(m_info->uid, TAState);

	if (r)
	{
		m_pDataContext->volume_data.forceUpdateMaskVolume();
		WIN_MANAGER->updateUI();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionMaskListMerge::redo()
{
	std::sort(m_uid_vector.begin(), m_uid_vector.end(), std::less<unsigned int>());//내림차순 정렬
	QDir dir(STRING_MANAGER->cacheFilePath);
	bool existSurface = true;
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);

		m_etcbox = new BoundingBoxI[m_info_map.size()];
		m_fillCount = new int[m_info_map.size()];
		m_taseq = new int[m_info_map.size()];
		m_TAState = new bool[m_info_map.size() + 1];
		int i = 0;
		m_TAState[i] = m_pDataContext->volume_data.getTAState(m_info->uid);
		for (auto f = m_info_map.begin(); f != m_info_map.end(); f++)
		{
			MaskInfo chInfo = f.value();
			m_fillCount[i] = m_pDataContext->volume_data.getVoxelCount(chInfo.uid);
			m_etcbox[i] = m_pDataContext->volume_data.getBoundingBox(chInfo.uid);
			m_TAState[i + 1] = m_pDataContext->volume_data.getTAState(chInfo.uid);
			if (WIN_MANAGER->GetTab())
			{
				AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();

				if (tab)
				{
					m_taseq[i] = tab->existResult(chInfo.uid, true);
					tab->DeleteResult(chInfo.uid, true);
				}
			}
			i++;
		}
		m_mv_id.clear();

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			bool r = m_pDataContext->volume_data.delMaskInfos(m_uid_vector, m_mv_id, existSurface);
			m_pDataContext->volume_data.setCurrentMaskIndex(0);
			MaskInfo * info = m_pDataContext->volume_data.getMaskInfo(0);
			std::vector<muint32> vecSelect;
			vecSelect.push_back(info->uid);
			m_pDataContext->volume_data.setMultiSelectUID(vecSelect);

			for (int i = 0; i < m_maskPCount[1]; i++)
				file.read((char*)m_pDataContext->volume_data.getMaskDataPoint(i), m_pDataContext->volume_data.getVolumeDataLength());
			file.close();
		}
		else
		{
			m_maskIndex = m_info->uid >= MASK_SECOND_MAX ? (m_info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));

			if (file.open(QIODevice::WriteOnly))
			{
				for (int i = 0; i < m_maskPCount[0]; i++)
					file.write((char*)m_pDataContext->volume_data.getMaskDataPoint(i), m_pDataContext->volume_data.getVolumeDataLength());
				file.close();
			}

			m_pDataContext->volume_data.createTempMaskData();
			for (int i = 0; i < 4; i++)
			{
				if (m_mask[i] == 0) continue;
				m_pDataContext->volume_data.moveMaskToTempMask(m_maskIndex == 0 ? m_info->mask_id : m_info->mask_id2, m_mask[i], m_maskIndex, i);
			}

			m_orgCount[1] = m_orgCount[0] = m_pDataContext->volume_data.getVoxelCount(m_info->uid);
			m_orgbox[0] = m_pDataContext->volume_data.getBoundingBox(m_info->uid);
			m_pDataContext->volume_data.applyTempMaskBitAdd(m_maskIndex == 0 ? m_info->mask_id : m_info->mask_id2, m_maskIndex);
			m_orgCount[1] += m_pDataContext->volume_data.fillMaskCount;
			m_pDataContext->volume_data.updateUIDBoundingBox(m_info->uid, true);
			m_orgbox[1] = m_pDataContext->volume_data.getBoundingBox(m_info->uid);

			bool r = m_pDataContext->volume_data.delMaskInfos(m_uid_vector, m_mv_id, existSurface);
			m_pDataContext->volume_data.setCurrentMaskIndex(0);
			MaskInfo * info = m_pDataContext->volume_data.getMaskInfo(0);
			std::vector<muint32> vecSelect;
			vecSelect.push_back(info->uid);
			m_pDataContext->volume_data.setMultiSelectUID(vecSelect);
			m_maskPCount[1] = m_pDataContext->volume_data.GetMaskPointCount();
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				for (int i = 0; i < m_maskPCount[1]; i++)
					file.write((char*)m_pDataContext->volume_data.getMaskDataPoint(i), m_pDataContext->volume_data.getVolumeDataLength());
				file.close();
			}
		}

		if (m_mesh)
			m_mesh->redo();

		m_pDataContext->volume_data.setVoxelCount(m_info->uid, m_orgCount[1]);
		m_pDataContext->volume_data.setBoundingBox(m_info->uid, m_orgbox[1]);
		m_pDataContext->volume_data.setTAState(m_info->uid, false);
		m_pDataContext->volume_data.forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->updateUI();
		WIN_MANAGER->renderLater_GridView();
	}

	WIN_MANAGER->setSaveState(false);
}

ActionMaskListAddRawFile::ActionMaskListAddRawFile(VOLUME_DATA* pVolumeData, const QString& filePath, QUndoCommand *parent)
	: QUndoCommand(parent),
	m_filePath(filePath)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_ADD_RAWFILE;
	m_id = s_id++;
	m_list_index = 0;
	m_isFirst = true;
	m_del = false;
	m_maskIndex = 0;
	m_fillCount = 0;
}

bool ActionMaskListAddRawFile::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListAddRawFile::undo()
{
	if (m_del)
	{
		bool r = m_pVolumeData->delMaskInfo(m_list_index);
		if (r)
		{
			WIN_MANAGER->updateUI();
			WIN_MANAGER->renderLater_GridView(false);
		}
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
			WIN_MANAGER->renderLater_GridView(false);
		}
	}
	m_pVolumeData->forceUpdateMaskVolume();
	//	m_pVolumeData->voxelCount[m_info.uid] = 0;
	//	m_pVolumeData->boundingBoxROI[m_info.uid].reset(true);
	m_pVolumeData->setVoxelCount(m_info.uid, 0);
	m_pVolumeData->setBoundingBox(m_info.uid, BoundingBoxI(), true);
	WIN_MANAGER->applyVoxelToUI(m_info.uid);
	WIN_MANAGER->setSaveState(false);
}

void ActionMaskListAddRawFile::redo()
{
	if (m_isFirst == true)
	{
		m_isFirst = false;
		//m_del = m_pVolumeData->createMaskInfo();
		QString fileName = QFileInfo(m_filePath).baseName();
		m_del = m_pVolumeData->createMaskInfoWithName(fileName);
		m_list_index = m_pVolumeData->getCurrentMaskInfoID();
		m_info = *m_pVolumeData->getCurrentMaskInfo();
		m_maskIndex = m_pVolumeData->getCurrentMaskIndex();
	}
	else if (m_del)
	{
		m_pVolumeData->insertMaskInfo(m_list_index, m_info);
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			m_pVolumeData->applyTempMaskBitChange(m_maskIndex == 0 ? m_info.mask_id : m_info.mask_id2, m_maskIndex, 1);
			m_pVolumeData->updateUIDBoundingBox(m_info.uid, true);
			//	m_orgbox = m_pVolumeData->boundingBoxROI[m_info.uid];
			m_orgbox = m_pVolumeData->getBoundingBox(m_info.uid);
			m_fillCount = m_pVolumeData->fillMaskCount;
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}
		}
		//	m_pVolumeData->voxelCount[m_info.uid] = m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_info.uid] = m_orgbox;
		m_pVolumeData->setVoxelCount(m_info.uid, m_fillCount);
		m_pVolumeData->setBoundingBox(m_info.uid, m_orgbox);
		WIN_MANAGER->renderLater_GridView(false);
	}

	WIN_MANAGER->updateUI(true, m_list_index);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->renderLater_GridView(false);
	WIN_MANAGER->setSaveState(false);
}

ActionMaskListDels::ActionMaskListDels(VOLUME_DATA* pVolumeData, std::vector<muint32>& list, bool bDeleteAll, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_DELS;
	m_id = s_id++;
	m_fillCount = NULL;
	m_etcbox = NULL;
	m_taseq = NULL;
	m_TAState = NULL;
	m_mesh = NULL;
	memset(m_mask, 0, sizeof(m_mask));
	muint32 cnt = m_pVolumeData->getMaskInfoListCnt();
	m_maskPCount[0] = m_pVolumeData->GetMaskPointCount();
	m_bDeleteAll = bDeleteAll;
	DrawcutTab* tab = nullptr;

#ifdef MULTI_DRAWCUT_MODE
	tab = WIN_MANAGER->GetTab()->getDrawTab();
#endif

	std::sort(list.begin(), list.end(), std::less<muint32>());//오름차순(0,1,2,,,,)

	for (int n = 0; n < list.size(); n++)
	{
		if (cnt > list[n])
		{
			MaskInfo * info = m_pVolumeData->getMaskInfo(list[n]);

			if (info)
			{
				int maskIndex = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
				m_uid_vector.push_back(info->uid);
				m_mask[maskIndex] |= ((0 == maskIndex) ? info->mask_id : info->mask_id2);
				m_info_map.insert(list[n], *info);

				// 211026 허 건과장
				// Mess 리스트 영향으로 인한 주석처리
				//if (info->meshConnected)
				//{
				//	mint8 mUID = m_pVolumeData->GetMeshUID(info->uid);

				//	if (mUID != -1)
				//		m_mesh_vector.push_back(mUID);
				//}

				if (tab)
				{
					if (tab->contains(info->uid))
						m_seed_vector.push_back(info->uid);
				}

			}
		}
	}
	std::sort(m_uid_vector.begin(), m_uid_vector.end(), std::less<unsigned int>());//내림차순 정렬(2,1,0,,,,)
}

ActionMaskListDels::~ActionMaskListDels()
{
	SAFE_DELETES(m_fillCount);
	SAFE_DELETES(m_etcbox);
	SAFE_DELETES(m_taseq);
	SAFE_DELETES(m_TAState);
	SAFE_DELETE(m_mesh);
}

void ActionMaskListDels::undo()
{
	bool r = true;
	bool existSurface = true;
	std::sort(m_uid_vector.begin(), m_uid_vector.end(), std::less<unsigned int>());//내림차순 정렬

	int i = 0;
	for (auto f = m_uid_vector.begin(); f != m_uid_vector.end(); f++)
	{
		if (i < m_mv_id.size())
		{
			int _uid = (*f);
			auto _chInfo = m_mv_id.at(i);
			MaskInfo *chInfo = m_pVolumeData->getMaskInfo(_uid, true);

			chInfo->mask_id = _chInfo.uid >= MASK_SECOND_MAX ? VM_MASK7 : _chInfo._m;
			chInfo->mask_id2 = _chInfo.uid >= MASK_SECOND_MAX ? _chInfo._m : 0;

			WIN_MANAGER->ChangeUID(chInfo->uid, _chInfo.uid);

			m_pVolumeData->setVoxelCount(_chInfo.uid, m_pVolumeData->getVoxelCount(chInfo->uid));
			m_pVolumeData->setBoundingBox(_chInfo.uid, m_pVolumeData->getBoundingBox(chInfo->uid));
			chInfo->uid = _chInfo.uid;
		}
		i++;
	}
	i = 0;
	AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();
	DrawcutTab* tab2 = nullptr;

#ifdef MULTI_DRAWCUT_MODE
	tab2 = WIN_MANAGER->GetTab()->getDrawTab();
#endif
	for (auto f = m_info_map.begin(); f != m_info_map.end(); f++)
	{
		MaskInfo info = f.value();
		m_pVolumeData->setVoxelCount(info.uid, m_fillCount[i]);
		m_pVolumeData->setBoundingBox(info.uid, m_etcbox[i]);
		m_pVolumeData->setTAState(info.uid, m_TAState[i]);
		m_pVolumeData->insertMaskInfo(f.key(), info);

		if ((m_taseq[i] != -1) && tab)
			tab->SetResultFromFile(info.uid, m_taseq[i]);

		if (tab2 && m_seed_vector.contains(info.uid))
			tab2->AddSeed(info.uid);

		i++;
	}

	SAFE_DELETES(m_fillCount);
	SAFE_DELETES(m_etcbox);
	SAFE_DELETES(m_taseq);
	SAFE_DELETES(m_TAState);

	m_pVolumeData->setCurrentMaskIndex(0);
	MaskInfo * info = m_pVolumeData->getMaskInfo(0);
	std::vector<muint32> vecSelect;
	vecSelect.push_back(info->uid);
	m_pVolumeData->setMultiSelectUID(vecSelect);

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			for (int i = 0; i < m_maskPCount[0]; i++)
				file.read((char*)m_pVolumeData->getMaskDataPoint(i), m_pVolumeData->getVolumeDataLength());
			file.close();
		}
	}

	// 211026 허 건과장
	// Mess 리스트 영향으로 인한 주석처리
	//if (m_mesh)
	//	m_mesh->undo();

	if (r)
	{
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updateUI();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionMaskListDels::redo()
{
	//	std::sort(m_list_index.begin(), m_list_index.end(), std::less<muint32>());
	std::sort(m_uid_vector.begin(), m_uid_vector.end(), std::less<unsigned int>());//내림차순 정렬
	QDir dir(STRING_MANAGER->cacheFilePath);
	bool existSurface = true;
	bool r = false;
	if (dir.exists())
	{
		m_etcbox = new BoundingBoxI[m_info_map.size()];
		m_fillCount = new int[m_info_map.size()];
		m_taseq = new int[m_info_map.size()];
		m_TAState = new bool[m_info_map.size()];
		int i = 0;
		for (auto f = m_info_map.begin(); f != m_info_map.end(); f++)
		{
			MaskInfo chInfo = f.value();
			m_fillCount[i] = m_pVolumeData->getVoxelCount(chInfo.uid);
			m_etcbox[i] = m_pVolumeData->getBoundingBox(chInfo.uid);
			m_TAState[i] = m_pVolumeData->getTAState(chInfo.uid);
			if (WIN_MANAGER->GetTab())
			{
				AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();

				if (tab)
				{
					m_taseq[i] = tab->existResult(chInfo.uid, true);
					tab->DeleteResult(chInfo.uid, true);
				}
			}
			i++;
		}
		m_mv_id.clear();

		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			r = m_pVolumeData->delMaskInfos(m_uid_vector, m_mv_id, existSurface, m_bDeleteAll);
			m_pVolumeData->setCurrentMaskIndex(0);
			MaskInfo * info = m_pVolumeData->getMaskInfo(0);
			std::vector<muint32> vecSelect;
			vecSelect.push_back(info->uid);
			m_pVolumeData->setMultiSelectUID(vecSelect);

			for (int i = 0; i < m_maskPCount[1]; i++)
				file.read((char*)m_pVolumeData->getMaskDataPoint(i), m_pVolumeData->getVolumeDataLength());
			file.close();
		}
		else
		{
			// 211026 허 건과장
			// Mess 리스트 영향으로 인한 주석처리
			//if (m_mesh_vector.size() > 0)
			//{
			//	m_mesh = new ActionDelMeshes(m_mesh_vector);
			//}


			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				for (int i = 0; i < m_maskPCount[0]; i++)
					file.write((char*)m_pVolumeData->getMaskDataPoint(i), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			r = m_pVolumeData->delMaskInfos(m_uid_vector, m_mv_id, existSurface, m_bDeleteAll);
			m_pVolumeData->setCurrentMaskIndex(0);
			MaskInfo * info = m_pVolumeData->getMaskInfo(0);
			std::vector<muint32> vecSelect;
			vecSelect.push_back(info->uid);
			m_pVolumeData->setMultiSelectUID(vecSelect);
			m_maskPCount[1] = m_pVolumeData->GetMaskPointCount();


			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				for (int i = 0; i < m_maskPCount[1]; i++)
					file.write((char*)m_pVolumeData->getMaskDataPoint(i), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

		}

		// 211026 허 건과장
		// Mess 리스트 영향으로 인한 주석처리
		//if (m_mesh)
		//	m_mesh->redo();


		if (r)
		{
			m_pVolumeData->forceUpdateMaskVolume();
			WIN_MANAGER->updatePlaneData_all();
			WIN_MANAGER->updateUI();
			WIN_MANAGER->renderLater_GridView(true);
		}
	}

	WIN_MANAGER->setSaveState(false);
}

void ActionMaskListDels::Do()
{
	//	std::sort(m_list_index.begin(), m_list_index.end(), std::less<muint32>());
	std::sort(m_uid_vector.begin(), m_uid_vector.end(), std::less<unsigned int>());//내림차순 정렬
	QDir dir(STRING_MANAGER->cacheFilePath);
	bool existSurface = true;
	bool r = false;
	if (dir.exists())
	{
		m_etcbox = new BoundingBoxI[m_info_map.size()];
		m_fillCount = new int[m_info_map.size()];
		m_taseq = new int[m_info_map.size()];
		m_TAState = new bool[m_info_map.size()];
		int i = 0;
		for (auto f = m_info_map.begin(); f != m_info_map.end(); f++)
		{
			MaskInfo chInfo = f.value();
			m_fillCount[i] = m_pVolumeData->getVoxelCount(chInfo.uid);
			m_etcbox[i] = m_pVolumeData->getBoundingBox(chInfo.uid);
			m_TAState[i] = m_pVolumeData->getTAState(chInfo.uid);
			if (WIN_MANAGER->GetTab())
			{
				AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();

				if (tab)
				{
					m_taseq[i] = tab->existResult(chInfo.uid, true);
					tab->DeleteResult(chInfo.uid, true);
				}
			}
			i++;
		}
		m_mv_id.clear();

		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);



		// 211026 허 건과장
		// Mess 리스트 영향으로 인한 주석처리
		//if (m_mesh_vector.size() > 0)
		//{
		//	m_mesh = new ActionDelMeshes(m_mesh_vector);
		//}


		r = m_pVolumeData->delMaskInfos(m_uid_vector, m_mv_id, existSurface, m_bDeleteAll);
		m_pVolumeData->setCurrentMaskIndex(0);
		MaskInfo* info = m_pVolumeData->getMaskInfo(0);
		std::vector<muint32> vecSelect;
		vecSelect.push_back(info->uid);
		m_pVolumeData->setMultiSelectUID(vecSelect);
		m_maskPCount[1] = m_pVolumeData->GetMaskPointCount();


		// 211026 허 건과장
		// Mess 리스트 영향으로 인한 주석처리
		//if (m_mesh)
		//	m_mesh->redo();


		if (r)
		{
			m_pVolumeData->forceUpdateMaskVolume();
			WIN_MANAGER->updatePlaneData_all();
			WIN_MANAGER->updateUI();
			WIN_MANAGER->renderLater_GridView(true);
		}
	}

	WIN_MANAGER->setSaveState(false);
}

bool ActionMaskListDels::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionMaskListAddROIFile::ActionMaskListAddROIFile(VOLUME_DATA* pVolumeData, QUndoCommand *parent /*= 0*/)
	:QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_ADD_ROIFILE;
	m_id = s_id++;
	m_list_index = 0;
	m_first = false;
	m_del = false;
	m_maskIndex = 0;
	m_fillCount = 0;
}

bool ActionMaskListAddROIFile::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionMaskListAddROIFile::undo()
{
	if (m_del)
	{
		bool r = m_pVolumeData->delMaskInfo(m_list_index);
		if (r)
		{
			WIN_MANAGER->updateUI();
			WIN_MANAGER->renderLater_GridView(false);
		}
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
			WIN_MANAGER->renderLater_GridView(false);
		}
	}
	m_pVolumeData->forceUpdateMaskVolume();
	m_pVolumeData->setVoxelCount(m_info.uid, 0);
	m_pVolumeData->setBoundingBox(m_info.uid, BoundingBoxI(), true);
	WIN_MANAGER->applyVoxelToUI(m_info.uid);
	WIN_MANAGER->setSaveState(false);
}

void ActionMaskListAddROIFile::redo()
{
	if (m_first == false)
	{
		m_first = true;
		m_del = m_pVolumeData->createMaskInfo();
		m_list_index = m_pVolumeData->getCurrentMaskInfoID();
		m_info = *m_pVolumeData->getCurrentMaskInfo();
		m_maskIndex = m_pVolumeData->getCurrentMaskIndex();
	}
	else if (m_del)
	{
		m_pVolumeData->insertMaskInfo(m_list_index, m_info);
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}
			m_pVolumeData->applyTempMaskBitChange(m_maskIndex == 0 ? m_info.mask_id : m_info.mask_id2, m_maskIndex, 1);
			m_pVolumeData->updateUIDBoundingBox(m_info.uid, true);
			//	m_orgbox = m_pVolumeData->boundingBoxROI[m_info.uid];
			m_orgbox = m_pVolumeData->getBoundingBox(m_info.uid);
			m_fillCount = m_pVolumeData->fillMaskCount;
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}
		}
		//	m_pVolumeData->voxelCount[m_info.uid] = m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_info.uid] = m_orgbox;
		m_pVolumeData->setVoxelCount(m_info.uid, m_fillCount);
		m_pVolumeData->setBoundingBox(m_info.uid, m_orgbox);
		WIN_MANAGER->renderLater_GridView(false);
	}

	WIN_MANAGER->updateUI(true, m_list_index);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->renderLater_GridView(false);
	WIN_MANAGER->setSaveState(false);
}

ActionMaskListAddTXTFile::ActionMaskListAddTXTFile(VOLUME_DATA* pVolumeData, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_ADD_TXTFILE;

	m_id = s_id++;
	m_list_index = 0;
	m_first = false;
	m_del = false;
	m_maskIndex = 0;

	m_fillCount = 0;
}

void ActionMaskListAddTXTFile::undo()
{
	if (m_del)
	{
		bool r = m_pVolumeData->delMaskInfo(m_list_index);
		if (r)
		{
			WIN_MANAGER->updateUI();
			WIN_MANAGER->renderLater_GridView(false);
		}
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
			WIN_MANAGER->renderLater_GridView(false);
		}
	}
	m_pVolumeData->forceUpdateMaskVolume();
	//	m_pVolumeData->voxelCount[m_info.uid] = 0;
	//	m_pVolumeData->boundingBoxROI[m_info.uid].reset(true);
	m_pVolumeData->setVoxelCount(m_info.uid, 0);
	m_pVolumeData->setBoundingBox(m_info.uid, BoundingBoxI(), true);
	WIN_MANAGER->applyVoxelToUI(m_info.uid);
	WIN_MANAGER->setSaveState(false);
}

void ActionMaskListAddTXTFile::redo()
{
	if (m_first == false)
	{
		m_first = true;
		m_del = m_pVolumeData->createMaskInfo();
		m_list_index = m_pVolumeData->getCurrentMaskInfoID();
		m_info = *m_pVolumeData->getCurrentMaskInfo();
		m_maskIndex = m_pVolumeData->getCurrentMaskIndex();
	}
	else if (m_del)
	{
		m_pVolumeData->insertMaskInfo(m_list_index, m_info);
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}
			m_pVolumeData->applyTempMaskBitChange(m_maskIndex == 0 ? m_info.mask_id : m_info.mask_id2, m_maskIndex, 1);
			m_pVolumeData->updateUIDBoundingBox(m_info.uid, true);
			//	m_orgbox = m_pVolumeData->boundingBoxROI[m_info.uid];
			m_orgbox = m_pVolumeData->getBoundingBox(m_info.uid);
			m_fillCount = m_pVolumeData->fillMaskCount;
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}
		}
		//		m_pVolumeData->voxelCount[m_info.uid] = m_fillCount;
		//		m_pVolumeData->boundingBoxROI[m_info.uid] = m_orgbox;
		m_pVolumeData->setVoxelCount(m_info.uid, m_fillCount);
		m_pVolumeData->setBoundingBox(m_info.uid, m_orgbox);
		WIN_MANAGER->renderLater_GridView(false);
	}

	WIN_MANAGER->updateUI(true, m_list_index);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->renderLater_GridView(false);
	WIN_MANAGER->setSaveState(false);
}

bool ActionMaskListAddTXTFile::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionMaskListAddNIIFile::ActionMaskListAddNIIFile(VOLUME_DATA* pVolumeData, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_MASKLIST_ADD_NIIFILE;
	m_id = s_id++;
	m_list_index = 0;
	m_first = false;
	m_del = false;
	m_maskIndex = 0;
	m_fillCount = 0;
}

void ActionMaskListAddNIIFile::undo()
{
	if (m_del)
	{
		bool r = m_pVolumeData->delMaskInfo(m_list_index);
		if (r)
		{
			WIN_MANAGER->updateUI();
		}
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
		}
	}
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->updatePlaneData_all();
	//	m_pVolumeData->voxelCount[m_info.uid] = 0;
	//	m_pVolumeData->boundingBoxROI[m_info.uid].reset(true);
	m_pVolumeData->setVoxelCount(m_info.uid, 0);
	m_pVolumeData->setBoundingBox(m_info.uid, BoundingBoxI(), true);
	WIN_MANAGER->applyVoxelToUI(m_info.uid);
	WIN_MANAGER->setSaveState(false);
	WIN_MANAGER->renderLater_GridView(false);
}

void ActionMaskListAddNIIFile::redo()
{
	if (m_first == false)
	{
		m_first = true;
		m_del = m_pVolumeData->createMaskInfo();
		m_list_index = m_pVolumeData->getCurrentMaskInfoID();
		m_info = *m_pVolumeData->getCurrentMaskInfo();
		m_maskIndex = m_pVolumeData->getCurrentMaskIndex();
	}
	else if (m_del)
	{
		m_pVolumeData->insertMaskInfo(m_list_index, m_info);
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask(m_maskIndex);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			m_pVolumeData->applyTempMaskBitChange(m_maskIndex == 0 ? m_info.mask_id : m_info.mask_id2, m_maskIndex, 1);
			m_pVolumeData->updateUIDBoundingBox(m_info.uid, true);
			//	m_orgbox = m_pVolumeData->boundingBoxROI[m_info.uid];
			m_orgbox = m_pVolumeData->getBoundingBox(m_info.uid);
			m_fillCount = m_pVolumeData->fillMaskCount;
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskIndex), m_pVolumeData->getVolumeDataLength());
				file.close();
			}
		}
		//	m_pVolumeData->voxelCount[m_info.uid] = m_fillCount;
		//	m_pVolumeData->boundingBoxROI[m_info.uid] = m_orgbox;
		m_pVolumeData->setVoxelCount(m_info.uid, m_fillCount);
		m_pVolumeData->setBoundingBox(m_info.uid, m_orgbox);
	}

	WIN_MANAGER->updateUI(true, m_list_index);
	m_pVolumeData->forceUpdateMaskVolume();
	WIN_MANAGER->updatePlaneData_all();
	WIN_MANAGER->renderLater_GridView(false);
	WIN_MANAGER->setSaveState(false);
}

bool ActionMaskListAddNIIFile::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionSeedColor::ActionSeedColor(DrawcutTab * tab, int index, COLOR color, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	static int s_id = ACT_ID_SEED_COLOR;

	m_id = s_id++;
	m_tab = tab;
	m_index = index;
	m_preColor = WIN_MANAGER->getSeedColor(m_index);
	m_color = color;
}

void ActionSeedColor::undo()
{
	if (m_tab)
	{
		WIN_MANAGER->setSeedColor(m_preColor, m_index);
		m_tab->changeSeedColor(m_index, m_preColor);
	}

	if (WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION
		|| WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_SKETCHDRAWSEGMENTATION)
		WIN_MANAGER->renderLater_GridView(false);
}

void ActionSeedColor::redo()
{
	if (m_tab)
	{
		WIN_MANAGER->setSeedColor(m_color, m_index);
		m_tab->changeSeedColor(m_index, m_color);
	}

	if (WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION
		|| WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_SKETCHDRAWSEGMENTATION)
		WIN_MANAGER->renderLater_GridView(false);
}

bool ActionSeedColor::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionSeedClearOne::ActionSeedClearOne(VOLUME_DATA* pVolumeData, int index, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	m_pVolumeData = pVolumeData;

	static int s_id = ACT_ID_SEED_CLEAR;

	m_id = s_id++;

	m_index = index;

	m_voxel = m_pVolumeData->getVoxelCount(m_index, true);

}

void ActionSeedClearOne::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask();
			m_pVolumeData->setVoxelCount(m_index, m_voxel, true, true);
			WIN_MANAGER->applyVoxelToUI(0, true);
			if (WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION
				|| WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_SKETCHDRAWSEGMENTATION)
				WIN_MANAGER->renderLater_GridView(false);
			WIN_MANAGER->setSaveState(false);
		}
	}
}

void ActionSeedClearOne::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_pVolumeData->createTempMaskData();
			file.read((char*)m_pVolumeData->pData3D_Mask_Temp, m_pVolumeData->getVolumeDataLength());
			file.close();

			m_pVolumeData->applyTempMask();
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(), m_pVolumeData->getVolumeDataLength());
				file.close();
			}

			m_pVolumeData->clearMaskData(m_index == 0 ? VM_MASK0 : VM_MASK1);
			m_pVolumeData->setVoxelCount(m_index, 0, true, true);
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_pVolumeData->getMaskDataPoint(), m_pVolumeData->getVolumeDataLength());
				file.close();
			}
		}

		WIN_MANAGER->applyVoxelToUI(0, true);

		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();

		if (WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION
			|| WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_SKETCHDRAWSEGMENTATION)
		{
			WIN_MANAGER->renderLater_GridView(false);
		}

		WIN_MANAGER->setSaveState(false);
	}
}

bool ActionSeedClearOne::mergeWith(const QUndoCommand * command)
{
	return false;
}

ActionAddDrawSeed::ActionAddDrawSeed(int uid, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	static int s_id = ACT_ID_SEED_ADD;

	m_uid.push_back(uid);

	m_id = s_id++;
}

ActionAddDrawSeed::ActionAddDrawSeed(std::vector<muint32> uid, QUndoCommand * parent)
{
	static int s_id = ACT_ID_SEED_ADD;

	m_uid.swap(uid);

	m_id = s_id++;
}

void ActionAddDrawSeed::undo()
{
	DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();

	if (tab)
		tab->DelSeed(m_uid);
}

void ActionAddDrawSeed::redo()
{
	DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();

	if (tab)
		tab->AddSeed(m_uid);
}

ActionDelDrawSeed::ActionDelDrawSeed(int uid, QUndoCommand * parent)
{
	static int s_id = ACT_ID_SEED_DEL;

	m_uid.push_back(uid);

	m_id = s_id++;
}

ActionDelDrawSeed::ActionDelDrawSeed(std::vector<muint32> uid, QUndoCommand * parent)
{
	static int s_id = ACT_ID_SEED_DEL;

	m_uid.swap(uid);

	m_id = s_id++;
}

void ActionDelDrawSeed::undo()
{
	DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();

	if (tab)
		tab->AddSeed(m_uid);
}

void ActionDelDrawSeed::redo()
{
	DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();

	if (tab)
		tab->DelSeed(m_uid);
}


/*
	ActionMaskListDelExtension
*/
ActionMaskListDelExtension::ActionMaskListDelExtension(VOLUME_DATA* pVolumeData, muint32 index, int maskIndex, QUndoCommand* parent)
	: QUndoCommand(parent),
	m_pVolumeData(pVolumeData)
{
	static int s_id = ACT_ID_MASKLIST_DEL;
	m_id = s_id++;
	m_maskByteIndex = maskIndex;
	m_deletedLayerIndex = index;
	m_taseq = -1;
	m_mesh = NULL;
	m_isAIDataExist = false;
	m_aiAIOutset = -1;
#ifdef MULTI_DRAWCUT_MODE
	{
		MaskInfo* info = m_pVolumeData->getMaskInfo(m_list_index);

		if (info)
		{
			DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();

			if (tab)
				m_seedchk = tab->contains(info->uid);
		}
	}
#else
	m_isCheckDrawcutSeed = false;
#endif
}

ActionMaskListDelExtension::~ActionMaskListDelExtension()
{
	if (m_mesh)
	{
		SAFE_DELETE(m_mesh);
	}
}
void ActionMaskListDelExtension::undo()
{
	//bool result = restore();
	bool result = restore_old();

	if (WIN_MANAGER->GetTab())
	{
		AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();
		if (tab)
		{
			if (m_taseq != -1)
			{
				tab->SetResultFromFile(m_deletedMaskInfo.uid, m_taseq);
			}
		}
	}

	m_pVolumeData->setCurrentMaskIndex(m_deletedLayerIndex);
	MaskInfo* info = m_pVolumeData->getMaskInfo(m_deletedLayerIndex);
	std::vector<muint32> vecSelect;
	vecSelect.push_back(info->uid);
	m_pVolumeData->setMultiSelectUID(vecSelect);

	/*
	*	AI 데이터 RollBack
	*/
	if (m_isAIDataExist)
	{
		std::pair<int, std::vector<unsigned char>> pairAIResultData = std::make_pair(m_deletedMaskInfo.uid, m_aiResultDatas);
		m_pVolumeData->m_vecAIResultData.push_back(pairAIResultData);

		std::pair<int, int> pairAIOutset = std::make_pair(m_deletedMaskInfo.uid, m_aiAIOutset);
		m_pVolumeData->m_vecAIOutset.push_back(pairAIOutset);
	}

	restoreUndoDataFromFile();

	if (m_isCheckDrawcutSeed)
	{
		if (WIN_MANAGER->GetTab())
		{
			DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();
			if (tab)
			{
				tab->AddSeed(m_deletedMaskInfo.uid);
			}
		}
	}

	if (result)
	{
		m_pVolumeData->setTAState(m_deletedMaskInfo.uid, m_TAState);
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updateUI();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionMaskListDelExtension::redo()
{
	m_deletedMaskInfo = *m_pVolumeData->getMaskInfo(m_deletedLayerIndex);
	m_originalBoundingBox[0] = m_pVolumeData->getBoundingBox(m_deletedMaskInfo.uid);
	m_fillCount = m_pVolumeData->getVoxelCount(m_deletedMaskInfo.uid);

	saveAIData();

	deleteRelatedAnalysisInfo();

	MaskInfo* pLastInfo = m_pVolumeData->getAtLastMaskInfo();
	if (pLastInfo->uid >= MASK_SECOND_MAX)
	{
		m_mvmaskByteIndex = (pLastInfo->uid - MASK_SECOND_MAX) / 8 + 1;
		m_mvmaskBitFlag = pLastInfo->mask_id2;
	}
	else
	{
		m_mvmaskByteIndex = 0;
		m_mvmaskBitFlag = pLastInfo->mask_id;
	}

	if (!(m_mvmaskByteIndex > m_maskByteIndex))
	{
		m_mvmaskByteIndex = -1;
	}
	else
	{
		m_mvmaskUID = pLastInfo->uid;
		m_originalBoundingBox[1] = m_pVolumeData->getBoundingBox(pLastInfo->uid);
	}

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)m_pVolumeData->getMaskDataPoint(m_maskByteIndex), m_pVolumeData->getVolumeDataLength());
			file.close();
		}
		else
		{
			saveUndoRedoDataToFile();

			m_pVolumeData->clearMaskData(m_maskByteIndex == 0 ? m_deletedMaskInfo.mask_id : m_deletedMaskInfo.mask_id2, m_maskByteIndex);
			m_TAState = m_pVolumeData->getTAState(m_deletedMaskInfo.uid);

			WIN_MANAGER->setDelMaskView(m_deletedLayerIndex);
		}
	}

	deleteRelatedDrawCutSeed();

	m_pVolumeData->setTAState(m_deletedMaskInfo.uid, false);

	bool result = m_pVolumeData->delMaskInfo(m_deletedLayerIndex, &m_deleteMaskResultInfo);

	if (result)
	{
		m_pVolumeData->forceUpdateMaskVolume();
		WIN_MANAGER->updateUI();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(true);
		WIN_MANAGER->setSaveState(false);
	}
}

bool ActionMaskListDelExtension::mergeWith(const QUndoCommand* command)
{
	return false;
}

bool ActionMaskListDelExtension::restore()
{
	MaskInfo* pDeleteMaskInfo = m_deleteMaskResultInfo.GetDeletedMaskInfo();
	int deletedIndex = m_deleteMaskResultInfo.GetDeletedMaskIndex();
	if(m_deleteMaskResultInfo.IsMoved())
	{
		//m_pVolumeData->moveMaskInfoAndData();
		/*
			1. 원본 Moved의 Index에 복구를 진행한다.
			2. 
		*/

		MaskInfo* pMovedMaskInfo = m_deleteMaskResultInfo.GetMovedMaskInfo();
		int movedIndex = m_deleteMaskResultInfo.GetMovedMaskIndex();
		Q_ASSERT(pMovedMaskInfo != nullptr);

		//m_pVolumeData->insertNewMaskInfo(movedIndex);
		m_pVolumeData->insertNewMaskInfo(deletedIndex);
		m_pVolumeData->moveMaskInfoAndData(pDeleteMaskInfo, pMovedMaskInfo);
	}
	else
	{
		m_pVolumeData->setVoxelCount(pDeleteMaskInfo->uid, m_fillCount);
		m_pVolumeData->setBoundingBox(m_deletedMaskInfo.uid, m_originalBoundingBox[0]);
		m_pVolumeData->insertMaskInfo(m_deletedLayerIndex, m_deletedMaskInfo);
	}

	return true;
}

bool ActionMaskListDelExtension::restore_old()
{
	if (-1 != m_mvmaskByteIndex)
	{
		//기존 Layer의 데이터, UID, VoxelCount, BoundingBox, AI Mask정보를 바꾼다.
		MaskInfo* pChangedMaskInfo = m_pVolumeData->getMaskInfo(m_deletedMaskInfo.uid, true);

		Q_ASSERT(pChangedMaskInfo != nullptr);

		mask oldMaskBitFlag = m_maskByteIndex == 0 ? pChangedMaskInfo->mask_id : pChangedMaskInfo->mask_id2;
		m_pVolumeData->moveMaskBitData(
			m_maskByteIndex,
			m_mvmaskByteIndex,
			oldMaskBitFlag,
			m_mvmaskBitFlag);
		pChangedMaskInfo->uid = m_mvmaskUID;
		pChangedMaskInfo->mask_id = VM_MASK7;
		pChangedMaskInfo->mask_id2 = m_mvmaskBitFlag;
		//m_pVolumeData->setVoxelCount(pChangedMaskInfo->uid, m_pVolumeData->getVoxelCount(m_deletedMaskInfo.uid));
		m_pVolumeData->setVoxelCount(pChangedMaskInfo->uid, m_fillCount);
		m_pVolumeData->setBoundingBox(pChangedMaskInfo->uid, m_originalBoundingBox[1]);

		WIN_MANAGER->ChangeUID_Ext(m_pVolumeData, m_deletedMaskInfo.uid, pChangedMaskInfo->uid);
		//result = m_pVolumeData->insertMaskInfo(m_layerIndex, *pChangedMaskInfo);

		//std::swap(m_deletedMaskInfo.uid, pChangedMaskInfo->uid);
	}
	else
	{
		m_pVolumeData->setVoxelCount(m_deletedMaskInfo.uid, m_fillCount);
		m_pVolumeData->setBoundingBox(m_deletedMaskInfo.uid, m_originalBoundingBox[0]);
		//result = m_pVolumeData->insertMaskInfo(m_layerIndex, m_deletedMaskInfo);
	}

	return m_pVolumeData->insertMaskInfo(m_deletedLayerIndex, m_deletedMaskInfo);
}

void ActionMaskListDelExtension::saveAIData()
{
	if (m_pVolumeData->IsAIMaskByIndex(m_deletedLayerIndex))
	{
		int uid = m_deletedMaskInfo.uid;
		m_isAIDataExist = true;
		for (auto itPair : m_pVolumeData->m_vecAIResultData)
		{
			if (itPair.first == uid)
			{
				m_aiResultDatas = itPair.second;
			}
		}

		for (auto itPair : m_pVolumeData->m_vecAIOutset)
		{
			if (itPair.first == uid)
			{
				m_aiAIOutset = itPair.second;
			}
		}
	}
}

void ActionMaskListDelExtension::deleteRelatedAnalysisInfo()
{
	if (WIN_MANAGER->GetTab())
	{
		AnalysisTab* tab = WIN_MANAGER->GetTab()->getAnalysisTab();

		if (tab)
		{
			m_taseq = tab->existResult(m_deletedMaskInfo.uid, true);
			tab->DeleteResult(m_deletedMaskInfo.uid, true);
		}
	}
}

void ActionMaskListDelExtension::deleteRelatedDrawCutSeed()
{
	if (m_isCheckDrawcutSeed)
	{
		if (WIN_MANAGER->GetTab())
		{
			DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();

			if (tab)
			{
				tab->DelSeed(m_deletedMaskInfo.uid);
			}
		}
	}
}

void ActionMaskListDelExtension::saveUndoRedoDataToFile()
{
	QFile file;
	file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
	if (file.open(QIODevice::WriteOnly))
	{
		file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskByteIndex), m_pVolumeData->getVolumeDataLength());
		file.close();
	}

	file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	if (file.open(QIODevice::WriteOnly))
	{
		file.write((char*)m_pVolumeData->getMaskDataPoint(m_maskByteIndex), m_pVolumeData->getVolumeDataLength());
		file.close();
	}
}

void ActionMaskListDelExtension::restoreUndoDataFromFile()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)m_pVolumeData->getMaskDataPoint(m_maskByteIndex), m_pVolumeData->getVolumeDataLength());
			file.close();
		}
	}
}

void ActionMaskListDelExtension::restoreRedoDataFromFile()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)m_pVolumeData->getMaskDataPoint(m_maskByteIndex), m_pVolumeData->getVolumeDataLength());
			file.close();
		}
	}
}

