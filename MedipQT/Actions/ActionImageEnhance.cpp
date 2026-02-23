#include "stdafx.h"
#include "ActionImageEnhance.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "system/stringManager.h"
#include "ActionRegionGrowing.h"


ActionImageEnhance::ActionImageEnhance(VOLUME_DATA * volumData, mask _m, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_ENHANCE;

	m_mask = _m;
	m_volumData = volumData;
	m_id = s_id++;

	for (int i = 0; i < MASK_MAX; i++)
		m_TAState[i] = m_volumData->getTAState(i);

}

ActionImageEnhance::~ActionImageEnhance()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

bool ActionImageEnhance::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionImageEnhance::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if ( file.exists() && file.open(QIODevice::ReadOnly) )
		{
			file.read((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
			file.close();
		}
		m_volumData->forceUpdateVolume();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->updateVolumeTextureGL((short *)m_volumData->getHUDataPoint());
		WIN_MANAGER->mainSegmentWidget->createHUHisto();
		for (int i = 0; i < MASK_MAX; i++)
			m_volumData->setTAState(i, m_TAState[i]);

		WIN_MANAGER->setSaveState(false);
	}
}

void ActionImageEnhance::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly) )
		{
			file.read((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength()*sizeof(mint16));
			file.close();
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
				file.close();
			}

			muint32 nSlice1 = m_volumData->getCZ();
			muint32 nHeight1 = m_volumData->getCY();
			muint32 nWidth1 = m_volumData->getCX();
			int HuMin = m_volumData->getHuMin();
			for (int z = 0; z < nSlice1;z++)
			{
				for (int y = 0; y < nHeight1; y++)
				{
					for (int x = 0; x < nWidth1; x++)
					{
						m_volumData->setData(x, y, z, (short)(m_volumData->pData3D_HU_Temp_D2[z * nHeight1 * nWidth1 + y * nWidth1 + x] + 0.5) + HuMin);
					}
				}
			}

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength()*sizeof(mint16));
				file.close();
			}
		}
		m_volumData->forceUpdateVolume();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->updateVolumeTextureGL((short *)m_volumData->getHUDataPoint());
		
		for (int i = 0; i < MASK_MAX; i++)
			m_volumData->setTAState(i, false);

		WIN_MANAGER->mainSegmentWidget->createHUHisto();

		WIN_MANAGER->setSaveState(false);
	}
}




ActionImageEnhanceBackOrigin::ActionImageEnhanceBackOrigin(VOLUME_DATA * volumData, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_ENHANCE_ORIGIN;

	m_volumData = volumData;
	m_id = s_id++;

	for (int i = 0; i < MASK_MAX; i++)
		m_TAState[i] = m_volumData->getTAState(i);

}

ActionImageEnhanceBackOrigin::~ActionImageEnhanceBackOrigin()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

bool ActionImageEnhanceBackOrigin::mergeWith(const QUndoCommand *command)
{
	const ActionImageEnhanceBackOrigin *markingCommand = static_cast<const ActionImageEnhanceBackOrigin *>(command);

	if (markingCommand == NULL) return false;

	if (markingCommand->m_id != m_id ) return false;

	return true;
}

void ActionImageEnhanceBackOrigin::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
			file.close();
		}
		m_volumData->forceUpdateVolume();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->updateVolumeTextureGL((short *)m_volumData->getHUDataPoint());
		
		for (int i = 0; i < MASK_MAX; i++)
			m_volumData->setTAState(i, m_TAState[i]);
		
		WIN_MANAGER->mainSegmentWidget->createHUHisto();


		WIN_MANAGER->setSaveState(false);
	}
}

void ActionImageEnhanceBackOrigin::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
			file.close();
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
				file.close();
			}

			QFile file(STRING_MANAGER->cacheFileOriginVolume);
			if (file.exists() && file.open(QIODevice::ReadOnly))
			{
				file.read((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
				file.close();
			}

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
				file.close();
			}
		}
		m_volumData->forceUpdateVolume();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->updateVolumeTextureGL((short *)m_volumData->getHUDataPoint());

		for (int i = 0; i < MASK_MAX; i++)
			m_volumData->setTAState(i, false);

		WIN_MANAGER->mainSegmentWidget->createHUHisto();

		WIN_MANAGER->setSaveState(false);
	}
}



