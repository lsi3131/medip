#include "stdafx.h"

#include "HCCPredictionDlg.h"
#include "System/styleManager.h"
#include "System/stringManager.h"
#include "windowManager.h"
#include "Main/MainMeshWidget.h"
#include "UI/MedipMultiBarChart.h"
#include "UI/MedipChartManager.h"
#include "DeepInsthink.h"
#include "volumedata.h"
#include "defineMEDIP.h"

#include "Actions/ActionManager.h"


HCCPredictionDlg::HCCPredictionDlg(VOLUME_DATA* pVolumeData, QWidget* parent)
	: QDialog(parent)
{
	QString _title(DLG_TITLE);

	int nWidth = WIN_MANAGER->ScreenWidth * 0.4;
	int nHeight = WIN_MANAGER->ScreenHeight * 0.65;
	setFixedSize(QSize(HCC_DLG_WIDTH, HCC_DLG_HEIGHT));
	//setFixedSize(QSize(nWidth, nHeight));
	setWindowTitle("HCC Prediction");
	setStyleSheet(STYLE_MANAGER->mainFrame);

	//
	m_pVolumeData = pVolumeData;

	//
	_initUI();

}

HCCPredictionDlg::~HCCPredictionDlg()
{
}

void HCCPredictionDlg::_initUI()
{
#ifdef HCC_PREDICTION_DLG
	//
	QString strStyleTitle = QString(
		"QLabel{"
		"border: 0px solid white;"
		"font-weight:bold;"
		"font-size:22px;"
		"}"
	);

	//
	QString strStyle = QString(
		"QLabel{"
		"border: 0px solid white;"
		"font-weight:bold;"
		"font-size:16px;"
		"}"
	);

	//ko
	pVLayout = new QVBoxLayout(this);
	QHBoxLayout *pHLayout_1 = new QHBoxLayout(this);
	QHBoxLayout *pHLayout_2 = new QHBoxLayout(this);
	QHBoxLayout *pHLayout_3 = new QHBoxLayout(this);
	QHBoxLayout *pHLayout_4 = new QHBoxLayout(this);
	QHBoxLayout *pHLayout_5 = new QHBoxLayout(this);
	QHBoxLayout *pHLayout_6 = new QHBoxLayout(this);
	QHBoxLayout *pHLayout_7 = new QHBoxLayout(this);
	QHBoxLayout *pHLayout_8 = new QHBoxLayout(this);
	QHBoxLayout *pHLayout_9 = new QHBoxLayout(this);
	QHBoxLayout *pHLayout_10 = new QHBoxLayout(this);
	QHBoxLayout *pHLayout_11 = new QHBoxLayout(this);
	QHBoxLayout *pHLayout_12 = new QHBoxLayout(this);

	pVLayout->addLayout(pHLayout_1);
	pVLayout->addLayout(pHLayout_2);
	pVLayout->addLayout(pHLayout_3);
	pVLayout->addLayout(pHLayout_4);
	pVLayout->addLayout(pHLayout_5);
	pVLayout->addLayout(pHLayout_6);
	pVLayout->addLayout(pHLayout_7);
	pVLayout->addLayout(pHLayout_8);
	pVLayout->addLayout(pHLayout_9);
	pVLayout->addLayout(pHLayout_10);
	pVLayout->addLayout(pHLayout_11);
	pVLayout->addLayout(pHLayout_12);

	//1
	QLabel* pLabel = new QLabel(this);
	pLabel->setText(DLG_TITLE);
	pLabel->setStyleSheet(strStyleTitle);

	pHLayout_1->addWidget(pLabel, 1, Qt::AlignCenter);

	//2
	QLabel* pLabel_2_L = new QLabel(this);
	QLabel* pLabel_2_R = new QLabel(this);
	pLabel_2_L->setText("Age:");
	pLabel_2_R->setText("Platelet, baseline:");
	pLabel_2_L->setStyleSheet(strStyle);
	pLabel_2_R->setStyleSheet(strStyle);

	pHLayout_2->addWidget(pLabel_2_L, 1, Qt::AlignLeft);
	pHLayout_2->addWidget(pLabel_2_R, 1, Qt::AlignLeft);

	//3
	QLabel* pLabel_3_L = new QLabel(this);
	QLabel* pLabel_3_R = new QLabel(this);

	QTextCodec * codec = QTextCodec::codecForName("eucKR");
	char* string = "x1000m§©";
	QString str1000mm = codec->toUnicode(string);

	pLabel_3_L->setText("Years");
	pLabel_3_R->setText(str1000mm);

	m_LineEditAge = new QLineEdit(this);
	m_LineEditAge->setText("0");
	m_LineEditAge->setFixedWidth(50);
	m_LineEditAge->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_LineEditAge->setAlignment(Qt::AlignRight);

	m_LineEditPlatelet = new QLineEdit(this);
	m_LineEditPlatelet->setText("0");
	m_LineEditPlatelet->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_LineEditPlatelet->setAlignment(Qt::AlignRight);

	pHLayout_3->addWidget(m_LineEditAge, 1, Qt::AlignRight);
	pHLayout_3->addWidget(pLabel_3_L, 1, Qt::AlignLeft);
	pHLayout_3->addWidget(m_LineEditPlatelet, 1, Qt::AlignRight);
	pHLayout_3->addWidget(pLabel_3_R, 1, Qt::AlignLeft);

	//4
	QLabel* pLabel_4_L = new QLabel(this);
	QLabel* pLabel_4_R = new QLabel(this);
	pLabel_4_L->setText("Gender:");
	pLabel_4_R->setText("Albumin, baseline:");
	pLabel_4_L->setStyleSheet(strStyle);
	pLabel_4_R->setStyleSheet(strStyle);

	pHLayout_4->addWidget(pLabel_4_L, 1, Qt::AlignLeft);
	pHLayout_4->addWidget(pLabel_4_R, 1, Qt::AlignLeft);

	//5	
	m_radioGenderMale = new QRadioButton(this);
	m_radioGenderMale->setText(QString("Male           "));
	m_radioGenderMale->setCheckable(true);
	m_radioGenderMale->setChecked(true);
	m_radioGenderMale->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	m_radioGenderFeMale = new QRadioButton(this);
	m_radioGenderFeMale->setText(QString("Female      "));
	m_radioGenderFeMale->setCheckable(true);
	m_radioGenderFeMale->setChecked(true);
	m_radioGenderFeMale->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	m_btnGroupGender = new QButtonGroup(this);
	m_btnGroupGender->addButton(m_radioGenderFeMale, 0);
	m_btnGroupGender->addButton(m_radioGenderMale, 1);

	m_LineEditAlbumin = new QLineEdit(this);
	m_LineEditAlbumin->setText("0");
	m_LineEditAlbumin->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_LineEditAlbumin->setAlignment(Qt::AlignRight);

	QLabel* pLabel_5_R = new QLabel(this);
	pLabel_5_R->setText("g/dL");

	pHLayout_5->addWidget(m_radioGenderMale, 1, Qt::AlignCenter);
	pHLayout_5->addWidget(m_radioGenderFeMale, 1, Qt::AlignCenter);
	pHLayout_5->addWidget(m_LineEditAlbumin, 1, Qt::AlignRight);
	pHLayout_5->addWidget(pLabel_5_R, 1, Qt::AlignLeft);

	//6	
	QLabel* pLabel_6_L = new QLabel(this);
	QLabel* pLabel_6_R = new QLabel(this);
	pLabel_6_L->setText("Antivirals agent:");
	pLabel_6_R->setText("Total bilirubin, baseline:");
	pLabel_6_L->setStyleSheet(strStyle);
	pLabel_6_R->setStyleSheet(strStyle);

	pHLayout_6->addWidget(pLabel_6_L, 1, Qt::AlignLeft);
	pHLayout_6->addWidget(pLabel_6_R, 1, Qt::AlignLeft);

	//7
	m_radioEnteCavir = new QRadioButton(this);
	m_radioEnteCavir->setText(QString("EnteCavir     "));
	m_radioEnteCavir->setCheckable(true);
	m_radioEnteCavir->setChecked(true);
	m_radioEnteCavir->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	m_radioTenofovir = new QRadioButton(this);
	m_radioTenofovir->setText(QString("Tenofovir     "));
	m_radioTenofovir->setCheckable(true);
	m_radioTenofovir->setChecked(true);
	m_radioTenofovir->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	m_btnGroupAntivirals = new QButtonGroup(this);
	m_btnGroupAntivirals->addButton(m_radioTenofovir, 0);
	m_btnGroupAntivirals->addButton(m_radioEnteCavir, 1);

	m_LineEditBilirubin = new QLineEdit(this);
	m_LineEditBilirubin->setText("0");
	m_LineEditBilirubin->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_LineEditBilirubin->setAlignment(Qt::AlignRight);
	QLabel* pLabel_7_R = new QLabel(this);
	pLabel_7_R->setText("mg/dL");

	pHLayout_7->addWidget(m_radioEnteCavir, 1, Qt::AlignCenter);
	pHLayout_7->addWidget(m_radioTenofovir, 1, Qt::AlignCenter);
	pHLayout_7->addWidget(m_LineEditBilirubin, 1, Qt::AlignRight);
	pHLayout_7->addWidget(pLabel_7_R, 1, Qt::AlignLeft);

	//8
	QLabel* pLabel_8_L = new QLabel(this);
	QLabel* pLabel_8_R = new QLabel(this);
	pLabel_8_L->setText("Cirrhosis, baseline:");
	pLabel_8_R->setText("ALT, baseline");
	pLabel_8_L->setStyleSheet(strStyle);
	pLabel_8_R->setStyleSheet(strStyle);

	pHLayout_8->addWidget(pLabel_8_L, 1, Qt::AlignLeft);
	pHLayout_8->addWidget(pLabel_8_R, 1, Qt::AlignLeft);

	//9
	m_radioCirrhosisYes = new QRadioButton(this);
	m_radioCirrhosisYes->setText(QString("Yes             "));
	m_radioCirrhosisYes->setCheckable(true);
	m_radioCirrhosisYes->setChecked(true);
	m_radioCirrhosisYes->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	m_radioCirrhosisNo = new QRadioButton(this);
	m_radioCirrhosisNo->setText(QString("No             "));
	m_radioCirrhosisNo->setCheckable(true);
	m_radioCirrhosisNo->setChecked(true);
	m_radioCirrhosisNo->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	m_btnGroupCirrhosis = new QButtonGroup(this);
	m_btnGroupCirrhosis->addButton(m_radioCirrhosisNo, 0);
	m_btnGroupCirrhosis->addButton(m_radioCirrhosisYes, 1);

	m_LineEditALT = new QLineEdit(this);
	m_LineEditALT->setText("0");
	m_LineEditALT->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_LineEditALT->setAlignment(Qt::AlignRight);

	QLabel* pLabel_9_R = new QLabel(this);
	pLabel_9_R->setText("U/L");

	pHLayout_9->addWidget(m_radioCirrhosisYes, 1, Qt::AlignCenter);
	pHLayout_9->addWidget(m_radioCirrhosisNo, 1, Qt::AlignCenter);
	pHLayout_9->addWidget(m_LineEditALT, 1, Qt::AlignRight);
	pHLayout_9->addWidget(pLabel_9_R, 1, Qt::AlignLeft);

	//10
	QLabel* pLabel_10_L = new QLabel(this);
	QLabel* pLabel_10_R = new QLabel(this);
	pLabel_10_L->setText("Presence of HBeAg, baseline:");
	pLabel_10_R->setText("HBV DNA, baseline");
	pLabel_10_L->setStyleSheet(strStyle);
	pLabel_10_R->setStyleSheet(strStyle);

	pHLayout_10->addWidget(pLabel_10_L, 1, Qt::AlignLeft);
	pHLayout_10->addWidget(pLabel_10_R, 1, Qt::AlignLeft);

	//11
	m_radioPresenceYes = new QRadioButton(this);
	m_radioPresenceYes->setText(QString("Yes             "));
	m_radioPresenceYes->setCheckable(true);
	m_radioPresenceYes->setChecked(true);
	m_radioPresenceYes->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	m_radioPresenceNo = new QRadioButton(this);
	m_radioPresenceNo->setText(QString("No             "));
	m_radioPresenceNo->setCheckable(true);
	m_radioPresenceNo->setChecked(true);
	m_radioPresenceNo->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	m_btnGroupPresence = new QButtonGroup(this);
	m_btnGroupPresence->addButton(m_radioPresenceNo, 0);
	m_btnGroupPresence->addButton(m_radioPresenceYes, 1);

	m_LineEditHBV = new QLineEdit(this);
	m_LineEditHBV->setText("0");
	m_LineEditHBV->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_LineEditHBV->setAlignment(Qt::AlignRight);

	QLabel* pLabel_11_R = new QLabel(this);
	pLabel_11_R->setText("IU/mL");

	pHLayout_11->addWidget(m_radioPresenceYes, 1, Qt::AlignCenter);
	pHLayout_11->addWidget(m_radioPresenceNo, 1, Qt::AlignCenter);
	pHLayout_11->addWidget(m_LineEditHBV, 1, Qt::AlignRight);
	pHLayout_11->addWidget(pLabel_11_R, 1, Qt::AlignLeft);

	//12
	m_pBtnClear = new QPushButton(this);
	m_pBtnClear->setObjectName("Clear");
	m_pBtnClear->setText(tr("Clear"));
	m_pBtnClear->setFixedWidth(100);
	m_pBtnClear->setStyleSheet(STYLE_MANAGER->buttonBehind);
	//m_pBtnClear->setToolTip("Save mask Info to *.csv file.");	


	m_pBtnPrediction = new QPushButton(this);
	m_pBtnPrediction->setObjectName("Prediction");
	m_pBtnPrediction->setText(tr("Prediction"));
	m_pBtnPrediction->setFixedWidth(100);
	m_pBtnPrediction->setStyleSheet(STYLE_MANAGER->buttonBehind);
	//m_pBtnPrediction->setToolTip("Save mask Info to *.csv file.");

	pHLayout_12->addStretch(2);
	pHLayout_12->addWidget(m_pBtnClear);
	pHLayout_12->addWidget(m_pBtnPrediction);

	//QPushButton
	connect(m_pBtnClear, &QPushButton::clicked, this, &HCCPredictionDlg::OnClear);
	connect(m_pBtnPrediction, &QPushButton::clicked, this, &HCCPredictionDlg::OnPrediction);

	//QLineEdit
	connect(m_LineEditAge, &QLineEdit::editingFinished, this, &HCCPredictionDlg::OnLineEditAge);
	connect(m_LineEditPlatelet, &QLineEdit::editingFinished, this, &HCCPredictionDlg::OnLineEditPlatelet);
	connect(m_LineEditAlbumin, &QLineEdit::editingFinished, this, &HCCPredictionDlg::OnLineEditAlbumin);
	connect(m_LineEditBilirubin, &QLineEdit::editingFinished, this, &HCCPredictionDlg::OnLineEditBilirubin);
	connect(m_LineEditALT, &QLineEdit::editingFinished, this, &HCCPredictionDlg::OnLineEditALT);
	connect(m_LineEditHBV, &QLineEdit::editingFinished, this, &HCCPredictionDlg::OnLineEditHBV);


	//
	connect(m_btnGroupGender, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioGender(int)));
	connect(m_btnGroupAntivirals, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioAntivirals(int)));
	connect(m_btnGroupCirrhosis, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioCirrhosis(int)));
	connect(m_btnGroupPresence, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioPresence(int)));

