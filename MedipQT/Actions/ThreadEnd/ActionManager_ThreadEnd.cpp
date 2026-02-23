#include "stdafx.h"
#include "ActionManager_ThreadEnd.h"
#include "WindowManager.h"
#include "Tabwindow.h"
#include "Tab/AISegTabDeepCatch.h"
#include "LicenseManager.h"
#include "ActionMaskList.h"
#include "Actions/AI/ActionPredictAddEx.h"
#include "Actions/AI/WorkDeepPredictResult.h"

ActionManager_ThreadEnd::ActionManager_ThreadEnd(VOLUME_DATA* pVolumeData, ActionManager* pActionManager, ProductManager* pProductManager, AISegTab* aiSegTab) :
	m_pVolumeData(pVolumeData),
	m_pActionManager(pActionManager),
	m_pProductManager(pProductManager),
	m_aiSegTab(aiSegTab),
	m_start(0),
	m_end(0),
	m_threshold(0),
	m_result(0)
{
	if (m_aiSegTab)
	{
		m_aiSubSetTab = m_aiSegTab->m_tabSet;
	}
}

bool ActionManager_ThreadEnd::Handle(ActionManagerThreadEndResult* pOutResult)
{
	m_result = m_pVolumeData->threadResult;
	qDebug() << "result count : " << m_result;


	if (m_result == -1)
	{
		QString errorMessage = readAIErrorLogFile();
		if (errorMessage.isEmpty())
		{
			errorMessage = STRING_MANAGER->getString(ERR_DH_0001);
		}
		clearResult(pOutResult, errorMessage);

		return false;
	}
	else if (m_result == -2)
	{
		QString errorMessage = QString("AI program does not exist. Please contact the person in charge.");
		clearResult(pOutResult, errorMessage);

		return false;
	}

	if (m_pVolumeData->threadStop)
	{
		clearResult(pOutResult, "");
		return false;
	}

	if (m_aiSegTab == nullptr)
	{
		clearResult(pOutResult, "");
		return false;
	}

	if (initParameterFromActionHashMap() == false)
	{
		clearResult(pOutResult, "");
		return false;
	}

	int newAIResultCount = m_result;
	if (newAIResultCount <= 0)
	{
		clearResult(pOutResult, "");

		return false;
	}

	if (isExceedMaxMaskCount(newAIResultCount))
	{
		handle_ExceedMaskCount(pOutResult);
		return false;
	}

	if (m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY) == PRODUCT_NAME_DEEPCATCH)
	{
		handle_DeepCatch(pOutResult);
	}
	else
	{
		handle_MEDIP(pOutResult);
	}

	return true;
}

bool ActionManager_ThreadEnd::initParameterFromActionHashMap()
{
	WorkDeepPredictResult result(&m_pActionManager->m_hashThreadResult);

	AIClassNameList* pAINameList = result.GetClassNameList();
	AIClassResultList* pAIResultList = result.GetClassResultList();

	if (pAINameList == nullptr || pAIResultList == nullptr)
	{
		return false;
	}

	m_AINameList = *pAINameList;
	m_AIResultList = *pAIResultList;
	m_weightType = result.GetWeightType();

	m_pActionManager->m_hashThreadResult.erase(result.KeyAIName());
	m_pActionManager->m_hashThreadResult.erase(result.KeyAIResult());
	m_pActionManager->m_hashThreadResult.erase(result.KeyWeightType());

	return true;
}

bool ActionManager_ThreadEnd::isExceedMaxMaskCount(int newAIResultCount)
{
	return (newAIResultCount + m_pVolumeData->getMaskInfoListCnt()) > MASK_MAX;
}

