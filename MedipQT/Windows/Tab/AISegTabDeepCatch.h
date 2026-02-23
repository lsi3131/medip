#pragma once

#include "AISegTab.h"

class VOLUME_DATA;
class ProductManager;

class AISegTabDeepCatch : public AISegTab
{
	Q_OBJECT
public:
	explicit AISegTabDeepCatch(
		VOLUME_DATA* pVolumeData,
		WindowManager* pWinManager,
		ActionManager* pActionManager,
		ProductManager* pProductManager,
		QWidget* parent = NULL);

public:
	void slot_OnPredict() override;

public:
	void StartPredict(QString strProjectName);

	void UpdateProductCombo(int nItemIdx);	// -1일때는 all
	void UpdateCreditEdit();	// -1일때는 all
	bool CompleteWork(void);

	bool IsProjectModule(DEEPCATCH_TYPE_PREDICT eProject);
	void FreezeDeepCatchProject(bool val);
	void FreezeDeepCatchAIPredictUI(bool val);

	bool IsCheckedIdReport(int nID);

	void SettingPredictOpt(const QMap<QString, sDeepcatchMacroColumnData>& vecOption);		// MACRO
	void SettingPredictOpt(DEEPCATCH_REPORT_PREDICT_INFO* pPredictOpt);				// load mipd

	void SettingFromDicomInfo();	// 다이콤 정보를 DeepCatch AI Seg. UI에 반영.

	// getter
	QString getReportHeightStr(void) { return m_strHeight; }
	QString getReportWeightStr(void) { return m_strWeight; }
	int getUNET() const { return m_nUNET; }
	int getContrast() const { return m_nContrast; }
	int getAWPredict() const { return m_nAWPredict; }

	// SUPPORT_DEEPCATCH_VERSION_2
	int getBodyComposition() const { return m_nBodyComposition; }
	int getBoneAnalysis() const { return m_nBoneAnalysis; }
	int getIOClass() const { return m_nIOClassification; }

	int getSingleSliceNum() const { return m_nSingleSliceNum; }
	int getMultiSliceUpNum() const { return m_nMultiSliceUpNum; }
	int getMultiSliceLowNum() const { return m_nMultiSliceLowNum; }
	QString getMultiSliceUpLowName();
	QString getVBString(int vbNum);
	int getCurrentVBComboIndex(int type);
	int pushBackVBThread(int singleVBNum, int multiVBUpNum, int multiVBLowNum);

	int getAdditinalOptions() const { return m_nAdditionalOptions; }
	int getSingleSlice() const { return m_nSingleSlice; }
	int getMultiSlice() const { return m_nMultiSlice; }
	int getPreferReporting() const { return m_nPreferReporting; }
	int getReportGender() const { return m_nReportGender; }
	int getProjectType() const { return m_nItemData; }
	bool getchkIOClassification() const { return m_chkIOClassificationPredict ? m_chkIOClassificationPredict->isChecked() : false; }
	bool getchkMuslceClassification() const { return m_chkMuscleClassification ? m_chkMuscleClassification->isChecked() : false; }

	// setter
	void setChkMuscleClassification(bool bChecked) { m_chkMuscleClassification->setChecked(bChecked); }

	// muscle quality map range 수정 ui
	int getMuslceQualityMapStartHU(eMuscleQualityMapCategorization type);
	int getMuslceQualityMapEndHU(eMuscleQualityMapCategorization type);
	void setMuslceQualityMapStartHU(eMuscleQualityMapCategorization type, int val);
	void setMuslceQualityMapEndHU(eMuscleQualityMapCategorization type, int val);

	//#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER		
	bool openHCCPredictionDlg();


private:
	// predict Option
	QComboBox* m_ComboDeepCatchModule = nullptr;
	QLineEdit* m_pAvailableCnt = nullptr;
	//QString		m_strProjectName = "WholeBody";	// WholeBody, Abdomen, Chest
	QString		m_strOptContrast = "Con";	// Non, Con
	QString		m_strOptUNET = "3D";	// 2D, 3D

