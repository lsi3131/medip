#include "stdafx.h"
#include "AISegTabDeepCatch.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "Productmanager.h"
#include "StyleManager.h"
#include "LicenseManager.h"
#include "Main/MainSegmentWidget.h"
#include "ActionMaskList.h"
#include "MedipQT.h"
#include "Dialogs/InputMessageDlg.h"
#include "Dialogs/HCCPredictionDlg.h"
#include "Windows/Tabwindow.h"
#include "DataContext.h"

#include <cctype>

AISegTabDeepCatch::AISegTabDeepCatch(
	VOLUME_DATA* pVolumeData, 
	WindowManager* pWinManager,
	ActionManager* pActionManager,
	ProductManager* pProductManager, 
	QWidget* parent /*= NULL*/) :
	AISegTab(pVolumeData, pWinManager, pActionManager, pProductManager, parent)
{
	m_strWeightPath = STRING_MANAGER->AISegmentationPath + "/weight_deepcatch";
	m_strInputPath = STRING_MANAGER->LocalAISegPath + "/predict";

	m_nUNET = eDUT2D;						// 0:2d, 1:3d
	m_nContrast = eDCTNoneContrast;			// 0:none contrast, 1:contrast
	m_nReportGender = eDGTUnknown;			// 0: Unknown, 1: Female, 1: male
	m_nSingleSlice = eDSliceSelectAuto;		// 0:L3, 1:User define
	m_nMultiSlice = eDSliceSelectAuto;		// 0:Abdominal waist range, 1:User define
	m_nPreferReporting = eDPTSingleSlice;	// 0: single slice, 1: multi slice
	m_nAWPredict = eDAWTCoronal;
	m_nAdditionalOptions = eDAONone;

	/////////////////////////////////////////////////////////////////////

	bool ddd = m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch);
	bool dd = m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2);
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch) && !m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		m_ComboDeepCatchModule = m_pProductFactory->createWidget<QComboBox>(MFL_Product_DeepCatch, this);
	}
	else if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		m_ComboDeepCatchModule = m_pProductFactory->createWidget<QComboBox>(MFL_Product_DeepCatch_DeepCatchV2, this);
	}

	if (m_ComboDeepCatchModule)
	{
		m_ComboDeepCatchModule->setStyleSheet(STYLE_MANAGER->comboBoxTab);
		m_ComboDeepCatchModule->installEventFilter(this);

		//SUPPORT_DEEPCATCH_VERSION_2 
		if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
		{
			m_comboMultiSliceUpper = nullptr;
			m_comboMultiSliceLower = nullptr;
			m_comboSingleSlice = nullptr;

			m_ComboDeepCatchModule->setHidden(true);
			//m_pAvailableCnt->setHidden(true);
		}
		else
		{
			m_pAvailableCnt = new QLineEdit(this);
			m_pAvailableCnt->setStyleSheet(STYLE_MANAGER->editBoxTab + QString("QLineEdit{color:yellow;}"));
			m_pAvailableCnt->setReadOnly(true);
			m_pAvailableCnt->setAlignment(Qt::AlignCenter);
			m_pAvailableCnt->setFixedWidth(50);

			addWidget(m_ComboDeepCatchModule, m_nRow, 4, QMargins(0, 0, 0, 0));
			addWidget(m_pAvailableCnt, m_nRow, 1);
			++m_nRow;
		}
	}

	// DeepCatch predict 옵션.
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch) || m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
		{
			QString text_1st = "Body composition";
			QString text_2nd = "Bone Analysis";
			QString text_3rd = "liver/spleen/aorta";
			QString text_4th = "Additional Option";
			QString text_5th = "Single Slice";
			QString text_6th = "Multi Slice";

			// 2D, 3D
			QLabel* pLabelBodyComp = new QLabel(this);
			pLabelBodyComp->setText(text_1st);

			m_btnGroupBodyComposition = new QButtonGroup(this);

			QTextCodec* codec = QTextCodec::codecForName("eucKR");
			char* string = "Abd＆WB";
			QString str = codec->toUnicode(string);

			m_radioWholeBody = new QRadioButton(this);
			m_radioWholeBody->setText(str);
			m_radioWholeBody->setCheckable(true);
			m_radioWholeBody->setChecked(true);
			m_radioWholeBody->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_radioChest = new QRadioButton(this);
			m_radioChest->setText(QString("Chest"));
			m_radioChest->setCheckable(true);
			m_radioChest->setChecked(false);
			m_radioChest->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_btnGroupBodyComposition->addButton(m_radioWholeBody, 0);
			m_btnGroupBodyComposition->addButton(m_radioChest, 1);

			addWidget(pLabelBodyComp, m_nRow, 2, QMargins(10, 5, 0, 5));
			addWidget(m_radioWholeBody, m_nRow, 2);
			addWidget(m_radioChest, m_nRow, 2);
			++m_nRow;

			// Contrast
			QLabel* pLabelContrast = new QLabel(this);
			pLabelContrast->setText(text_2nd);

			m_btnGroupBoneAnalysis = new QButtonGroup(this);

			m_radioBoneAnalysisYES = new QRadioButton(this);
			m_radioBoneAnalysisYES->setText(QString("YES"));
			m_radioBoneAnalysisYES->setCheckable(true);
			m_radioBoneAnalysisYES->setChecked(true);
			m_radioBoneAnalysisYES->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_radioBoneAnalysisNO = new QRadioButton(this);
			m_radioBoneAnalysisNO->setText(QString("NO"));
			m_radioBoneAnalysisNO->setCheckable(true);
			m_radioBoneAnalysisNO->setChecked(false);
			m_radioBoneAnalysisNO->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_btnGroupBoneAnalysis->addButton(m_radioBoneAnalysisYES, 0);
			m_btnGroupBoneAnalysis->addButton(m_radioBoneAnalysisNO, 1);

			addWidget(pLabelContrast, m_nRow, 2, QMargins(10, 5, 0, 5));
			addWidget(m_radioBoneAnalysisYES, m_nRow, 2);
			addWidget(m_radioBoneAnalysisNO, m_nRow, 2);
			++m_nRow;

			// AW Predict
			QLabel* pLabelAW = new QLabel(this);
			pLabelAW->setText(text_3rd);

			//m_btnGroupIOClass = new QButtonGroup(this);		
			m_checkIOClassificationLiverSpleen = new QCheckBox(QString("liver/spleen"), this);
			m_checkIOClassificationLiverSpleen->setChecked(false);
			m_checkIOClassificationLiverSpleen->setStyleSheet(STYLE_MANAGER->m_Checkbox);

			m_checkIOClassificationAorta = new QCheckBox(QString("aorta"), this);
			m_checkIOClassificationAorta->setChecked(false);
			m_checkIOClassificationAorta->setStyleSheet(STYLE_MANAGER->m_Checkbox);

			connect(m_checkIOClassificationLiverSpleen, &QCheckBox::clicked, this, &AISegTabDeepCatch::slot_OnIOClassification);
			connect(m_checkIOClassificationAorta, &QCheckBox::clicked, this, &AISegTabDeepCatch::slot_OnIOClassification);

			addWidget(pLabelAW, m_nRow, 2, QMargins(10, 5, 0, 5));
			addWidget(m_checkIOClassificationLiverSpleen, m_nRow, 2);
			addWidget(m_checkIOClassificationAorta, m_nRow, 2);
			++m_nRow;


			// Additional options
			QLabel* pAdditionalOptions = new QLabel(this);
			pAdditionalOptions->setText(text_4th);

			m_btnGroupAdditionalOptions = new QButtonGroup(this);

			m_radioAddOptionsNone = new QRadioButton(this);
			m_radioAddOptionsNone->setText(QString("None"));
			m_radioAddOptionsNone->setCheckable(true);
			m_radioAddOptionsNone->setChecked(true);
			m_radioAddOptionsNone->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_radioAddOptionsOnlyTrunk = new QRadioButton(this);
			m_radioAddOptionsOnlyTrunk->setText(QString("Trunk"));
			m_radioAddOptionsOnlyTrunk->setCheckable(true);
			m_radioAddOptionsOnlyTrunk->setChecked(false);
			m_radioAddOptionsOnlyTrunk->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_radioAddOptionsQCT = new QRadioButton(this);
			m_radioAddOptionsQCT->setText(QString("QCT"));
			m_radioAddOptionsQCT->setCheckable(true);
			m_radioAddOptionsQCT->setChecked(false);
			m_radioAddOptionsQCT->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_btnGroupAdditionalOptions->addButton(m_radioAddOptionsNone, eDAONone);
			m_btnGroupAdditionalOptions->addButton(m_radioAddOptionsOnlyTrunk, eDAOOnlyTrunk);
			m_btnGroupAdditionalOptions->addButton(m_radioAddOptionsQCT, eDAOQCT);

			addWidget(pAdditionalOptions, m_nRow, 204, QMargins(10, 5, 0, 5));
			addWidget(m_radioAddOptionsNone, m_nRow, 132);
			addWidget(m_radioAddOptionsOnlyTrunk, m_nRow, 132);
			addWidget(m_radioAddOptionsQCT, m_nRow, 132);
			++m_nRow;

			// VB Combo
			//QLabel*  pSpace = new QLabel(this);
			//pSpace->setText(" ");
			QLabel* pSpace1 = new QLabel(this);
			pSpace1->setText(" ");
			QLabel* pSpace2 = new QLabel(this);
			pSpace2->setText(" ");

			QLabel* pVBComboSingle = new QLabel(this);
			pVBComboSingle->setText(text_5th);

			m_comboSingleSlice = createVBComboBox("single", 180);
			if (m_comboSingleSlice)
			{
				connect(m_comboSingleSlice, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnVBSingleComboChanged(int)));
				m_comboSingleSlice->setCurrentIndex(eDCVB_L3);
				slot_OnVBSingleComboChanged(eDCVB_L3);
			}


			addWidget(pVBComboSingle, m_nRow, 204, QMargins(10, 5, 0, 5));
			addWidget(m_comboSingleSlice, m_nRow, 500, QMargins(0, 0, 0, 0), Qt::AlignLeft);
			++m_nRow;

			// VB Combo
			QLabel* pVBComboMulti = new QLabel(this);
			pVBComboMulti->setText(text_6th);

			QLabel* pVBComboMultiUp = new QLabel(this);
			pVBComboMultiUp->setText("up");

			QLabel* pVBComboMultiLower = new QLabel(this);
			pVBComboMultiLower->setText("low");


			m_comboMultiSliceUpper = createVBComboBox("MultiUp", 180);
			if (m_comboMultiSliceUpper)
			{
				m_comboMultiSliceUpper->addItem(COMBO_ITEMNAME_ILIAC_CHEST);
				m_comboMultiSliceUpper->setItemData(m_comboMultiSliceUpper->count() - 1, eDCVB_MAX, Qt::UserRole);
				connect(m_comboMultiSliceUpper, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnVBMultiUpperComboChanged(int)));
				slot_OnVBMultiUpperComboChanged(eDCVB_Abdominal_waist);
			}

			m_comboMultiSliceLower = createVBComboBox("MultiLow", 180);
			if (m_comboMultiSliceLower)
			{
				m_comboMultiSliceLower->addItem(COMBO_ITEMNAME_LOWER_LIB);
				m_comboMultiSliceLower->setItemData(m_comboMultiSliceLower->count() - 1, eDCVB_MAX, Qt::UserRole);
				connect(m_comboMultiSliceLower, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnVBMultiLowerComboChanged(int)));
				slot_OnVBMultiLowerComboChanged(eDCVB_Abdominal_waist);
			}


			addWidget(pVBComboMulti, m_nRow, 204, QMargins(10, 5, 0, 5));
			addWidget(pVBComboMultiUp, m_nRow, 10);
			addWidget(m_comboMultiSliceUpper, m_nRow, 250);
			addWidget(pSpace1, m_nRow, 5);
			addWidget(pVBComboMultiLower, m_nRow, 10);
			addWidget(m_comboMultiSliceLower, m_nRow, 250);
			addWidget(pSpace2, m_nRow, 5);
			++m_nRow;
		}
		else
		{
			// 2D, 3D
			QLabel* pLabel2D3D = new QLabel(this);
			pLabel2D3D->setText(QString("UNET"));

			m_btnGroupUNET = new QButtonGroup(this);

			m_radio2D = new QRadioButton(this);
			m_radio2D->setText(QString("2D"));
			m_radio2D->setCheckable(true);
			m_radio2D->setChecked(true);
			m_radio2D->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_radio3D = new QRadioButton(this);
			m_radio3D->setText(QString("3D"));
			m_radio3D->setCheckable(true);
			m_radio3D->setChecked(false);
			m_radio3D->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_btnGroupUNET->addButton(m_radio2D, 0);
			m_btnGroupUNET->addButton(m_radio3D, 1);

			addWidget(pLabel2D3D, m_nRow, 2, QMargins(10, 5, 0, 5));
			addWidget(m_radio2D, m_nRow, 2);
			addWidget(m_radio3D, m_nRow, 2);
			++m_nRow;

			// Contrast
			QLabel* pLabelContrast = new QLabel(this);
			pLabelContrast->setText(QString("Contrast"));

			m_btnGroupContrast = new QButtonGroup(this);

			m_radioNonContrast = new QRadioButton(this);
			m_radioNonContrast->setText(QString("Non contrast"));
			m_radioNonContrast->setCheckable(true);
			m_radioNonContrast->setChecked(true);
			m_radioNonContrast->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_radioContrast = new QRadioButton(this);
			m_radioContrast->setText(QString("Contrast"));
			m_radioContrast->setCheckable(true);
			m_radioContrast->setChecked(false);
			m_radioContrast->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_btnGroupContrast->addButton(m_radioNonContrast, 0);
			m_btnGroupContrast->addButton(m_radioContrast, 1);

			addWidget(pLabelContrast, m_nRow, 2, QMargins(10, 5, 0, 5));
			addWidget(m_radioNonContrast, m_nRow, 2);
			addWidget(m_radioContrast, m_nRow, 2);
			++m_nRow;

			// AW Predict
			QLabel* pLabelAW = new QLabel(this);
			pLabelAW->setText(QString("AW Selection"));

			m_btnGroupAWPredict = new QButtonGroup(this);

			m_radioAWcoronal = new QRadioButton(this);
			m_radioAWcoronal->setText(QString("Coronal"));
			m_radioAWcoronal->setCheckable(true);
			m_radioAWcoronal->setChecked(true);
			m_radioAWcoronal->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_radioAWsagittal = new QRadioButton(this);
			m_radioAWsagittal->setText(QString("Sagittal"));
			m_radioAWsagittal->setCheckable(true);
			m_radioAWsagittal->setChecked(false);
			m_radioAWsagittal->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_btnGroupAWPredict->addButton(m_radioAWcoronal, 0);
			m_btnGroupAWPredict->addButton(m_radioAWsagittal, 1);

			addWidget(pLabelAW, m_nRow, 2, QMargins(10, 5, 0, 5));
			addWidget(m_radioAWcoronal, m_nRow, 2);
			addWidget(m_radioAWsagittal, m_nRow, 2);
			++m_nRow;

			// Additional options
			QLabel* pAdditionalOptions = new QLabel(this);
			pAdditionalOptions->setText(QString("Additional Option"));

			m_btnGroupAdditionalOptions = new QButtonGroup(this);

			m_radioAddOptionsNone = new QRadioButton(this);
			m_radioAddOptionsNone->setText(QString("None"));
			m_radioAddOptionsNone->setCheckable(true);
			m_radioAddOptionsNone->setChecked(true);
			m_radioAddOptionsNone->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_radioAddOptionsOnlyTrunk = new QRadioButton(this);
			m_radioAddOptionsOnlyTrunk->setText(QString("Trunk"));
			m_radioAddOptionsOnlyTrunk->setCheckable(true);
			m_radioAddOptionsOnlyTrunk->setChecked(false);
			m_radioAddOptionsOnlyTrunk->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_radioAddOptionsQCT = new QRadioButton(this);
			m_radioAddOptionsQCT->setText(QString("QCT"));
			m_radioAddOptionsQCT->setCheckable(true);
			m_radioAddOptionsQCT->setChecked(false);
			m_radioAddOptionsQCT->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_btnGroupAdditionalOptions->addButton(m_radioAddOptionsNone, eDAONone);
			m_btnGroupAdditionalOptions->addButton(m_radioAddOptionsOnlyTrunk, eDAOOnlyTrunk);
			m_btnGroupAdditionalOptions->addButton(m_radioAddOptionsQCT, eDAOQCT);

			addWidget(pAdditionalOptions, m_nRow, 204, QMargins(10, 5, 0, 5));
			addWidget(m_radioAddOptionsNone, m_nRow, 132);
			addWidget(m_radioAddOptionsOnlyTrunk, m_nRow, 132);
			addWidget(m_radioAddOptionsQCT, m_nRow, 132);
			++m_nRow;
		}

		//++m_nRow;

		// io classification 체크 버튼.
		//	QLabel*  pLabelIOClassification = new QLabel(this);
		//	pLabelIOClassification->setText(QString("IO Classification"));
		//	m_chkIOClassificationPredict = new QCheckBox(this);
		//	m_chkIOClassificationPredict->setChecked(false);
		//	m_chkIOClassificationPredict->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		//	addWidget(pLabelIOClassification, m_nRow, 2, QMargins(10, 5, 0, 5));
		//	addWidget(m_chkIOClassificationPredict, m_nRow, 4);
		//	++m_nRow;



		// predict 버튼.
		m_btnPredict = new QPushButton(this);
		m_btnPredict->setText("Predict");
		m_btnPredict->setStyleSheet(STYLE_MANAGER->buttonBehind
			+ QString("QPushButton{min-width:%1px;}").arg(m_btnPredict->text().size() * 7));
		addWidget(m_btnPredict, m_nRow, 1, QMargins(3, 0, 1, 0));
		if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2) && !LICENSE_DATA->IsOnline()) //offline
		{
			QString strLicenseType = LICENSE_DATA->getLicenseType();

			m_pAvailableCnt = new QLineEdit(this);
			m_pAvailableCnt->setStyleSheet(STYLE_MANAGER->editBoxTab + QString("QLineEdit{color:yellow;}"));
			m_pAvailableCnt->setReadOnly(true);
			m_pAvailableCnt->setAlignment(Qt::AlignCenter);
			m_pAvailableCnt->setFixedWidth(50);


			if (!strLicenseType.compare(LICENSE_TYPE_TRIAL))
			{
				addWidget(m_pAvailableCnt, m_nRow, 1);
			}
			else if (!strLicenseType.compare(LICENSE_TYPE_SUBSCIPTION) || !strLicenseType.compare(LICENSE_TYPE_PERMANENT) || !strLicenseType.compare(LICENSE_TYPE_TEMPORARY))
			{
				m_pAvailableCnt->setHidden(true);
			}
			//else if (!strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_REJECTED))
			//{
			//	m_pAvailableCnt->setHidden(true);
			//	ButtonPredict->setDisabled(true);
			//}
			else
			{
				addWidget(m_pAvailableCnt, m_nRow, 1);
			}

		}
		else if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
		{
			sFuncUsableCountResult data{ 0, "" };
			int nCount = LICENSE_DATA->chkFunctionUsableCount((eMEDIP_FUNCTION_LEVEL)MFL_DeepCatchV2_DeepCatchV2Module_PredictUsableCount_Credit, eLSPTChkFunctionUsableCount, data);
			QString strPredictCntState = data.strFuncUsableCountState;

			m_pAvailableCnt = new QLineEdit(this);
			m_pAvailableCnt->setStyleSheet(STYLE_MANAGER->editBoxTab + QString("QLineEdit{color:yellow;}"));
			m_pAvailableCnt->setReadOnly(true);
			m_pAvailableCnt->setAlignment(Qt::AlignCenter);
			m_pAvailableCnt->setFixedWidth(50);


			if (!strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_NORMAL))
			{
				addWidget(m_pAvailableCnt, m_nRow, 1);
			}
			else if (!strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_UNLIMITED))
			{
				m_pAvailableCnt->setHidden(true);
			}
			else if (!strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_REJECTED))
			{
				m_pAvailableCnt->setHidden(true);
				m_btnPredict->setDisabled(true);
			}
			else
			{
				addWidget(m_pAvailableCnt, m_nRow, 1);
			}
		}

		++m_nRow;
		connect(m_btnPredict, &QPushButton::clicked, this, &AISegTabDeepCatch::slot_OnPredict);
	}

	// DeepCatch report 옵션.
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch) || m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
#if 0
		m_pWdgAbdomenOption = new QWidget(this);

		QBoxLayout* vLayout = new QVBoxLayout;
		vLayout->setContentsMargins(0, 0, 0, 0);
		m_pWdgAbdomenOption->setLayout(vLayout);

		QFrame* frLine = new QFrame(m_pWdgAbdomenOption);
		frLine->setStyleSheet("background-color:#414141;");
		frLine->setFrameShape(QFrame::HLine);
		frLine->setFrameShadow(QFrame::Sunken);
		frLine->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
		frLine->setContentsMargins(0, 0, 0, 5);
		vLayout->addWidget(frLine, 1);

		QBoxLayout* hLayout4 = new QHBoxLayout;
		vLayout->addLayout(hLayout4);
		hLayout4->setContentsMargins(10, 5, 0, 5);

		QLabel* pLabelGender = new QLabel(this);
		pLabelGender->setText(QString("Gender"));
		hLayout4->addWidget(pLabelGender, 204);

		m_radioGenderUnknown = new QRadioButton(this);
		m_radioGenderUnknown->setText(QString("Unknown"));
		m_radioGenderUnknown->setCheckable(true);
		m_radioGenderUnknown->setChecked(false);
		m_radioGenderUnknown->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_radioGenderFemale = new QRadioButton(this);
		m_radioGenderFemale->setText(QString("Female"));
		m_radioGenderFemale->setCheckable(true);
		m_radioGenderFemale->setChecked(false);
		m_radioGenderFemale->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_radioGenderMale = new QRadioButton(this);
		m_radioGenderMale->setText(QString("Male"));
		m_radioGenderMale->setCheckable(true);
		m_radioGenderMale->setChecked(false);
		m_radioGenderMale->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_btnGroupReporGender = new QButtonGroup(this);
		m_btnGroupReporGender->addButton(m_radioGenderUnknown, eDGTUnknown);
		m_btnGroupReporGender->addButton(m_radioGenderFemale, eDGTFemale);
		m_btnGroupReporGender->addButton(m_radioGenderMale, eDGTMale);

		hLayout4->addWidget(m_radioGenderUnknown, 132);
		hLayout4->addWidget(m_radioGenderFemale, 132);
		hLayout4->addWidget(m_radioGenderMale, 132);
		//	hLayout4->setContentsMargins(QMargins(10, 5, 0, 10));

		QBoxLayout* hLayout5 = new QHBoxLayout;
		vLayout->addLayout(hLayout5);
		hLayout5->setContentsMargins(10, 5, 0, 5);

		QLabel* pLabelHeight = new QLabel(this);
		pLabelHeight->setText(QString("Height"));
		hLayout5->addWidget(pLabelHeight, 1);

		m_pReportHeight = new QLineEdit(this);
		m_pReportHeight->setText("0");
		m_pReportHeight->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_pReportHeight->setAlignment(Qt::AlignCenter);
		hLayout5->addWidget(m_pReportHeight, 2);

		QLabel* pLabelCM = new QLabel(this);
		pLabelCM->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		pLabelCM->setText(QString("cm"));
		hLayout5->addWidget(pLabelCM, 1);

		QLabel* pLabelWeight = new QLabel(this);
		pLabelWeight->setText(QString("Weight"));
		hLayout5->addWidget(pLabelWeight, 1);

		m_pReportWeight = new QLineEdit(this);
		m_pReportWeight->setText("0");
		m_pReportWeight->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_pReportWeight->setAlignment(Qt::AlignCenter);
		hLayout5->addWidget(m_pReportWeight, 2);

		QLabel* pLabelKG = new QLabel(this);
		pLabelKG->setText(QString("kg"));
		pLabelKG->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		hLayout5->addWidget(pLabelKG, 1);


		/////////////////////////////////////////////////////////////////
		addWidget(m_pWdgAbdomenOption, m_nRow++, 1);

		m_strFileName = "WholeBody" + m_strOptContrast + m_strOptUNET;

		//////////////////////////////////////////////////////
		//m_pWdgReportOption = new QWidget(this);

		//QBoxLayout *vLayoutReport = new QVBoxLayout;
		//m_pWdgReportOption->setLayout(vLayoutReport);
		////////////////////////////////////////////////////
		// Preference for reporting
		/*
		frLine = new QFrame(m_pWdgAbdomenOption);
		frLine->setStyleSheet("background-color:#414141;");
		frLine->setFrameShape(QFrame::HLine);
		frLine->setFrameShadow(QFrame::Sunken);
		frLine->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
		vLayout->addWidget(frLine, 1);
		*/

		QLabel* pLabelReporting = new QLabel(this);
		pLabelReporting->setText(QString("Slice"));
		//	pLabelReporting->setContentsMargins(QMargins(0, 5, 0, 0));
		//	vLayout->addWidget(pLabelReporting, 1);

		m_btnGroupReportSlice = new QButtonGroup(this);

		m_radioReportSingle = new QRadioButton(this);
		m_radioReportSingle->setText(QString("L3"));
		m_radioReportSingle->setCheckable(true);
		m_radioReportSingle->setChecked(true);
		m_radioReportSingle->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_radioReportMulti = new QRadioButton(this);
		m_radioReportMulti->setText(QString("Abdominal Waist"));
		m_radioReportMulti->setCheckable(true);
		m_radioReportMulti->setChecked(false);
		m_radioReportMulti->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_btnGroupReportSlice->addButton(m_radioReportSingle, 0);
		m_btnGroupReportSlice->addButton(m_radioReportMulti, 1);
		//m_btnGroupReportSlice->addButton(m_radioReportNone, 2);

		QBoxLayout* hLayout3 = new QHBoxLayout;
		vLayout->addLayout(hLayout3);
		hLayout3->setContentsMargins(QMargins(10, 5, 0, 5));

		hLayout3->addWidget(pLabelReporting, 2);
		hLayout3->addWidget(m_radioReportSingle, 2);
		hLayout3->addWidget(m_radioReportMulti, 2);
		//hLayout3->addWidget(m_radioReportNone, 2, Qt::AlignCenter);

		// muscle classification 체크 버튼.
		QBoxLayout* hLayout7 = new QHBoxLayout;
		vLayout->addLayout(hLayout7);
		hLayout7->setContentsMargins(QMargins(10, 5, 0, 5));

		QLabel* pLabelMuscleClassification = new QLabel(this);
		pLabelMuscleClassification->setText(QString("Muscle Quality Map"));
		m_chkMuscleClassification = new QCheckBox(this);
		m_chkMuscleClassification->setChecked(false);
		m_chkMuscleClassification->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		hLayout7->addWidget(pLabelMuscleClassification, 2);
		hLayout7->addWidget(m_chkMuscleClassification, 4);

		m_pBtnMakeReport = new QPushButton(this);
		m_pBtnMakeReport->setObjectName("Make Report");
		m_pBtnMakeReport->setText(tr("Make Report"));
		m_pBtnMakeReport->setStyleSheet(STYLE_MANAGER->buttonBehind);
		m_pBtnMakeReport->setToolTip("Save mask Info to *.csv file.");
		vLayout->addWidget(m_pBtnMakeReport);
		connect(m_pBtnMakeReport, &QPushButton::clicked, this, &AISegTabDeepCatch::slot_OnMakeReport);
		addWidget(m_pWdgAbdomenOption, m_nRow++, 1);
