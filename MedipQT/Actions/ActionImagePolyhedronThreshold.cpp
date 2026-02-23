#include "stdafx.h"
#include "ActionImagePolyhedronThreshold.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
#include "ActionRegionGrowing.h"
#include "MaskInfoHelper.h"
#include "PolyhedronMaker.h"

ActionImagePolyhedronThreshold::ActionImagePolyhedronThreshold(VOLUME_DATA * volumData, mask _m, int _maskIndex, QUndoCommand *parent)
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

ActionImagePolyhedronThreshold::~ActionImagePolyhedronThreshold()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

bool ActionImagePolyhedronThreshold::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionImagePolyhedronThreshold::undo()
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
		//	m_volumData->voxelCount[m_uid] = m_fillCount[0];

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

void ActionImagePolyhedronThreshold::redo()
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
	//	m_volumData->voxelCount[m_uid] += m_fillCount[1];
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

void WorkImagePolyhedronThreshold::setProgressValue(int value, bool init)
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

void WorkImagePolyhedronThreshold::threadRun()
{
	mask *outputData = _volumData->pData3D_Mask_Temp;
	muint32 dataCX = _volumData->getCX();
	muint32 dataCY = _volumData->getCY();
	muint32 dataCZ = _volumData->getCZ();
	muint32 length = _volumData->getVolumeDataLength();
	MaskInfo* pMaskInfo = _volumData->findMaskInfo(_maskIndex, _mask);

	int uid = pMaskInfo->uid;
	int nVoxel = _volumData->getVoxelCount(uid);

	if (pMaskInfo && outputData && nVoxel > 0)
	{
		setProgressValue(0, true);

		//
		//cout << "void WorkImagePolyhedronThreshold::threadRun()" << endl;

		//
		int volumeSize = length;
		VOLUME_DATA* _volumeData = _volumData;
		int cx = dataCX;
		int cy = dataCY;
		int cz = dataCZ;

		if (_direction == 1) {
			// axial
			// 입력 mask
			MaskInfoHelper helper(_volumeData, pMaskInfo);
			mask* maskVolume = helper.getMaskVolume();
			mask maskValue = helper.getMaskValue();

			// polyhedron 저장용
			mask* resultVolume = new mask[volumeSize];
			mask resultValue = 1;

			PolyhedronMaker pt(cx, cy, cz, maskVolume, maskValue, resultVolume, resultValue, updateProgress, this);
			pt.process(Util::TargetPlane::Axial);

			// Hole Filling 적용		
			mask* hfTempBuffer = new mask[volumeSize];
			memcpy(hfTempBuffer, resultVolume, volumeSize);
			memset(resultVolume, 0, volumeSize);

			BoundingBoxI tempBox = _volumeData->getBoundingBox(pMaskInfo->uid);
			mip::BoundingBoxSimple workingBoundary;
			workingBoundary.minX = tempBox.minX;
			workingBoundary.minY = tempBox.minY;
			workingBoundary.minZ = tempBox.minZ;
			workingBoundary.maxX = tempBox.maxX;
			workingBoundary.maxY = tempBox.maxY;
			workingBoundary.maxZ = tempBox.maxZ;

			mip::HoleFilling hf(_volumeData->getCY(), _volumeData->getCX(), _volumeData->getCZ(),
				workingBoundary,
				_volumeData->getHuMin(), _volumeData->getHuMax(),
				1, //helper.getMaskValue(),
				_volumeData->getHUDataPoint(),
				hfTempBuffer,
				resultVolume,
				&(_volumeData->threadStop),
				mip::HoleFilling::Mode::Mode2DPlaneAxial
			);
			hf.setProgress(updateProgress, this);
			hf.startFilling();




			// Thresholding	적용
			int lowerThreshold = WIN_MANAGER->getThreLower(); // qDebug() << "lowerThreshold = " << lowerThreshold;
			int upperThreshold = WIN_MANAGER->getThreUpper(); // qDebug() << "upperThreshold = " << upperThreshold;						
			for (int z = 0; z < cz; z++) {
				for (int y = 0; y < cy; y++) {
					for (int x = 0; x < cx; x++) {
						int index = z * cx * cy + y * cx + x;

						// threshold
						if (resultVolume[index] == 1 && (lowerThreshold <= _volumeData->getHUDataPoint()[index] && _volumeData->getHUDataPoint()[index] <= upperThreshold)) {

							// no threshold
							//if (resultVolume[index] == 1) {
							outputData[index] |= helper.getMaskValue();
						}
						else {
							outputData[index] &= ~helper.getMaskValue();
						}

					}
				}
			}

			delete[] resultVolume;
			delete[] hfTempBuffer;
		}
		else if (_direction == 2) {
			// coronal
			// 입력 mask
			MaskInfoHelper helper(_volumeData, pMaskInfo);
			mask* maskVolume = helper.getMaskVolume();
			mask maskValue = helper.getMaskValue();

			// polyhedron 저장용
			mask* resultVolume = new mask[volumeSize];
			mask resultValue = 1;

			PolyhedronMaker pt(cx, cy, cz, maskVolume, maskValue, resultVolume, resultValue, updateProgress, this);
			pt.process(Util::TargetPlane::Coronal);

			// Hole Filling 적용		
			mask* hfTempBuffer = new mask[volumeSize];
			memcpy(hfTempBuffer, resultVolume, volumeSize);
			memset(resultVolume, 0, volumeSize);

			BoundingBoxI tempBox = _volumeData->getBoundingBox(pMaskInfo->uid);
			mip::BoundingBoxSimple workingBoundary;
			workingBoundary.minX = tempBox.minX;
			workingBoundary.minY = tempBox.minY;
			workingBoundary.minZ = tempBox.minZ;
			workingBoundary.maxX = tempBox.maxX;
			workingBoundary.maxY = tempBox.maxY;
			workingBoundary.maxZ = tempBox.maxZ;

			mip::HoleFilling hf(_volumeData->getCY(), _volumeData->getCX(), _volumeData->getCZ(),
				workingBoundary,
				_volumeData->getHuMin(), _volumeData->getHuMax(),
				1, //helper.getMaskValue(),
				_volumeData->getHUDataPoint(),
				hfTempBuffer,
				resultVolume,
				&(_volumeData->threadStop),
				mip::HoleFilling::Mode::Mode2DPlaneCoronal
			);
			hf.setProgress(updateProgress, this);
			hf.startFilling();


			// Thresholding	적용
			int lowerThreshold = WIN_MANAGER->getThreLower(); // qDebug() << "lowerThreshold = " << lowerThreshold;
			int upperThreshold = WIN_MANAGER->getThreUpper(); // qDebug() << "upperThreshold = " << upperThreshold;						
			for (int z = 0; z < cz; z++) {
				for (int y = 0; y < cy; y++) {
					for (int x = 0; x < cx; x++) {
						int index = z * cx * cy + y * cx + x;
						if (resultVolume[index] == 1 && (lowerThreshold <= _volumeData->getHUDataPoint()[index] && _volumeData->getHUDataPoint()[index] <= upperThreshold)) {
							outputData[index] |= helper.getMaskValue();
						}
						else {
							outputData[index] &= ~helper.getMaskValue();
						}

					}
				}
			}

			delete[] resultVolume;
			delete[] hfTempBuffer;
		}
		else if (_direction == 3) {
			// sagittal
			// 입력 mask
			MaskInfoHelper helper(_volumeData, pMaskInfo);
			mask* maskVolume = helper.getMaskVolume();
			mask maskValue = helper.getMaskValue();

			// polyhedron 저장용
			mask* resultVolume = new mask[volumeSize];
			mask resultValue = 1;

			PolyhedronMaker pt(cx, cy, cz, maskVolume, maskValue, resultVolume, resultValue, updateProgress, this);
			pt.process(Util::TargetPlane::Sagittal);

			// Hole Filling 적용		
			mask* hfTempBuffer = new mask[volumeSize];
			memcpy(hfTempBuffer, resultVolume, volumeSize);
			memset(resultVolume, 0, volumeSize);

			BoundingBoxI tempBox = _volumeData->getBoundingBox(pMaskInfo->uid);
			mip::BoundingBoxSimple workingBoundary;
			workingBoundary.minX = tempBox.minX;
			workingBoundary.minY = tempBox.minY;
			workingBoundary.minZ = tempBox.minZ;
			workingBoundary.maxX = tempBox.maxX;
			workingBoundary.maxY = tempBox.maxY;
			workingBoundary.maxZ = tempBox.maxZ;

			mip::HoleFilling hf(_volumeData->getCY(), _volumeData->getCX(), _volumeData->getCZ(),
				workingBoundary,
				_volumeData->getHuMin(), _volumeData->getHuMax(),
				1, //helper.getMaskValue(),
				_volumeData->getHUDataPoint(),
				hfTempBuffer,
				resultVolume,
				&(_volumeData->threadStop),
				mip::HoleFilling::Mode::Mode2DPlaneSagittal
			);
			hf.setProgress(updateProgress, this);
			hf.startFilling();


			// Thresholding	적용
			int lowerThreshold = WIN_MANAGER->getThreLower(); // qDebug() << "lowerThreshold = " << lowerThreshold;
			int upperThreshold = WIN_MANAGER->getThreUpper(); // qDebug() << "upperThreshold = " << upperThreshold;						
			for (int z = 0; z < cz; z++) {
				for (int y = 0; y < cy; y++) {
					for (int x = 0; x < cx; x++) {
						int index = z * cx * cy + y * cx + x;
						if (resultVolume[index] == 1 && (lowerThreshold <= _volumeData->getHUDataPoint()[index] && _volumeData->getHUDataPoint()[index] <= upperThreshold)) {
							outputData[index] |= helper.getMaskValue();
						}
						else {
							outputData[index] &= ~helper.getMaskValue();
						}

					}
				}
			}

			delete[] resultVolume;
			delete[] hfTempBuffer;
		}

		QString _maskname = QString::fromWCharArray(pMaskInfo->maskName);
		ACTION_MANAGER->m_qThreadNext.front().pTempData = make_shared<QString>(_maskname);
	}

	emit finished();
}

/*
mip::IP::Erosion(dataCX, dataCY, dataCZ,
outputData,
_volumData->getMaskDataPoint(_maskIndex),
_mask,
_direction,
&(_volumData->threadStop),
updateProgress,
this);
*/

void WorkImagePolyhedronThreshold::updateProgress(int val, void * dt)
{
	WorkImagePolyhedronThreshold* worker = (WorkImagePolyhedronThreshold*)dt;
	worker->setProgressValue(val);
}
