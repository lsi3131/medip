#include "stdafx.h"
#include "ActionMaskListDelete.h"
#include "ActionMaskList.h"
#include "windowManager.h"
#include "Tabwindow.h"

ActionMaskListDelExtension2::ActionMaskListDelExtension2(WindowManager* pWindowManager, VOLUME_DATA* pVolumeData, int deleteLayerIndex, QUndoCommand* parent) :
	QUndoCommand(parent)
{
	init(pWindowManager, pVolumeData, { deleteLayerIndex });
}

ActionMaskListDelExtension2::ActionMaskListDelExtension2(WindowManager* pWindowManager, VOLUME_DATA* pVolumeData, std::vector<int> deleteLayerIndexList, QUndoCommand* parent)
	: QUndoCommand(parent)
{
	init(pWindowManager, pVolumeData, deleteLayerIndexList);
}

void ActionMaskListDelExtension2::init(WindowManager* pWindowManager, VOLUME_DATA* pVolumeData, std::vector<int> deleteLayerIndexList)
{
	static int s_id = ACT_ID_MASKLIST_DEL;
	m_id = s_id++;
#ifdef MULTI_DRAWCUT_MODE
	{
		MaskInfo* info = m_pWindowManager->volume_data.getMaskInfo(m_list_index);

		if (info)
		{
			DrawcutTab* tab = m_pWindowManager->GetTab()->getDrawTab();

			if (tab)
				m_seedchk = tab->contains(info->uid);
		}
	}
	m_isCheckDrawcutSeed = false;
#else
	m_isCheckDrawcutSeed = false;
#endif

	m_pWindowManager = pWindowManager;
	m_pVolumeData = pVolumeData;
	m_deleteLayerIndexList = deleteLayerIndexList;
	m_TAExistResult = -1;
}

ActionMaskListDelExtension2::~ActionMaskListDelExtension2()
{
}
void ActionMaskListDelExtension2::undo()
{
	/* restore는 역순으로 한다. */
	int lastIndex = m_deleteMaskResultInfoList.size() - 1;
	for (int i = lastIndex; i >= 0; --i)
	{
		restoreMaskInfo(i);
		restoreMovedMaskBitDataToOrigin(i);

		restoreAI(i);

		restoreRelatedAnalysisInfo(i);
	}

	for (int maskByteIndex : m_maskByteIndexList)
	{
		//File -> Delete - restore
		restoreUndoDataFromFile(maskByteIndex);
	}

	restoreSelectUID();

	bool result = true;
	if (result)
	{
#ifndef USE_UNIT_TEST
		//m_pVolumeData->setTAState(m_deletedMaskInfo.uid, m_TAState);
		m_pVolumeData->forceUpdateMaskVolume();
		m_pWindowManager->updateUI();
		m_pWindowManager->updatePlaneData_all();
		m_pWindowManager->renderLater_GridView(true);
		m_pWindowManager->setSaveState(false);
#endif
	}
}

void ActionMaskListDelExtension2::redo()
{
	bool result = false;
	for (int i = 0; i < m_deleteLayerIndexList.size(); ++i)
	{
		saveUndoRedoDataToFile(i);

		deleteRelatedAnalysisInfo(i);
		deleteRelatedDrawCutSeed(i);
	}

	result = m_pVolumeData->delMaskInfoList(m_deleteLayerIndexList, &m_deleteMaskResultInfoList);

	if (result)
	{
#ifndef USE_UNIT_TEST
		m_pVolumeData->forceUpdateMaskVolume();
		m_pWindowManager->updateUI();
		m_pWindowManager->updatePlaneData_all();
		m_pWindowManager->renderLater_GridView(true);
		m_pWindowManager->setSaveState(false);
#endif
	}
}

bool ActionMaskListDelExtension2::mergeWith(const QUndoCommand* command)
{
	return false;
}

int ActionMaskListDelExtension2::id() const
{
	return m_id;
}

QString ActionMaskListDelExtension2::GetUndoFilePathByMaskByteIndex(int maskByteIndex)
{
	return STRING_MANAGER->cacheFilePath + QString("/undo%1_%2").arg(m_id).arg(maskByteIndex);
}

QString ActionMaskListDelExtension2::GetRedoFilePathByMaskByteIndex(int maskByteIndex)
{
	return STRING_MANAGER->cacheFilePath + QString("/redo%1_%2").arg(m_id).arg(maskByteIndex);
}