#else
		QFrame* frLine = new QFrame();
		frLine->setStyleSheet("background-color:#414141;");
		frLine->setFrameShape(QFrame::HLine);
		frLine->setFrameShadow(QFrame::Sunken);
		frLine->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
		frLine->setContentsMargins(0, 0, 0, 5);
		addWidget(frLine, m_nRow, 1);
		m_nRow++;

		QLabel* pLabelGender = new QLabel(this);
		pLabelGender->setText(QString("Gender"));
		addWidget(pLabelGender, m_nRow, 204, QMargins(10, 5, 0, 5));

		m_radioGenderUnknown = new QRadioButton(this);
		m_radioGenderUnknown->setText(QString("Unknown"));
		m_radioGenderUnknown->setCheckable(true);
		m_radioGenderUnknown->setChecked(false);
		m_radioGenderUnknown->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_radioGenderFemale = new QRadioButton(this);
		m_radioGenderFemale->setText(QString("Female"));
		m_radioGenderFemale->setCheckable(true);
		m_radioGenderFemale->setChecked(false);
		m_radioGenderFemale->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_radioGenderMale = new QRadioButton(this);
		m_radioGenderMale->setText(QString("Male"));
		m_radioGenderMale->setCheckable(true);
		m_radioGenderMale->setChecked(false);
		m_radioGenderMale->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_btnGroupReporGender = new QButtonGroup(this);
		m_btnGroupReporGender->addButton(m_radioGenderUnknown, eDGTUnknown);
		m_btnGroupReporGender->addButton(m_radioGenderFemale, eDGTFemale);
		m_btnGroupReporGender->addButton(m_radioGenderMale, eDGTMale);

		addWidget(m_radioGenderUnknown, m_nRow, 132);
		addWidget(m_radioGenderFemale, m_nRow, 132);
		addWidget(m_radioGenderMale, m_nRow, 132);
		m_nRow++;

		QLabel* pLabelHeight = new QLabel(this);
		pLabelHeight->setText(QString("Height"));
		addWidget(pLabelHeight, m_nRow, 1, QMargins(10, 5, 0, 5));

		m_pReportHeight = new QLineEdit(this);
		m_pReportHeight->setText("0");
		m_pReportHeight->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_pReportHeight->setAlignment(Qt::AlignCenter);
		addWidget(m_pReportHeight, m_nRow, 2);

		QLabel* pLabelCM = new QLabel(this);
		pLabelCM->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		pLabelCM->setText(QString("cm"));
		addWidget(pLabelCM, m_nRow, 1);

		QLabel* pLabelWeight = new QLabel(this);
		pLabelWeight->setText(QString("Weight"));
		addWidget(pLabelWeight, m_nRow, 1);

		m_pReportWeight = new QLineEdit(this);
		m_pReportWeight->setText("0");
		m_pReportWeight->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_pReportWeight->setAlignment(Qt::AlignCenter);
		addWidget(m_pReportWeight, m_nRow, 2);

		QLabel* pLabelKG = new QLabel(this);
		pLabelKG->setText(QString("kg"));
		pLabelKG->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		addWidget(pLabelKG, m_nRow, 1);
		m_nRow++;

		m_strFileName = "WholeBody" + m_strOptContrast + m_strOptUNET;

		QLabel* pLabelReporting = new QLabel(this);
		pLabelReporting->setText(QString("Slice"));

		m_btnGroupReportSlice = new QButtonGroup(this);

		m_radioReportSingle = new QRadioButton(this);
		m_radioReportSingle->setText(QString("L3"));
		m_radioReportSingle->setCheckable(true);
		m_radioReportSingle->setChecked(true);
		m_radioReportSingle->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_radioReportMulti = new QRadioButton(this);
		m_radioReportMulti->setText(QString("Abdominal Waist"));
		m_radioReportMulti->setCheckable(true);
		m_radioReportMulti->setChecked(false);
		m_radioReportMulti->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_btnGroupReportSlice->addButton(m_radioReportSingle, 0);
		m_btnGroupReportSlice->addButton(m_radioReportMulti, 1);
		//m_btnGroupReportSlice->addButton(m_radioReportNone, 2);

		addWidget(pLabelReporting, m_nRow, 2, QMargins(10, 5, 0, 5));
		addWidget(m_radioReportSingle, m_nRow, 2);
		addWidget(m_radioReportMulti, m_nRow, 2);
		m_nRow++;
		//hLayout3->addWidget(m_radioReportNone, 2, Qt::AlignCenter);

		// muscle classification 체크 버튼.
		QLabel* pLabelMuscleClassification = new QLabel(this);
		pLabelMuscleClassification->setText(QString("Muscle Quality Map"));
		m_chkMuscleClassification = new QCheckBox(this);
		m_chkMuscleClassification->setChecked(false);
		m_chkMuscleClassification->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		addWidget(pLabelMuscleClassification, m_nRow, 2, QMargins(10, 5, 0, 5));
		addWidget(m_chkMuscleClassification, m_nRow, 4);
		m_nRow++;

		connect(m_chkMuscleClassification, &QCheckBox::clicked, this, &AISegTabDeepCatch::slot_OnMuscleQualityMap);

		// Muscle_adp_tissue
		m_labelMuscleAdpTissue = new QLabel("Adipose tissue HU Range", this);

		m_textMuscleAdpTissue_startHU = new QLineEdit(this);
		m_textMuscleAdpTissue_startHU->setText("-190");
		m_textMuscleAdpTissue_startHU->setObjectName("Adp_tissue_EditStart");
		m_textMuscleAdpTissue_startHU->setValidator(new QIntValidator(this));
		m_textMuscleAdpTissue_startHU->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textMuscleAdpTissue_startHU->installEventFilter(this);
		m_textMuscleAdpTissue_startHU->setMouseTracking(true);
		connect(m_textMuscleAdpTissue_startHU, &QLineEdit::editingFinished, this, &AISegTabDeepCatch::slot_OnStartTextChanged_MuscleAdpTissue);
		connect(m_textMuscleAdpTissue_startHU, &QLineEdit::returnPressed, this, &AISegTabDeepCatch::slot_OnStartTextChanged_MuscleAdpTissue);

		m_labelMuscleAdpTissue_rangChar = new QLabel("~", this);

		m_textMuscleAdpTissue_endHU = new QLineEdit(this);
		m_textMuscleAdpTissue_endHU->setText("-30");
		m_textMuscleAdpTissue_endHU->setObjectName("Adp_tissue_EditEnd");
		m_textMuscleAdpTissue_endHU->setValidator(new QIntValidator(this));
		m_textMuscleAdpTissue_endHU->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textMuscleAdpTissue_endHU->installEventFilter(this);
		m_textMuscleAdpTissue_endHU->setMouseTracking(true);
		connect(m_textMuscleAdpTissue_endHU, &QLineEdit::editingFinished, this, &AISegTabDeepCatch::slot_OnEndTextChanged_MuscleAdpTissue);
		connect(m_textMuscleAdpTissue_endHU, &QLineEdit::returnPressed, this, &AISegTabDeepCatch::slot_OnEndTextChanged_MuscleAdpTissue);

		addWidget(m_labelMuscleAdpTissue, m_nRow, 2, QMargins(20, 0, 0, 0));
		addWidget(m_textMuscleAdpTissue_startHU, m_nRow, 1);
		addWidget(m_labelMuscleAdpTissue_rangChar, m_nRow);
		addWidget(m_textMuscleAdpTissue_endHU, m_nRow, 1);
		m_nRow++;

		// Muscle_abnormal1
		m_labelMuscleAbnormal1 = new QLabel("Abnormal 1 HU Range", this);

		m_textMuscleAbnormal1_startHU = new QLineEdit(this);
		m_textMuscleAbnormal1_startHU->setText("-29");
		m_textMuscleAbnormal1_startHU->setObjectName("Abnormal1_EditStart");
		m_textMuscleAbnormal1_startHU->setValidator(new QIntValidator(this));
		m_textMuscleAbnormal1_startHU->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textMuscleAbnormal1_startHU->installEventFilter(this);
		m_textMuscleAbnormal1_startHU->setMouseTracking(true);
		connect(m_textMuscleAbnormal1_startHU, &QLineEdit::editingFinished, this, &AISegTabDeepCatch::slot_OnStartTextChanged_MuscleAbnormal1);
		connect(m_textMuscleAbnormal1_startHU, &QLineEdit::returnPressed, this, &AISegTabDeepCatch::slot_OnStartTextChanged_MuscleAbnormal1);

		m_labelMuscleAbnormal1_rangChar = new QLabel("~", this);

		m_textMuscleAbnormal1_endHU = new QLineEdit(this);
		m_textMuscleAbnormal1_endHU->setText("0");
		m_textMuscleAbnormal1_endHU->setObjectName("Abnormal1_EditEnd");
		m_textMuscleAbnormal1_endHU->setValidator(new QIntValidator(this));
		m_textMuscleAbnormal1_endHU->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textMuscleAbnormal1_endHU->installEventFilter(this);
		m_textMuscleAbnormal1_endHU->setMouseTracking(true);
		connect(m_textMuscleAbnormal1_endHU, &QLineEdit::editingFinished, this, &AISegTabDeepCatch::slot_OnEndTextChanged_MuscleAbnormal1);
		connect(m_textMuscleAbnormal1_endHU, &QLineEdit::returnPressed, this, &AISegTabDeepCatch::slot_OnEndTextChanged_MuscleAbnormal1);

		addWidget(m_labelMuscleAbnormal1, m_nRow, 2, QMargins(20, 0, 0, 0));
		addWidget(m_textMuscleAbnormal1_startHU, m_nRow, 1);
		addWidget(m_labelMuscleAbnormal1_rangChar, m_nRow);
		addWidget(m_textMuscleAbnormal1_endHU, m_nRow, 1);
		m_nRow++;

		// Muscle_abnormal2
		m_labelMuscleAbnormal2 = new QLabel("Abnormal 2 HU Range", this);

		m_textMuscleAbnormal2_startHU = new QLineEdit(this);
		m_textMuscleAbnormal2_startHU->setText("1");
		m_textMuscleAbnormal2_startHU->setObjectName("Abnormal2_EditStart");
		m_textMuscleAbnormal2_startHU->setValidator(new QIntValidator(this));
		m_textMuscleAbnormal2_startHU->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textMuscleAbnormal2_startHU->installEventFilter(this);
		m_textMuscleAbnormal2_startHU->setMouseTracking(true);
		connect(m_textMuscleAbnormal2_startHU, &QLineEdit::editingFinished, this, &AISegTabDeepCatch::slot_OnStartTextChanged_MuscleAbnormal2);
		connect(m_textMuscleAbnormal2_startHU, &QLineEdit::returnPressed, this, &AISegTabDeepCatch::slot_OnStartTextChanged_MuscleAbnormal2);

		m_labelMuscleAbnormal2_rangChar = new QLabel("~", this);

		m_textMuscleAbnormal2_endHU = new QLineEdit(this);
		m_textMuscleAbnormal2_endHU->setText("29");
		m_textMuscleAbnormal2_endHU->setObjectName("Abnormal2_EditEnd");
		m_textMuscleAbnormal2_endHU->setValidator(new QIntValidator(this));
		m_textMuscleAbnormal2_endHU->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textMuscleAbnormal2_endHU->installEventFilter(this);
		m_textMuscleAbnormal2_endHU->setMouseTracking(true);
		connect(m_textMuscleAbnormal2_endHU, &QLineEdit::editingFinished, this, &AISegTabDeepCatch::slot_OnEndTextChanged_MuscleAbnormal2);
		connect(m_textMuscleAbnormal2_endHU, &QLineEdit::returnPressed, this, &AISegTabDeepCatch::slot_OnEndTextChanged_MuscleAbnormal2);

		addWidget(m_labelMuscleAbnormal2, m_nRow, 2, QMargins(20, 0, 0, 0));
		addWidget(m_textMuscleAbnormal2_startHU, m_nRow, 1);
		addWidget(m_labelMuscleAbnormal2_rangChar, m_nRow);
		addWidget(m_textMuscleAbnormal2_endHU, m_nRow, 1);
		m_nRow++;

		// Muscle_normal
		m_labelMuscleNormal = new QLabel("Normal HU Range", this);

		m_textMuscleNormal_startHU = new QLineEdit(this);
		m_textMuscleNormal_startHU->setText("30");
		m_textMuscleNormal_startHU->setObjectName("Normal_EditStart");
		m_textMuscleNormal_startHU->setValidator(new QIntValidator(this));
		m_textMuscleNormal_startHU->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textMuscleNormal_startHU->installEventFilter(this);
		m_textMuscleNormal_startHU->setMouseTracking(true);
		connect(m_textMuscleNormal_startHU, &QLineEdit::editingFinished, this, &AISegTabDeepCatch::slot_OnStartTextChanged_MuscleNormal);
		connect(m_textMuscleNormal_startHU, &QLineEdit::returnPressed, this, &AISegTabDeepCatch::slot_OnStartTextChanged_MuscleNormal);

		m_labelMuscleNormal_rangChar = new QLabel("~", this);

		m_textMuscleNormal_endHU = new QLineEdit(this);
		m_textMuscleNormal_endHU->setText("150");
		m_textMuscleNormal_endHU->setObjectName("Normal_EditEnd");
		m_textMuscleNormal_endHU->setValidator(new QIntValidator(this));
		m_textMuscleNormal_endHU->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textMuscleNormal_endHU->installEventFilter(this);
		m_textMuscleNormal_endHU->setMouseTracking(true);
		connect(m_textMuscleNormal_endHU, &QLineEdit::editingFinished, this, &AISegTabDeepCatch::slot_OnEndTextChanged_MuscleNormal);
		connect(m_textMuscleNormal_endHU, &QLineEdit::returnPressed, this, &AISegTabDeepCatch::slot_OnEndTextChanged_MuscleNormal);

		addWidget(m_labelMuscleNormal, m_nRow, 2, QMargins(20, 0, 0, 0));
		addWidget(m_textMuscleNormal_startHU, m_nRow, 1);
		addWidget(m_labelMuscleNormal_rangChar, m_nRow);
		addWidget(m_textMuscleNormal_endHU, m_nRow, 1);
		m_nRow++;

		emit m_chkMuscleClassification->clicked();

		m_pBtnMakeReport = new QPushButton(this);
		m_pBtnMakeReport->setObjectName("Make Report");
		m_pBtnMakeReport->setText(tr("Make Report"));
		m_pBtnMakeReport->setStyleSheet(STYLE_MANAGER->buttonBehind);
		m_pBtnMakeReport->setToolTip("Save mask Info to *.csv file.");
		addWidget(m_pBtnMakeReport, m_nRow, 1, QMargins(0, 5, 0, 0));
		connect(m_pBtnMakeReport, &QPushButton::clicked, this, &AISegTabDeepCatch::slot_OnMakeReport);
