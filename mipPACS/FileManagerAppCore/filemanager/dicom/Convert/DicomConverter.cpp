#include "stdafx.h"
#include "DicomConverter.h"
#include "filemanager/dicom/DicomDataset.h"
#include <dcmtk/dcmdata/dcencdoc.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/cmdlnarg.h>
#include <dcmtk/dcmdata/libi2d/i2dbmps.h>
#include <dcmtk/dcmdata/libi2d/i2d.h>
#include <dcmtk/dcmdata/libi2d/i2dplsc.h>
#include <dcmtk/ofstd/ofconapp.h>
#include <dcmtk/ofstd/ofexit.h>

namespace fm
{
	bool DicomConverter::ConvertPDFFile_To_DcmFile(std::wstring srcPDFFilePath, std::wstring outDcmFilePath, DicomDataset* pAppendedDataset)
	{
		PDFToDcm converter;
		return converter.ConvertToFile(srcPDFFilePath, outDcmFilePath, pAppendedDataset);
	}

	bool DicomConverter::ConvertPDFFile_To_DcmDataset(std::wstring srcPDFFilePath, DicomDataset* pOutDataset, DicomDataset* pAppendedDataset)
	{
		PDFToDcm converter;
		return converter.ConvertToDataset(srcPDFFilePath, pOutDataset, pAppendedDataset);
	}

	bool DicomConverter::ConvertBmpFile_To_DcmFile(std::wstring srcBmpFilePath, std::wstring outDcmFilePath, DicomDataset* pAppendedDataset)
	{
		BmpToDcm converter;
		return converter.ConvertToFile(srcBmpFilePath, outDcmFilePath, pAppendedDataset);
	}

	bool DicomConverter::ConvertBmpFile_To_DcmDataset(std::wstring srcBmpFilePath, DicomDataset* pOutDataset, DicomDataset* pAppendedDataset)
	{
		BmpToDcm converter;
		return converter.ConvertToDataset(srcBmpFilePath, pOutDataset, pAppendedDataset);
	}

	bool DicomConverter::ConvertDcmFile_To_PDFFile(std::wstring srcDcmFilePath, std::wstring targetPDFFilePath)
	{
		DcmToPDF converter;
		return converter.ConvertToFile(srcDcmFilePath, targetPDFFilePath);
	}

	bool DicomConverter::ConvertDcmDataset_To_PDFFile(DicomDataset& dcmDataset, std::wstring targetPDFFilePath)
	{
		DcmToPDF converter;
		return converter.ConvertToFile(dcmDataset, targetPDFFilePath);
	}

	bool DicomConverter::ConvertDcmFile_To_ImgFile(std::wstring srcDcmFilePath, std::wstring targetImgFilePath)
	{
		DcmToImage converter;
		return converter.ConvertToFile(srcDcmFilePath, targetImgFilePath);
	}

	bool DicomConverter::ConvertDcmFile_To_QImage(std::wstring srcDcmFilePath, QImage& image)
	{
		DcmToImage converter;
		return converter.ConvertToImage(srcDcmFilePath, image);
	}

	bool DicomConverter::ConvertDcmDataset_To_QImage(DicomDataset& dcmDataset, QImage& image)
	{
		DcmToImage converter;
		return converter.ConvertToImage(dcmDataset, image);
	}

}


