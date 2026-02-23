#include "stdafx.h"
#include "ActionDeepCatchReport.h"
#include "algorithm/Radiomics.h"
#include "Windows/windowManager.h"
#include "MedipType.h"
#include <cctype> //C++


WorkDeepCatchReport::WorkDeepCatchReport(VOLUME_DATA *volume, DEEPCATCH_REPORT* pOutReport, DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo, int funLevel)
{	
	m_pVolumeData = volume;
	m_pOutReport = pOutReport;
	m_pPredictedInfo = pPredictedInfo;
	m_nFunLevel = funLevel;
}

void WorkDeepCatchReport::setProgressValue(int value, bool init /*= false*/)
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

void WorkDeepCatchReport::threadRun()
{
	setProgressValue(0, true);

	// preferernce for reporting : single slice, multi slice
	// single slice : L3, User Define
	// multi slice : Abdominal Waist, User Define
	qint8 &preferenceType = m_pPredictedInfo->stPredictOpt.preferenceType;
	qint8 &singleSliceType = m_pPredictedInfo->stPredictOpt.singleSliceType;
	qint8 &multiSliceType = m_pPredictedInfo->stPredictOpt.multiSliceType;

	// calculate image depth
	int cx = m_pVolumeData->getCX();
	int cy = m_pVolumeData->getCY();
	int cz = m_pVolumeData->getCZ();
	int nMaxAxialNum = 0, nPreMaxAxialNum = 0;
	int nAxialDepth = -1, nPreAxialDepth = -1;

	if (m_pPredictedInfo->nAxialDepth != -1)
		m_pOutReport->nDisplayAxialDepth = (cz - 1) - m_pPredictedInfo->nAxialDepth;
	if (m_pPredictedInfo->nStartAxialDepth != -1)
		m_pOutReport->nDisplayStartAxialDepth = (cz - 1) - m_pPredictedInfo->nStartAxialDepth;
	if (m_pPredictedInfo->nEndAxialDepth != -1)
		m_pOutReport->nDisplayEndAxialDepth = (cz - 1) - m_pPredictedInfo->nEndAxialDepth;
//	printf("void WorkDeepCatchReport::threadRun() : %d, %d, %d\n", m_pOutReport->nAxialDepth, m_pOutReport->nStartAxialDepth, m_pOutReport->nEndAxialDepth);

	bool bInvalid_MaskData = false;
#ifdef DEEPCATCH_REPORT_MASK_UID_USE
	QMap<QString, int> &mapDeepCatchPredictResultRoi = m_pPredictedInfo->mapDeepCatchPredictResultRoi;
	if (!mapDeepCatchPredictResultRoi.isEmpty())
	{
		int curUid = -1;
		int L3_Mask_Uid = -1;
		int AbdominalWaist_Mask_Uid = -1;
		QMap<QString, int>::const_iterator iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_L3);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			L3_Mask_Uid = iterMap.value();
		iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			AbdominalWaist_Mask_Uid = iterMap.value();

		if (preferenceType == eDPTSingleSlice)
			curUid = L3_Mask_Uid;
		else if (preferenceType == eDPTMultiSlice)
			curUid = AbdominalWaist_Mask_Uid;

		/* mask data가 무효한 경우 예외 처리. */
		// mask 생성이 안된 경우, voxel count 가 0인 경우 예외 처리.
		if (curUid == -1 || !m_pVolumeData->getVoxelCount(curUid))
		{
			//	printf("pMaskInfo : %s\n", (pMaskInfo == nullptr) ? "pMaskInfo == nullptr" : (!m_pVolumeData->getVoxelCount(pMaskInfo->uid) ? "voxel count == 0" : "good"));
			bInvalid_MaskData = true;	// mask data 무효함.
			m_pOutReport->reportThreadResult = eDRTRCMaskInvalid;
		}

		iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			m_pOutReport->vecAxialViewMaskUIDs.push_back(iterMap.value());
		iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_AVF);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			m_pOutReport->vecAxialViewMaskUIDs.push_back(iterMap.value());
		iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_OF);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			m_pOutReport->vecAxialViewMaskUIDs.push_back(iterMap.value());
	}
#else
	MaskInfo* pMaskInfo = nullptr;
//	MaskInfo* pMaskInfo_L3 = nullptr;
//	MaskInfo* pMaskInfo_abdominalWaist = nullptr;
	for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
	{
		pMaskInfo = m_pVolumeData->getMaskInfo(i, false);
		QString maskName = QString::fromWCharArray(pMaskInfo->maskName);
	//	if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_L3))
	//		pMaskInfo_L3 = pMaskInfo;
	//	if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST))
	//		pMaskInfo_abdominalWaist = pMaskInfo;
		if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE) ||
			!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_AVF) ||
			!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_OF))
			m_pOutReport->vecAxialViewMaskUIDs.push_back(pMaskInfo->uid);
	}

/*
	if (preferenceType == eDPTSingleSlice)
		pMaskInfo = pMaskInfo_L3;
	else if (preferenceType == eDPTMultiSlice)
		pMaskInfo = pMaskInfo_abdominalWaist;

	// mask 생성이 안된 경우, voxel count 가 0인 경우 예외 처리.
	if (pMaskInfo == nullptr || !m_pVolumeData->getVoxelCount(pMaskInfo->uid))
	{
		//	printf("pMaskInfo : %s\n", (pMaskInfo == nullptr) ? "pMaskInfo == nullptr" : (!m_pVolumeData->getVoxelCount(pMaskInfo->uid) ? "voxel count == 0" : "good"));
		bInvalid_MaskData = true;	// mask data 무효함.
	}
*/
#endif

	setProgressValue(50);

	if (!bInvalid_MaskData)
	{
		if (preferenceType == eDPTSingleSlice)
		{
			if (m_pPredictedInfo->nAxialDepth == -1)
				bInvalid_MaskData = true;
		}
		else if (preferenceType == eDPTMultiSlice)
		{
			if (m_pPredictedInfo->nStartAxialDepth == -1 && m_pPredictedInfo->nEndAxialDepth == -1)
				bInvalid_MaskData = true;
			else if (m_pPredictedInfo->nStartAxialDepth == -1 || m_pPredictedInfo->nEndAxialDepth == -1)
			{
				// single slice 처리.
			//	m_pOutReport->deepcatchUISettingInfo.preferenceType = eDRPTSingleSlice;
			//	m_pOutReport->nDisplayAxialDepth = (m_pPredictedInfo->nStartAxialDepth != -1) ? ((cz - 1) - m_pPredictedInfo->nStartAxialDepth) : ((cz - 1) - m_pPredictedInfo->nEndAxialDepth);
			//	printf("else if (m_pOutReport->nStartAxialDepth == -1 || m_pOutReport->nEndAxialDepth == -1) : %d, %d, %d", m_pOutReport->nAxialDepth, m_pOutReport->nStartAxialDepth, m_pOutReport->nEndAxialDepth);
				bInvalid_MaskData = true;
			}
		//	else
		//	{
				// start - end의 중간 위치.
		//		m_pOutReport->nDisplayAxialDepth = (((cz - 1) - m_pPredictedInfo->nStartAxialDepth) + ((cz - 1) - m_pPredictedInfo->nEndAxialDepth)) / 2;
			//	printf("else : %d, %d, %d", m_pOutReport->nAxialDepth, m_pOutReport->nStartAxialDepth, m_pOutReport->nEndAxialDepth);
		//	}
			//	printf("nStartAxialDepth : %d, nEndAxialDepth : %d, nAxialDepth : %d\n", m_pOutReport->nStartAxialDepth, m_pOutReport->nEndAxialDepth, m_pOutReport->nAxialDepth);
		}
	}

	setProgressValue(75);
	initChartData_percentage_(bInvalid_MaskData);

	setProgressValue(90);
	setProgressValue(100);

	// multi slice에서 axial drawing view는 multi slice range의 중간 depth를 이용.
	if (!bInvalid_MaskData && preferenceType == eDPTMultiSlice)
		m_pOutReport->nDisplayAxialDepth = (((cz - 1) - m_pPredictedInfo->nStartAxialDepth) + ((cz - 1) - m_pPredictedInfo->nEndAxialDepth)) / 2;

	emit finished();
}

