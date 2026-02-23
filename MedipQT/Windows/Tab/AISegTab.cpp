#include "stdafx.h"
#include "AISegTab.h"
#include "volumedata.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "StyleManager.h"
#include "LicenseManager.h"
#include "Renderer/Renderer.h"
#include "Dialogs/ProjectDialog.h"
#include "FileManager.h"
#include "Tabwindow.h"
#include "ROITab2.h"
#include "Main/MainSegmentWidget.h"
#include "DeepInsthink.h"

AISegTab::AISegTab(
	VOLUME_DATA* pVolumeData,
	WindowManager* pWinManager,
	ActionManager* pActionManager,
	ProductManager* pProductManager,
	QWidget* parent) :
	CollapseWidget(QString(), parent),
	m_pVolumeData(pVolumeData),
	m_pWinManager(pWinManager),
	m_pActionManager(pActionManager),
	m_pProductManager(pProductManager),
	m_cboProject(nullptr),
	m_tabSet(nullptr)
{
	m_pProductFactory = m_pProductManager->getFactory();
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch) == false)
	{
		initWidget_MEDIP();

		m_strWeightPath = STRING_MANAGER->AISegmentationPath + "/weight";
		m_strInputPath = STRING_MANAGER->LocalAISegPath + "/predict";
	}

	QString title = "AI Segmentation";
	setTitle(title);

	setOpenWidget(false);
}

int AISegTab::getAIType()
{
	if (m_cboProject)
	{
		return m_cboProject->currentIndex();
	}
	return 0;
}

eDeepPredictAICopyMask AISegTab::GetCopyMaskType() const
{
	if (m_chkCopyMask->isChecked())
	{
		return eDeepPredictAICopyMask::copy;
	}
	else
	{
		return eDeepPredictAICopyMask::none_copy;
	}
}

void AISegTab::SetAIWeightType(eMEDIP_FUNCTION_LEVEL weightType)
{
	m_eWeightType = weightType;
}

bool AISegTab::IsHeatMapMode()
{
	if (m_tabSet)
	{
		return m_tabSet->isHeatMapChecked();
	}

	return false;
}

bool AISegTab::IsHeatMapEnabled()
{
	if (m_tabSet)
	{
		return m_tabSet->isHeatMapEnabled();
	}

	return false;
}

void AISegTab::SetHeatMapMode(bool bCheck)
{
	if (m_tabSet)
	{
		m_tabSet->setHeatMapChecked(bCheck);
	}
}

bool AISegTab::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == nullptr)
	{
		return QWidget::eventFilter(watched, event);
	}

	if (watched->objectName().contains("Slider"))
	{
		if (event->type() == QEvent::Scroll ||
			event->type() == QEvent::Wheel)
		{
			return true;
		}
	}

	if (watched->objectName().contains("Edit"))
	{
		if (event->type() == QEvent::FocusIn)
		{
			m_pWinManager->setMoveFocus(false);
		}
	}

	return QWidget::eventFilter(watched, event);
}

void AISegTab::initWidget_MEDIP()
{
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_MEDIP_Plugin_AIPack) == false)
	{
		m_labelMsg = new QLabel(this);
		m_labelMsg->setTextFormat(Qt::RichText);
		m_labelMsg->setTextInteractionFlags(Qt::TextBrowserInteraction);
		m_labelMsg->setOpenExternalLinks(true);
		m_labelMsg->setText(STRING_MANAGER->getString(STR_NONE_DEEPDRAW_PACK));
		m_labelMsg->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		addWidget(m_labelMsg, m_nRow++, 0, QMargins(0, 5, 0, 5));
	}

	m_labelDeepDraw = new QLabel(this);
	m_labelDeepDraw->setText("Project");
	addWidget(m_labelDeepDraw, m_nRow, 4, QMargins(0, 5, 0, 5));

	m_labelCredit = new QLabel(this);
	m_labelCredit->setText("Credit");
	m_labelCredit->setStyleSheet(QString("color:yellow;"));
	addWidget(m_labelCredit, m_nRow++, 1);

	m_cboProject = new QComboBox(this);

	if (m_cboProject)
	{
		m_cboProject->setStyleSheet(STYLE_MANAGER->comboBoxTab);
		m_cboProject->installEventFilter(this);

		addWidget(m_cboProject, m_nRow, 4);

		connect(m_cboProject, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnProjectChanged(int)));

		m_LineEditAvailableCount = new QLineEdit(this);
		m_LineEditAvailableCount->setStyleSheet(STYLE_MANAGER->editBoxTab + QString("QLineEdit{color:yellow;}"));
		m_LineEditAvailableCount->setReadOnly(true);
		m_LineEditAvailableCount->setAlignment(Qt::AlignCenter);
		m_LineEditAvailableCount->setText(QString("N/A"));
		addWidget(m_LineEditAvailableCount, m_nRow++, 1);

		LoadProject(true);
		UpdateProductCombo(0);
	}

