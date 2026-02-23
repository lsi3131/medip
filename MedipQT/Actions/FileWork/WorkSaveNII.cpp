#include "stdafx.h"
#include "WorkSaveNII.h"

WorkSaveNII::WorkSaveNII(const QString& strFileName, VOLUME_DATA* pVolumeData, bool patchy, BoundingBoxI box)
{
	m_pVolumeData = pVolumeData;
	_FileName = strFileName;
	_patchy = patchy;
	_box = box;
	_addValue = 0;
}

void WorkSaveNII::setProgressValue(int value, bool init)
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

void WorkSaveNII::threadRun()
{
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
		short* pS = m_pVolumeData->pData3D_HU_Temp;

		bool resX, resY, resZ;

		resX = resY = resZ = false;

		for (int z = 0; z < slice; z++)
		{
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
						pS[(slice - z - 1) * height * width + y * width + x] = m_pVolumeData->getData(z * height * width + y * width + x);
					else
						pS[(slice - z - 1) * height * width + y * width + x] = -3024;
				}
			}
			if ((((float)slice / 8) * _addValue) <= z)
			{
				_addValue++;
				setProgressValue(_addValue * 10);
			}
		}

		vtkShortArray* scalars = vtkShortArray::New();
		scalars->SetArray(pS, width * height * slice, 1);

		vtkSmartPointer<vtkImageData> image =
			vtkSmartPointer<vtkImageData>::New();

		image->SetDimensions(width, height, slice);
		image->SetSpacing(x_spacing, y_spacing, z_spacing);

		image->AllocateScalars(VTK_SHORT, 1);
		image->GetPointData()->SetScalars(scalars);
		_addValue = 90;
		setProgressValue(_addValue);
		scalars->Delete();




		vtkSmartPointer<vtkNIFTIImageWriter> writer =
			vtkSmartPointer<vtkNIFTIImageWriter>::New();
		writer->SetInputData(image);
		writer->SetFileName(_TempName.toLocal8Bit().constData());
		writer->Write();


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

//// ¿øº»
//void WorkSaveNII::threadRun()
//{
//	VOLUME_DATA *volume_data = &WIN_MANAGER->volume_data;
//	int width = volume_data->getCX();
//	int height = volume_data->getCY();
//	int slice = volume_data->getCZ();
//	double x_spacing = volume_data->getSpaceX(true);
//	double y_spacing = volume_data->getSpaceY(true);
//	double z_spacing = volume_data->getSpaceZ(true);
//
//	bool res = volume_data->createTempHUData();
//	QString _TempName = _FileName;
//
//	setProgressValue(0, true);
//	_TempName.append(".tmp_");
//	if (!res)
//	{
//		volume_data->threadResult = FALSE;
//		emit finished();
//		return;
//	}
//	else
//	{
//		_addValue = 0;
//		short *pS = volume_data->pData3D_HU_Temp;
//
//		bool resX, resY, resZ;
//
//		resX = resY = resZ = false;
//
//		for (int z = 0; z < slice; z++)
//		{
//			if (_patchy)
//			{
//				if (_box.minZ <= z && z <= _box.maxZ)
//					resZ = true;
//				else
//					resZ = false;
//			}
//
//			for (int y = 0; y < height; y++)
//			{
//				if (_patchy)
//				{
//					if (_box.minY <= y && y <= _box.maxY)
//						resY = true;
//					else
//						resY = false;
//				}
//
//				for (int x = 0; x < width; x++)
//				{
//					if (_patchy)
//					{
//						if (_box.minX <= x && x <= _box.maxX)
//							resX = true;
//						else
//							resX = false;
//					}
//					if ((resX && resY && resZ) || !_patchy)
//						pS[(slice - z - 1)* height * width + y * width + x] = volume_data->getData(z * height * width + y * width + x);
//					else
//						pS[(slice - z - 1)* height * width + y * width + x] = -3024;
//				}
//			}
//			if ((((float)slice / 8)*_addValue) <= z)
//			{
//				_addValue++;
//				setProgressValue(_addValue * 10);
//			}
//		}
//
//		vtkShortArray *scalars = vtkShortArray::New();
//		scalars->SetArray(pS, width*height*slice, 1);
//
//		vtkSmartPointer<vtkImageData> image =
//			vtkSmartPointer<vtkImageData>::New();
//
//		image->SetDimensions(width, height, slice);
//		image->SetSpacing(x_spacing, y_spacing, z_spacing);
//
//		image->AllocateScalars(VTK_SHORT, 1);
//		image->GetPointData()->SetScalars(scalars);
//		_addValue = 90;
//		setProgressValue(_addValue);
//		scalars->Delete();
//
//
//
//		vtkSmartPointer<vtkNIFTIImageWriter> writer =
//			vtkSmartPointer<vtkNIFTIImageWriter>::New();
//		writer->SetInputData(image);
//		writer->SetFileName(_TempName.toLocal8Bit().constData());
//		writer->Write();
//
//
//		_addValue = 100;
//		setProgressValue(_addValue);
//
//		QFile orgFile(_FileName);
//
//		bool res = true;
//
//		if (orgFile.exists())
//			res = orgFile.remove();
//
//		if (res)
//		{
//			QFile file(_TempName);
//			if (file.rename(_FileName))
//				volume_data->threadResult = TRUE;
//		}
//
//		emit finished();
//	}
//}