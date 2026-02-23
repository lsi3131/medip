#include "stdafx.h"

#include "ActionImageScale.h"
#include "ActionRegionGrowing.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "System/stringManager.h"
#include "System/VolumeCalculator.h"
#include <QFile>


WorkImageScale::~WorkImageScale()
{
}

void WorkImageScale::setProgressValue(int value, bool init /*= false*/)
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

void WorkImageScale::updateProgress(int val, void *dt)
{
	WorkImageScale* worker = (WorkImageScale*)dt;
	worker->setProgressValue(val);
}


void WorkImageScale::threadRun()
{
	setProgressValue(0, true);
	std::string cachePath = STRING_MANAGER->cacheFilePath.toLocal8Bit().toStdString();
	int nIndex = _volumeData->GetMaskPointCount();

	mip::Downscaling dsc(cachePath, _volumeData->getCX(), _volumeData->getCY(), _volumeData->getCZ(),
		_volumeData->getSpaceX(), _volumeData->getSpaceY(), _volumeData->getSpaceZ(),
		_volumeData->getHUDataPoint(), nIndex, &(_volumeData->threadStop));

	for (int i = 0; i <= nIndex; i++)
		dsc.setMaskPoint(i, _volumeData->getMaskDataPoint(i));

	dsc.setProgress(updateProgress, this);

	_volumeData->threadResult = dsc.startScaling();

	emit finished();
}

ActionImageScale::ActionImageScale(VOLUME_DATA* pVolumeData, QUndoCommand * parent)
	:QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_SCALE;
	m_volumData = pVolumeData;
	m_id = s_id++;


	maskCount = m_maskIndex = 0;
	voxelUID = voxelCounts[0] = voxelCounts[1] = NULL;
	boxes[0] = boxes[1] = NULL;

	if (NULL != m_volumData->getAtLastMaskInfo())
	{
		m_maskIndex = m_volumData->getAtLastMaskInfo()->uid >= MASK_SECOND_MAX ?
			(m_volumData->getAtLastMaskInfo()->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

		maskCount = m_volumData->getMaskInfoListCnt();







		for (int i = 0; i



			< 2; i++)
		{
			voxelCounts[i] = new int[maskCount];
			boxes[i] = new BoundingBoxI[maskCount];

			memset(voxelCounts[i], 0, sizeof(int)*(maskCount));
		}

		voxelUID = new int[maskCount];
		m_TAState = new bool[maskCount];
		memset(voxelUID, 0, sizeof(int)*(maskCount));

		for (int i = 0; i < maskCount; i++)
		{
			MaskInfo *info = m_volumData->getMaskInfo(i);

			voxelUID[i] = info->uid;

			m_volumData->setVoxelCount(voxelUID[i], voxelCounts[0][i]);
			m_volumData->setBoundingBox(voxelUID[i], boxes[0][i]);
			m_TAState[i] = m_volumData->getTAState(voxelUID[i]);
		}

	}
}

ActionImageScale::~ActionImageScale()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}

	SAFE_DELETES(voxelUID);
	SAFE_DELETES(m_TAState);

	for (int i = 0; i < 2; i++)
	{
		SAFE_DELETES(voxelCounts[i]);
		SAFE_DELETES(boxes[i]);
	}
}

void ActionImageScale::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		WIN_MANAGER->setRenderable(false);
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			mint16 *newHuData = new mint16[m_scInfo.orgX*m_scInfo.orgY*m_scInfo.orgZ];
			memset(newHuData, 0, sizeof(mint16)*(m_scInfo.orgX*m_scInfo.orgY*m_scInfo.orgZ));

			file.read((char *)newHuData, sizeof(mint16) * (m_scInfo.orgX*m_scInfo.orgY*m_scInfo.orgZ));

			m_volumData->createData(newHuData, m_scInfo.orgX, m_scInfo.orgY, m_scInfo.orgZ,
				m_scInfo.orgSpX, m_scInfo.orgSpY, m_scInfo.orgSpZ, false);

			SAFE_DELETES(newHuData);

			mask * newData;
			newData = new mask[m_scInfo.orgX*m_scInfo.orgY*m_scInfo.orgZ];
			for (int i = 0; i <= m_maskIndex; i++)
			{
				memset(newData, 0, (m_scInfo.orgX*m_scInfo.orgY*m_scInfo.orgZ));
				file.read((char*)newData, m_scInfo.orgX*m_scInfo.orgY*m_scInfo.orgZ);
				m_volumData->createMaskData(newData, i);
			}
			SAFE_DELETES(newData);

			file.close();
		}

		for (int i = 0; i < maskCount; i++)
		{
			m_volumData->setVoxelCount(voxelUID[i], voxelCounts[0][i]);
			m_volumData->setBoundingBox(voxelUID[i], boxes[0][i]);
			m_volumData->setTAState(voxelUID[i], m_TAState[i]);
		}

		/* Volume 원본 데이터 Update */
		VOLUME_CALCULATOR->Initialize(m_volumData);

		WIN_MANAGER->resetResource();

		m_volumData->forceUpdateVolume();
		m_volumData->forceUpdateMaskVolume();
		WIN_MANAGER->setRenderable(true);
		WIN_MANAGER->mainSegmentWidget->resetUI(true);
		WIN_MANAGER->updateUI();
		WIN_MANAGER->renderLater_All();
		WIN_MANAGER->mainSegmentWidget->createHUHisto();

		WIN_MANAGER->patchyPoint = QVector3D(-1, -1, -1);
		WIN_MANAGER->setPatchyPoint();


		WIN_MANAGER->setSaveState(false);
	}
}