#endif
	}

	if (m_ComboDeepCatchModule)
	{
		int nCBIdx = 0;		// 현재 총4개
		if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch) == eAVAILABLE_STATE::CREATE ||
			m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2) == eAVAILABLE_STATE::CREATE)
		{
			/* jhc [22.07.26] http rest api 호출 최적화로 DeepCatch 로딩시 2회 usable count를 확인하는데 1회로 변경.
			m_nPredictCnt = 1;
			#else
			m_nPredictCnt = LICENSE_DATA->chkFunctionUsableCount(MFL_DeepCatch_DeepCatchModule_PredictUsableCount_Credit, eLSPTChkFunctionUsableCount);
			#endif
			*/

			// Wholebody & Abdomen			
			m_ComboDeepCatchModule->addItem(QString("Wholebody & ") + QString::fromStdString(m_pProductManager->m_strDeepCatchAbdomen));
			m_ComboDeepCatchModule->setItemData(nCBIdx++, QVariant(QString::fromStdString(m_pProductManager->m_strDeepCatchAbdomen)));

			//// chest
			//	m_ComboDeepCatchModule->addItem(QString::fromStdString(m_pProductManager->m_strDeepCatchChest) + QString::fromStdString("  (comming soon)"));
			//	m_ComboDeepCatchModule->setItemData(nCBIdx, QVariant(QString::fromStdString(m_pProductManager->m_strDeepCatchChest)));

			connect(m_ComboDeepCatchModule, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnComboChangedAIModule(int)));

			// config
			QString strVal;
			if (m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_Project, strVal))
			{
				int idx = m_ComboDeepCatchModule->findData(QVariant(strVal));
				if (idx >= 0)
				{
					m_ComboDeepCatchModule->setCurrentIndex(idx);
					slot_OnComboChangedAIModule(idx);
				}
				else
				{
					slot_OnComboChangedAIModule(0);
				}
			}
			else
			{
				slot_OnComboChangedAIModule(0);
			}
		}
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{

		if (m_btnGroupBodyComposition)
		{
			connect(m_btnGroupBodyComposition, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnBodyComposition(int)));

			// config
			QString strVal;
			if (m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_BodyComposition, strVal))
			{
				int idx = strVal.toInt();
				if (idx >= 0)
				{
					QRadioButton* pBtn = (QRadioButton*)m_btnGroupBodyComposition->button(idx);
					pBtn->setChecked(true);
					slot_OnBodyComposition(idx);
				}
			}
			else
				slot_OnBodyComposition(eDCBTAbdomenWholeBody);
		}

		if (m_btnGroupBoneAnalysis)
		{
			connect(m_btnGroupBoneAnalysis, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnBoneAnalysis(int)));

			// config
			QString strVal;
			if (m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_BoneAnalysis, strVal))
			{
				int idx = strVal.toInt();
				if (idx >= 0)
				{
					QRadioButton* pBtn = (QRadioButton*)m_btnGroupBoneAnalysis->button(idx);
					pBtn->setChecked(true);
					slot_OnBoneAnalysis(idx);
				}
			}
			else
				slot_OnBoneAnalysis(eDCVTNone);
		}

		if (m_checkIOClassificationLiverSpleen)
		{
			// config
			QString strVal;
			if (m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_IO_Network, strVal))
			{
				int idx = strVal.toInt();
				m_nIOClassification = idx;
				if (idx == eDCIO_Liver_Spleen || idx == eDCIO_Both)
					m_checkIOClassificationLiverSpleen->setChecked(true);
				else
					m_checkIOClassificationLiverSpleen->setChecked(false);
			}
			else
				m_checkIOClassificationLiverSpleen->setChecked(false);
		}
		if (m_checkIOClassificationAorta)
		{
			// config
			QString strVal;
			if (m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_IO_Network, strVal))
			{
				int idx = strVal.toInt();
				m_nIOClassification = idx;
				if (idx == eDCIO_Aorta || idx == eDCIO_Both)
					m_checkIOClassificationAorta->setChecked(true);
				else
					m_checkIOClassificationAorta->setChecked(false);
			}
			else
				m_checkIOClassificationAorta->setChecked(false);
		}


		if (m_comboSingleSlice)
		{
			int idx = setComboConfigValue(m_comboSingleSlice, STRING_MANAGER->m_config_DeepCatch_SingleSlice);
			m_nSingleSliceNum = idx;
		}

		if (m_comboMultiSliceLower)
		{
			int idx = setComboConfigValue(m_comboMultiSliceLower, STRING_MANAGER->m_config_DeepCatch_MultiSliceUp, eDCVB_Abdominal_waist);
			m_nMultiSliceUpNum = idx;
		}

		if (m_comboMultiSliceUpper)
		{
			int idx = setComboConfigValue(m_comboMultiSliceUpper, STRING_MANAGER->m_config_DeepCatch_MultiSliceLow, eDCVB_Abdominal_waist);
			m_nMultiSliceLowNum = idx;
		}
	}
	else
	{
		if (m_btnGroupUNET)
		{
			connect(m_btnGroupUNET, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnChange2D3D(int)));

			// config
			QString strVal;
			if (m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_Predict_UNET, strVal))
			{
				int idx = strVal.toInt();
				if (idx >= 0)
				{
					QRadioButton* pBtn = (QRadioButton*)m_btnGroupUNET->button(idx);
					pBtn->setChecked(true);
					slot_OnChange2D3D(idx);
				}
			}
		}

		if (m_btnGroupContrast)
		{
			connect(m_btnGroupContrast, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnChangeContrast(int)));

			// config
			QString strVal;
			if (m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_Contrast, strVal))
			{
				int idx = strVal.toInt();
				if (idx >= 0)
				{
					QRadioButton* pBtn = (QRadioButton*)m_btnGroupContrast->button(idx);
					pBtn->setChecked(true);
					slot_OnChangeContrast(idx);
				}
			}
		}

		if (m_btnGroupAWPredict)
		{
			connect(m_btnGroupAWPredict, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnChangeAWPredict(int)));

			// config
			QString strVal;
			if (m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_AWPredict, strVal))
			{
				int idx = strVal.toInt();
				if (idx >= 0)
				{
					QRadioButton* pBtn = (QRadioButton*)m_btnGroupAWPredict->button(idx);
					pBtn->setChecked(true);
					slot_OnChangeAWPredict(idx);
				}
			}
		}
	}


	// additional options
	if (m_btnGroupAdditionalOptions)
	{
		connect(m_btnGroupAdditionalOptions, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnChangeAddtionalOptions(int)));

		// config
		QString strVal;
		if (m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_AdditionalOptions, strVal))
		{
			int idx = strVal.toInt();
			if (idx >= 0)
			{
				QRadioButton* pBtn = (QRadioButton*)m_btnGroupAdditionalOptions->button(idx);
				pBtn->setChecked(true);
				slot_OnChangeAddtionalOptions(idx);
			}
		}
	}

	// IO Classification 옵션.
	if (m_chkIOClassificationPredict)
	{
		// config
		QString strVal;
		if (m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_IOClassificationPredict, strVal))
		{
			int idx = strVal.toInt();
			if (idx >= 0)
				m_chkIOClassificationPredict->setChecked(idx == 0 ? false : true);
		}
	}

	// reprort slice 옵션.
	if (m_btnGroupReportSlice)
	{
		connect(m_btnGroupReportSlice, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnChangePreferReporting(int)));

		// config
		QString strVal;
		if (m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_PreferenceForReporting, strVal))
		{
			int idx = strVal.toInt();
			if (idx >= 0)
			{
				QRadioButton* pBtn = (QRadioButton*)m_btnGroupReportSlice->button(idx);
				pBtn->setChecked(true);
				slot_OnChangePreferReporting(idx);
			}
		}
	}

	// muscle quality map
	if (m_chkMuscleClassification)
	{
		// config
		QString strVal;
		if (m_pWinManager->getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_MuslceQualityMap, strVal))
		{
			int idx = strVal.toInt();
			if (idx >= 0)
			{
				m_chkMuscleClassification->setChecked(idx == 0 ? false : true);
				emit m_chkMuscleClassification->clicked();
			}
		}
	}

	if (m_btnGroupReporGender)
	{
		connect(m_btnGroupReporGender, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnChangeReportGender(int)));
		slot_OnChangeReportGender(eDGTUnknown);
		m_radioGenderUnknown->setChecked(true);
	}

	if (m_pReportHeight)
	{
		connect(m_pReportHeight, &QLineEdit::textChanged, this, &AISegTabDeepCatch::slot_OnChangeHeight);
	}

	if (m_pReportWeight)
	{
		connect(m_pReportWeight, &QLineEdit::textChanged, this, &AISegTabDeepCatch::slot_OnChangeWeight);
	}

	m_nRow++;

	QWidget* emptyBox0;
	emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	addWidget(emptyBox0, m_nRow++);

	updateHeight();
	updateGeometry();

	setOpenWidget(true);
}

