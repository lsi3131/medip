#include "stdafx.h"
#include "ActionThreadEndDeepDrawPredict.h"
#include "WindowManager.h"
#include "graphics/DataContext.h"
#include "ProductManager.h"
#include "Actions/ActionManager.h"
#include "Windows/Tab/AISegTab.h"
#include "Windows/Tab/AISegTabDeepCatch.h"
#include "Windows/Tabwindow.h"
#include "LicenseManager.h"
#include "Actions/ActionMaskList.h"

static const int MAX_THRESHOLD_VALUE = 256;

ActionThreadEndDeepDrawPredict::ActionThreadEndDeepDrawPredict(
	WindowManager* pWinManager,
	DataContext* pDataContext,
	ProductManager* pProductManager,
	LicenseManager* pLicenseManager,
	ActionManager* pActionManager) :
	m_pWinManager(pWinManager),
	m_pDataContext(pDataContext),
	m_pProductManager(pProductManager),
	m_pLicenseManager(pLicenseManager),
	m_pActionManager(pActionManager),
	m_weightType(0),
	m_addedNewMaskCount(0),
	m_pAISegTab(nullptr),
	m_pDeepCatchTab(nullptr),
	m_start(0),
	m_end(0),
	m_threshold(0)
{
}

bool ActionThreadEndDeepDrawPredict::Do()
{
	m_addedNewMaskCount = m_pDataContext->volume_data.threadResult;

	TabWindow* tabWindow = m_pWinManager->GetTab();
	if (tabWindow == nullptr)
	{
		return false;
	}

	m_pAISegTab = tabWindow->getAITab();
	if (m_pAISegTab == nullptr)
	{
		return false;
	}

	if (isDeepCatchProduct())
	{
		m_pDeepCatchTab = dynamic_cast<AISegTabDeepCatch*>(tabWindow->getAITab());
		if (m_pDeepCatchTab == nullptr)
		{
			return false;
		}
	}

	qDebug() << "add new mask count : " << m_addedNewMaskCount;
	if (canAddNewMask())
	{
		return addNewMask();
	}
	else if (m_addedNewMaskCount == -1)
	{
		QString filePath = STRING_MANAGER->m_strAppDataLocalPath + QString("/AI_error.log");
		QFile file(filePath);
		bool isFileExist = file.exists();
		if (isFileExist)
		{
			QString errorLogText;
			if (file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				QTextStream stream(&file);
				errorLogText = stream.readAll();
				file.close();
				//	file.remove();

				QMessageBox::warning(nullptr, "Warning!!", errorLogText);
			}
		}
		else
		{
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0001)).exec();
		}

		enableAIReletedUI();
		return false;
	}
	else if (m_addedNewMaskCount == -2)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN),
			QString("AI program does not exist. Please contact the person in charge."));

		enableAIReletedUI();
		return false;
	}
	else
	{
		enableAIReletedUI();
		return false;
	}
}

bool ActionThreadEndDeepDrawPredict::canAddNewMask() const
{
	return (m_pDataContext->volume_data.threadStop == false) && (m_addedNewMaskCount > 0);
}

bool ActionThreadEndDeepDrawPredict::addNewMask()
{
	if (isNewMaskExceedCount())
	{
		handleExceedMaxMask();
	}
	else
	{
		handleAddNewMask();
	}

	return true;
}

bool ActionThreadEndDeepDrawPredict::isNewMaskExceedCount() const
{
	int maskListCount = m_addedNewMaskCount + m_pDataContext->volume_data.getMaskInfoListCnt();
	return maskListCount > MASK_MAX;
}

void ActionThreadEndDeepDrawPredict::handleExceedMaxMask()
{
	if (isMEDIPProduct())
	{
		m_pActionManager->ClearReservationWorkList();
		m_pWinManager->FreezeProject_InAISegProcessing(false);
	}
	else if (isDeepCatchProduct())
	{
		// jhc [2022.01.07] - DeepCatch이고 macro모드인데 생성할 수 있는 마스크 갯수가 모자른 경우 다이얼로그 팝업 생략하고 해당 오류 기록 처리하고 다음 Macro 진행되도록 수정.
		if (m_pActionManager->m_IsMacroMode)
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
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1002)).exec();
			m_pDeepCatchTab->FreezeDeepCatchProject(false);
		}
	}
}