void ActionImageScale::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);

	if (dir.exists())
	{
		WIN_MANAGER->setRenderable(false);

		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);

		QFile file(filename);

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			mint16 *newHuData = new mint16[m_scInfo.newX*m_scInfo.newY*m_scInfo.newZ];
			memset(newHuData, 0, sizeof(mint16)*(m_scInfo.newX*m_scInfo.newY*m_scInfo.newZ));

			file.read((char*)newHuData, sizeof(mint16) *(m_scInfo.newX*m_scInfo.newY*m_scInfo.newZ));

			m_volumData->createData(newHuData, m_scInfo.newX, m_scInfo.newY, m_scInfo.newZ,
				m_scInfo.newSpX, m_scInfo.newSpY, m_scInfo.newSpZ, false);

			SAFE_DELETES(newHuData);

			mask * newData;

			newData = new mask[m_scInfo.newX*m_scInfo.newY*m_scInfo.newZ];

			for (int i = 0; i <= m_maskIndex; i++)
			{
				memset(newData, 0, (m_scInfo.newX*m_scInfo.newY*m_scInfo.newZ));
				file.read((char*)newData, m_scInfo.newX*m_scInfo.newY*m_scInfo.newZ);
				m_volumData->createMaskData(newData, i);
			}
			SAFE_DELETES(newData);

			file.close();

		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
				for (int i = 0; i <= m_maskIndex; i++)
				{
					file.write((char*)m_volumData->getMaskDataPoint(i), m_volumData->getVolumeDataLength());
				}

				file.close();
			}

			filename = STRING_MANAGER->cacheFilePath + "/scale.tmp";
			file.setFileName(filename);

			if (file.exists() && file.open(QIODevice::ReadOnly))
			{
				file.read((char*)&m_scInfo, sizeof(ScaleHeader));

				mint16 *newHuData = new mint16[m_scInfo.newX*m_scInfo.newY*m_scInfo.newZ];
				memset(newHuData, 0, sizeof(mint16)*(m_scInfo.newX*m_scInfo.newY*m_scInfo.newZ));

				file.read((char*)newHuData, sizeof(mint16) *(m_scInfo.newX*m_scInfo.newY*m_scInfo.newZ));

				m_volumData->createData(newHuData, m_scInfo.newX, m_scInfo.newY, m_scInfo.newZ,
					m_scInfo.newSpX, m_scInfo.newSpY, m_scInfo.newSpZ, false);

				SAFE_DELETES(newHuData);

				mask * newData;
				newData = new mask[m_scInfo.newX*m_scInfo.newY*m_scInfo.newZ];

				for (int i = 0; i <= m_maskIndex; i++)
				{
					memset(newData, 0, (m_scInfo.newX*m_scInfo.newY*m_scInfo.newZ));
					file.read((char*)newData, m_scInfo.newX*m_scInfo.newY*m_scInfo.newZ);
					m_volumData->createMaskData(newData, i);

				}
				SAFE_DELETES(newData);

				file.close();

				file.remove();
			}

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));

			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));

				for (int i = 0; i <= m_maskIndex; i++)
					file.write((char*)m_volumData->getMaskDataPoint(i), m_volumData->getVolumeDataLength());

				file.close();
			}

			//		m_volumData->getAllMaskBoundingBox();
			m_volumData->getAllMaskVoxel();

			for (int i = 0; i < maskCount; i++)
			{
				voxelCounts[1][i] = m_volumData->getVoxelCount(voxelUID[i]);
				boxes[1][i] = m_volumData->getBoundingBox(voxelUID[i]);
			}

		}

		for (int i = 0; i < maskCount; i++)
		{
			m_volumData->setVoxelCount(voxelUID[i], voxelCounts[1][i]);
			m_volumData->setBoundingBox(voxelUID[i], boxes[1][i]);
			m_volumData->setTAState(voxelUID[i], false);
		}

		/* Volume 원본 데이터 Update */
		VOLUME_CALCULATOR->Initialize(m_volumData);

		WIN_MANAGER->resetTextures();

		m_volumData->forceUpdateVolume();
		m_volumData->forceUpdateMaskVolume();
		WIN_MANAGER->setRenderable(true);
		WIN_MANAGER->mainSegmentWidget->resetUI(true);
		WIN_MANAGER->updateUI();
		WIN_MANAGER->renderLater_All();
		WIN_MANAGER->mainSegmentWidget->createHUHisto();

		WIN_MANAGER->patchyPoint = QVector3D(-1, -1, -1);
		WIN_MANAGER->setPatchyPoint();
		WIN_MANAGER->setSaveState(false);
	}
}