void WorkDeepCatchReport::initChartData_percentage_(bool bInvalid_MaskData)
{
	qint8 &preferenceType = m_pPredictedInfo->stPredictOpt.preferenceType;
	qint8 &singleSliceType = m_pPredictedInfo->stPredictOpt.singleSliceType;
	qint8 &multiSliceType = m_pPredictedInfo->stPredictOpt.multiSliceType;

	int cx = m_pVolumeData->getCX();
	int cy = m_pVolumeData->getCY();
	int cz = m_pVolumeData->getCZ();

	int mask_Uid[eDWMMaxSize];
	memset(mask_Uid, -1, sizeof(mask_Uid));
	int mask_Uid_muscleQualityMap[eDMQMCMaxSize];
	memset(mask_Uid_muscleQualityMap, -1, sizeof(mask_Uid_muscleQualityMap));

//SUPPORT_DEEPCATCH_VERSION_2
	int mask_Uid_VertebraMap[eDVertebraSize];
	memset(mask_Uid_VertebraMap, -1, sizeof(mask_Uid_VertebraMap));

	int mask_Uid_ioClass_Map[eDIOClassSize];
	memset(mask_Uid_ioClass_Map, -1, sizeof(mask_Uid_ioClass_Map));

#ifdef DEEPCATCH_REPORT_MASK_UID_USE
	QMap<QString, int> &mapDeepCatchPredictResultRoi = m_pPredictedInfo->mapDeepCatchPredictResultRoi;
	if (!mapDeepCatchPredictResultRoi.isEmpty())
	{
		QMap<QString, int>::const_iterator iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_SKIN);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			mask_Uid[eDWMSkin] = iterMap.value();
		iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_BONE);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			mask_Uid[eDWMBone] = iterMap.value();
		iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			mask_Uid[eDWMMuscle] = iterMap.value();
		iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_AVF);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			mask_Uid[eDWMAVF] = iterMap.value();
		iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_OF);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			mask_Uid[eDWMOF] = iterMap.value();
		iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_IO);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			mask_Uid[eDWMIO] = iterMap.value();
		iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_BSC);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			mask_Uid[eDWMBSC] = iterMap.value();
	}
#else
	MaskInfo* pMaskInfo = nullptr;
	for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
	{
		pMaskInfo = m_pVolumeData->getMaskInfo(i, false);
		QString maskName = QString::fromWCharArray(pMaskInfo->maskName);

		// DeepCatch default 7 classes mask
		if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_SKIN))
			mask_Uid[eDWMSkin] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_BONE))
			mask_Uid[eDWMBone] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE))
			mask_Uid[eDWMMuscle] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_AVF))
			mask_Uid[eDWMAVF] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_OF))
			mask_Uid[eDWMOF] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO))
			mask_Uid[eDWMIO] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_BSC))
			mask_Uid[eDWMBSC] = pMaskInfo->uid;

		// muslce quality map 4 classes mask
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ADP_TISSUE))
			mask_Uid_muscleQualityMap[eDMQMCMuscleAdpTissue] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ABNORMAL1))
			mask_Uid_muscleQualityMap[eDMQMCMuscleAbnormal1] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ABNORMAL2))
			mask_Uid_muscleQualityMap[eDMQMCMuscleAbnormal2] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_NORMAL))
			mask_Uid_muscleQualityMap[eDMQMCMuscleNormal] = pMaskInfo->uid;

//SUPPORT_DEEPCATCH_VERSION_2
		// vertebra map classes mask
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_L1))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_L1);
			mask_Uid_VertebraMap[eDVertebra_L1] = _uid;
		}			
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_L2))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_L2);
			mask_Uid_VertebraMap[eDVertebra_L2] = _uid;
		}	
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_L3))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_L3);
			mask_Uid_VertebraMap[eDVertebra_L3] = _uid;
		}	
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_L4))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_L4);
			mask_Uid_VertebraMap[eDVertebra_L4] = _uid;
		}	
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_L5))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_L5);
			mask_Uid_VertebraMap[eDVertebra_L5] = _uid;
		}			
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_T1))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_T1);
			mask_Uid_VertebraMap[eDVertebra_T1] = _uid;
		}	
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_T2))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_T2);
			mask_Uid_VertebraMap[eDVertebra_T2] = _uid;
		}			
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_T3))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_T3);
			mask_Uid_VertebraMap[eDVertebra_T3] = _uid;
		}			
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_T4))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_T4);
			mask_Uid_VertebraMap[eDVertebra_T4] = _uid;
		}			
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_T5))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_T5);
			mask_Uid_VertebraMap[eDVertebra_T5] = _uid;
		}	
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_T6))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_T6);
			mask_Uid_VertebraMap[eDVertebra_T6] = _uid;
		}			
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_T7))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_T7);
			mask_Uid_VertebraMap[eDVertebra_T7] = _uid;
		}			
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_T8))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_T8);
			mask_Uid_VertebraMap[eDVertebra_T8] = _uid;
		}			
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_T9))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_T9);
			mask_Uid_VertebraMap[eDVertebra_T9] = _uid;
		}	
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_T10))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_T10);
			mask_Uid_VertebraMap[eDVertebra_T10] = _uid;
		}			
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_T11))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_T11);
			mask_Uid_VertebraMap[eDVertebra_T11] = _uid;
		}			
		else if (!maskName.compare(DEEPCATCH_VB_MASKNAME_T12))
		{
			muint32 _uid = MAX_int32;
			_uid = getDuplicatedOutputMaskID(DEEPCATCH_VB_MASKNAME_T12);
			mask_Uid_VertebraMap[eDVertebra_T12] = _uid;
		}			
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_LIVER))
			mask_Uid_ioClass_Map[eDIOClass_Liver] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_SPLEEN))
			mask_Uid_ioClass_Map[eDIOClass_Spleen] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_AORTA_ARTERY))
			mask_Uid_ioClass_Map[eDIOClass_Aorta_Artery] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_AORTA_VEIN))
			mask_Uid_ioClass_Map[eDIOClass_Aorta_Vein] = pMaskInfo->uid;	
	}
