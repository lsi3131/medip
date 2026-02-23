#include "stdafx.h"
#include "WorkSaveROINII.h"

WorkSaveROINII::WorkSaveROINII(VOLUME_DATA* pVolumeData, mask _m, int _mI, const QString& strFileName)
{
	m_pVolumeData = pVolumeData;
	this->_m = _m;
	this->_mI = _mI;
	_FileName = strFileName;
	_addValue = 0;
}

void WorkSaveROINII::threadRun()
{
	int width = m_pVolumeData->getCX();
	int height = m_pVolumeData->getCY();
	int slice = m_pVolumeData->getCZ();
	double x_spacing = m_pVolumeData->getSpaceX(true);
	double y_spacing = m_pVolumeData->getSpaceY(true);
	double z_spacing = m_pVolumeData->getSpaceZ(true);
	unsigned char _mask;
	m_pVolumeData->createTempMaskData();
	mask* pm = m_pVolumeData->getMaskTempDataPoint();

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
					pm[(slice - z - 1) * height * width + y * width + x] = 1;
				}
			}
		}
		if ((((float)slice / 8) * _addValue) <= z)
		{
			_addValue++;
			setProgressValue(_addValue * 10);
		}
	}

	vtkUnsignedCharArray* scalars = vtkUnsignedCharArray::New();
	scalars->SetArray(pm, width * height * slice, 1);

	vtkSmartPointer<vtkImageData> image =
		vtkSmartPointer<vtkImageData>::New();
	image->SetDimensions(width, height, slice);
	image->SetSpacing(x_spacing, y_spacing, z_spacing);
	image->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
	image->GetPointData()->SetScalars(scalars);

	scalars->Delete();
	_addValue = 90;
	setProgressValue(_addValue);

	double srow_x[4];
	double srow_y[4];
	double srow_z[4];


	srow_x[0] = -m_pVolumeData->getSpaceX(TRUE);
	srow_x[1] = 0;
	srow_x[2] = 0;
	srow_x[3] = 0;//-m_pVolumeData->getImgPosX();

	srow_y[0] = 0;
	srow_y[1] = -m_pVolumeData->getSpaceY(TRUE);
	srow_y[2] = 0;
	srow_y[3] = 0;//-m_pVolumeData->getImgPosY();

	srow_z[0] = 0;
	srow_z[1] = 0;
	srow_z[2] = m_pVolumeData->getSpaceZ(TRUE);;
	srow_z[3] = 0;// m_pVolumeData->getImgPosZ();

	// dicom, ITK = RAI
	// Nifti, ITK-SNAP = LPI

	//220726 kyd nii 틀어짐 현상으로 수정
	mip::VECTOR3 orientX, orientY, orientZ, origin;
	float dt[3] = {};
	m_pVolumeData->getImgOrientation(true, dt);
	orientX = mip::VECTOR3(dt[0], dt[1], dt[2]);
	m_pVolumeData->getImgOrientation(false, dt);
	orientY = mip::VECTOR3(dt[0], dt[1], dt[2]);
	orientZ = orientX ^ orientY;

	origin.x = m_pVolumeData->getImgPosX() - (orientZ.x * z_spacing) * (slice - 1);
	origin.y = m_pVolumeData->getImgPosY() - (orientZ.y * z_spacing) * (slice - 1);
	origin.z = m_pVolumeData->getImgPosZ() - (orientZ.z * z_spacing) * (slice - 1);

	srow_x[3] = -origin.x;
	srow_y[3] = -origin.y;
	srow_z[3] = origin.z;
	//
	double  qoffX_B = origin.x;
	double  qoffY_B = origin.y;
	double  qoffZ_B = -origin.z;


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

	emit finished();

}

void WorkSaveROINII::setProgressValue(int value, bool init)
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