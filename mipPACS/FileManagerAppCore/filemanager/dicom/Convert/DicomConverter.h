#pragma once

#include <string>
#include "filemanager/export.h"

class DcmEncapsulatedDocument;
class DicomImage;
class I2DImgSource;
class I2DOutputPlug;
class QImage;

namespace fm
{
	class DicomDataset;

	class FM_CORE_EXPORT DicomConverter
	{
	public:
		static bool ConvertPDFFile_To_DcmFile(std::wstring srcPDFFilePath, std::wstring targetDcmFilePath, DicomDataset* pAppendedDataset = nullptr);
		static bool ConvertPDFFile_To_DcmDataset(std::wstring srcPDFFilePath, DicomDataset* pOutDataset, DicomDataset* pAppendedDataset = nullptr);

		static bool ConvertBmpFile_To_DcmFile(std::wstring srcBmpFilePath, std::wstring targetDcmFilePath, DicomDataset* pAppendedDataset = nullptr);
		static bool ConvertBmpFile_To_DcmDataset(std::wstring srcBmpFilePath, DicomDataset* pOutDataset, DicomDataset* pAppendedDataset = nullptr);

		static bool ConvertDcmFile_To_PDFFile(std::wstring srcDcmFilePath, std::wstring targetPDFFilePath);
		static bool ConvertDcmDataset_To_PDFFile(DicomDataset& dcmDataset, std::wstring targetPDFFilePath);

		static bool ConvertDcmFile_To_ImgFile(std::wstring srcDcmFilePath, std::wstring targetImgFilePath);
		static bool ConvertDcmFile_To_QImage(std::wstring srcDcmFilePath, QImage& image);
		static bool ConvertDcmDataset_To_QImage(DicomDataset& dcmDataset, QImage& image);

	private:
		//==============================================
		//			PDFToDcm
		//==============================================
		class PDFToDcm
		{
		public:
			PDFToDcm();
			~PDFToDcm();
		public:
			bool ConvertToDataset(std::wstring srcPDFFilePath, DicomDataset* pOutDataset, DicomDataset* pAppendedDataset);
			bool ConvertToFile(std::wstring srcPDFFilePath, std::wstring targetDcmFilePath, DicomDataset* pAppendedDataset);

		private:
			DcmEncapsulatedDocument* m_pEncapsulator;
		};

		//==============================================
		//			BmpToDcm
		//==============================================
		class BmpToDcm
		{
		public:
			BmpToDcm();
			~BmpToDcm();
		public:
			bool ConvertToDataset(std::wstring srcPDFFilePath, DicomDataset* pOutDataset, DicomDataset* pAppendedDataset);
			bool ConvertToFile(std::wstring srcBmpFilePath, std::wstring targetPDFFilePath, DicomDataset* pAppendedDataset);

		private:
			I2DImgSource* m_inputPlug;
			I2DOutputPlug* m_outPlug;
		};

		//==============================================
		//			DcmToPDF
		//==============================================
		class DcmToPDF
		{
		public:
			DcmToPDF();
			~DcmToPDF();

		public:
			bool ConvertToFile(std::wstring srcDcmFilePath, std::wstring targetPDFFilePath);
			bool ConvertToFile(DicomDataset& dcmDataset, std::wstring targetPDFFilePath);
		};

		//==============================================
		//			DcmToImage
		//==============================================
		class DcmToImage
		{
		public:
			DcmToImage();
			~DcmToImage();

		public:
			bool ConvertToFile(std::wstring srcDcmFilePath, std::wstring targetImgFilePath);
			bool ConvertToImage(DicomDataset& dcmDataset, QImage& image);
			bool ConvertToImage(std::wstring srcDcmFilePath, QImage& image);

		private:
			void RegisterCodec();
			void CleanupCodec();
			bool InitDicomImage(DicomDataset& dcmDataset);
			bool WriteDicomImageToTempBMP();
			bool ConvertTempBmpToImage(QImage& image);

		private:
			QString m_tempBmpImage;
			DicomImage* m_dcmImage;
		};
	};
}
