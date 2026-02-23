#include "stdafx.h"
#include "DicomDataset.h"
#include "DicomDatasetIOParameter.h"
#include "DicomReader.h"
#include "Image/DicomDataImage_sint16.h"
#include "filemanager/appcore/System/FilePathManager.h"

#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcmetinf.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmjpeg/djdecode.h>

#include <gdcmPixmapReader.h>
#include <gdcmPixmapWriter.h>
#include <gdcmPixmap.h>
#include <gdcmJPEG2000Codec.h>
#include <gdcmJPEGCodec.h>
#include <gdcmJPEGLSCodec.h>
#include <gdcmImageChangeTransferSyntax.h>

#include "gdcmReader.h"
#include "gdcmFileDerivation.h"
#include "gdcmAnonymizer.h"
#include "gdcmVersion.h"
#include "gdcmPixmapReader.h"
#include "gdcmPixmapWriter.h"
#include "gdcmWriter.h"
#include "gdcmSystem.h"
#include "gdcmFileMetaInformation.h"
#include "gdcmDataSet.h"
#include "gdcmIconImageGenerator.h"
#include "gdcmAttribute.h"
#include "gdcmSequenceOfItems.h"
#include "gdcmUIDGenerator.h"
#include "gdcmImage.h"
#include "gdcmImageChangeTransferSyntax.h"
#include "gdcmImageApplyLookupTable.h"
#include "gdcmFileDecompressLookupTable.h"
#include "gdcmImageFragmentSplitter.h"
#include "gdcmImageChangePlanarConfiguration.h"
#include "gdcmImageChangePhotometricInterpretation.h"
#include "gdcmFileExplicitFilter.h"
#include "gdcmJPEG2000Codec.h"
#include "gdcmJPEGCodec.h"
#include "gdcmJPEGLSCodec.h"
#include "gdcmSequenceOfFragments.h"
#include "gdcmImageWriter.h"
#include "gdcmImageReader.h"

#include <qfileinfo>
#include <qdir>

namespace fm
{
	bool DicomDataset::IsDicomFile(QString filepath)
	{
		bool result = false;
		FILE* file = NULL;

		std::wstring wfilepath = filepath.toStdWString();
		const wchar_t* pWFilePath = wfilepath.c_str();
		if (filepath.size() > 0)
		{
			file = _wfopen(pWFilePath, L"rb");
		}
		if (file == NULL)
		{
			result = false;
		}
		else
		{
			char signature[4];
			if ((fseek(file, DCM_PreambleLen, SEEK_SET) < 0) ||
				(fread(signature, 1, DCM_MagicLen, file) != DCM_MagicLen))
			{
				result = false;
			}
			else if (strncmp(signature, DCM_Magic, DCM_MagicLen) != 0)
			{
				result = false;
			}
			else
			{
				/* looks ok */
				result = true;
			}
			fclose(file);
		}
		return result;
	}


	bool DicomDataset::LoadList(std::vector<DicomDataset>& dcmDataList, QString filepath)
	{
		dcmDataList.clear();

		QFileInfo fileInfo(filepath);

		if (fileInfo.isDir())
		{
			QDir dir(filepath);
			QFileInfoList fileInfoList = dir.entryInfoList(QDir::Filter::Files);
			for (QFileInfo& flist : fileInfoList)
			{
				DicomDataset dcmDataset;
				if (dcmDataset.LoadFromFile(flist.absoluteFilePath()))
				{
					dcmDataList.push_back(dcmDataset);
				}
			}
		}
		else if (fileInfo.isFile())
		{
			DicomDataset dcmDataset;
			if (dcmDataset.LoadFromFile(filepath))
			{
				dcmDataList.push_back(dcmDataset);
			}
		}

		return !dcmDataList.empty();
	}

	DicomDataset::DicomDataset()
	{
		m_pDcmDataset = new DcmDataset();
	}

	DicomDataset::~DicomDataset()
	{
		delete m_pDcmDataset;
	}

	DicomDataset::DicomDataset(DcmtkSeriesInfo* pDcmtkSeriesInfo)
	{
		m_pDcmDataset = new DcmDataset();
		Init(pDcmtkSeriesInfo);
	}

	DicomDataset::DicomDataset(DcmDataset* pDcmDataset)
	{
		m_pDcmDataset = new DcmDataset(*pDcmDataset);
	}

	DicomDataset::DicomDataset(const DicomDataset& rhs)
	{
		m_pDcmDataset = new DcmDataset(*rhs.m_pDcmDataset);
		m_extensionDataMap = rhs.m_extensionDataMap;
	}

	DicomDataset& DicomDataset::operator=(const DicomDataset& rhs)
	{
		*m_pDcmDataset = *rhs.m_pDcmDataset;
		m_extensionDataMap = rhs.m_extensionDataMap;
		return *this;
	}

