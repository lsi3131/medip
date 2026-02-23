#ifndef ACTIONDEEPCATCHREPORT_H
#define ACTIONDEEPCATCHREPORT_H

#pragma once
#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>

#ifdef QT_CHARTS_USE
#include <QtCharts>
#endif

class WindowBase;
class MyThreadQT;
class VOLUME_DATA;

typedef enum {
	eDWMSkin,
	eDWMBone,
	eDWMMuscle,
	eDWMAVF,
	eDWMOF,
	eDWMIO,
	eDWMBSC,
	eDWMMaxSize,
} eDeepCatchWholebodyClasses;

typedef enum {
	eDMQMCMuscleAdpTissue,
	eDMQMCMuscleAbnormal1,
	eDMQMCMuscleAbnormal2,
	eDMQMCMuscleNormal,
	eDMQMCMaxSize,
} eDeepCatchMuscleQualityMapClasses;

typedef enum {
	eDVertebra_L1,
	eDVertebra_L2,
	eDVertebra_L3,
	eDVertebra_L4,
	eDVertebra_L5,
	eDVertebra_T1,
	eDVertebra_T2,
	eDVertebra_T3,
	eDVertebra_T4,
	eDVertebra_T5,
	eDVertebra_T6,
	eDVertebra_T7,
	eDVertebra_T8,
	eDVertebra_T9,
	eDVertebra_T10,
	eDVertebra_T11,
	eDVertebra_T12,	
	eDVertebraSize,
} eDeepCatchVertebraMapClasses;

typedef enum {
	eDIOClass_Liver,
	eDIOClass_Spleen,
	eDIOClass_Aorta_Artery,
	eDIOClass_Aorta_Vein,
	eDIOClassSize,
} eDeepCatchIOClassMapClasses;

typedef enum {
	eDMMTNone,
	eDMMTLeast,
	eDMMTLittle,
	eDMMTMuch,
} eDeepcatchMuscleMassType;

typedef enum {
	eDVFMTNone,
	eDVFMTNormal,
	eDVFMTMuch,
} eDeepcatchVisceralFatMassType;

typedef enum {
	eART72PPI,
//	eART96PPI,
	eART150PPI,
//	eART300PPI,
} eA4ResolutionType;

typedef enum {
	eDRTRCSuccess,			// 성공
	eDRTRCMaskInvalid		// mask invalid
} eDeepcatchReportThreadResultCode;


struct DEEPCATCH_REPORT_AREA_CHART_INFO
{
#ifdef QT_CHARTS_USE
	QLineSeries lineSeries_muscle;
	QLineSeries lineSeries_avf;
	QLineSeries lineSeries_of;
#endif	// QT_CHARTS_USE

	int	nHorizontalAxesMinValue;
	int	nHorizontalAxesMaxValue;
	int	nVerticalAxesMinValue;
	int	nVerticalAxesMaxValue;
};

//struct DEEPCATCH_REPORT_UI_INFO
struct DEEPCATCH_PREDICT_OPTION
{
	qint8 nProjectType = -1;			// 0:abdomen
	qint8 UNETType = -1;				// 0:2D,		1:3D
	qint8 contrastType = -1;			// 0:Non,		1:Con
	qint8 AWConfirmType = -1;			// 0:coronal,		1:sagittal

	qint8 genderType = -1; 				// 0: Unknown, 1: Female, 2: Male
	QString strHeight = "";
	QString strWeight = "";			
	qint8 age_ = -1;
	qint8 singleSliceType = -1;			// 0:Auto,	1:User Define
	qint8 multiSliceType = -1;			// 0:Auto,	1:User Define	
	qint8 preferenceType = -1;			// report preference type	
	QString strReportingDate;

	// 2021.09.16 추가.
	qint8 AdditionalOptions = -1;		// 0: None,	1: Only Trunk,	2: QCT
	bool bIOClassificationPredict = false;
	bool bMuscleQualityMap = false;

