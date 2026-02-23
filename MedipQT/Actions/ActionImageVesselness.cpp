#include "stdafx.h"
#include "ActionImageVesselness.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
#include "algorithm/MagicCut.h"


ActionImageVesselness::ActionImageVesselness(VOLUME_DATA * volumData, mask _m, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_VESSELNESS;
	memset(m_fillCount, 0, sizeof(m_fillCount));

	m_mask = _m;
	m_volumData = volumData;
	m_uid = m_volumData->getCurrentMaskInfo()->uid;
//	m_fillCount[0] = m_volumData->voxelCount[m_uid];
//	m_orgbox[0] = m_volumData->boundingBoxROI[m_uid];

	m_fillCount[0] = m_volumData->getVoxelCount(m_uid);
	m_orgbox[0] = m_volumData->getBoundingBox(m_uid);
	m_TAState = m_volumData->getTAState(m_uid);
	m_maskIndex = _mI;
	m_id = s_id++;
}

ActionImageVesselness::~ActionImageVesselness()
{
	QDir dir(STRING_MANAGER->cacheFilePath);
	if ( dir.exists() )
	{
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/undo%1").arg(m_id));
		dir.remove(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
	}
}

bool ActionImageVesselness::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionImageVesselness::undo()
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
	//		m_volumData->voxelCount[m_uid] = m_fillCount[0];
	//		m_volumData->boundingBoxROI[m_uid] = m_orgbox[0];
			m_volumData->setVoxelCount(m_uid, m_fillCount[0]);
			m_volumData->setBoundingBox(m_uid, m_orgbox[0]);
			m_volumData->forceUpdateMaskVolume();
			WIN_MANAGER->updatePlaneData_all();
			WIN_MANAGER->renderLater_GridView();
			m_volumData->setTAState(m_uid, m_TAState);
			WIN_MANAGER->applyVoxelToUI(m_uid);
			WIN_MANAGER->setSaveState(false);
		}
	}
}

void ActionImageVesselness::redo()
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

void WorkImageVesselness::setProgressValue(int value, bool init /*= false*/)
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