void AISegTabDeepCatch::StartPredict(QString strProjectName)
{
	QString strProgressBarMsg = QString("DeepCatch - Segmentation");
	eDeepcatchUNETType n2D3D = eDeepcatchUNETType::eDUT2D;
	if (m_strOptUNET.compare("2D") == 0)
	{
		n2D3D = eDeepcatchUNETType::eDUT2D;
	}
	else if (m_strOptUNET.compare("3D") == 0)
	{
		n2D3D = eDeepcatchUNETType::eDUT3D;
	}

	if (m_pActionManager->m_IsMacroMode == false)
	{
		if (isAvailablePredict(n2D3D) == false)
		{
			return;
		}
	}

	
	if (!m_pVolumeData->isValidate())
	{
		return;
	}

	if (getAdditinalOptions() == eDAOQCT)
	{
		strProgressBarMsg = QString("DeepCatch - QCT Segmentation");
		strProjectName = "QCT_Trunk";
		m_pActionManager->SetCurrentThread(THREAD_QCT_PREDICT);
	}


	bool bVB = false;
	bool bVB_LiverSpleen = false;
	bool bVB_Aorta = false;
	bool bVB_LiverSpleen_Aorta = false;
	bool bLiverSpleen = false;
	bool bAorta = false;
	bool bLiverSpleen_Aorta = false;
	qint8 BodyType;
	qint8 VBNetType;
	qint8 IONetType;

	qint8 single;
	qint8 multiUp;
	qint8 multiLow;

	DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->DeepcatchReportPredictedInfoSafeCreate();
	BodyType = getBodyComposition();	 // 0: Wholebody & Abdomen, 1:Chest
	VBNetType = getBoneAnalysis();	 // 0: Vertebra Network, 1: nothing
	IONetType = getIOClass();	 // 0: liver/spleen, 1: aorta, 2: both, 3: nothing

	single = getSingleSliceNum();	 // 0~16 : T1~L5
	multiUp = getMultiSliceUpNum();	 // 0~16 : T1~L5
	multiLow = getMultiSliceLowNum();	 // 0~16 : T1~L5

	if (VBNetType == eUseVertebraNetwork && IONetType == eDCIOTNone)			bVB = true;
	if (VBNetType == eUseVertebraNetwork && IONetType == eDCIO_Liver_Spleen)	bVB_LiverSpleen = true;
	if (VBNetType == eUseVertebraNetwork && IONetType == eDCIO_Aorta)			bVB_Aorta = true;
	if (VBNetType == eUseVertebraNetwork && IONetType == eDCIO_Both)			bVB_LiverSpleen_Aorta = true;
	if (VBNetType == eDCVTNone && IONetType == eDCIO_Liver_Spleen)				bLiverSpleen = true;
	if (VBNetType == eDCVTNone && IONetType == eDCIO_Aorta)						bAorta = true;
	if (VBNetType == eDCVTNone && IONetType == eDCIO_Both)						bLiverSpleen_Aorta = true;

	int start, end;
	BoundingBoxI box = getValidRegion();
	QString filePath = m_strWeightPath + QString("/%1.mipx").arg(strProjectName);

	QFile file(filePath);

	bool res = file.exists();
	bool predict_res = false;
	if (res)
	{
		if (m_pActionManager->m_IsMacroMode == false)
		{
			if (!getchkIOClassification())
			{
				switch (getAdditinalOptions())
				{
				case eDAONone:
				{
					//SUPPORT_DEEPCATCH_VERSION_2
					if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
					{
						if (bVB)
						{
							std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
							ACTION_PROCESSING Action_Proc = ACTP_NONE;
							Action_Proc = (ACTION_PROCESSING)pushBackVBThread(single, multiUp, multiLow);
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(Action_Proc, THREAD_NONE, nullptr));
						}
						else if (bVB_LiverSpleen/*bUseMANet*/)
						{
							std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
							ACTION_PROCESSING Action_Proc = ACTP_NONE;
							Action_Proc = (ACTION_PROCESSING)pushBackVBThread(single, multiUp, multiLow);

							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(Action_Proc, THREAD_IO_LIVER_SPLEEN_PREDICT, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_LIVER_SPLEEN, THREAD_NONE, nullptr));
						}
						else if (bVB_Aorta/*bUseMANet*/)
						{
							std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
							ACTION_PROCESSING Action_Proc = ACTP_NONE;
							Action_Proc = (ACTION_PROCESSING)pushBackVBThread(single, multiUp, multiLow);

							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(Action_Proc, THREAD_IO_AORTA_PREDICT, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_AORTA, THREAD_NONE, nullptr));
						}
						else if (bVB_LiverSpleen_Aorta/*bUseMANet*/)
						{
							std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
							ACTION_PROCESSING Action_Proc = ACTP_NONE;
							Action_Proc = (ACTION_PROCESSING)pushBackVBThread(single, multiUp, multiLow);

							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(Action_Proc, THREAD_IO_LIVER_SPLEEN_PREDICT, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_LIVER_SPLEEN, THREAD_IO_AORTA_PREDICT, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_AORTA, THREAD_NONE, nullptr));
						}
						else if (bLiverSpleen/*bUseMANet*/)
						{
							std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));//20220622_add_Thread_byPHS						
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_IO_LIVER_SPLEEN_PREDICT, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_LIVER_SPLEEN, THREAD_NONE, nullptr));
						}
						else if (bAorta/*bUseMANet*/)
						{
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_AORTA, THREAD_L3_AORTA_NONE, nullptr));
						}
						else if (bLiverSpleen_Aorta/*bUseMANet*/)
						{
							std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));//20220622_add_Thread_byPHS						
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_IO_LIVER_SPLEEN_PREDICT, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_LIVER_SPLEEN, THREAD_IO_AORTA_PREDICT, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_IO_AORTA, THREAD_NONE, nullptr));
						}
						else
						{
							std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
							m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_NONE, nullptr));
						}
					}
					else
					{
						std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
						m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
						m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
						m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
						m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_NONE, nullptr));
					}
				}
				break;
				case eDAOOnlyTrunk:
				{
					std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_TRUNK_PREDICT, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_TRUNK_PREDICT_APPLY, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_HOLE_FILLING, THREAD_NONE, nullptr));
				}
				break;
				case eDAOQCT:
				{
					std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPCATCH_DEEPDRAW_PREDICT, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_NONE, nullptr));
				}
				break;
				}
			}
			// io classification predict 추가.
			else
			{
				switch (getAdditinalOptions())
				{
				case eDAONone:
				{
					std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_IO_CLASSIFICATION_PREDICT, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_IO_CLASSIFICATION_PREDICT2, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_L3_PREDICT, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_NONE, nullptr));
					//m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE, THREAD_NONE, nullptr));

					//	m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_CIRCUMFERENCE, THREAD_SAVE_3D_VOLUME, nullptr));
					//	m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_SAVE_3D_VOLUME, THREAD_DEEPCATCH_REPORT, nullptr));
				}
				break;
				case eDAOOnlyTrunk:
				{
					std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_IO_CLASSIFICATION_PREDICT, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_IO_CLASSIFICATION_PREDICT2, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_L3_PREDICT, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_TRUNK_PREDICT, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_TRUNK_PREDICT_APPLY, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_HOLE_FILLING, THREAD_NONE, nullptr));
				}
				break;
				case eDAOQCT:
				{
					std::deque<ActionThreadArgument>().swap(m_pActionManager->m_qThreadNext);
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_DEEPCATCH_DEEPDRAW_PREDICT, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_IO_CLASSIFICATION_PREDICT, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_IO_CLASSIFICATION_PREDICT2, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_L3_PREDICT, nullptr));

					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
					m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_NONE, nullptr));
				}
				break;
				}
			}
			m_pActionManager->m_nCurrentThreadCount = m_pActionManager->m_nMultiThreadTotalCount = m_pActionManager->m_qThreadNext.size();
		}

		// Report data 생성
		DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->DeepcatchReportPredictedInfoSafeCreate();

		// deepcatch ui setting info 등록.		
		pPredictedInfo->stPredictOpt.nProjectType = (DEEPCATCH_TYPE_PREDICT)getProjectType();
		pPredictedInfo->stPredictOpt.UNETType = (eDeepcatchUNETType)getUNET();
		pPredictedInfo->stPredictOpt.contrastType = (eDeepcatchContrastType)getContrast();
		pPredictedInfo->stPredictOpt.AWConfirmType = (eDeepcatchAWSelectType)getAWPredict();
		pPredictedInfo->stPredictOpt.AdditionalOptions = (eDeepcatchAdditionalOptions)getAdditinalOptions();
		pPredictedInfo->stPredictOpt.bIOClassificationPredict = getchkIOClassification();
		//SUPPORT_DEEPCATCH_VERSION_2
		pPredictedInfo->stPredictOpt.BodyCompositionType = getBodyComposition();
		pPredictedInfo->stPredictOpt.useVBNetwork = getBoneAnalysis();
		pPredictedInfo->stPredictOpt.useIONetwork = getIOClass();

		pPredictedInfo->stPredictOpt.singleSliceNum = getSingleSliceNum();
		pPredictedInfo->stPredictOpt.multiSliceUpperNum = getMultiSliceUpNum();
		pPredictedInfo->stPredictOpt.multiSliceLowerNum = getMultiSliceLowNum();

		// date
		QDateTime date;
		QString strTime;
#if 0
		if (network::GetServerTime(date))
		{
			//str = date.toString("yyyy-MM-dd HH:mm:ss");		// 장과장님 질문
			strTime = date.toString("yyyy-MM-dd");
		}
		else
#else	// 로컬 타임으로만 처리.
		strTime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
#endif
		pPredictedInfo->stPredictOpt.strReportingDate = strTime;



		//SUPPORT_DEEPCATCH_VERSION_2
		if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
		{
			if (bAorta && m_pActionManager->m_IsMacroMode == false)
			{
				std::vector<pair<int, std::vector<unsigned char>>>().swap(m_pVolumeData->m_vecAIResultData);
				m_pActionManager->action_IO_Aorta_predict(m_strInputPath, m_strWeightPath, MFL_Product_DeepCatch_DeepCatchV2, PROJNAME_IO_AORTA_CALSS, ACTP_DEEPCATCH_IO_AORTA, box, m_GPU);
				FreezeDeepCatchProject(true);
			}
			else
			{
				std::vector<pair<int, std::vector<unsigned char>>>().swap(m_pVolumeData->m_vecAIResultData);
				m_pActionManager->action_MultiConnect_predict(strProgressBarMsg, m_strInputPath, m_strWeightPath, MFL_Product_DeepCatch_DeepCatchV2, strProjectName, box, m_GPU);
				FreezeDeepCatchProject(true);
			}
		}
		else
		{
			std::vector<pair<int, std::vector<unsigned char>>>().swap(m_pVolumeData->m_vecAIResultData);
			m_pActionManager->action_MultiConnect_predict(strProgressBarMsg, m_strInputPath, m_strWeightPath, MFL_Product_DeepCatch, strProjectName, box, m_GPU);
			FreezeDeepCatchProject(true);
		}

	}
	else
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), "Before you can run Predict, you need to run Train or choose other trained project.");
		return;
	}

}

void AISegTabDeepCatch::slot_OnPredict()
{
	/////////////////////////////////////////////////
	m_pWinManager->mainSegmentWidget->setWorkMode(WORK_NONE);

	// chkFunctionUsableCount
	//#ifdef SUPPORT_DEEPCATCH_VERSION_2
	int nCount = 0;
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch) == eAVAILABLE_STATE::CREATE &&
		m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2) != eAVAILABLE_STATE::CREATE)
	{
		nCount = LICENSE_DATA->chkFunctionUsableCount((eMEDIP_FUNCTION_LEVEL)MFL_DeepCatch_DeepCatchModule_PredictUsableCount_Credit, eLSPTChkFunctionUsableCount);
	}
	else if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2) == eAVAILABLE_STATE::CREATE)
	{
		sFuncUsableCountResult data{ 0, "" };
		nCount = LICENSE_DATA->chkFunctionUsableCount((eMEDIP_FUNCTION_LEVEL)MFL_DeepCatchV2_DeepCatchV2Module_PredictUsableCount_Credit, eLSPTChkFunctionUsableCount, data);
		QString strPredictCntState = data.strFuncUsableCountState;

		QString strLicenseType = LICENSE_DATA->getLicenseType();

		if (!strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_UNLIMITED) ||
			(!strLicenseType.compare(LICENSE_TYPE_SUBSCIPTION) || !strLicenseType.compare(LICENSE_TYPE_PERMANENT) || !strLicenseType.compare(LICENSE_TYPE_TEMPORARY)))
		{
			nCount = 1;
		}
	}

	//int nCount = 1;
	if (nCount > 0)
	{
		if (m_pActionManager->m_IsMacroMode == false)
		{
#ifndef DEEPCATCH_REPORT_MASK_UID_USE
			std::vector<muint32> indeces;
			MaskInfo* pMaskInfo = nullptr;
			for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
			{
				pMaskInfo = m_pVolumeData->getMaskInfo(i, false);
				QString maskName = QString::fromWCharArray(pMaskInfo->maskName);
				if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_L3)
					|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST)
					|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_SKIN)
					|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_BONE)
					|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE)
					|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_AVF)
					|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_OF)
					|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO)
					|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_BSC))
				{
					indeces.push_back(i);
				}
				// io classification predict 진행시 io 관련된 마스크들도 삭제.
				if (getchkIOClassification())
				{
					if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_LIVER)
						|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_GB)
						|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_SPLEEN)
						|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_LEFTKIDNEY)
						|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_RIGHTKIDNEY)
						|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_BLADDER)
						|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_PANCREAS))
					{
						indeces.push_back(i);
					}
				}
			}
#endif

			//SUPPORT_DEEPCATCH_VERSION_2
			int btnState = 1; int Maskcnt = 0;
			if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
			{
				AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)m_pWinManager->GetTab()->getAITab();
				btnState = pDeepCatchtab->getBoneAnalysis(); // 0: Vertebra Network, 1: nothing		

				if (btnState == eUseVertebraNetwork)
				{
					indeces.clear();
					for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
					{
						indeces.push_back(i);
					}
				}
				Maskcnt = m_pVolumeData->getMaskInfoListCnt();
			}

			if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
			{
				if ((btnState == eUseVertebraNetwork && (Maskcnt > MASK_MAX - VB_MASK_MAX)) ||
					(btnState != eUseVertebraNetwork && m_pWinManager->IsPredictComplete()))
				{
					if (QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1008)
						, (QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel)).exec() == QMessageBox::Ok)
					{
#ifdef DEEPCATCH_REPORT_MASK_UID_USE
						DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->getDeepCatch_PredictedInfo();
						if (pPredictedInfo)
						{
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_BSC]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_IO]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_OF]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_AVF]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_BONE]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_SKIN]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_L3]);
						}
						m_pWinManager->updateUI();
						m_pWinManager->renderLater_GridView(true);
#else
#if 1
						if (!indeces.empty())
						{
							//	m_pActionManager->action_MaskList_del_list(indeces, true);
							ActionMaskListDels maskListDels(&DATA_CONTEXT->volume_data, indeces, true);
							maskListDels.Do();
						}
#endif
#endif
					}
					else
						return;

				}
				else if (btnState == eUseVertebraNetwork)
				{
				}
				else
				{
					// deepcatch predict로 생성되는 mask와 동일한 mask가 존재하는 경우 예외 처리.
					if (!indeces.empty())
					{
						QMessageBox::warning(this, "Predict", "DeepCatch seven class masks already exists. retry after cleaning the mask.\n - seven class mask names : Skin, Bone, Muscle, AVF, SF, IO, CNS");
						return;
					}
				}

			}
			else
			{
				if (m_pWinManager->IsPredictComplete())
				{
					if (QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1008)
						, (QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel)).exec() == QMessageBox::Ok)
						// 				if (QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1008)
						// 				, (QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel)) == QMessageBox::Ok)
					{
#ifdef DEEPCATCH_REPORT_MASK_UID_USE
						DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->getDeepCatch_PredictedInfo();
						if (pPredictedInfo)
						{
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_BSC]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_IO]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_OF]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_AVF]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_BONE]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_SKIN]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST]);
							m_pVolumeData->delMaskInfo(pPredictedInfo->mapDeepCatchPredictResultRoi[DEEPCATCH_WHOLEBODY_MASKNAME_L3]);
						}
						m_pWinManager->updateUI();
						m_pWinManager->renderLater_GridView(true);
#else
#if 1
						if (!indeces.empty())
						{
							//	m_pActionManager->action_MaskList_del_list(indeces, true);
							ActionMaskListDels maskListDels(&DATA_CONTEXT->volume_data, indeces, true);
							maskListDels.Do();
						}
#endif
#endif
					}
					else
						return;
				}
				else
				{
					// deepcatch predict로 생성되는 mask와 동일한 mask가 존재하는 경우 예외 처리.
					if (!indeces.empty())
					{
						QMessageBox::warning(this, "Predict", "DeepCatch seven class masks already exists. retry after cleaning the mask.\n - seven class mask names : Skin, Bone, Muscle, AVF, SF, IO, CNS");
						return;
					}
				}
			}
		}

		QString strProjectName = m_strFileName;
		StartPredict(strProjectName);
	}
	else
	{
		// nCount == -1인 경우는 credit count 갯수를 정상적으로 읽어오지 못한 경우로 예외 처리.
		if (nCount == -1)
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DE_0007)).exec();
		else
		{
			//QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1009)).exec();
			QMessageBox msg(NULL);
			msg.setWindowTitle(STRING_MANAGER->getString(STR_WARN));
			msg.setTextFormat(Qt::RichText);
			msg.setText(STRING_MANAGER->getString(ERR_DU_1009));
			msg.setStandardButtons(QMessageBox::Ok);
			msg.exec();
		}

		// predict 진행이 안된 경우는 더 이상 next thread를 진행하는게 의미가 없기 때문에 
		// thread queue 비우기.
		m_pActionManager->threadQueueClear();
	}

	m_pWinManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_Project, m_ComboDeepCatchModule->itemData(m_ComboDeepCatchModule->currentIndex()).toString());

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		m_pWinManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_BodyComposition, QString::number(m_btnGroupBodyComposition->checkedId()));
		m_pWinManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_BoneAnalysis, QString::number(m_btnGroupBoneAnalysis->checkedId()));

		bool bLiverSpleen = m_checkIOClassificationLiverSpleen->isChecked();
		bool bAorta = m_checkIOClassificationAorta->isChecked();
		int idx = 3;
		if (bLiverSpleen)	idx = 0;
		else if (bAorta)	idx = 1;
		if (bLiverSpleen && bAorta)	idx = 2;
		m_pWinManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_IO_Network, QString::number(idx));
	}
	else
	{
		m_pWinManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_Predict_UNET, QString::number(m_btnGroupUNET->checkedId()));
		m_pWinManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_Contrast, QString::number(m_btnGroupContrast->checkedId()));
		m_pWinManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_AWPredict, QString::number(m_btnGroupAWPredict->checkedId()));
	}

	m_pWinManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_AdditionalOptions, QString::number(m_btnGroupAdditionalOptions->checkedId()));
	m_pWinManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_IOClassificationPredict, getchkIOClassification() ? "1" : "0");
	m_pWinManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_PreferenceForReporting, QString::number(m_btnGroupReportSlice->checkedId()));
}