	int nMuslceQualityMapStartHU[eMQMCMaxSize];
	int nMuslceQualityMapEndHU[eMQMCMaxSize];

//SUPPORT_DEEPCATCH_VERSION_2
	qint8 BodyCompositionType = -1;			// 0: Wholebody & Abdomen, 1:Chest
	qint8 useVBNetwork = -1;			// 0: use Vertebra Network, 1: nothing
	qint8 useIONetwork = -1;			// 0: liver/spleen, 1: aorta, 2: both, 3: nothing

	qint8 singleSliceNum = -1;			// 0~16 : T1~L5
	qint8 multiSliceUpperNum = -1;			// 0~16 : T1~L5
	qint8 multiSliceLowerNum = -1;			// 0~16 : T1~L5
};

struct DEEPCATCH_REPORT_TEMP_INFO
{
	QVector<bool> tempMaskShowInfoVec;
	bool bUseSingleThreadProgress = false;
	bool bVolumeViewFullScreenState = false;
	bool bMPRViewFullScreenState[3] = { false, false, false };

	// 3d tab status save
	int presetComboIndex = -1;
	int shaderComboIndex = -1;
	int slideLevelValue = -1;
	int slideWidthValue = -1;
	int sliderVolumeAlpha = -1;
	int sliderLayerAlpha = -1;

	// report capture end count 저장.
	int reportVolumeVeiwCaptureLastStepCount = -1;
};

struct DEEPCATCH_REPORT_EXPERT_INFO
{
	bool bInvalidBMI = false;
	float BMI;
	//QString strReportingDate;
	int mask_Uid[eDWMMaxSize];
	int mask_Uid_muscleQualityMap[eDMQMCMaxSize];
	int mask_Uid_VertebraMap[eDVertebraSize];
	int mask_Uid_IOClassMap[eDIOClassSize];

	QMap<eDeepCatchWholebodyClasses, int> mapAbdominalWaistVoxelCount;								// abdominal waist range에서 각 class의 volxel count 합계
	QMap<eDeepCatchWholebodyClasses, int> mapL3VoxelCount;											// l3 slice에서 각 class의 volxel count

	QMap<eDeepCatchMuscleQualityMapClasses, int> mapAbdominalWaistVoxelCount_muscleQualityMap;		// abdominal waist range에서 각 class의 volxel count 합계
	QMap<eDeepCatchMuscleQualityMapClasses, int> mapL3VoxelCount_muscleQualityMap;					// l3 slice에서 각 class의 volxel count

	QMap<eDeepCatchWholebodyClasses, float> mapAbdominalWaistMeanHU;								// abdominal waist range에서 각 class의 HU value 합계.
	QMap<eDeepCatchWholebodyClasses, float> mapL3MeanHU;											// l3 slice에서 각 class의 HU value.

	QMap<eDeepCatchVertebraMapClasses, int>		mapVertebraVoxelCount;					// 
	QMap<eDeepCatchVertebraMapClasses, float>	mapVertebraMeanHU;						// 	
	QMap<eDeepCatchVertebraMapClasses, float>	mapVertebraTScore;						// 	

	QMap<eDeepCatchIOClassMapClasses, int>		mapIOClassVoxelCount;					// 
	QMap<eDeepCatchIOClassMapClasses, float>	mapIOClassMeanHU;						// 	
};

struct DEEPCATCH_REPORT_PREDICT_INFO
{
	QMap<QString, int> mapDeepCatchPredictResultRoi;						// mask name, uid 맵.
	int	nAxialDepth = -1;													// single slice의 z축 depth
	int	nStartAxialDepth = -1;												// start z
	int	nEndAxialDepth = -1;												// end z	
	DEEPCATCH_PREDICT_OPTION stPredictOpt;									// deepcatch ui setting info.
};