void WorkImageEnhance::threadRun() // 2017.08.18 ÀÌµÎÈñ ÆÀÀå Ãß°¡
{
	mask *outputData = _volumData->pData3D_Mask_Temp;
	BoundingBoxI boundingBox = _volumData->getBoundingBox();

	int nSlice1 = _volumData->getCZ();
	int nHeight1 = _volumData->getCY();
	int nWidth1 = _volumData->getCX();
	int result = 0;

	double *input = _volumData->pData3D_HU_Temp_D1;
	double *output = _volumData->pData3D_HU_Temp_D2;
	int HuMin = _volumData->getHuMin();
	for (int z = 0;z < nSlice1;z++)
	{
		for (int y = 0; y < nHeight1; y++)
		{
			for (int x = 0; x < nWidth1; x++)
			{
//				if (HuMin < -1024)
					input[z * nHeight1 * nWidth1 + y * nWidth1 + x] = _volumData->getData(z * nHeight1 * nWidth1 + y * nWidth1 + x) - HuMin;// +1024;

			}
		}
	}

	double lambda = 0.0f;
	if (_index == 1) // highest
		lambda = 0.001;
	else if (_index == 2) // Higher
		lambda = 0.005;
	else if (_index == 3) // High
		lambda = 0.009;
	else if (_index == 4) // Mid
		lambda = 0.05;
	else if (_index == 5) // Low
		lambda = 0.1;
	setProgressValue(0, true);

	result = mip::VP::riciandenoise3(output, input, nWidth1, nHeight1, nSlice1, 1, lambda, 10, boundingBox.minX, boundingBox.maxX,
		boundingBox.minY, boundingBox.maxY, boundingBox.minZ, boundingBox.maxZ, &(_volumData->threadStop), updateProgress,this);

	emit finished();
}

void WorkImageEnhance::setProgressValue(int value, bool init)
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



void WorkImageGaussian::threadRun()
{
	setProgressValue(0, true);

	BoundingBoxI boundingBox = _volumData->getBoundingBox();

	int cz = _volumData->getCZ();
	int cy = _volumData->getCY();
	int cx = _volumData->getCX();

	bool res = _volumData->createTempHUData();

	if (!res)
	{
		_volumData->threadResult = -1;
		emit finished();
		return;
	}

	short *output = _volumData->pData3D_HU_Temp;
	short *input = _volumData->getHUDataPoint();
	
	int filterWidth = kernel_size;
	int filterHeight = kernel_size;
	int filterSlice = kernel_size;
	double *kernel = (double*)malloc(sizeof(double)*filterHeight*filterSlice*filterWidth);

	mip::VP::getGaussian(kernel_size, kernel_size, kernel_size, 1, kernel, &(_volumData->threadStop),
		updateProgress, this);

	if (_volumData->threadStop)
	{
		_volumData->threadResult = -1;
		free(kernel);
		emit finished();
		return;
	}

	mip::VP::ConvolutionFiltering(output, input, kernel_size, kernel, cx, cy, cz,
		boundingBox.minX, boundingBox.maxX, boundingBox.minY, boundingBox.maxY,
		boundingBox.minZ, boundingBox.maxZ, &(_volumData->threadStop), updateProgress,this, false);

	if (_volumData->threadStop)
		_volumData->threadResult = -1;
	else
		_volumData->threadResult = 1;

	free(kernel);
	
	emit finished();
	return;

}

void WorkImageGaussian::updateProgress(int val, void* dt)
{
	WorkImageGaussian* worker = (WorkImageGaussian*)dt;
	worker->setProgressValue(val);
}

void WorkImageGaussian::setProgressValue(int value, bool init /*= false*/)
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

ActionImageGaussian::ActionImageGaussian(VOLUME_DATA * volumData, QUndoCommand * parent)
{
	static int s_id = ACT_ID_IMAGE_GAUSSIAN;
	m_id = s_id++;

	m_volumData = volumData;

	for (int i = 0; i < MASK_MAX; i++)
		m_TAState[i] = m_volumData->getTAState(i);
}

ActionImageGaussian::~ActionImageGaussian()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

void ActionImageGaussian::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
			file.close();
		}
		m_volumData->forceUpdateVolume();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->updateVolumeTextureGL((short *)m_volumData->getHUDataPoint());
		WIN_MANAGER->mainSegmentWidget->createHUHisto();
		for (int i = 0; i < MASK_MAX; i++)
			m_volumData->setTAState(i, m_TAState[i]);

		WIN_MANAGER->setSaveState(false);
	}
}