void AISegTabDeepCatch::UpdateProductCombo(int nItemIdx)
{
	if (m_ComboDeepCatchModule == nullptr)
		return;

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch) == eAVAILABLE_STATE::CREATE &&
		m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2) != eAVAILABLE_STATE::CREATE)
	{
		//#ifdef SUPPORT_DEEPCATCH_VERSION_2
		// wholebody
		m_nPredictCnt = LICENSE_DATA->chkFunctionUsableCount(MFL_DeepCatch_DeepCatchModule_PredictUsableCount_Credit, eLSPTChkFunctionUsableCount);
	}
	else if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2) == eAVAILABLE_STATE::CREATE)
	{
		m_nPredictCnt = LICENSE_DATA->chkFunctionUsableCount(MFL_DeepCatchV2_DeepCatchV2Module_PredictUsableCount_Credit, eLSPTChkFunctionUsableCount);
	}
}

void AISegTabDeepCatch::UpdateCreditEdit()
{

	if (m_nPredictCnt == -1) // not available로 변경 not applicable에서 
	{
		m_pAvailableCnt->setText("N/A");
	}
	else if (m_nPredictCnt == -2)	// 무한대 사용
	{
		m_pAvailableCnt->setText("Unlimited");
	}
	else
	{
		//if (LICENSE_DATA->IsOnline())
		//{
		//	if (!m_strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_REJECTED))
		//		m_pAvailableCnt->setText("Rejected");
		//	else if (!m_strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_UNLIMITED))
		//		m_pAvailableCnt->setText("Unlimited");
		//	else if (!m_strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_LIMITED))
		//		m_pAvailableCnt->setText("Limited");
		//	else
		//		m_pAvailableCnt->setText(QString("%1").arg(m_nPredictCnt));
		//}
		//else
		//	m_pAvailableCnt->setText(QString("%1").arg(m_nPredictCnt));
	}
}

bool AISegTabDeepCatch::CompleteWork(void)
{
	bool result = false;
	//int nCnt = network::chkFunctionUsableCount(LICENSE_DATA->getUrl(eLSPTDecreaseFunctionUsableCount), (eMEDIP_FUNCTION_LEVEL)MFL_PRODUCT_TYPE_DEEPCATCH_USABLE_COUNT);
	int nCnt = -1;

	//#ifdef SUPPORT_DEEPCATCH_VERSION_2
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch) == eAVAILABLE_STATE::CREATE &&
		m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2) != eAVAILABLE_STATE::CREATE)
	{
		nCnt = LICENSE_DATA->chkFunctionUsableCount(MFL_DeepCatch_DeepCatchModule_PredictUsableCount_Credit, eLSPTDecreaseFunctionUsableCount);
		if (nCnt == m_nPredictCnt - 1)
		{
			// UI reload
			slot_OnComboChangedAIModule(m_ComboDeepCatchModule->currentIndex());
			result = true;
		}
		else
		{
			// UI reload
			slot_OnComboChangedAIModule(m_ComboDeepCatchModule->currentIndex());
			result = false;
		}
	}
	else if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2) == eAVAILABLE_STATE::CREATE)
	{
		sFuncUsableCountResult data{ 0, "" };
		nCnt = LICENSE_DATA->chkFunctionUsableCount((eMEDIP_FUNCTION_LEVEL)MFL_DeepCatchV2_DeepCatchV2Module_PredictUsableCount_Credit, eLSPTDecreaseFunctionUsableCount, data);
		QString strPredictCntState = data.strFuncUsableCountState;

		//nCnt = LICENSE_DATA->chkFunctionUsableCount(MFL_DeepCatchV2_DeepCatchV2Module_PredictUsableCount_Credit, eLSPTDecreaseFunctionUsableCount);
		if (nCnt == m_nPredictCnt - 1)
		{
			result = true;
		}
		else if (!strPredictCntState.compare(FUNCTION_USABLECOUNT_STATE_UNLIMITED))
		{
			result = true;
		}
		else
		{
			result = false;
		}


		QString strLicenseType = LICENSE_DATA->getLicenseType();
		if (!strLicenseType.compare(LICENSE_TYPE_SUBSCIPTION) || !strLicenseType.compare(LICENSE_TYPE_PERMANENT) || !strLicenseType.compare(LICENSE_TYPE_TEMPORARY))
		{
			result = true;
		}

		slot_OnComboChangedAIModule(0);
	}

	FreezeDeepCatchProject(false);

	return result;
}

bool AISegTabDeepCatch::IsProjectModule(DEEPCATCH_TYPE_PREDICT eProject)
{
	bool result = false;

	if (m_nItemData == eProject)
	{
		result = true;
	}

	return result;
}

void AISegTabDeepCatch::FreezeDeepCatchProject(bool val)
{
	m_radioAddOptionsNone->setEnabled(!val);
	m_radioAddOptionsOnlyTrunk->setEnabled(!val);
	m_radioAddOptionsQCT->setEnabled(!val);

	if (m_chkIOClassificationPredict)
		m_chkIOClassificationPredict->setEnabled(!val);

	m_radioReportSingle->setEnabled(!val);
	m_radioReportMulti->setEnabled(!val);

	m_radioGenderUnknown->setEnabled(!val);
	m_radioGenderFemale->setEnabled(!val);
	m_radioGenderMale->setEnabled(!val);

	m_pReportHeight->setEnabled(!val);
	m_pReportWeight->setEnabled(!val);

	m_btnPredict->setEnabled(!val);
	m_pBtnMakeReport->setEnabled(!val);

	m_chkMuscleClassification->setEnabled(!val);

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		FreezeDeepCatchAIPredictUI(val);
	}
	else
	{
		m_ComboDeepCatchModule->setEnabled(!val);
		m_radio2D->setEnabled(!val);
		m_radio3D->setEnabled(!val);

		m_radioNonContrast->setEnabled(!val);
		m_radioContrast->setEnabled(!val);

		m_radioAWcoronal->setEnabled(!val);
		m_radioAWsagittal->setEnabled(!val);
	}
}

void AISegTabDeepCatch::FreezeDeepCatchAIPredictUI(bool val)
{
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		m_radioWholeBody->setEnabled(!val);
		m_radioChest->setEnabled(!val);

		m_radioBoneAnalysisYES->setEnabled(!val);
		m_radioBoneAnalysisNO->setEnabled(!val);

		m_checkIOClassificationLiverSpleen->setEnabled(!val);
		m_checkIOClassificationAorta->setEnabled(!val);

		m_comboSingleSlice->setEnabled(!val);
		m_comboMultiSliceLower->setEnabled(!val);
		m_comboMultiSliceUpper->setEnabled(!val);

		slot_OnBoneAnalysis(m_nBoneAnalysis);
	}
	else
	{
		m_radio2D->setEnabled(!val);
		m_radio3D->setEnabled(!val);

		m_radioNonContrast->setEnabled(!val);
		m_radioContrast->setEnabled(!val);

		m_radioAWcoronal->setEnabled(!val);
		m_radioAWsagittal->setEnabled(!val);
	}

	m_radioAddOptionsNone->setEnabled(!val);
	m_radioAddOptionsOnlyTrunk->setEnabled(!val);
	m_radioAddOptionsQCT->setEnabled(!val);

}

bool AISegTabDeepCatch::IsCheckedIdReport(int nID)
{
	bool result = false;
	if (nID == m_btnGroupReportSlice->checkedId())
		result = true;

	return result;
}

void AISegTabDeepCatch::SettingPredictOpt(const QMap<QString, sDeepcatchMacroColumnData>& vecOption)
{
	// DeepCatch Type 
	QMap<QString, sDeepcatchMacroColumnData>::const_iterator iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_TYPE);
	if (iterMap != vecOption.end())
	{
		sDeepcatchMacroColumnData data = iterMap.value();
		QString strDeepCatchType = data.data.at(0);
		if (!strDeepCatchType.compare(QString::fromStdString(m_pProductManager->m_strDeepCatchAbdomen), Qt::CaseInsensitive))
		{
			int nIdx = m_ComboDeepCatchModule->findData(QString::fromStdString(m_pProductManager->m_strDeepCatchAbdomen));
			m_ComboDeepCatchModule->setCurrentIndex(nIdx);
			slot_OnComboChangedAIModule(nIdx);
		}
		else if (!strDeepCatchType.compare(QString::fromStdString(m_pProductManager->m_strDeepCatchChest), Qt::CaseInsensitive))
		{
			int nIdx = m_ComboDeepCatchModule->findData(QString::fromStdString(m_pProductManager->m_strDeepCatchChest));
			m_ComboDeepCatchModule->setCurrentIndex(nIdx);
			slot_OnComboChangedAIModule(nIdx);
		}
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2)) // USE_DEEPCATCH_VER_2_MACRO
	{
		// Vertebra
		iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_VERTEBRA);
		QString strVB;
		if (iterMap != vecOption.end())
		{
			sDeepcatchMacroColumnData data = iterMap.value();
			strVB = data.data.at(0);
			if (!strVB.compare(QString("Y"), Qt::CaseInsensitive))
			{
				m_radioBoneAnalysisYES->setChecked(true);
				m_radioBoneAnalysisNO->setChecked(false);
				slot_OnBoneAnalysis(eUseVertebraNetwork);
			}
			else if (!strVB.compare(QString("N"), Qt::CaseInsensitive))
			{
				m_radioBoneAnalysisYES->setChecked(false);
				m_radioBoneAnalysisNO->setChecked(true);
				slot_OnBoneAnalysis(eDCVTNone);
			}
		}

		// BodyComposition
		iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_BODYCOMP);
		if (iterMap != vecOption.end())
		{
			sDeepcatchMacroColumnData data = iterMap.value();
			QString strBodyComp = data.data.at(0);

			if (!strBodyComp.compare(QString("Chest"), Qt::CaseInsensitive) && !strVB.compare(QString("Y"), Qt::CaseInsensitive))
			{
				m_radioWholeBody->setChecked(false);
				m_radioChest->setChecked(true);
				m_nBodyComposition = eDCBTChest;

				m_comboSingleSlice->setCurrentIndex(eDCVB_L1);
				m_comboMultiSliceLower->setCurrentIndex(eDCVB_L1);
				m_comboMultiSliceUpper->setCurrentIndex(eDCVB_T12);

				slot_OnVBSingleComboChanged(eDCVB_L1);
				slot_OnVBMultiLowerComboChanged(eDCVB_L1);
				slot_OnVBMultiUpperComboChanged(eDCVB_T12);
			}
			else
			{
				m_radioWholeBody->setChecked(true);
				m_radioChest->setChecked(false);
				m_nBodyComposition = eDCBTAbdomenWholeBody;

				m_comboSingleSlice->setCurrentIndex(eDCVB_L3);
				slot_OnVBSingleComboChanged(eDCVB_L3);
				slot_OnVBMultiLowerComboChanged(eDCVB_Abdominal_waist);
				slot_OnVBMultiUpperComboChanged(eDCVB_Abdominal_waist);
			}
		}
		else
		{
			{
				m_radioWholeBody->setChecked(true);
				m_radioChest->setChecked(false);
				m_nBodyComposition = eDCBTAbdomenWholeBody;

				m_comboSingleSlice->setCurrentIndex(eDCVB_L3);
				slot_OnVBSingleComboChanged(eDCVB_L3);
				slot_OnVBMultiLowerComboChanged(eDCVB_Abdominal_waist);
				slot_OnVBMultiUpperComboChanged(eDCVB_Abdominal_waist);
			}
		}

		// Liver/Spleen
		iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_LIVER_SPLEEN);
		if (iterMap != vecOption.end())
		{
			sDeepcatchMacroColumnData data = iterMap.value();
			QString strLIVER_SPLEEN = data.data.at(0);
			if (!strLIVER_SPLEEN.compare(QString("Y"), Qt::CaseInsensitive))
				m_checkIOClassificationLiverSpleen->setChecked(true);
			else
				m_checkIOClassificationLiverSpleen->setChecked(false);

			slot_OnIOClassification();
		}

		// Aorta
		iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_AORTA);
		if (iterMap != vecOption.end())
		{
			sDeepcatchMacroColumnData data = iterMap.value();
			QString strAorta = data.data.at(0);
			if (!strAorta.compare(QString("Y"), Qt::CaseInsensitive))
				m_checkIOClassificationAorta->setChecked(true);
			else
				m_checkIOClassificationAorta->setChecked(false);

			slot_OnIOClassification();
		}
	}
	else
	{
		// UNET
		iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_UNET);
		if (iterMap != vecOption.end())
		{
			sDeepcatchMacroColumnData data = iterMap.value();
			QString strUNET = data.data.at(0);
			if (!strUNET.compare(QString("2D"), Qt::CaseInsensitive))
			{
				QRadioButton* pBtn = (QRadioButton*)m_btnGroupUNET->button(0);
				pBtn->setChecked(true);
				slot_OnChange2D3D(eDUT2D);
			}
			else if (!strUNET.compare(QString("3D"), Qt::CaseInsensitive))
			{
				QRadioButton* pBtn = (QRadioButton*)m_btnGroupUNET->button(1);
				pBtn->setChecked(true);
				slot_OnChange2D3D(eDUT3D);
			}
		}

		// Contrast
		iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_CONTRAST);
		if (iterMap != vecOption.end())
		{
			sDeepcatchMacroColumnData data = iterMap.value();
			QString strContrast = data.data.at(0);
			if (!strContrast.compare(QString("Non"), Qt::CaseInsensitive))
			{
				QRadioButton* pBtn = (QRadioButton*)m_btnGroupContrast->button(0);
				pBtn->setChecked(true);
				slot_OnChangeContrast(eDCTNoneContrast);
			}
			else if (!strContrast.compare(QString("Con"), Qt::CaseInsensitive))
			{
				QRadioButton* pBtn = (QRadioButton*)m_btnGroupContrast->button(1);
				pBtn->setChecked(true);
				slot_OnChangeContrast(eDCTContrast);
			}
		}
	}

	// Gender
	iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_GENDER);
	if (iterMap != vecOption.end())
	{
		sDeepcatchMacroColumnData data = iterMap.value();
		QString strGender = data.data.at(0);
		if (!strGender.compare(QString("Unknown"), Qt::CaseInsensitive))
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupReporGender->button(eDGTUnknown);
			pBtn->setChecked(true);
			slot_OnChangeReportGender(eDGTUnknown);
		}
		else if (!strGender.compare(QString("F"), Qt::CaseInsensitive))
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupReporGender->button(eDGTFemale);
			pBtn->setChecked(true);
			slot_OnChangeReportGender(eDGTFemale);
		}
		else if (!strGender.compare(QString("M"), Qt::CaseInsensitive))
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupReporGender->button(eDGTMale);
			pBtn->setChecked(true);
			slot_OnChangeReportGender(eDGTMale);
		}
	}

	// Height
	iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_HEIGHT);
	if (iterMap != vecOption.end())
	{
		sDeepcatchMacroColumnData data = iterMap.value();
		QString strHeight = data.data.at(0);
		m_strHeight = strHeight;
		if (m_pReportHeight)
			m_pReportHeight->setText(strHeight);
	}

	// Weight
	iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_WEIGHT);
	if (iterMap != vecOption.end())
	{
		sDeepcatchMacroColumnData data = iterMap.value();
		QString strWeight = data.data.at(0);
		m_strWeight = strWeight;
		if (m_pReportWeight)
			m_pReportWeight->setText(strWeight);
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch) && !m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		// slice
		QRadioButton* pAWBtn = (QRadioButton*)m_btnGroupAWPredict->button(eDAWTCoronal);
		if (pAWBtn)
		{
			pAWBtn->setChecked(true);
			slot_OnChangeAWPredict(eDAWTCoronal);
		}
	}

	iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_SLICE);
	if (iterMap != vecOption.end())
	{
		sDeepcatchMacroColumnData data = iterMap.value();
		QString strSliceType = data.data.at(0);
		QString strSliceNum = data.data.at(1);
		if (!strSliceType.compare(QString("L3"), Qt::CaseInsensitive))
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupReportSlice->button(0);
			pBtn->setChecked(true);
			slot_OnChangePreferReporting(eDPTSingleSlice);
		}
		else if (!strSliceType.compare(QString("AW"), Qt::CaseInsensitive))
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupReportSlice->button(1);
			pBtn->setChecked(true);
			slot_OnChangePreferReporting(eDPTMultiSlice);

			if (!strSliceNum.compare(QString("A"), Qt::CaseInsensitive))
			{
				// 	if (!vecOption[eSlice][2].compare(QString("coronal"), Qt::CaseInsensitive))
				// 	{
				// 		QRadioButton* pBtn = (QRadioButton*)m_btnGroupAWPredict->button(eDAWTCoronal);
				// 		pBtn->setChecked(true);
				// 		slot_OnChangeAWPredict(eDAWTCoronal);
				// 	}
				// 	else if (!vecOption[eSlice][2].compare(QString("sagittal"), Qt::CaseInsensitive))
				// 	{
				// 		QRadioButton* pBtn = (QRadioButton*)m_btnGroupAWPredict->button(eDAWTSagittal);
				// 		pBtn->setChecked(true);
				// 		slot_OnChangeAWPredict(eDAWTSagittal);
				//	}
			}
		}
	}

	// MacroProcessType
	QRadioButton* pBtnAdditionalOptionsNone = (QRadioButton*)m_btnGroupAdditionalOptions->button(eDAONone);
	if (pBtnAdditionalOptionsNone)
	{
		pBtnAdditionalOptionsNone->setChecked(true);
		slot_OnChangeAddtionalOptions(eDAONone);
	}
	iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_MACROTYPE);
	if (iterMap != vecOption.end())
	{
		sDeepcatchMacroColumnData data = iterMap.value();
		QString strMacroType = data.data.at(0);

		bool convertResult = false;
		unsigned short macroType = eDMTDefault;
		unsigned short curMacroType = eDMTDefault;
		//	unsigned short additionalMacroType;
		if (!strMacroType.isEmpty())
		{
			macroType = strMacroType.toUShort(&convertResult);
			curMacroType = macroType & 0x00FF;								// 하위 1바이트만 추출.
			//	additionalMacroType = (macroType >> 8) & 0x00FF;				// 상위 1바이트만 추출.
		}
		qDebug() << "curMacroType : " << curMacroType;
		if (!convertResult || curMacroType == eDMTDefault)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupAdditionalOptions->button(eDAONone);
			pBtn->setChecked(true);
			slot_OnChangeAddtionalOptions(eDAONone);
		}
		else if (curMacroType == eDMTTrunkPredictAdd)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupAdditionalOptions->button(eDAOOnlyTrunk);
			pBtn->setChecked(true);
			slot_OnChangeAddtionalOptions(eDAOOnlyTrunk);
		}
		else if (curMacroType == eDMTQCTPredictAdd)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupAdditionalOptions->button(eDAOQCT);
			pBtn->setChecked(true);
			slot_OnChangeAddtionalOptions(eDAOQCT);
		}
	}

	// muscle quality map
	bool bMuscleQualityMap = false;
	iterMap = vecOption.find(DEEPCATCH_MACRO_CATEGORY_MUSCLEQUALITYMAP);
	if (iterMap != vecOption.end())
	{
		sDeepcatchMacroColumnData data = iterMap.value();
		QString strMuscleQualitMap = data.data.at(0);

		bMuscleQualityMap = QVariant(strMuscleQualitMap).toBool();
	}
	setChkMuscleClassification(bMuscleQualityMap);
}

