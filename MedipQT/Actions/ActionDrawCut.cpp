#include "stdafx.h"
#include "ActionDrawCut.h"
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "graphics/volumedata.h"
#include "system/stringManager.h"
#include "algorithm/MagicCut.h"

ActionImageDrawCut::ActionImageDrawCut(VOLUME_DATA * volumData, mask _m, int _mI, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	static int s_id = ACT_ID_IMAGE_DRAWCUT;

	m_mask = _m;
	m_volumData = volumData;
	m_maskIndex = _mI;
	m_maskNumber = volumData->getCurrentMaskInfo()->uid;
	m_fillCount = volumData->fillMaskCount;
	m_seedVoxel[0] = volumData->getVoxelCount(0, true);
	m_seedVoxel[1] = volumData->getVoxelCount(1, true);
	m_seedBox[0] = volumData->getBoundingBox(0, true);
	m_seedBox[1] = volumData->getBoundingBox(1, true);
//	m_orgbox[0] = volumData->boundingBoxROI[m_maskNumber];
	m_orgbox[0] = volumData->getBoundingBox(m_maskNumber);
	m_TAState = volumData->getTAState(m_maskNumber);
	m_id = s_id++;
}

bool ActionImageDrawCut::mergeWith(const QUndoCommand *command)
{
	return false;
}

void ActionImageDrawCut::undo()
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
			m_volumData->applyTempMask();

			m_volumData->createTempMaskData();
			file.read((char*)m_volumData->pData3D_Mask_Temp, m_volumData->getVolumeDataLength());
			file.close();

			m_volumData->applyTempMask(m_maskIndex);

		//	m_volumData->voxelCount[m_maskNumber] -= m_fillCount;
		//	m_volumData->boundingBoxROI[m_maskNumber] = m_orgbox[0];

			m_volumData->setVoxelCount(m_maskNumber, -m_fillCount,false);
			m_volumData->setBoundingBox(m_maskNumber, m_orgbox[0]);
			m_volumData->setVoxelCount(0, m_seedVoxel[0], true, true);
			m_volumData->setVoxelCount(1, m_seedVoxel[1], true, true);
			m_volumData->setBoundingBox(0, m_seedBox[0], false, true);
			m_volumData->setBoundingBox(1, m_seedBox[1], false, true);
			m_volumData->forceUpdateMaskVolume();
			WIN_MANAGER->setDrawcutMode();
			WIN_MANAGER->setSaveState(false);

			m_volumData->setTAState(m_maskNumber, m_TAState);

			WIN_MANAGER->applyVoxelToUI(m_maskNumber);
			WIN_MANAGER->applyVoxelToUI(0, true);

			WIN_MANAGER->updatePlaneData_all();
			WIN_MANAGER->renderLater_GridView(false);

		}
	}
}

void ActionImageDrawCut::redo()
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
				file.write((char*)m_volumData->getMaskDataPoint(), m_volumData->getVolumeDataLength());
				file.write((char*)m_volumData->getMaskDataPoint(m_maskIndex), m_volumData->getVolumeDataLength());
				file.close();
			}

			m_volumData->applyTempMaskBitChange(m_mask, m_maskIndex, 4);
			m_fillCount = m_volumData->fillMaskCount;
			m_volumData->updateUIDBoundingBox(m_maskNumber, true);
		//	m_orgbox[1] = m_volumData->boundingBoxROI[m_maskNumber];
			m_orgbox[1] = m_volumData->getBoundingBox(m_maskNumber);
		//	if (0 == m_maskIndex)
			m_volumData->clearMaskData(VM_MASK0|VM_MASK1, 0);
			m_volumData->setVoxelCount(0, 0, true, true);
			m_volumData->setVoxelCount(1, 0, true, true);
			m_volumData->setBoundingBox(0, BoundingBoxI(), true, true);
			m_volumData->setBoundingBox(1, BoundingBoxI(), true, true);
			file.setFileName(STRING_MANAGER->cacheFilePath + QString("/redo%1").arg(m_id));
			if (file.open(QIODevice::WriteOnly))
			{
				file.write((char*)m_volumData->getMaskDataPoint(m_maskIndex), m_volumData->getVolumeDataLength());
				file.close();
			}
		}
	//	WIN_MANAGER->volume_data.boundingBoxROI[m_maskNumber] = m_orgbox[1];
	//	m_volumData->voxelCount[m_maskNumber] += m_fillCount;

		m_volumData->setBoundingBox(m_maskNumber, m_orgbox[1]);
		m_volumData->setVoxelCount(m_maskNumber, m_fillCount, false);

		m_volumData->forceUpdateMaskVolume();
		WIN_MANAGER->setSaveState(false);
		m_volumData->setTAState(m_maskNumber, false);
		WIN_MANAGER->applyVoxelToUI(m_maskNumber);
		WIN_MANAGER->applyVoxelToUI(0, true);
		WIN_MANAGER->updatePlaneData_all();
		WIN_MANAGER->renderLater_GridView(false);
	}
}


