#include "stdafx.h"
#include "Actions/ActionLevelset.h"
#include "System/stringManager.h"
#include "Windows/windowManager.h"
// #include "System/llist.h"
// #include "System/sfm_local_chanvese_mex.cpp"

void WorkLevelset::setProgressValue(int value, bool init)
{
	static int val = 0;

	if (init)
	{
		val = 0;
		emit progress(val);
		return;
	}

	if (100 <= val)
		return;

	if ((val + 10) <= value)
	{
		val = value;
		emit progress(val);
	}
}

void WorkLevelset::threadRun()
{
	setProgressValue(0, true);
	_volumeData->createTempMaskData(true, -1, _maskIndex);
	mint32 length = _volumeData->getVolumeDataLength();
	mask *oldValcopy = new mask[length];
	mask *oldVal = _volumeData->getMaskDataPoint(_maskIndex);
	for (int i = 0; i < length; i++)
	{
		if (oldVal[i] & _mask)
			oldValcopy[i] = 1;
		else
			oldValcopy[i] = 0;
	}
	setProgressValue(10);
//	LevelSet(_volumeData->getHUDataPoint(), oldValcopy, _volumeData->getMaskTempDataPoint(_maskIndex), _volumeData->getCX(), _volumeData->getCY(), _volumeData->getCZ(),0.1, 10, 10);
	
//	LevelSet(_volumeData->getHUDataPoint(), oldValcopy, _volumeData->getMaskTempDataPoint(_maskIndex), _volumeData->getCX(), _volumeData->getCY(), _volumeData->getCZ(), _lambda, 
//		_iter, _radious);
	setProgressValue(90);
	SAFE_DELETES(oldValcopy);

	emit finished();
}

ActionLevelset::ActionLevelset(VOLUME_DATA * volumData, mask _m, int _mI, QUndoCommand * parent)
{
	static int s_id = ACT_ID_IMAGE_LEVEL_SET;
	memset(m_fillCount, 0, sizeof(m_fillCount));
	m_mask = _m;
	m_maskIndex = _mI;
	m_volumData = volumData;
	m_uid = m_volumData->getCurrentMaskInfo()->uid;
	m_TAState = m_volumData->getTAState(m_uid);
//	m_fillCount[0] = m_volumData->voxelCount[m_uid];
//	m_orgbox[0] = m_volumData->boundingBoxROI[m_uid];
	m_fillCount[0] = m_volumData->getVoxelCount(m_uid);
	m_orgbox[0] = m_volumData->getBoundingBox(m_uid);
	m_id = s_id++;
}

ActionLevelset::~ActionLevelset()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

void ActionLevelset::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_volumData->createTempMaskData();
			file.read((char*)m_volumData->pData3D_Mask_Temp, m_volumData->getVolumeDataLength());
			file.close();

			m_volumData->applyTempMask(m_maskIndex);
		//	m_volumData->boundingBoxROI[m_uid] = m_orgbox[0];
		//	WIN_MANAGER->volume_data.voxelCount[m_uid] = m_fillCount[0];
			m_volumData->setBoundingBox(m_uid, m_orgbox[0]);
			m_volumData->setVoxelCount(m_uid, m_fillCount[0]);
			m_volumData->forceUpdateMaskVolume();
			WIN_MANAGER->updatePlaneData_all();
			WIN_MANAGER->renderLater_GridView(false);
			m_volumData->setTAState(m_uid, m_TAState);
			WIN_MANAGER->applyVoxelToUI(m_uid);
			WIN_MANAGER->setSaveState(false);
		}
	}
}

void ActionLevelset::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			m_volumData->createTempMaskData();
			file.read((char*)m_volumData->pData3D_Mask_Temp, m_volumData->getVolumeDataLength());
			file.close();

			m_volumData->applyTempMask(m_maskIndex);
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getMaskDataPoint(m_maskIndex), m_volumData->getVolumeDataLength());
				file.close();
			}

			m_volumData->applyTempMaskBitChange(m_mask, m_maskIndex,1, m_maskIndex);
			m_fillCount[1] = m_volumData->fillMaskCount;
			m_volumData->updateUIDBoundingBox(m_uid, true);
		//	m_orgbox[1] = m_volumData->boundingBoxROI[m_uid];
			m_orgbox[1] = m_volumData->getBoundingBox(m_uid);
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getMaskDataPoint(m_maskIndex), m_volumData->getVolumeDataLength());
				file.close();
			}
		}
	//	m_volumData->boundingBoxROI[m_uid] = m_orgbox[1];
	//	WIN_MANAGER->volume_data.voxelCount[m_uid] += m_fillCount[1];
		m_volumData->setBoundingBox(m_uid, m_orgbox[1]);
		m_volumData->setVoxelCount(m_uid, m_fillCount[1], false);
		m_volumData->forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(false);
		m_volumData->setTAState(m_uid, false);
		WIN_MANAGER->applyVoxelToUI(m_uid);
		WIN_MANAGER->setSaveState(false);
	}
}

bool ActionLevelset::mergeWith(const QUndoCommand * command)
{
	return false;
}