#ifdef SUPPORT_HEART_SOLUTION
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_DEV_AIheartsolution))
	{
		if (!m_spinCluster)
		{
			QLabel* label = new QLabel("cluster : ", this);
			label->setStyleSheet(STYLE_MANAGER->labelNormal);
			label->setFixedSize(QSize(50, 20));
			addWidget(label, m_nRow, 1);

			m_spinCluster = new QSpinBox(this);
			m_spinCluster->setStyleSheet(STYLE_MANAGER->spinbox);
			m_spinCluster->setMinimum(2);
			m_spinCluster->setMaximum(3);
			m_spinCluster->setAlignment(Qt::AlignCenter);

			QSize size = m_spinCluster->size();
			m_spinCluster->setFixedSize(QSize(40, size.height()));

			addWidget(m_spinCluster, m_nRow, 1);
		}
	}
#endif

	//if (m_pProductManager->IsAvailableFunctionLevel(MFL_PLUGIN_DEEPDRAW_PACK))			
	{
		m_btnPredict = new QPushButton(this);
		m_chkCopyMask = new QCheckBox(this);
	}

	if (m_btnPredict && m_chkCopyMask)
	{
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Product_MEDIP_Plugin_AIPack))
		{
			m_btnPredict->setEnabled(false);
			m_chkCopyMask->setEnabled(false);
		}
		m_btnPredict->setText("Predict");
		m_btnPredict->setStyleSheet(STYLE_MANAGER->buttonBehind
			+ QString("QPushButton{min-width:%1px;}").arg(m_btnPredict->text().size() * 7));

		m_chkCopyMask->setText("Copy Mask");

		addWidget(m_btnPredict, m_nRow, 1);
		addWidget(m_chkCopyMask, m_nRow++, 0);

		connect(m_btnPredict, &QPushButton::clicked, this, &AISegTab::slot_OnPredict);
	}

#ifdef DEV_VER
	//component button
	{
		QHBoxLayout* hLay = new QHBoxLayout;
		// 		grpLay->addLayout(hLay);

		QGroupBox* pComponent = nullptr;
		pComponent = m_pProductFactory->createWidget<QGroupBox>(MFL_PLUGIN_AI_COMPONENT, this); //new QGroupBox(this);
		if (pComponent)
		{
			pComponent->setTitle("Component");
			//hLay->addWidget(pComponent, 1);
			addWidget(pComponent, m_nRow++, 1);

			hLay = new QHBoxLayout;
			pComponent->setLayout(hLay);


			QLabel* label = new QLabel(this);
			label->setText("No. of Components");

			hLay->addWidget(label);


			m_editComponent = new QLineEdit(this);
			m_editComponent->setValidator(new QIntValidator(this));
			m_editComponent->setStyleSheet(STYLE_MANAGER->editBoxTab);
			m_editComponent->setObjectName("EditComponent");
			m_editComponent->setMouseTracking(true);
			m_editComponent->installEventFilter(this);
			m_editComponent->setReadOnly(false);
			m_editComponent->setValidator(new QIntValidator(this));
			m_editComponent->setText(QString("1"));


			hLay->addWidget(m_editComponent);

			m_componentBox = new QCheckBox(this);
			m_componentBox->setText("Auto");
			m_componentBox->setCheckable(true);
			m_componentBox->setChecked(true);
			m_componentBox->setStyleSheet(STYLE_MANAGER->m_Checkbox);

			connect(m_componentBox, &QCheckBox::stateChanged, this, &AISegTab::slot_OnComponentBox);

			hLay->addWidget(m_componentBox);

			QPushButton* btnApply = new QPushButton(this);
			btnApply->setText("Apply");
			btnApply->setStyleSheet(STYLE_MANAGER->buttonBehind);

			hLay->addWidget(btnApply, 1);
			connect(btnApply, &QPushButton::clicked, this, &AISegTab::slot_OnComponentApply);
		}
	}
#endif

	QFrame* frLine = new QFrame(this);
	frLine->setStyleSheet("background-color:#414141;");
	frLine->setFrameShape(QFrame::HLine);
	frLine->setFrameShadow(QFrame::Sunken);
	frLine->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	addWidget(frLine, m_nRow++, 1, QMargins(0, 10, 0, 10));

	eAVAILABLE_STATE state = m_pProductManager->IsAvailableFunctionLevel(MFL_Product_MEDIP_Plugin_AIPack);
	if (state != eAVAILABLE_STATE::NON_CREATE)
	{
		m_tabSet = new AISetTab(m_pVolumeData, m_pProductManager, this);
	}

	if (m_tabSet)
	{
		addWidget(m_tabSet, m_nRow++, 1);
		m_tabSet->setEnableObject(false);

		QWidget* emptyBox0;
		emptyBox0 = new QWidget(this);
		emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

		addWidget(emptyBox0, m_nRow++);
	}
}

