#include "stdafx.h"
#include "FileManagerImportData.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "filemanager/dicom/Image/DicomDataImage_sint16.h"
#include "filemanager/dicom/DicomType.h"
#include "filemanager/data/Image/VolumeUtility.h"

#define UPDATE_VOLUMEDATA_START_RATE 0.0f
//#define UPDATE_VOLUMEDATA_MAX_RATE 0.1f
#define UPDATE_VOLUMEDATA_MAX_RATE 1.0f
#define APPLY_OFFSET_START_RATE 0.1f
#define APPLY_OFFSET_MAX_RATE 0.9f

namespace fm
{
	FileManagerImportData::FileManagerImportData() :
		m_pVolumeDataPtr(nullptr),
		m_importDataType(EFileManagerImportDataType::mip_project),
		m_isAbortProgress(false)
	{
	}

	FileManagerImportData::~FileManagerImportData()
	{
		ClearDicom();
	}

	FileManagerImportData::FileManagerImportData(const FileManagerImportData& rhs)
	{
		Init(rhs);
	}

	FileManagerImportData& FileManagerImportData::operator=(const FileManagerImportData& rhs)
	{
		Init(rhs);
		return *this;
	}

	void FileManagerImportData::SetImportDataType(EFileManagerImportDataType type)
	{
		m_importDataType = type;
	}

	EFileManagerImportDataType FileManagerImportData::GetImportDataType()
	{
		return m_importDataType;
	}

	void FileManagerImportData::SetMIPProjectFilePath(QString value)
	{
		m_mipProjectFilePath = value;
	}

	QString FileManagerImportData::GetMIPProjectFilePath()
	{
		return m_mipProjectFilePath;
	}

	bool FileManagerImportData::SetDicomData(DicomInfomationModelSeriesObject* pSeries)
	{
		m_isAbortProgress = false;
		emit setDicomDataStarted();
		DicomDataset* pDataset;
		//int index = (pSeries->ImageInstanceCount() - 1) / 2;

		//Z축 기준 Top에 위치한 ImagePosition을 읽어 온다.
		pSeries->SortImageList();
		int index = 0;
		if (pSeries->Get(&pDataset, index) == false)
		{
			qWarning() << "fail to get image from series(index=" << index << ")";
			emit setDicomDataFinished();
			return false;
		}

		/* Dicom 파일 경로 추가 */
		m_dcmFilePath = QString::fromStdWString(pDataset->GetFilePath_Extension());

		//DicomDataset::EType type = pDataset->GetDicomType();
		DicomType dcmType(pDataset);

		if (dcmType.IsCompatibleImageToMEDIP() == false)
		{
			qWarning() << "not supported dicom type...";
			emit setDicomDataFinished();
			return false;
		}

		/* Series -> Volume Info */
		float sliceThickness = 0;
		pSeries->GetSliceThicknessBetweenTwoImage(sliceThickness);
		int imageCount = pSeries->ImageInstanceCount();
		SetupDicomVolumeInfo(pDataset, imageCount, sliceThickness);

		/* Series -> Series Info */
		SetupDicomSeriesInfo(pDataset);

		/* Series -> Volume Ptr 데이터 */
		std::vector<DicomDataset*> dcmDatasetList = pSeries->GetDatasetList();
		if (UpdateVolumeData_ReverseOrder(dcmDatasetList) == false)
		{
			qWarning() << "fail to update volume data reverse order";
			emit setDicomDataFinished();
			return false;
		}

		/* HU Offset 정보 Apply*/
		//ApplyHUOffsetToDcmVolumeData();

		emit setDicomDataFinished();

		return true;
	}

	QFuture<bool> FileManagerImportData::SetDicomData_Async(DicomInfomationModelSeriesObject* pSeries)
	{
		QFuture<bool> future = QtConcurrent::run([=]() {
			return SetDicomData(pSeries);
			});
		return future;
	}

	void FileManagerImportData::SetupDicomVolumeInfo(DicomDataset* pDataset, int imageCount, float sliceThickness)
	{
		m_dcmVolumeInfo = pDataset->ToDicomVolumeInfo(imageCount, sliceThickness);
	}

	void FileManagerImportData::SetupDicomSeriesInfo(DicomDataset* pDataset)
	{
		m_dcmSeriesInfo = pDataset->ToDcmtkSeriesInfo();
	}

