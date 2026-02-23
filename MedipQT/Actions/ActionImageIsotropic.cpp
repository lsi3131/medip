#include "stdafx.h"
#include "ActionImageIsotropic.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "System/VolumeCalculator.h"
#include "system/stringManager.h"
#include "algorithm/MagicCut.h"


ActionImageIsotropicConversion::ActionImageIsotropicConversion(VOLUME_DATA* pVolumeData, int newCZ, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_ISOTROPIC;
	m_volumData = pVolumeData;
	m_id = s_id++;
	dataCZ[0] = m_volumData->getCZ();
	dataCZ[1] = newCZ;
	
	float spaceX = m_volumData->getSpaceX(true);
	spaceZ[0] = m_volumData->getSpaceZ(true);
	spaceZ[1] = spaceX;

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
}

ActionImageIsotropicConversion::~ActionImageIsotropicConversion()
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

bool ActionImageIsotropicConversion::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionImageIsotropicConversion::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		WIN_MANAGER->setRenderable(false);
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		muint32 dataCX = m_volumData->getCX();
		muint32 dataCY = m_volumData->getCY();
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			mask * newData[4];

			for (int i = 0; i < 4; i++)
				newData[i] = 0;

			for (int i = 0; i <= m_maskIndex; i++)
			{
				newData[i] = new mask[dataCX*dataCY*dataCZ[0]];
				memset(newData[i], 0, (dataCX*dataCY*dataCZ[0]));
				file.read((char*)newData[i], dataCX*dataCY*dataCZ[0]);
			}

			short *newHUData = new short[dataCX*dataCY*dataCZ[0]];
			memset(newHUData, 0, sizeof(short) * (dataCX*dataCY*dataCZ[0]));
			file.read((char*)newHUData, sizeof(mint16)*(dataCX*dataCY*dataCZ[0]));
			file.close();
			m_volumData->createData(newHUData, dataCX, dataCY, dataCZ[0],
				m_volumData->getSpaceX(), m_volumData->getSpaceY(), spaceZ[0] / 10, false);
			for (int i = 0; i <= m_maskIndex; i++)
			{
				m_volumData->createMaskData(newData[i], i);
				SAFE_DELETES(newData[i]);
			}

			SAFE_DELETES(newHUData);
		}

		for (int i = 0; i < maskCount; i++)
		{
			m_volumData->setVoxelCount(voxelUID[i], voxelCounts[0][i]);
			m_volumData->setBoundingBox(voxelUID[i], boxes[0][i]);
			m_volumData->setTAState(voxelUID[i], m_TAState[i]);
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

void ActionImageIsotropicConversion::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);

	WIN_MANAGER->setRenderable(false);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		muint32 dataCX = m_volumData->getCX();
		muint32 dataCY = m_volumData->getCY();
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			short *newHUData = new short[dataCX*dataCY*dataCZ[1]];
			memset(newHUData, 0, sizeof(short) * (dataCX*dataCY*dataCZ[1]));

			mask * newData[4];

			for (int i = 0; i < 4; i++)
				newData[i] = 0;

			for (int i = 0; i <= m_maskIndex; i++)
			{
				newData[i] = new mask[dataCX*dataCY*dataCZ[1]];
				memset(newData[i], 0, (dataCX*dataCY*dataCZ[1]));
				file.read((char*)newData[i], dataCX*dataCY*dataCZ[1]);
			}

			file.read((char*)newHUData, sizeof(mint16)*(dataCX*dataCY*dataCZ[1]));
			file.close();

			m_volumData->createData(newHUData, dataCX, dataCY, dataCZ[1],
				m_volumData->getSpaceX(), m_volumData->getSpaceY(), spaceZ[1] / 10, false);

			for (int i = 0; i <= m_maskIndex; i++)
			{
				m_volumData->createMaskData(newData[i], i);
				SAFE_DELETES(newData[i]);
			}
			SAFE_DELETES(newHUData);
		}
		else
		{
			QString filename2 = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
			QFile file2(filename2);

			file2.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file2.open(QIODevice::WriteOnly))
			{
				for (int i = 0; i <= m_maskIndex; i++)
				{
					file2.write((char*)m_volumData->getMaskDataPoint(i), m_volumData->getVolumeDataLength());
				}
				file2.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
				file2.close();
			}

			short *newHUData = new short[dataCX*dataCY*dataCZ[1]];
			memset(newHUData, 0, sizeof(short) * (dataCX*dataCY*dataCZ[1]));
			memcpy(newHUData, m_volumData->pData3D_HU_Temp, sizeof(short) * (dataCX*dataCY*dataCZ[1]));

			mask * newData[4];

			for (int i = 0; i < 4; i++)
				newData[i] = 0;

			for (int i = 0; i <= m_maskIndex; i++)
			{
				newData[i] = new mask[dataCX*dataCY*dataCZ[1]];
				memset(newData[i], 0, (dataCX*dataCY*dataCZ[1]));

				memcpy_s(newData[i], (dataCX*dataCY*dataCZ[1]),
					m_volumData->getMaskTempDataPoint(i), (dataCX*dataCY*dataCZ[1]));

			//	memcpy(newData[i], m_volumData->getMaskTempDataPoint(i), (dataCX*dataCY*dataCZ[1]));
			}

			m_volumData->createData(newHUData, dataCX, dataCY, dataCZ[1],
				m_volumData->getSpaceX(), m_volumData->getSpaceY(), spaceZ[1] / 10, false);
			for (int i = 0; i <= m_maskIndex; i++)
			{
				m_volumData->createMaskData(newData[i], i);
				SAFE_DELETES(newData[i]);
			}
			SAFE_DELETES(newHUData);

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				for (int i = 0; i <= m_maskIndex; i++)
					file.write((char*)m_volumData->getMaskDataPoint(i), m_volumData->getVolumeDataLength());

				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));

				file.close();
			}

			//voxel count & bounding box
	//		m_volumData->getAllMaskBoundingBox();
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

	BoundingBoxI box = m_volumData->getBoundingBox();
	WIN_MANAGER->setAIDepth(box);

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

