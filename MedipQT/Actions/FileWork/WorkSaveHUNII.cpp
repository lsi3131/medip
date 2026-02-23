#include "stdafx.h"
#include "WorkSaveHUNII.h"

WorkSaveHUNII::WorkSaveHUNII(VOLUME_DATA* pVolumeData, mask _m, int _mI, const QString& strFileName)
{
	m_pVolumeData = pVolumeData;

	this->_m = _m;
	this->_mI = _mI;
	_FileName = strFileName;
	_addValue = 0;
}

void WorkSaveHUNII::threadRun()
{
	int width = m_pVolumeData->getCX();
	int height = m_pVolumeData->getCY();
	int slice = m_pVolumeData->getCZ();
	double x_spacing = m_pVolumeData->getSpaceX(true);
	double y_spacing = m_pVolumeData->getSpaceY(true);
	double z_spacing = m_pVolumeData->getSpaceZ(true);
	unsigned char _mask;
	m_pVolumeData->createTempMaskData();
	short* pm = new short[width * height * slice];

	QString _TempName = _FileName;

	_TempName.append(".tmp_");

	_addValue = 0;
	for (int z = 0; z < slice; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				_mask = m_pVolumeData->getMaskData(z * height * width + y * width + x, _mI);
				if (_mask & _m)
				{
					pm[(slice - z - 1) * height * width + y * width + x] = m_pVolumeData->getHUDataPoint()[z * height * width + y * width + x];
				}
				else
				{
					pm[(slice - z - 1) * height * width + y * width + x] = 0;
				}
			}
		}
		if ((((float)slice / 8) * _addValue) <= z)
		{
			_addValue++;
			setProgressValue(_addValue * 10);
		}
	}

	vtkShortArray* scalars = vtkShortArray::New();
	scalars->SetArray(pm, width * height * slice, 1);

	vtkSmartPointer<vtkImageData> image =
		vtkSmartPointer<vtkImageData>::New();
	image->SetDimensions(width, height, slice);
	image->SetSpacing(x_spacing, y_spacing, z_spacing);
	image->AllocateScalars(VTK_SHORT, 1);
	image->GetPointData()->SetScalars(scalars);

	scalars->Delete();
	_addValue = 90;
	setProgressValue(_addValue);


	double  qoffX_B = -m_pVolumeData->getImgPosX();
	double  qoffY_B = -m_pVolumeData->getImgPosY();
	double  qoffZ_B = m_pVolumeData->getImgPosZ();

	double srow_x[4];
	double srow_y[4];
	double srow_z[4];


	srow_x[0] = -m_pVolumeData->getSpaceX(TRUE);
	srow_x[1] = 0;
	srow_x[2] = 0;
	srow_x[3] = -m_pVolumeData->getImgPosX();

	srow_y[0] = 0;
	srow_y[1] = -m_pVolumeData->getSpaceY(TRUE);
	srow_y[2] = 0;
	srow_y[3] = -m_pVolumeData->getImgPosY();

	srow_z[0] = 0;
	srow_z[1] = 0;
	srow_z[2] = m_pVolumeData->getSpaceZ(TRUE);;
	srow_z[3] = m_pVolumeData->getImgPosZ();




	vtkSmartPointer<vtkNIFTIImageHeader> header =
		vtkSmartPointer<vtkNIFTIImageHeader>::New();

	header->SetQOffsetX(qoffX_B);
	header->SetQOffsetY(qoffY_B);
	header->SetQOffsetZ(qoffZ_B);

	header->SetSRowX(srow_x);
	header->SetSRowY(srow_y);
	header->SetSRowZ(srow_z);

	header->SetSFormCode(1);
	header->SetDescrip("MEDIP_v1.3.1.0");

	vtkSmartPointer<vtkNIFTIImageWriter> writer =
		vtkSmartPointer<vtkNIFTIImageWriter>::New();
	writer->SetInputData(image);

	writer->SetFileName(_TempName.toLocal8Bit().constData());
	writer->SetNIFTIHeader(header);
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
	delete[]pm;
	emit finished();

}

void WorkSaveHUNII::setProgressValue(int value, bool init)
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