void WorkImageVesselness::threadRun()
{
	muint32 dataCX = _volumData->getCX();
	muint32 dataCY = _volumData->getCY();
	muint32 dataCZ = _volumData->getCZ();
	
	int width = dataCX;
	int height = dataCY;
	int slice = dataCZ;
	
	short *stage0_input = new short[width*height*slice];		
	short *stage1_input = new short[width/2*height/2*slice/2];
	short *stage2_input = new short[width/4*height/4*slice/4];
	
	unsigned char *stage0_mask = new unsigned char[width  * height  * slice];
	setProgressValue(0, true);
	for (int z = 0; z < slice; z++)
	{
		for (int y = 0;y < height;y++)
		{
			for (int x = 0;x < width;x++)
			{
				int idx = (width*height*z) + (x + width*y);
				stage0_input[idx] = _volumData->getData(x, y, z);
				if (_volumData->getMaskData(z*width*height + y*width + x, _maskIndex) & _mask)
					stage0_mask[(width*height*z) + (x + width*y)] = 255;
					else stage0_mask[(width*height*z) + (x + width*y)] = 0;
			}
		}

	}
	mip::Hessian::ImageDownSampling(stage0_input, stage1_input, width, height, slice);
	mip::Hessian::ImageDownSampling(stage1_input, stage2_input, width / 2, height / 2, slice / 2);
	
	setProgressValue(20);
	
	unsigned char *stage1_mask = new unsigned char[width / 2 * height / 2 * slice / 2];
	unsigned char *stage2_mask = new unsigned char[width / 4 * height / 4 * slice / 4];

	mask *outputData = _volumData->pData3D_Mask_Temp;
	
	mip::Hessian::ImageDownSampling(stage0_mask, stage1_mask, width, height, slice);
	mip::Hessian::ImageDownSampling(stage1_mask, stage2_mask, width / 2, height / 2, slice / 2);

	setProgressValue(40);
// 	WIN_MANAGER->writeRawFile(stage0_mask, width  * height  * slice , QString("D:\\stage0_mask.raw"));
// 	WIN_MANAGER->writeRawFile(stage1_mask, width / 2 * height / 2 * slice / 2, QString("D:\\stage1_mask.raw"));
// 	WIN_MANAGER->writeRawFile(stage2_mask, width / 4 * height / 4 * slice / 4, QString("D:\\stage2_mask.raw"));

	unsigned char *stage0_result = new unsigned char[width*height*slice];
	unsigned char *stage1_result = new unsigned char[width / 2 * height / 2 * slice / 2];
	unsigned char *stage2_result = new unsigned char[width / 4 * height / 4 * slice / 4];

	
	mip::Hessian::HessianMatrix(stage0_input, stage0_mask,  stage0_result, width, height, slice, HESSIAN_VESSEL);
	mip::Hessian::HessianMatrix(stage1_input, stage1_mask,  stage1_result, width/2, height/2, slice/2, HESSIAN_VESSEL);
	mip::Hessian::HessianMatrix(stage2_input, stage2_mask,  stage2_result, width/4, height/4, slice/4, HESSIAN_VESSEL);
	
	setProgressValue(60);
// 	WIN_MANAGER->writeRawFile(stage0_result, width  * height  * slice, QString("D:\\stage0_result.raw"));
// 	WIN_MANAGER->writeRawFile(stage1_result, width / 2 * height / 2 * slice / 2, QString("D:\\stage1_result.raw"));
// 	WIN_MANAGER->writeRawFile(stage2_result, width / 4 * height / 4 * slice / 4, QString("D:\\stage2_result.raw"));


	delete[]stage0_mask;
	delete[]stage1_mask;
	delete[]stage2_mask;

	delete[]stage0_input;
	delete[]stage1_input;
	delete[]stage2_input;


	unsigned char *stage0_final_result = new unsigned char[width*height*slice];
	unsigned char *stage1_final_result = new unsigned char[width / 2 * height / 2 * slice / 2];
	unsigned char *stage2_final_result = new unsigned char[width / 4 * height / 4 * slice / 4];

	for (int z = 0; z < slice; z++)
	{
		//		if (_volumData->threadStop) break;

		//		emit progress(z / float(dataCZ) * 100);
		for (int y = 0;y < height;y++)
		{
			for (int x = 0;x < width;x++)
			{

				if (stage0_result[(width*height*z) + (x + width*y)] != 0)
				{
					_volumData->setTempMaskBit(x, y, z, _mask);

				}
			}
		}
	}
	

	mip::Hessian::ImageUpSampling_trilinear(stage1_result, stage0_final_result, width/2, height/2, slice /2);
//	ImageGaussianFilter(stage0_final_result, width,height,slice);
	setProgressValue(70);
//	WIN_MANAGER->writeRawFile(stage0_final_result, width* height * slice, QString("D:\\stage1 to stage0.raw"));
	qDebug() << 1 << endl;

	for (int z = 0; z < slice; z++)
	{
//		if (_volumData->threadStop) break;

//		emit progress(z / float(dataCZ) * 100);
		for (int y = 0;y < height;y++)
		{
			for (int x = 0;x < width;x++)
			{

				if (stage0_final_result[(width*height*z) + (x + width*y)] != 0)
				{
					_volumData->setTempMaskBit(x, y, z, _mask);

				}
			}
		}
	}
	
	mip::Hessian::ImageUpSampling_trilinear(stage2_result, stage1_final_result, width/4, height /4, slice /4);
//	ImageGaussianFilter(stage1_final_result, width/2, height/2, slice/2);
	setProgressValue(80);

	mip::Hessian::ImageUpSampling_trilinear(stage1_final_result, stage0_final_result, width / 2, height / 2, slice / 2);
//	ImageGaussianFilter(stage0_final_result, width, height, slice);
//	WIN_MANAGER->writeRawFile(stage0_final_result, width  * height  * slice , QString("D:\\stage2,1 to stage0.raw"));
	setProgressValue(90);

	for (int z = 0; z < slice; z++)
	{
// 		if (_volumData->threadStop) break;
// 
// 		emit progress(z / float(dataCZ) * 100);
		for (int y = 0;y < height;y++)
		{
			for (int x = 0;x < width;x++)
			{

				if (stage0_final_result[(width*height*z) + (x + width*y)] != 0)
				{
					_volumData->setTempMaskBit(x, y, z, _mask);

				}
			}
		}
	}
	setProgressValue(100);
	qDebug() << 7 << endl;
	delete[]stage0_final_result;
	delete[]stage1_final_result;
	delete[]stage2_final_result;


	delete[]stage0_result;
	delete[]stage1_result;	
	delete[]stage2_result;


	
	emit finished();
}