	QWidget* m_pWdgAbdomenOption = nullptr;
	QRadioButton* m_radio2D = nullptr;
	QRadioButton* m_radio3D = nullptr;

	QRadioButton* m_radioNonContrast = nullptr;
	QRadioButton* m_radioContrast = nullptr;

	QRadioButton* m_radioAWcoronal = nullptr;
	QRadioButton* m_radioAWsagittal = nullptr;

	QRadioButton* m_radioAddOptionsNone = nullptr;
	QRadioButton* m_radioAddOptionsOnlyTrunk = nullptr;
	QRadioButton* m_radioAddOptionsQCT = nullptr;

	QCheckBox* m_chkIOClassificationPredict = nullptr;

	QRadioButton* m_radioGenderUnknown = nullptr;
	QRadioButton* m_radioGenderFemale = nullptr;
	QRadioButton* m_radioGenderMale = nullptr;

	QRadioButton* m_radioReportSingle = nullptr;
	QRadioButton* m_radioReportMulti = nullptr;

	//SUPPORT_DEEPCATCH_VERSION_2
	//DeepCatch Version 2
	QRadioButton* m_radioWholeBody = nullptr;		//Body composition
	QRadioButton* m_radioChest = nullptr;			//Body composition

	QRadioButton* m_radioBoneAnalysisYES = nullptr;
	QRadioButton* m_radioBoneAnalysisNO = nullptr;

	QCheckBox* m_checkIOClassificationLiverSpleen = nullptr;	// 0: liver/spleen, 1: aorta, 2: both, 3: nothing
	QCheckBox* m_checkIOClassificationAorta = nullptr;			// 0: liver/spleen, 1: aorta, 2: both, 3: nothing

	QButtonGroup* m_btnGroupBodyComposition = nullptr;
	QButtonGroup* m_btnGroupBoneAnalysis = nullptr;
	QButtonGroup* m_btnGroupIOClass = nullptr;

	QComboBox* m_comboSingleSlice;
	QComboBox* m_comboMultiSliceLower;
	QComboBox* m_comboMultiSliceUpper;

	QButtonGroup* m_btnGroupUNET = nullptr;						// UNET
	QButtonGroup* m_btnGroupContrast = nullptr;					// Contrast, Non
	QButtonGroup* m_btnGroupAWPredict = nullptr;				// AW, coronal, sagittal
	QButtonGroup* m_btnGroupAdditionalOptions = nullptr;		// none, only trunk, QCT

	QButtonGroup* m_btnGroupReportSlice = nullptr;				// report
	QButtonGroup* m_btnGroupReporGender = nullptr;				// report - gender

	QLineEdit* m_pReportHeight = nullptr;
	QLineEdit* m_pReportWeight = nullptr;

	QCheckBox* m_chkMuscleClassification = nullptr;

	// adp_tissue
	QLabel* m_labelMuscleAdpTissue = nullptr;
	QLineEdit* m_textMuscleAdpTissue_startHU = nullptr;
	QLabel* m_labelMuscleAdpTissue_rangChar = nullptr;
	QLineEdit* m_textMuscleAdpTissue_endHU = nullptr;
	// abnormal1
	QLabel* m_labelMuscleAbnormal1 = nullptr;
	QLineEdit* m_textMuscleAbnormal1_startHU = nullptr;
	QLabel* m_labelMuscleAbnormal1_rangChar = nullptr;
	QLineEdit* m_textMuscleAbnormal1_endHU = nullptr;
	// abnormal2
	QLabel* m_labelMuscleAbnormal2 = nullptr;
	QLineEdit* m_textMuscleAbnormal2_startHU = nullptr;
	QLabel* m_labelMuscleAbnormal2_rangChar = nullptr;
	QLineEdit* m_textMuscleAbnormal2_endHU = nullptr;
	// normal
	QLabel* m_labelMuscleNormal = nullptr;
	QLineEdit* m_textMuscleNormal_startHU = nullptr;
	QLabel* m_labelMuscleNormal_rangChar = nullptr;
	QLineEdit* m_textMuscleNormal_endHU = nullptr;