void AISegTab::load_AIRegisteredWeightDirPath()
{
	// 설치 폴더의 DeepDraw predict 로드
	QString aiSegmentationDirPath = STRING_MANAGER->AISegmentationPath;
	QDir aiWeightDirPath(aiSegmentationDirPath);
	if (aiWeightDirPath.exists() == false)
	{
		qWarning() << "ai segmentation path : " << aiSegmentationDirPath;
		return;
	}

	QString weightDirPath = aiSegmentationDirPath + "/weight";
	aiWeightDirPath.setPath(weightDirPath);

	if (aiWeightDirPath.exists() == false)
	{
		return;
	}

	QStringList filters;
	filters << "*.mipx" << "*.MIPX";
	aiWeightDirPath.setNameFilters(filters);

	QStringList weightFileNameList = aiWeightDirPath.entryList();

	for (auto itPair : m_pProductManager->m_mapAIGoodsMatchProcess)
	{
		//for (auto iterProc = iterGood->second.begin(); iterProc != iterGood->second.end(); ++iterProc)
		// vector의 첫번째(시작프로세스)만 조사
		QString currentWeightFileName = QString::fromStdString(itPair.first);
		std::vector<ActionThreadArgument> argumentList = itPair.second;
		if (argumentList.empty() == false)
		{
			for (QString weightFileName : weightFileNameList)
			{
				weightFileName = weightFileName.replace(QString(".mipx"), "", Qt::CaseInsensitive);
				if (weightFileName.compare(currentWeightFileName) == 0)
				{
					ProjectItemData itemData;
					itemData.ProductName = QString::fromStdString(m_pProductManager->m_strDeepDrawPredict_MEDIP_CREDIT);
					itemData.FunctionLevel = MFL_Common_AI_PredictUsableCount_Credit;
					itemData.WeightFileName = weightFileName;

					m_productItemMap.insert(weightFileName, itemData);
				}
			}
		}
	}
}

void AISegTab::load_AILocalWeightDirPath()
{
	// appdata local 폴더의 내부mipx
	QDir dirWeightLocalAI(STRING_MANAGER->LocalAISegPath);

	if (dirWeightLocalAI.exists())
	{
		return;
	}
	QString Path = STRING_MANAGER->m_strLocalAISegWeightPath;
	dirWeightLocalAI.setPath(Path);

	if (dirWeightLocalAI.exists() == false)
	{
		return;
	}

	QStringList filters;
	filters << "*.mipx" << "*.MIPX";
	dirWeightLocalAI.setNameFilters(filters);

	QStringList weightFileNameList = dirWeightLocalAI.entryList();

	bool bHeart_3DNN = false;
	bool bHeartVM_3DNN = false;

	for each (QString weightFileName in weightFileNameList)
	{
		// mipx 확인
		QString strAdded = "";

		if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_AIResearch_AIWeight_Selectweight))
		{
			weightFileName.replace(QString(".mipx"), "", Qt::CaseInsensitive);
			ProjectItemData itemData;
			itemData.ProductName = QString::fromStdString(m_pProductManager->m_strDeepDrawPredict_MEDIP_CREDIT);
			itemData.FunctionLevel = MFL_Common_AIResearch_AIWeight_Selectweight;
			itemData.WeightFileName = weightFileName;

			if (weightFileName.compare("CT_Heart_3DNN") == 0)
			{
				bHeart_3DNN = true;
			}
			else if (weightFileName.compare("CT_HeartVM_3DNN") == 0)
			{
				bHeartVM_3DNN = true;
			}
		}
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_DEV_AIheartsolution) && bHeart_3DNN && bHeartVM_3DNN)
	{
		ProjectItemData itemData;
		itemData.ProductName = QString::fromStdString(m_pProductManager->m_strDeepDrawPredict_MEDIP_CREDIT);
		itemData.FunctionLevel = MFL_Common_AIResearch_AIWeight_Selectweight;
		itemData.WeightFileName = "CT_Heart_3DNN";
	}
}

void AISegTab::load_AILocalUserWeightDirPath()
{
	// appdata local 폴더의 사용자 mipx 로드
	QDir dirAIWeightUser(STRING_MANAGER->LocalAISegPath);

	if (dirAIWeightUser.exists() == false)
	{
		return;
	}
	QString Path = STRING_MANAGER->m_strLocalAISegUserWeightPath;
	dirAIWeightUser.setPath(Path);

	if (dirAIWeightUser.exists() == false)
	{
		return;
	}
	QStringList filters;
	filters << "*.mipx" << "*.MIPX";
	dirAIWeightUser.setNameFilters(filters);

	QStringList weightFileNameList = dirAIWeightUser.entryList();

	for each (QString weightFileName in weightFileNameList)
	{
		if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_AITrainning_CustomTrainningTool))
		{
			weightFileName.replace(QString(".mipx"), "", Qt::CaseInsensitive);

			ProjectItemData itemData;
			itemData.ProductName = QString::fromStdString(m_pProductManager->m_strMEDIP);
			itemData.FunctionLevel = MFL_Common_AITrainning_CustomTrainningTool;
			itemData.WeightFileName = weightFileName;
		}
	}
}

void AISegTab::slot_OnProcChanged(int index)
{
	if (!m_GrProc->button(index)->isCheckable())
	{
		return;
	}

	if (index == 0)
	{
		m_GrProc->button(1)->setChecked(false);
	}
	else
	{
		m_GrProc->button(0)->setChecked(true);
	}

	m_GPU = m_GrProc->button(index)->isChecked();
}