void ActionImageGaussian::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
			file.close();
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
				file.close();
			}

			memcpy_s(m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16),
				m_volumData->pData3D_HU_Temp, m_volumData->getVolumeDataLength() * sizeof(mint16));

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
				file.close();
			}
		}
		m_volumData->forceUpdateVolume();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->updateVolumeTextureGL((short *)m_volumData->getHUDataPoint());

		for (int i = 0; i < MASK_MAX; i++)
			m_volumData->setTAState(i, false);

		WIN_MANAGER->mainSegmentWidget->createHUHisto();

		WIN_MANAGER->setSaveState(false);
	}
}

bool ActionImageGaussian::mergeWith(const QUndoCommand * command)
{
	return false;
}




void WorkImageLaplacian::threadRun()
{
	setProgressValue(0, true);

	BoundingBoxI boundingBox = _volumData->getBoundingBox();

	int cz = _volumData->getCZ();
	int cy = _volumData->getCY();
	int cx = _volumData->getCX();
	//	int result = 0;

	setProgressValue(50);
	bool res = _volumData->createTempHUData();

	if (!res)
	{
		_volumData->threadResult = -1;
		emit finished();
		return;
	}
	short *output = _volumData->pData3D_HU_Temp;
	short *input = _volumData->getHUDataPoint();

	int filterWidth = kernel_size;
	int filterHeight = kernel_size;
	int filterSlice = kernel_size;
	double *kernel = (double*)malloc(sizeof(double)*filterHeight*filterSlice*filterWidth);
	mip::VP::getLaplacian(kernel_size, kernel_size, kernel_size, 1, kernel, &(_volumData->threadStop), updateProgress, this);

	if (_volumData->threadStop)
	{
		free(kernel);
		_volumData->threadResult = -1;
		emit finished();
		return;
	}

	mip::VP::ConvolutionFiltering(output, input, kernel_size, kernel, cx, cy, cz,
		boundingBox.minX, boundingBox.maxX, boundingBox.minY, boundingBox.maxY, boundingBox.minZ,
		boundingBox.maxZ, &(_volumData->threadStop), updateProgress, this, false);

	free(kernel);

	if (_volumData->threadStop)
		_volumData->threadResult = -1;
	else
		_volumData->threadResult = 1;

	emit finished();
	return;

}


void WorkImageLaplacian::updateProgress(int val, void* dt)
{
	WorkImageLaplacian* worker = (WorkImageLaplacian*)dt;
	worker->setProgressValue(val);
}

void WorkImageLaplacian::setProgressValue(int value, bool init /*= false*/)
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

ActionImageLaplacian::ActionImageLaplacian(VOLUME_DATA * volumData, QUndoCommand * parent)
{
	static int s_id = ACT_ID_IMAGE_LAPLACIAN;
	m_id = s_id++;

	m_volumData = volumData;

	for (int i = 0; i < MASK_MAX; i++)
		m_TAState[i] = m_volumData->getTAState(i);
}

ActionImageLaplacian::~ActionImageLaplacian()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

void ActionImageLaplacian::undo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
			file.close();
		}
		m_volumData->forceUpdateVolume();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->updateVolumeTextureGL((short *)m_volumData->getHUDataPoint());
		WIN_MANAGER->mainSegmentWidget->createHUHisto();
		for (int i = 0; i < MASK_MAX; i++)
			m_volumData->setTAState(i, m_TAState[i]);

		WIN_MANAGER->setSaveState(false);
	}
}

void ActionImageLaplacian::redo()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);
		if (file.exists() && file.open(QIODevice::ReadOnly))
		{
			file.read((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
			file.close();
		}
		else
		{
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
				file.close();
			}

			memcpy_s(m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16),
				m_volumData->pData3D_HU_Temp, m_volumData->getVolumeDataLength() * sizeof(mint16));

			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getHUDataPoint(), m_volumData->getVolumeDataLength() * sizeof(mint16));
				file.close();
			}
		}
		m_volumData->forceUpdateVolume();
		WIN_MANAGER->renderLater_GridView(false);
		WIN_MANAGER->updateVolumeTextureGL((short *)m_volumData->getHUDataPoint());

		for (int i = 0; i < MASK_MAX; i++)
			m_volumData->setTAState(i, false);

		WIN_MANAGER->mainSegmentWidget->createHUHisto();

		WIN_MANAGER->setSaveState(false);
	}
}

bool ActionImageLaplacian::mergeWith(const QUndoCommand * command)
{
	return false;
}

void WorkImageEnhance::updateProgress(int val, void * dt)
{
	WorkImageEnhance* worker = (WorkImageEnhance*)dt;
	worker->setProgressValue(val);
}