#endif
}

#ifdef HCC_PREDICTION_DLG
void HCCPredictionDlg::OnClear(void)
{
}

void HCCPredictionDlg::OnPrediction(void)
{
	if (m_pVolumeData == nullptr) return;

	float fliver = 0.0f;
	float fspleen = 0.0f;
	_getLiverSpleenValue(fliver, fspleen);	



	float fOffset = 0.001;
	float age = getAge();
	float gender = getGender();
	float agent = getAgent();
	float cirr = getCirr();
	float hbeag = getHbeag();
	float plt = getPlatelet();
	float alb = getAlbumin();
	float tb = getBilirubin();
	float alt = getALT();
	float hbv = getHBV();
	float liver = fliver * fOffset;
	float spleen = fspleen* fOffset;

#if 0
	printf_s("\n ---------- User Input");
	printf_s("\n age	: %f ", age);
	printf_s("\n gender : %f ", gender);
	printf_s("\n agent	: %f ", agent);
	printf_s("\n cirr	: %f ", cirr);
	printf_s("\n hbeag	: %f ", hbeag);
	printf_s("\n plt	: %f ", plt);
	printf_s("\n alb	: %f ", alb);
	printf_s("\n tb		: %f ", tb);
	printf_s("\n alt	: %f ", alt);
	printf_s("\n hbv	: %f ", hbv);
	printf_s("\n liver	: %f ", liver);
	printf_s("\n spleen	: %f ", spleen);
	printf_s("\n ----------");
#endif
	
	hide();	
	ACTION_MANAGER->action_HCC_Prediction(age, gender, agent, cirr, hbeag, plt,
		alb, tb, alt, hbv, liver, spleen);
	show();

#if 1
	DEEPCATCH_REPORT *pReport = ACTION_MANAGER->getDeepCatchAnalysisVals();
	if (pReport != nullptr && pReport->HCC_Result_Percentage.size() > 0)
	{
		MedipChartManager manager;
		QWidget * pWidget = manager.initPlotBarCharts(pReport->HCC_Result_Percentage);
		manager.makeBarChartPng();
	}	
#endif

	accept();
}