#endif

	memcpy(m_pOutReport->expertReportInfo.mask_Uid, mask_Uid, sizeof(mask_Uid));
	memcpy(m_pOutReport->expertReportInfo.mask_Uid_muscleQualityMap, mask_Uid_muscleQualityMap, sizeof(mask_Uid_muscleQualityMap));


	if (m_nFunLevel == MFL_Product_DeepCatch_DeepCatchV2)
	{
		memcpy(m_pOutReport->expertReportInfo.mask_Uid_VertebraMap, mask_Uid_VertebraMap, sizeof(mask_Uid_VertebraMap));
		memcpy(m_pOutReport->expertReportInfo.mask_Uid_IOClassMap, mask_Uid_ioClass_Map, sizeof(mask_Uid_ioClass_Map));
	}

	for (int i = 0; i < eDWMMaxSize; i++)
	{
		if (mask_Uid[i] == -1)
			m_pOutReport->reportThreadResult = eDRTRCMaskInvalid;
	}

	// deepcatch 7 classes
	mask *pMask3D_skin = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(mask_Uid[eDWMSkin]));
	mask maskBit_skin = m_pVolumeData->getMask(mask_Uid[eDWMSkin]);
	mask *pMask3D_bone = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(mask_Uid[eDWMBone]));
	mask maskBit_bone = m_pVolumeData->getMask(mask_Uid[eDWMBone]);
	mask *pMask3D_muscle = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(mask_Uid[eDWMMuscle]));
	mask maskBit_muscle = m_pVolumeData->getMask(mask_Uid[eDWMMuscle]);
	mask *pMask3D_avf = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(mask_Uid[eDWMAVF]));
	mask maskBit_avf = m_pVolumeData->getMask(mask_Uid[eDWMAVF]);
	mask *pMask3D_of = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(mask_Uid[eDWMOF]));
	mask maskBit_of = m_pVolumeData->getMask(mask_Uid[eDWMOF]);
	mask *pMask3D_io = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(mask_Uid[eDWMIO]));
	mask maskBit_io = m_pVolumeData->getMask(mask_Uid[eDWMIO]);
	mask *pMask3D_bsc = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(mask_Uid[eDWMBSC]));
	mask maskBit_bsc = m_pVolumeData->getMask(mask_Uid[eDWMBSC]);

	int nCnt_skin = 0;
	int nCnt_bone = 0;
	int nCnt_muscle = 0;
	int nCnt_avf = 0;
	int nCnt_of = 0;
	int nCnt_io = 0;
	int nCnt_bsc = 0;
	int stackedMuscleCount = 0;
	int stackedOfCount = 0;
	int sumMask = 0;
	int index = 0;

	// muslce quality map classes
	mask *pMask3D_muscle_adp_tissue = nullptr;
	if (mask_Uid_muscleQualityMap[eDMQMCMuscleAdpTissue] != -1)
		pMask3D_muscle_adp_tissue = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(mask_Uid_muscleQualityMap[eDMQMCMuscleAdpTissue]));
	mask maskBit_muscle_adp_tissue = m_pVolumeData->getMask(mask_Uid_muscleQualityMap[eDMQMCMuscleAdpTissue]);

	mask *pMask3D_muscle_abnormal1 = nullptr;
	if (mask_Uid_muscleQualityMap[eDMQMCMuscleAbnormal1] != -1)
		pMask3D_muscle_abnormal1 = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(mask_Uid_muscleQualityMap[eDMQMCMuscleAbnormal1]));
	mask maskBit_muscle_abnormal1 = m_pVolumeData->getMask(mask_Uid_muscleQualityMap[eDMQMCMuscleAbnormal1]);

	mask *pMask3D_muscle_abnormal2 = nullptr;
	if (mask_Uid_muscleQualityMap[eDMQMCMuscleAbnormal2] != -1)
		pMask3D_muscle_abnormal2 = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(mask_Uid_muscleQualityMap[eDMQMCMuscleAbnormal2]));
	mask maskBit_muscle_abnormal2 = m_pVolumeData->getMask(mask_Uid_muscleQualityMap[eDMQMCMuscleAbnormal2]);

	mask *pMask3D_muscle_normal = nullptr;
	if (mask_Uid_muscleQualityMap[eDMQMCMuscleNormal] != -1)
		pMask3D_muscle_normal = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(mask_Uid_muscleQualityMap[eDMQMCMuscleNormal]));
	mask maskBit_muscle_normal = m_pVolumeData->getMask(mask_Uid_muscleQualityMap[eDMQMCMuscleNormal]);

	int nCnt_muscle_adp_tissue = 0;
	int nCnt_muscle_abnormal1 = 0;
	int nCnt_muscle_abnormal2 = 0;
	int nCnt_muscle_normal = 0;

	// hu 값 sum
	qlonglong nSkinHuSum = 0;
	qlonglong nBoneHuSum = 0;
	qlonglong nMuscleHuSum = 0;
	qlonglong nAvfHuSum = 0;
	qlonglong nOfHuSum = 0;
	qlonglong nIoHuSum = 0;
	qlonglong nBscHuSum = 0;

	QVector<QMap<eDeepCatchWholebodyClasses, int>> mapDeepcatchClassToVolxelCountArray;								// abdominal waist volxel count array.
	QVector<QMap<eDeepCatchMuscleQualityMapClasses, int>> mapDeepcatchClassToVolxelCountArray_muslceQualityMap;

	QVector<QMap<eDeepCatchWholebodyClasses, float>> meanHuMapArray;												// abdominal waist hu map array.

																														
	std::vector<mask*> MaskVBPtList;
	std::vector<mask> MaskVBBitList;
	
	std::vector<mask*> MaskIOPtList;
	std::vector<mask> MaskIOBitList;

	if (m_nFunLevel == MFL_Product_DeepCatch_DeepCatchV2)
	{
		_createMaskPtBit(MaskVBPtList, MaskVBBitList, mask_Uid_VertebraMap, (int)eDVertebraSize);
		_createMaskPtBit(MaskIOPtList, MaskIOBitList, mask_Uid_ioClass_Map, (int)eDIOClassSize);
	}

	int nCnt_vb[eDVertebraSize] = { 0, };
	int sumVBMask[eDVertebraSize] = { 0, };
	qlonglong arrVBHuSum[eDVertebraSize] = { 0, };

	QVector<QMap<eDeepCatchVertebraMapClasses, int>> mapDeepcatchClassToVolxelCountArray_vertebraMap;
	QVector<QMap<eDeepCatchVertebraMapClasses, float>> meanvertebraHuMapArray;

	int nCnt_IO_Class [eDIOClassSize] = { 0, };
	qlonglong arrIOHuSum[eDIOClassSize] = { 0, };
	int sumIOMask[eDIOClassSize] = { 0, };

	QVector<QMap<eDeepCatchIOClassMapClasses, int>> mapDeepcatchClassToVolxelCountArray_IO_Map;
	QVector<QMap<eDeepCatchIOClassMapClasses, float>> meanIOHuMapArray;	

	//	for (int z = 0; z < cz; ++z)
	for (int z = cz - 1; z >= 0; z--)
	{
		nCnt_skin = 0;
		nCnt_bone = 0;
		nCnt_muscle = 0;
		nCnt_avf = 0;
		nCnt_of = 0;
		nCnt_io = 0;
		nCnt_bsc = 0;
		stackedMuscleCount = 0;
		stackedOfCount = 0;
		sumMask = 0;

		nCnt_muscle_adp_tissue = 0;
		nCnt_muscle_abnormal1 = 0;
		nCnt_muscle_abnormal2 = 0;
		nCnt_muscle_normal = 0;

		nSkinHuSum = 0;
		nBoneHuSum = 0;
		nMuscleHuSum = 0;
		nAvfHuSum = 0;
		nOfHuSum = 0;
		nIoHuSum = 0;
		nBscHuSum = 0;

//SUPPORT_DEEPCATCH_VERSION_2
		if (m_nFunLevel == MFL_Product_DeepCatch_DeepCatchV2)
		{
			for (int ii = eDVertebra_L1; ii < eDVertebraSize; ii++)
			{
				nCnt_vb[ii] = 0;
				arrVBHuSum[ii] = 0;
			}

			for (int ii = 0; ii < eDIOClassSize; ii++)
			{
				nCnt_IO_Class[ii] = 0;
				arrIOHuSum[ii] = 0;
			}
		}

		for (int y = 0; y < cy; ++y)
		{
			for (int x = 0; x < cx; ++x)
			{
				int idx = z*cx*cy + y*cx + x;

				// deepcatch 7 classes
				if (pMask3D_skin[idx] & maskBit_skin)
				{
					++nCnt_skin;
					nSkinHuSum += m_pVolumeData->getData(idx);
				}
				if (pMask3D_bone[idx] & maskBit_bone)
				{
					++nCnt_bone;
					nBoneHuSum += m_pVolumeData->getData(idx);
				}
				if (pMask3D_muscle[idx] & maskBit_muscle)
				{
					++nCnt_muscle;
					nMuscleHuSum += m_pVolumeData->getData(idx);
				}
				if (pMask3D_avf[idx] & maskBit_avf)
				{
					++nCnt_avf;
					nAvfHuSum += m_pVolumeData->getData(idx);
				}
				if (pMask3D_of[idx] & maskBit_of)
				{
					++nCnt_of;
					nOfHuSum += m_pVolumeData->getData(idx);
				}
				if (pMask3D_io[idx] & maskBit_io)
				{
					++nCnt_io;
					nIoHuSum += m_pVolumeData->getData(idx);
				}
				if (pMask3D_bsc[idx] & maskBit_bsc)
				{
					++nCnt_bsc;
					nBscHuSum += m_pVolumeData->getData(idx);
				}

				// muscle quality map classes
				if (pMask3D_muscle_adp_tissue)
				{
					if (pMask3D_muscle_adp_tissue[idx] & maskBit_muscle_adp_tissue)
						++nCnt_muscle_adp_tissue;
				}
				if (pMask3D_muscle_abnormal1)
				{
					if (pMask3D_muscle_abnormal1[idx] & maskBit_muscle_abnormal1)
						++nCnt_muscle_abnormal1;
				}
				if (pMask3D_muscle_abnormal2)
				{
					if (pMask3D_muscle_abnormal2[idx] & maskBit_muscle_abnormal2)
						++nCnt_muscle_abnormal2;
				}
				if (pMask3D_muscle_normal)
				{
					if (pMask3D_muscle_normal[idx] & maskBit_muscle_normal)
						++nCnt_muscle_normal;
				}

//SUPPORT_DEEPCATCH_VERSION_2
				if (m_nFunLevel == MFL_Product_DeepCatch_DeepCatchV2)
				{
					for (int ii = eDVertebra_L1; ii < eDVertebraSize; ii++)
					{
						mask* pMaskPT = MaskVBPtList[ii];
						mask maskBit = MaskVBBitList[ii];
						if (pMaskPT == nullptr) continue;
						if (pMaskPT[idx] & maskBit)
						{
							++nCnt_vb[ii];
							arrVBHuSum[ii] += m_pVolumeData->getData(idx);
						}
					}

					for (int ii = 0; ii < eDIOClassSize; ii++)
					{
						mask* pMaskPT = MaskIOPtList[ii];
						mask maskBit = MaskIOBitList[ii];
						if (pMaskPT == nullptr) continue;
						if (pMaskPT[idx] & maskBit)
						{
							++nCnt_IO_Class[ii];
							arrIOHuSum[ii] += m_pVolumeData->getData(idx);
						}
					}
				}
			}
		}
		stackedMuscleCount = nCnt_avf + nCnt_muscle;
		stackedOfCount = nCnt_avf + nCnt_muscle + nCnt_of;
		sumMask = nCnt_skin + nCnt_bone + nCnt_muscle + nCnt_avf + nCnt_of + nCnt_io + nCnt_bsc;
		//	if ((z % 50) == 0)
		{
#ifdef QT_CHARTS_USE
			m_pOutReport->areaChartInfo.lineSeries_avf << QPointF(index, ((qreal)nCnt_avf / sumMask) * 100);
			m_pOutReport->areaChartInfo.lineSeries_muscle << QPointF(index, ((qreal)stackedMuscleCount / sumMask) * 100);
			m_pOutReport->areaChartInfo.lineSeries_of << QPointF(index, ((qreal)stackedOfCount / sumMask) * 100);
#endif	// QT_CHARTS_USE
		}
		index++;

		if (!bInvalid_MaskData)
		{
			// single slize -> l3 mask depth를 이용하여 각 마스크 복셀수 카운트.
			if (isSingleSliceRange(z))
			{
				// deepcatch 7 classes
				m_pOutReport->expertReportInfo.mapL3VoxelCount.insert(eDWMSkin, nCnt_skin);
				m_pOutReport->expertReportInfo.mapL3VoxelCount.insert(eDWMBone, nCnt_bone);
				m_pOutReport->expertReportInfo.mapL3VoxelCount.insert(eDWMMuscle, nCnt_muscle);
				m_pOutReport->expertReportInfo.mapL3VoxelCount.insert(eDWMAVF, nCnt_avf);
				m_pOutReport->expertReportInfo.mapL3VoxelCount.insert(eDWMOF, nCnt_of);
				m_pOutReport->expertReportInfo.mapL3VoxelCount.insert(eDWMIO, nCnt_io);
				m_pOutReport->expertReportInfo.mapL3VoxelCount.insert(eDWMBSC, nCnt_bsc);

				if (nCnt_skin)
					m_pOutReport->expertReportInfo.mapL3MeanHU.insert(eDWMSkin, (float)nSkinHuSum/nCnt_skin);
				if (nCnt_bone)
					m_pOutReport->expertReportInfo.mapL3MeanHU.insert(eDWMBone, (float)nBoneHuSum/nCnt_bone);
				if (nCnt_muscle)
					m_pOutReport->expertReportInfo.mapL3MeanHU.insert(eDWMMuscle, (float)nMuscleHuSum/nCnt_muscle);
				if (nCnt_avf)
					m_pOutReport->expertReportInfo.mapL3MeanHU.insert(eDWMAVF, (float)nAvfHuSum/nCnt_avf);
				if (nCnt_of)
					m_pOutReport->expertReportInfo.mapL3MeanHU.insert(eDWMOF, (float)nOfHuSum/nCnt_of);
				if (nCnt_io)
					m_pOutReport->expertReportInfo.mapL3MeanHU.insert(eDWMIO, (float)nIoHuSum/nCnt_io);
				if (nCnt_bsc)
					m_pOutReport->expertReportInfo.mapL3MeanHU.insert(eDWMBSC, (float)nBscHuSum/nCnt_bsc);

				// muscle quality map
				if (nCnt_muscle_adp_tissue)
					m_pOutReport->expertReportInfo.mapL3VoxelCount_muscleQualityMap.insert(eDMQMCMuscleAdpTissue, nCnt_muscle_adp_tissue);
				if (nCnt_muscle_abnormal1)
					m_pOutReport->expertReportInfo.mapL3VoxelCount_muscleQualityMap.insert(eDMQMCMuscleAbnormal1, nCnt_muscle_abnormal1);
				if (nCnt_muscle_abnormal2)
					m_pOutReport->expertReportInfo.mapL3VoxelCount_muscleQualityMap.insert(eDMQMCMuscleAbnormal2, nCnt_muscle_abnormal2);
				if (nCnt_muscle_normal)
					m_pOutReport->expertReportInfo.mapL3VoxelCount_muscleQualityMap.insert(eDMQMCMuscleNormal, nCnt_muscle_normal);
			}
			if (isMultiSliceRange(z))
			{
				// deepcatch 7 classes
				QMap<eDeepCatchWholebodyClasses, int> _mapDeepcatchClassToVolxelCount;
				_mapDeepcatchClassToVolxelCount.insert(eDWMSkin, nCnt_skin);
				_mapDeepcatchClassToVolxelCount.insert(eDWMBone, nCnt_bone);
				_mapDeepcatchClassToVolxelCount.insert(eDWMMuscle, nCnt_muscle);
				_mapDeepcatchClassToVolxelCount.insert(eDWMAVF, nCnt_avf);
				_mapDeepcatchClassToVolxelCount.insert(eDWMOF, nCnt_of);
				_mapDeepcatchClassToVolxelCount.insert(eDWMIO, nCnt_io);
				_mapDeepcatchClassToVolxelCount.insert(eDWMBSC, nCnt_bsc);
				mapDeepcatchClassToVolxelCountArray.push_back(_mapDeepcatchClassToVolxelCount);

				QMap<eDeepCatchWholebodyClasses, float> meanHuMap;
				if (nCnt_skin)
					meanHuMap.insert(eDWMSkin, (float)nSkinHuSum / nCnt_skin);
				if (nCnt_bone)
					meanHuMap.insert(eDWMBone, (float)nBoneHuSum / nCnt_bone);
				if (nCnt_muscle)
					meanHuMap.insert(eDWMMuscle, (float)nMuscleHuSum / nCnt_muscle);
				if (nCnt_avf)
					meanHuMap.insert(eDWMAVF, (float)nAvfHuSum / nCnt_avf);
				if (nCnt_of)
					meanHuMap.insert(eDWMOF, (float)nOfHuSum / nCnt_of);
				if (nCnt_io)
					meanHuMap.insert(eDWMIO, (float)nIoHuSum / nCnt_io);
				if (nCnt_bsc)
					meanHuMap.insert(eDWMBSC, (float)nBscHuSum / nCnt_bsc);
				meanHuMapArray.push_back(meanHuMap);

				// muscle quality map
				QMap<eDeepCatchMuscleQualityMapClasses, int> _mapDeepcatchClassToVolxelCount_muslcleQualityMap;
				if (nCnt_muscle_adp_tissue)
					_mapDeepcatchClassToVolxelCount_muslcleQualityMap.insert(eDMQMCMuscleAdpTissue, nCnt_muscle_adp_tissue);
				if (nCnt_muscle_abnormal1)
					_mapDeepcatchClassToVolxelCount_muslcleQualityMap.insert(eDMQMCMuscleAbnormal1, nCnt_muscle_abnormal1);
				if (nCnt_muscle_abnormal2)
					_mapDeepcatchClassToVolxelCount_muslcleQualityMap.insert(eDMQMCMuscleAbnormal2, nCnt_muscle_abnormal2);
				if (nCnt_muscle_normal)
					_mapDeepcatchClassToVolxelCount_muslcleQualityMap.insert(eDMQMCMuscleNormal, nCnt_muscle_normal);
				if (!_mapDeepcatchClassToVolxelCount_muslcleQualityMap.isEmpty())
					mapDeepcatchClassToVolxelCountArray_muslceQualityMap.push_back(_mapDeepcatchClassToVolxelCount_muslcleQualityMap);	
			}

			//SUPPORT_DEEPCATCH_VERSION_2
			if (m_nFunLevel == MFL_Product_DeepCatch_DeepCatchV2)
			{
				// vertebra map classes
				QMap<eDeepCatchVertebraMapClasses, int> _mapDeepcatchClassVBToVolxelCount;
				for (int ii = eDVertebra_L1; ii < eDVertebraSize; ii++)
				{
					_mapDeepcatchClassVBToVolxelCount.insert((eDeepCatchVertebraMapClasses)ii, nCnt_vb[ii]);
				}
				mapDeepcatchClassToVolxelCountArray_vertebraMap.push_back(_mapDeepcatchClassVBToVolxelCount);

				QMap<eDeepCatchVertebraMapClasses, float> meanVBHuMap;
				for (int ii = eDVertebra_L1; ii < eDVertebraSize; ii++)
				{
					if (nCnt_vb[ii] != 0)
#if 0
						meanVBHuMap.insert((eDeepCatchVertebraMapClasses)ii, (float)arrVBHuSum[ii] / nCnt_vb[ii]);
#else
						meanVBHuMap.insert((eDeepCatchVertebraMapClasses)ii, (float)arrVBHuSum[ii]);
#endif
					else
						meanVBHuMap.insert((eDeepCatchVertebraMapClasses)ii, 0);
				}
				meanvertebraHuMapArray.push_back(meanVBHuMap);

				// io map classes
				QMap<eDeepCatchIOClassMapClasses, int> _mapDeepcatchClassIOToVolxelCount;
				for (int ii = 0; ii < eDIOClassSize; ii++)
				{
					_mapDeepcatchClassIOToVolxelCount.insert((eDeepCatchIOClassMapClasses)ii, nCnt_IO_Class[ii]);
				}
				mapDeepcatchClassToVolxelCountArray_IO_Map.push_back(_mapDeepcatchClassIOToVolxelCount);

				QMap<eDeepCatchIOClassMapClasses, float> meanIOHuMap;
				for (int ii = 0; ii < eDIOClassSize; ii++)
				{
					if (nCnt_IO_Class[ii] > 0)
						meanIOHuMap.insert((eDeepCatchIOClassMapClasses)ii, (float)arrIOHuSum[ii] / nCnt_IO_Class[ii]);
					else
						meanIOHuMap.insert((eDeepCatchIOClassMapClasses)ii, 0);
				}
				meanIOHuMapArray.push_back(meanIOHuMap);
			}			
		}
	}
	m_pOutReport->areaChartInfo.nVerticalAxesMinValue = 0;
	m_pOutReport->areaChartInfo.nVerticalAxesMaxValue = 100;
	m_pOutReport->areaChartInfo.nHorizontalAxesMinValue = 0;
	m_pOutReport->areaChartInfo.nHorizontalAxesMaxValue = cz;

	// abdominal waist voxel count 합계.
	int sumVoxelCountSkin = 0;
	int sumVoxelCountBone = 0;
	int sumVoxelCountMiscle = 0;
	int sumVoxelCountAVF = 0;
	int sumVoxelCountOf = 0;
	int sumVoxelCountIo = 0;
	int sumVoxelCountBsc = 0;
	int arraySize = mapDeepcatchClassToVolxelCountArray.size();
	if (!bInvalid_MaskData && !mapDeepcatchClassToVolxelCountArray.isEmpty())
	{
		QMap<eDeepCatchWholebodyClasses, int> mapDeepcatchClassToVolxelCount;
		QMap<eDeepCatchWholebodyClasses, int>::const_iterator iterMap;
		for (int i = 0; i < arraySize; i++)
		{
			mapDeepcatchClassToVolxelCount = mapDeepcatchClassToVolxelCountArray.at(i);
			iterMap = mapDeepcatchClassToVolxelCount.find(eDWMSkin);
			if (iterMap != mapDeepcatchClassToVolxelCount.end())
				sumVoxelCountSkin += iterMap.value();
			iterMap = mapDeepcatchClassToVolxelCount.find(eDWMBone);
			if (iterMap != mapDeepcatchClassToVolxelCount.end())
				sumVoxelCountBone += iterMap.value();
			iterMap = mapDeepcatchClassToVolxelCount.find(eDWMMuscle);
			if (iterMap != mapDeepcatchClassToVolxelCount.end())
				sumVoxelCountMiscle += iterMap.value();
			iterMap = mapDeepcatchClassToVolxelCount.find(eDWMAVF);
			if (iterMap != mapDeepcatchClassToVolxelCount.end())
				sumVoxelCountAVF += iterMap.value();
			iterMap = mapDeepcatchClassToVolxelCount.find(eDWMOF);
			if (iterMap != mapDeepcatchClassToVolxelCount.end())
				sumVoxelCountOf += iterMap.value();
			iterMap = mapDeepcatchClassToVolxelCount.find(eDWMIO);
			if (iterMap != mapDeepcatchClassToVolxelCount.end())
				sumVoxelCountIo += iterMap.value();
			iterMap = mapDeepcatchClassToVolxelCount.find(eDWMBSC);
			if (iterMap != mapDeepcatchClassToVolxelCount.end())
				sumVoxelCountBsc += iterMap.value();
		}
		// abdominal waist voxel count sum insert.
		m_pOutReport->expertReportInfo.mapAbdominalWaistVoxelCount.insert(eDWMSkin, sumVoxelCountSkin);
		m_pOutReport->expertReportInfo.mapAbdominalWaistVoxelCount.insert(eDWMBone, sumVoxelCountBone);
		m_pOutReport->expertReportInfo.mapAbdominalWaistVoxelCount.insert(eDWMMuscle, sumVoxelCountMiscle);
		m_pOutReport->expertReportInfo.mapAbdominalWaistVoxelCount.insert(eDWMAVF, sumVoxelCountAVF);
		m_pOutReport->expertReportInfo.mapAbdominalWaistVoxelCount.insert(eDWMOF, sumVoxelCountOf);
		m_pOutReport->expertReportInfo.mapAbdominalWaistVoxelCount.insert(eDWMIO, sumVoxelCountIo);
		m_pOutReport->expertReportInfo.mapAbdominalWaistVoxelCount.insert(eDWMBSC, sumVoxelCountBsc);
	}

	// muscle quality map voxel count 합계.
	int sumVoxelCountMuscleAdpTissue = 0;
	int sumVoxelCountMuscleAbnormal1 = 0;
	int sumVoxelCountMuscleAbnormal2 = 0;
	int sumVoxelCountMuscleNormal = 0;
	int arraySize_muslceQualityMap = mapDeepcatchClassToVolxelCountArray_muslceQualityMap.size();
	if (!bInvalid_MaskData && !mapDeepcatchClassToVolxelCountArray_muslceQualityMap.isEmpty())
	{
		QMap<eDeepCatchMuscleQualityMapClasses, int> mapDeepcatchClassToVolxelCount;
		QMap<eDeepCatchMuscleQualityMapClasses, int>::const_iterator iterMap;
		for (int i = 0; i < arraySize_muslceQualityMap; i++)
		{
			mapDeepcatchClassToVolxelCount = mapDeepcatchClassToVolxelCountArray_muslceQualityMap.at(i);
			iterMap = mapDeepcatchClassToVolxelCount.find(eDMQMCMuscleAdpTissue);
			if (iterMap != mapDeepcatchClassToVolxelCount.end())
				sumVoxelCountMuscleAdpTissue += iterMap.value();
			iterMap = mapDeepcatchClassToVolxelCount.find(eDMQMCMuscleAbnormal1);
			if (iterMap != mapDeepcatchClassToVolxelCount.end())
				sumVoxelCountMuscleAbnormal1 += iterMap.value();
			iterMap = mapDeepcatchClassToVolxelCount.find(eDMQMCMuscleAbnormal2);
			if (iterMap != mapDeepcatchClassToVolxelCount.end())
				sumVoxelCountMuscleAbnormal2 += iterMap.value();
			iterMap = mapDeepcatchClassToVolxelCount.find(eDMQMCMuscleNormal);
			if (iterMap != mapDeepcatchClassToVolxelCount.end())
				sumVoxelCountMuscleNormal += iterMap.value();
		}

		// abdominal waist voxel count sum insert. (muscle quality map)
		m_pOutReport->expertReportInfo.mapAbdominalWaistVoxelCount_muscleQualityMap.insert(eDMQMCMuscleAdpTissue, sumVoxelCountMuscleAdpTissue);
		m_pOutReport->expertReportInfo.mapAbdominalWaistVoxelCount_muscleQualityMap.insert(eDMQMCMuscleAbnormal1, sumVoxelCountMuscleAbnormal1);
		m_pOutReport->expertReportInfo.mapAbdominalWaistVoxelCount_muscleQualityMap.insert(eDMQMCMuscleAbnormal2, sumVoxelCountMuscleAbnormal2);
		m_pOutReport->expertReportInfo.mapAbdominalWaistVoxelCount_muscleQualityMap.insert(eDMQMCMuscleNormal, sumVoxelCountMuscleNormal);
	}


	if (m_nFunLevel == MFL_Product_DeepCatch_DeepCatchV2)
	{

		// Vertebra map voxel count 합계
		int sumVoxelCountVertebraL1 = 0;
		int sumVoxelCountVertebra[eDVertebraSize] = { 0, };
		int nArraySize = mapDeepcatchClassToVolxelCountArray_vertebraMap.size();
		if (!bInvalid_MaskData && !mapDeepcatchClassToVolxelCountArray_vertebraMap.isEmpty())
		{
			QMap<eDeepCatchVertebraMapClasses, int> _mapDeepcatchClassVBToVolxelCount;
			QMap<eDeepCatchVertebraMapClasses, int>::const_iterator iterMap;
			for (int ii = eDVertebra_L1; ii < eDVertebraSize; ii++)
			{
				for (int i = 0; i < nArraySize; i++)
				{
					_mapDeepcatchClassVBToVolxelCount = mapDeepcatchClassToVolxelCountArray_vertebraMap.at(i);
					iterMap = _mapDeepcatchClassVBToVolxelCount.find((eDeepCatchVertebraMapClasses)ii);
					if (iterMap != _mapDeepcatchClassVBToVolxelCount.end())
						sumVoxelCountVertebra[ii] += iterMap.value();
				}

				m_pOutReport->expertReportInfo.mapVertebraVoxelCount.insert((eDeepCatchVertebraMapClasses)ii, sumVoxelCountVertebra[ii]);
			}
		}

		// Vertebra mean HU
		for (int ii = eDVertebra_L1; ii < eDVertebraSize; ii++)
			nCnt_vb[ii] = 0;

		double sumVBMeanHu[eDVertebraSize] = { 0, };

		nArraySize = meanvertebraHuMapArray.size();
		if (!bInvalid_MaskData && !meanvertebraHuMapArray.isEmpty())
		{
			QMap<eDeepCatchVertebraMapClasses, float> meanHuMap;
			QMap<eDeepCatchVertebraMapClasses, float>::const_iterator iterMap;
			for (int ii = eDVertebra_L1; ii < eDVertebraSize; ii++)
			{
				for (int i = 0; i < nArraySize; i++)
				{
					meanHuMap = meanvertebraHuMapArray.at(i);
					iterMap = meanHuMap.find((eDeepCatchVertebraMapClasses)ii);
					if (iterMap != meanHuMap.end())
					{
						float value = iterMap.value();						
						sumVBMeanHu[ii] += value;

						if (value != 0)
							nCnt_vb[ii]++;
					}
				}				
				
				float f_HU_Val = 0.0f;				
#if 0
				//HU
				if (nCnt_vb[ii] > 0)
					f_HU_Val = sumVBMeanHu[ii] / nCnt_vb[ii];
#else
				//HU
				if (sumVoxelCountVertebra[ii] > 0)
					f_HU_Val = sumVBMeanHu[ii] / sumVoxelCountVertebra[ii];
#endif

				m_pOutReport->expertReportInfo.mapVertebraMeanHU.insert((eDeepCatchVertebraMapClasses)ii, f_HU_Val);

				if ((eDeepCatchVertebraMapClasses)ii != eDVertebra_L1) continue;

				DcmtkSeriesInfo* pDcmSeriesInfo = WIN_MANAGER->GetDicomInfo();
				std::string strAge = pDcmSeriesInfo->age_;
				int nAge = -1;
				if (isNumeric(strAge))
					nAge = std::stoi(strAge);
				else
				{
					if (m_pPredictedInfo->stPredictOpt.age_ > 0)
						nAge = m_pPredictedInfo->stPredictOpt.age_;
					else
					{
						m_pOutReport->expertReportInfo.mapVertebraTScore.insert(eDVertebra_L1, 0);
						continue;
					}
				}

				float TScore = calculateTScore(nAge, f_HU_Val);
				m_pOutReport->expertReportInfo.mapVertebraTScore.insert(eDVertebra_L1, TScore);
			}
		}

		// io map voxel count 합계
		int sumVoxelCountIOa = 0;
		int sumVoxelCountIO[eDIOClassSize] = { 0, };
		nArraySize = mapDeepcatchClassToVolxelCountArray_IO_Map.size();
		if (!bInvalid_MaskData && !mapDeepcatchClassToVolxelCountArray_IO_Map.isEmpty())
		{
			QMap<eDeepCatchIOClassMapClasses, int> _mapDeepcatchClassIOToVolxelCount;
			QMap<eDeepCatchIOClassMapClasses, int>::const_iterator iterMap;

			for (int ii = 0; ii < eDIOClassSize; ii++)
			{
				for (int i = 0; i < nArraySize; i++)
				{
					_mapDeepcatchClassIOToVolxelCount = mapDeepcatchClassToVolxelCountArray_IO_Map.at(i);
					iterMap = _mapDeepcatchClassIOToVolxelCount.find((eDeepCatchIOClassMapClasses)ii);
					if (iterMap != _mapDeepcatchClassIOToVolxelCount.end())
						sumVoxelCountIO[ii] += iterMap.value();
				}

				m_pOutReport->expertReportInfo.mapIOClassVoxelCount.insert((eDeepCatchIOClassMapClasses)ii, sumVoxelCountIO[ii]);
			}
		}

		// io mean HU
		for (int ii = 0; ii < eDIOClassSize; ii++)
			nCnt_IO_Class[ii] = 0;

		double sumIOMeanHu[eDIOClassSize] = { 0, };

		nArraySize = meanIOHuMapArray.size();
		if (!bInvalid_MaskData && !meanIOHuMapArray.isEmpty())
		{
			QMap<eDeepCatchIOClassMapClasses, float> meanHuMap;
			QMap<eDeepCatchIOClassMapClasses, float>::const_iterator iterMap;
			for (int ii = 0; ii < eDIOClassSize; ii++)
			{
				for (int i = 0; i < nArraySize; i++)
				{
					meanHuMap = meanIOHuMapArray.at(i);
					iterMap = meanHuMap.find((eDeepCatchIOClassMapClasses)ii);
					if (iterMap != meanHuMap.end())
					{
						sumIOMeanHu[ii] += iterMap.value();
						nCnt_IO_Class[ii]++;
					}
				}

				float f_HU_Val = 0.0f;
				//HU
				if (nCnt_IO_Class[ii] > 0)
					f_HU_Val = sumIOMeanHu[ii] / nCnt_IO_Class[ii];

				m_pOutReport->expertReportInfo.mapIOClassMeanHU.insert((eDeepCatchIOClassMapClasses)ii, f_HU_Val);
			}
		}
	}

	// abdominal waist mean HU
	double sumMeanHuSkin = 0;
	double sumMeanHuBone = 0;
	double sumMeanHuMiscle = 0;
	double sumMeanHuAVF = 0;
	double sumMeanHuOf = 0;
	double sumMeanHuIo = 0;
	double sumMeanHuBsc = 0;

	//
	int nCnt_skinHu = 0;
	int nCnt_boneHu = 0;
	int nCnt_muscleHu = 0;
	int nCnt_avfHu = 0;
	int nCnt_ofHu = 0;
	int nCnt_ioHu = 0;
	int nCnt_bscHu = 0;

	arraySize = meanHuMapArray.size();
	if (!bInvalid_MaskData && !meanHuMapArray.isEmpty())
	{
		QMap<eDeepCatchWholebodyClasses, float> meanHuMap;
		QMap<eDeepCatchWholebodyClasses, float>::const_iterator iterMap;
		for (int i = 0; i < arraySize; i++)
		{
			meanHuMap = meanHuMapArray.at(i);
			iterMap = meanHuMap.find(eDWMSkin);
			if (iterMap != meanHuMap.end())
			{
				sumMeanHuSkin += iterMap.value();
				nCnt_skinHu++;
			}
			iterMap = meanHuMap.find(eDWMBone);
			if (iterMap != meanHuMap.end())
			{
				sumMeanHuBone += iterMap.value();
				nCnt_boneHu++;
			}
			iterMap = meanHuMap.find(eDWMMuscle);
			if (iterMap != meanHuMap.end())
			{
				sumMeanHuMiscle += iterMap.value();
				nCnt_muscleHu++;
			}
			iterMap = meanHuMap.find(eDWMAVF);
			if (iterMap != meanHuMap.end())
			{
				sumMeanHuAVF += iterMap.value();
				nCnt_avfHu++;
			}
			iterMap = meanHuMap.find(eDWMOF);
			if (iterMap != meanHuMap.end())
			{
				sumMeanHuOf += iterMap.value();
				nCnt_ofHu++;
			}
			iterMap = meanHuMap.find(eDWMIO);
			if (iterMap != meanHuMap.end())
			{
				sumMeanHuIo += iterMap.value();
				nCnt_ioHu++;
			}
			iterMap = meanHuMap.find(eDWMBSC);
			if (iterMap != meanHuMap.end())
			{
				sumMeanHuBsc += iterMap.value();
				nCnt_bscHu++;
			}
		}
		// abdominal waist voxel count sum insert.
		if (nCnt_skinHu)
			m_pOutReport->expertReportInfo.mapAbdominalWaistMeanHU.insert(eDWMSkin, sumMeanHuSkin / nCnt_skinHu);
		if (nCnt_boneHu)
			m_pOutReport->expertReportInfo.mapAbdominalWaistMeanHU.insert(eDWMBone, sumMeanHuBone / nCnt_boneHu);
		if (nCnt_muscleHu)
			m_pOutReport->expertReportInfo.mapAbdominalWaistMeanHU.insert(eDWMMuscle, sumMeanHuMiscle / nCnt_muscleHu);
		if (nCnt_avfHu)
			m_pOutReport->expertReportInfo.mapAbdominalWaistMeanHU.insert(eDWMAVF, sumMeanHuAVF / nCnt_avfHu);
		if (nCnt_ofHu)
			m_pOutReport->expertReportInfo.mapAbdominalWaistMeanHU.insert(eDWMOF, sumMeanHuOf / nCnt_ofHu);
		if (nCnt_ioHu)
			m_pOutReport->expertReportInfo.mapAbdominalWaistMeanHU.insert(eDWMIO, sumMeanHuIo / nCnt_ioHu);
		if (nCnt_bscHu)
			m_pOutReport->expertReportInfo.mapAbdominalWaistMeanHU.insert(eDWMBSC, sumMeanHuBsc / nCnt_bscHu);

	}

	// multi slice or singl slice에 따라 report에서 사용할 map 선택.
	// single slice : l3 slcie voxel count
	// multi  slice : abdominal waist voxel count
	if (preferenceType == eDPTSingleSlice)
	{
		m_pOutReport->mapDeepcatchClassToVolxelCount = m_pOutReport->expertReportInfo.mapL3VoxelCount;
	}
	else if (preferenceType == eDPTMultiSlice)
	{
		// average = sum / count 
		arraySize = mapDeepcatchClassToVolxelCountArray.size();
		if (arraySize)
		{
			m_pOutReport->mapDeepcatchClassToVolxelCount.insert(eDWMSkin, (sumVoxelCountSkin / arraySize));
			m_pOutReport->mapDeepcatchClassToVolxelCount.insert(eDWMBone, (sumVoxelCountBone / arraySize));
			m_pOutReport->mapDeepcatchClassToVolxelCount.insert(eDWMMuscle, (sumVoxelCountMiscle / arraySize));
			m_pOutReport->mapDeepcatchClassToVolxelCount.insert(eDWMAVF, (sumVoxelCountAVF / arraySize));
			m_pOutReport->mapDeepcatchClassToVolxelCount.insert(eDWMOF, (sumVoxelCountOf / arraySize));
			m_pOutReport->mapDeepcatchClassToVolxelCount.insert(eDWMIO, (sumVoxelCountIo / arraySize));
			m_pOutReport->mapDeepcatchClassToVolxelCount.insert(eDWMBSC, (sumVoxelCountBsc / arraySize));
		}
	}

}