void ActionManager_ThreadEnd::handle_ExceedMaskCount(ActionManagerThreadEndResult* pOutResult)
{
	if (LICENSE_DATA->getProductType() == PRODUCT_NAME_MEDIP)
	{
		m_pActionManager->ClearReservationWorkList();
		pOutResult->IsFreezeProject = false;
	}

	// jhc [2022.01.07] - DeepCatch이고 macro모드인데 생성할 수 있는 마스크 갯수가 모자른 경우 다이얼로그 팝업 생략하고 해당 오류 기록 처리하고 다음 Macro 진행되도록 수정.
	if (m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY) == PRODUCT_NAME_DEEPCATCH && m_pActionManager->m_IsMacroMode)
	{
		m_pActionManager->MacroErrorRecord(m_pActionManager->m_ListMacroCommandLine.first(), STRING_MANAGER->getString(ERR_DU_1002));
		while (m_pActionManager->m_qThreadNext.front().eNextThread != THREAD_DEEPCATCH_NEXT_LOOP)
		{
			m_pActionManager->m_qThreadNext.pop_front();
			--m_pActionManager->m_nCurrentThreadCount;
		}
	}
	else
	{
		pOutResult->ErrorMessage = STRING_MANAGER->getString(ERR_DU_1002);
	}
}

bool ActionManager_ThreadEnd::handle_DeepCatch(ActionManagerThreadEndResult* pOutResult)
{
	AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)m_aiSegTab;

	// qct 처리 후 
	if (THREAD_DEEPCATCH_DEEPDRAW_PREDICT == m_pActionManager->GetCurrentThread())
	{
		// qct trunk mask 삭제.
		std::vector<muint32> indeces;
		MaskInfo* pMaskInfo = nullptr;
		for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
		{
			pMaskInfo = m_pVolumeData->getMaskInfo(i, false);
			QString maskName = QString::fromWCharArray(pMaskInfo->maskName);
			if (!maskName.compare(DEEPCATCH_MASKNAME_QCT_TRUNK))
			{
				indeces.push_back(i);
			}
		}
		if (!indeces.empty())
		{
			//	m_pActionManager->action_MaskList_del_list(indeces, true);
			ActionMaskListDels maskListDels(m_pVolumeData, indeces, true);
			maskListDels.Do();
		}
	}

	// trunk predict인 경우
	if (
		THREAD_TRUNK_PREDICT == m_pActionManager->GetAfterThread() ||
		THREAD_QCT_PREDICT == m_pActionManager->GetAfterThread() ||
		THREAD_IO_CLASSIFICATION_PREDICT == m_pActionManager->GetAfterThread() ||
		THREAD_IO_CLASSIFICATION_PREDICT2 == m_pActionManager->GetAfterThread()
		)
	{
		if (m_aiSegTab)
		{
			m_threshold = 256 / 2;
			m_aiSegTab->GetDepth(&m_start, &m_end);

			ActionPredictAddEx predictAdd(
				m_pVolumeData,
				m_pProductManager,
				m_start,
				m_end,
				m_result,
				m_threshold,
				m_AINameList,
				m_AIResultList,
				isGMMAutoApply()
			);
			predictAdd.Do();
		}

		pOutResult->IsFreezeDeepCatchProject = false;
	}
	// DeepCatch predict시
	else
	{
		if (pDeepCatchtab->CompleteWork())
		{
#ifdef SUPPORT_DEEPCATCH_VB_NETWORK
			if (pDeepCatchtab->IsProjectModule(DTP_WHOLEBODY_ABDOMEN) || pDeepCatchtab->IsProjectModule(DTP_EXTRACT_VERTEBRA))
#else
			if (pDeepCatchtab->IsProjectModule(DTP_WHOLEBODY_ABDOMEN))
#endif
			{
				// Report data 생성
				DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->getDeepCatch_PredictedInfo();

				// set L3 mask
				m_pVolumeData->createMaskInfo();
				MaskInfo* pNewMaskInfo = m_pVolumeData->getCurrentMaskInfo();

#ifdef SUPPORT_DEEPCATCH_VB_NETWORK
				if (pDeepCatchtab->IsProjectModule(DTP_EXTRACT_VERTEBRA))
				{
					m_pVolumeData->setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_L3_SLICE, pNewMaskInfo->uid, true);
				}
				else
					m_pVolumeData->setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_L3, pNewMaskInfo->uid, true);
#else
				m_pVolumeData->setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_L3, pNewMaskInfo->uid, true);