bool ActionImageScale::mergeWith(const QUndoCommand * command)
{
	return false;
}

WorkImageCrop::WorkImageCrop(VOLUME_DATA* pVolumeData)
{
	m_pVolumeData = pVolumeData;
}

void WorkImageCrop::setProgressValue(int value, bool init)
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

void WorkImageCrop::threadRun()
{
	setProgressValue(0, true);

	VOLUME_DATA* vol_dt = m_pVolumeData;
	BoundingBoxI box = vol_dt->getBoundingBox();
	muint32 cx, cy, cz;
	muint32 length;
	muint32 newx, newy, newz;

	vol_dt->threadResult = 1;
	vol_dt->getLengthForScreen(WT_AXIAL, cx, cy, cz);

	length = cx*cy*cz;

	int index = 0;

	QString tmpCache = STRING_MANAGER->cacheFilePath + QString("/CropVolume");

	QFile file(tmpCache);

	if (!file.open(QIODevice::WriteOnly))
	{
		vol_dt->threadResult = -1;
		emit finished();
		return;
	}

	newx = box.maxX - box.minX + 1;
	newy = box.maxY - box.minY + 1;
	newz = box.maxZ - box.minZ + 1;

	short * dt = new short[newx*newy*newz];
	memset(dt, 0, sizeof(short)*(newx*newy*newz));

	int cnt = 0;
	int proVal = newz / 4;
	for (int z = box.minZ; z <= box.maxZ; z++)
	{
		for (int y = box.minY; y <= box.maxY; y++)
		{
			for (int x = box.minX; x <= box.maxX; x++)
			{
				index = z*cx*cy + y*cx + x;

				if (index < 0 || index >= length)
					continue;

				dt[cnt++] = vol_dt->getData(index);
			}
		}

		if ((z - box.minZ) % proVal == 0)
		{
			if (vol_dt->threadStop)
				break;

			setProgressValue(((z - box.minZ) / proVal) * 10);
		}
	}

	if(!vol_dt->threadStop)
	{
		file.write((char*)&newx, sizeof(muint32));
		file.write((char*)&newy, sizeof(muint32));
		file.write((char*)&newz, sizeof(muint32));

		file.write((char*)dt, sizeof(short)*cnt);
	}

	file.close();

	SAFE_DELETES(dt);

	if (vol_dt->threadStop)
	{
		emit finished();
		return;
	}
	
	setProgressValue(50);

	int mCount = vol_dt->GetMaskPointCount();

	uchar *mdt = new uchar[newx*newy*newz];

	for (int i = 0; i < mCount; i++)
	{
		tmpCache = STRING_MANAGER->cacheFilePath + QString("/CropMask%1").arg(i);

		file.setFileName(tmpCache);

		if (!file.open(QIODevice::WriteOnly))
		{
			vol_dt->threadResult = -1;
			break;
		}

		memset(mdt, 0, newx*newy*newz);

		cnt = 0;

		for (int z = box.minZ; z <= box.maxZ; z++)
		{
			for (int y = box.minY; y <= box.maxY; y++)
			{
				for (int x = box.minX; x <= box.maxX; x++)
				{
					index = z*cx*cy + y*cx + x;
					
					if (index < 0 || index >= length)
						continue;

					mdt[cnt++] = vol_dt->getMaskData(index, i);
				}
			}

			if ((z - box.minZ) % proVal == 0)
			{
				if (vol_dt->threadStop)
					break;
			}
		}

		if(!vol_dt->threadStop)
			file.write((char*)mdt, newx*newy*newz);
		file.close();
		
		if (vol_dt->threadStop)
			break;

		setProgressValue(50 + ((i + 1) / mCount) * 50);
	}

	SAFE_DELETES(mdt);

	emit finished();
}