bool WorkDeepCatchReport::isSingleSliceRange(int curDepth)
{
	// 범위가 무효한 경우 예외 처리.
	if (m_pOutReport->nDisplayAxialDepth == -1)
		return false;

	return m_pOutReport->nDisplayAxialDepth == curDepth;
}

bool WorkDeepCatchReport::isMultiSliceRange(int curDepth)
{
	int nMinDepth = 0, nMaxDepth = 0;

	// 범위가 무효한 경우 예외 처리.
	if (m_pOutReport->nDisplayStartAxialDepth == -1 || m_pOutReport->nDisplayEndAxialDepth == -1)
		return false;

	if (m_pOutReport->nDisplayStartAxialDepth <= m_pOutReport->nDisplayEndAxialDepth)
	{
		nMinDepth = m_pOutReport->nDisplayStartAxialDepth;
		nMaxDepth = m_pOutReport->nDisplayEndAxialDepth;
	}
	else
	{
		nMaxDepth = m_pOutReport->nDisplayStartAxialDepth;
		nMinDepth = m_pOutReport->nDisplayEndAxialDepth;
	}
	return (nMinDepth <= curDepth && curDepth <= nMaxDepth) ? true : false;
}

bool WorkDeepCatchReport::isNumeric(std::string & str)
{	
	auto it = str.begin();

	int nCntNum = 0;
	for (int ii = 0; ii < str.size(); ii++)
	{
		char it = str[ii];
		if (std::isdigit(it))
			nCntNum++;
		else
		{
			str.erase(ii, 1);
			ii = -1;
			nCntNum = 0;
			continue;
		}
	}

	return !str.empty() && nCntNum > 0;
}

