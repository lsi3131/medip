#include "stdafx.h"
#include "ActionImageComponent.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
#include "ActionRegionGrowing.h"

ActionImageComponent::ActionImageComponent(VOLUME_DATA* volumData, mask _m, int _mI, QUndoCommand* parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_DILATION;

	qInfo() << "call action image component";
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

ActionImageComponent::~ActionImageComponent()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

bool ActionImageComponent::mergeWith(const QUndoCommand* command)
{
	return false;
}

void ActionImageComponent::undo()
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

void ActionImageComponent::redo()
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

void ActionImageComponent::Do()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);

		m_volumData->applyTempMaskBitChange(m_mask, m_maskIndex);
		m_fillCount[1] = m_volumData->fillMaskCount;
		m_volumData->updateUIDBoundingBox(m_uid, true);
		//		m_orgbox[1] = m_volumData->boundingBoxROI[m_uid];
		m_orgbox[1] = m_volumData->getBoundingBox(m_uid);

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

void WorkImageComponent::updateProgress(int val, void* dt)
{
	WorkImageComponent* worker = (WorkImageComponent*)dt;
	worker->setProgressValue(val);
}

void WorkImageComponent::setProgressValue(int value, bool init)
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

void WorkImageComponent::threadRun()
{
	mask* outputData = _volumData->pData3D_Mask_Temp;

	setProgressValue(10);

	MaskInfo* info = _volumData->getCurrentMaskInfo();

	int uid = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0; // 0~3 (bin)			
	unsigned char* inputData = _volumData->getMaskDataPoint(uid);

	int maskIndex = uid == 0 ? info->mask_id : info->mask_id2; // maskIndex

	qDebug() << "WorkImageComponent" << info->uid << " : " << QString().fromWCharArray(info->maskName);
	qDebug() << "WorkImageComponent" << maskIndex << endl;

	muint32 dataCX = _volumData->getCX();
	muint32 dataCY = _volumData->getCY();
	muint32 dataCZ = _volumData->getCZ();
	muint32 length = dataCX * dataCY * dataCZ;// WIN_MANAGER->volume_data.getLength();
	int* output = new int[length];
	int* result = new int[length];

	unsigned char _ml = 0;
	unsigned char* input = new unsigned char[length];

	setProgressValue(20);

	for (int i = 0; i < length; i++)
	{
		if (inputData[i] & maskIndex)
		{
			input[i] = 1;
		}

		else input[i] = 0;
		output[i] = 0;
	}

	int res = mip::LabelImage(dataCX, dataCY, dataCZ, input, output);
	setProgressValue(50);

	mip::SortLabelingMask(output, result, dataCX, dataCY, dataCZ, res);
	setProgressValue(70);

	for (int i = 0; i < length; i++)
	{
		if (result[i] != 0 && result[i] <= _componentNum)
		{
			//			inputData[i] |= maskIndex;	
			outputData[i] |= maskIndex;
		}
		else
		{
			outputData[i] = 0;
		}
	}

	delete[] result;
	delete[] input;
	delete[] output;

	setProgressValue(100);

	emit finished();
}

void CustomImageComponent::startFunc(const std::vector<unsigned char>& vecInput, std::vector<mint8>& vecResult)
{
	muint32 length = m_nCx * m_nCy * m_nCz;// WIN_MANAGER->volume_data.getLength();
	int* output = new int[length];
	int* result = new int[length];

	unsigned char* input = new unsigned char[length];

	for (int i = 0; i < length; i++)
	{
		if (vecInput[i] > m_nThreshold)
		{
			input[i] = 1;
		}
		else
		{
			input[i] = 0;
		}

		output[i] = 0;
	}

	int res = mip::LabelImage(m_nCx, m_nCy, m_nCz, input, output);
	mip::SortLabelingMask(output, result, m_nCx, m_nCy, m_nCz, res);

	for (int i = 0; i < length; i++)
	{
		if (result[i] != 0 && result[i] <= m_nComponentNum)
		{
			vecResult[i] = 1;
		}
		else
		{
			vecResult[i] = 0;
		}
	}

	delete[] result;
	delete[] input;
	delete[] output;
}
