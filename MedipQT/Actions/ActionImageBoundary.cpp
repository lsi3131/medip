#include "stdafx.h"
#include "ActionImageBoundary.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
#include "qfile.h"

ActionImageBoundary::ActionImageBoundary(VOLUME_DATA * volumData, mask _m, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_BOUNDARY;
	memset(m_fillCount, 0, sizeof(m_fillCount));
	m_mask = _m;
	m_maskIndex = _mI;
	m_volumData = volumData;
	m_uid = m_volumData->getCurrentMaskInfo()->uid;
//	m_fillCount[0] = m_volumData->voxelCount[m_uid];
//	m_orgbox[0] = m_volumData->boundingBoxROI[m_uid];
	m_fillCount[0] = m_volumData->getVoxelCount(m_uid);
	m_orgbox[0] = m_volumData->getBoundingBox(m_uid);
	m_id = s_id++;
}

ActionImageBoundary::~ActionImageBoundary()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

bool ActionImageBoundary::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionImageBoundary::undo()
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
			m_volumData->forceUpdateMaskVolume();
		//	m_volumData->voxelCount[m_uid] = m_fillCount[0];
		//	m_volumData->boundingBoxROI[m_uid] = m_orgbox[0];
			m_volumData->setVoxelCount(m_uid, m_fillCount[0]);
			m_volumData->setBoundingBox(m_uid, m_orgbox[0]);
			WIN_MANAGER->renderLater_GridView(false);
			WIN_MANAGER->setSaveState(false);

			WIN_MANAGER->applyVoxelToUI(m_uid);
		}
	}
}

void ActionImageBoundary::redo()
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
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->setSaveState(false);

		WIN_MANAGER->applyVoxelToUI(m_uid);
	}
}

void WorkImageBoundary::setProgressValue(int value, bool init)
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

void WorkImageBoundary::threadRun() 
{
	mask *outputData = _volumData->pData3D_Mask_Temp;
	mask *maskData = _volumData->getMaskDataPoint(_maskIndex);
	muint32 width = _volumData->getCX();
	muint32 height = _volumData->getCY();
	muint32 slice = _volumData->getCZ();

	mask ee;
	mask mcheck[6];
	unsigned char mBoolBound;
	setProgressValue(0, true);
	if (_windowType == WT_AXIAL)
	{
		int z = _depth;
		for (int j = 0; j < height; j++)
		{
			if (_volumData->threadStop) break;
			setProgressValue(j / float(height) * 100);

			for (int i = 0; i < width; i++)
			{
				if (i - 1 >= 0 && i + 1 < width && j - 1 >= 0 && j + 1 < height && z - 1 >= 0 && z + 1 < slice)
				{
					outputData[(width*height*z) + (i + width*j)] = 0;
					ee = maskData[(width*height*z) + (i + width*j)];
					mcheck[0] = maskData[(width*height*z) + ((i - 1) + width*j)];
					mcheck[1] = maskData[(width*height*z) + ((i + 1) + width*j)];
					mcheck[2] = maskData[(width*height*z) + (i + width*(j - 1))];
					mcheck[3] = maskData[(width*height*z) + (i + width*(j + 1))];
					mcheck[4] = maskData[(width*height*(z - 1)) + (i + width*j)];
					mcheck[5] = maskData[(width*height*(z + 1)) + (i + width*j)];
					mBoolBound = FALSE;
					if (ee == 1)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 1;
						}
					}
					if (ee == 2)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 2;
						}
					}
					if (ee == 3)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 3;
						}
					}
					if (ee == 4)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 4;
						}
					}
					if (ee == 5)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 5;
						}
					}
					if (mBoolBound != 0)
					{
						outputData[(width*height*z) + (i + width*j)] = _mask;
					}
				}
			}
		}
	}
	else if (_windowType == WT_CORONAL)
	{
		int j = _depth;
		for (int z = 0; z < slice; z++)
		{
			if (_volumData->threadStop) break;
			setProgressValue(z / float(slice) * 100);

			for (int i = 0; i < width; i++)
			{
				if (i - 1 >= 0 && i + 1 < width && j - 1 >= 0 && j + 1 < height && z - 1 >= 0 && z + 1 < slice)
				{
					outputData[(width*height*z) + (i + width*j)] = 0;
					ee = maskData[(width*height*z) + (i + width*j)];
					mcheck[0] = maskData[(width*height*z) + ((i - 1) + width*j)];
					mcheck[1] = maskData[(width*height*z) + ((i + 1) + width*j)];
					mcheck[2] = maskData[(width*height*z) + (i + width*(j - 1))];
					mcheck[3] = maskData[(width*height*z) + (i + width*(j + 1))];
					mcheck[4] = maskData[(width*height*(z - 1)) + (i + width*j)];
					mcheck[5] = maskData[(width*height*(z + 1)) + (i + width*j)];
					mBoolBound = FALSE;
					if (ee == 1)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 1;
						}
					}
					if (ee == 2)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 2;
						}
					}
					if (ee == 3)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 3;
						}
					}
					if (ee == 4)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 4;
						}
					}
					if (ee == 5)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 5;
						}
					}
					if (mBoolBound != 0)
					{
						outputData[(width*height*z) + (i + width*j)] = _mask;
					}
				}
			}
		}
	}
	else if (_windowType == WT_SAGITTAL)
	{
		int i = _depth;
		for (int z = 0; z < slice; z++)
		{
			if (_volumData->threadStop) break;
			setProgressValue(z / float(slice) * 100);

			for (int j = 0; j < height; j++)
			{
				if (i - 1 >= 0 && i + 1 < width && j - 1 >= 0 && j + 1 < height && z - 1 >= 0 && z + 1 < slice)
				{
					outputData[(width*height*z) + (i + width*j)] = 0;
					ee = maskData[(width*height*z) + (i + width*j)];
					mcheck[0] = maskData[(width*height*z) + ((i - 1) + width*j)];
					mcheck[1] = maskData[(width*height*z) + ((i + 1) + width*j)];
					mcheck[2] = maskData[(width*height*z) + (i + width*(j - 1))];
					mcheck[3] = maskData[(width*height*z) + (i + width*(j + 1))];
					mcheck[4] = maskData[(width*height*(z - 1)) + (i + width*j)];
					mcheck[5] = maskData[(width*height*(z + 1)) + (i + width*j)];
					mBoolBound = FALSE;
					if (ee == 1)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 1;
						}
					}
					if (ee == 2)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 2;
						}
					}
					if (ee == 3)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 3;
						}
					}
					if (ee == 4)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 4;
						}
					}
					if (ee == 5)
					{
						for (int k = 0; k < 6; k++)
						{
							if (mcheck[k] == 0)
								mBoolBound = 5;
						}
					}
					if (mBoolBound != 0)
					{
						outputData[(width*height*z) + (i + width*j)] = _mask;
					}
				}
			}
		}
	}
	
	emit finished();
}