WorkImageIsotropicConversion::WorkImageIsotropicConversion(VOLUME_DATA* pVolumeData)
{
	_volumData = pVolumeData;
}

void WorkImageIsotropicConversion::updateProgress(int val, void*data)
{
	WorkImageIsotropicConversion* worker = (WorkImageIsotropicConversion*)data;

	worker->setProgressValue(val);
}

void WorkImageIsotropicConversion::threadRun() // 2017.08.18 이두희 팀장 추가
{
	muint32 dataCX = _volumData->getCX();
	muint32 dataCY = _volumData->getCY();
	muint32 dataCZ = _volumData->getCZ();

	float spaceX = _volumData->getSpaceX(true);
	float spaceZ = _volumData->getSpaceZ(true);

	setProgressValue(0, true);
	mip::Isotropic iso(dataCX, dataCY, dataCZ, spaceX, spaceZ, &(_volumData->threadStop));

	
	iso.setShortIsotropic(_volumData->getHUDataPoint());
	iso.setProgress(updateProgress, this);

	float newCZ = iso.getIsoCount();

	_volumData->createTempHUData(true, dataCX*dataCY*newCZ);
	
	iso.startIsotropic(_volumData->pData3D_HU_Temp);

	if (_volumData->threadStop)
	{
		emit finished();
		return;
	}

	setProgressValue(50);

	int maskIndex = 0;
	if (NULL != _volumData->getAtLastMaskInfo())
		maskIndex = _volumData->getAtLastMaskInfo()->uid >= MASK_SECOND_MAX ?
		(_volumData->getAtLastMaskInfo()->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	const int maskTotal = _volumData->getMaskInfoListCnt();
	for (int i = 0; i <= maskIndex; i++)
	{
		_volumData->createTempMaskData(true, dataCX*dataCY*newCZ, i);
		for (int j = 0; j < maskTotal; j++)
		{
			if (_volumData->isEmptyMaskVoxel(j))
			{
				continue;
			}

			MaskInfo *info = _volumData->getMaskInfo(j);

			int index = _volumData->getMaskInfo(j)->uid >= MASK_SECOND_MAX ? (_volumData->getMaskInfo(j)->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

			if (index != i) continue;

			mask maskID = info->uid >= MASK_SECOND_MAX ? info->mask_id2 : info->mask_id;

			iso.setUCharIsotropic(_volumData->getMaskDataPoint(i), maskID);

			iso.startIsotropic(_volumData->getMaskTempDataPoint(i));

			setProgressValue(50 + (j / (float)maskTotal) * 50);

			if (_volumData->threadStop)
			{
				emit finished();
				return;
			}
			
		}
	}

	setProgressValue(100);
	_volumData->threadResult = newCZ;
	emit strProgress(QString("Isotropic conversion completed!"));
	emit finished();
	
	
	
}

void WorkImageIsotropicConversion::setProgressValue(int value, bool init)
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
void WorkImageIsotropicConversion::Run(void *input, void *output, int  width, int height, int input_slice, int output_slice, bool isMask, mask maskID)
{
	
}

/////////////////////////////////////////////////////////////////////////////////////


ActionImageIsotropicConversionModification::ActionImageIsotropicConversionModification(VOLUME_DATA* pVolumData, int newCZ, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_ISOTROPIC_MODIFICATION;
	m_volumData = pVolumData;
	m_id = s_id++;
	dataCZ[0] = m_volumData->getCZ();
	dataCZ[1] = newCZ;

	float spaceX = m_volumData->getSpaceX(true);
	spaceZ[0] = m_volumData->getSpaceZ(true);
	spaceZ[1] = spaceX;

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
}

ActionImageIsotropicConversionModification::~ActionImageIsotropicConversionModification()
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

bool ActionImageIsotropicConversionModification::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionImageIsotropicConversionModification::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		WIN_MANAGER->setRenderable(false);
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		muint32 dataCX = m_volumData->getCX();
		muint32 dataCY = m_volumData->getCY();
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			mask * newData[4];

			for (int i = 0; i < 4; i++)
				newData[i] = 0;

			for (int i = 0; i <= m_maskIndex; i++)
			{
				newData[i] = new mask[dataCX*dataCY*dataCZ[0]];
				memset(newData[i], 0, (dataCX*dataCY*dataCZ[0]));
				file.read((char*)newData[i], dataCX*dataCY*dataCZ[0]);
			}

			short *newHUData = new short[dataCX*dataCY*dataCZ[0]];
			memset(newHUData, 0, sizeof(short) * (dataCX*dataCY*dataCZ[0]));
			file.read((char*)newHUData, sizeof(mint16)*(dataCX*dataCY*dataCZ[0]));
			file.close();
			m_volumData->createData(newHUData, dataCX, dataCY, dataCZ[0],
				m_volumData->getSpaceX(), m_volumData->getSpaceY(), spaceZ[0] / 10, false);
			for (int i = 0; i <= m_maskIndex; i++)
			{
				m_volumData->createMaskData(newData[i], i);
				SAFE_DELETES(newData[i]);
			}

			SAFE_DELETES(newHUData);
		}

		for (int i = 0; i < maskCount; i++)
		{
			m_volumData->setVoxelCount(voxelUID[i], voxelCounts[0][i]);
			m_volumData->setBoundingBox(voxelUID[i], boxes[0][i]);
			m_volumData->setTAState(voxelUID[i], m_TAState[i]);
		}

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

void ActionImageIsotropicConversionModification::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);

	WIN_MANAGER->setRenderable(false);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		muint32 _dataCX = m_volumData->getCX();
		muint32 _dataCY = m_volumData->getCY();
		muint32 _dataCZ = m_volumData->getCZ();

		// + 0.5
		/*muint32 newCX = (muint32)((m_volumData->getSpaceX(true)*_dataCX) + 0.5);
		muint32 newCY = (muint32)((m_volumData->getSpaceY(true)*_dataCY) + 0.5);
		muint32 newCZ = (muint32)((m_volumData->getSpaceZ(true)*_dataCZ) + 0.5);*/

		// 
		//muint32 newCX = (muint32)((m_volumData->getSpaceX(true)*_dataCX));
		//muint32 newCY = (muint32)((m_volumData->getSpaceY(true)*_dataCY));
		//muint32 newCZ = (muint32)m_volumData->getCZ(); //(muint32)((m_volumData->getSpaceZ(true)*_dataCZ));

		// 
		muint32 newCX = (muint32)_dataCX;
		muint32 newCY = (muint32)_dataCY;
		muint32 newCZ = (muint32)_dataCZ;

		qDebug() << newCX;
		qDebug() << newCY;
		qDebug() << newCZ;

		muint32 dataCX = _dataCX;
		muint32 dataCY = _dataCY;

		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			short *newHUData = new short[dataCX*dataCY*dataCZ[1]];
			memset(newHUData, 0, sizeof(short) * (dataCX*dataCY*dataCZ[1]));

			mask * newData[4];

			for (int i = 0; i < 4; i++)
				newData[i] = 0;

			for (int i = 0; i <= m_maskIndex; i++)
			{
				newData[i] = new mask[dataCX*dataCY*dataCZ[1]];
				memset(newData[i], 0, (dataCX*dataCY*dataCZ[1]));
				file.read((char*)newData[i], dataCX*dataCY*dataCZ[1]);
			}

			file.read((char*)newHUData, sizeof(mint16)*(dataCX*dataCY*dataCZ[1]));
			file.close();

			m_volumData->createData(newHUData, dataCX, dataCY, dataCZ[1],
				m_volumData->getSpaceX(), m_volumData->getSpaceY(), spaceZ[1] / 10, false);

			for (int i = 0; i <= m_maskIndex; i++)
			{
				m_volumData->createMaskData(newData[i], i);
				SAFE_DELETES(newData[i]);
			}
			SAFE_DELETES(newHUData);
		}
		else
		{
			qDebug() << "else";

			QString filename2 = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
			QFile file2(filename2);

			file2.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file2.open(QIODevice::WriteOnly))
			{
				for (int i = 0; i <= m_maskIndex; i++)
				{
					// 현재 데이터 캐시 저장
					file2.write((char*)m_volumData->getMaskDataPoint(i), m_volumData->getVolumeDataLength());
				}
				file2.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
				file2.close();
			}

			int updateVolumeSize = newCX*newCY*newCZ;
			short *newHUData = new short[updateVolumeSize];
			memset(newHUData, 0, sizeof(short) * (updateVolumeSize));
			memcpy(newHUData, m_volumData->pData3D_HU_Temp, sizeof(short) * (updateVolumeSize));

			mask * newData[4];
			for (int i = 0; i < 4; i++)
				newData[i] = 0;

			for (int i = 0; i <= m_maskIndex; i++)
			{
				newData[i] = new mask[updateVolumeSize];
				memset(newData[i], 0, (updateVolumeSize));

				memcpy_s(newData[i], (updateVolumeSize), m_volumData->getMaskTempDataPoint(i), (updateVolumeSize));

				//	memcpy(newData[i], m_volumData->getMaskTempDataPoint(i), (dataCX*dataCY*dataCZ[1]));
			}

			//m_volumData->createData(newHUData, dataCX, dataCY, dataCZ[1], m_volumData->getSpaceX(), m_volumData->getSpaceY(), spaceZ[1] / 10, false);
			// magicCut 값
			float fSpaceXYResizing = 0.976f * 0.1f;	// AI 트레이닝 셋 기준
			m_volumData->createData(newHUData, newCX, newCY, newCZ, fSpaceXYResizing, fSpaceXYResizing, m_volumData->getSpaceZ(), false);


			for (int i = 0; i <= m_maskIndex; i++)
			{
				m_volumData->createMaskData(newData[i], i);
				SAFE_DELETES(newData[i]);
			}
			SAFE_DELETES(newHUData);

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				for (int i = 0; i <= m_maskIndex; i++)
					file.write((char*)m_volumData->getMaskDataPoint(i), m_volumData->getVolumeDataLength());

				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));

				file.close();
			}


			//voxel count & bounding box
			//		m_volumData->getAllMaskBoundingBox();
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