ActionImageCrop::ActionImageCrop(VOLUME_DATA* pVolumeData, QUndoCommand *parent /*= 0*/)
	: QUndoCommand(parent),
	m_volumData(pVolumeData)
{
	static int s_id = ACT_ID_IMAGE_CROP;
	
	maskCount = m_maskIndex = 0;
	voxelUID = voxelCounts[0] = voxelCounts[1] = NULL;
	boxes[0] = boxes[1] = NULL;

	if (NULL != m_volumData->getAtLastMaskInfo())
	{
		m_maskIndex = m_volumData->getAtLastMaskInfo()->uid >= MASK_SECOND_MAX ?
			(m_volumData->getAtLastMaskInfo()->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
		maskCount = m_volumData->getMaskInfoListCnt();

		for (int i = 0; i < 2; i++)
		{
			voxelCounts[i] = new int[maskCount];
			boxes[i] = new BoundingBoxI[maskCount];

			memset(voxelCounts[i], 0, sizeof(int)*(maskCount));
		}

		voxelUID = new int[maskCount];
		m_TAState = new bool[maskCount];

		memset(voxelUID, 0, sizeof(int)*(maskCount));

		for (int i = 0; i < maskCount; i++)
		{
			MaskInfo *info = m_volumData->getMaskInfo(i);

			voxelUID[i] = info->uid;

			voxelCounts[0][i] = m_volumData->getVoxelCount(voxelUID[i]);
			boxes[0][i] = m_volumData->getBoundingBox(voxelUID[i]);
			m_TAState[i] = m_volumData->getTAState(voxelUID[i]);
		}

	}


	m_id = s_id++;
}

ActionImageCrop::~ActionImageCrop()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}

	SAFE_DELETES(voxelUID);
	SAFE_DELETES(m_TAState);

	for (int i = 0; i < 2; i++)
	{
		SAFE_DELETES(voxelCounts[i]);
		SAFE_DELETES(boxes[i]);
	}
}

