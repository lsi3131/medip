#include "stdafx.h"
#include "DicomConverter.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/DicomDatasetIOParameter.h"
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
	DicomConverter::BmpToDcm::BmpToDcm()
	{
		m_inputPlug = new I2DBmpSource();
		m_outPlug = new I2DOutputPlugSC();
	}

	DicomConverter::BmpToDcm::~BmpToDcm()
	{
		delete m_inputPlug;
		delete m_outPlug;
	}

	bool DicomConverter::BmpToDcm::ConvertToDataset(std::wstring srcBmpFilePath, DicomDataset* pOutDataset, DicomDataset* pAppendedDataset)
	{
		Image2Dcm imgToDcmObj;
		E_TransferSyntax writeXfer = EXS_Unknown;

		OFList<OFString> overrideKeys;
		OFBool insertLatin1 = OFTrue;
		OFBool insertType2 = OFTrue;
		OFBool inventType1 = OFTrue;
		OFBool doChecks = OFTrue;

		imgToDcmObj.setOverrideKeys(overrideKeys);
		imgToDcmObj.setISOLatin1(insertLatin1);
		imgToDcmObj.setValidityChecking(doChecks, insertType2, inventType1);

		m_inputPlug->setImageFile(StringUtil::WideStringToOFString(srcBmpFilePath));

		DcmDataset* pResultObject = NULL;
		OFCondition cond = imgToDcmObj.convert(m_inputPlug, m_outPlug, pResultObject, writeXfer);
		bool result = false;

		if (cond.bad())
		{
			qWarning() << "fail to convert input plugin to output plugin. result : " << cond.text();
			result = false;
		}
		else
		{
			*pOutDataset = DicomDataset(pResultObject);

			/* Default °ª Àû¿ë */
			pOutDataset->SetTagValue(fm::DicomTagID::SeriesDescription, L"MEDIP Image Data");
			pOutDataset->SetTagValue(fm::DicomTagID::Modality, L"SC");

			if (pAppendedDataset)
			{
				pOutDataset->AppendDataset(pAppendedDataset);
			}

			result = true;
		}

		return result;
	}

	bool DicomConverter::BmpToDcm::ConvertToFile(std::wstring srcBmpFilePath, std::wstring targetDcmFilePath, DicomDataset* pAppendedDataset)
	{
		Image2Dcm imgToDcmObj;
		E_TransferSyntax writeXfer = EXS_Unknown;

		OFList<OFString> overrideKeys;
		OFBool insertLatin1 = OFTrue;
		OFBool insertType2 = OFTrue;
		OFBool inventType1 = OFTrue;
		OFBool doChecks = OFTrue;

		imgToDcmObj.setOverrideKeys(overrideKeys);
		imgToDcmObj.setISOLatin1(insertLatin1);
		imgToDcmObj.setValidityChecking(doChecks, insertType2, inventType1);

		m_inputPlug->setImageFile(StringUtil::WideStringToOFString(srcBmpFilePath));

		DcmDataset* pResultObject = NULL;
		OFCondition cond = imgToDcmObj.convert(m_inputPlug, m_outPlug, pResultObject, writeXfer);
		bool result = false;

		if (cond.bad())
		{
			qWarning() << "fail to convert input plugin to output plugin. result : " << cond.text();
			result = false;
		}
		else
		{
			DicomDataset dcmDatasetResult(pResultObject);
			if (pAppendedDataset)
			{
				dcmDatasetResult.AppendDataset(pAppendedDataset);
			}

			DicomDatasetSaveParameter param;
			param.WriteMode = EWM_fileformat;
			param.WriteXfer = writeXfer;
			param.EncodingType = EET_ExplicitLength;

			if (dcmDatasetResult.SaveToFile(QString::fromStdWString(targetDcmFilePath), &param) == false)
			{
				qWarning() << "fail to Saving output DICOM to file : " << srcBmpFilePath.c_str();
				result = false;
			}
			result = true;
		}

		return result;
	}
}