void AISegTab::slot_OnTrain()
{
	const QString projPath = STRING_MANAGER->LocalAISegPath + "/weight";

	if (!m_pVolumeData->isValidate())
	{
		return;
	}

	int index = m_cboProject->currentIndex();
	index++;
	QString projName = m_cboProject->currentText();
	if (index == m_cboProject->count()) //new
	{
		ProjectDialog dlg(this);

		if (dlg.exec() == dlg.Accepted)
		{
			projName = dlg.getProjName();

			if (projName.isEmpty())
			{
				QMessageBox::warning(this, QString("Project name"), QString("It can not be set to a name that is the same as an existing project."));
				return;
			}
		}
		else
		{
			return;
		}
	}

	ROITab2* tab = m_pWinManager->GetTab()->getROITab();

	if (tab)
	{
		QList<QTreeWidgetItem*>& list = tab->ROIList->selectedItems();
		QList<QTreeWidgetItem*> Explist;

		for (int i = 0; i < list.size(); i++)
		{
			QTreeWidgetItem* item = list.at(i);

			if (item->childCount() == 0)
				item = item->parent();

			muint32 index = tab->ROIList->indexOfTopLevelItem(item);

			if (!Explist.contains(item))
			{
				Explist.push_back(item);
			}
		}

		QVector<MaskInfo*> projInfo;
		bool res = true;
		QString emptyProj;

		projInfo.clear();

		for (int i = 0; i < Explist.size(); i++)
		{
			index = tab->ROIList->indexOfTopLevelItem(Explist[i]);
			MaskInfo* pInfo = m_pVolumeData->getMaskInfo(index);

			if (pInfo)
			{
				if (m_pVolumeData->isEmptyMaskVoxel(index))
				{
					res = false;
					emptyProj = m_pVolumeData->getMaskName(index);
					break;
				}

				projInfo.push_back(pInfo);
			}
		}

		if (res)
		{
			QString filePath = projPath + QString("/%1.mipx").arg(projName);

			QFile file(filePath);

			res = file.exists();

			res = m_pActionManager->action_Deepdraw_train(projName, projInfo, m_GPU, res);

			m_pWinManager->FreezeProject_InAISegProcessing(true);
		}
		else
		{
			QMessageBox::warning(this, QString("Project Train"), QString("%1 has no selected segmentation area.").arg(emptyProj));
		}
	}
}

void AISegTab::slot_OnComponentApply()
{
	if (!m_pVolumeData->isValidate())
	{
		return;
	}

	// 
	int m_n_component = m_editComponent->text().toInt();
	m_pActionManager->action_ImageComponentChoise(m_pWinManager->getSelectedMask(), m_pWinManager->getSelectedMaskByteIndex(), m_n_component);
}

void AISegTab::slot_OnMaskInfoSave()
{
	QString strLatest;
	bool latest = m_pWinManager->lastestPathGet(strLatest);

	strLatest.remove(strLatest.section(".", -1));
	strLatest.remove(strLatest.size() - 1, 1);

	QFileInfo fileInfo(strLatest);
	QString fileName = ExportFileDialog(
		this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_SAVE),
		fileInfo.fileName(),
		fileInfo.dir().path(),
		tr("CSV File(*.csv;*.CSV)"),
		QFileDialog::ShowDirsOnly
	);
	if (!fileName.isEmpty())
	{
		//mask 이름, 복셀 숫자, 비율, 면적
		QString valText;

		QFile newFile(fileName);

		if (!newFile.open(QIODevice::WriteOnly))
		{
			QMessageBox::warning(nullptr, "Mask info save fail.", "csv file create fail.");
			return;
		}

		int nMaskCnt = m_pVolumeData->getMaskInfoListCnt();

		valText = QString("Mask Name,Voxel,Ratio,Volume\r\n");
		newFile.write((const char*)valText.toStdWString().c_str(), sizeof(WCHAR) * valText.size());

		for (mint16 index = 0; index < nMaskCnt; index++)
		{
			MaskInfo* info = m_pVolumeData->getMaskInfo(index);
			int nVoxelCnt = m_pVolumeData->getVoxelCount(info->uid);
			int nPer = (int)(((float)nVoxelCnt / m_pVolumeData->getVolumeDataLength()) * 10000);
			float fArea = (float)nPer * 0.01;

			valText = QString().fromWCharArray(info->maskName) + QString(",%1,%2,%3\r\n").arg(nVoxelCnt).arg(fArea)
				.arg(QString::number(m_pVolumeData->getSpace3D(true) * m_pVolumeData->getVoxelCount(info->uid), 'f', 2));
			newFile.write((const char*)valText.toStdWString().c_str(), sizeof(WCHAR) * valText.size());
		}

		newFile.close();
	}
}