std::set<int> ActionMaskListDelExtension2::GetMaskByteIndexList()
{
	return m_maskByteIndexList;
}

bool ActionMaskListDelExtension2::IsMaskByteIndexExist(int maskByteIndex)
{
	return m_maskByteIndexList.find(maskByteIndex) != m_maskByteIndexList.end();
}

bool ActionMaskListDelExtension2::restoreMaskInfo(int idx)
{
	BackupMaskInfo* pDeleteBackupInfo = m_deleteMaskResultInfoList[idx].GetDeletedBackupInfo();
	BackupMaskInfo* pMovedBackupInfo = m_deleteMaskResultInfoList[idx].GetMovedBackupInfo();
	if (m_deleteMaskResultInfoList[idx].IsMoved())
	{
		insertBackupInfo(*pDeleteBackupInfo);
		copyBackupInfo(*pMovedBackupInfo);
	}
	else
	{
		insertBackupInfo(*pDeleteBackupInfo);
	}

	return true;
}

void ActionMaskListDelExtension2::restoreMovedMaskBitDataToOrigin(int idx)
{
	//Delete -> Move - restore
	BackupMaskInfo* pDeleteBackupInfo = m_deleteMaskResultInfoList[idx].GetDeletedBackupInfo();
	BackupMaskInfo* pMovedBackupInfo = m_deleteMaskResultInfoList[idx].GetMovedBackupInfo();
	if (m_deleteMaskResultInfoList[idx].IsMoved())
	{
		m_pVolumeData->moveMaskBitData(
			m_pVolumeData->GetMaskByteIndex(pDeleteBackupInfo->Info.get()),
			m_pVolumeData->GetMaskByteIndex(pMovedBackupInfo->Info.get()),
			m_pVolumeData->GetMaskBitFlag(pDeleteBackupInfo->Info.get()),
			m_pVolumeData->GetMaskBitFlag(pMovedBackupInfo->Info.get())
		);
	}

}

bool ActionMaskListDelExtension2::restoreAI(int idx)
{
	BackupMaskInfo* pDeleteBackupInfo = m_deleteMaskResultInfoList[idx].GetDeletedBackupInfo();
	BackupMaskInfo* pMovedBackupInfo = m_deleteMaskResultInfoList[idx].GetMovedBackupInfo();

	if (m_deleteMaskResultInfoList[idx].IsMoved())
	{
		if (pMovedBackupInfo->IsAIDataExist())
		{
			m_pVolumeData->SetAIDataByUID(pMovedBackupInfo->Info->uid, pMovedBackupInfo->AIResult, pMovedBackupInfo->AIOutset);
		}

		if (pDeleteBackupInfo->IsAIDataExist())
		{
			m_pVolumeData->SetAIDataByUID(pDeleteBackupInfo->Info->uid, pDeleteBackupInfo->AIResult, pDeleteBackupInfo->AIOutset);
		}
	}
	else
	{
		if (pDeleteBackupInfo->IsAIDataExist())
		{
			m_pVolumeData->SetAIDataByUID(pDeleteBackupInfo->Info->uid, pDeleteBackupInfo->AIResult, pDeleteBackupInfo->AIOutset);
		}
	}

	return true;
}

void ActionMaskListDelExtension2::restoreSelectUID()
{
	m_pVolumeData->setCurrentMaskIndex(m_deleteLayerIndexList[0]);

	std::vector<muint32> selectUIDList;
	for (int layerIndex : m_deleteLayerIndexList)
	{
		MaskInfo* pMaskInfo = m_pVolumeData->getMaskInfo(layerIndex);
		selectUIDList.push_back(pMaskInfo->uid);
	}
	m_pVolumeData->setMultiSelectUID(selectUIDList);
}

void ActionMaskListDelExtension2::deleteRelatedAnalysisInfo(int idx)
{
	if (m_pWindowManager->GetTab())
	{
		AnalysisTab* tab = m_pWindowManager->GetTab()->getAnalysisTab();

		if (tab)
		{
			m_TAExistResult = tab->existResult(getDeleteMaskInfo(m_deleteLayerIndexList[idx])->uid, true);
			tab->DeleteResult(getDeleteMaskInfo(m_deleteLayerIndexList[idx])->uid, true);
		}
	}
}