void AISegTabDeepCatch::SettingPredictOpt(DEEPCATCH_REPORT_PREDICT_INFO* pPredictOpt)
{
	if (pPredictOpt == nullptr)
		return;

	if (LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH) || LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH_V2))
		return;

	//if (vecOption[eType].size()>0 && !vecOption[eType][0].compare(QString::fromStdString(m_pProductManager->m_strDeepCatchAbdomen), Qt::CaseInsensitive))
	if ((DEEPCATCH_TYPE_PREDICT)pPredictOpt->stPredictOpt.nProjectType == DTP_WHOLEBODY_ABDOMEN)
	{
		int nIdx = m_ComboDeepCatchModule->findData(QString::fromStdString(m_pProductManager->m_strDeepCatchAbdomen));
		nIdx = nIdx < 0 ? 0 : nIdx;
		m_ComboDeepCatchModule->setCurrentIndex(nIdx);
		slot_OnComboChangedAIModule(nIdx);
	}
	//else if (vecOption[eType].size() > 0 && !vecOption[eType][0].compare(QString::fromStdString(m_pProductManager->m_strDeepCatchChest), Qt::CaseInsensitive))
	else if ((DEEPCATCH_TYPE_PREDICT)pPredictOpt->stPredictOpt.nProjectType == DTP_CHEST)
	{
		int nIdx = m_ComboDeepCatchModule->findData(QString::fromStdString(m_pProductManager->m_strDeepCatchChest));
		nIdx = nIdx < 0 ? 0 : nIdx;
		m_ComboDeepCatchModule->setCurrentIndex(nIdx);
		slot_OnComboChangedAIModule(nIdx);
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{

		if (pPredictOpt->stPredictOpt.BodyCompositionType == eDCBTAbdomenWholeBody)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupBodyComposition->button(eDCBTAbdomenWholeBody);
			pBtn->setChecked(true);
			slot_OnBodyComposition(eDCBTAbdomenWholeBody);
		}
		else if (pPredictOpt->stPredictOpt.BodyCompositionType == eDCBTChest)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupBodyComposition->button(eDCBTChest);
			pBtn->setChecked(true);
			slot_OnBodyComposition(eDCBTChest);
		}

		if (pPredictOpt->stPredictOpt.useVBNetwork == eUseVertebraNetwork)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupBoneAnalysis->button(eUseVertebraNetwork);
			pBtn->setChecked(true);
			slot_OnBoneAnalysis(eUseVertebraNetwork);
		}
		else if (pPredictOpt->stPredictOpt.useVBNetwork == eDeepcatchVertebraType::eDCVTNone)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupBoneAnalysis->button(eDeepcatchVertebraType::eDCVTNone);
			pBtn->setChecked(true);
			slot_OnBoneAnalysis(eDeepcatchVertebraType::eDCVTNone);
		}
	}
	else
	{
		if (pPredictOpt->stPredictOpt.UNETType == eDUT2D)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupUNET->button(0);
			pBtn->setChecked(true);
			slot_OnChange2D3D(0);
		}
		else if (pPredictOpt->stPredictOpt.UNETType == eDUT3D)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupUNET->button(1);
			pBtn->setChecked(true);
			slot_OnChange2D3D(1);
		}

		if (pPredictOpt->stPredictOpt.contrastType == eDCTNoneContrast)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupContrast->button(0);
			pBtn->setChecked(true);
			slot_OnChangeContrast(0);
		}
		else if (pPredictOpt->stPredictOpt.contrastType == eDCTContrast)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupContrast->button(1);
			pBtn->setChecked(true);
			slot_OnChangeContrast(1);
		}

		if (pPredictOpt->stPredictOpt.AWConfirmType == eDAWTCoronal)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupAWPredict->button(eDAWTCoronal);
			pBtn->setChecked(true);
			slot_OnChangeAWPredict(eDAWTCoronal);
		}
		else if (pPredictOpt->stPredictOpt.AWConfirmType == eDAWTSagittal)
		{
			QRadioButton* pBtn = (QRadioButton*)m_btnGroupAWPredict->button(eDAWTSagittal);
			pBtn->setChecked(true);
			slot_OnChangeAWPredict(eDAWTSagittal);
		}
	}

	if (pPredictOpt->stPredictOpt.genderType == eDGTUnknown)
	{
		QRadioButton* pBtn = (QRadioButton*)m_btnGroupReporGender->button(eDGTUnknown);
		pBtn->setChecked(true);
		slot_OnChangeReportGender(eDGTUnknown);
	}
	else if (pPredictOpt->stPredictOpt.genderType == eDGTFemale)
	{
		QRadioButton* pBtn = (QRadioButton*)m_btnGroupReporGender->button(eDGTFemale);
		pBtn->setChecked(true);
		slot_OnChangeReportGender(eDGTFemale);
	}
	else if (pPredictOpt->stPredictOpt.genderType == eDGTMale)
	{
		QRadioButton* pBtn = (QRadioButton*)m_btnGroupReporGender->button(eDGTMale);
		pBtn->setChecked(true);
		slot_OnChangeReportGender(eDGTMale);
	}


	if (!pPredictOpt->stPredictOpt.strHeight.isEmpty())
	{
		QString strHeight = pPredictOpt->stPredictOpt.strHeight;

		m_strHeight = strHeight;
		if (m_pReportHeight)
			m_pReportHeight->setText(strHeight);
	}

	if (!pPredictOpt->stPredictOpt.strWeight.isEmpty())
	{
		QString strWeight = pPredictOpt->stPredictOpt.strWeight;

		m_strWeight = strWeight;
		if (m_pReportWeight)
			m_pReportWeight->setText(strWeight);
	}

	if (pPredictOpt->stPredictOpt.preferenceType == eDPTSingleSlice)
	{
		QRadioButton* pBtn = (QRadioButton*)m_btnGroupReportSlice->button(0);
		pBtn->setChecked(true);
		slot_OnChangePreferReporting(0);
	}
	else if (pPredictOpt->stPredictOpt.preferenceType == eDPTMultiSlice)
	{
		QRadioButton* pBtn = (QRadioButton*)m_btnGroupReportSlice->button(1);
		pBtn->setChecked(true);
		slot_OnChangePreferReporting(1);
	}

	// additional options
	switch (pPredictOpt->stPredictOpt.AdditionalOptions)
	{
	case eDAONone:
	{
		QRadioButton* pBtn = (QRadioButton*)m_btnGroupAdditionalOptions->button(eDAONone);
		pBtn->setChecked(true);
		slot_OnChangeAddtionalOptions(eDAONone);
	}
	break;
	case eDAOOnlyTrunk:
	{
		QRadioButton* pBtn = (QRadioButton*)m_btnGroupAdditionalOptions->button(eDAOOnlyTrunk);
		pBtn->setChecked(true);
		slot_OnChangeAddtionalOptions(eDAOOnlyTrunk);
	}
	break;
	case eDAOQCT:
	{
		QRadioButton* pBtn = (QRadioButton*)m_btnGroupAdditionalOptions->button(eDAOQCT);
		pBtn->setChecked(true);
		slot_OnChangeAddtionalOptions(eDAOQCT);
	}
	break;
	}

	if (pPredictOpt->stPredictOpt.bIOClassificationPredict)
	{
		if (m_chkIOClassificationPredict)
			m_chkIOClassificationPredict->setChecked(true);
	}
	else
	{
		if (m_chkIOClassificationPredict)
			m_chkIOClassificationPredict->setChecked(false);
	}

	if (pPredictOpt->stPredictOpt.bMuscleQualityMap)
		m_chkMuscleClassification->setChecked(true);
	else
		m_chkMuscleClassification->setChecked(false);
	emit m_chkMuscleClassification->clicked();
}

// 다이콤 정보를 DeepCatch AI Seg. UI에 반영.
void AISegTabDeepCatch::SettingFromDicomInfo()
{
	DcmtkSeriesInfo* dicomFileInfo = m_pWinManager->GetDicomInfo();

	if (!dicomFileInfo)
		return;

	// height
	std::string strDCMHeight = dicomFileInfo->patientSize_;
	if (!strDCMHeight.empty())
	{
		QString strHeight = QString::fromStdString(strDCMHeight);
		bool convertResult = false;
		float heightm = 0.0f;
		float heightcm = 0.0f;
		heightm = strHeight.toFloat(&convertResult);
		if (convertResult && heightm)
		{
			heightcm = heightm * 100.0f;
			if (m_pReportHeight)
				m_pReportHeight->setText(QString::number(heightcm));
		}
	}
	else
	{
		if (m_pReportHeight)
			m_pReportHeight->setText("0");
	}

	// weight
	std::string strDCMWeight = dicomFileInfo->patientWeight_;
	if (!strDCMWeight.empty())
	{
		if (m_pReportWeight)
			m_pReportWeight->setText(QString::fromStdString(strDCMWeight));
	}
	else
	{
		if (m_pReportWeight)
			m_pReportWeight->setText("0");
	}

	// sex
	std::string strDCMSex = dicomFileInfo->sex_;
	eDeepcatchGenderType genderType = eDGTUnknown;
	if (!strDCMSex.empty())
	{
		QString strSex = QString::fromStdString(strDCMSex);
		QChar c = strSex.at(0);
		if (c == 'F' || c == 'f')
			genderType = eDGTFemale;
		else if (c == 'M' || c == 'm')
			genderType = eDGTMale;
	}
	slot_OnChangeReportGender(genderType);
	switch (genderType)
	{
	case eDGTUnknown:
		m_radioGenderUnknown->setChecked(true);
		break;
	case eDGTFemale:
		m_radioGenderFemale->setChecked(true);
		break;
	case eDGTMale:
		m_radioGenderMale->setChecked(true);
		break;
	}

	// contrast 
	bool bContrast = true;
	// studyDescription
	QString studyDescription = QString::fromStdString(dicomFileInfo->studyDescription);
	// SeriesDescription
	QString SeriesDescription = QString::fromStdString(dicomFileInfo->description_);
	if (studyDescription.contains("noncon", Qt::CaseInsensitive))
		bContrast = false;
	else if (SeriesDescription.contains("pre", Qt::CaseInsensitive))
		bContrast = false;

	slot_OnChangeContrast(bContrast ? eDCTContrast : eDCTNoneContrast);

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch) && !m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		if (bContrast)
			m_radioContrast->setChecked(true);
		else
			m_radioNonContrast->setChecked(true);
	}
}

//SUPPORT_DEEPCATCH_VERSION_2
QComboBox* AISegTabDeepCatch::createVBComboBox(QString objName, int width)
{
	QComboBox* combo = nullptr;

	combo = new QComboBox(this);
	if (combo)
	{
		combo->setObjectName(objName);

		for (int ii = 0; ii < eDCVB_T12 + 1; ii++)
		{
			QString addText = QString("T-%1").arg(ii + 1);
			combo->addItem(addText);
			combo->setItemData(combo->count() - 1, eDCVB_T1 + ii, Qt::UserRole);
		}

		for (int ii = 0; ii < eDCVB_L5 - eDCVB_T12; ii++)
		{
			QString addText = QString("L-%1").arg(ii + 1);
			combo->addItem(addText);
			combo->setItemData(combo->count() - 1, eDCVB_L1 + ii, Qt::UserRole);
		}

		combo->setStyleSheet(STYLE_MANAGER->comboBoxTab);
		combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		combo->installEventFilter(this);
		combo->setFixedWidth(width);
	}

	return combo;
}

int AISegTabDeepCatch::setComboConfigValue(QComboBox* pCombo, QString configValue, int _default)
{
	if (pCombo)
	{
		QString strVal;
		if (m_pWinManager->getConfigValue(ELEMENT_FILE, configValue, strVal))
		{
			int idx = strVal.toInt();
			if (idx >= 0)
			{
				pCombo->setCurrentIndex(idx);
				return idx;
			}
		}
		else
		{
			pCombo->setCurrentIndex(_default);
			return _default;
		}
	}
	return -1;
}

bool AISegTabDeepCatch::isNumeric(std::string& str)
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

int AISegTabDeepCatch::pushBackVBThread(/*std::deque<ActionThreadArgument>& _qThreadNext,*/int singleVBNum, int multiVBUpNum, int multiVBLowNum)
{

	if (singleVBNum == eDCVB_L3 && multiVBUpNum == eDCVB_Abdominal_waist && multiVBLowNum == eDCVB_Abdominal_waist)
	{
		//printf_s("\n [ pushBackVBThread eDCVB_L3 ] \n");

		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));

		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_BONE2VB_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_BONE2VB_PREDICT, THREAD_VB2IND_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2IND_PREDICT, THREAD_VB2BMCB_PREDICT, nullptr));
		//m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2BMCB_PREDICT, THREAD_NONE, nullptr));
		return ACTP_DEEPCATCH_VB2BMCB_PREDICT;
	}
	else if (singleVBNum == eDCVB_L3 && multiVBUpNum != eDCVB_Abdominal_waist && multiVBLowNum != eDCVB_Abdominal_waist)
	{
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_L3_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_BONE2VB_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_BONE2VB_PREDICT, THREAD_VB2IND_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2IND_PREDICT, THREAD_VB2BMCB_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2BMCB_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
		return ACTP_ABDOMIAL_WAIST_RANGE_PREDICT;
	}
	else if (singleVBNum != eDCVB_L3 && multiVBUpNum == eDCVB_Abdominal_waist && multiVBLowNum == eDCVB_Abdominal_waist)
	{
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_ABDOMIAL_WAIST_RANGE_PREDICT, THREAD_BONE2VB_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_BONE2VB_PREDICT, THREAD_VB2IND_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2IND_PREDICT, THREAD_VB2BMCB_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2BMCB_PREDICT, THREAD_L3_PREDICT, nullptr));
		return ACTP_L3_PREDICT;
	}
	else if (singleVBNum != eDCVB_L3 && multiVBUpNum != eDCVB_Abdominal_waist && multiVBLowNum != eDCVB_Abdominal_waist)
	{
		//printf_s("\n [ pushBackVBThread !!!! no eDCVB_L3 ] \n");

		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEP_DRAW_PREDICT, THREAD_COMPONENT_CHOICE, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_COMPONENT_CHOISE, THREAD_BONE2VB_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_BONE2VB_PREDICT, THREAD_VB2IND_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2IND_PREDICT, THREAD_VB2BMCB_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_VB2BMCB_PREDICT, THREAD_L3_PREDICT, nullptr));
		m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_L3_PREDICT, THREAD_ABDOMIAL_WAIST_RANGE, nullptr));
		return ACTP_ABDOMIAL_WAIST_RANGE_PREDICT;
	}

	return ACTP_NONE;
}