void AISegTab::slot_OnPredict()
{
	if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Product_MEDIP_Plugin_AIPack))
	{
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_NONE_DEEPDRAW_PACK)).exec();
		return;
	}

	if (isAvailablePredict() == false)
	{
		return;
	}

	int nCount = -1;
	QString strPredictCntState = "";

	if (!m_pVolumeData->isValidate())
	{
		return;
	}

	if (!m_pActionManager->isActionFinished())
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	m_pWinManager->mainSegmentWidget->setWorkMode(WORK_NONE);

	if (m_eWeightType == MFL_Common_AI_AIPredict_Predict)
	{
		nCount = 1;

		if (m_pProductManager->m_mapAIResearchMatchProcess.size() > 0)
		{
			std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);

			m_pActionManager->ClearReservationWorkList();

			auto finditer = m_pProductManager->m_mapAIResearchMatchProcess.find(m_strProjectName.toStdString());
			if (finditer != m_pProductManager->m_mapAIResearchMatchProcess.end())
			{
				for (auto iter = finditer->second.begin(); iter != finditer->second.end(); ++iter)
				{
					auto strData = std::static_pointer_cast<std::string>(iter->pTempData);
					ActionThreadArgument threadArg(iter->eCurrentThread, iter->eNextThread
						, strData != nullptr ? std::make_shared<std::string>(*strData) : nullptr);
					m_pActionManager->m_qThreadNext.push_back(threadArg);
				}
			}

			m_pActionManager->m_nCurrentThreadCount = m_pActionManager->m_nMultiThreadTotalCount = m_pActionManager->m_qThreadNext.size();
		}
	}
	else if (m_eWeightType == MFL_Common_AITrainning_CustomTrainningTool)
	{
		nCount = 1;
	}
	else if (m_eWeightType == MFL_Common_AI_PredictUsableCount_Credit)
	{
		// chkFunctionUsableCount
		if (LICENSE_DATA->IsOnline())
		{
			sFuncUsableCountResult data{ 0, "" };
			nCount = LICENSE_DATA->chkFunctionUsableCount((eMEDIP_FUNCTION_LEVEL)MFL_Common_AI_PredictUsableCount_Credit, eLSPTChkFunctionUsableCount, data);
			strPredictCntState = data.strFuncUsableCountState;

			if (!strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_REJECTED))
			{
				QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN),
					"You were denied use of this function.<br>Please contact to <a href='mailto:contact@medicalip.com'>contact@medicalip.com</a>").exec();
				return;
			}
			else if (!strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_LIMITED))
			{
				QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN),
					"You were restricted from use of this function.<br>Please contact to <a href='mailto:contact@medicalip.com'>contact@medicalip.com</a>").exec();
				return;
			}
		}
		else
		{
			nCount = LICENSE_DATA->chkFunctionUsableCount((eMEDIP_FUNCTION_LEVEL)MFL_Common_AI_PredictUsableCount_Credit, eLSPTChkFunctionUsableCount);
		}

		m_nMedipCreditPredictCnt = nCount;
		m_strMedipCreditPredictCntState = strPredictCntState;

		std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);

		m_pActionManager->ClearReservationWorkList();

		auto it = m_pProductManager->m_mapAIGoodsMatchProcess.find(m_strProjectName.toStdString());
		if (it != m_pProductManager->m_mapAIGoodsMatchProcess.end())
		{
			std::vector<ActionThreadArgument> threadArgumentList;
			threadArgumentList = it->second;
			for (const ActionThreadArgument& arg : threadArgumentList)
			{
				std::shared_ptr<std::string> pData = std::static_pointer_cast<std::string>(arg.pTempData);
				ActionThreadArgument threadArg(
					arg.eCurrentThread,
					arg.eNextThread,
					pData != nullptr ? std::make_shared<std::string>(*pData) : nullptr);
				m_pActionManager->m_qThreadNext.push_back(threadArg);
			}
		}

		m_pActionManager->m_nCurrentThreadCount = m_pActionManager->m_nMultiThreadTotalCount = m_pActionManager->m_qThreadNext.size();

	}

	qDebug() << QString("count : %1").arg(nCount);
	//int nCount = 1;
	if (nCount > 0)
	{
		QString filePath = m_strWeightPath + QString("/%1.mipx").arg(m_strFileName);
		QFile file(filePath);

		bool res = file.exists();
		bool predict_res = false;
		if (res)
		{
			BoundingBoxI box = getValidRegion();
			QString strProgressName = QString("DeepDraw - %1").arg(m_strProjectName);
			QString weightFileName = m_strFileName;
			eDeepPredictAICopyMask copyMask = GetCopyMaskType();

			qInfo() << "predict current thread next count : " << m_pActionManager->m_qThreadNext.size();
			if (m_pActionManager->m_qThreadNext.size() > 1)
			{
				std::shared_ptr<std::string> pWeightFileName = std::static_pointer_cast<std::string>(m_pActionManager->m_qThreadNext[0].pTempData);
				weightFileName = QString::fromStdString(*pWeightFileName);
			}

			m_pActionManager->action_ProgressBegin(strProgressName);
			m_pActionManager->action_Deepdraw_predict(m_strInputPath, m_strWeightPath, m_eWeightType, weightFileName, box, m_GPU, copyMask, false);

			/* Predict 진행 중 Input Block*/
			m_pWinManager->FreezeProject_InAISegProcessing(true);
		}
		else
		{
			QMessageBox::warning(this, "AI Predict", "Before you can run Predict, you need to run Train or choose other trained project.");
			return;
		}
	}
	else
	{
		// nCount == -1인 경우는 credit count 갯수를 정상적으로 읽어오지 못한 경우로 예외 처리.
		if (nCount == -1)
		{
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DE_0007)).exec();
		}
		else
		{
			QMessageBox msg(nullptr);
			msg.setWindowTitle(STRING_MANAGER->getString(STR_WARN));
			msg.setTextFormat(Qt::RichText);
			msg.setText(STRING_MANAGER->getString(ERR_DU_1009));
			msg.setStandardButtons(QMessageBox::Ok);
			msg.exec();
		}
	}

}