void HCCPredictionDlg::_OnLineEdit(QLineEdit* _pLineEdit, float & output)
{
	QLineEdit* pLineEdit = _pLineEdit;

	double tmpVal = 0.0f;
	QString text = pLineEdit->text();
	QString name = pLineEdit->objectName();
	if (dynamic_cast<const QDoubleValidator*>(pLineEdit->validator()) != nullptr)
		tmpVal = text.toDouble();
	else
		tmpVal = text.toInt();

	pLineEdit->blockSignals(true);

	int max = MAX_int32;
	int min = 0;
	if (tmpVal < min)		tmpVal = min;
	else if (tmpVal > max)	tmpVal = max;


	if (QString::number(tmpVal) != text)
		pLineEdit->setText(QString::number(tmpVal));

	output = tmpVal;

	pLineEdit->blockSignals(false);
}

void HCCPredictionDlg::_getLiverSpleenValue(float & fLiver, float & fSpleen)
{
	typedef unsigned char mask;

	//WIN_MANAGER->volume_data.setBoundingBox(m_post_box);

	int cx = m_pVolumeData->getCX();
	int cy = m_pVolumeData->getCY();
	int cz = m_pVolumeData->getCZ();

	const int eDIOClass_Liver = 0; const int eDIOClass_Spleen = 1;
	const int IO_CLASS_CNT = 2;

	int mask_Uid_ioClass_Map[IO_CLASS_CNT];
	memset(mask_Uid_ioClass_Map, -1, sizeof(mask_Uid_ioClass_Map));

	MaskInfo* pMaskInfo = nullptr;
	for (int i = 0; i < m_pVolumeData->getMaskInfoListCnt(); ++i)
	{
		pMaskInfo = m_pVolumeData->getMaskInfo(i, false);
		QString maskName = QString::fromWCharArray(pMaskInfo->maskName);

		if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_LIVER))
			mask_Uid_ioClass_Map[eDIOClass_Liver] = pMaskInfo->uid;
		else if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_IO_SPLEEN))
			mask_Uid_ioClass_Map[eDIOClass_Spleen] = pMaskInfo->uid;
	}

	std::vector<mask> MaskIOBitList;
	std::vector<mask*> MaskIOPtList;

	_createMaskPtBit(MaskIOPtList, MaskIOBitList, mask_Uid_ioClass_Map, IO_CLASS_CNT);

	int nCnt_IO_Class[IO_CLASS_CNT] = { 0, };
	qlonglong arrIOHuSum[IO_CLASS_CNT] = { 0, };
	int sumIOMask[IO_CLASS_CNT] = { 0, };

	QVector<QMap<int, int>> mapDeepcatchClassToVolxelCountArray_IO_Map;
	QVector<QMap<int, float>> meanIOHuMapArray;


	//	for (int z = 0; z < cz; ++z)
	for (int z = cz - 1; z >= 0; z--)
	{		
		for (int ii = 0; ii < IO_CLASS_CNT; ii++)
		{
			nCnt_IO_Class[ii] = 0;
			arrIOHuSum[ii] = 0;
		}


		for (int y = 0; y < cy; ++y)
		{
			for (int x = 0; x < cx; ++x)
			{
				int idx = z*cx*cy + y*cx + x;

				for (int ii = 0; ii < IO_CLASS_CNT; ii++)
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

		// io map classes
		QMap<int, int> _mapDeepcatchClassIOToVolxelCount;
		for (int ii = 0; ii < IO_CLASS_CNT; ii++)
		{
			_mapDeepcatchClassIOToVolxelCount.insert(ii, nCnt_IO_Class[ii]);
		}
		mapDeepcatchClassToVolxelCountArray_IO_Map.push_back(_mapDeepcatchClassIOToVolxelCount);		
	}

	int fVoxelCountIO[IO_CLASS_CNT] = { 0, };

	QMap<int, int>		mapIOClassVoxelCount;					// 
	// io map voxel count ÇÕ°è
	int sumVoxelCountIOa = 0;
	int sumVoxelCountIO[IO_CLASS_CNT] = { 0, };
	int nArraySize = mapDeepcatchClassToVolxelCountArray_IO_Map.size();
	if (!mapDeepcatchClassToVolxelCountArray_IO_Map.isEmpty())
	{
		QMap<int, int> _mapDeepcatchClassIOToVolxelCount;
		QMap<int, int>::const_iterator iterMap;

		for (int ii = 0; ii < IO_CLASS_CNT; ii++)
		{
			for (int i = 0; i < nArraySize; i++)
			{
				_mapDeepcatchClassIOToVolxelCount = mapDeepcatchClassToVolxelCountArray_IO_Map.at(i);
				iterMap = _mapDeepcatchClassIOToVolxelCount.find(ii);
				if (iterMap != _mapDeepcatchClassIOToVolxelCount.end())
					sumVoxelCountIO[ii] += iterMap.value();
			}

			mapIOClassVoxelCount.insert(ii, sumVoxelCountIO[ii]);
			fVoxelCountIO[ii] = sumVoxelCountIO[ii];
		}
	}

	float fSpace3D = m_pVolumeData->getSpace3D(true);
	fLiver	= fVoxelCountIO[eDIOClass_Liver] * fSpace3D;
	fSpleen = fVoxelCountIO[eDIOClass_Spleen] * fSpace3D;
}

template<typename T>
void HCCPredictionDlg::_createMaskPtBit(std::vector<T*>& _maskPtList, std::vector<T>& _maskVitList, int* MaskMap, int Max)
{
	std::vector<T*> maskPtList;
	std::vector<T> maskVitList;

	for (int ii = 0; ii < Max; ii++)
	{
		T *pMask3D_vb = nullptr;
		T maskBit_vb;
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

void HCCPredictionDlg::OnLineEditAge(void)
{
	_OnLineEdit(m_LineEditAge, m_fAge);
}

void HCCPredictionDlg::OnLineEditPlatelet(void)
{
	_OnLineEdit(m_LineEditPlatelet, m_fPlatelet);
}

void HCCPredictionDlg::OnLineEditAlbumin(void)
{
	_OnLineEdit(m_LineEditAlbumin, m_fAlbumin);
}

void HCCPredictionDlg::OnLineEditBilirubin(void)
{
	_OnLineEdit(m_LineEditBilirubin, m_fBilirubin);
}

void HCCPredictionDlg::OnLineEditALT(void)
{
	_OnLineEdit(m_LineEditALT, m_fALT);
}

void HCCPredictionDlg::OnLineEditHBV(void)
{
	_OnLineEdit(m_LineEditHBV, m_fHBV);
}

void HCCPredictionDlg::OnRadioGender(int idx)
{
	m_bGender = idx;
}

void HCCPredictionDlg::OnRadioAntivirals(int idx)
{
	m_bAntivirals = idx;
}

void HCCPredictionDlg::OnRadioCirrhosis(int idx)
{
	m_bCirrhosis = idx;
}

void HCCPredictionDlg::OnRadioPresence(int idx)
{
	m_bPresence = idx;
}


#endif
