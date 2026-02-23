#include "stdafx.h"
#include "ActionImageErosion.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
#include "ActionRegionGrowing.h"

ActionImageErosion::ActionImageErosion(VOLUME_DATA * volumData, mask _m, int _maskIndex, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_EROSION;
	memset(m_fillCount, 0, sizeof(m_fillCount));
	m_mask = _m;
	m_volumData = volumData;
	m_maskIndex = _maskIndex;
	m_id = s_id++;
	m_uid = m_volumData->getCurrentMaskInfo()->uid;
	m_TAState = m_volumData->getTAState(m_uid);
	m_fillCount[0] = m_volumData->getVoxelCount(m_uid);
	m_orgbox[0] = m_volumData->getBoundingBox(m_uid);
}

ActionImageErosion::~ActionImageErosion()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

bool ActionImageErosion::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionImageErosion::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if ( file.exists() && file.open(QIODevice::ReadOnly) )
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
			WIN_MANAGER->applyVoxelToUI(m_uid);
			m_volumData->setTAState(m_uid, m_TAState);
			WIN_MANAGER->setSaveState(false);
		}
	}
}

void ActionImageErosion::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly) )
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

			m_volumData->applyTempMaskBitChange(m_mask, m_maskIndex);
			m_volumData->updateUIDBoundingBox(m_uid, true);
		//	m_orgbox[1] = m_volumData->boundingBoxROI[m_uid];
			m_orgbox[1] = m_volumData->getBoundingBox(m_uid);
			m_fillCount[1] = m_volumData->fillMaskCount;
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

void WorkImageErosion::setProgressValue(int value, bool init)
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

void WorkImageErosion::threadRun() // 2017.08.18 ÀÌµÎÈñ ÆÀÀå Ãß°¡
{
	mask *outputData = _volumData->pData3D_Mask_Temp;
	muint32 dataCX = _volumData->getCX();
	muint32 dataCY = _volumData->getCY();
	muint32 dataCZ = _volumData->getCZ();
	muint32 length = _volumData->getVolumeDataLength();

	setProgressValue(0, true);

	mip::IP::Erosion(dataCX, dataCY, dataCZ, outputData, _volumData->getMaskDataPoint(_maskIndex),
		_mask, _direction, &(_volumData->threadStop), updateProgress, this);

	emit finished();
}

void WorkImageErosion::updateProgress(int val, void * dt)
{
	WorkImageErosion* worker = (WorkImageErosion*)dt;
	worker->setProgressValue(val);
}