void AISegTab::slot_OnSegment()
{
	if (!m_pVolumeData->isValidate())
	{
		return;
	}

	QString vendor = QString::fromLocal8Bit(g_Renderer->getVendorName());

	if (!vendor.contains(QString("NVIDIA"), Qt::CaseInsensitive))
	{
		QMessageBox::warning(this, QString("AI Segmentation GPU"), QString("AI Segmentation supports only NVIDIA GPU."));
		return;
	}

	AI_TYPE type = (AI_TYPE)m_cboProject->currentIndex();

	int ww, wl, start, to;

	ww = wl = start = to = 0;

	m_tabSet->getWWWL(ww, wl);
	GetDepth(&start, &to);

	switch (type)
	{
	case AI_LIVER:
		m_pActionManager->action_MaskList_add_LiverResult(ww, wl, start, to);
		break;
	default:
		break;
	}

}

void AISegTab::LoadProject(bool isReload)
{
#ifdef AI_VER
	m_productItemMap.clear();

	load_AIRegisteredWeightDirPath();
	load_AILocalWeightDirPath();
	load_AILocalUserWeightDirPath();

	m_cboProject->blockSignals(true);

	if (isReload)
	{
		m_cboProject->clear();
	}

	if (isReload)
	{
		m_cboProject->addItem("");
	}

	for (auto& key : m_productItemMap.keys())
	{
		m_cboProject->addItem(key);
	}

	m_cboProject->blockSignals(false);
#endif
}

void AISegTab::GetDepth(int* pOutStart, int* pOutEnd)
{
	int tmpStart, tmpEnd;
	tmpStart = m_ValidRegion.getMinZ();
	tmpEnd = m_ValidRegion.getMaxZ();

	*pOutStart = tmpStart;
	*pOutEnd = tmpEnd;
}

void AISegTab::UpdateDepth(bool checked)
{
	if (!m_pVolumeData->isValidate())
	{
		return;
	}

	if (checked)
	{
		m_ValidRegion = m_pVolumeData->getBoundingBox();
	}
	else
	{
		m_ValidRegion = m_pVolumeData->getBoundingBox3D();
	}

	int CZ = m_pVolumeData->getCZ();

	int tmpStart = CZ - (m_ValidRegion.minZ + 1);
	int tmpTo = CZ - (m_ValidRegion.maxZ + 1);

	m_ValidRegion.maxZ = tmpStart;
	m_ValidRegion.minZ = tmpTo;
}

QStringList AISegTab::GetAIProjectNameList() const
{
	QStringList projectNameList;
	if (m_cboProject == nullptr)
	{
		return projectNameList;
	}

	for (int i = 0; i < m_cboProject->count(); ++i)
	{
		projectNameList << m_cboProject->itemText(i);
	}

	return projectNameList;
}

int AISegTab::getOutVal()
{
	if (m_tabSet)
	{
		return m_tabSet->getSliderOutVal();
	}
	return 127;
}

void AISegTab::FreezeProject(bool val)
{
	if (m_cboProject)
	{
		m_cboProject->setEnabled(!val);
	}

	if (m_btnPredict)
	{
		m_btnPredict->setEnabled(!val);
	}

	if (m_spinCluster)
	{
		m_spinCluster->setEnabled(!val);
	}

	if (m_chkCopyMask)
	{
		m_chkCopyMask->setEnabled(!val);
	}
}

void AISegTab::AddProject(QString proj)
{
	//not implemented
	qCritical() << "add project not implemented";
	Q_ASSERT(false);
}

eMEDIP_FUNCTION_LEVEL AISegTab::getWeightType()
{
	return m_eWeightType;
}

int AISegTab::GetClusterNumber() const
{
	if (m_spinCluster == nullptr)
	{
		return 2;
	}

	return m_spinCluster->value();
}

bool AISegTab::isAvailablePredict(eDeepcatchUNETType unetType)
{
	bool ret = false;
#ifdef AI_VER
	if (AIBase::isCudaAvailable() == false)
	{
		//QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0006)).exec();
		QMessageBox msg(this);
		msg.setWindowTitle(STRING_MANAGER->getString(STR_WARN));
		msg.setTextFormat(Qt::RichText);
		//msg.setText("GPU processor not detected. Please make sure the NVIDIA graphics card and the latest drivers(<a href='https://www.nvidia.co.kr/Download/index.aspx?lang=en\'>link</a>) are installed.");
		msg.setText(STRING_MANAGER->getString(ERR_DH_0006));
		msg.setStandardButtons(QMessageBox::Ok);
		msg.exec();

		return ret;
	}

	// opengl 로 gpu 정보 얻어오는 부분
	QOpenGLContext context;
	context.create();

	QOffscreenSurface surface;
	surface.create();

	QOpenGLFunctions func;
	context.makeCurrent(&surface);
	func.initializeOpenGLFunctions();

	//qDebug() << "gpu info 1:" << QString::fromUtf8(reinterpret_cast<const char*>(func.glGetString(GL_EXTENSIONS))) << endl;
	//qDebug() << "gpu info 2:" << QString::fromUtf8(reinterpret_cast<const char*>(func.glGetString(GL_VENDOR))) << endl;

	GLint total_mem_kb = 0, cur_avail_mem_kb = 0;
	func.glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_mem_kb);
	func.glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &cur_avail_mem_kb);

	qDebug() << "total_mem_kb" << total_mem_kb;
	qDebug() << "cur_avail_mem_kb" << cur_avail_mem_kb;

	// 500MB = 524288
	// 2GB = 2097152 = 2048KB * 1024
	// 3GB = 3170304 = 3096KB * 1024
	// 6GB = 6291456 = 6144KB * 1024
	// 6.5GB = 6291456 + 524288 = 6815744
	// 7GB = 7340032 = 7168KB * 1024	
	// 8GB = 8388608 = 8192KB * 1024
	int nReqMemSize = 0;
	int nReqGBMemSize = 0;
	//if ((m_pProductManager->GetProductInfo(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_MEDIP_DEEPCATCH))
	if (unetType == eDeepcatchUNETType::eDUT2D) // 2D
	{
		nReqMemSize = 2097152;
		nReqGBMemSize = 3;
	}
	else if (unetType == eDeepcatchUNETType::eDUT3D)	//3D
	{
		nReqMemSize = 6815744;
		nReqGBMemSize = 8;
	}

	if (cur_avail_mem_kb < nReqMemSize)
	{
		QMessageBox::warning(nullptr, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0001));
		// return false;
		// 메모리 부족할 땐 메시지 알림만하고 predict 할 수 있게 한다.

	}
	ret = true;

