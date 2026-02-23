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
	DicomConverter::DcmToPDF::DcmToPDF()
	{
	}

	DicomConverter::DcmToPDF::~DcmToPDF()
	{
	}

	bool DicomConverter::DcmToPDF::ConvertToFile(std::wstring srcDcmFilePath, std::wstring targetPDFFilePath)
	{
		if (srcDcmFilePath.empty())
		{
			qWarning() << "invalid filename: <empty string>";
			return false;
		}

		DicomDataset dcmDataset;
		if (dcmDataset.LoadFromFile(QString::fromStdWString(srcDcmFilePath)) == false)
		{
			qWarning() << "failed to load pdf file: " << QString::fromStdWString(srcDcmFilePath);
			return false;
		}

		return ConvertToFile(dcmDataset, targetPDFFilePath);
	}
	bool DicomConverter::DcmToPDF::ConvertToFile(DicomDataset& srcDcmDataset, std::wstring targetPDFFilePath)
	{
		if (!dcmDataDict.isDictionaryLoaded())
		{
			qWarning() << "no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE;
			return false;
		}

		/* Copy를 통해 원본 손실 방지 */
		DicomDataset targetDcmDataset = srcDcmDataset;
		DcmDataset* dataset = targetDcmDataset.Data();

		OFString sopClass;
		OFCondition cond = dataset->findAndGetOFString(DCM_SOPClassUID, sopClass);
		if (cond.bad() || sopClass != UID_EncapsulatedPDFStorage)
		{
			qWarning() << "not an Encapsulated PDF Storage object";
			return false;
		}

		DcmElement* delem = NULL;
		cond = dataset->findAndGetElement(DCM_EncapsulatedDocument, delem);
		if (cond.bad() || delem == NULL)
		{
			qWarning() << "attribute (0042,0011) Encapsulated Document missing.";
			return false;
		}

		Uint32 len = delem->getLength();
		Uint8* pdfDocument = NULL;
		cond = delem->getUint8Array(pdfDocument);
		if (cond.bad() || pdfDocument == NULL || len == 0)
		{
			qWarning() << "attribute (0042,0011) Encapsulated Document empty or wrong VR.";
			return false;
		}

		/* strip pad byte at end of file, if there is one. The PDF format expects
		* files to end with %%EOF followed by CR/LF (although in some cases the
		* CR/LF may be missing or you might only find CR or LF).
		* If the last character of the file is not a CR or LF, and not the
		* letter 'F', we assume it is either trailing garbage or a pad byte, and remove it.
		*/
		if (pdfDocument[len - 1] != 10 && pdfDocument[len - 1] != 13 && pdfDocument[len - 1] != 'F')
		{
			--len;
		}

		//= fopen(targetPDFFilePath.c_str(), "wb");
		FILE* pdffile = NULL;
		errno_t errFile = _wfopen_s(&pdffile, targetPDFFilePath.c_str(), L"wb");

		if (pdffile == NULL)
		{
			qWarning() << "unable to create file " << targetPDFFilePath.c_str();
			return false;
		}

		if (len != fwrite(pdfDocument, 1, len, pdffile))
		{
			qWarning() << "write error in file " << targetPDFFilePath.c_str();
			fclose(pdffile);
			return false;
		}

		fclose(pdffile);

		qWarning() << "conversion successful";
		return true;
	}
}