#endif

				pNewMaskInfo->color = COLOR(115, 255, 64, 255);
				pNewMaskInfo->layerAlpha = 127;

				if (pPredictedInfo)
				{
					pPredictedInfo->mapDeepCatchPredictResultRoi.insert(DEEPCATCH_WHOLEBODY_MASKNAME_L3, pNewMaskInfo->uid);
				}

				// create, set Abdominal waist mask
				m_pVolumeData->createMaskInfo();
				pNewMaskInfo = m_pVolumeData->getCurrentMaskInfo();
				m_pVolumeData->setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST, pNewMaskInfo->uid, true);
				pNewMaskInfo->color = COLOR(76, 87, 243, 255);
				pNewMaskInfo->layerAlpha = 127;

				if (pPredictedInfo)
				{
					pPredictedInfo->mapDeepCatchPredictResultRoi.insert(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST, pNewMaskInfo->uid);
				}
			}
			else
			{
				pOutResult->IsFreezeDeepCatchProject = false;
			}

			if (m_aiSegTab)
			{
				m_aiSegTab->GetDepth(&m_start, &m_end);
				m_threshold = 256 / 2;

				ActionPredictAddEx predictAdd(
					m_pVolumeData,
					m_pProductManager,
					m_start,
					m_end,
					m_result,
					m_threshold,
					m_AINameList,
					m_AIResultList,
					isGMMAutoApply()
				);
				predictAdd.Do();
			}

			pOutResult->IsPredictComplete = true;
		}
		else
		{
			// jhc [2022.01.05] - DeepCatch이고 macro모드인데 Credit 감소 실패한 경우 다이얼로그 팝업 생략하고 해당 오류 기록 처리하고 다음 Macro 진행되도록 수정.
			if (m_pActionManager->m_IsMacroMode)
			{
				m_pActionManager->MacroErrorRecord(m_pActionManager->m_ListMacroCommandLine.first(), STRING_MANAGER->getString(ERR_DU_1003));
				while (m_pActionManager->m_qThreadNext.front().eNextThread != THREAD_DEEPCATCH_NEXT_LOOP)
				{
					m_pActionManager->m_qThreadNext.pop_front();
					--m_pActionManager->m_nCurrentThreadCount;
				}
			}
			else
			{
				pOutResult->ErrorMessage = STRING_MANAGER->getString(ERR_DU_1003);
				pOutResult->IsFreezeDeepCatchProject = false;
				m_pActionManager->threadQueueClear();
			}
		}
	}
	return true;
}