void ActionThreadEndDeepDrawPredict::handleAddNewMask()
{
	// result copy
	auto sharedName = std::static_pointer_cast<std::vector<std::string>>(m_pActionManager->m_hashThreadResult[std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_name")]);
	m_vecAIName = std::move(*sharedName);
	sharedName.reset();
	m_pActionManager->m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_name"));

	auto sharedResult = std::static_pointer_cast<std::vector<std::vector<unsigned char>>>(m_pActionManager->m_hashThreadResult[std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_result")]);
	m_vecAIResult = std::move(*sharedResult);
	sharedResult.reset();
	m_pActionManager->m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("AI_result"));

	auto sharedWeightType = std::static_pointer_cast<int>(m_pActionManager->m_hashThreadResult[std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("WeightType")]);
	m_weightType = *sharedWeightType;
	sharedWeightType.reset();
	m_pActionManager->m_hashThreadResult.erase(std::to_string((int)ACTP_DEEP_DRAW_PREDICT) + std::string("WeightType"));

	if (isDeepCatchProduct())
	{
		handleAddNewMask_DeepCatch();
	}
	else
	{
		handleAddNewMask_MEDIP();
	}
}

bool ActionThreadEndDeepDrawPredict::isDeepCatchProduct() const
{
	return
		(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH) == 0) ||
		(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_DEEPCATCH_V2) == 0);
}

bool ActionThreadEndDeepDrawPredict::isMEDIPProduct() const
{
	return
		(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(PRODUCT_NAME_MEDIP) == 0);
}

void ActionThreadEndDeepDrawPredict::handleAddNewMask_DeepCatch()
{
	// qct 처리 후 
	if (THREAD_DEEPCATCH_DEEPDRAW_PREDICT == m_pActionManager->GetAfterThread())
	{
		// qct trunk mask 삭제.
		std::vector<muint32> deleteMaskIndexList;
		MaskInfo* pMaskInfo = nullptr;
		for (int i = 0; i < m_pDataContext->volume_data.getMaskInfoListCnt(); ++i)
		{
			pMaskInfo = m_pDataContext->volume_data.getMaskInfo(i, false);
			QString maskName = QString::fromWCharArray(pMaskInfo->maskName);
			if (maskName.compare(DEEPCATCH_MASKNAME_QCT_TRUNK) == 0)
			{
				deleteMaskIndexList.push_back(i);
			}
		}

		if (!deleteMaskIndexList.empty())
		{
			//	action_MaskList_del_list(indeces, true);
			bool isDeleteAll = true;
			ActionMaskListDels maskListDels(&m_pDataContext->volume_data, deleteMaskIndexList, isDeleteAll);
			maskListDels.Do();
		}
	}

	// trunk predict인 경우
	if (isTrunkPredict())
	{
		handleDeepCatch_AddPredict_Trunk();
	}
	else
	{
		handleDeepCatch_AddPredict();
	}
}

void ActionThreadEndDeepDrawPredict::handleAddNewMask_MEDIP()
{
	m_pAISegTab->GetDepth(&m_start, &m_end);
	m_threshold = m_pAISegTab->getOutVal();

	bool isGMMAutoApply = false;
	if (m_pAISegTab->m_tabSet)
	{
		isGMMAutoApply = m_pAISegTab->m_tabSet->isGMMAutoApplyChecked();
	}

	ActionPredictAddEx* pActionAdd = new ActionPredictAddEx(
		&m_pDataContext->volume_data, m_pProductManager, m_start, m_end, m_addedNewMaskCount, m_threshold, m_vecAIName, m_vecAIResult, isGMMAutoApply,
		m_weightType);

	m_pActionManager->AddReservationWork(pActionAdd);

	std::vector<QUndoCommand*> reservationWorkList = m_pActionManager->GetReservationWorkList();

	eMedipAICompleteWorkReturnType aiCompleteResult = m_pAISegTab->CompleteWork();
	if (aiCompleteResult == eMACWRTSuccess)
	{
		std::list<std::vector<MaskInfo>> generatedNewMaskInfos;
		for (int i = reservationWorkList.size() - 1; i >= 0; --i)
		{
			((ActionPredictAddEx*)(reservationWorkList[i]))->redo();
			m_pActionManager->m_newGenerateAIMaskInfos = ((ActionPredictAddEx*)(reservationWorkList[i]))->getNewMaskInfo();

			generatedNewMaskInfos.push_front(((ActionPredictAddEx*)(reservationWorkList[i]))->getNewMaskInfo());
		}

		m_pActionManager->ClearReservationWorkList();
		m_pWinManager->FreezeProject_InAISegProcessing(false);

		// lung 필터 적용 weight인 경우 Lung 2차 Predict 후 lung 마스크와 intersect 처리 추가.
		// - CT_Cavity3D, CT_COVID19, CT_Lung Fissure Lobe3D, CT_Lung Lesion, CT_BO2D_Addin, CT_BO3D_Addin
		if (m_weightType == MFL_Common_AI_PredictUsableCount_Credit)
		{
			auto sharedProject = std::static_pointer_cast<std::string>(m_pActionManager->m_qThreadNext.front().pTempData);
			std::string projectName = *sharedProject;
			QString strProjectName = QString::fromStdString(projectName);
			qDebug() << "projectName :" << QString::fromStdString(projectName);

			if (
				!strProjectName.compare("CT_Cavity3D") || !strProjectName.compare("CT_COVID19") || !strProjectName.compare("CT_Lung Fissure, Lobe3D") ||
				!strProjectName.compare("CT_Lung Lesion") || !strProjectName.compare("CT_BO2D_Addin") || !strProjectName.compare("CT_BO3D_Addin") ||
				!strProjectName.compare("CT_BO2DNN_Addin") || !strProjectName.compare("CT_BO3DNN_Addin") || !strProjectName.compare("CT_COVID19_3DNN"))
			{
				if (!generatedNewMaskInfos.empty() && generatedNewMaskInfos.size() == 2)
				{
					MaskInfo* lungMaskInfo = m_pDataContext->volume_data.getMaskInfo(generatedNewMaskInfos.front().at(0).uid, true);
					MaskInfo* curMaskInfo = m_pDataContext->volume_data.getMaskInfo(generatedNewMaskInfos.back().at(0).uid, true);
					if (curMaskInfo && lungMaskInfo)
					{
						ActionMaskListIntersection intersect(&m_pDataContext->volume_data, *curMaskInfo, *lungMaskInfo);
						intersect.redo();
					}
				}
			}
		}
	}
	else if (aiCompleteResult == eMACWRTPredictCountErr)
	{
		m_pActionManager->threadQueueClear();
		m_pActionManager->ClearReservationWorkList();

		m_pWinManager->FreezeProject_InAISegProcessing(false);
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1003)).exec();
	}
}