void WorkImageDrawCut::updateprogress2(int value, void * data)
{
	WorkImageDrawCut* worker = (WorkImageDrawCut*)data;
	worker->setProgressValue(value / 2 + 50);
}

void WorkImageDrawCut::updateprogress(int value, void * data)
{
	WorkImageDrawCut* worker = (WorkImageDrawCut*)data;
	worker->setProgressValue(value / 5 * 2 + 10);
}

void WorkImageDrawCut::setProgressValue(int value, bool init)
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

void WorkImageDrawCut::processDrawCut()
{
	setProgressValue(0, true);
	muint32 width = _volumData->getCX();
	muint32 height = _volumData->getCY();
	muint32 slice = _volumData->getCZ();
	_volumData->fillMaskCount = 0;
	unsigned short *unsigned_value_mask = NULL;

	BoundingBoxI boundingBox = _volumData->getBoundingBox();

	Neighbor3D26C ***m_ppNLink3D26C = NULL;

	try
	{
		m_ppNLink3D26C = new Neighbor3D26C**[boundingBox.maxZ - boundingBox.minZ+1];
		memset(m_ppNLink3D26C, 0, sizeof(Neighbor3D26C**) * (boundingBox.maxZ - boundingBox.minZ + 1));

		for (int x = 0; x < boundingBox.maxZ - boundingBox.minZ + 1; x++)
		{
			m_ppNLink3D26C[x] = new Neighbor3D26C*[boundingBox.maxY - boundingBox.minY + 1];
			memset(m_ppNLink3D26C[x], 0, sizeof(Neighbor3D26C*) * (boundingBox.maxY - boundingBox.minY + 1));

			for (int y = 0; y < boundingBox.maxY - boundingBox.minY + 1; y++)
			{
				m_ppNLink3D26C[x][y] = new Neighbor3D26C[boundingBox.maxX - boundingBox.minX + 1];
				memset(m_ppNLink3D26C[x][y], 0, sizeof(Neighbor3D26C)*(boundingBox.maxX - boundingBox.minX + 1));
			}
		}
		setProgressValue(5);
	}
	catch (...)
	{
		_volumData->threadResult = -1;
	}

	if (_volumData->threadStop == false && _volumData->threadResult >= 0)
	{
		unsigned_value_mask = new unsigned short[width*height*slice];
		mask mk;

		int minimum_HU = _volumData->getHuMin();


		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					unsigned_value_mask[z*width*height + y*width + x] = _volumData->getData(x, y, z) - minimum_HU;
				}
			}
		}

		setProgressValue(10);
	}

	mask selMask = _mask;
	//	double GPC_lambda = 50.0;
	if (_volumData->threadStop == false && _volumData->threadResult >= 0)
	{
		qDebug() << 1 << endl;
		qDebug() << _GPC_lambda << endl;
		mip::CreateNLink3D26C_by_spacing(unsigned_value_mask, m_ppNLink3D26C, width, height, slice,
			boundingBox.minX, boundingBox.maxX, boundingBox.minY, boundingBox.maxY, boundingBox.minZ, boundingBox.maxZ, _volumData->getSpaceX(), _volumData->getSpaceZ(), _GPC_lambda, updateprogress, this);
	}

	if (_volumData->threadStop == false && _volumData->threadResult >= 0)
	{
		_volumData->threadResult = mip::InitialCut3D26C(_volumData->pData3D_Mask_Temp, _volumData->getMaskDataPoint(), m_ppNLink3D26C, unsigned_value_mask, width, height, slice,
			boundingBox.minX, boundingBox.maxX, boundingBox.minY, boundingBox.maxY, boundingBox.minZ, boundingBox.maxZ, 4, updateprogress2, this);
	}

	for (int x = 0; x < boundingBox.maxZ - boundingBox.minZ; x++)
	{
		if (m_ppNLink3D26C[x] != NULL)
		{
			for (int y = 0; y < boundingBox.maxY - boundingBox.minY; y++)
			{
				SAFE_DELETES(m_ppNLink3D26C[x][y]);
			}
		}
	}

	for (int x = 0; x < boundingBox.maxZ - boundingBox.minZ; x++)
	{
		SAFE_DELETES(m_ppNLink3D26C[x]);
	}

	SAFE_DELETES(m_ppNLink3D26C);

	SAFE_DELETES(unsigned_value_mask);
	_volumData->fillMaskCount = 0;
	mask* resMask = _volumData->getMaskDataPoint(_maskIndex);
	for (int i = 0; i < _volumData->getVolumeDataLength(); i++)
	{
		if (_volumData->pData3D_Mask_Temp[i] != 0 && (!(resMask[i] & _mask)))
			_volumData->fillMaskCount++;
	}


	//emit finished();
}

