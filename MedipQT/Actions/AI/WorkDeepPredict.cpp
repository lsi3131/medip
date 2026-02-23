#include "stdafx.h"
#include "WorkDeepPredict.h"
#include "Windows/windowManager.h"
#include "Windows/Tabwindow.h"
#include "System/stringManager.h"
#include "Actions/ActionManager.h"
#include "algorithm/Radiomics.h"

#include "DeepInsthink.h"
#include "Definitions.h"
#include "Metadata.h"
#include "Network/Network.h"
#include "LicenseManager.h"
#include "DataContext.h"

#define AI_MEDIAN_VALUE (127)
#define WHOLEBODY_FILTER_DEFAULT_VALUE (-1024)
#define WHOLEBODY_IO_NAME ("IO")

WorkDeepPredict::WorkDeepPredict(
	VOLUME_DATA* pVolumeData,
	LicenseManager* pLicenseManager,
	ActionManager* pActionManager,
	ProductManager* pProductManager,
	QString predictDirPath,
	QString weightDirPath,
	eMEDIP_FUNCTION_LEVEL weightType,
	QString projectName,
	BoundingBoxI box,
	bool useGPU,
	eDeepPredictAICopyMask copyMaskType,
	int nFilterIdx) :
	m_pActionPredictAdd(nullptr)
{
	m_pVolumeData = pVolumeData;
	m_pLicenseManager = pLicenseManager;
	m_pActionManager = pActionManager;
	m_pProductManager = pProductManager;

	m_pResult = std::make_unique<WorkDeepPredictResult>(&m_pActionManager->m_hashThreadResult);

	m_predictDirPath = predictDirPath;
	m_weightDirPath = weightDirPath;
	m_weightType = weightType;
	m_projectName = projectName;
	m_useGPU = useGPU;
	m_BoundingBox = box;
	m_nFilterIdx = nFilterIdx;
	m_start = m_BoundingBox.getLimitZ() - m_BoundingBox.getMaxZ() - 1;
	m_end = m_BoundingBox.getLimitZ() - m_BoundingBox.getMinZ() - 1;

	m_copyMaskType = copyMaskType;

	m_pActionPredictAdd = getActionPredictAdd();

	qInfo() << "work deep draw predict : " << m_projectName << ", filter index : " << m_nFilterIdx << ", action predict add : " << (void*)m_pActionPredictAdd;
}

const WorkDeepPredictResult* WorkDeepPredict::GetResult() const
{
	return m_pResult.get();
}

void WorkDeepPredict::setProgressValue(int value, bool init)
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

void WorkDeepPredict::setRawDataWithVolumePlane_WholeBodyIO(mint16* maskRaw, VOLUME_DATA* vol_dt, int cx, int cy, int i)
{
	int wholeBody_IO_Index = 0;
	bool isWholeBody_IO_IndexExist = false;
	if (m_pActionPredictAdd)
	{
		isWholeBody_IO_IndexExist = m_pActionPredictAdd->GetIndexByAIName(&wholeBody_IO_Index, WHOLEBODY_IO_NAME);
	}

	setRawDataWithVolumePlane(maskRaw, vol_dt, cx, cy, i, isWholeBody_IO_IndexExist, wholeBody_IO_Index, WHOLEBODY_FILTER_DEFAULT_VALUE);
}

void WorkDeepPredict::setRawDataWithVolumePlane(mint16* maskRaw, VOLUME_DATA* vol_dt, int cx, int cy, int i, bool isFilterExist, int indexAI, int fliterValue)
{
	if (m_pActionPredictAdd == nullptr)
	{
		isFilterExist = false;
	}

	for (int y = 0; y < cy; y++)
	{
		for (int x = 0; x < cx; x++)
		{
			maskRaw[y * cx + x] = fliterValue;

			if (
				(m_BoundingBox.getMinX() <= x && m_BoundingBox.getMaxX() >= x) &&
				(m_BoundingBox.getMinY() <= y && m_BoundingBox.getMaxY() >= y)
				)
			{
				if (isFilterExist)
				{
					int idxVolume = i * cy * cx + y * cx + x;
					if (m_pActionPredictAdd->m_vecAIresult[indexAI][idxVolume] > AI_MEDIAN_VALUE)
					{
						maskRaw[y * cx + x] = vol_dt->getData(x, y, i);
					}
				}
				else
				{
					maskRaw[y * cx + x] = vol_dt->getData(x, y, i);
				}
			}
		}
	}
}

