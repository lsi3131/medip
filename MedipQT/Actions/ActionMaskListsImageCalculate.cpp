#include "stdafx.h"
#include "graphics/volumedata.h"
#include "Windows/windowManager.h"
#include "System/StringManager.h"
#include "System/VolumeCalculator.h"
#include "ActionMaskListsImageCalculate.h"
#include "DataContext.h"

//=======================================
//		ActionMaskListsImageCalculate
//=======================================
ActionMaskListsImageCalculate::ActionMaskListsImageCalculate(mint16* pOriginVolumeDataPoint, QUndoCommand *parent) :
	m_pOriginHUVolumeDataPoint(pOriginVolumeDataPoint)
{
	static int s_id = ACT_ID_MASKLIST_IMAGE_CALCULATE;
	m_id = s_id++;

	m_redoFilePath = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
	m_undoFilePath = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
}

ActionMaskListsImageCalculate::~ActionMaskListsImageCalculate()
{
}

void ActionMaskListsImageCalculate::undo()
{
	VOLUME_DATA* pVolumeData = &DATA_CONTEXT->volume_data;

	/* Volume HU Data -> undo file */
	if (LoadVolumeData(pVolumeData, m_undoFilePath) == false)
	{
		return;
	}

	WIN_MANAGER->forceUpdate2DViewData(false, true);
	WIN_MANAGER->updatePlaneData_all();
	WIN_MANAGER->updateUI();
	WIN_MANAGER->renderLater_GridView();
}

void ActionMaskListsImageCalculate::redo()
{
	VOLUME_DATA* pVolumeData = &DATA_CONTEXT->volume_data;

	/* cache directory 존재 여부 확인 */
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists() == false)
	{
		return;
	}

	if (IsRedoFileExist())
	{
		/* 수정본 redo file -> Volume HU Data */
		if (LoadVolumeData(pVolumeData, m_redoFilePath) == false)
		{
			return;
		}
	}
	else
	{
		/* 원본 Volume HU Data -> undo file */
		if (SaveVolumeData(
			m_pOriginHUVolumeDataPoint,
			pVolumeData->getCX(),
			pVolumeData->getCY(),
			pVolumeData->getCZ(),
			m_undoFilePath) == false)
		{
			return;
		}

		/* 수정본 Volume HU Data -> redo file */
		if (SaveVolumeData(pVolumeData, m_redoFilePath) == false)
		{
			return;
		}
	}

	WIN_MANAGER->forceUpdate2DViewData(false, true);
	WIN_MANAGER->updatePlaneData_all();
	WIN_MANAGER->updateUI();
	WIN_MANAGER->renderLater_GridView();
}

bool ActionMaskListsImageCalculate::IsRedoFileExist()
{
	QFile file(m_redoFilePath);
	return file.exists();
}

bool ActionMaskListsImageCalculate::LoadVolumeData(VOLUME_DATA* pVolumeData, QString filepath)
{
	QFile file(filepath);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		return false;
	}

	muint32 length = pVolumeData->getVolumeDataLength();
	mint16 *newHUData = new mint16[length];
	memset(newHUData, 0, sizeof(mint16) * length);

	file.read((char*)newHUData, sizeof(mint16)*length);

	pVolumeData->setHUData(newHUData,
		pVolumeData->getCX(),
		pVolumeData->getCY(),
		pVolumeData->getCZ()
	);

	SAFE_DELETES(newHUData);

	VOLUME_CALCULATOR->Initialize(pVolumeData);

	return true;
}

bool ActionMaskListsImageCalculate::SaveVolumeData(VOLUME_DATA* pVolumeData, QString filepath)
{
	return SaveVolumeData(pVolumeData->getHUDataPoint(),
		pVolumeData->getCX(),
		pVolumeData->getCY(),
		pVolumeData->getCZ(),
		filepath
	);
}

bool ActionMaskListsImageCalculate::SaveVolumeData(mint16 * pVolumeData, int cx, int cy, int cz, QString filepath)
{
	QFile file(filepath);
	if (file.open(QIODevice::WriteOnly) == false)
	{
		return false;
	}

	int HUDataSizeofByte = cx * cy * cz * sizeof(mint16);
	file.write((char*)pVolumeData, HUDataSizeofByte);
	file.close();
	return true;
}

bool ActionMaskListsImageCalculate::mergeWith(const QUndoCommand * command)
{
	return true;
}
