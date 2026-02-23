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
	static OFLogger dummyLogger = OFLog::getLogger("dummy logger");

	bool DicomConverter::PDFToDcm::ConvertToDataset(std::wstring srcPDFFilePath, DicomDataset* pOutDataset, DicomDataset* pAppendedDataset)
	{
		int errorCode = EXITCODE_NO_ERROR;

		if (!dcmDataDict.isDictionaryLoaded())
		{
			qWarning() << "no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE;
		}

		m_pEncapsulator->setInputFileName(StringUtil::WideStringToOFString(srcPDFFilePath));

		OFCondition cond = m_pEncapsulator->createIdentifiers(dummyLogger);

		errorCode = m_pEncapsulator->insertEncapsulatedDocument(pOutDataset->Data(), dummyLogger);

		if (errorCode != EXITCODE_NO_ERROR)
		{
			qWarning() << "unable to create PDF encapsulation to DICOM format";
			return false;
		}

		cond = m_pEncapsulator->createHeader(pOutDataset->Data(), dummyLogger);
		if (cond.bad())
		{
			qWarning() << "unable to create DICOM header: " << cond.text();
			return false;
		}

		DcmXfer transferSyntax(m_pEncapsulator->getTransferSyntax());
		pOutDataset->Data()->chooseRepresentation(m_pEncapsulator->getTransferSyntax(), NULL);
		if (pOutDataset->Data()->canWriteXfer(m_pEncapsulator->getTransferSyntax()) == false)
		{
			qWarning() << "No conversion to transfer syntax " << transferSyntax.getXferName() << " possible!";
			return false;
		}

		cond = m_pEncapsulator->applyOverrideKeys(pOutDataset->Data());
		if (cond.bad())
		{
			qWarning() << "There was a problem while overriding a key :" << cond.text();
			return false;
		}

		/* TODO : 외부에서 적용할지 여부 확인. */
		pOutDataset->SetTagValue(fm::DicomTagID::SeriesDescription, L"MEDIP PDF Data");

		if (pAppendedDataset)
		{
			pOutDataset->AppendDataset(pAppendedDataset);
		}

		return true;
	}

	bool DicomConverter::PDFToDcm::ConvertToFile(std::wstring srcPDFFilePath, std::wstring targetDcmFilePath, DicomDataset* pAppendedDataset)
	{
		DicomDataset dataset;
		m_pEncapsulator->setOutputFileName(StringUtil::WideStringToOFString(targetDcmFilePath));

		if (ConvertToDataset(srcPDFFilePath, &dataset, pAppendedDataset) == false)
		{
			return false;
		}

		DcmFileFormat dcmff(dataset.Data());
		OFCondition cond = m_pEncapsulator->saveFile(dcmff);
		if (cond.bad())
		{
			qWarning() << cond.text() << ": writing file: " << m_pEncapsulator->getOutputFileName().data();
			return false;
		}

		return true;
	}

	DicomConverter::PDFToDcm::PDFToDcm()
	{
		m_pEncapsulator = new DcmEncapsulatedDocument();
		m_pEncapsulator->setFileType("pdf");
	}

	DicomConverter::PDFToDcm::~PDFToDcm()
	{
		delete m_pEncapsulator;
	}
}