QString AISegTabDeepCatch::getMultiSliceUpLowName()
{
	QString MultiSliceStr = "Abdominal Waist";
	int MultiSliceUpNum = getMultiSliceUpNum();
	int MultiSliceLowNum = getMultiSliceLowNum();
	if (getBoneAnalysis() == eUseVertebraNetwork && (MultiSliceUpNum != eDCVB_Abdominal_waist && MultiSliceLowNum != eDCVB_Abdominal_waist))
	{
		QString MultiSliceUpStr = getVBString(MultiSliceUpNum);
		QString MultiSliceLowStr = getVBString(MultiSliceLowNum);
		MultiSliceStr = MultiSliceUpStr + "-" + MultiSliceLowStr;
	}

	return MultiSliceStr;
}

QString AISegTabDeepCatch::getVBString(int vbNum)
{
	switch (vbNum)
	{
	case eDCVB_T1:
		return QString(DEEPCATCH_VB_MASKNAME_T1);
		break;
	case eDCVB_T2:
		return QString(DEEPCATCH_VB_MASKNAME_T2);
		break;
	case eDCVB_T3:
		return QString(DEEPCATCH_VB_MASKNAME_T3);
		break;
	case eDCVB_T4:
		return QString(DEEPCATCH_VB_MASKNAME_T4);
		break;
	case eDCVB_T5:
		return QString(DEEPCATCH_VB_MASKNAME_T5);
		break;
	case eDCVB_T6:
		return QString(DEEPCATCH_VB_MASKNAME_T6);
		break;
	case eDCVB_T7:
		return QString(DEEPCATCH_VB_MASKNAME_T7);
		break;
	case eDCVB_T8:
		return QString(DEEPCATCH_VB_MASKNAME_T8);
		break;
	case eDCVB_T9:
		return QString(DEEPCATCH_VB_MASKNAME_T9);
		break;
	case eDCVB_T10:
		return QString(DEEPCATCH_VB_MASKNAME_T10);
		break;
	case eDCVB_T11:
		return QString(DEEPCATCH_VB_MASKNAME_T11);
		break;
	case eDCVB_T12:
		return QString(DEEPCATCH_VB_MASKNAME_T12);
		break;
	case eDCVB_L1:
		return QString(DEEPCATCH_VB_MASKNAME_L1);
		break;
	case eDCVB_L2:
		return QString(DEEPCATCH_VB_MASKNAME_L2);
		break;
	case eDCVB_L3:
		return QString(DEEPCATCH_VB_MASKNAME_L3);
		break;
	case eDCVB_L4:
		return QString(DEEPCATCH_VB_MASKNAME_L4);
		break;
	case eDCVB_L5:
		return QString(DEEPCATCH_VB_MASKNAME_L5);
		break;
	default:
		break;
	}

	return nullptr;
}

void AISegTabDeepCatch::slot_OnChange2D3D(int idx)
{
	m_nUNET = idx;
	if (idx == 0)
	{
		m_strOptUNET = "2D";
	}
	else if (idx == 1)
	{
		m_strOptUNET = "3D";
	}
#ifndef SUPPORT_DEEPCATCH_FIX_NNUNET
	m_strFileName = "WholeBody" + m_strOptContrast + m_strOptUNET;
#endif
}

void AISegTabDeepCatch::slot_OnChangeContrast(int idx)
{
	m_nContrast = idx;
	if (idx == 0)
	{
		m_strOptContrast = "Non";
	}
	else if (idx == 1)
	{
		m_strOptContrast = "Con";
	}
#ifndef SUPPORT_DEEPCATCH_FIX_NNUNET
	m_strFileName = "WholeBody" + m_strOptContrast + m_strOptUNET;
#endif
}

void AISegTabDeepCatch::slot_OnChangeAWPredict(int idx)
{
	m_nAWPredict = idx;
}

// 0: None, 1: Only Trunk 2: QCT
void AISegTabDeepCatch::slot_OnChangeAddtionalOptions(int idx)
{
	m_nAdditionalOptions = idx;
}

//SUPPORT_DEEPCATCH_VERSION_2
//0: Wholebody & Abdomen, 1 : Chest
void AISegTabDeepCatch::slot_OnBodyComposition(int idx)
{
	m_nBodyComposition = idx;

	if (getBoneAnalysis() != eDCVTNone)
	{
		m_comboSingleSlice->setEnabled(true);
		m_comboMultiSliceLower->setEnabled(true);
		m_comboMultiSliceUpper->setEnabled(true);
	}

	if (idx == eDCBTChest)
	{
		m_comboSingleSlice->setCurrentIndex(eDCVB_L1);
		m_comboMultiSliceLower->setCurrentIndex(eDCVB_L1);
		m_comboMultiSliceUpper->setCurrentIndex(eDCVB_T12);

		slot_OnVBSingleComboChanged(eDCVB_L1);
		slot_OnVBMultiLowerComboChanged(eDCVB_L1);
		slot_OnVBMultiUpperComboChanged(eDCVB_T12);

		if (!m_radioBoneAnalysisYES->isChecked())
		{
			m_comboSingleSlice->setEnabled(false);
			m_comboMultiSliceLower->setEnabled(false);
			m_comboMultiSliceUpper->setEnabled(false);
		}
	}
	else
	{
		m_comboSingleSlice->setCurrentIndex(eDCVB_L3);
		slot_OnVBSingleComboChanged(eDCVB_L3);
		if (!m_radioBoneAnalysisYES->isChecked())
		{
			m_comboSingleSlice->setEnabled(false);
			m_comboMultiSliceLower->setEnabled(false);
			m_comboMultiSliceUpper->setEnabled(false);
		}
	}
}

// 0: Vertebra Network, 1: nothing
void AISegTabDeepCatch::slot_OnBoneAnalysis(int idx)
{
	m_nBoneAnalysis = idx;

	if (idx != eUseVertebraNetwork)
	{
		m_comboSingleSlice->setEnabled(false);
		m_comboMultiSliceLower->setEnabled(false);
		m_comboMultiSliceUpper->setEnabled(false);
	}
	else
	{
		//if (getBodyComposition() != eDCBTChest)
		{
			m_comboSingleSlice->setEnabled(true);
			m_comboMultiSliceLower->setEnabled(true);
			m_comboMultiSliceUpper->setEnabled(true);
		}
	}
}

// 0: liver/spleen, 1: aorta, 2: both, 3: nothing
void AISegTabDeepCatch::slot_OnIOClassification()
{
	m_pWinManager->setMoveFocus(true);
	bool bLiverSpleen = m_checkIOClassificationLiverSpleen->isChecked();
	bool bAorta = m_checkIOClassificationAorta->isChecked();

	int idx = 3;
	if (bLiverSpleen)
	{
		idx = 0;
	}
	else if (bAorta)
	{
		idx = 1;
	}

	if (bLiverSpleen && bAorta) idx = 2;
	m_nIOClassification = idx;
}

void AISegTabDeepCatch::slot_checkLiverSpleenChkBox(bool bcheck)
{
	m_checkIOClassificationLiverSpleen->setChecked(bcheck);
	slot_OnIOClassification();
}

void AISegTabDeepCatch::slot_checkAortaChkBox(bool bcheck)
{
	m_checkIOClassificationAorta->setChecked(bcheck);
	slot_OnIOClassification();
}

void AISegTabDeepCatch::slot_OnVBSingleComboChanged(int index)
{
	m_nSingleSliceNum = index;

	if (index == (int)eDCVB_L3 && m_comboMultiSliceUpper && m_comboMultiSliceLower)
	{
		m_comboMultiSliceUpper->setCurrentIndex(eDCVB_Abdominal_waist);
		m_comboMultiSliceLower->setCurrentIndex(eDCVB_Abdominal_waist);
	}
}

void AISegTabDeepCatch::slot_OnVBMultiLowerComboChanged(int index)
{
	m_nMultiSliceLowNum = index;

	if (index == (int)eDCVB_Abdominal_waist && m_comboMultiSliceUpper)
	{
		m_comboMultiSliceUpper->setCurrentIndex(eDCVB_Abdominal_waist);
	}
	else if (index != (int)eDCVB_Abdominal_waist && m_comboMultiSliceUpper && m_comboMultiSliceUpper->currentIndex() == eDCVB_Abdominal_waist)
	{
		m_comboMultiSliceUpper->setCurrentIndex(eDCVB_T12);
	}
}

void AISegTabDeepCatch::slot_OnVBMultiUpperComboChanged(int index)
{
	m_nMultiSliceUpNum = index;

	if (index == (int)eDCVB_Abdominal_waist && m_comboMultiSliceLower)
	{
		m_comboMultiSliceLower->setCurrentIndex(eDCVB_Abdominal_waist);
	}
	else if (index != (int)eDCVB_Abdominal_waist && m_comboMultiSliceLower && m_comboMultiSliceLower->currentIndex() == eDCVB_Abdominal_waist)
	{
		m_comboMultiSliceLower->setCurrentIndex(eDCVB_L1);
	}
}
//SUPPORT_DEEPCATCH_VERSION_2 end

void AISegTabDeepCatch::slot_OnChangeHeight(void)
{
	if (m_pReportHeight)
		m_strHeight = m_pReportHeight->text();
}

void AISegTabDeepCatch::slot_OnChangeWeight(void)
{
	if (m_pReportWeight)
		m_strWeight = m_pReportWeight->text();
}

void AISegTabDeepCatch::slot_OnChangePreferReporting(int idx)
{
	m_nPreferReporting = idx;
}

void AISegTabDeepCatch::slot_OnChangeReportGender(int idx)
{
	m_nReportGender = idx;
}

void AISegTabDeepCatch::slot_OnComboChangedAIModule(int nIdx)
{
	UpdateProductCombo(nIdx);

	//m_nItemData = m_ComboDeepCatchModule->itemData(nIdx).toInt();
	m_nItemData = nIdx;

	qDebug() << "set m_nItemData" << m_nItemData;
	//	m_pWdgAbdomenOption->hide();
	//	m_pWdgReportOption->hide();
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch) && !m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		FreezeDeepCatchAIPredictUI(false);

		if (!m_ComboDeepCatchModule->itemData(m_nItemData).toString().compare(QString::fromStdString(m_pProductManager->m_strDeepCatchAbdomen)))
		{
			//		m_pWdgAbdomenOption->show();
			m_pAvailableCnt->setText(QString("%1").arg(m_nPredictCnt));
		}
		else if (!m_ComboDeepCatchModule->itemData(m_nItemData).toString().compare(QString::fromStdString(m_pProductManager->m_strDeepCatchChest)))
		{
			m_pAvailableCnt->setText(QString("%1").arg(m_nPredictCnt));
		}

		m_strFileName = "WholeBody" + m_strOptContrast + m_strOptUNET;
	}
	else if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		FreezeDeepCatchAIPredictUI(false);
		m_pAvailableCnt->setText(QString("%1").arg(m_nPredictCnt));
	}

	m_pAvailableCnt->setText(QString("%1").arg(m_nPredictCnt));

}

void AISegTabDeepCatch::slot_OnMakeReport()
{
	if (nullptr == m_pWinManager->mainReportWidget)
	{
		m_pWinManager->mainWindow->createReportTab();
	}

	if (!m_pWinManager->IsPredictComplete())
	{
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1001)).exec();
		// macro 동작시 thread를 통한 report 진행중인 경우는 thread queue 초기화하여 종료.
		if (m_pActionManager->GetAfterThread() == THREAD_DEEPCATCH_REPORT_START)
			m_pActionManager->threadQueueClear();
		return;
	}

#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER	
	m_pWinManager->tempReportData.clear();
	QString _str = STRING_MANAGER->report_format;
	STRING_MANAGER->report_format = STRING_MANAGER->programPath + "/report";
	m_pWinManager->mainReportWidget->initFormatComboBox();
	STRING_MANAGER->report_format = _str;
#endif

#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER
	bool bDOCTORANSWER_CHART_FUNCTION = true;
	if (bDOCTORANSWER_CHART_FUNCTION)
		openHCCPredictionDlg();
#endif


	if (m_pActionManager->m_IsMacroMode == false)
	{
		// 7개 class mask name이 mask list에 1개라도 존재하지 않으면 report시 data 누락이 발생할 수 있음을 경고.
		MaskInfo* pMInfo = nullptr;
		int countDeepcatchClass = 0;
		for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
		{
			pMInfo = m_pVolumeData->getMaskInfo(i, false);
			QString maskName = QString::fromWCharArray(pMInfo->maskName);
			if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_SKIN)
				|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_BONE)
				|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE)
				|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_AVF)
				|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_OF)
				|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO)
				|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_BSC))
				countDeepcatchClass++;
		}
		if (countDeepcatchClass != eDWMMaxSize)
		{
			QMessageBox(QMessageBox::NoIcon, STRING_MANAGER->getString(STR_INFO), QString("One or more of the DeepCatch seven classes are missing. therefore, some omission of report display data will occur.\n - seven class names : Skin, Bone, Muscle, AVF, SF, IO, CNS")).exec();
			//QMessageBox(QMessageBox::NoIcon, STRING_MANAGER->getString(STR_INFO), QString("내용")).exec();
			//QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), QString("내용")).exec();
		}
	}

	DEEPCATCH_REPORT_PREDICT_INFO* predictInfo = m_pActionManager->getDeepCatch_PredictedInfo();

	if (!predictInfo)
	{
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1001)).exec();
		// macro 동작시 thread를 통한 report 진행중인 경우는 thread queue 초기화하여 종료.
		if (m_pActionManager->GetAfterThread() == THREAD_DEEPCATCH_REPORT_START)
		{
			m_pActionManager->threadQueueClear();
		}
		return;
	}

	//SUPPORT_DEEPCATCH_VERSION_2
	MaskInfo* pMInfo = nullptr;
	bool isL1Mask = false;

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
	{
		for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
		{
			pMInfo = m_pVolumeData->getMaskInfo(i, false);
			QString maskName = QString::fromWCharArray(pMInfo->maskName);
			if (!maskName.compare(DEEPCATCH_VB_MASKNAME_L1))
				isL1Mask = true;
		}

		if (isL1Mask && m_pActionManager->m_IsMacroMode == false)
		{
			DcmtkSeriesInfo* pDcmSeriesInfo = m_pWinManager->GetDicomInfo();
			std::string strAge = pDcmSeriesInfo->age_;
			bool bExistAge = false;
			if (isNumeric(strAge))
			{
				predictInfo->stPredictOpt.age_ = std::stoi(strAge);
				bExistAge = true;
			}

			if (!bExistAge)
			{
				InputMessageDlg dlg("Enter the patient's age", "AGE", this);
				if (dlg.exec() == QDialog::Accepted)
				{
					int num = dlg.getInputNum();
					if (num < 1) return;

					predictInfo->stPredictOpt.age_ = num;
				}
				else
				{
					return;
				}
			}
		}
	}


	// deepcatch ui setting info 등록.
	predictInfo->stPredictOpt.genderType = (eDeepcatchGenderType)getReportGender();
	predictInfo->stPredictOpt.strHeight = getReportHeightStr();
	predictInfo->stPredictOpt.strWeight = getReportWeightStr();
	predictInfo->stPredictOpt.preferenceType = (eDeepcatchPreferenceType)getPreferReporting();
	predictInfo->stPredictOpt.bMuscleQualityMap = getchkMuslceClassification();

	predictInfo->stPredictOpt.nMuslceQualityMapStartHU[eMQMCAptTissue] = getMuslceQualityMapStartHU(eMQMCAptTissue);
	predictInfo->stPredictOpt.nMuslceQualityMapEndHU[eMQMCAptTissue] = getMuslceQualityMapEndHU(eMQMCAptTissue);
	predictInfo->stPredictOpt.nMuslceQualityMapStartHU[eMQMCAbnormal1] = getMuslceQualityMapStartHU(eMQMCAbnormal1);
	predictInfo->stPredictOpt.nMuslceQualityMapEndHU[eMQMCAbnormal1] = getMuslceQualityMapEndHU(eMQMCAbnormal1);
	predictInfo->stPredictOpt.nMuslceQualityMapStartHU[eMQMCAbnormal2] = getMuslceQualityMapStartHU(eMQMCAbnormal2);
	predictInfo->stPredictOpt.nMuslceQualityMapEndHU[eMQMCAbnormal2] = getMuslceQualityMapEndHU(eMQMCAbnormal2);
	predictInfo->stPredictOpt.nMuslceQualityMapStartHU[eMQMCNormal] = getMuslceQualityMapStartHU(eMQMCNormal);
	predictInfo->stPredictOpt.nMuslceQualityMapEndHU[eMQMCNormal] = getMuslceQualityMapEndHU(eMQMCNormal);

	int skinUid = -1;
	int muscleUid = -1;