void WorkImageDrawCut::processTargetedDrawCut()
{
	setProgressValue(0, true);
	muint32 width = _volumData->getCX();
	muint32 height = _volumData->getCY();
	muint32 slice = _volumData->getCZ();
	_volumData->fillMaskCount = 0;
	unsigned short *unsigned_value_mask = NULL;

	BoundingBoxI boundingBox = _volumData->getBoundingBox();

	Neighbor3D26C ***m_ppNLink3D26C = NULL;

	//////////////////////////////////////////////////////////////////////////
	// 

	VOLUME_DATA * volumeData = _volumData;

	int startHU, endHU;
	startHU = WIN_MANAGER->getThreLower();
	endHU = WIN_MANAGER->getThreUpper();
	qDebug() << startHU << "(startHU)";
	qDebug() << endHU << "(endHU)";

	int volumeSize = width*height*slice;
	//bool *previewVolume = new bool[volumeSize];
	unsigned char *previewVolume = new unsigned char[volumeSize];
	for (int z = 0; z < slice; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {

				mint16 voxel = volumeData->getData(x, y, z);
				if (startHU <= voxel && voxel <= endHU)
				{
					previewVolume[(z*width*height) + ((y)*width) + x] = 255;
					//previewVolume[((slice - z - 1)*width*height) + ((y)*width) + x] = 255;
				}
				else
				{
					previewVolume[(z*width*height) + ((y)*width) + x] = 0;
					//previewVolume[((slice - z - 1)*width*height) + ((y)*width) + x] = 0;
				}
			}
		}
	}
	/*FILE *fp = fopen("D:\\preview.raw", "wb");
	fwrite(previewVolume, 1, volumeSize, fp);
	fclose(fp);
	SAFE_DELETES(previewVolume);
	exit(-1);*/
	// >> 계산은 똑바로, 출력은 거꾸로?

	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//
	// N-link 메모리 할당
	//
	//////////////////////////////////////////////////////////////////////////
	try
	{
		// Z 할당
		m_ppNLink3D26C = new Neighbor3D26C**[boundingBox.maxZ - boundingBox.minZ];
		memset(m_ppNLink3D26C, 0, sizeof(Neighbor3D26C**) * (boundingBox.maxZ - boundingBox.minZ));


		for (int x = 0; x < boundingBox.maxZ - boundingBox.minZ; x++)
		{
			// Y 할당
			m_ppNLink3D26C[x] = new Neighbor3D26C*[boundingBox.maxY - boundingBox.minY];
			memset(m_ppNLink3D26C[x], 0, sizeof(Neighbor3D26C*) * (boundingBox.maxY - boundingBox.minY));

			for (int y = 0; y < boundingBox.maxY - boundingBox.minY; y++)
			{
				// X 할당
				m_ppNLink3D26C[x][y] = new Neighbor3D26C[boundingBox.maxX - boundingBox.minX];
				memset(m_ppNLink3D26C[x][y], 0, sizeof(Neighbor3D26C)*(boundingBox.maxX - boundingBox.minX));
			}
		}
		setProgressValue(5);
	}
	catch (...)
	{
		_volumData->threadResult = -1;
	}


	//////////////////////////////////////////////////////////////////////////
	//
	// HU normalization ( unsigned )
	//
	//////////////////////////////////////////////////////////////////////////

	// HU값으로 volume을 구성한다. 0~(max-min)으로 normalization한다.
	if (_volumData->threadStop == false && _volumData->threadResult >= 0)
	{
		unsigned_value_mask = new unsigned short[width*height*slice];
		mask mk;

		int minimum_HU = _volumData->getHuMin();


		for (int z = 0; z < slice; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					unsigned_value_mask[z*width*height + y*width + x] = _volumData->getData(x, y, z) - minimum_HU;
				}
			}
		}

		setProgressValue(10);
	}


	//////////////////////////////////////////////////////////////////////////
	//
	// N-link 계산
	// 
	//////////////////////////////////////////////////////////////////////////

	mask selMask = _mask;
	//	double GPC_lambda = 50.0;
	if (_volumData->threadStop == false && _volumData->threadResult >= 0)
	{
		qDebug() << 1 << endl;
		qDebug() << _GPC_lambda << endl;

		// 		(
		// 			unsigned short *m_pRawVolume, 
		// 			Neighbor3D26C ***m_ppNLink3D26C, 
		// 			int width, int height, int slice, 
		// 			int x_min, int x_max, 
		// 			int y_min, int y_max, 
		// 			int z_min, int z_max, 
		// 			double xy_spacing, double z_spacing, 
		// 			double m_nLambda, 
		// 			progUpdatefunc update, 
		// 			void * data
		// 		)

		// n-link 계산, graph구성은 아래에서
		mip::CreateNLink3D26C_by_spacing_targeted(
			previewVolume,
			unsigned_value_mask,	// HU volume (unsigned)
			m_ppNLink3D26C,			// N-link
			width, height, slice,	// X Y Z
			boundingBox.minX, boundingBox.maxX, // X min max
			boundingBox.minY, boundingBox.maxY, // Y min max
			boundingBox.minZ, boundingBox.maxZ, // Z min max
			_volumData->getSpaceX(), _volumData->getSpaceZ(), // spacing
			_GPC_lambda,			// lambda
			updateprogress,
			this);

		//mip::CreateNLink3D26C_by_spacing(
		//	unsigned_value_mask,	// HU volume (unsigned)
		//	m_ppNLink3D26C,			// N-link
		//	width, height, slice, // X Y Z
		//	boundingBox.minX, boundingBox.maxX, // X min max
		//	boundingBox.minY, boundingBox.maxY, // Y min max
		//	boundingBox.minZ, boundingBox.maxZ, // Z min max
		//	_volumData->getSpaceX(), _volumData->getSpaceZ(), // spacing
		//	_GPC_lambda,			// lambda
		//	updateprogress,
		//	this);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// T-link 할당 및 graph computation	
	// : graph flow ( Source -> Sink )
	// 
	//////////////////////////////////////////////////////////////////////////

	if (_volumData->threadStop == false && _volumData->threadResult >= 0)
	{
		//int InitialCut3D26C(
		//	unsigned char *_outpudata, 
		//	unsigned char *_volumData, 
		//	Neighbor3D26C ***m_ppNLink3D26C, 
		//	unsigned short *m_pRawVolume, 
		//	int width, int height, int slice, 
		//	int x_min, int x_max, 
		//	int y_min, int y_max, 
		//	int z_min, int z_max, 
		//	unsigned char current_mask, 
		//	progUpdatefunc update, 
		//	void * data)

		_volumData->threadResult =
			mip::InitialCut3D26C_targeted(
				previewVolume,
				_volumData->pData3D_Mask_Temp,	// graph-cut 결과 저장 버퍼
				_volumData->getMaskDataPoint(), // 0번 mask 전달, (fg, bg)반영 위해서
				m_ppNLink3D26C,					// N-link
				unsigned_value_mask,			// HU (unsigned) 
				width, height, slice,
				boundingBox.minX, boundingBox.maxX,
				boundingBox.minY, boundingBox.maxY,
				boundingBox.minZ, boundingBox.maxZ,
				4,		// uid 4번 mask로 저장?
				updateprogress2,
				this
			);

		/*_volumData->threadResult =
		mip::InitialCut3D26C(
		_volumData->pData3D_Mask_Temp,
		_volumData->getMaskDataPoint(),
		m_ppNLink3D26C,
		unsigned_value_mask,
		width, height, slice,
		boundingBox.minX, boundingBox.maxX,
		boundingBox.minY, boundingBox.maxY,
		boundingBox.minZ, boundingBox.maxZ,
		4,
		updateprogress2,
		this
		);*/

		/*_volumData->threadResult = mip::InitialCut3D26C(_volumData->pData3D_Mask_Temp, _volumData->getMaskDataPoint(), m_ppNLink3D26C, unsigned_value_mask, width, height, slice,
		boundingBox.minX, boundingBox.maxX, boundingBox.minY, boundingBox.maxY, boundingBox.minZ, boundingBox.maxZ, 4, updateprogress2, this);*/
	}

	for (int x = 0; x < boundingBox.maxZ - boundingBox.minZ; x++)
	{
		if (m_ppNLink3D26C[x] != NULL)
		{
			for (int y = 0; y < boundingBox.maxY - boundingBox.minY; y++)
			{
				SAFE_DELETES(m_ppNLink3D26C[x][y]);
			}
		}
	}

	for (int x = 0; x < boundingBox.maxZ - boundingBox.minZ; x++)
	{
		SAFE_DELETES(m_ppNLink3D26C[x]);
	}

	SAFE_DELETES(m_ppNLink3D26C);

	SAFE_DELETES(unsigned_value_mask);
	_volumData->fillMaskCount = 0;
	mask* resMask = _volumData->getMaskDataPoint(_maskIndex);
	for (int i = 0; i < _volumData->getVolumeDataLength(); i++)
	{
		if (_volumData->pData3D_Mask_Temp[i] != 0 && (!(resMask[i] & _mask)))
			_volumData->fillMaskCount++;
	}

	// release
	SAFE_DELETES(previewVolume);

	//emit finished();

}

void WorkImageDrawCut::threadRun()
{
	bool bTargetedDrawCut = WIN_MANAGER->getTargetedDrawCutMode();

	if (bTargetedDrawCut) // targeted draw cut
	{	
		processTargetedDrawCut();
	}
	else // draw cut
	{
		processDrawCut(); 
	}

	emit finished();

	/*clock_t begin, end;
	begin = clock();
	end = clock();
	cout << "elapsed time : " << ((end - begin) / CLOCKS_PER_SEC) << endl;*/
}