void ActionMaskListDelExtension2::restoreRelatedAnalysisInfo(int idx)
{
	if (m_pWindowManager->GetTab())
	{
		AnalysisTab* tab = m_pWindowManager->GetTab()->getAnalysisTab();
		if (tab)
		{
			if (m_TAExistResult != -1)
			{
				tab->SetResultFromFile(m_deleteMaskResultInfoList[idx].GetDeletedMaskInfo()->uid, m_TAExistResult);
			}
		}
	}
}

void ActionMaskListDelExtension2::deleteRelatedDrawCutSeed(int idx)
{
	if (m_isCheckDrawcutSeed)
	{
		if (m_pWindowManager->GetTab())
		{
			DrawcutTab* tab = m_pWindowManager->GetTab()->getDrawTab();

			if (tab)
			{
				tab->DelSeed(getDeleteMaskInfo(m_deleteLayerIndexList[idx])->uid);
			}
		}
	}
}

void ActionMaskListDelExtension2::saveUndoRedoDataToFile(int idx)
{
	int maskByteIndex = getDeleteMaskInfo_ByteIndex(m_deleteLayerIndexList[idx]);
	auto it = m_maskByteIndexList.find(maskByteIndex);
	if (it == m_maskByteIndexList.end())
	{
		m_maskByteIndexList.insert(maskByteIndex);

		saveUndoDataToFile(maskByteIndex);
		saveRedoDataToFile(maskByteIndex);
	}
}

void ActionMaskListDelExtension2::restoreRedoDataFromFile()
{
	//QDir dir(STRING_MANAGER->cacheFilePath);
	//if (dir.exists())
	//{
	//	QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
	//	QFile file(filename);
	//	if (file.exists() && file.open(QIODevice::ReadOnly))
	//	{
	//		file.read((char*)m_pVolumeData->getMaskDataPoint(m_maskByteIndex), m_pVolumeData->getVolumeDataLength());
	//		file.close();
	//	}
	//}
}

int ActionMaskListDelExtension2::getDeleteMaskInfo_ByteIndex(int layerIndex)
{
	return m_pVolumeData->GetMaskByteIndex(m_pVolumeData->getMaskInfoByIndex(layerIndex));
}

MaskInfo* ActionMaskListDelExtension2::getDeleteMaskInfo(int layerIndex)
{
	return m_pVolumeData->getMaskInfoByIndex(layerIndex);
}

void ActionMaskListDelExtension2::insertBackupInfo(const BackupMaskInfo& backupInfo)
{
	m_pVolumeData->insertMaskInfo(backupInfo.LayerIndex, *backupInfo.Info);
	m_pVolumeData->setVoxelCount(backupInfo.Info->uid, backupInfo.VoxelCount);
	m_pVolumeData->setBoundingBox(backupInfo.Info->uid, backupInfo.BoundingBox);
}

void ActionMaskListDelExtension2::copyBackupInfo(const BackupMaskInfo& backupInfo)
{
	int layerIndex = backupInfo.LayerIndex;

	m_pVolumeData->resetMaskInfo(layerIndex, *backupInfo.Info);
	m_pVolumeData->setVoxelCount(backupInfo.Info->uid, backupInfo.VoxelCount);
	m_pVolumeData->setBoundingBox(backupInfo.Info->uid, backupInfo.BoundingBox);
}

void ActionMaskListDelExtension2::restoreUndoDataFromFile(int maskByteIndex)
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists() == false)
	{
		return;
	}
	QString undoFilePath = GetUndoFilePathByMaskByteIndex(maskByteIndex);
	QFile file(undoFilePath);
	if (file.exists() == false)
	{
		return;
	}

	if (file.open(QIODevice::ReadOnly))
	{
		file.read((char*)m_pVolumeData->getMaskDataPoint(maskByteIndex), m_pVolumeData->getVolumeDataLength());
		file.close();
	}
}

void ActionMaskListDelExtension2::saveUndoDataToFile(int maskByteIndex)
{
	QString undoFilePath = GetUndoFilePathByMaskByteIndex(maskByteIndex);
	QFile file(undoFilePath);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write((char*)m_pVolumeData->getMaskDataPoint(maskByteIndex), m_pVolumeData->getVolumeDataLength());
		file.close();
	}
}

void ActionMaskListDelExtension2::saveRedoDataToFile(int maskByteIndex)
{
	QString redoFilePath = GetRedoFilePathByMaskByteIndex(maskByteIndex);
	QFile file(redoFilePath);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write((char*)m_pVolumeData->getMaskDataPoint(maskByteIndex), m_pVolumeData->getVolumeDataLength());
		file.close();
	}
}

