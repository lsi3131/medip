#include "stdafx.h"
#include "ActionImageHoleFilling.h"
#include "ActionRegionGrowing.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
#include "algorithm/MagicCut.h"


ActionImageHoleFilling::ActionImageHoleFilling(VOLUME_DATA * volumData, mask _m, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_HOLEFILLING;
	memset(m_fillCount, 0, sizeof(m_fillCount));
	m_mask = _m;
	m_maskIndex = _mI;
	m_volumData = volumData;
	m_uid = m_volumData->getCurrentMaskInfo()->uid;
//	m_fillCount[0] = m_volumData->voxelCount[m_uid];
//	m_orgbox[0] = m_volumData->boundingBoxROI[m_uid];
	m_fillCount[0] = m_volumData->getVoxelCount(m_uid);
	m_orgbox[0] = m_volumData->getBoundingBox(m_uid);
	m_TAState = m_volumData->getTAState(m_uid);
	m_id = s_id++;
}

ActionImageHoleFilling::~ActionImageHoleFilling()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if ( dir.exists() )
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

bool ActionImageHoleFilling::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionImageHoleFilling::undo()
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
		//	m_volumData->voxelCount[m_uid] = m_fillCount[0];
		//	m_volumData->boundingBoxROI[m_uid] = m_orgbox[0];
			m_volumData->setVoxelCount(m_uid, m_fillCount[0]);
			m_volumData->setBoundingBox(m_uid, m_orgbox[0]);
			m_volumData->forceUpdateMaskVolume();
			WIN_MANAGER->updatePlaneData_all();
			WIN_MANAGER->renderLater_GridView();
			WIN_MANAGER->setSaveState(false);
			m_volumData->setTAState(m_uid, m_TAState);
			WIN_MANAGER->applyVoxelToUI(m_uid);
		}
	}
}

void ActionImageHoleFilling::redo()
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


			m_volumData->applyTempMaskBitAdd(m_mask, m_maskIndex);
			//	m_volumData->applyTempMaskBitChange(m_mask, m_maskIndex);
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
		//	m_volumData->voxelCount[m_uid] += m_fillCount[1];
		m_volumData->setBoundingBox(m_uid, m_orgbox[1]);
		m_volumData->setVoxelCount(m_uid, m_fillCount[1], false);
		m_volumData->forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView();
		WIN_MANAGER->setSaveState(false);
		m_volumData->setTAState(m_uid, false);
		WIN_MANAGER->applyVoxelToUI(m_uid);
	}
}

void ActionImageHoleFilling::Do()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);


		m_volumData->applyTempMaskBitAdd(m_mask, m_maskIndex);
		//	m_volumData->applyTempMaskBitChange(m_mask, m_maskIndex);
		m_volumData->updateUIDBoundingBox(m_uid, true);
		//	m_orgbox[1] = m_volumData->boundingBoxROI[m_uid];
		m_orgbox[1] = m_volumData->getBoundingBox(m_uid);
		m_fillCount[1] = m_volumData->fillMaskCount;

		//	m_volumData->boundingBoxROI[m_uid] = m_orgbox[1];
		//	m_volumData->voxelCount[m_uid] += m_fillCount[1];
		m_volumData->setBoundingBox(m_uid, m_orgbox[1]);
		m_volumData->setVoxelCount(m_uid, m_fillCount[1], false);
		m_volumData->forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView();
		WIN_MANAGER->setSaveState(false);
		m_volumData->setTAState(m_uid, false);
		WIN_MANAGER->applyVoxelToUI(m_uid);
	}
}

void WorkImageHoleFilling::setProgressValue(int value, bool init)
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

void WorkImageHoleFilling::threadRun()
{
	mask *outputData = _volumData->pData3D_Mask_Temp;
	muint32 dataCX = _volumData->getCX();
	muint32 dataCY = _volumData->getCY();
	muint32 dataCZ = _volumData->getCZ();
	muint32 dataLenth = _volumData->getVolumeDataLength();
	int width = dataCX;
	int height = dataCY;
	int slice = dataCZ;

	setProgressValue(0, true);

	BoundingBoxI tempBox = _volumData->getBoundingBox(_uid);

	mip::BoundingBoxSimple workingBoundary;
	workingBoundary.minX = tempBox.minX;
	workingBoundary.minY = tempBox.minY;
	workingBoundary.minZ = tempBox.minZ;
	workingBoundary.maxX = tempBox.maxX;
	workingBoundary.maxY = tempBox.maxY;
	workingBoundary.maxZ = tempBox.maxZ;

	mip::HoleFilling hf(height, width, slice, workingBoundary,
		_volumData->getHuMin(), _volumData->getHuMax(), _mask, _volumData->getHUDataPoint(),
		_volumData->getMaskDataPoint(_maskIndex), outputData, &(_volumData->threadStop), this->_mode);

	hf.setProgress(updateProgress, this);

	hf.startFilling();
	 
	emit finished();
}

void WorkImageHoleFilling::updateProgress(int val, void * dt)
{
	WorkImageHoleFilling* worker = (WorkImageHoleFilling*)dt;
	worker->setProgressValue(val);
}