	QPushButton* m_pBtnMakeReport = nullptr;

	///////////////////////////////
	int		m_nItemData = DTP_WHOLEBODY_ABDOMEN;
	// post-processing Option
	int			m_nUNET;				// 0:2d, 1:3d
	int			m_nContrast;			// 0:none contrast, 1:contrast
	int			m_nAWPredict;			// 0:coronal, 1:sagittal
	int			m_nAdditionalOptions;	// 0: None, 1: Only Trunk 2: QCT
	int			m_nSingleSlice;			// 0:Auto, 1:User define
	int			m_nMultiSlice;			// 0:Auto, 1:User define
	int			m_nPreferReporting;		// 0: single slice, 1: multi slice
	int			m_nReportGender;		// 0: Unknown, 1: Female, 1: male
	QString		m_strHeight;
	QString		m_strWeight;

	//SUPPORT_DEEPCATCH_VERSION_2
	// DeepCatch Version 2
	int			m_nBodyComposition;		// 0: Wholebody & Abdomen, 1:Chest
	int			m_nBoneAnalysis;		// 0: Vertebra Network, 1: nothing
	int			m_nIOClassification;	// 0: liver/spleen, 1: aorta, 2: both, 3: nothing

	int			m_nSingleSliceNum;		// 0~16 : T1~L5
	int			m_nMultiSliceLowNum;	// 0~17 : T1~L5, waist(lower lib)
	int			m_nMultiSliceUpNum;		// 0~17 : T1~L5, waist(iliac crest)	

	int			m_nPredictCnt = 0;
private:
	//SUPPORT_DEEPCATCH_VERSION_2
	QComboBox* createVBComboBox(QString objName, int width);
	int setComboConfigValue(QComboBox* pCombo, QString configValue, int _default = eDCVB_L3);
	bool isNumeric(std::string& str);

public slots:
	void slot_OnComboChangedAIModule(int nIdx);
	void slot_OnChange2D3D(int idx);					// 0: 2D,  1: 3D
	void slot_OnChangeContrast(int idx);				// 0: Non, 1: Con
	void slot_OnChangeAWPredict(int idx);			// 0: coronal, 1: sagittal
	void slot_OnChangeAddtionalOptions(int idx);		// 0: None, 1: Only Trunk 2: QCT

	//SUPPORT_DEEPCATCH_VERSION_2
	void slot_OnBodyComposition(int idx);			// 0: Wholebody & Abdomen, 1:Chest
	void slot_OnBoneAnalysis(int idx);				// 0: Vertebra Network, 1: nothing
	void slot_OnIOClassification();					// 0: liver/spleen, 1: aorta, 2: both, 3: nothing
	void slot_checkLiverSpleenChkBox(bool bcheck);	// 0: liver/spleen, 1: aorta, 2: both, 3: nothing
	void slot_checkAortaChkBox(bool bcheck);			// 0: liver/spleen, 1: aorta, 2: both, 3: nothing


	void slot_OnVBSingleComboChanged(int index);
	void slot_OnVBMultiLowerComboChanged(int index);
	void slot_OnVBMultiUpperComboChanged(int index);



	void slot_OnChangeHeight(void);
	void slot_OnChangeWeight(void);

	void slot_OnChangePreferReporting(int idx);
	void slot_OnChangeReportGender(int idx);

	void slot_OnMakeReport();

	void slot_OnMuscleQualityMap();

	void slot_OnStartTextChanged_MuscleAdpTissue();
	void slot_OnEndTextChanged_MuscleAdpTissue();

	void slot_OnStartTextChanged_MuscleAbnormal1();
	void slot_OnEndTextChanged_MuscleAbnormal1();

	void slot_OnStartTextChanged_MuscleAbnormal2();
	void slot_OnEndTextChanged_MuscleAbnormal2();

	void slot_OnStartTextChanged_MuscleNormal();
	void slot_OnEndTextChanged_MuscleNormal();

};