muint32 WorkDeepCatchReport::getDuplicatedOutputMaskID(QString _str)
{
	int duplicationNumMax = DEEPCATCH_DUPLICATION_MAX;
	QString str = _str;
	MaskInfo* pMaskInfo = m_pVolumeData->findMaskInfo(str);
	MaskInfo* pMaskInfo_dup = nullptr;
	QString strDup;

	for (int jj = 0; jj < duplicationNumMax; jj++)
	{
		strDup = str + "-dup" + QString::number(jj);
		pMaskInfo_dup = m_pVolumeData->findMaskInfo(strDup);
		if (pMaskInfo_dup != nullptr)
		{
			pMaskInfo = pMaskInfo_dup;
			str = strDup;
			break;
		}
	}
	
	return pMaskInfo->uid;
}

//SUPPORT_DEEPCATCH_VERSION_2
float WorkDeepCatchReport::calculateTScore(int age, float meanHU /*, float meanREF, float sdREF*/)
{
	if (age < 0 || meanHU == 0) return 0;

	typedef enum {
		AGE_MIN,
		AGE_MAX,
		MEDIAN,
		MEAN,
		SD,
	} Attribute;

	const int COL_CNT = 13;

	float fTScore = 0.0f;
	for (int ii = 0; ii < COL_CNT; ii++)
	{
		int _age_min	= BoneDensityTable[ii][AGE_MIN];
		int _age_max	= BoneDensityTable[ii][AGE_MAX];
		
		float median	= BoneDensityTable[ii][MEDIAN];
		float REFmean	= BoneDensityTable[ii][MEAN];
		float sd		= BoneDensityTable[ii][SD];
		if ((age >= _age_min  && age <= _age_max))
		{
			fTScore = meanHU - BoneDensityTable[ii][MEAN] / BoneDensityTable[ii][SD];
			break;
		}
	}

	return fTScore;
}
void WorkDeepCatchReport::_createMaskPtBit(std::vector<mask*>& _maskPtList, std::vector<mask>& _maskVitList, int* MaskMap, int Max)
{
	std::vector<mask*> maskPtList;
	std::vector<mask> maskVitList;

	for (int ii = 0; ii < Max; ii++)
	{
		mask *pMask3D_vb = nullptr;
		mask maskBit_vb;
		if (MaskMap[ii] != -1)
		{
			pMask3D_vb = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(MaskMap[ii]));
			maskBit_vb = m_pVolumeData->getMask(MaskMap[ii]);
		}
		maskPtList.push_back(pMask3D_vb);
		maskVitList.push_back(maskBit_vb);
	}

	_maskPtList.swap(maskPtList);
	maskVitList.swap(_maskVitList);
}

