#include "stdafx.h"
#include "ActionRegionGrowing.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
#include "graphics/BoundingBox.h"
//#include "graphics/TVM_3D.h"
#include "algorithm/MagicCut.h"

ActionRegionGrowing::ActionRegionGrowing(LAYER_RG_SHORTCUT type,VOLUME_DATA * volumData, mask _m, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent),
	m_type(type)
{
	static int s_id = ACT_ID_REGIONGROWING;
	memset(m_fillCount, 0, sizeof(m_fillCount));
	m_mask = _m;
	m_maskIndex = _mI;
	m_volumData = volumData;
	m_uid = m_volumData->getCurrentMaskInfo()->uid;
	//	m_fillCount[0] = m_volumData->voxelCount[m_uid];
	//	m_orgbox[0] = m_volumData->boundingBoxROI[m_uid];
	m_fillCount[0] = m_volumData->getVoxelCount(m_uid);
	m_TAState = m_volumData->getTAState(m_uid);
	m_orgbox[0] = m_volumData->getBoundingBox(m_uid);
	m_orgbox[1].reset(true);
	m_id = s_id++;
}

ActionRegionGrowing::~ActionRegionGrowing()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

bool ActionRegionGrowing::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionRegionGrowing::undo()
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
			m_volumData->forceUpdateMaskVolume();
			//	m_volumData->voxelCount[m_uid] = m_fillCount[0];
			//	m_volumData->boundingBoxROI[m_uid] = m_orgbox[0];
			m_volumData->setVoxelCount(m_uid, m_fillCount[0]);
			m_volumData->setBoundingBox(m_uid, m_orgbox[0]);
			WIN_MANAGER->forceUpdate2DViewData(false, true);
			WIN_MANAGER->renderLater_GridView();
			m_volumData->setTAState(m_uid, m_TAState);
			WIN_MANAGER->applyVoxelToUI(m_uid);
			WIN_MANAGER->setSaveState(false);
		}
	}
}

void ActionRegionGrowing::redo()
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
			if (m_type != LAYER_RG_ALT)
			{
				m_volumData->applyTempMaskBitChange(m_mask, m_maskIndex, 0, 1);
				m_fillCount[1] = m_volumData->fillMaskCount;
			}
			else
			{
				m_volumData->applyTempMaskBitDel(m_mask, m_maskIndex, 0, 1);
				m_fillCount[1] = m_volumData->fillMaskCount;
			}
			
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
		//	m_volumData->voxelCount[m_uid] += m_fillCount[1];
		//	m_volumData->boundingBoxROI[m_uid] = m_orgbox[1];
		m_volumData->setVoxelCount(m_uid, m_fillCount[1], false);
		m_volumData->setBoundingBox(m_uid, m_orgbox[1]);
		m_volumData->forceUpdateMaskVolume();
		WIN_MANAGER->forceUpdate2DViewData(false, true);
		WIN_MANAGER->renderLater_GridView();
		m_volumData->setTAState(m_uid, false);
		WIN_MANAGER->applyVoxelToUI(m_uid);
		WIN_MANAGER->setSaveState(false);
	}
}

void WorkRegionGrowing::setProgressValue(int value, bool init /*= false*/)
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

void WorkRegionGrowing::updateProgress(int val, void* data)
{
	WorkRegionGrowing* worker = (WorkRegionGrowing*)data;

	worker->setProgressValue(val);
}

void WorkRegionGrowing::threadRun() // 2017.08.18 ÀÌµÎÈñ ÆÀÀå Ãß°¡
{
	int progressVal = 0;
	int width = _volumData->getCX();
	int height = _volumData->getCY();
	int slice = _volumData->getCZ();
	int dataLenth = _volumData->getVolumeDataLength();

	unsigned char *originM = _underROI ? _volumData->getMaskDataPoint(_maskIndex) : _volumData->getMaskTempDataPoint();
	unsigned char *resM = _volumData->getMaskTempDataPoint(1);

	mip::BoundingBoxSimple workingBoundary;
	workingBoundary.minX = _volumData->getBoundingBox().minX;
	workingBoundary.minY = _volumData->getBoundingBox().minY;
	workingBoundary.minZ = _volumData->getBoundingBox().minZ;
	workingBoundary.maxX = _volumData->getBoundingBox().maxX;
	workingBoundary.maxY = _volumData->getBoundingBox().maxY;
	workingBoundary.maxZ = _volumData->getBoundingBox().maxZ;

	// RG(unsigned char *exist_mask, unsigned char *result_mask, int w, int h, int d, unsigned char chk_mask, BoundingBoxSimple boundingbox, std::vector<Position3D> *points, bool *threadStop);
	mip::RG rGrowing(originM, resM, _volumData->getCX(), _volumData->getCY(), _volumData->getCZ(), _mask, workingBoundary, (&rg_s), &(_volumData->threadStop));

	setProgressValue(0, true);

	if (!_underROI) rGrowing.setRangeMode(_volumData->getHUDataPoint(), _lower, _upper);
	rGrowing.setResultVal(_mask);
	rGrowing.expandConnectivity(!_connect_6);
	
	if (shortcutRG == LAYER_RG_SHIFT)
		rGrowing.SetRollingBall(true);
	rGrowing.setProgress(updateProgress, this);

	rGrowing.startRG();

	_volumData->threadResult = shortcutRG;
	emit finished();
}