#ifdef DEEPCATCH_REPORT_MASK_UID_USE
	QMap<QString, int>& mapDeepCatchPredictResultRoi = predictInfo->mapDeepCatchPredictResultRoi;
	if (!mapDeepCatchPredictResultRoi.isEmpty())
	{
		QMap<QString, int>::const_iterator iterMap = mapDeepCatchPredictResultRoi.find(DEEPCATCH_WHOLEBODY_MASKNAME_SKIN);
		if (iterMap != mapDeepCatchPredictResultRoi.end())
			skinUid = iterMap.value();
	}
#else
	MaskInfo* pSkinMaskInfo = m_pVolumeData->findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_SKIN);
	skinUid = pSkinMaskInfo ? pSkinMaskInfo->uid : -1;
	MaskInfo* pMuscleMaskInfo = m_pVolumeData->findMaskInfo(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE);
	muscleUid = pMuscleMaskInfo ? pMuscleMaskInfo->uid : -1;
#endif

	if (getchkMuslceClassification())
	{
		// 1. muscle - skin - report
		if ((muscleUid != -1 && skinUid != -1) || m_pActionManager->m_IsMacroMode)
		{
			if (m_pActionManager->m_IsMacroMode == false)
			{
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_MUSCLE_CLASSIFICATION_ADP_TISSUE, nullptr));
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_ABNORMAL1, nullptr));
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_ABNORMAL2, nullptr));
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_MUSCLE_CLASSIFICATION_NORMAL, nullptr));

				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_THRESHOLD_SEL, THREAD_CIRCUMFERENCE, nullptr));
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_CIRCUMFERENCE, THREAD_DEEPCATCH_REPORT, nullptr));
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_REPORT, THREAD_NONE, nullptr));
				m_pActionManager->m_nCurrentThreadCount = m_pActionManager->m_nMultiThreadTotalCount = m_pActionManager->m_qThreadNext.size();
				m_pActionManager->action_ProgressBegin(QString("DeepCatch - Muscle Classification adipose"));
			}

			// 기존 마스크가 있으면 지우고
			std::vector<muint32> indeces;
			MaskInfo* pMaskInfo = nullptr;
			for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
			{
				pMaskInfo = m_pVolumeData->getMaskInfo(i, false);
				QString maskName = QString::fromWCharArray(pMaskInfo->maskName);
				if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ADP_TISSUE)
					|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ABNORMAL1)
					|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ABNORMAL2)
					|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_NORMAL))
				{
					indeces.push_back(i);
				}
			}

			if (!indeces.empty())
			{
				//	m_pActionManager->action_MaskList_del_list(indeces, true);
				ActionMaskListDels maskListDels(&DATA_CONTEXT->volume_data, indeces, true);
				maskListDels.Do();
			}

			m_pActionManager->action_EmptyStart(ACTP_NONE);
		}
		// 2. skin - report (muscle이 없는 경우)
		else if (skinUid != -1)
		{
			if (m_pActionManager->m_IsMacroMode == false)
			{
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_CIRCUMFERENCE, nullptr));
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_CIRCUMFERENCE, THREAD_DEEPCATCH_REPORT, nullptr));
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_REPORT, THREAD_NONE, nullptr));
				m_pActionManager->m_nCurrentThreadCount = m_pActionManager->m_nMultiThreadTotalCount = m_pActionManager->m_qThreadNext.size();
				m_pActionManager->action_ProgressBegin(QString("DeepCatch - Calculate L3 Circumference"));
			}
			m_pActionManager->action_EmptyStart(ACTP_NONE);
		}
		// 3. report (muscle, skin이 없는 경우)
		else
		{
			if (m_pActionManager->m_IsMacroMode == false)
			{
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_REPORT, THREAD_NONE, nullptr));
				m_pActionManager->m_nCurrentThreadCount = m_pActionManager->m_nMultiThreadTotalCount = m_pActionManager->m_qThreadNext.size();
				m_pActionManager->action_ProgressBegin(QString("DeepCatch - Report"));
			}
			m_pWinManager->mainWindow->deepcatchResultToReport(false);
		}
	}
	else
	{
		// 1. skin - report
		if (skinUid != -1 || m_pActionManager->m_IsMacroMode)
		{
			if (m_pActionManager->m_IsMacroMode == false)
			{
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_NONE, THREAD_CIRCUMFERENCE, nullptr));
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_IMAGE_CIRCUMFERENCE, THREAD_DEEPCATCH_REPORT, nullptr));
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_REPORT, THREAD_NONE, nullptr));
				m_pActionManager->m_nCurrentThreadCount = m_pActionManager->m_nMultiThreadTotalCount = m_pActionManager->m_qThreadNext.size();
				m_pActionManager->action_ProgressBegin(QString("DeepCatch - Calculate L3 Circumference"));
			}
			m_pActionManager->action_EmptyStart(ACTP_NONE);
		}
		// 2. report (skin이 없는 경우)
		else // skin이 없는 경우 둘레 계산 thread 생략.
		{
			if (m_pActionManager->m_IsMacroMode == false)
			{
				m_pActionManager->m_qThreadNext.push_back(ActionThreadArgument(ACTP_DEEPCATCH_REPORT, THREAD_NONE, nullptr));
				m_pActionManager->m_nCurrentThreadCount = m_pActionManager->m_nMultiThreadTotalCount = m_pActionManager->m_qThreadNext.size();
				m_pActionManager->action_ProgressBegin(QString("DeepCatch - Report"));
			}
			m_pWinManager->mainWindow->deepcatchResultToReport(false);
		}

	}

	m_pWinManager->setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_MuslceQualityMap, m_chkMuscleClassification->isChecked() ? "1" : "0");

}

void AISegTabDeepCatch::slot_OnMuscleQualityMap()
{
	m_pWinManager->setMoveFocus(true);
	bool res = m_chkMuscleClassification->isChecked();

	// adp_tissue
	m_labelMuscleAdpTissue->setHidden(!res);
	m_textMuscleAdpTissue_startHU->setHidden(!res);
	m_labelMuscleAdpTissue_rangChar->setHidden(!res);
	m_textMuscleAdpTissue_endHU->setHidden(!res);

	// abnormal1
	m_labelMuscleAbnormal1->setHidden(!res);
	m_textMuscleAbnormal1_startHU->setHidden(!res);
	m_labelMuscleAbnormal1_rangChar->setHidden(!res);
	m_textMuscleAbnormal1_endHU->setHidden(!res);

	// abnormal2
	m_labelMuscleAbnormal2->setHidden(!res);
	m_textMuscleAbnormal2_startHU->setHidden(!res);
	m_labelMuscleAbnormal2_rangChar->setHidden(!res);
	m_textMuscleAbnormal2_endHU->setHidden(!res);

	// normal
	m_labelMuscleNormal->setHidden(!res);
	m_textMuscleNormal_startHU->setHidden(!res);
	m_labelMuscleNormal_rangChar->setHidden(!res);
	m_textMuscleNormal_endHU->setHidden(!res);

	updateHeight();
	updateGeometry();
}

void AISegTabDeepCatch::slot_OnStartTextChanged_MuscleAdpTissue()
{
	if (!m_pVolumeData->isValidate())
		return;

	int Val = m_textMuscleAdpTissue_startHU->text().toInt();

	if (getMuslceQualityMapEndHU(eMQMCAptTissue) < Val)
		Val = getMuslceQualityMapEndHU(eMQMCAptTissue);

	if (!(m_pVolumeData->getHuMin() <= Val &&
		m_pVolumeData->getHuMax() >= Val))
		Val = m_pVolumeData->getHuMin();

	m_textMuscleAdpTissue_startHU->setText(QString::number(Val));

	//	m_pWinManager->setMoveFocus(true);
}

void AISegTabDeepCatch::slot_OnEndTextChanged_MuscleAdpTissue()
{
	if (!m_pVolumeData->isValidate())
		return;

	int Val = m_textMuscleAdpTissue_endHU->text().toInt();

	if (getMuslceQualityMapStartHU(eMQMCAptTissue) > Val)
		Val = getMuslceQualityMapStartHU(eMQMCAptTissue);

	if (!(m_pVolumeData->getHuMin() <= Val &&
		m_pVolumeData->getHuMax() >= Val))
		Val = m_pVolumeData->getHuMax();

	m_textMuscleAdpTissue_endHU->setText(QString::number(Val));

	//	m_pWinManager->setMoveFocus(true);
}

void AISegTabDeepCatch::slot_OnStartTextChanged_MuscleAbnormal1()
{
	if (!m_pVolumeData->isValidate())
		return;

	int Val = m_textMuscleAbnormal1_startHU->text().toInt();

	if (getMuslceQualityMapEndHU(eMQMCAbnormal1) < Val)
		Val = getMuslceQualityMapEndHU(eMQMCAbnormal1);

	if (!(m_pVolumeData->getHuMin() <= Val &&
		m_pVolumeData->getHuMax() >= Val))
		Val = m_pVolumeData->getHuMin();

	m_textMuscleAbnormal1_startHU->setText(QString::number(Val));

	//	m_pWinManager->setMoveFocus(true);
}

void AISegTabDeepCatch::slot_OnEndTextChanged_MuscleAbnormal1()
{
	if (!m_pVolumeData->isValidate())
		return;

	int Val = m_textMuscleAbnormal1_endHU->text().toInt();

	if (getMuslceQualityMapStartHU(eMQMCAbnormal1) > Val)
		Val = getMuslceQualityMapStartHU(eMQMCAbnormal1);

	if (!(m_pVolumeData->getHuMin() <= Val &&
		m_pVolumeData->getHuMax() >= Val))
		Val = m_pVolumeData->getHuMax();

	m_textMuscleAbnormal1_endHU->setText(QString::number(Val));

	//	m_pWinManager->setMoveFocus(true);
}

void AISegTabDeepCatch::slot_OnStartTextChanged_MuscleAbnormal2()
{
	if (!m_pVolumeData->isValidate())
		return;

	int Val = m_textMuscleAbnormal2_startHU->text().toInt();

	if (getMuslceQualityMapEndHU(eMQMCAbnormal2) < Val)
		Val = getMuslceQualityMapEndHU(eMQMCAbnormal2);

	if (!(m_pVolumeData->getHuMin() <= Val &&
		m_pVolumeData->getHuMax() >= Val))
		Val = m_pVolumeData->getHuMin();

	m_textMuscleAbnormal2_startHU->setText(QString::number(Val));

	//	m_pWinManager->setMoveFocus(true);
}

void AISegTabDeepCatch::slot_OnEndTextChanged_MuscleAbnormal2()
{
	if (!m_pVolumeData->isValidate())
		return;

	int Val = m_textMuscleAbnormal2_endHU->text().toInt();

	if (getMuslceQualityMapStartHU(eMQMCAbnormal2) > Val)
		Val = getMuslceQualityMapStartHU(eMQMCAbnormal2);

	if (!(m_pVolumeData->getHuMin() <= Val &&
		m_pVolumeData->getHuMax() >= Val))
		Val = m_pVolumeData->getHuMax();

	m_textMuscleAbnormal2_endHU->setText(QString::number(Val));

	//	m_pWinManager->setMoveFocus(true);
}

void AISegTabDeepCatch::slot_OnStartTextChanged_MuscleNormal()
{
	if (!m_pVolumeData->isValidate())
		return;

	int Val = m_textMuscleNormal_startHU->text().toInt();

	if (getMuslceQualityMapEndHU(eMQMCNormal) < Val)
		Val = getMuslceQualityMapEndHU(eMQMCNormal);

	if (!(m_pVolumeData->getHuMin() <= Val &&
		m_pVolumeData->getHuMax() >= Val))
		Val = m_pVolumeData->getHuMin();

	m_textMuscleNormal_startHU->setText(QString::number(Val));

	//	m_pWinManager->setMoveFocus(true);
}

void AISegTabDeepCatch::slot_OnEndTextChanged_MuscleNormal()
{
	if (!m_pVolumeData->isValidate())
		return;

	int Val = m_textMuscleNormal_endHU->text().toInt();

	if (getMuslceQualityMapStartHU(eMQMCNormal) > Val)
		Val = getMuslceQualityMapStartHU(eMQMCNormal);

	if (!(m_pVolumeData->getHuMin() <= Val &&
		m_pVolumeData->getHuMax() >= Val))
		Val = m_pVolumeData->getHuMax();

	m_textMuscleNormal_endHU->setText(QString::number(Val));

	//	m_pWinManager->setMoveFocus(true);
}

//SUPPORT_DEEPCATCH_VERSION_2
int AISegTabDeepCatch::getCurrentVBComboIndex(int type)
{
	int idx = -1;
	switch (type)
	{
	case eSST_MultiLow:
		idx = m_comboMultiSliceLower->currentIndex();
		break;
	case eSST_MultiUp:
		idx = m_comboMultiSliceUpper->currentIndex();
		break;
	case eSST_Single:
		idx = m_comboSingleSlice->currentIndex();
		break;
	default:
		break;
	}

	return idx;
}

int AISegTabDeepCatch::getMuslceQualityMapStartHU(eMuscleQualityMapCategorization type)
{
	QLineEdit* pLineEdtStartHU = nullptr;
	switch (type)
	{
	case eMQMCAptTissue:
		pLineEdtStartHU = m_textMuscleAdpTissue_startHU;
		break;
	case eMQMCAbnormal1:
		pLineEdtStartHU = m_textMuscleAbnormal1_startHU;
		break;
	case eMQMCAbnormal2:
		pLineEdtStartHU = m_textMuscleAbnormal2_startHU;
		break;
	case eMQMCNormal:
		pLineEdtStartHU = m_textMuscleNormal_startHU;
		break;
	}

	if (pLineEdtStartHU)
		return pLineEdtStartHU->text().toInt();
	return 0;
}

int AISegTabDeepCatch::getMuslceQualityMapEndHU(eMuscleQualityMapCategorization type)
{
	QLineEdit* pLineEdtEndHU = nullptr;
	switch (type)
	{
	case eMQMCAptTissue:
		pLineEdtEndHU = m_textMuscleAdpTissue_endHU;
		break;
	case eMQMCAbnormal1:
		pLineEdtEndHU = m_textMuscleAbnormal1_endHU;
		break;
	case eMQMCAbnormal2:
		pLineEdtEndHU = m_textMuscleAbnormal2_endHU;
		break;
	case eMQMCNormal:
		pLineEdtEndHU = m_textMuscleNormal_endHU;
		break;
	}

	if (pLineEdtEndHU)
		return pLineEdtEndHU->text().toInt();
	return 0;
}

void AISegTabDeepCatch::setMuslceQualityMapStartHU(eMuscleQualityMapCategorization type, int val)
{
	QLineEdit* pLineEdtStartHU = nullptr;
	switch (type)
	{
	case eMQMCAptTissue:
		pLineEdtStartHU = m_textMuscleAdpTissue_startHU;
		break;
	case eMQMCAbnormal1:
		pLineEdtStartHU = m_textMuscleAbnormal1_startHU;
		break;
	case eMQMCAbnormal2:
		pLineEdtStartHU = m_textMuscleAbnormal2_startHU;
		break;
	case eMQMCNormal:
		pLineEdtStartHU = m_textMuscleNormal_startHU;
		break;
	}

	if (!(m_pVolumeData->getHuMin() <= val &&
		m_pVolumeData->getHuMax() >= val))
		val = m_pVolumeData->getHuMin();

	if (pLineEdtStartHU)
		pLineEdtStartHU->setText(QString::number(val));

	if (getMuslceQualityMapEndHU(type) < val)
		setMuslceQualityMapEndHU(type, val);
}

void AISegTabDeepCatch::setMuslceQualityMapEndHU(eMuscleQualityMapCategorization type, int val)
{
	QLineEdit* pLineEdtEndHU = nullptr;
	switch (type)
	{
	case eMQMCAptTissue:
		pLineEdtEndHU = m_textMuscleAdpTissue_endHU;
		break;
	case eMQMCAbnormal1:
		pLineEdtEndHU = m_textMuscleAbnormal1_endHU;
		break;
	case eMQMCAbnormal2:
		pLineEdtEndHU = m_textMuscleAbnormal2_endHU;
		break;
	case eMQMCNormal:
		pLineEdtEndHU = m_textMuscleNormal_endHU;
		break;
	}

	if (!(m_pVolumeData->getHuMin() <= val &&
		m_pVolumeData->getHuMax() >= val))
		val = m_pVolumeData->getHuMax();

	if (pLineEdtEndHU)
		pLineEdtEndHU->setText(QString::number(val));

	if (getMuslceQualityMapStartHU(type) > val)
		setMuslceQualityMapStartHU(type, val);
}



bool AISegTabDeepCatch::openHCCPredictionDlg()
{
#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER
	if (m_pActionManager->m_IsMacroMode == false)
	{
		// Liver, Spleen 이 mask list에 1개라도 존재하지 않으면 취소
		MaskInfo* pMInfo = nullptr;
		int countDeepcatchClass = 0;
		for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
		{
			pMInfo = m_pVolumeData->getMaskInfo(i, false);
			QString maskName = QString::fromWCharArray(pMInfo->maskName);
			if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_LIVER)
				|| !maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_SPLEEN))
				countDeepcatchClass++;
		}
#if 1
		if (countDeepcatchClass != 2)
		{
			QMessageBox(QMessageBox::NoIcon, STRING_MANAGER->getString(STR_INFO), QString("There is not Spleen, Liver mask ")).exec();
			return false;
		}
		else
#endif
		{
			HCCPredictionDlg dlg(&DATA_CONTEXT->volume_data);
			if (dlg.exec() == QDialog::Accepted)
			{
			}
			else
				return false;
		}
	}
#endif
	return true;

}