#endif
	return ret;
}

BoundingBoxI AISegTab::getValidRegion(void)
{
	return m_ValidRegion;
}

void AISegTab::setValidRegion(BoundingBoxI box)
{
	m_ValidRegion = box;
}

void AISegTab::UpdateProductCombo(int nItemIdx)
{
#ifdef AI_VER
	m_eWeightType = (eMEDIP_FUNCTION_LEVEL)0;
	m_nPredictCnt = -1;
	m_strPredictCntState = "";

	if (m_cboProject == nullptr || m_cboProject->count() <= nItemIdx)
	{
		return;
	}

	QString projectName = m_cboProject->itemText(nItemIdx);

	auto it = m_productItemMap.find(projectName);
	if (it == m_productItemMap.end())
	{
		return;
	}

	ProjectItemData itemData = *it;

	if (itemData.ProductName == QString(m_pProductManager->m_strMEDIP.c_str()))
	{
		if (itemData.FunctionLevel == MFL_Common_AI_AIPredict_Predict)
		{
			QString weightDirPath = STRING_MANAGER->m_strLocalAISegWeightPath;
			QString filename = itemData.WeightFileName;
			QString filenameWithExtension = itemData.WeightFileName + QString(".mipx");

			SegmentationMeta meta;
			std::vector< std::vector<int>> notuse;
			Segmentation::readMeta(meta, weightDirPath.toLocal8Bit().toStdString(), filenameWithExtension.toLocal8Bit().toStdString(), true, notuse);
			if (meta.type == ClientType::MEDIP)	// DeepDraw research
			{
				m_eWeightType = MFL_Common_AI_AIPredict_Predict;
				m_strWeightPath = STRING_MANAGER->m_strLocalAISegWeightPath;
				m_strInputPath = STRING_MANAGER->m_strLocalAISegPredictPath;

				m_strFileName = filename;
				m_strProjectName = projectName;
				m_nPredictCnt = -2;
				m_strPredictCntState = FUNCTION_USABLECOUNT_STATE_UNLIMITED;
			}
			else
			{
				// 에러 처리
			}
		}
		else if (itemData.FunctionLevel == MFL_Common_AITrainning_CustomTrainningTool)
		{
			QString weightDirPath = STRING_MANAGER->m_strLocalAISegUserWeightPath;
			QString filename = itemData.WeightFileName;
			QString filenameWithExtension = itemData.WeightFileName + QString(".mipx");

			SegmentationMeta meta;
			std::vector<std::vector<int>> notuse;
			Segmentation::readMeta(meta, weightDirPath.toLocal8Bit().toStdString(), filenameWithExtension.toLocal8Bit().toStdString(), true, notuse);
			if (meta.type == ClientType::USER_CREATED)
			{
				m_eWeightType = MFL_Common_AITrainning_CustomTrainningTool;
				m_strWeightPath = STRING_MANAGER->m_strLocalAISegUserWeightPath;
				m_strInputPath = STRING_MANAGER->m_strLocalAISegPredictPath;

				m_strFileName = filename;
				m_strProjectName = projectName;
				m_nPredictCnt = -2;
				m_strPredictCntState = FUNCTION_USABLECOUNT_STATE_UNLIMITED;
			}
			else
			{
				// 에러 처리
			}
		}
	}
	else if (itemData.ProductName == QString(m_pProductManager->m_strDeepDrawPredict_MEDIP_CREDIT.c_str()))
	{
		if (itemData.FunctionLevel == MFL_Common_AI_PredictUsableCount_Credit)
		{
			QString weightDirPath = STRING_MANAGER->AISegmentationPath + "/weight";
			QString filename = itemData.WeightFileName;
			QString filenameWithExtension = itemData.WeightFileName + QString(".mipx");

			SegmentationMeta meta;
			std::vector< std::vector<int>> notuse;
			Segmentation::readMeta(meta, weightDirPath.toLocal8Bit().toStdString(), filenameWithExtension.toLocal8Bit().toStdString(), true, notuse);
			if (meta.type == ClientType::MEDIP_CREDIT)
			{
				m_eWeightType = MFL_Common_AI_PredictUsableCount_Credit;
				m_strWeightPath = STRING_MANAGER->AISegmentationPath + "/weight";
				m_strInputPath = STRING_MANAGER->m_strLocalAISegPredictPath;
				m_strFileName = filename;
				m_strProjectName = projectName;

				if (!m_oneTimeCheck_AI)
				{
					m_oneTimeCheck_AI = true;
					if (LICENSE_DATA->IsOnline())
					{
						sFuncUsableCountResult data{ 0, "" };
						m_nPredictCnt = LICENSE_DATA->chkFunctionUsableCount((eMEDIP_FUNCTION_LEVEL)m_eWeightType, eLSPTChkFunctionUsableCount, data);
						m_strPredictCntState = data.strFuncUsableCountState;
					}
					else
					{
						m_nPredictCnt = LICENSE_DATA->chkFunctionUsableCount((eMEDIP_FUNCTION_LEVEL)m_eWeightType, eLSPTChkFunctionUsableCount);
					}
					m_nMedipCreditPredictCnt = m_nPredictCnt;
					m_strMedipCreditPredictCntState = m_strPredictCntState;
				}
				else
				{
					m_nPredictCnt = m_nMedipCreditPredictCnt;
					m_strPredictCntState = m_strMedipCreditPredictCntState;
				}
			}
			else
			{
				// 에러 처리
			}
		}
	}
#endif

}