ActionThresholdSelect::ActionThresholdSelect(VOLUME_DATA * volumData, mask _m, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_REGIONALLSEL;
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
	m_orgbox[1].reset(true);
	m_id = s_id++;
}

ActionThresholdSelect::~ActionThresholdSelect()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

bool ActionThresholdSelect::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionThresholdSelect::Do()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		QString filename = STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id);
		QFile file(filename);


		m_volumData->applyTempMaskBitChange(m_mask, m_maskIndex);
		m_volumData->updateUIDBoundingBox(m_uid, true);
		//	m_orgbox[1] = m_volumData->boundingBoxROI[m_uid];
		m_orgbox[1] = m_volumData->getBoundingBox(m_uid);
		m_fillCount[1] = m_volumData->fillMaskCount;


		//	m_volumData->boundingBoxROI[m_uid] = m_orgbox[1];
		//	m_volumData->voxelCount[m_uid] += m_fillCount[1];
		m_volumData->setVoxelCount(m_uid, m_fillCount[1], false);
		m_volumData->setBoundingBox(m_uid, m_orgbox[1]);
		m_volumData->forceUpdateMaskVolume();
		WIN_MANAGER->forceUpdate2DViewData(false, true);
		WIN_MANAGER->renderLater_GridView();
		m_volumData->setTAState(m_uid, false);
		WIN_MANAGER->applyVoxelToUI(m_uid);
		WIN_MANAGER->setSaveState(false);
	}
}

void ActionThresholdSelect::undo()
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
			//	m_volumData->voxelCount[m_uid] = m_fillCount[0];
			//	m_volumData->boundingBoxROI[m_uid] = m_orgbox[0];
			m_volumData->setVoxelCount(m_uid, m_fillCount[0]);
			m_volumData->setBoundingBox(m_uid, m_orgbox[0]);
			m_volumData->forceUpdateMaskVolume();
			WIN_MANAGER->forceUpdate2DViewData(false, true);
			WIN_MANAGER->renderLater_GridView();
			m_volumData->setTAState(m_uid, m_TAState);
			WIN_MANAGER->applyVoxelToUI(m_uid);
			WIN_MANAGER->setSaveState(false);
		}
	}
}

void ActionThresholdSelect::redo()
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
		m_volumData->setVoxelCount(m_uid, m_fillCount[1], false);
		m_volumData->setBoundingBox(m_uid, m_orgbox[1]);
		m_volumData->forceUpdateMaskVolume();
		WIN_MANAGER->forceUpdate2DViewData(false, true);
		WIN_MANAGER->renderLater_GridView();
		m_volumData->setTAState(m_uid, false);
		WIN_MANAGER->applyVoxelToUI(m_uid);
		WIN_MANAGER->setSaveState(false);
	}
}


void WorkThresholdSelect::setProgressValue(int value, bool init)
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

void WorkThresholdSelect::updateProgress(int value, void* data)
{
	WorkThresholdSelect* worker = (WorkThresholdSelect*)data;
	worker->setProgressValue(value);
}

void WorkThresholdSelect::threadRun()
{
	int depth = _volumData->getCZ();
	int width = _volumData->getCX();
	int height = _volumData->getCY();

	BoundingBoxI box = _volumData->getBoundingBox();

	mint16 data;
	int length = box.maxZ - box.minZ;
	int current_z = 0;
	setProgressValue(0, true);

	mip::BoundingBoxSimple workingBoundary;
	workingBoundary.minX = box.minX;
	workingBoundary.minY = box.minY;
	workingBoundary.minZ = box.minZ;
	workingBoundary.maxX = box.maxX;
	workingBoundary.maxY = box.maxY;
	workingBoundary.maxZ = box.maxZ;

	mip::Threshold thre(depth, width, height, _lower, _upper, _volumData->getHUDataPoint(),
		_volumData->getMaskTempDataPoint(), _mask, workingBoundary, &(_volumData->threadStop));

	if (_within) thre.setLayermode(_volumData->getMaskDataPoint(_maskIndex));

	thre.setProgress(updateProgress, this);

	thre.startThre();

	emit finished();
}