#if 0
// avf, muscle, of 면적으로 데이터 구성.
void WorkDeepCatchReport::initChartData()
{
	int cx = m_pVolumeData->getCX();
	int cy = m_pVolumeData->getCY();
	int cz = m_pVolumeData->getCZ();

	MaskInfo* pMaskInfo = nullptr;
	MaskInfo* pMaskInfo_Muscle = nullptr;
	MaskInfo* pMaskInfo_AVF = nullptr;
	MaskInfo* pMaskInfo_OF = nullptr;
	for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
	{
		pMaskInfo = m_pVolumeData->getMaskInfo(i, false);
		QString maskName = QString::fromWCharArray(pMaskInfo->maskName);
		if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE))
			pMaskInfo_Muscle = pMaskInfo;
		if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_AVF))
			pMaskInfo_AVF = pMaskInfo;
		if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_OF))
			pMaskInfo_OF = pMaskInfo;
	}

	mask *pMask3D_muscle = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(pMaskInfo_Muscle->uid));
	mask maskBit_muscle = m_pVolumeData->getMask(pMaskInfo_Muscle->uid);
	mask *pMask3D_avf = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(pMaskInfo_AVF->uid));
	mask maskBit_avf = m_pVolumeData->getMask(pMaskInfo_AVF->uid);
	mask *pMask3D_of = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(pMaskInfo_OF->uid));
	mask maskBit_of = m_pVolumeData->getMask(pMaskInfo_OF->uid);
	// axial
	for (int z = 0; z < cz; ++z)
	{
		int nCnt_muscle = 0;
		int nCnt_avf = 0;
		int nCnt_of = 0;
		int stackedMuscleCount = 0;
		int stackedOfCount = 0;
		for (int y = 0; y < cy; ++y)
		{
			for (int x = 0; x < cx; ++x)
			{
				int idx = z*cx*cy + y*cx + x;
				if (pMask3D_avf[idx] & maskBit_avf)
				{
					++nCnt_avf;
				}
				if (pMask3D_muscle[idx] & maskBit_muscle)
				{
					++nCnt_muscle;
				}
				if (pMask3D_of[idx] & maskBit_of)
				{
					++nCnt_of;
				}
			}
		}
		stackedMuscleCount = nCnt_avf + nCnt_muscle;
		stackedOfCount = nCnt_avf + nCnt_muscle + nCnt_of;
		//	if ((z % 50) == 0)
		{
			m_pOutReport->areaChartInfo.lineSeries_avf << QPointF(z, nCnt_avf);
			m_pOutReport->areaChartInfo.lineSeries_muscle << QPointF(z, stackedMuscleCount);
			m_pOutReport->areaChartInfo.lineSeries_of << QPointF(z, stackedOfCount);
		}
		if (nCnt_avf < m_pOutReport->areaChartInfo.nVerticalAxesMinValue)
			m_pOutReport->areaChartInfo.nVerticalAxesMinValue = nCnt_avf;
		if (m_pOutReport->areaChartInfo.nVerticalAxesMaxValue < stackedOfCount)
			m_pOutReport->areaChartInfo.nVerticalAxesMaxValue = stackedOfCount;

	}
	m_pOutReport->areaChartInfo.nHorizontalAxesMinValue = 0;
	m_pOutReport->areaChartInfo.nHorizontalAxesMaxValue = cz;
}