struct DEEPCATCH_REPORT
{	
	int	nDisplayAxialDepth = -1;											// single slice의 z축 depth(display용)
	int	nDisplayStartAxialDepth = -1;										// multi slice의 z축 start depth(display용)
	int	nDisplayEndAxialDepth = -1;											// multi slice의 z축 end depth(display용)
	QMap<eDeepCatchWholebodyClasses, int> mapDeepcatchClassToVolxelCount;	// UI 선택 옵션(L3 slice or abdominal waist)에 따라 각 class의 volxel count를 저장.
	QImage scaledVolumeViewImg[5];											// captured volume view
	QImage axialDrawingImg;													// axial drawing image avf
	QImage axialDrawingImg_muscleQualityMap;								// axial drawing image muscle quality map
	QVector<int> vecAxialViewMaskUIDs;										// muscle, avf, of의 uid vector
	DEEPCATCH_REPORT_AREA_CHART_INFO areaChartInfo;							// area chart에서 사용될 series data 정보.
	eA4ResolutionType curA4Res;												// a4 pixel size
//	DEEPCATCH_REPORT_UI_INFO deepcatchUISettingInfo;						// deepcatch ui setting info.
	DEEPCATCH_REPORT_TEMP_INFO tempInfo;									// report 처리 과정에서 임시로 사용되는 데이터.
//	bool bUpdateReport = false;												// report update flag
	quint8 reportThreadResult = eDRTRCSuccess;								// report thread에서 예외 상황 발생에 대한 case별 enumeration 값
	DEEPCATCH_REPORT_EXPERT_INFO expertReportInfo;							// 전문가 리포트에서 사용되는 자료 구조.
	float fAbdomialCircumference = -1.0f;									// Abdomial Circumference	
//#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER
	QImage chartImg;
	std::vector<float> HCC_Result_Percentage;
};

typedef struct {
	eDeepCatchWholebodyClasses curClassType;
	mask *pMask3D;
	mask maskBit;
} sCurrentMaskInfo;

const float BoneDensityTable[14][5] =
{
// age, age, median, mean, sd
	{ 0,  29, 223, 226, 44 },	//30
	{ 30, 34, 212, 212, 40 },	//30 - 34
	{ 35, 39, 206, 206, 42 },	//35 - 39
	{ 40, 44, 202, 201, 48 },	//40 - 44
	{ 45, 49, 191, 192, 39 },	//45 - 49
	{ 50, 54, 179, 182, 40 },	//50 - 54
	{ 55, 59, 163, 166, 38 },	//55 - 59
	{ 60, 64, 153, 157, 38 },	//60 - 64
	{ 65, 69, 139, 144, 50 },	//65 - 69
	{ 70, 74, 132, 137, 48 },	//70 - 74
	{ 75, 79, 117, 123, 43 },	//75 - 79
	{ 80, 84, 111, 116, 44 },	//80 - 84
	{ 85, 89, 98, 106, 47 },	//85 - 89
	{ 90, 130, 84, 89, 38 },    //≥90
};

class WorkDeepCatchReport : public QObject
{
	Q_OBJECT
public:
	WorkDeepCatchReport(VOLUME_DATA *volume, DEEPCATCH_REPORT* pOutReport, DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo, int funLevel);
	virtual ~WorkDeepCatchReport() {};

	//static void updateProgress(int, void*);

	void setProgressValue(int value, bool init = false);
private:

	VOLUME_DATA *m_pVolumeData;
	DEEPCATCH_REPORT *m_pOutReport;
	DEEPCATCH_REPORT_PREDICT_INFO *m_pPredictedInfo;
	int m_nFunLevel;
	
public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();

private:
	void initChartData_percentage_(bool bInvalid_MaskData);
	bool isSingleSliceRange(int curDepth);
	bool isMultiSliceRange(int curDepth);

	bool isNumeric(std::string & str);
	muint32 getDuplicatedOutputMaskID(QString _str);


//SUPPORT_DEEPCATCH_VERSION_2
	float calculateTScore(int age, float meanHU /*, float meanREF, float sdREF*/);
	void _createMaskPtBit(std::vector<mask*> & _maskPtList, std::vector<mask> & _maskVitList, 
		int* MaskMap, int Max);

#if 0
	void initChartData();
	void initChartData_percentage();
//	QMap<eDeepCatchWholebodyClasses, MaskInfo*> m_mapDeepcatchClassToMaskinfo;
	QVector<QPair<QString, sCurrentMaskInfo>> m_VecDeepcatchClassToMaskinfo;
#endif
};

#endif // ACTIONDEEPCATCHREPORT_H