bool ActionThreadEndDeepDrawPredict::isTrunkPredict() const
{
	return
		(THREAD_TRUNK_PREDICT == m_pActionManager->GetAfterThread()) ||
		(THREAD_QCT_PREDICT == m_pActionManager->GetAfterThread()) ||
		(THREAD_IO_CLASSIFICATION_PREDICT == m_pActionManager->GetAfterThread()) ||
		(THREAD_IO_CLASSIFICATION_PREDICT2 == m_pActionManager->GetAfterThread());
}

void ActionThreadEndDeepDrawPredict::handleDeepCatch_AddPredict()
{
	if (m_pDeepCatchTab->CompleteWork() == false)
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
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1003)).exec();
			m_pActionManager->threadQueueClear();
			m_pDeepCatchTab->FreezeDeepCatchProject(false);
		}
		return;
	}

	handleDeepCatch_CompleteWork();
}

void ActionThreadEndDeepDrawPredict::handleDeepCatch_CompleteWork()
{
	if (m_pDeepCatchTab->IsProjectModule(DTP_WHOLEBODY_ABDOMEN))
	{
		if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
		{
			handleDeepCatch_CompleteWork_Report_V2();
		}
		else
		{
			handleDeepCatch_CompleteWork_Report();
		}
	}
	else
	{
		m_pDeepCatchTab->FreezeDeepCatchProject(false);
	}

	m_pAISegTab->GetDepth(&m_start, &m_end);
	m_threshold = MAX_THRESHOLD_VALUE / 2;

	bool isGMMAutoApply = false;
	if (m_pAISegTab->m_tabSet)
	{
		isGMMAutoApply = m_pAISegTab->m_tabSet->isGMMAutoApplyChecked();
	}

	ActionPredictAddEx predictAdd(&m_pDataContext->volume_data, m_pProductManager, m_start, m_end, m_addedNewMaskCount, m_threshold, m_vecAIName, m_vecAIResult, isGMMAutoApply);
	predictAdd.Do();

	m_pWinManager->SetPredictComplete(true);
	m_pWinManager->SetEnableViewControls(m_pWinManager->IsPredictComplete());
}