// 7개 합산 백분율로 계산 2
void WorkDeepCatchReport::initChartData_percentage()
{
	int cx = m_pVolumeData->getCX();
	int cy = m_pVolumeData->getCY();
	int cz = m_pVolumeData->getCZ();

	m_VecDeepcatchClassToMaskinfo.clear();
	MaskInfo* pMaskInfo = nullptr;
	for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
	{
		pMaskInfo = m_pVolumeData->getMaskInfo(i, false);
		if (pMaskInfo)
		{
			QString maskName = QString::fromWCharArray(pMaskInfo->maskName);
			eDeepCatchWholebodyClasses curClassType = eDWMMaxSize;
			if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_SKIN))
				curClassType = eDWMSkin;
			if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_BONE))
				curClassType = eDWMBone;
			if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE))
				curClassType = eDWMMuscle;
			if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_AVF))
				curClassType = eDWMAVF;
			if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_OF))
				curClassType = eDWMOF;
			if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO))
				curClassType = eDWMIO;
			if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_BSC))
				curClassType = eDWMBSC;

			if (curClassType != eDWMMaxSize)
			{
				mask *pMask3D = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(pMaskInfo->uid));
				mask maskBit = m_pVolumeData->getMask(pMaskInfo->uid);
				sCurrentMaskInfo curData = { curClassType, pMask3D, maskBit };
				m_VecDeepcatchClassToMaskinfo.append(QPair<QString, sCurrentMaskInfo>(maskName, curData));
			}
		}
	}

	// axial
	int nCount[eDWMMaxSize] = { 0, };
	int stackedAvfCount = 0;
	int stackedMuscleCount = 0;
	int stackedOfCount = 0;
	int stackedAvfPercent = 0;
	int stackedMusclePercent = 0;
	int stackedOfPercent = 0;
	int nSumCount = 0;

	int index = 0;
	QVector<QPair<QString, sCurrentMaskInfo>>::const_iterator vecIter;
	//	for (int z = 0; z < cz; ++z)
	for (int z = cz - 1; z >= 0; z--)
	{
		memset(nCount, 0x00, sizeof(nCount));
		stackedAvfCount = 0;
		stackedMuscleCount = 0;
		stackedOfCount = 0;
		stackedAvfPercent = 0;
		stackedMusclePercent = 0;
		stackedOfPercent = 0;
		nSumCount = 0;
		for (int y = 0; y < cy; ++y)
		{
			for (int x = 0; x < cx; ++x)
			{
				int idx = z*cx*cy + y*cx + x;

				for (vecIter = m_VecDeepcatchClassToMaskinfo.constBegin(); vecIter != m_VecDeepcatchClassToMaskinfo.constEnd(); vecIter++)
				{
					if (vecIter->second.pMask3D[idx] & vecIter->second.maskBit)
						nCount[vecIter->second.curClassType]++;
				}
			}
		}
		for (int i = 0; i < eDWMMaxSize; i++)
			nSumCount += nCount[i];
		stackedAvfCount = nCount[eDWMAVF];
		stackedMuscleCount = nCount[eDWMAVF] + nCount[eDWMMuscle];
		stackedOfCount = nCount[eDWMAVF] + nCount[eDWMMuscle] + nCount[eDWMOF];
		stackedAvfPercent = ((float)stackedAvfCount / nSumCount) * 100;
		stackedMusclePercent = ((float)stackedMuscleCount / nSumCount) * 100;
		stackedOfPercent = ((float)stackedOfCount / nSumCount) * 100;
		//	if ((z % 50) == 0)
		{
			m_pOutReport->areaChartInfo.lineSeries_avf << QPoint(index, stackedAvfPercent);
			m_pOutReport->areaChartInfo.lineSeries_muscle << QPoint(index, stackedMusclePercent);
			m_pOutReport->areaChartInfo.lineSeries_of << QPoint(index, stackedOfPercent);
		}
		index++;
	}
	m_pOutReport->areaChartInfo.nVerticalAxesMinValue = 0;
	m_pOutReport->areaChartInfo.nVerticalAxesMaxValue = 100;
	m_pOutReport->areaChartInfo.nHorizontalAxesMinValue = 0;
	m_pOutReport->areaChartInfo.nHorizontalAxesMaxValue = cz;
}
#endif