	bool FileManagerImportData::UpdateVolumeData_ReverseOrder(std::vector<DicomDataset*> dcmDatasetList)
	{
		InitDicomVolumeData();
		int count = dcmDatasetList.size();

		/* 반대 방향 순으로 copy 진행 */
		for (int i = 0; i < count; ++i)
		{
			if (m_isAbortProgress)
			{
				qDebug() << "UpdateVolumeData_ReverseOrder() aborted. ";
				return false;
			}

			DicomDataImage_sint16 dcmImage;
			if (dcmImage.Init(dcmDatasetList[i]) == false)
			{
				qWarning() << "fail to init series. pixel data is nullptr";
				ClearDicom();
				return false;
			}

			int destPtrIdx = (count - 1 - i) * dcmImage.Length();
			int imageSize = dcmImage.Size();
			memcpy((mint16*)m_pVolumeDataPtr + destPtrIdx, dcmImage.Ptr(), imageSize);

			float rate = (float)i / (float)count;
			UpdateProgressRate(rate, UPDATE_VOLUMEDATA_START_RATE, UPDATE_VOLUMEDATA_MAX_RATE);
		}
		return true;
	}

	void FileManagerImportData::UpdateProgressRate(float progressRate, float startRate, float maxRate)
	{
		float resultRate = startRate + (progressRate * maxRate);
		emit setDicomDataInProgress(resultRate);
	}

	bool FileManagerImportData::ApplyHUOffsetToDcmVolumeData()
	{
		auto callback = [this] (float rate) {
			UpdateProgressRate(rate, APPLY_OFFSET_START_RATE, APPLY_OFFSET_MAX_RATE);
		};
		return ApplyHUOffsetToVolume(
			m_pVolumeDataPtr,
			m_dcmVolumeInfo.dx,
			m_dcmVolumeInfo.dy,
			m_dcmVolumeInfo.dz,
			m_dcmVolumeInfo.modality_mode,
			m_dcmVolumeInfo.pixelRepresentation,
			m_dcmVolumeInfo.rescaleIntercept,
			m_dcmVolumeInfo.rescaleSlose,
			m_dcmVolumeInfo.bitsStored,
			&m_isAbortProgress,
			callback
		);
	}

	void FileManagerImportData::ClearDicom()
	{
		if (m_pVolumeDataPtr)
		{
			delete m_pVolumeDataPtr;
			m_pVolumeDataPtr = nullptr;
		}
		m_dcmSeriesInfo = DcmtkSeriesInfo();
		m_dcmVolumeInfo = DicomVolumeInfo();
	}

	void FileManagerImportData::Abort()
	{
		qDebug() << "abort set dicom data";
		m_isAbortProgress = true;
	}

	DcmtkSeriesInfo* fm::FileManagerImportData::GetDcmSeriesInfo()
	{
		return &m_dcmSeriesInfo;
	}

	DicomVolumeInfo* fm::FileManagerImportData::GetDcmVolumeInfo()
	{
		return &m_dcmVolumeInfo;
	}

	mint16* fm::FileManagerImportData::Data()
	{
		return (mint16*)m_pVolumeDataPtr;
	}

	QString FileManagerImportData::GetDcmFilePath()
	{
		return m_dcmFilePath;
	}

	void FileManagerImportData::SetImportFilePathList(QStringList pathList)
	{
		m_importFilePathList = pathList;
	}

	QStringList FileManagerImportData::GetImportFilePathList()
	{
		return m_importFilePathList;
	}

	void fm::FileManagerImportData::Init(const FileManagerImportData& rhs)
	{
		m_importDataType = rhs.m_importDataType;

		/* MIP */
		m_mipProjectFilePath = rhs.m_mipProjectFilePath;

		/* DICOM */
		m_dcmSeriesInfo = rhs.m_dcmSeriesInfo;
		m_dcmVolumeInfo = rhs.m_dcmVolumeInfo;
		m_dcmFilePath = rhs.m_dcmFilePath;

		if (InitDicomVolumeData(rhs.m_pVolumeDataPtr) == false)
		{
			ClearDicom();
		}

		/* IMPORT */
		m_importFilePathList = rhs.m_importFilePathList;
	}

	bool FileManagerImportData::InitDicomVolumeData(void* pData)
	{
		int cx = m_dcmVolumeInfo.dx;
		int cy = m_dcmVolumeInfo.dy;
		int cz = m_dcmVolumeInfo.dz;
		int length = cx * cy * cz;
		int size = length * sizeof(mint16);
		if (length <= 0)
		{
			return false;
		}
		m_pVolumeDataPtr = new mint16[length];
		if (pData)
		{
			memcpy(m_pVolumeDataPtr, pData, size);
		}
		return true;
	}
}
