#include "stdafx.h"
#include "ActionCOVIDReport.h"
#include "algorithm/Radiomics.h"
#include "Windows/windowManager.h"

ActionCOVIDReport::ActionCOVIDReport()
{

}

ActionCOVIDReport::~ActionCOVIDReport()
{

}

WorkCOVIDReport::WorkCOVIDReport(VOLUME_DATA * volume, COVID_REPORT* pOutReport)
{	
	m_pVolumeData = volume;
	m_pOutReport = pOutReport;
}

void WorkCOVIDReport::setProgressValue(int value, bool init /*= false*/)
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

void WorkCOVIDReport::threadRun()
{
	int nMaskCnt = m_pVolumeData->getMaskInfoListCnt();
	//int nMaskCnt = 0;

	// calculate feature	

	///////////////////////// lung
	COVID_TEXTURE_FEATURE feature = COVID_TEXTURE_FEATURE();

	MaskInfo* pMaskInfo_1 = nullptr;
	MaskInfo* pMaskInfo_2 = nullptr;

	for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
	{
		MaskInfo* pTmpInfo = m_pVolumeData->getMaskInfo(i, false);
		if (!wcscmp(pTmpInfo->maskName, L"Lung"))
			pMaskInfo_1 = pTmpInfo;

		if (!wcscmp(pTmpInfo->maskName, L"Pneumonia"))
			pMaskInfo_2 = pTmpInfo;
	}

	if (pMaskInfo_1 == nullptr || pMaskInfo_2 == nullptr)
	{
		m_pVolumeData->threadStop = true;
		emit finished();
		return;
	}
	
	int nMaskUID_1 = pMaskInfo_1->uid;
	int nMaskByteIdx_1 = m_pVolumeData->GetMaskByteIndex(nMaskUID_1);

	std::wstring ws(pMaskInfo_1->maskName);
	feature.strROIName = std::string(ws.begin(), ws.end());
	feature.fObjectVolume = getVoxelVolumeFeatureValue(m_pVolumeData->getMaskDataPoint(nMaskByteIdx_1)
		, m_pVolumeData->getHUDataPoint(), m_pVolumeData->getCX(), m_pVolumeData->getCY()
		, m_pVolumeData->getCZ(), m_pVolumeData->getMask(nMaskUID_1)
		, m_pVolumeData->getSpaceX(false), m_pVolumeData->getSpaceY(false)
		, m_pVolumeData->getSpaceZ(false));

	feature.fMeanHU = getMeanFeatureValue(m_pVolumeData->getMaskDataPoint(nMaskByteIdx_1)
		, m_pVolumeData->getHUDataPoint(), m_pVolumeData->getCX()
		, m_pVolumeData->getCY(), m_pVolumeData->getCZ()
		, m_pVolumeData->getMask(nMaskUID_1));

	feature.fStdDeviation = getStandardDeviationFeatureValue(m_pVolumeData->getMaskDataPoint(nMaskByteIdx_1)
		, m_pVolumeData->getHUDataPoint()
		, m_pVolumeData->getCX(), m_pVolumeData->getCY(), m_pVolumeData->getCZ()
		, m_pVolumeData->getMask(nMaskUID_1));

	feature.fPneumoniaBurden = -1.f;
	feature.fRatio = -1.f;
	m_pOutReport->vecFeature.push_back(feature);

	setProgressValue(25);
	//

	/////////////////////////// pneumonia
	feature = COVID_TEXTURE_FEATURE();

	int nMaskUID_2 = pMaskInfo_2->uid;
	int nMaskByteIdx_2 = m_pVolumeData->GetMaskByteIndex(nMaskUID_2);

	 std::wstring ws2(pMaskInfo_2->maskName);
	feature.strROIName = std::string(ws2.begin(), ws2.end());
	feature.fObjectVolume = getVoxelVolumeFeatureValue(m_pVolumeData->getMaskDataPoint(nMaskByteIdx_2)
		, m_pVolumeData->getHUDataPoint(), m_pVolumeData->getCX(), m_pVolumeData->getCY()
		, m_pVolumeData->getCZ(), m_pVolumeData->getMask(nMaskUID_2)
		, m_pVolumeData->getSpaceX(false), m_pVolumeData->getSpaceY(false)
		, m_pVolumeData->getSpaceZ(false));

	feature.fMeanHU = getMeanFeatureValue(m_pVolumeData->getMaskDataPoint(nMaskByteIdx_2)
		, m_pVolumeData->getHUDataPoint(), m_pVolumeData->getCX()
		, m_pVolumeData->getCY(), m_pVolumeData->getCZ()
		, m_pVolumeData->getMask(nMaskUID_2));

	feature.fStdDeviation = getStandardDeviationFeatureValue(m_pVolumeData->getMaskDataPoint(nMaskByteIdx_2)
		, m_pVolumeData->getHUDataPoint()
		, m_pVolumeData->getCX(), m_pVolumeData->getCY(), m_pVolumeData->getCZ()
		, m_pVolumeData->getMask(nMaskUID_2));

	feature.fPneumoniaBurden = getPneumoniaBurdenValue(m_pVolumeData->getMaskDataPoint(nMaskByteIdx_2)
		, m_pVolumeData->getHUDataPoint(), m_pVolumeData->getMask(nMaskUID_2)
		, m_pVolumeData->getCX(), m_pVolumeData->getCY(), m_pVolumeData->getCZ()
		, m_pVolumeData->getSpaceX(false), m_pVolumeData->getSpaceY(false), m_pVolumeData->getSpaceZ(false));

	feature.fRatio = getExtentRatioValue(m_pVolumeData->getMaskDataPoint(nMaskByteIdx_1)
		, m_pVolumeData->getMaskDataPoint(nMaskByteIdx_2)
		, m_pVolumeData->getMask(nMaskUID_1)
		, m_pVolumeData->getMask(nMaskUID_2)
		, m_pVolumeData->getCX(), m_pVolumeData->getCY(), m_pVolumeData->getCZ());

	m_pOutReport->vecFeature.push_back(feature);	

	setProgressValue(50);
	///

	// calculate image depth
	int cx = m_pVolumeData->getCX();
	int cy = m_pVolumeData->getCY();
	int cz = m_pVolumeData->getCZ();
	int maxAxialNum = 0, maxCoronalNum = 0, maxSaggitalNum = 0;
	int nAxialDepth = -1, nCoronalDepth = -1, nSaggitalDepth = -1;

	MaskInfo* pMaskInfo = nullptr;
	for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
	{
		pMaskInfo = m_pVolumeData->getMaskInfo(i, false);
		if (!wcscmp(pMaskInfo->maskName, L"Pneumonia"))
		{			
			qDebug() << "mask name:" << QString::fromStdWString(pMaskInfo->maskName) << endl;;
			qDebug() << "uid:" << QString::number(pMaskInfo->uid) << endl;;
			break;
			
		}
		pMaskInfo = nullptr;
	}

	if (pMaskInfo == nullptr)
		return;

	m_pOutReport->nUID = pMaskInfo->uid;
	mask* pMask3D = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(pMaskInfo->uid));
	mask maskBit = m_pVolumeData->getMask(pMaskInfo->uid);

	setProgressValue(60);
	
	// axial
	for (int z = 0; z < cz; ++z)
	{
		int nCnt = 0;
		for (int y = 0; y < cy; ++y)
		{
			for (int x = 0; x < cx; ++x)
			{
				int idx = z*cx*cy + y*cx + x;
				if (pMask3D[idx] & maskBit)
				{
					++nCnt;
				}
			}
		}

		if (maxAxialNum < nCnt)
		{
			maxAxialNum = nCnt;
			nAxialDepth = z;
		}		
	}
	m_pOutReport->nAxialDepth = nAxialDepth;

	setProgressValue(70);

	// coronal
	for (int y = 0; y < cy; ++y)	
	{
		int nCnt = 0;
		for (int z = 0; z < cz; ++z)
		{
			for (int x = 0; x < cx; ++x)
			{
				int idx = z*cx*cy + y*cx + x;
				if (pMask3D[idx] & maskBit)
				{
					++nCnt;
				}
			}
		}

		if (maxCoronalNum < nCnt)
		{
			maxCoronalNum = nCnt;
			nCoronalDepth = y;
		}
	}
	m_pOutReport->nCoronalDepth = nCoronalDepth;

	setProgressValue(80);

	// sagittal
	for (int x = 0; x < cx; ++x)
	{
		int nCnt = 0;
		for (int z = 0; z < cz; ++z)
		{
			for (int y = 0; y < cy; ++y)
			{
				int idx = z*cx*cy + y*cx + x;
				if (pMask3D[idx] & maskBit)
				{
					++nCnt;
				}
			}
		}

		if (maxSaggitalNum < nCnt)
		{
			maxSaggitalNum = nCnt;
			nSaggitalDepth = x;
		}
	}
	m_pOutReport->nSaggitalDepth = nSaggitalDepth;

	setProgressValue(90);
	
			
	setProgressValue(100);

	emit finished();
}
