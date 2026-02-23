#include "stdafx.h"
#include "DicomConverter.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/DicomDatasetIOParameter.h"
#include "filemanager/appcore/System/FilePathManager.h"
#include <qimage>
#include <qfileinfo>
#include <qdir>
#include <dcmtk/dcmdata/dcencdoc.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/cmdlnarg.h>
#include <dcmtk/dcmdata/libi2d/i2dbmps.h>
#include <dcmtk/dcmdata/libi2d/i2d.h>
#include <dcmtk/dcmdata/libi2d/i2dplsc.h>
#include <dcmtk/dcmdata/dcrledrg.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmimage/dipipng.h>
#include <dcmtk/dcmimage/diregist.h>
#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmjpeg/dipijpeg.h>
#include <dcmtk/dcmjpls/djdecode.h>
#include <dcmtk/ofstd/ofconapp.h>
#include <dcmtk/ofstd/ofexit.h>

namespace fm
{
	DicomConverter::DcmToImage::DcmToImage() :
		m_dcmImage(nullptr)
	{
		m_tempBmpImage = QString::fromStdWString(FilePathManager::Instance()->TempDirPath() + L"/temp.bmp");
	}

	DicomConverter::DcmToImage::~DcmToImage()
	{
		QFile::remove(m_tempBmpImage);

		if (m_dcmImage)
		{
			delete m_dcmImage;
			m_dcmImage = nullptr;
		}
	}

	bool DicomConverter::DcmToImage::ConvertToFile(std::wstring srcDcmFilePath, std::wstring targetImgFilePath)
	{
		QImage img;
		if (ConvertToImage(srcDcmFilePath, img) == false)
		{
			return false;
		}

		return img.save(QString::fromStdWString(targetImgFilePath));
	}

	bool DicomConverter::DcmToImage::ConvertToImage(DicomDataset& dcmDataset, QImage& image)
	{
		RegisterCodec();

		if (InitDicomImage(dcmDataset) == false)
		{
			return false;
		}

		if (WriteDicomImageToTempBMP() == false)
		{
			return false;
		}

		CleanupCodec();

		return ConvertTempBmpToImage(image);
	}

	bool DicomConverter::DcmToImage::ConvertToImage(std::wstring srcDcmFilePath, QImage& image)
	{
		DicomDatasetLoadParameter loadParam;
		loadParam.GroupLength = EGL_withoutGL;
		loadParam.ReadXfer = EXS_Unknown;
		loadParam.MaxReadLength = DCM_MaxReadLength;
		loadParam.ReadMode = ERM_autoDetect;

		DicomDataset dcmDataset;
		if (dcmDataset.LoadFromFile(QString::fromStdWString(srcDcmFilePath)) == false)
		{
			qWarning() << "fail to load dicom image : " << QString::fromStdWString(srcDcmFilePath);
			return false;
		}

		return ConvertToImage(dcmDataset, image);
	}

	void DicomConverter::DcmToImage::RegisterCodec()
	{
		E_DecompressionColorSpaceConversion decompCSconversion = EDC_photometricInterpretation;
		OFBool predictor6WorkaroundEnable = OFFalse;
		OFBool cornellWorkaroundEnable = OFFalse;
		OFBool forceSingleFragmentPerFrame = OFFalse;

		DcmRLEDecoderRegistration::registerCodecs();

		DJDecoderRegistration::registerCodecs(decompCSconversion, EUC_default,
			EPC_default, predictor6WorkaroundEnable, cornellWorkaroundEnable,
			forceSingleFragmentPerFrame);

		DJLSDecoderRegistration::registerCodecs();
	}

	void DicomConverter::DcmToImage::CleanupCodec()
	{
		DcmRLEDecoderRegistration::cleanup();
		DJDecoderRegistration::cleanup();
	}

	bool DicomConverter::DcmToImage::InitDicomImage(DicomDataset& dcmDataset)
	{
		unsigned long compatibilityMode = CIF_MayDetachPixelData | CIF_TakeOverExternalDataset | CIF_DecompressCompletePixelData;
		OFCmdUnsignedInt frameStart = 0;
		OFCmdUnsignedInt frameCount = 1;

		/* 원본의 손실이 있기 때문에 Copy 필요*/
		DcmDataset* copyDcmDataset = new DcmDataset(*dcmDataset.Data());
		E_TransferSyntax xfer = copyDcmDataset->getOriginalXfer();
		m_dcmImage = new DicomImage(
			copyDcmDataset,
			xfer,
			compatibilityMode,
			frameStart,
			frameCount);

		if (m_dcmImage->getStatus() != EIS_Normal)
		{
			qWarning() << "fail to init dicom image : " << DicomImage::getString(m_dcmImage->getStatus());
			return false;
		}

		return true;
	}

	bool DicomConverter::DcmToImage::WriteDicomImageToTempBMP()
	{
		FILE* file = NULL;
		std::wstring tempFilePathWString = m_tempBmpImage.toStdWString();
		errno_t errFile = _wfopen_s(&file, tempFilePathWString.c_str(), L"wb");
		if (errFile != 0)
		{
			qWarning() << "cannot create file " << QString::fromStdWString(tempFilePathWString);
			return false;
		}

		int frameIndex = 0;
		int result = m_dcmImage->writeBMP(file, 0, frameIndex);
		fclose(file);

		if (!result)
		{
			qWarning() << "cannot write frame";
			return false;
		}

		return true;
	}

	bool DicomConverter::DcmToImage::ConvertTempBmpToImage(QImage& image)
	{
		if (image.load(m_tempBmpImage) == false)
		{
			qWarning() << "fail to load QImage : " << m_tempBmpImage;
			return false;
		}

		QFile::remove(m_tempBmpImage);

		return true;
	}
}