void ActionImageCrop::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		WIN_MANAGER->setRenderable(false);
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			muint32 cx, cy, cz;
			
			file.read((char*)&cx, sizeof(muint32));
			file.read((char*)&cy, sizeof(muint32));
			file.read((char*)&cz, sizeof(muint32));

			int length = cx*cy*cz;

			short *newHUData = new short[length];
			memset(newHUData, 0, sizeof(short) * length);
			file.read((char*)newHUData, sizeof(mint16)*length);

			m_volumData->createData(newHUData, cx,cy,cz,
				m_volumData->getSpaceX(), m_volumData->getSpaceY(), m_volumData->getSpaceZ(), false);
			
			SAFE_DELETES(newHUData);

			m_volumData->createTempMaskData();

			mask *mdt = m_volumData->getMaskTempDataPoint();
			
			for (int i = 0; i <= m_maskIndex; i++)
			{
				memset(mdt, 0, length);

				file.read((char*)mdt, length);
				file.close();
				
				m_volumData->createMaskData(mdt, i);
			}

		}

		for (int i = 0; i < maskCount; i++)
		{
			m_volumData->setVoxelCount(voxelUID[i], voxelCounts[0][i]);
			m_volumData->setBoundingBox(voxelUID[i], boxes[0][i]);
			m_volumData->setTAState(voxelUID[i], m_TAState[i]);
		}

		/* Volume 원본 데이터 Update */
		VOLUME_CALCULATOR->Initialize(m_volumData);

		m_volumData->clearTempData();
		WIN_MANAGER->resetTextures();

		m_volumData->forceUpdateVolume();
		m_volumData->forceUpdateMaskVolume();
		WIN_MANAGER->setRenderable(true);
		WIN_MANAGER->mainSegmentWidget->resetUI(true);
		WIN_MANAGER->updateUI();
		WIN_MANAGER->renderLater_All();
		WIN_MANAGER->mainSegmentWidget->createHUHisto();

		WIN_MANAGER->patchyPoint = QVector3D(-1, -1, -1);
		WIN_MANAGER->setPatchyPoint();
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionImageCrop::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);

	WIN_MANAGER->setRenderable(false);

	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		
		muint32 cx, cy, cz;
		int length;
		cx = cy = cz = length = 0;

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)&cx, sizeof(muint32));
			file.read((char*)&cy, sizeof(muint32));
			file.read((char*)&cz, sizeof(muint32));

			length = cx*cy*cz;

			short *newHUData = new short[length];
			memset(newHUData, 0, sizeof(short) * length);

			file.read((char*)newHUData, sizeof(mint16)*length);
			
			m_volumData->createData(newHUData, cx, cy, cz,
				m_volumData->getSpaceX(), m_volumData->getSpaceY(), m_volumData->getSpaceZ(), false);

			SAFE_DELETES(newHUData);

			m_volumData->createTempMaskData();

			mask *mdt = m_volumData->getMaskTempDataPoint();//clear later

			for (int i = 0; i <= m_maskIndex; i++)
			{
				memset(mdt, 0, length);

				file.read((char*)mdt, length);

				m_volumData->createMaskData(mdt, i);
			}
			file.close();

		}
		else
		{
			QString filename2 = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
			QFile file2(filename2);

			m_volumData->getLengthForScreen(WT_AXIAL, cx, cy, cz);

			if (file2.open(QIODevice::WriteOnly))
			{
				file2.write((char*)&cx, sizeof(muint32));
				file2.write((char*)&cy, sizeof(muint32));
				file2.write((char*)&cz, sizeof(muint32));

				file2.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));

				for (int i = 0; i <= m_maskIndex; i++)
					file2.write((char*)m_volumData->getMaskDataPoint(i), m_volumData->getVolumeDataLength());

				file2.close();

			}

			cx = cy = cz = 0;

			file2.setFileName(STRING_MANAGER->cacheFilePath + QString("/CropVolume"));

			if (file2.open(QIODevice::ReadOnly))
			{
				file2.read((char*)&cx, sizeof(muint32));
				file2.read((char*)&cy, sizeof(muint32));
				file2.read((char*)&cz, sizeof(muint32));

				length = cx*cy*cz;
				short *newHUData = new short[length];
				memset(newHUData, 0, sizeof(short) * length);

				file2.read((char*)newHUData, sizeof(short) * length);
				file2.close();
				file2.remove();

				m_volumData->createData(newHUData, cx, cy, cz,
					m_volumData->getSpaceX(), m_volumData->getSpaceY(), m_volumData->getSpaceZ(), false);

				SAFE_DELETES(newHUData);

				m_volumData->createTempMaskData();

				mask *mdt = m_volumData->getMaskTempDataPoint();//clear later

				for (int i = 0; i <= m_maskIndex; i++)
				{
					memset(mdt, 0, length);

					file2.setFileName(STRING_MANAGER->cacheFilePath + QString("/CropMask%1").arg(i));

					if (file2.open(QIODevice::ReadOnly))
					{
						file2.read((char*)mdt, length);
						file2.close();
						file2.remove();
						m_volumData->createMaskData(mdt, i);
					}
				}
			}

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)&cx, sizeof(muint32));
				file.write((char*)&cy, sizeof(muint32));
				file.write((char*)&cz, sizeof(muint32));

				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));

				for (int i = 0; i <= m_maskIndex; i++)
					file.write((char*)m_volumData->getMaskDataPoint(i), m_volumData->getVolumeDataLength());

				file.close();
			}

			m_volumData->getAllMaskVoxel();

			for (int i = 0; i < maskCount; i++)
			{
				voxelCounts[1][i] = m_volumData->getVoxelCount(voxelUID[i]);
				boxes[1][i] = m_volumData->getBoundingBox(voxelUID[i]);
			}
		}
	}

	for (int i = 0; i < maskCount; i++)
	{
		m_volumData->setVoxelCount(voxelUID[i], voxelCounts[1][i]);
		m_volumData->setBoundingBox(voxelUID[i], boxes[1][i]);
		m_volumData->setTAState(voxelUID[i], false);
	}

	/* Volume 원본 데이터 Update */
	VOLUME_CALCULATOR->Initialize(m_volumData);

	m_volumData->clearTempData();
	WIN_MANAGER->resetTextures();

	m_volumData->forceUpdateVolume();
	m_volumData->forceUpdateMaskVolume();
	WIN_MANAGER->setRenderable(true);
	WIN_MANAGER->mainSegmentWidget->resetUI(true);
	WIN_MANAGER->updateUI();
	WIN_MANAGER->renderLater_All();
	WIN_MANAGER->mainSegmentWidget->createHUHisto();

	WIN_MANAGER->patchyPoint = QVector3D(-1, -1, -1);
	WIN_MANAGER->setPatchyPoint();
	WIN_MANAGER->setSaveState(false);

}
