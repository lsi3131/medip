#include "stdafx.h"
#include "WorkNIIOpen.h"
#include "ActionManager.h"
#include "stringManager.h"
#include "ProductManager.h"
#include "windowManager.h"

WorkNIIOpen::WorkNIIOpen(const QString& filename, VOLUME_DATA* volume_data)
{
	_volumData = volume_data;
	_strFilename = filename;
	_addValue = _result = 0;
}

void WorkNIIOpen::setProgressValue(int value, bool init /*= false*/)
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

void WorkNIIOpen::threadRun() //~80
{
	setProgressValue(0, true);
	_addValue = 1;

	_volumData->threadResult = -1;

	vtkSmartPointer<vtkNIFTIImageReader> reader =
		vtkSmartPointer<vtkNIFTIImageReader>::New();

	reader->SetFileName(_strFilename.toLocal8Bit().constData());
	reader->Update();

	if (reader->GetNIFTIHeader())
	{
		double quaterB = reader->GetNIFTIHeader()->GetQuaternB();
		double quaterC = reader->GetNIFTIHeader()->GetQuaternC();
		double quaterD = reader->GetNIFTIHeader()->GetQuaternD();
		std::cout << quaterB << std::endl;
		std::cout << quaterC << std::endl;
		std::cout << quaterD << std::endl;

		double qoffX_B = reader->GetNIFTIHeader()->GetQOffsetX();
		double qoffY_B = reader->GetNIFTIHeader()->GetQOffsetY();
		double qoffZ_B = reader->GetNIFTIHeader()->GetQOffsetZ();


		std::cout << qoffX_B << std::endl;
		std::cout << qoffY_B << std::endl;
		std::cout << qoffZ_B << std::endl;

		double* srow_x;
		double* srow_y;
		double* srow_z;

		srow_x = reader->GetNIFTIHeader()->GetSRowX();
		srow_y = reader->GetNIFTIHeader()->GetSRowY();
		srow_z = reader->GetNIFTIHeader()->GetSRowZ();

		std::cout << srow_x[0] << std::endl;
		std::cout << srow_x[1] << std::endl;
		std::cout << srow_x[2] << std::endl;
		std::cout << srow_x[3] << std::endl;

		std::cout << srow_y[0] << std::endl;
		std::cout << srow_y[1] << std::endl;
		std::cout << srow_y[2] << std::endl;
		std::cout << srow_y[3] << std::endl;

		std::cout << srow_z[0] << std::endl;
		std::cout << srow_z[1] << std::endl;
		std::cout << srow_z[2] << std::endl;
		std::cout << srow_z[3] << std::endl;

		vtkNIFTIImageHeader::DataTypeEnum dtType = (vtkNIFTIImageHeader::DataTypeEnum)reader->GetNIFTIHeader()->GetDataType();
		int* dim = reader->GetOutput()->GetDimensions();
		int width = dim[0];
		int height = dim[1];
		int slice = dim[2];

		setProgressValue(_addValue * 10);

		_addValue++;


		if ((dtType == vtkNIFTIImageHeader::TypeUInt8) && PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask))//roi nii
		{
			if (!_volumData->isValidate())
			{
				ACTION_MANAGER->actionResult.SetErrorResult(
					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD),
					STRING_MANAGER->getString(STR_LOAD_FIRST));

				emit finished();
				return;
			}


			if (width != _volumData->getCX() ||
				height != _volumData->getCY() ||
				slice != _volumData->getCZ())
			{
				emit finished();

				return;
			}


			vtkDataArray* scalars = vtkShortArray::New();
			scalars = reader->GetOutput()->GetPointData()->GetScalars();

			if (_volumData->getMaskInfoListCnt() >= MASK_MAX)
			{
				ACTION_MANAGER->actionResult.SetErrorResult(
					STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD),
					STRING_MANAGER->getString(STR_DELETE_FIRST));

				emit finished();

				return;
			}

			bool res = _volumData->createTempMaskData();

			if (!res)
			{
				emit finished();

				return;
			}

			mask* image = _volumData->getMaskTempDataPoint();

			int dataLenth = _volumData->getVolumeDataLength();

			int zVal;
			for (int z = 0; z < slice; z++)
			{
				zVal = slice - z - 1;
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						muint32 index = (z * width * height) + (y * width) + x;

						if (index > (dataLenth - 1)) continue;

						if ((uchar)scalars->GetTuple1(zVal * height * width + y * width + x) != 0)
						{
							image[index] = 1;
						}
					}
				}
				if ((((float)slice / 8) * _addValue) <= z)
				{
					setProgressValue(_addValue * 10);
					_addValue++;
				}
				if (_volumData->threadStop == true)
				{
					emit finished();
					return;
				}
			}

			_volumData->threadResult = 1;
		}
		else if ((dtType == vtkNIFTIImageHeader::TypeInt16) && PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Import_NII_HU)) //hu nii
		{
			if (ACTION_MANAGER->GetAfterThread() == THREAD_IMPORT_FILES)
			{
				ACTION_MANAGER->actionResult.SetErrorResult("Import files", "Multi-open only supports ROI NII files.");
				emit finished();
				return;
			}

			WIN_MANAGER->setRenderable(false);
			double* spacing;
			spacing = reader->GetDataSpacing();

			double m_xspacing = spacing[0] * 0.1f;
			double m_yspacing = spacing[1] * 0.1f;
			double m_zspacing = spacing[2] * 0.1f;

			vtkDataArray* scalars = vtkShortArray::New();
			scalars = reader->GetOutput()->GetPointData()->GetScalars();

			emit resetRC();

			_volumData->createData(width, height, slice, m_xspacing, m_yspacing, m_zspacing);
			double rescaleSlope = reader->GetNIFTIHeader()->GetSclSlope();
			double rescaleInter = reader->GetNIFTIHeader()->GetSclInter();
			double offset = 0.0;
			if (rescaleSlope == 0.0 || rescaleInter == 0.0)
			{
				offset = 0.0;
			}
			else
			{
				offset = rescaleInter / rescaleSlope;
			}

			int zVal;
			for (int z = 0; z < slice; z++)
			{
				zVal = slice - z - 1;
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						short value = (short)scalars->GetTuple1(zVal * height * width + y * width + x) + offset;
						_volumData->setData(x, y, z, value);
					}
				}
				if ((((float)slice / 8) * _addValue) <= z)
				{
					setProgressValue(_addValue * 10);
					_addValue++;
				}
				if (_volumData->threadStop == true)
				{
					emit finished();
					return;
				}
			}

			int nRan = _volumData->getHuMax() - _volumData->getHuMin();
			emit initUI(nRan, nRan / 2 + _volumData->getHuMin());

			setProgressValue(_addValue * 10);
			_addValue++;

			WIN_MANAGER->setRenderable(true);
			WIN_MANAGER->setSaveState(true);

			_volumData->threadResult = 2;
		}
	}

	emit finished();

	return;
}