	bool DicomDataset::Init(DcmtkSeriesInfo* pDcmtkSeriesInfo)
	{
		QDateTime currentDatetime = QDateTime::currentDateTime();
		QString curDateText = currentDatetime.toString("yyyyMMdd");
		QString curTimeText = currentDatetime.toString("hhmmss");

		SetTagValue(DicomTagID::StudyInstanceUID, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->strStudyUID));
		SetTagValue(DicomTagID::PatientName, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->patientsName_));
		SetTagValue(DicomTagID::PatientID, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->patientId_));
		SetTagValue(DicomTagID::PatientSize, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->patientSize_));
		SetTagValue(DicomTagID::PatientWeight, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->patientWeight_));
		SetTagValue(DicomTagID::PatientBirthDate, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->birthday_));
		SetTagValue(DicomTagID::PatientAge, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->age_));
		SetTagValue(DicomTagID::PatientSex, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->sex_));
		SetTagValue(DicomTagID::SeriesDate, curDateText.toStdWString());
		SetTagValue(DicomTagID::SeriesTime, curTimeText.toStdWString());
		SetTagValue(DicomTagID::Modality, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->modality_));
		SetTagValue(DicomTagID::StudyDate, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->studyDate_));
		SetTagValue(DicomTagID::StudyTime, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->studyTime_));
		SetTagValue(DicomTagID::StudyDescription, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->studyDescription));
		SetTagValue(DicomTagID::StudyID, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->examID));
		SetTagValue(DicomTagID::AccessionNumber, StringUtil::MultiByteToWide(pDcmtkSeriesInfo->accessionNumber));

		/* Manufacturer, Manufacturer Model -> MEDICAL IP로 설정 */
		SetTagValue(DicomTagID::Manufacturer, L"MEDICAL-IP");
		SetTagValue(DicomTagID::ManufacturerModelName, L"MEDIP");

		//m_pDcmDataset->
		return true;
	}

	bool DicomDataset::SetNewStudyInstanceUID()
	{
		char uid[256] = "";
		dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT);
		SetTagValue(fm::DicomTagID::StudyInstanceUID, StringUtil::MultiByteToWide(uid));
		return true;
	}

	bool DicomDataset::SaveToFile(QString filepath, DicomDatasetSaveParameter* pParam)
	{
		DcmFileFormat dcmff(m_pDcmDataset);
		OFCondition cond;
		if (pParam)
		{
			cond = dcmff.saveFile(
				filepath.toLocal8Bit().data(),
				pParam->WriteXfer,
				pParam->EncodingType,
				pParam->GroupLength,
				pParam->PadEncoding,
				pParam->PadLength,
				pParam->SubPadLength,
				pParam->WriteMode
			);
		}
		else
		{
			cond = dcmff.saveFile(filepath.toLocal8Bit().data());
		}
		if (cond.bad())
		{
			qWarning() << "fail to save dicom file path : " << filepath << ", result : " << cond.text();
			return false;
		}
		return true;
	}

	bool DicomDataset::LoadFromFile(QString filepath, const DicomDatasetLoadParameter* pParam)
	{
		/*
			https://groups.google.com/g/comp.protocols.dicom/c/CfaDN7mOLU8
			구버전 DICOM 파일 중에 meta information, size preamble을 제공하지 않는 Case존재한다.
			해당 파일을 지원하기 위해 .dcm, .[empty] 확장자의 경우  load를 진행한다.
			나머지 확장자는 Pass 한다.
		*/
		//if (IsDicomFile(filepath) == false)
		//{
		//	qWarning() << "invalid dicom file format data. can't load dicom file(=" << filepath << ")";
		//	return false;
		//}

		/* dcm, [empty] 확장자만 Load*/
		QString suffix = QFileInfo(filepath).suffix().toLower();
		bool canLoadable = (suffix.isEmpty() || suffix == "dcm");
		if (canLoadable == false)
		{
			qWarning() << "can't loadable dicom file format. path(=" << filepath << ")";
			return false;
		}

		DcmFileFormat dcmFileFormat;
		if (LoadDcmFileFormat(&dcmFileFormat, filepath, pParam) == false)
		{
			return false;
		}

		/* 압축 여부에 따라 압축 해제 진행 */
		DcmDataset* dataset = dcmFileFormat.getDataset();
		if (ShouldDecompress(*dataset))
		{
			qDebug() << "this dicom file should be decompressed. file decompress started. path(=" << filepath << ")";
			if (DecompressDicomFileFormat(&dcmFileFormat, filepath, pParam) == false)
			{
				qWarning() << "fail to load decompressed dicom file path : " << filepath;
				return false;
			}
		}

		E_TransferSyntax xfer = dcmFileFormat.getDataset()->getCurrentXfer();
		OFCondition cond = dcmFileFormat.chooseRepresentation(xfer, nullptr);
		if (cond.bad())
		{
			qWarning() << "fail to choose representation : " << filepath << ", result : " << cond.text();
			return false;
		}

		*m_pDcmDataset = *dcmFileFormat.getDataset();

		return true;
	}

	bool DicomDataset::SetTagValue(DicomTagID tagID, std::wstring value)
	{
		DcmTagKey tagKey = GetTagKeyByID(tagID);

		return AddElement_To_Dataset(m_pDcmDataset, tagKey, value);
	}

	bool DicomDataset::SetTagValue(DicomTagID tagID, std::string value)
	{
		return SetTagValue(tagID, StringUtil::MultiByteToWide(value));
	}

	bool DicomDataset::IsTagExists(DicomTagID tagID) const
	{
		std::wstring outValue;
		return TryGetValueText(tagID, &outValue);
	}

	bool DicomDataset::TryGetValueText(DicomTagID tagID, std::wstring* pValue, int pos) const
	{
		DcmTagKey tagKey = GetTagKeyByID(tagID);

		return GetString_From_Dataset(m_pDcmDataset, tagKey, *pValue, pos);
	}

	std::wstring DicomDataset::GetValueWString(DicomTagID tagID, int pos) const
	{
		std::wstring value;
		if (TryGetValueText(tagID, &value, pos) == false)
		{
			return L"";
		}
		return value;
	}

	std::string DicomDataset::GetValueString(DicomTagID tagID, int pos) const
	{
		return StringUtil::WideToMulitiByte(GetValueWString(tagID, pos));
	}

	std::vector<std::wstring> DicomDataset::GetValueWStringList(DicomTagID tagID, int pos) const
	{
		DcmTagKey tagKey = GetTagKeyByID(tagID);

		return GetStringList_From_Dataset(m_pDcmDataset, tagKey);
	}

	void* DicomDataset::GetValuePtr(DicomTagID tagID, int* pCount) const
	{
		OFString str = "";

		const Uint16* ptr16 = nullptr;

		//OFCondition cond = m_pDcmDataset->findAndGetString(GetTagKeyByID(tagID), ptr);
		DcmTagKey key = GetTagKeyByID(tagID);
		OFCondition cond = m_pDcmDataset->findAndGetUint16Array(GetTagKeyByID(tagID), ptr16, (unsigned long*)pCount);

		if (cond.bad())
		{
			return nullptr;
		}

		return (void*)ptr16;
	}

	void* DicomDataset::GetValuePtr(DicomTagID tagID, int width, int height) const
	{
		unsigned long expectedCount = width * height;
		int count = 0;
		void* ptr = GetValuePtr(tagID, &count);
		if (expectedCount != count)
		{
			qWarning() << QString("get value ptr count is not equal. expected %1 but actual %2").arg(expectedCount).arg(count);
			return nullptr;
		}

		return ptr;
	}

	bool DicomDataset::TryGetValueInt(DicomTagID tagID, int* pOutValue, int pos) const
	{
		DcmTagKey tagKey = GetTagKeyByID(tagID);
		std::wstring text;
		if (TryGetValueText(tagID, &text, pos) == false)
		{
			return false;
		}

		*pOutValue = _wtoi(text.c_str());

		return true;
	}

	int DicomDataset::GetValueInt(DicomTagID tagID, int pos) const
	{
		DcmTagKey tagKey = GetTagKeyByID(tagID);
		return _wtoi(GetValueWString(tagID, pos).c_str());
	}

	bool DicomDataset::TryGetValueFloat(DicomTagID tagID, float* pOutValue, int pos) const
	{
		DcmTagKey tagKey = GetTagKeyByID(tagID);
		std::wstring text;
		if (TryGetValueText(tagID, &text, pos) == false)
		{
			return false;
		}

		*pOutValue = _wtof(text.c_str());

		return true;
	}

	float DicomDataset::GetValueFloat(DicomTagID tagID, int pos) const
	{
		DcmTagKey tagKey = GetTagKeyByID(tagID);
		std::wstring str = GetValueWString(tagID, pos);
		return _wtof(str.c_str());
	}

	bool DicomDataset::TryGetImagePositionPatient(mip::VECTOR3* pOutVector) const
	{
		if (TryGetValueFloat(DicomTagID::ImagePositionPatient, &pOutVector->x, 0) == false)
		{
			return false;
		}

		if (TryGetValueFloat(DicomTagID::ImagePositionPatient, &pOutVector->y, 1) == false)
		{
			return false;
		}

		if (TryGetValueFloat(DicomTagID::ImagePositionPatient, &pOutVector->z, 2) == false)
		{
			return false;
		}

		return true;
	}

	bool DicomDataset::TryGetImageOrientationPatient(mip::VECTOR3* pOutVectorX, mip::VECTOR3* pOutVectorY) const
	{
		if (TryGetValueFloat(DicomTagID::ImageOrientationPatient, &pOutVectorX->x, 0) == false)
		{
			return false;
		}

		if (TryGetValueFloat(DicomTagID::ImageOrientationPatient, &pOutVectorX->y, 1) == false)
		{
			return false;
		}

		if (TryGetValueFloat(DicomTagID::ImageOrientationPatient, &pOutVectorX->z, 2) == false)
		{
			return false;
		}

		if (TryGetValueFloat(DicomTagID::ImageOrientationPatient, &pOutVectorY->x, 3) == false)
		{
			return false;
		}

		if (TryGetValueFloat(DicomTagID::ImageOrientationPatient, &pOutVectorY->y, 4) == false)
		{
			return false;
		}

		if (TryGetValueFloat(DicomTagID::ImageOrientationPatient, &pOutVectorY->z, 5) == false)
		{
			return false;
		}

		return true;
	}

	void DicomDataset::SetValue_Extension(std::wstring id, std::wstring data)
	{
		auto it = m_extensionDataMap.find(id);
		if (it != m_extensionDataMap.end())
		{
			(*it).second = data;
		}
		else
		{
			m_extensionDataMap.insert(std::pair<std::wstring, std::wstring>(id, data));
		}
	}

	void DicomDataset::SetValue_Extension(std::wstring id, std::string data)
	{
		return SetValue_Extension(id, StringUtil::MultiByteToWide(data));
	}


	std::wstring DicomDataset::GetValue_Extension(std::wstring id) const
	{
		auto it = m_extensionDataMap.find(id);
		if (it != m_extensionDataMap.end())
		{
			return (*it).second;
		}

		return L"";
	}

	void DicomDataset::SetFilePath_Extension(std::wstring data)
	{
		SetValue_Extension(DCM_EXT_FILE_PATH, data);
	}

	std::wstring DicomDataset::GetFilePath_Extension() const
	{
		return GetValue_Extension(DCM_EXT_FILE_PATH);
	}

	void DicomDataset::AppendDataset(DicomDataset* dataset)
	{
		std::vector<TagValuePair> tagValues = dataset->GetAllTagValues();
		for (auto& tagValue : tagValues)
		{
			SetTagValue(tagValue.first, tagValue.second);
		}
	}

	std::vector<TagValuePair> DicomDataset::GetAllTagValues() const
	{
		std::vector<TagValuePair> tagValues;
		//for (DicomTagID id : DICOM_TAG_ID_TABLE_TOTAL)
		for (int i = 0; i < GetRegisteredTagTableCount(); ++i)
		{
			DicomTagID id = DICOM_TAG_ID_TABLE_REGISTERED[i];
			std::wstring outValue;
			if (GetString_From_Dataset(m_pDcmDataset, GetTagKeyByID(id), outValue, 0))
			{
				tagValues.push_back(TagValuePair(id, outValue));
			}
		}
		return tagValues;
	}

	DcmDataset* DicomDataset::Data() const
	{
		return m_pDcmDataset;
	}

	bool DicomDataset::HasImageData() const
	{
		int width = GetValueInt(DicomTagID::Columns);
		if (width <= 0)
		{
			return false;
		}

		int height = GetValueInt(DicomTagID::Rows);
		if (height <= 0)
		{
			return false;
		}

		void* pData = GetValuePtr(fm::DicomTagID::PixelData);
		if (pData == nullptr)
		{
			return false;
		}

		return true;
	}

	bool DicomDataset::LoadDcmFileFormat(DcmFileFormat* pOutDcmFileFormat, const QString& dcmFilePath, const DicomDatasetLoadParameter* pParam)
	{
		OFCondition cond;

		/* 해당 값을 ON/OFF 할 수 있도록 설정하여 Load를 유도하도록 한다. */
		dcmEnableCP246Support.set(OFFalse);

		QString dcmFilePathNativeSeperator = QDir::toNativeSeparators(dcmFilePath);

		if (pParam)
		{
			cond = pOutDcmFileFormat->loadFile(
				dcmFilePathNativeSeperator.toLocal8Bit().data(),
				pParam->ReadXfer,
				pParam->GroupLength,
				pParam->MaxReadLength,
				pParam->ReadMode);
		}
		else
		{
			cond = pOutDcmFileFormat->loadFile(
				dcmFilePathNativeSeperator.toLocal8Bit().data(),
				EXS_Unknown,
				EGL_noChange,
				DCM_MaxReadLength,
				ERM_autoDetect);
		}

		if (cond.bad())
		{
			qWarning() << "fail to load dicom file path : " << dcmFilePathNativeSeperator << ", result : " << cond.text();
			return false;
		}

		return true;
	}

	bool DicomDataset::ShouldDecompress(const DcmDataset& dcmDataset)
	{
		/*
		* EXS_LittleEndianImplicit, EXS_LittleEndianExplicit이 아닐 경우 Decompress 진행
		*/
		E_TransferSyntax xferSyntax = dcmDataset.getCurrentXfer();
		return
			(
				((xferSyntax == EXS_LittleEndianImplicit) ||
					(xferSyntax == EXS_LittleEndianExplicit)) == false
				);
	}

	bool DicomDataset::DecompressDicomFileFormat(DcmFileFormat* pOutDcmFileFormat, const QString& dcmFilePath, const DicomDatasetLoadParameter* pParam)
	{
		/* 1. dicom file을 gdcm버전으로 load */
		gdcm::PixmapReader reader;
		if (LoadDicomFile_GDCM(&reader, dcmFilePath) == false)
		{
			return false;
		}

		gdcm::Pixmap& pixmap = reader.GetPixmap();
		gdcm::File& file = reader.GetFile();

		/* 2. pixel 데이터를 Decompress */
		if (Decompress_GDCMPixmap(&pixmap) == false)
		{
			return false;
		}

		/* 3. Decompress 임시 Directory 생성 */
		QString fileName = QFileInfo(dcmFilePath).fileName();
		QString tempDecompressedDirectoryPath = QString::fromStdWString(FilePathManager::Instance()->TempDirPath());
		QString tempDecompressedFilePath = tempDecompressedDirectoryPath + "/" + fileName;
		QFileInfo tempDecompressedDirectoryInfo(tempDecompressedDirectoryPath);
		if (tempDecompressedDirectoryInfo.exists() == false)
		{
			QDir().mkpath(tempDecompressedDirectoryPath);
		}

		/* 4. Decompress된 데이터를 임시 데이터에 저장 */
		if (SaveDicomFile_GDCM(tempDecompressedFilePath, pixmap, file) == false)
		{
			return false;
		}

		/* 5. Dicom File Format 다시 Load */
		return LoadDcmFileFormat(pOutDcmFileFormat, tempDecompressedFilePath, pParam);
	}

	bool DicomDataset::LoadDicomFile_GDCM(gdcm::PixmapReader* outPixmapReader, const QString& dcmFilePath)
	{
		/*
			PixmapReader.SetFileName 함수 호출 시 Unicode가 정상처리되지 않음.
			따라서 ifstream으로 file Open 후 read 처리 진행
		*/
		std::ifstream inputStream;
		inputStream.open(dcmFilePath.toStdWString(), std::ios::binary);
		if (inputStream.is_open() == false)
		{
			qWarning() << "open stream file failed(=" << dcmFilePath << ")";
			return false;
		}
		outPixmapReader->SetStream(inputStream);
		if (!outPixmapReader->Read())
		{
			qWarning() << "gdcm read file failed(=" << dcmFilePath << ")";
			return false;
		}

		return true;
	}

	bool DicomDataset::Decompress_GDCMPixmap(gdcm::Pixmap* outPixmap)
	{
		gdcm::JPEG2000Codec j2kcodec;
		gdcm::JPEGCodec jpegcodec;
		gdcm::JPEGLSCodec jpeglscodec;

		gdcm::ImageChangeTransferSyntax imageChange;

		imageChange.SetInput(*outPixmap);

		const gdcm::TransferSyntax& ts = outPixmap->GetTransferSyntax();

		if (ts.IsExplicit())
		{
			imageChange.SetTransferSyntax(gdcm::TransferSyntax::ExplicitVRLittleEndian);
		}
		else
		{
			imageChange.SetTransferSyntax(gdcm::TransferSyntax::ImplicitVRLittleEndian);
		}

		if (imageChange.Change() == false)
		{
			qCritical() << "gdcm ImageChangeTransferSyntax fail to change image.";
			return false;
		}

		return true;
	}

	bool DicomDataset::SaveDicomFile_GDCM(const QString& dcmFilePath, const gdcm::Pixmap& pmap, const gdcm::File& file)
	{
		/*
			PixmapWriter.SetFileName() 함수 호출 시 Unicode 이슈 발생
			ofstream을 사용하여 처리해야함
		*/
		gdcm::PixmapWriter writer;
		std::ofstream fstream;
		fstream.open(dcmFilePath.toStdWString(), std::ios::out | std::ios::binary);
		if (fstream.is_open() == false)
		{
			qWarning() << "fail to load filepath";
			return false;
		}

		writer.SetStream(fstream);
		writer.SetFile(file);
		writer.SetImage(pmap);
		if (!writer.Write())
		{
			qCritical() << "fail to gdcm PixmapWriter write error.";
			return false;
		}

		return true;
	}

	DicomDataset::EType DicomDataset::getTypeAsBitData() const
	{
		EType type = EType::NotSupported;
		int bitAllocated = 0;
		if (TryGetValueInt(fm::DicomTagID::BitsAllocated, &bitAllocated) == false)
		{
			if (TryGetValueInt(fm::DicomTagID::BitsStored, &bitAllocated) == false)
			{
				bitAllocated = 16;
				qWarning() << "bits allocated, bits stored not exist. set to default value : " << bitAllocated;
			}
		}

		int samplePerPixel = 1;
		if (TryGetValueInt(fm::DicomTagID::SamplesPerPixel, &samplePerPixel) == false)
		{
			samplePerPixel = 1;
			qWarning() << "sample per pixel not exist. set to default value : " << samplePerPixel;
		}

		int bitPerPixel = bitAllocated * samplePerPixel;
		if (bitPerPixel == 8)
		{
			type = EType::Image_8bit;
		}
		else if (bitPerPixel == 16)
		{
			type = EType::Image_16bit;
		}
		else if (bitPerPixel == 24)
		{
			type = EType::Image_24bit;
		}
		else if (bitPerPixel == 32)
		{
			type = EType::Image_32bit;
		}
		else
		{
			type = EType::NotSupported;
		}

		return type;
	}

	DicomDataset::EType DicomDataset::getTypeAsBitDataSecondaryImage() const
	{
		EType type = EType::NotSupported;
		int bitAllocated = 0;
		if (TryGetValueInt(fm::DicomTagID::BitsAllocated, &bitAllocated) == false)
		{
			if (TryGetValueInt(fm::DicomTagID::BitsStored, &bitAllocated) == false)
			{
				bitAllocated = 16;
				qWarning() << "bits allocated, bits stored not exist. set to default value : " << bitAllocated;
			}
		}

		int samplePerPixel = 1;
		if (TryGetValueInt(fm::DicomTagID::SamplesPerPixel, &samplePerPixel) == false)
		{
			samplePerPixel = 1;
			qWarning() << "sample per pixel not exist. set to default value : " << samplePerPixel;
		}

		int bitPerPixel = bitAllocated * samplePerPixel;
		if (bitPerPixel == 8)
		{
			type = EType::Image_SecondaryCapture_8bit;
		}
		else if (bitPerPixel == 16)
		{
			type = EType::Image_SecondaryCapture_16bit;
		}
		else if (bitPerPixel == 24)
		{
			type = EType::Image_SecondaryCapture_24bit;
		}
		else if (bitPerPixel == 32)
		{
			type = EType::Image_SecondaryCapture_32bit;
		}
		else
		{
			type = EType::NotSupported;
		}

		return type;
	}

	DICOM_HEADER_INFO DicomDataset::ToDcmHeaderInfo() const
	{
		DICOM_HEADER_INFO info;

		std::string studyDateText = StringUtil::WideToMulitiByte(GetValueWString(DicomTagID::StudyDate));
		std::string studyTimeText = StringUtil::WideToMulitiByte(GetValueWString(DicomTagID::StudyTime));
		DateTime::GetDateTime_TextYYYYMMDD_hhmmss(info.StudyDateTime, studyDateText, studyTimeText);

		std::string seriesDateText = StringUtil::WideToMulitiByte(GetValueWString(DicomTagID::SeriesDate));
		std::string seriesTimeText = StringUtil::WideToMulitiByte(GetValueWString(DicomTagID::SeriesTime));
		DateTime::GetDateTime_TextYYYYMMDD_hhmmss(info.SeriesDateTime, seriesDateText, seriesTimeText);

		std::string patientBirthDateString = StringUtil::WideToMulitiByte(GetValueWString(DicomTagID::PatientBirthDate));
		std::string patientBirthTimeString = "000000";
		DateTime::GetDateTime_TextYYYYMMDD_hhmmss(info.DateOfBirth, patientBirthDateString, patientBirthTimeString);

		info.StudyInstanceUID = GetValueWString(DicomTagID::StudyInstanceUID);
		info.SeriesInstanceUID = GetValueWString(DicomTagID::SeriesInstanceUID);

		info.PatientName = GetValueWString(DicomTagID::PatientName);
		info.PatientID = GetValueWString(DicomTagID::PatientID);
		info.PatientSex = GetValueWString(DicomTagID::PatientSex);
		info.PatientAge = GetValueWString(DicomTagID::PatientAge);

		info.Modality = GetValueWString(DicomTagID::Modality);
		/* DicomTagID::Modality Tag가 없을 경우 DicomTagID::ModalitiesInStudy를 반환한다. */
		if (info.Modality.empty())
		{
			info.Modality = GetValueWString(DicomTagID::ModalitiesInStudy);
		}
		info.StudyDescription = GetValueWString(DicomTagID::StudyDescription);
		info.SeriesDescription = GetValueWString(DicomTagID::SeriesDescription);
		info.AccessionNumber = GetValueWString(DicomTagID::AccessionNumber);
		info.ReferringPhysician = GetValueWString(DicomTagID::ReferringPhysicianName);
		info.PerformingPhysician = GetValueWString(DicomTagID::PerformingPhysicianName);
		info.RequestingPhysician = GetValueWString(DicomTagID::RequestingPhysician);
		info.ExamID = GetValueWString(DicomTagID::StudyID);
		info.SeriesNumber = _wtoi(GetValueWString(DicomTagID::SeriesNumber).c_str());

		info.NumberOfPatientRelatedStudies = _wtoi(GetValueWString(DicomTagID::NumberOfPatientRelatedStudies).c_str());
		info.NumberOfPatientRelatedSeries = _wtoi(GetValueWString(DicomTagID::NumberOfPatientRelatedSeries).c_str());
		info.NumberOfPatientRelatedInstances = _wtoi(GetValueWString(DicomTagID::NumberOfPatientRelatedInstances).c_str());
		info.NumberOfStudyRelatedSeries = _wtoi(GetValueWString(DicomTagID::NumberOfStudyRelatedSeries).c_str());
		info.NumberOfStudyRelatedInstances = _wtoi(GetValueWString(DicomTagID::NumberOfStudyRelatedInstances).c_str());
		info.NumberOfSeriesRelatedInstances = _wtoi(GetValueWString(DicomTagID::NumberOfSeriesRelatedInstances).c_str());
		info.InstanceNumber = _wtoi(GetValueWString(DicomTagID::InstanceNumber).c_str());

		info.Manufacturer = GetValueWString(DicomTagID::Manufacturer);
		info.ManufacturerModelName = GetValueWString(DicomTagID::ManufacturerModelName);
		info.InstitutionName = GetValueWString(DicomTagID::InstitutionName);

		info.ImagesInAcqusition = _wtoi(GetValueWString(DicomTagID::ImagesInAcquisition).c_str());

		info.RETIRED_AcquisitionsInSeries = _wtoi(GetValueWString(DicomTagID::RETIRED_AcquisitionsInSeries).c_str());
		info.RETIRED_ImagesInSeries = _wtoi(GetValueWString(DicomTagID::RETIRED_ImagesInSeries).c_str());
		info.RETIRED_AcquisitionsInStudy = _wtoi(GetValueWString(DicomTagID::RETIRED_AcquisitionsInStudy).c_str());
		info.RETIRED_ImagesInStudy = _wtoi(GetValueWString(DicomTagID::RETIRED_ImagesInStudy).c_str());

		return info;
	}

	DcmtkSeriesInfo DicomDataset::ToDcmtkSeriesInfo() const
	{
		DcmtkSeriesInfo info;

		info.strSeriesUID = GetValueString(DicomTagID::SeriesInstanceUID);           ///< globally unique series identifier
		info.strStudyUID = GetValueString(DicomTagID::StudyInstanceUID);            ///< globally unique study identifier
		info.strAcquisitionNum = GetValueString(DicomTagID::AcquisitionNumber);		// Acquisition Number
		info.patientsName_ = GetValueString(DicomTagID::PatientName);			///< patient name
		info.patientId_ = GetValueString(DicomTagID::PatientID);				///< patient identifier
		info.patientSize_ = GetValueString(DicomTagID::PatientSize);			///< patient height
		info.patientWeight_ = GetValueString(DicomTagID::PatientWeight);			///< patient weight
		info.studyDate_ = GetValueString(DicomTagID::StudyDate);				///< date the study was taken
		info.studyTime_ = GetValueString(DicomTagID::StudyTime);				///< time the study was tacken
		info.modality_ = GetValueString(DicomTagID::Modality);				///< e.g. CT, MR, US
		info.numImages_ = GetValueString(DicomTagID::NumberOfSeriesRelatedInstances);				///< number of images in the series
		info.description_ = GetValueString(DicomTagID::SeriesDescription);			///< SeriesDescription
		info.birthday_ = GetValueString(DicomTagID::PatientBirthDate);
		info.age_ = GetValueString(DicomTagID::PatientAge);
		info.kvp_ = GetValueString(DicomTagID::KVP);
		info.ma_ = GetValueString(DicomTagID::XRayTubeCurrent);
		info.sex_ = GetValueString(DicomTagID::PatientSex);
		info.sopuid_ = GetValueString(DicomTagID::SOPInstanceUID);

		info.sliceThickness = GetValueString(DicomTagID::SliceThickness);
		info.width = GetValueString(DicomTagID::Columns);
		info.height = GetValueString(DicomTagID::Rows);

		std::string imgPosX = GetValueString(DicomTagID::ImagePositionPatient, 0);
		std::string imgPosY = GetValueString(DicomTagID::ImagePositionPatient, 1);
		std::string imgPosZ = GetValueString(DicomTagID::ImagePositionPatient, 2);
		if (!imgPosX.empty() && !imgPosY.empty() && !imgPosZ.empty())
		{
			info.imagePosition = imgPosX + "/" + imgPosY + "/" + imgPosZ;
		}

		info.xySpacing = GetValueString(DicomTagID::ImagerPixelSpacing);
		info.zSpacing = GetValueString(DicomTagID::SpacingBetweenSlices);
		info.convolutionkernel = GetValueString(DicomTagID::ConvolutionKernel);
		info.manufacturerModel = GetValueString(DicomTagID::ManufacturerModelName);
		info.seriesDate = GetValueString(DicomTagID::SeriesDate);

		info.studyDescription = GetValueString(DicomTagID::StudyDescription);
		info.Manufacturer = GetValueString(DicomTagID::Manufacturer);

		info.units_ = GetValueString(DicomTagID::Units);
		info.radionuclideTotalDose_ = GetValueString(DicomTagID::RadionuclideTotalDose);
		info.radionuclideHalfLife = GetValueString(DicomTagID::RadionuclideHalfLife);
		info.radiopharmaceuticalStartTime_ = GetValueString(DicomTagID::RadiopharmaceuticalStartTime);
		info.acquisitionTime_ = GetValueString(DicomTagID::AcquisitionTime);

		info.frameOfReferenceUID_ = GetValueString(DicomTagID::FrameOfReferenceUID);

		info.examID = GetValueString(DicomTagID::StudyID);
		info.accessionNumber = GetValueString(DicomTagID::AccessionNumber);

		return info;
	}

	DicomVolumeInfo DicomDataset::ToDicomVolumeInfo(int sliceCount, float sliceThickness) const
	{
		DicomVolumeInfo info;

		info.dx = GetValueInt(DicomTagID::Columns);
		info.dy = GetValueInt(DicomTagID::Rows);
		info.dz = sliceCount;

		if (TryGetValueFloat(DicomTagID::PixelSpacing, &info.x_spacing, 0) == false)
		{
			if (TryGetValueFloat(DicomTagID::ImagerPixelSpacing, &info.x_spacing, 0) == false)
			{
				qWarning() << "x pixel spacing tag is empty.";
			}
		}

		if (TryGetValueFloat(DicomTagID::PixelSpacing, &info.y_spacing, 1) == false)
		{
			if (TryGetValueFloat(DicomTagID::ImagerPixelSpacing, &info.y_spacing, 1) == false)
			{
				qWarning() << "y pixel spacing tag is empty.";
			}
		}

		if (sliceThickness > 0.f)
		{
			info.z_spacing = sliceThickness;
		}
		else
		{
			/* Spacing Between Slices를 우선순위로 둔다. */
			info.z_spacing = GetValueFloat(DicomTagID::SpacingBetweenSlices);
			if (info.z_spacing == 0.f)
			{
				info.z_spacing = GetValueFloat(DicomTagID::SliceThickness);
			}
			if (info.z_spacing == 0.f)
			{
				qWarning() << "[DicomDataset::ToDicomVolumeInfo] z_spacing is empty. set default '1.0f'.";
				info.z_spacing = 1.f;
			}
		}

		/* TODO : Image Position을 사용하여 z_spacing을 다시 계산한다. */

		info.scaled_dx = 0;
		info.scaled_dy = 0;
		info.scaled_dz = 0;

		info.scaled_x_spacing = 0;
		info.scaled_y_spacing = 0;
		info.scaled_z_spacing = 0;

		info.bitsStored = GetValueInt(DicomTagID::BitsStored);
		info.rescaleIntercept = (int)GetValueFloat(DicomTagID::RescaleIntercept);
		info.rescaleSlose = GetValueFloat(DicomTagID::RescaleSlope);
		info.samplesPerPixel = GetValueInt(DicomTagID::SamplesPerPixel);
		info.pixelRepresentation = GetValueInt(DicomTagID::PixelRepresentation);

		int bit_size = info.bitsStored * info.samplesPerPixel;
		if (bit_size <= 8)
		{
			info.bytesPerVoxel = 1;
		}
		else if (info.bitsStored * info.samplesPerPixel <= 16)
		{
			info.bytesPerVoxel = 2;
		}
		else if (info.bitsStored * info.samplesPerPixel <= 32)
		{
			info.bytesPerVoxel = 3;
		}
		else
		{
			info.bytesPerVoxel = 4;
		}

		//redmine #1069 http://www.medicalip.net:30002/projects/medip-bug/issues
		/*
			[AA\BB\CC]으로 저장될 수 있음
			AA->BB->CC 순으로 값을 읽어올 수 있도록 할 것
		*/
		// [AA\BB] => AA 값으로 적용&
		float windowingWidth = 0.f;
		float windowingLevel = 0.f;
		TryGetWindowingWidth(&windowingWidth);
		TryGetWindowingLevel(&windowingLevel);

		info.window_width = windowingWidth;
		info.window_level = windowingLevel;

		info.instance_number = GetValueInt(DicomTagID::InstanceNumber);

		info.xAxis = mip::VECTOR3(1, 0, 0);
		std::string xAxisPosX = GetValueString(DicomTagID::ImageOrientationPatient, 0);
		std::string xAxisPosY = GetValueString(DicomTagID::ImageOrientationPatient, 1);
		std::string xAxisPosZ = GetValueString(DicomTagID::ImageOrientationPatient, 2);
		if (!xAxisPosX.empty() && !xAxisPosY.empty() && !xAxisPosZ.empty())
		{
			info.xAxis.x = atof(xAxisPosX.c_str());
			info.xAxis.y = atof(xAxisPosY.c_str());
			info.xAxis.z = atof(xAxisPosZ.c_str());
		}

		info.yAxis = mip::VECTOR3(0, 1, 0);
		std::string yAxisPosX = GetValueString(DicomTagID::ImageOrientationPatient, 3);
		std::string yAxisPosY = GetValueString(DicomTagID::ImageOrientationPatient, 4);
		std::string yAxisPosZ = GetValueString(DicomTagID::ImageOrientationPatient, 5);

		if (!yAxisPosX.empty() && !yAxisPosY.empty() && !yAxisPosZ.empty())
		{
			info.yAxis.x = atof(yAxisPosX.c_str());
			info.yAxis.y = atof(yAxisPosY.c_str());
			info.yAxis.z = atof(yAxisPosZ.c_str());
		}

		info.normal = info.xAxis ^ info.yAxis;

		std::string imgPosX = GetValueString(DicomTagID::ImagePositionPatient, 0);
		std::string imgPosY = GetValueString(DicomTagID::ImagePositionPatient, 1);
		std::string imgPosZ = GetValueString(DicomTagID::ImagePositionPatient, 2);
		if (!imgPosX.empty() && !imgPosY.empty() && !imgPosZ.empty())
		{
			info.offset.x = atof(imgPosX.c_str());
			info.offset.y = atof(imgPosY.c_str());
			info.offset.z = atof(imgPosZ.c_str());
		}

		info.modality_mode = GetDcmModalityType();

		return info;
	}

	DCM_MODAL_TYPE DicomDataset::GetDcmModalityType() const
	{
		DCM_MODAL_TYPE type = DCM_CT;
		std::string mod = GetValueString(DicomTagID::Modality);
		if (!mod.compare("CT"))
			type = DCM_CT;
		else if (!mod.compare("UK"))
			type = DCM_UK;
		else if (!mod.compare("MR"))
			type = DCM_MR;
		else if (!mod.compare("RF"))
			type = DCM_RF;
		else if (!mod.compare("US"))
			type = DCM_US;
		else if (!mod.compare("XA"))
			type = DCM_XA;
		else if (!mod.compare("XRAY"))
			type = DCM_XRAY;
		else if (!mod.compare("MCT"))
			type = DCM_MCT;
		else if (!mod.compare("PT"))
			type = DCM_PT;
		else
			type = DCM_CT;

		return type;
	}

	bool DicomDataset::TryGetWindowingWidth(float* pOutValue) const
	{
		// [AA\BB] => AA 값으로 적용
		if (TryGetValueFloat(DicomTagID::WindowWidth, pOutValue, 0) == false)
		{
			// [AA\BB] => BB 값으로 적용
			if (TryGetValueFloat(DicomTagID::WindowWidth, pOutValue, 1) == false)
			{
				return false;
			}
		}
		return true;
	}

	bool DicomDataset::TryGetWindowingLevel(float* pOutValue) const
	{
		// [AA\BB] => AA 값으로 적용
		if (TryGetValueFloat(DicomTagID::WindowCenter, pOutValue, 0) == false)
		{
			// [AA\BB] => BB 값으로 적용
			if (TryGetValueFloat(DicomTagID::WindowCenter, pOutValue, 1) == false)
			{
				return false;
			}
		}
		return true;
	}

	DicomDataset::EType DicomDataset::GetDicomType() const
	{
		QString SOPClassUID = QString::fromStdWString(GetValueWString(fm::DicomTagID::SOPClassUID));
		EType type = EType::NotSupported;
		if (SOPClassUID == UID_EncapsulatedPDFStorage)
		{
			type = EType::EncapsulatedPDFStorage;
		}
		else if (SOPClassUID == UID_SecondaryCaptureImageStorage)
		{
			type = getTypeAsBitDataSecondaryImage();
		}
		else
		{
			if (HasImageData())
			{
				type = getTypeAsBitData();
			}
			else
			{
				type = EType::NotSupported;
			}
		}
		return type;
	}

}