eMedipAICompleteWorkReturnType AISegTab::CompleteWork()
{
	eMedipAICompleteWorkReturnType result = eMACWRTSuccess;
	int nCnt = -1;

	if (m_eWeightType == MFL_Common_AI_AIPredict_Predict || m_eWeightType == MFL_Common_AITrainning_CustomTrainningTool)
	{
		//result = eMACWRTSuccess;
		bool bSeriseThread = false;
		if (!m_pActionManager->m_qThreadNext.empty())
		{
			auto sharedProject = std::static_pointer_cast<std::string>(m_pActionManager->m_qThreadNext.front().pTempData);
			std::string projectName = *sharedProject;
			qDebug() << "projectName :" << QString::fromStdString(projectName);

			if (getProjectName().toStdString().compare(projectName))
			{
				return eMACWRTNextPredict;
			}
		}
		else
		{
			result = eMACWRTSuccess;
		}
	}
	else
	{
		bool bSeriseThread = false;
		if (!m_pActionManager->m_qThreadNext.empty())
		{
			auto sharedProject = std::static_pointer_cast<std::string>(m_pActionManager->m_qThreadNext.front().pTempData);
			std::string projectName = *sharedProject;
			qDebug() << "projectName :" << QString::fromStdString(projectName);

			if (getProjectName().toStdString().compare(projectName))
			{
				return eMACWRTNextPredict;
			}
		}

		if (LICENSE_DATA->IsOnline())
		{
			sFuncUsableCountResult data{ 0, "" };
			nCnt = LICENSE_DATA->chkFunctionUsableCount(MFL_Common_AI_PredictUsableCount_Credit, eLSPTDecreaseFunctionUsableCount, data);
			m_nMedipCreditPredictCnt = nCnt;
			m_strMedipCreditPredictCntState = data.strFuncUsableCountState;
			if (!data.strFuncUsableCountState.compare(FUNCTION_USABLECOUNT_STATE_UNLIMITED))
			{
				result = eMACWRTSuccess;
			}
			else if (nCnt == m_nPredictCnt - 1)
			{
				m_nPredictCnt--;
				m_LineEditAvailableCount->setText(QString("%1").arg(m_nPredictCnt));

				result = eMACWRTSuccess;
			}
			else
			{
				result = eMACWRTPredictCountErr;
			}
		}
		else
		{
			nCnt = LICENSE_DATA->chkFunctionUsableCount(MFL_Common_AI_PredictUsableCount_Credit, eLSPTDecreaseFunctionUsableCount);
			m_nMedipCreditPredictCnt = nCnt;
			if (nCnt == m_nPredictCnt - 1)
			{
				m_nPredictCnt--;
				m_LineEditAvailableCount->setText(QString("%1").arg(m_nPredictCnt));

				result = eMACWRTSuccess;
			}
			else
			{
				result = eMACWRTPredictCountErr;
			}
		}
	}

	return result;
}

QString AISegTab::getProjectName()
{
	return m_strProjectName;
}

QString AISegTab::GetFileName() const
{
	return m_strFileName;
}

bool AISegTab::IsUseGPU() const
{
	return m_GPU;
}

void AISegTab::slot_OnProjectChanged(int index)
{
	UpdateProductCombo(index);

	if (m_nPredictCnt == -1) // not available로 변경 not applicable에서 
	{
		m_LineEditAvailableCount->setText("N/A");
	}
	else if (m_nPredictCnt == -2)	// 무한대 사용
	{
		m_LineEditAvailableCount->setText("Unlimited");
	}
	else
	{
		if (LICENSE_DATA->IsOnline())
		{
			if (!m_strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_REJECTED))
			{
				m_LineEditAvailableCount->setText("Rejected");
			}
			else if (!m_strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_UNLIMITED))
			{
				m_LineEditAvailableCount->setText("Unlimited");
			}
			else if (!m_strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_LIMITED))
			{
				m_LineEditAvailableCount->setText("Limited");
			}
			else
			{
				m_LineEditAvailableCount->setText(QString("%1").arg(m_nPredictCnt));
			}
		}
		else
		{
			m_LineEditAvailableCount->setText(QString("%1").arg(m_nPredictCnt));
		}
	}
}