WorkImageIsotropicConversionModification::WorkImageIsotropicConversionModification(VOLUME_DATA* pVolumeData)
{
	_volumData = pVolumeData;
}

void WorkImageIsotropicConversionModification::updateProgress(int val, void*data)
{
	WorkImageIsotropicConversionModification* worker = (WorkImageIsotropicConversionModification*)data;

	worker->setProgressValue(val);
}

void WorkImageIsotropicConversionModification::threadRun() // 2017.08.18 이두희 팀장 추가
{
	muint32 dataCX = _volumData->getCX();
	muint32 dataCY = _volumData->getCY();
	muint32 dataCZ = _volumData->getCZ();
	qDebug() << "slice count _ X, Y, Z : " << dataCX << ", " << dataCY << ", " << dataCZ;

	float spaceX = _volumData->getSpaceX(true);
	float spaceY = _volumData->getSpaceY(true);
	float spaceZ = _volumData->getSpaceZ(true);
	qDebug() << "space _ X, Y, Z : " << _volumData->getSpaceX(true) << ", " << _volumData->getSpaceY(true) << ", " << _volumData->getSpaceZ(true);

	setProgressValue(50, true);
	setProgressValue(60);
	mmip::Isotropic iso(dataCX, dataCY, dataCZ, spaceX, spaceY, spaceZ, &(_volumData->threadStop));

	iso.setShortIsotropic(_volumData->getHUDataPoint());
	iso.setProgress(updateProgress, this);

	int newCX = iso.getIsoCount(0);
	int newCY = iso.getIsoCount(1);
	int newCZ = iso.getIsoCount(2); // 0:x , 1:y , 2:z

	qDebug("newSize : %d, %d, %d", newCX, newCY, newCZ);

	bool wholeMode = true;
	if (wholeMode) {
		_volumData->createTempHUData(true, iso.getIsoCount(0)* iso.getIsoCount(1)* iso.getIsoCount(2));
	}
	else {
		_volumData->createTempHUData(true, dataCX*dataCY*newCZ);
	}

	// volume interpolation
	iso.startIsotropic(_volumData->pData3D_HU_Temp);

	if (_volumData->threadStop)
	{
		emit finished();
		return;
	}

	setProgressValue(70);
	qDebug() << "setProgressValue(70);";

	// 	
	muint32 xFitting = dataCX;
	muint32 yFitting = dataCY;
	muint32 zFitting = dataCZ;
	muint32 sizeFitting = xFitting * yFitting * zFitting;

	mint16 *huTemp = _volumData->pData3D_HU_Temp;
	mint16 *huReplace = new mint16[sizeFitting];
	for (int z = 0; z < zFitting; z++) {
		for (int y = 0; y < yFitting; y++) {
			for (int x = 0; x < xFitting; x++) {
				int index = z*yFitting*xFitting + y*xFitting + x;
				huReplace[index] = -1024;
			}
		}
	}

	//
	int posStartXY = (dataCX - newCX) / 2;
	int posStartZ = (dataCZ - newCZ) / 2;

	for (int z = posStartZ; z < posStartZ + newCZ; z++) {
		for (int y = posStartXY; y < posStartXY + newCY; y++) {
			for (int x = posStartXY; x < posStartXY + newCX; x++) {

				int index = z*dataCY*dataCX + y*dataCX + x;
				int indexHuTemp = (z - posStartZ)*newCX*newCY + (y - posStartXY)*newCX + (x - posStartXY);
				huReplace[index] = huTemp[indexHuTemp];

			}
		}
	}

	_volumData->pData3D_HU_Temp = huReplace;
	delete[] huTemp;

	setProgressValue(90);
	qDebug() << "setProgressValue(90);";


	//// mask(layer) interpolation
	int maskIndex = 0;
	if (NULL != _volumData->getAtLastMaskInfo()) {
		maskIndex = _volumData->getAtLastMaskInfo()->uid >= MASK_SECOND_MAX ? (_volumData->getAtLastMaskInfo()->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	}

	const int maskTotal = _volumData->getMaskInfoListCnt();
	for (int i = 0; i <= maskIndex; i++) // maskIndex : 8 8 8 8 (32)
	{
		//_volumData->createTempMaskData(true, dataCX*dataCY*newCZ, i);				
		//int sizeMaskVolume = newCX*newCY*newCZ;
		int sizeMaskVolume = sizeFitting;
		_volumData->createTempMaskData(true, sizeMaskVolume, i); // pData3D_Mask_Temp

		memcpy(_volumData->getMaskTempDataPoint(i), _volumData->getMaskDataPoint(i), sizeMaskVolume * sizeof(mask));

		//for (int j = 0; j < maskTotal; j++)
		//{
		//	if (_volumData->isEmptyMaskVoxel(j))
		//	{
		//		continue;
		//	}

		//	MaskInfo *info = _volumData->getMaskInfo(j);

		//	int index = _volumData->getMaskInfo(j)->uid >= MASK_SECOND_MAX ? (_volumData->getMaskInfo(j)->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

		//	if (index != i) continue;

		//	mask maskID = info->uid >= MASK_SECOND_MAX ? info->mask_id2 : info->mask_id;

		//	iso.setUCharIsotropic(_volumData->getMaskDataPoint(i), maskID);

		//	iso.startIsotropic(_volumData->getMaskTempDataPoint(i));

		//	//setProgressValue(50 + (j / (float)maskTotal) * 50);

		//	if (_volumData->threadStop)
		//	{
		//		emit finished();
		//		return;
		//	}

		//}
	}

	setProgressValue(100);
	qDebug() << "setProgressValue(100);";

	// ThreadEnd에서 호출 시 해당 값이 사용된다. 
	// undoStack->push(new ActionImageIsotropicConversion(WIN_MANAGER->volume_data.threadResult))
	_volumData->threadResult = newCZ;

	emit finished();


}

//void WorkImageIsotropicConversionModification::setProgressValue(int value, bool init)
//{
//	static int val = 0;
//
//	if (init)
//	{
//		val = 0;
//		emit progress(val);
//		return;
//	}
//
//	if (100 <= val)
//		return;
//
//	if ((val + 10) <= value)
//	{
//		val = value;
//		emit progress(val);
//	}
//}
