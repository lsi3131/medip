#include "stdafx.h"
#include "ActionImageMove.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
#include "ActionRegionGrowing.h"

ActionImageMove::ActionImageMove(VOLUME_DATA * volumData, mask _m, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_MOVE;
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

ActionImageMove::~ActionImageMove()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if ( dir.exists() )
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

bool ActionImageMove::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionImageMove::undo()
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
			m_volumData->setBoundingBox(m_uid, m_orgbox[0]);
			m_volumData->forceUpdateMaskVolume();
		//	WIN_MANAGER->volume_data.voxelCount[m_uid] = m_fillCount[0];
			m_volumData->setVoxelCount(m_uid, m_fillCount[0]);
			WIN_MANAGER->updatePlaneData_all();
			WIN_MANAGER->renderLater_GridView(false);
			WIN_MANAGER->setSaveState(false);
			m_volumData->setTAState(m_uid, m_TAState);
			WIN_MANAGER->applyVoxelToUI(m_uid);
		}
	}
}

void ActionImageMove::redo()
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

			m_volumData->applyTempMaskBitChange(m_mask, m_maskIndex);
			m_fillCount[1] = m_volumData->fillMaskCount;
			m_volumData->updateUIDBoundingBox(m_uid, true);
	//		m_orgbox[1] = m_volumData->boundingBoxROI[m_uid];
			m_orgbox[1] = m_volumData->getBoundingBox(m_uid);
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getMaskDataPoint(m_maskIndex), m_volumData->getVolumeDataLength());
				file.close();
			}
		}
		//m_volumData->boundingBoxROI[m_uid] = m_orgbox[1];
		//WIN_MANAGER->volume_data.voxelCount[m_uid] += m_fillCount[1];
		m_volumData->setBoundingBox(m_uid, m_orgbox[1]);
		m_volumData->setVoxelCount(m_uid, m_fillCount[1], false);
		m_volumData->forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->setSaveState(false);
		m_volumData->setTAState(m_uid, false);
		WIN_MANAGER->applyVoxelToUI(m_uid);
	}
}

void WorkImageMove::updateProgress(int val, void* dt)
{
	WorkImageMove* worker = (WorkImageMove*)dt;
	worker->setProgressValue(val);
}

void WorkImageMove::setProgressValue(int value, bool init)
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

void WorkImageMove::threadRun() 
{
	mask *outputData = m_volumData->pData3D_Mask_Temp;
	mask *srcMaskData = m_volumData->getMaskDataPoint(m_maskIndex);

	muint32 cx = m_volumData->getCX();
	muint32 cy = m_volumData->getCY();
	muint32 cz = m_volumData->getCZ();
	muint32 length = m_volumData->getVolumeDataLength();
	

	setProgressValue(0, true);

	if(m_eDirection == DIRECTION_LEFT)
	{
		for (int z = 0; z < cz; z++)
		{
			for (int y = 0; y < cy; y++)
			{
				for (int x = 0; x < cx - 1; x++)
				{
					int offsetSrc = z*cx*cy + y*cx + x;
					int offsetDest = z*cx*cy + y*cx + x + 1;

					outputData[offsetDest] = srcMaskData[offsetSrc];
				}
			}
		}
	}
	else if (m_eDirection == DIRECTION_RIGHT)
	{
		for (int z = 0; z < cz; z++)
		{
			for (int y = 0; y < cy; y++)
			{
				for (int x = 1; x < cx; x++)
				{
					int offsetSrc = z*cx*cy + y*cx + x;
					int offsetDest = z*cx*cy + y*cx + x - 1;

					outputData[offsetDest] = srcMaskData[offsetSrc];
				}
			}
		}		
	}
	else if (m_eDirection == DIRECTION_POSTERIOR)
	{
		for (int z = 0; z < cz; z++)
		{
			for (int y = 0; y < cy-1; y++)
			{
				for (int x = 0; x < cx - 1; x++)
				{
					int offsetSrc = z*cx*cy + y*cx + x;
					int offsetDest = z*cx*cy + (y+1)*cx + x ;

					outputData[offsetDest] = srcMaskData[offsetSrc];
				}
			}
		}
	}
	else if (m_eDirection == DIRECTION_ANTERIOR)
	{
		for (int z = 0; z < cz; z++)
		{
			for (int y = 1; y < cy; y++)
			{
				for (int x = 0; x < cx; x++)
				{
					int offsetSrc = z*cx*cy + y*cx + x;
					int offsetDest = z*cx*cy + (y - 1)*cx + x;

					outputData[offsetDest] = srcMaskData[offsetSrc];
				}
			}
		}
	}
	else if (m_eDirection == DIRECTION_SUPERIOR)
	{
		for (int z = 0; z < cz-1; z++)
		{
			for (int y = 0; y < cy; y++)
			{
				for (int x = 0; x < cx ; x++)
				{
					int offsetSrc = z*cx*cy + y*cx + x;
					int offsetDest = (z+1)*cx*cy + y*cx + x;

					outputData[offsetDest] = srcMaskData[offsetSrc];
				}
			}
		}
	}
	else if (m_eDirection == DIRECTION_INFERIOR)
	{
		for (int z = 1; z < cz ; z++)
		{
			for (int y = 0; y < cy; y++)
			{
				for (int x = 0; x < cx; x++)
				{
					int offsetSrc = z*cx*cy + y*cx + x;
					int offsetDest = (z - 1)*cx*cy + y*cx + x;

					outputData[offsetDest] = srcMaskData[offsetSrc];
				}
			}
		}
	}

// 	actDilation = dilation->addAction("Left direction"); // x+1
// 	actDilation = dilation->addAction("Right direction"); // x-1
// 	actDilation = dilation->addAction("Posterior direction"); // y+1
// 	actDilation = dilation->addAction("Anterior direction"); // y-1
// 	actDilation = dilation->addAction("Superior direction"); // z+1
// 	actDilation = dilation->addAction("Inferior direction"); // z-1

	

	
	emit finished();
}
