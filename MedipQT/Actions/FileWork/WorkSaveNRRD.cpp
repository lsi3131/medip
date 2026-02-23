#include "stdafx.h"
#include "WorkSaveNRRD.h"
#include <NRRD/NRRD/nrrd.h>
#include <NRRD/NRRD/nrrd_image.h>
#include <NRRD/NRRD/nrrd_image_view.h>

// WorkSaveNRRD - Start ////////////////////////////////////////////////
WorkSaveNRRD::WorkSaveNRRD(VOLUME_DATA* pVolumeData, const QString& strFileName, bool patchy, BoundingBoxI box)
{
	m_pVolumeData = pVolumeData;
	_FileName = strFileName;
	_patchy = patchy;
	_box = box;
	_addValue = 0;
}


void WorkSaveNRRD::setProgressValue(int value, bool init)
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

#if 1 // 이미지 사이즈 반으로 줄이기
void WorkSaveNRRD::threadRun()
{
	/*double d_scaleX = 0.5;
	double d_scaleY = 0.5;
	double d_scaleZ = 0.5;*/
	// volume rawdata 및 nrrd x/y 축으로 절반으로 downsizing
	double d_scaleX = 0.5;
	double d_scaleY = 0.5;
	double d_scaleZ = 1.0;

	int width = m_pVolumeData->getCX();
	int height = m_pVolumeData->getCY();
	int slice = m_pVolumeData->getCZ();
	int nTargetWidth = width * d_scaleX;
	int nTargetHeight = height * d_scaleY;
	int nTargetSlice = slice * d_scaleZ;
	double x_spacing = m_pVolumeData->getSpaceX(true);
	double y_spacing = m_pVolumeData->getSpaceY(true);
	double z_spacing = m_pVolumeData->getSpaceZ(true);

	bool res = m_pVolumeData->createTempHUData();
	QString _TempName = _FileName;

	setProgressValue(0, true);
	_TempName.append(".tmp_");
	if (!res)
	{
		m_pVolumeData->threadResult = FALSE;
		emit finished();
		return;
	}
	else
	{
		_addValue = 0;
		//		short *pS = m_pVolumeData->pData3D_HU_Temp;
		//		unsigned char *pS = (unsigned char*)malloc(sizeof(unsigned char)*width*height*slice);
		unsigned char* pS = (unsigned char*)malloc(sizeof(unsigned char) * nTargetWidth * nTargetHeight * nTargetSlice);

		bool resX, resY, resZ;

		resX = resY = resZ = false;

		for (int z = 0; z < nTargetSlice; z++)
		{
			if (_patchy)
			{
				if (_box.minZ * d_scaleZ <= z && z <= _box.maxZ * d_scaleZ)
					resZ = true;
				else
					resZ = false;
			}

			for (int y = 0; y < nTargetHeight; y++)
			{
				if (_patchy)
				{
					if (_box.minY * d_scaleY <= y && y <= _box.maxY * d_scaleY)
						resY = true;
					else
						resY = false;
				}

				for (int x = 0; x < nTargetWidth; x++)
				{
					if (_patchy)
					{
						if (_box.minX * d_scaleX <= x && x <= _box.maxX * d_scaleX)
							resX = true;
						else
							resX = false;
					}
					if ((resX && resY && resZ) || !_patchy)
					{
						//						pS[(nTargetSlice - z - 1)* nTargetHeight * nTargetWidth + y * nTargetWidth + x] = m_pVolumeData->getData(z * 2 * height * width + y * 2 * width + x * 2);
						// 16bit gray -> 8bit gray color 변경
						pS[(nTargetSlice - z - 1) * nTargetHeight * nTargetWidth + y * nTargetWidth + x] = (m_pVolumeData->getData((z / d_scaleZ) * height * width + (y / d_scaleY) * width + (x / d_scaleX)) - m_pVolumeData->getHuMin()) * 255.0 / (double)(m_pVolumeData->getHuMax() - m_pVolumeData->getHuMin());
					}
					else
						pS[(nTargetSlice - z - 1) * nTargetHeight * nTargetSlice + y * nTargetWidth + x] = -3024;
				}
			}
			if ((((float)(slice * d_scaleZ) / 8) * _addValue) <= z)
			{
				_addValue++;
				setProgressValue(_addValue * 10);
			}
		}

		NRRD::ImageView <unsigned char>image_view;
		//		unsigned char *pTarget = image_view.convert_type<short, unsigned char>(pS, width*height*slice);	

		image_view.set(nTargetWidth, nTargetHeight, nTargetSlice, pS);
		//		image_view.set(width, height, slice, pTarget);

		//		NRRD::save("test.nrrd", pS, (int)NRRD::dim.size(), &(NRRD::dim[0]));
		image_view.nrrd_header["spacings"] = vectorToString<double>(image_view.element_spacing, " ");

		image_view.save(m_pVolumeData, _TempName.toLocal8Bit().constData(), d_scaleX, d_scaleY, d_scaleZ);
		//		image_view.resample_onefile(_TempName.toLocal8Bit().constData());

		if (pS)
		{
			free(pS);
			pS = NULL;
		}

		_addValue = 100;
		setProgressValue(_addValue);

		QFile orgFile(_FileName);

		bool res = true;

		if (orgFile.exists())
			res = orgFile.remove();

		if (res)
		{
			QFile file(_TempName);
			if (file.rename(_FileName))
				m_pVolumeData->threadResult = TRUE;
		}

		emit finished();
	}
}
#else