void ActionThreadEndDeepDrawPredict::handleDeepCatch_CompleteWork_Report_V2()
{
	DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->getDeepCatch_PredictedInfo();
	QString singleSliceStr = DEEPCATCH_WHOLEBODY_MASKNAME_L3;

	if (pPredictedInfo->stPredictOpt.useVBNetwork == eUseVertebraNetwork &&
		pPredictedInfo->stPredictOpt.singleSliceNum != eDCVB_L3)
	{
		int singleSliceNum = m_pDeepCatchTab->getSingleSliceNum();
		singleSliceStr = m_pDeepCatchTab->getVBString(singleSliceNum);
	}

	// create, single slice mask
	// set single mask
	m_pDataContext->volume_data.createMaskInfo();
	MaskInfo* pNewMaskInfo = m_pDataContext->volume_data.getCurrentMaskInfo();
	m_pDataContext->volume_data.setMaskName(singleSliceStr, pNewMaskInfo->uid, true);

	pNewMaskInfo->color = COLOR(115, 255, 64, 255);
	pNewMaskInfo->layerAlpha = 127;

	if (pPredictedInfo)
	{
		pPredictedInfo->mapDeepCatchPredictResultRoi.insert(singleSliceStr, pNewMaskInfo->uid);
	}

	// set multi mask
	QString multiSliceText = DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST;

	multiSliceText = m_pDeepCatchTab->getMultiSliceUpLowName();

	m_pDataContext->volume_data.createMaskInfo();
	MaskInfo* pNewMaskInfo_multi = m_pDataContext->volume_data.getCurrentMaskInfo();
	m_pDataContext->volume_data.setMaskName(multiSliceText, pNewMaskInfo_multi->uid, true);

	pNewMaskInfo_multi->color = COLOR(100, 149, 237, 255);
	pNewMaskInfo_multi->layerAlpha = 127;

	if (pPredictedInfo)
	{
		pPredictedInfo->mapDeepCatchPredictResultRoi.insert(multiSliceText, pNewMaskInfo_multi->uid);
	}
}

void ActionThreadEndDeepDrawPredict::handleDeepCatch_CompleteWork_Report()
{
	DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->getDeepCatch_PredictedInfo();

	// set L3 mask
	m_pDataContext->volume_data.createMaskInfo();
	MaskInfo* pNewMaskInfo = m_pDataContext->volume_data.getCurrentMaskInfo();
	m_pDataContext->volume_data.setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_L3, pNewMaskInfo->uid, true);

	pNewMaskInfo->color = COLOR(115, 255, 64, 255);
	pNewMaskInfo->layerAlpha = 127;

	if (pPredictedInfo)
	{
		pPredictedInfo->mapDeepCatchPredictResultRoi.insert(DEEPCATCH_WHOLEBODY_MASKNAME_L3, pNewMaskInfo->uid);
	}

	// create, set Abdominal waist mask
	m_pDataContext->volume_data.createMaskInfo();
	pNewMaskInfo = m_pDataContext->volume_data.getCurrentMaskInfo();
	m_pDataContext->volume_data.setMaskName(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST, pNewMaskInfo->uid, true);
	pNewMaskInfo->color = COLOR(76, 87, 243, 255);
	pNewMaskInfo->layerAlpha = 127;

	if (pPredictedInfo)
	{
		pPredictedInfo->mapDeepCatchPredictResultRoi.insert(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST, pNewMaskInfo->uid);
	}
}

void ActionThreadEndDeepDrawPredict::enableAIReletedUI()
{
	if (isDeepCatchProduct())
	{
		std::deque<ActionThreadArgument> empty;
		std::swap(m_pActionManager->m_qThreadNext, empty);

		m_pDeepCatchTab->FreezeDeepCatchProject(false);
	}
	else
	{
		m_pWinManager->FreezeProject_InAISegProcessing(false);
	}
}

void ActionThreadEndDeepDrawPredict::handleDeepCatch_AddPredict_Trunk()
{
	m_pAISegTab->GetDepth(&m_start, &m_end);
	m_threshold = MAX_THRESHOLD_VALUE / 2;

	bool isGMMAutoApply = false;
	if (m_pAISegTab->m_tabSet)
	{
		isGMMAutoApply = m_pAISegTab->m_tabSet->isGMMAutoApplyChecked();
	}

	ActionPredictAddEx actionPredictAdd(
		&m_pDataContext->volume_data, m_pProductManager, m_start, m_end, m_addedNewMaskCount, m_threshold, m_vecAIName, m_vecAIResult, isGMMAutoApply
	);
	actionPredictAdd.Do();

	m_pDeepCatchTab->FreezeDeepCatchProject(false);
}