bool ActionManager_ThreadEnd::handle_MEDIP(ActionManagerThreadEndResult* pOutResult)
{
	m_aiSegTab->GetDepth(&m_start, &m_end);
	m_threshold = m_aiSegTab->getOutVal();

	ActionPredictAddEx* actionPredictAdd = new ActionPredictAddEx(
		m_pVolumeData,
		m_pProductManager,
		m_start,
		m_end,
		m_result,
		m_threshold,
		m_AINameList,
		m_AIResultList,
		isGMMAutoApply(),
		m_weightType);

	m_pActionManager->AddReservationWork(actionPredictAdd);

	eMedipAICompleteWorkReturnType returnValue = m_aiSegTab->CompleteWork();
	if (returnValue == eMACWRTSuccess)
	{
		pOutResult->IsFreezeProject = false;

		std::list<std::vector<MaskInfo>> generatedNewMaskInfos;
		std::vector<QUndoCommand*> reservationWorkList = m_pActionManager->GetReservationWorkList();

		for (int i = reservationWorkList.size() - 1; i >= 0; --i)
		{
			((ActionPredictAddEx*)(reservationWorkList[i]))->redo();
			generatedNewMaskInfos.push_front(((ActionPredictAddEx*)(reservationWorkList[i]))->getNewMaskInfo());
		}

		m_pActionManager->ClearReservationWorkList();

		// lung 필터 적용 weight인 경우 Lung 2차 Predict 후 lung 마스크와 intersect 처리 추가.
		// - CT_Cavity3D, CT_COVID19, CT_Lung Fissure Lobe3D, CT_Lung Lesion, CT_BO2D_Addin, CT_BO3D_Addin
		if (m_weightType == MFL_Common_AI_PredictUsableCount_Credit)
		{
			if (!m_pActionManager->m_qThreadNext.empty())
			{
				auto sharedProject = std::static_pointer_cast<std::string>(m_pActionManager->m_qThreadNext.front().pTempData);
				std::string projectName = *sharedProject;
				QString strProjectName = QString::fromStdString(projectName);
				qDebug() << "projectName :" << QString::fromStdString(projectName);

				if (!strProjectName.compare("CT_Cavity3D") || !strProjectName.compare("CT_COVID19") || !strProjectName.compare("CT_Lung Fissure, Lobe3D")
					|| !strProjectName.compare("CT_Lung Lesion") || !strProjectName.compare("CT_BO2D_Addin") || !strProjectName.compare("CT_BO3D_Addin")
					|| !strProjectName.compare("CT_BO2DNN_Addin") || !strProjectName.compare("CT_BO3DNN_Addin") || !strProjectName.compare("CT_COVID19_3DNN"))
				{
					if (!generatedNewMaskInfos.empty() && generatedNewMaskInfos.size() == 2)
					{
						MaskInfo* lungMaskInfo = m_pVolumeData->getMaskInfo(generatedNewMaskInfos.front().at(0).uid, true);
						MaskInfo* curMaskInfo = m_pVolumeData->getMaskInfo(generatedNewMaskInfos.back().at(0).uid, true);
						if (curMaskInfo && lungMaskInfo)
						{
							ActionMaskListIntersection intersect(m_pVolumeData, *curMaskInfo, *lungMaskInfo);
							intersect.redo();
						}
					}
				}
			}
		}
	}
	else if (returnValue == eMACWRTPredictCountErr)
	{
		m_pActionManager->threadQueueClear();
		m_pActionManager->ClearReservationWorkList();

		pOutResult->IsFreezeProject = false;
		pOutResult->ErrorMessage = STRING_MANAGER->getString(ERR_DU_1003);
	}
	else if (returnValue == eMACWRTNextPredict)
	{
		//Skip
	}

	return true;
}

void ActionManager_ThreadEnd::clearResult(ActionManagerThreadEndResult* pOutResult, const QString& errorMessage)
{
	if (m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY) == PRODUCT_NAME_DEEPCATCH)
	{
		std::deque<ActionThreadArgument> empty;
		std::swap(m_pActionManager->m_qThreadNext, empty);

		pOutResult->IsFreezeDeepCatchProject = false;
	}
	else
	{
		pOutResult->IsFreezeProject = false;
	}

	pOutResult->ErrorMessage = errorMessage;
}

QString ActionManager_ThreadEnd::readAIErrorLogFile()
{
	QString filePath = STRING_MANAGER->m_strAppDataLocalPath + QString("/AI_error.log");
	QFile file(filePath);
	QString errorLogMessage;
	if (file.exists())
	{
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream stream(&file);
			errorLogMessage = stream.readAll();
			file.close();
		}
	}

	return errorLogMessage;
}

bool ActionManager_ThreadEnd::isGMMAutoApply() const
{
	if (m_aiSubSetTab == nullptr)
	{
		return false;
	}

	return m_aiSubSetTab->isGMMAutoApplyChecked();
}