void WorkSaveNRRD::threadRun()
{
	VOLUME_DATA* volume_data = &WIN_MANAGER->volume_data;
	int width = m_pVolumeData->getCX();
	int height = m_pVolumeData->getCY();
	int slice = m_pVolumeData->getCZ();
	double x_spacing = m_pVolumeData->getSpaceX(true);
	double y_spacing = m_pVolumeData->getSpaceY(true);
	double z_spacing = m_pVolumeData->getSpaceZ(true);

	bool res = m_pVolumeData->createTempHUData();
	QString _TempName = _FileName;

	setProgressValue(0, true);
	_TempName.append(".tmp_");
	if (!res)
	{
		m_pVolumeData->threadResult = FALSE;
		emit finished();
		return;
	}
	else
	{
		_addValue = 0;
		//		short *pS = m_pVolumeData->pData3D_HU_Temp;
		//		char *pS = (char*)m_pVolumeData->pData3D_HU_Temp;
		unsigned char* pS = (unsigned char*)malloc(sizeof(unsigned char) * width * height * slice);

		bool resX, resY, resZ;

		resX = resY = resZ = false;

		for (int z = 0; z < slice; z++)
		{
			//			strWriteTxt = "\r?\n";
			//			fputs(strWriteTxt.toStdString().c_str(), fp);

			if (_patchy)
			{
				if (_box.minZ <= z && z <= _box.maxZ)
					resZ = true;
				else
					resZ = false;
			}

			for (int y = 0; y < height; y++)
			{
				if (_patchy)
				{
					if (_box.minY <= y && y <= _box.maxY)
						resY = true;
					else
						resY = false;
				}

				for (int x = 0; x < width; x++)
				{
					if (_patchy)
					{
						if (_box.minX <= x && x <= _box.maxX)
							resX = true;
						else
							resX = false;
					}
					if ((resX && resY && resZ) || !_patchy)
					{
						//						pS[(slice - z - 1)* height * width + y * width + x] = m_pVolumeData->getData(z * height * width + y * width + x);												
						// 16bit gray -> 8bit gray color 변경
						pS[(slice - z - 1) * height * width + y * width + x] = (m_pVolumeData->getData(z * height * width + y * width + x) - m_pVolumeData->getHuMin()) * 255.0 / (double)(m_pVolumeData->getHuMax() - m_pVolumeData->getHuMin());
					}
					else
					{
						pS[(slice - z - 1) * height * width + y * width + x] = -3024;
					}
				}

			}
			if ((((float)slice / 8) * _addValue) <= z)
			{
				_addValue++;
				setProgressValue(_addValue * 10);
			}
		}

		NRRD::ImageView <unsigned char>image_view;

		//		unsigned char *pTarget = image_view.convert_type<short, unsigned char>(pS, width*height*slice);
		image_view.set(width, height, slice, pS);
		//		image_view.set(width, height, slice, pTarget);

		//		NRRD::save("test.nrrd", pS, (int)NRRD::dim.size(), &(NRRD::dim[0]));
		image_view.nrrd_header["spacings"] = vectorToString<double>(image_view.element_spacing, " ");

		image_view.save(_TempName.toLocal8Bit().constData());

		if (pS)
		{
			free(pS);
			pS = NULL;
		}

		_addValue = 100;
		setProgressValue(_addValue);

		QFile orgFile(_FileName);

		bool res = true;

		if (orgFile.exists())
			res = orgFile.remove();

		if (res)
		{
			QFile file(_TempName);
			if (file.rename(_FileName))
				m_pVolumeData->threadResult = TRUE;
		}

		emit finished();
	}
}
#endif