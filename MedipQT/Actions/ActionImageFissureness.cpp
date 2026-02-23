#include "stdafx.h"
#include "ActionImageFissureness.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
#include "algorithm/MagicCut.h"

ActionImageFissureness::ActionImageFissureness(VOLUME_DATA * volumData, mask _m, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_FISSURENESS;
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

ActionImageFissureness::~ActionImageFissureness()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if ( dir.exists() )
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

bool ActionImageFissureness::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionImageFissureness::undo()
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
	//		m_volumData->boundingBoxROI[m_uid] = m_orgbox[0];
	//		m_volumData->voxelCount[m_uid] = m_fillCount[0];
			m_volumData->setBoundingBox(m_uid, m_orgbox[0]);
			m_volumData->setVoxelCount(m_uid, m_fillCount[0]);
			m_volumData->forceUpdateMaskVolume();
			WIN_MANAGER->updatePlaneData_all();
			WIN_MANAGER->renderLater_GridView();
			m_volumData->setTAState(m_uid, m_TAState);
			WIN_MANAGER->applyVoxelToUI(m_uid);
			WIN_MANAGER->setSaveState(false);
		}
	}
}

void ActionImageFissureness::redo()
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

			m_volumData->applyTempMask();
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
	//	m_volumData->voxelCount[m_uid] += m_fillCount[1];
		m_volumData->setBoundingBox(m_uid, m_orgbox[1]);
		m_volumData->setVoxelCount(m_uid, m_fillCount[1], false);
		m_volumData->forceUpdateMaskVolume();
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView();
		m_volumData->setTAState(m_uid, false);
		WIN_MANAGER->applyVoxelToUI(m_uid);
		WIN_MANAGER->setSaveState(false);
	}
}

void WorkImageFissureness::setProgressValue(int value, bool init)
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

void WorkImageFissureness::threadRun()
{
	muint32 dataCX = _volumData->getCX();
	muint32 dataCY = _volumData->getCY();
	muint32 dataCZ = _volumData->getCZ();
	
	int width = dataCX;
	int height = dataCY;
	int slice = dataCZ;
	
	short *input = new short[width*height*slice];
	unsigned char *result = new unsigned char[width*height*slice];
	
	for (int z = 0; z < slice; z++)
	{
		for (int y = 0;y < height;y++)
		{
			for (int x = 0;x < width;x++)
			{
				int idx = (width*height*z) + (x + width*y);
				input[idx] =  _volumData->getData(x,y,z);
			}
		}
	}

	setProgressValue(0, true);
	mask *outputData = _volumData->pData3D_Mask_Temp;

	mip::Hessian::HessianMatrix(input, _volumData->pData3D_Mask_Temp,  result, width, height, slice, HESSIAN_FISSURE);
	for (int z = 0; z < slice; z++)
	{
		if (_volumData->threadStop) break;

		setProgressValue(z / float(dataCZ) * 100);
		for (int y = 0;y < height;y++)
		{
			for (int x = 0;x < width;x++)
			{
				
				if (result[(width*height*z) + (x + width*y)] != 0)
				{
					_volumData->setTempMaskBit(x, y, z, _mask);
					
				}
				else
					_volumData->setTempMaskBit(x, y, z, 0);
			}
		}
	}
	
	delete []input;
	delete []result;
	emit finished();
}