ActionPredictAddEx* WorkDeepPredict::getActionPredictAdd() const
{
	vector<QUndoCommand*> reservationWorkList = m_pActionManager->GetReservationWorkList();
	ActionPredictAddEx* pActionPredictAdd = nullptr;

	if ((m_nFilterIdx >= 0) && (reservationWorkList.size() >= (m_nFilterIdx + 1)))
	{
		pActionPredictAdd = (ActionPredictAddEx*)reservationWorkList[m_nFilterIdx];
	}

	return pActionPredictAdd;
}

void WorkDeepPredict::threadRun()
{
#ifdef AI_VER
	qInfo() << "<<<<<<<<<<<<<<<<<<<< AI thread Run";

	std::vector<QUndoCommand*> reservationWorkList = m_pActionManager->GetReservationWorkList();

	////////////////////////////// 경로 설정
	QString predictDirPath = m_predictDirPath;// STRING_MANAGER->AISegmentationPath + "/predict";
	QString weightDirPath = m_weightDirPath;// STRING_MANAGER->AISegmentationPath + "/weight";

	QString predictDirDataPath = predictDirPath + "/data";//deepdraw_slice_1.raw

	//QDir dir(predictDirPath);
	QDir().mkpath(predictDirPath);
	QDir().mkpath(predictDirDataPath);

	std::cout << "predictDirPath : " << predictDirPath.toStdString().c_str() << std::endl;
	std::cout << "weightDirPath : " << weightDirPath.toStdString().c_str() << std::endl;
	std::cout << "predictDirDataPath : " << predictDirDataPath.toStdString().c_str() << std::endl;

	muint32 cx, cy, cz;
	m_pVolumeData->getLengthForScreen(WT_AXIAL, cx, cy, cz);
	setProgressValue(0, true);

	float xSpacing, ySpacing, zSpacing;
	xSpacing = m_pVolumeData->getSpaceX(true);
	ySpacing = m_pVolumeData->getSpaceY(true);
	zSpacing = m_pVolumeData->getSpaceZ(true);

	///////////////////////////////////// make input data
	mint16* maskRaw = new mint16[cx * cy];

	std::cout << "reservationWorkList.size() : " << reservationWorkList.size() << std::endl;
	std::cout << "m_nFilterIdx : " << m_nFilterIdx << std::endl;
	std::cout << "m_start : " << m_start << std::endl;
	std::cout << "m_end : " << m_end << std::endl;

	bool bMEDIP = m_pLicenseManager->getProductType().compare(PRODUCT_NAME_MEDIP) == 0;
	bool bDeepCatch =
		(m_pLicenseManager->getProductType().compare(PRODUCT_NAME_DEEPCATCH) == 0) ||
		(m_pLicenseManager->getProductType().compare(PRODUCT_NAME_DEEPCATCH_V2) == 0);

	// 1차 Lung Predict에서 Lung이 아닌 부분을 -3024로 하여 입력 파일을 만든다.
	bool bMEDIP_LungFilteredMode = false;
	bool bMEDIP_LungFilteredMode_nnUNet = false;
	bool bMEDIP_WholeBodyIOFilteredMode = false;
	bool bMEDIP_HeartFilteredMode = false;	// 심장솔루션 체크(220407 허 건 과장)
	if (bMEDIP && m_weightType == MFL_Common_AI_PredictUsableCount_Credit)
	{
		if (!m_projectName.compare("CT_Cavity3D") ||
			!m_projectName.compare("CT_COVID19") ||
			!m_projectName.compare("CT_Lung Fissure, Lobe3D") ||
			!m_projectName.compare("CT_Lung Lesion") ||
			!m_projectName.compare("CT_BO2D_Addin") ||
			!m_projectName.compare("CT_BO3D_Addin")
			)
		{
			bMEDIP_LungFilteredMode = true;
		}
		else if (
			!m_projectName.compare("CT_BO2DNN_Addin") ||
			!m_projectName.compare("CT_BO3DNN_Addin") ||
			!m_projectName.compare("CT_COVID19_3DNN"))
		{
			bMEDIP_LungFilteredMode_nnUNet = true;
		}
		else if (!m_projectName.compare("CT_PS_3DNN_Addin"))
		{
			bMEDIP_WholeBodyIOFilteredMode = true;
		}
		else if (!m_projectName.compare("CT_HeartVM_3DNN"))
		{
			bMEDIP_HeartFilteredMode = true;
		}
	}
	else if (bMEDIP && m_weightType == MFL_Common_AI_AIPredict_Predict)
	{
		if (!m_projectName.compare("CT_HeartVM_3DNN"))
		{
			bMEDIP_HeartFilteredMode = true;
		}
	}

	std::cout << "lung filter : " << bMEDIP_LungFilteredMode << std::endl;

	bool bDeepCatch_QCTMode = false;
	mask* pMask3D_QCTTrunk = nullptr;
	mask maskBit_QCTTrunk;
	bool bDeepCatch_IOClassificationPredict = false;
	mask* pMask3D_IO = nullptr;
	mask maskBit_IO;
	if (bDeepCatch)
	{
		// deepcatch && qct mode인 경우 처리 건.
		if (THREAD_DEEPCATCH_DEEPDRAW_PREDICT == m_pActionManager->GetCurrentThread())
		{
			bDeepCatch_QCTMode = true;
			MaskInfo* pTrunkMask = m_pVolumeData->findMaskInfo(DEEPCATCH_MASKNAME_QCT_TRUNK);
			if (pTrunkMask)
			{
				pMask3D_QCTTrunk = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(pTrunkMask->uid));
				maskBit_QCTTrunk = m_pVolumeData->getMask(pTrunkMask->uid);
			}
		}
		// deepcatch && io classification 분류.
		else if (THREAD_IO_CLASSIFICATION_PREDICT == m_pActionManager->GetCurrentThread() ||
			THREAD_IO_CLASSIFICATION_PREDICT2 == m_pActionManager->GetCurrentThread())
		{
			bDeepCatch_IOClassificationPredict = true;
			MaskInfo* pIOMask = m_pVolumeData->findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_IO);
			if (pIOMask)
			{
				pMask3D_IO = m_pVolumeData->getMaskDataPoint(m_pVolumeData->GetMaskByteIndex(pIOMask->uid));
				maskBit_IO = m_pVolumeData->getMask(pIOMask->uid);
			}

		}
	}

	bool isSuccess = false;
	for (int i = m_start; i <= m_end; i++)
	{
		memset(maskRaw, 0, sizeof(mint16) * (cx * cy));
		if (bDeepCatch_QCTMode)
		{
			for (int y = 0; y < cy; y++)
			{
				for (int x = 0; x < cx; x++)
				{
					maskRaw[y * cx + x] = -1000; // air value

					if (pMask3D_QCTTrunk && !(pMask3D_QCTTrunk[i * cx * cy + y * cx + x] & maskBit_QCTTrunk))
					{
						continue;
					}

					if (m_BoundingBox.getMinX() <= x && m_BoundingBox.getMaxX() >= x
						&& m_BoundingBox.getMinY() <= y && m_BoundingBox.getMaxY() >= y)
					{
						if (m_nFilterIdx == -1)
						{
							maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, i);
						}
						else if (m_pActionPredictAdd)
						{
							if (m_pActionPredictAdd->m_vecAIresult[0][i * cy * cx + y * cx + x] > AI_MEDIAN_VALUE)
							{
								maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, i);
							}
						}
					}
				}
			}
		}
		else if (bDeepCatch_IOClassificationPredict)
		{
			for (int y = 0; y < cy; y++)
			{
				for (int x = 0; x < cx; x++)
				{
					maskRaw[y * cx + x] = -1000; // air value

					if (pMask3D_IO && !(pMask3D_IO[i * cx * cy + y * cx + x] & maskBit_IO))
						continue;

					if (m_BoundingBox.getMinX() <= x && m_BoundingBox.getMaxX() >= x
						&& m_BoundingBox.getMinY() <= y && m_BoundingBox.getMaxY() >= y)
					{
						if (m_nFilterIdx == -1)
						{
							maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, i);
						}
						else if (m_pActionPredictAdd)
						{
							if (m_pActionPredictAdd->m_vecAIresult[0][i * cy * cx + y * cx + x] > 127)
							{
								maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, i);
							}
						}
					}
				}
			}
		}
		else if (bMEDIP_LungFilteredMode)
		{
			for (int y = 0; y < cy; y++)
			{
				for (int x = 0; x < cx; x++)
				{
					maskRaw[y * cx + x] = -3024;

					if (m_BoundingBox.getMinX() <= x && m_BoundingBox.getMaxX() >= x
						&& m_BoundingBox.getMinY() <= y && m_BoundingBox.getMaxY() >= y)
					{
						if (m_nFilterIdx == -1)
						{
							maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, i);
						}
						else if (m_pActionPredictAdd)
						{
							if (m_pActionPredictAdd->m_vecAIresult[0][i * cy * cx + y * cx + x] > AI_MEDIAN_VALUE)
							{
								maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, i);
							}
						}
					}
				}
			}
		}
		else if (bMEDIP_LungFilteredMode_nnUNet)
		{
			for (int y = 0; y < cy; y++)
			{
				for (int x = 0; x < cx; x++)
				{
					maskRaw[y * cx + x] = -1024;

					if (m_BoundingBox.getMinX() <= x && m_BoundingBox.getMaxX() >= x
						&& m_BoundingBox.getMinY() <= y && m_BoundingBox.getMaxY() >= y)
					{
						if (m_nFilterIdx == -1)
						{
							maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, i);
						}
						else if (m_pActionPredictAdd)
						{
							if (m_pActionPredictAdd->m_vecAIresult[0][i * cy * cx + y * cx + x] > AI_MEDIAN_VALUE)
							{
								maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, i);
							}
						}
					}
				}
			}
		}
		else if (bMEDIP_WholeBodyIOFilteredMode)
		{
			setRawDataWithVolumePlane_WholeBodyIO(maskRaw, m_pVolumeData, cx, cy, i);
		}
		else if (bMEDIP_HeartFilteredMode)
		{
			for (int y = 0; y < cy; y++)
			{
				for (int x = 0; x < cx; x++)
				{
					maskRaw[y * cx + x] = -1024;

					if (m_BoundingBox.getMinX() <= x && m_BoundingBox.getMaxX() >= x
						&& m_BoundingBox.getMinY() <= y && m_BoundingBox.getMaxY() >= y)
					{
						if (m_nFilterIdx == -1)
						{
							maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, i);
						}
						else if (m_pActionPredictAdd)
						{
							if (m_pActionPredictAdd->m_vecAIresult[0][i * cy * cx + y * cx + x] > AI_MEDIAN_VALUE)
							{
								maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, i);
							}
						}
					}
				}
			}
		}
		else
		{
			for (int y = 0; y < cy; y++)
			{
				for (int x = 0; x < cx; x++)
				{
					maskRaw[y * cx + x] = -1000; // air value

					if (m_BoundingBox.getMinX() <= x && m_BoundingBox.getMaxX() >= x
						&& m_BoundingBox.getMinY() <= y && m_BoundingBox.getMaxY() >= y)
					{
						if (m_nFilterIdx == -1)
						{
							maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, i);
						}
						else if (m_pActionPredictAdd)
						{
							if (m_pActionPredictAdd->m_vecAIresult[0][i * cy * cx + y * cx + x] > AI_MEDIAN_VALUE)
							{
								maskRaw[y * cx + x] = m_pVolumeData->getData(x, y, i);
							}
						}
					}
				}
			}
		}

		QString rawName = predictDirDataPath + QString("/deepdraw_slice_%1.raw").arg(i);
		QString tmpName = rawName + ".tmp_";

		QFile file(tmpName);

		if (file.exists())
		{
			file.remove();
		}

		if (!file.open(QIODevice::WriteOnly))
		{
			file.remove();
			emit finished();
			return;
		}

		file.write((const char*)maskRaw, (cx * cy) * sizeof(mint16));
		file.close();

		QFile orgFile(rawName);

		isSuccess = true;

		if (orgFile.exists())
		{
			isSuccess = orgFile.remove();
		}

		if (isSuccess)
		{
			isSuccess = file.rename(rawName);
		}

		if (!isSuccess)
		{
			break;
		}

		setProgressValue(((float)(i) / m_end) * 20);

		std::cout << "mask data save complete : " << rawName.toStdString().c_str() << std::endl;

		if (m_pVolumeData->threadStop)
		{
			SAFE_DELETES(maskRaw);
			emit finished();
			return;
		}

	}//20

	qInfo() << ">>>>>>>>>>>>> predit data start";
	qInfo() << "start" << m_start;
	qInfo() << "end" << m_end;
	qInfo() << "cx" << cx;
	qInfo() << "cy" << cy;
	qInfo() << "cz" << cz;
	qInfo() << "predictDirDataPath" << predictDirDataPath;
	qInfo() << "predictDirPath" << predictDirPath;
	qInfo() << "weightDirPath" << weightDirPath;
	qInfo() << "projectName" << m_projectName;
	qInfo() << "copy mask" << (int)m_copyMaskType;
	qInfo() << ">>>>>>>>>>>>> predit data end";

	if (!isSuccess)
	{
		m_pVolumeData->threadResult = -1;
		SAFE_DELETES(maskRaw);
		emit finished();
		return;
	}

	SAFE_DELETES(maskRaw);

	// salt 값 설정
	int addValue = 20;
	std::vector< std::vector<int> >salt;
	if (m_weightType == MFL_Product_COVID19)
	{
		salt = m_pProductManager->m_mapAI_Salt[(unsigned int)ClientType::COVID19];
	}
	else if (m_weightType == MFL_Product_DeepCatch || m_weightType == MFL_Product_DeepCatch_DeepCatchV2)
	{
		salt = m_pProductManager->m_mapAI_Salt[(unsigned int)ClientType::DEEPCATCH];
	}
	else if (m_weightType == MFL_Common_AI_AIPredict_Predict)
	{
		salt = m_pProductManager->m_mapAI_Salt[(unsigned int)ClientType::MEDIP];
	}
	else if (m_weightType == MFL_Common_AITrainning_CustomTrainningTool)
	{
		salt = m_pProductManager->m_mapAI_Salt[(unsigned int)ClientType::USER_CREATED];
	}
	else if (m_weightType == MFL_Common_AI_PredictUsableCount_Credit)
	{
		// 일단 고정 salt 값, 서버 처리되면 salt 값 받아서 처리, salt값은 개인별로 줘야 할듯 함
		salt = m_pProductManager->m_mapAI_Salt[(unsigned int)ClientType::MEDIP_CREDIT];
	}

	Segmentation::setExtension("mipx");
	std::shared_ptr<Segmentation> pSegment;
	pSegment = std::make_shared<Segmentation>(weightDirPath.toLocal8Bit().toStdString(), m_projectName.toLocal8Bit().toStdString(), true, salt);

	CheckData check;

	int milestone = addValue;

	qInfo() << "run medip predict : " << m_projectName << ", predict dir path : " << predictDirDataPath;
	//if (pSegment->medipPredict(predictDirDataPath.toLocal8Bit().toStdString(), resPath.toLocal8Bit().toStdString() +"/", m_useGPU, cy, cx, cz, m_start)) {
	if (pSegment->medipPredict(predictDirDataPath.toLocal8Bit().toStdString(), "", m_useGPU, cy, cx, cz, m_start, xSpacing, ySpacing, zSpacing))
	{
		while (true)
		{
			if (m_pVolumeData->threadStop)
			{
				pSegment->medipStop();
			}
			check = pSegment->getCheckData();
			if (check.status == Status::NORMAL)
			{
				int progress = milestone + (check.progress * (100 - milestone) / 100);
				if (addValue < progress)
				{
					addValue = progress;//send signal
					setProgressValue(addValue);
				}
			}
			else if (check.status == Status::STOPPING)
			{
			}
			else
			{
				break;
			}
		}

	}
	else
	{
		check = pSegment->getCheckData();
		isSuccess = false;
	}

	if (check.status == Status::EXCEPTION_TERMINATED)
	{
		QString filename = STRING_MANAGER->m_strAppDataLocalPath + "/AI_error.log";
		QFile file(filename);
		if (file.open(QIODevice::ReadWrite))
		{
			QTextStream stream(&file);
			stream << check.exception.c_str() << endl;
		}
		isSuccess = false;
	}

	m_pResult->SetResult(
		pSegment->getClasses(),
		pSegment->getVectorResults(),
		m_weightType,
		m_copyMaskType
	);

	int classResultCount = pSegment->getClasses().size();

	pSegment.reset();

	AIBase::clearCache();
	if (!isSuccess)
	{
		m_pVolumeData->threadResult = -1;
		emit finished();
		return;
	}
	else
	{
		m_pVolumeData->threadResult = classResultCount;
	}

	QDir(predictDirDataPath).removeRecursively();

#endif

	emit finished();
}
