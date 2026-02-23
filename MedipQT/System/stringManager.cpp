#include "stdafx.h"
#include "stringManager.h"
#include "Windows/windowManager.h"
#include "System/ProductManager.h"
#include <string>

#include "version.h"

LocMap StringManager::locMap;

StringManager::StringManager()
{
	Initialize();
}

StringManager::~StringManager()
{

}

bool StringManager::Initialize(const QString& sessionName)
{
	m_sessionName = sessionName;
	programPath = QString::fromLocal8Bit(QCoreApplication::applicationDirPath().toLocal8Bit());

	// local path 설정.(계정/local/ProductName)
	m_strAppDataLocalPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
	if (!m_sessionName.isEmpty())
	{
		m_strAppDataLocalPath += "/" + m_sessionName;
	}

	if (!m_strAppDataLocalPath.isEmpty())
	{
		QDir appDataMedipPath(m_strAppDataLocalPath);
		if (!appDataMedipPath.exists())
		{
			appDataMedipPath.mkpath(".");
		}
	}
	else
	{
		m_strAppDataLocalPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);	// not empty
		if (!m_strAppDataLocalPath.isEmpty())
		{
			m_strAppDataLocalPath += "/";
			m_strAppDataLocalPath += qApp->applicationName();
			if (!m_sessionName.isEmpty())
			{
				m_strAppDataLocalPath += "/" + m_sessionName;
			}

			QDir appDataMedipPath(m_strAppDataLocalPath);
			if (!appDataMedipPath.exists())
			{
				appDataMedipPath.mkpath(".");
			}
		}
	}


#ifndef DEV_USE_APPDATA_PATH
	configFileName = programPath + "/config.dat";
	cacheFilePath = programPath + "/cache";
	dumpFilePath = programPath + "/dump";

	license_incode_file = programPath + "/medip.idt";
	license_decode_file = programPath + "/medip.ddt";
	dxSahderAsmFile = programPath + "/medicalip.asm";
#else
	configFileName = m_strAppDataLocalPath + "/config.dat";
	cacheFilePath = m_strAppDataLocalPath + "/cache";
	dumpFilePath = m_strAppDataLocalPath + "/dump";
#endif

	presetFilePath = programPath + "/prd";
	presetFileName = presetFilePath + "/medip.prd";
	report_format = programPath + "/report";
	ProgramPathLicenseDirPath = programPath + "/license";
	ProgramPathFunctionLevelDirPath = ProgramPathLicenseDirPath + "/FunctionLevel";

	localPresetFilePath = m_strAppDataLocalPath + "/prd";
	QDir localPresetFilePathDir(localPresetFilePath);
	if (!localPresetFilePathDir.exists())
		localPresetFilePathDir.mkpath(".");

	localPresetFileName = localPresetFilePath + "/medip.prd";
	localReport_format = m_strAppDataLocalPath + "/report";
	QDir localReport_formatDir(localReport_format);
	if (!localReport_formatDir.exists())
		localReport_formatDir.mkpath(".");

	presetFilePathVec.push_back(presetFilePath);
	presetFilePathVec.push_back(localPresetFilePath);

	// DeepCatch는 programPath를 MEDIP AI는 appdata path를 사용하므로 유지.
	AIFilePath = programPath + "/AI";
	AISegmentationPath = AIFilePath + "/deepdraw";
	AITranslationPath = AIFilePath + "/translation";
	AIClassificationPath = AIFilePath + "/classification";
	AIReconstructionPath = AIFilePath + "/reconstruction";

	Omniverse_AppData_DirectoryPath_Root = m_strAppDataLocalPath + "/Omniverse";
	Omniverse_AppData_DirectoryPath_Preset = Omniverse_AppData_DirectoryPath_Root + "/Preset";
	QDir().mkpath(Omniverse_AppData_DirectoryPath_Root);
	QDir().mkpath(Omniverse_AppData_DirectoryPath_Preset);

	OmniverseMaterialDirectoryPath = programPath + "/Omniverse/mdl/OmniSurface";
	OmniverseMaterialDirectoryPath_Preset = programPath + "/Omniverse/mdl/Preset";

	OmniverseConfigJsonFilePath = Omniverse_AppData_DirectoryPath_Root + "/config.json";

	config_LatestPath = "LastestPath";
	config_cachePath = "CachePath";
	config_expertReportPath = "ExpertReportPath";
	config_shader_quality = "ShaderQuality";
	config_3dAnno = "Annotation3D";
	config_color2D = "Color2D";
	config_invertingLUT = "InvertingLUT";
	fontFileName = programPath + "/medip.kft";

	config_Layer2DAlpha = "LayerAlpha";

	config_ToolArea = "ToolArea";
	config_SegDockArea = "SegmentArea";
	config_DockTop = "DockTop";
	config_DockShow = "DockHide";
	config_rePos = "RePosition";
	config_wheelZoom = "WheelZoom";
	config_backfaceCulling = "backfaceCulling";
	config_undoLimit = "UndoLimit";
	config_RenderDockArea = "RenderArea";

	config_cusWidth = "CustomWidth";
	config_cusLevel = "CustomLevel";
	config_cusPreset = "CustomPreset";
	config_cusPreset_AppData = "CustomPreset_AppData";
	config_gamma = "Gamma";

	config_BrushThickness = "Brush_Thickness";
	config_BrushOpacity = "Brush_Opacity";

	config_imageListShow = "ImageListHide";
	config_expandROI = "ExpandROI";
	config_expandMesh = "ExpandMesh";
	config_recentFile = "RecentFiles";
	config_autoSave = "AutoSave";
	config_autoSaveInterval = "AutoSaveInterval";

	config_visualPrintURL = "VisualPrintURL";
	config_visualPrintPort = "VisualPrintPort";
	config_MDBOX_Mode = "MDBOX_Mode";

	config_VolumeOpacity3D = "VolumeOpacity_3D";
	config_RegionGrowingConnectivity = "RegionGrowingConnectivity";
	config_DrawCutShape = "DrawCutShape";

	m_strTempFilePath = "C:/MedipTemp/";
	expertReportFilePath = "C:/MedipTemp/expertReport.csv";

	//resourceFile = programPath + "/meditim.dat";
	//resourceDir = programPath + "/Resources";
	cacheFileOriginVolume = cacheFilePath + "/core.data";

	report_basic_formats.push_back("Texture_Feature");

	config_lastExecutedTime = "LastExecuteTime";
	config_SavedID = "SavedID";

	// DeepCatch 추가
	m_config_DeepCatch_Project = "Project";
	m_config_DeepCatch_Predict_UNET = "UNET";
	m_config_DeepCatch_Contrast = "Contrast";
	m_config_DeepCatch_AWPredict = "AWPredict";
	m_config_DeepCatch_AdditionalOptions = "PredictAdditionalOptions";
	m_config_DeepCatch_IOClassificationPredict = "IO_Classification";

	//SUPPORT_DEEPCATCH_VERSION_2
	m_config_DeepCatch_BodyComposition = "BodyComp";
	m_config_DeepCatch_BoneAnalysis = "Vertebra";
	m_config_DeepCatch_IO_Network = "IO_Network";

	m_config_DeepCatch_SingleSlice		= "SingleSliceNum";
	m_config_DeepCatch_MultiSliceUp		= "MultiSliceUpNum";
	m_config_DeepCatch_MultiSliceLow	= "MultiSliceLowNum";

	m_config_DeepCatch_PreferenceForReporting = "PreferReport";
	m_config_DeepCatch_MuslceQualityMap = "MuscleQualityMap";

	m_config_DeepCatch_SelectionDlg_PositionX = "SelectDlgPosX";
	m_config_DeepCatch_SelectionDlg_PositionY = "SelectDlgPosY";

	// MEDIP AI
	m_strAppDataLocalAIPath = m_strAppDataLocalPath + "/AI";

	LocalAISegPath = m_strAppDataLocalAIPath + "/deepdraw";
	m_strLocalAISegPredictPath = LocalAISegPath + "/predict";
	m_strLocalAISegWeightPath = LocalAISegPath + "/weight";

	m_strLocalAISegUserWeightPath = LocalAISegPath + "/weight_user";

	m_strLocalAITransPath = m_strAppDataLocalAIPath + "/translation";
	m_strLocalAIClassPath = m_strAppDataLocalAIPath + "/classification";

	m_strLocalLogPath = m_strAppDataLocalPath + "/log";
	LocalLicenseDirPath = m_strAppDataLocalPath + "/license";

	return true;
}

bool StringManager::init()
{
	switch (locMap.loc)
	{
	case KOREAN:
	{
		locMap.rstr = STR_EXPORT_SPLIT_FILE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"분할 레이어 저장"));

		locMap.rstr = STR_TA;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Texture Analysis"));

		locMap.rstr = STR_CUSTOM;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"사용자 정의"));

		locMap.rstr = STR_SAVE_CAPTURE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"캡쳐 이미지 저장"));

		locMap.rstr = STR_EXIST_TEMP_FILE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"임시 파일이 존재합니다. 파일을 지운 후 다시 시도해주십시오."));

		locMap.rstr = STR_SOFT_TISSUE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Soft tissue"));

		locMap.rstr = STR_AIR;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Air"));

		locMap.rstr = STR_FAT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Fat"));

		locMap.rstr = STR_FLUIDS;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Fluids"));

		locMap.rstr = STR_PRESET;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"프리셋"));

		locMap.rstr = STR_ANNOTATION;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Annotation"));

		locMap.rstr = STR_LEVEL_SET;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Level set"));

		locMap.rstr = STR_PASSWORD;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"비밀번호 입력"));

		locMap.rstr = STR_DRAW_CUT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Image Draw Cut"));


		locMap.rstr = STR_RECOMMEND_1;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Recommend 1"));
		locMap.rstr = STR_RECOMMEND_2;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Recommend 2"));

		locMap.rstr = STR_ISOTROPIC;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Iso-Z"));

		locMap.rstr = STR_SAVE_FILE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"파일 저장"));
		locMap.rstr = STR_SAVE_FILE_DESC;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"변경된 사항을 파일에 적용하시겠습니까?"));

		locMap.rstr = STR_OPEN;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"파일 열기"));
		locMap.rstr = STR_MEDIP_FILE_SAVE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Medical IP 파일 저장"));
		locMap.rstr = STR_MEDIP_FILE_OPEN;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Medical IP 파일 열기"));
		locMap.rstr = STR_CROP;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"DICOM 파일 크롭 후 열기"));

		locMap.rstr = STR_SAVE_AS;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"다른 이름으로 저장"));
		locMap.rstr = STR_UPLOAD_PACS;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"PACS로 업로드"));
		locMap.rstr = STR_LOAD;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"로드"));

		locMap.rstr = STR_UPLOAD;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"서버로 파일 전송"));
		locMap.rstr = STR_UPLOAD_SUCCESS;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"성공적으로 전송되었습니다."));
		locMap.rstr = STR_UPLOAD_FAIL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"전송에 실패했습니다."));

		locMap.rstr = STR_REMOTE_FILE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"원격 파일 열기"));
		locMap.rstr = STR_REMOTE_FILE_DELAYED;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"원격 파일은 로딩시간이 지연됩니다."));


		locMap.rstr = STR_SAVE_OBJ_FILE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"OBJ 파일 추출"));

		locMap.rstr = STR_SHOW_ANNO;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Annotations 표시"));
		locMap.rstr = STR_HIDE_ANNO;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Annotations 감추기"));


		locMap.rstr = STR_FILE_TRANSFER;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"파일 전송"));
		locMap.rstr = STR_TRANSFER_DESC;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"현재 파일을 \"Medical IP\" 서버에 전송하시겠습니까?"));

		locMap.rstr = STR_CONTACT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Contact Us"));
		locMap.rstr = STR_SEGMENT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Segmentation"));
		locMap.rstr = STR_MEASUREMENT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Measurement"));
		locMap.rstr = STR_MESH_EDITING;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Mesh Editing"));
		locMap.rstr = STR_REPORT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Report"));


		locMap.rstr = STR_ENHANCED;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Enhancement"));
		locMap.rstr = STR_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"ROI"));
		locMap.rstr = STR_INFO;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Information"));
		locMap.rstr = STR_INFO_DESC;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"프로그램 정보 및 라이센스 발급 여부를 확인하실 수 있습니다."));

		locMap.rstr = STR_WINDOW_WL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Window Width & Level"));
		locMap.rstr = STR_DEFAULT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Default"));
		locMap.rstr = STR_BONE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Bone (CT)"));
		locMap.rstr = STR_LUNG;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Lung (CT)"));
		locMap.rstr = STR_MEDIASTINAL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Mediastinal (CT)"));
		locMap.rstr = STR_BRAIN;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Brain (CT)"));
		locMap.rstr = STR_ABD;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"ABD (CT)"));
		locMap.rstr = STR_LIVER;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Liver (CT)"));
		locMap.rstr = STR_KIDNEY;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Kidney (CT)"));
		locMap.rstr = STR_URINARY_TRACK_BLADDER;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Urinary track + bladder (CT)"));

		locMap.rstr = STR_WIDTH;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"너비"));
		locMap.rstr = STR_HEIGHT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"높이"));
		locMap.rstr = STR_LEVEL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Level"));
		locMap.rstr = STR_WORK;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"진행중.."));
		locMap.rstr = STR_OK;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"확인"));
		locMap.rstr = STR_CANCEL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"취소"));
		locMap.rstr = STR_AUTO;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"자동"));
		locMap.rstr = STR_RESET;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"되돌리기"));
		locMap.rstr = STR_APPLY;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"적용"));
		locMap.rstr = STR_Z_FLIP;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Z축 뒤집기"));
		locMap.rstr = STR_IMG_ENHANCED;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Image Enhancement"));
		locMap.rstr = STR_ORG_IMG;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Original Image"));
		locMap.rstr = STR_IMG_CIRCUMFERENCE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Image Circumference"));

		locMap.rstr = STR_RG;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Region Growing"));
		locMap.rstr = STR_NOT_SELECTED;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"지정된 Seed point 없음"));
		locMap.rstr = STR_SELECT_SEED;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Seed point 지정"));
		locMap.rstr = STR_IRP;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Inside Region Proposal"));
		locMap.rstr = STR_SEED_APPLY;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Seed 적용"));
		locMap.rstr = STR_ALL_SELECT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Range 적용"));
		locMap.rstr = STR_RG_ALL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Region All Select"));
		locMap.rstr = STR_ROI_LIST;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"ROI LIST"));

		locMap.rstr = STR_CLR_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Clear layer"));
		locMap.rstr = STR_NEW;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"레이어 생성"));
		locMap.rstr = STR_RENAME;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"이름 바꾸기"));
		locMap.rstr = STR_DELETE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"선택 레이어 삭제"));
		locMap.rstr = STR_COPY;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"선택 레이어 복사"));
		locMap.rstr = STR_INVERSE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"선택 레이어 영역 반전"));
		locMap.rstr = STR_FFS_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"선택 레이어 차집합(A=A-B)"));
		locMap.rstr = STR_INTERSECTION_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"선택 레이어 교집합"));
		locMap.rstr = STR_MERGE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"선택 레이어 병합"));
		locMap.rstr = STR_SPLIT_REGION;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"선택 레이어 분할"));
		locMap.rstr = STR_EROSION_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Erosion"));
		locMap.rstr = STR_DILATION_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Dilation"));
		locMap.rstr = STR_MOVE_MASK_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Move Mask"));
		locMap.rstr = STR_SET_SEED_POINT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Set Seed Point"));
		locMap.rstr = STR_HOLEFILLING_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Hole Filling(3D)"));
		locMap.rstr = STR_HOLEFILLING2D_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Hole Filling(2D)"));
		locMap.rstr = STR_3D_INTERPOLATION;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"3D Interpolation"));

		locMap.rstr = STR_COMPONENT_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Component Choice"));

		locMap.rstr = STR_VESSELNESS_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Vesselness"));
		locMap.rstr = STR_FISSURENESS_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Fissureness"));

		locMap.rstr = STR_ANTI_MULTIPLE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"다중 선택을 지원하지 않는 기능입니다."));
		locMap.rstr = STR_ONE_SELECT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"1개의 아이템을 선택하십시오. :: "));


		locMap.rstr = STR_IMG_EROSION;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Image Erosion"));
		locMap.rstr = STR_IMG_DILATION;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Image Dilation"));
		locMap.rstr = STR_IMG_MOVE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Image Move"));
		locMap.rstr = STR_SELECT_2_WARN;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"2개의 레이어만 선택하십시오."));
		locMap.rstr = STR_SELECT_MORE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"여러 개의 레이어를 선택하십시오"));
		locMap.rstr = STR_SELECT_L3;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"L3 레이어를 선택하십시오"));
		locMap.rstr = STR_SELECT_ABDOMINAL_WAIST;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Abdominal Waist 레이어를 선택하십시오"));
		locMap.rstr = STR_WARN;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"경고"));
		locMap.rstr = STR_SELECT_SEED_FIRST;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"\"Seed point 지정\" 버튼을 누르고 화면 상의 지점을 클릭하십시오."));
		locMap.rstr = STR_SELECT_RANGE_FIRST;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"\"Level\" 범위 값을 먼저 입력하십시오."));

		locMap.rstr = STR_INVALID_NUMBER;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"올바르지 않은 포맷입니다. (낮은 숫자 ~ 높은 숫자)"));
		locMap.rstr = STR_LOAD_FIRST;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"DICOM 파일을 먼저 로딩하십시오"));
		locMap.rstr = STR_LOAD_FILE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"파일 로딩중..."));
		locMap.rstr = STR_CROPPING_DICOM_DATA;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"DICOM 데이터 Cropping 중..."));
		locMap.rstr = STR_LOAD_DICOM_FILE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"DICOM 파일 로딩중..."));
		locMap.rstr = STR_LOAD_DICOM_FILE_FAIL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"DICOM 파일 로딩에 실패하였습니다."));

		locMap.rstr = STR_DELETE_FIRST;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"레이어를 최대갯수까지 생성하였습니다. 레이어 삭제 후 다시 시도하여 주십시오."));
		locMap.rstr = STR_DIFFER_SIZE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"해당 파일은 이미지 크기와 동일하지 않아 로딩할 수 없습니다."));

		locMap.rstr = STR_PREVIEW_SURFACE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Surface 미리보기"));
		locMap.rstr = STR_EXPORT_STL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"STL 파일 추출"));
		locMap.rstr = STR_EXPORT_OBJ;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"OBJ 파일 추출"));
		locMap.rstr = STR_EXPORT_3MF; 
		strList.insert(locMap.rstr, QString::fromWCharArray(L"3MF 파일 추출"));
		locMap.rstr = STR_EXPORT_USD; 
		strList.insert(locMap.rstr, QString::fromWCharArray(L"USD 파일 추출"));

		locMap.rstr = STR_EXPORT_RAW;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Raw(Mask) 파일 추출"));

		locMap.rstr = STR_EXPORT_HURAW;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Raw(HU) 파일 추출"));

		locMap.rstr = STR_EXPORT_TXT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"TXT 파일 추출"));

		locMap.rstr = STR_FORWARD_SEED;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"ForeSeed로 영역 복사"));
		locMap.rstr = STR_BACKWARD_SEED;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"BackSeed로 영역 복사"));

		locMap.rstr = STR_Y_FLIP;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Y축 뒤집기"));

		locMap.rstr = STR_X_FLIP;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"X축 뒤집기"));

		locMap.rstr = STR_FAILED_TO;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"실패하였습니다. :: "));

		locMap.rstr = STR_MODALITY;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Modality"));
		locMap.rstr = STR_MACHINE_MODEL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Machine Model"));
		locMap.rstr = STR_STUDY_DESC;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Study Description"));
		locMap.rstr = STR_SERIES_DESC;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Series Description"));
		locMap.rstr = STR_ID;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"ID"));
		locMap.rstr = STR_UID;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Series UID"));
		locMap.rstr = STR_IMG_POS;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Image Position"));
		locMap.rstr = STR_EXAM_DATE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Exam Date"));
		locMap.rstr = STR_NAME;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Patient Name"));
		locMap.rstr = STR_SLICE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Slice"));
		locMap.rstr = STR_ACQUISITION_TIME;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Acquisition Time"));
		locMap.rstr = STR_SLICE_THICK;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Slice Thickness"));
		locMap.rstr = STR_SEX;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Sex"));
		locMap.rstr = STR_KVP;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Kvp"));
		locMap.rstr = STR_AGE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Age"));
		locMap.rstr = STR_MAS;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"mAs"));
		locMap.rstr = STR_WXH;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"W x H x D"));
		locMap.rstr = STR_X_SPACING;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"X Spacing"));
		locMap.rstr = STR_Y_SPACING;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Y Spacing"));
		locMap.rstr = STR_Z_SPACING;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Z Spacing"));

		locMap.rstr = STR_SCALE_NOT_MATCHED;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"파일 크기가 맞지 않습니다."));


		locMap.rstr = STR_SMOOTHING_LEVEL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Smooth"));
		locMap.rstr = STR_COLOR_CHANGE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"문구 색상 "));
		locMap.rstr = STR_TEXT_SIZE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"문구 크기 "));

		locMap.rstr = STR_INIT_FAIL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"초기화 실패"));
		//locMap.rstr = STR_DX_INIT_FAIL;
		//strList.insert(locMap.rstr, QString::fromWCharArray(L"OpenGL 초기화에 실패하였습니다."));
		locMap.rstr = STR_APP_INIT_FAIL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"어플리케이션 초기화에 실패하였습니다"));
		//locMap.rstr = STR_RC_INIT_FAIL;
		//strList.insert(locMap.rstr, QString::fromWCharArray(L"리소스 파일을 찾을 수 없습니다."));
		locMap.rstr = STR_APP_SYSTEMDATE_INVALID;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"시스템 시간이 마지막 실행 시간보다 이전입니다. 확인 후 다시 시작해주세요"));
		//locMap.rstr = STR_APP_MEDIPVERSIONINFO_FILE_LOAD_FAIL;
		//strList.insert(locMap.rstr, QString::fromWCharArray(L"제품 정보가 있는 \"medipVersionInfo.dat\" 파일 로드에 실패하였습니다. 확인 후 다시 시작해주세요"));


		locMap.rstr = STR_INFO_DIALOG;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"MEDIP 정보"));

		locMap.rstr = STR_LOGIN_DIALOG;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"로그인"));

		locMap.rstr = STR_OFFLINELICENSEREG_DIALOG;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"오프라인 라이센스 등록"));

		locMap.rstr = STR_COPY_CODE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"코드 복사"));

		locMap.rstr = STR_ACTIVATE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"활성화"));

		locMap.rstr = STR_PROGRAM_VERSION;
		//		strList.insert(locMap.rstr, QString::fromWCharArray(L"Version %1 (%2)").arg(VER_PRODUCTVERSION_STR).arg(RELEASE_DATE));

		locMap.rstr = STR_PROGRAM_TITLE_CONTENT;
		strList.insert(locMap.rstr, QString(" - %1").arg(WIN_MANAGER->IsLicensePass() ? QString::fromWCharArray(L"교육 및 연구 용도") : QString::fromWCharArray(L"의료 용도로 사용하지 말 것")));

		//locMap.rstr = STR_PROGRAM_EDITION;
		//strList.insert(locMap.rstr, QString::fromWCharArray(L"<strong>%1 v%2</strong>")
		//	.arg(QString(LICENSE_DATA->getProductType()) + " " + QString::fromUtf8(PRODUCT_MANAGER->GetProductInfo(PRODUCT_NAME_KEY).c_str()) + QString(VER_ADD_STR))
		//	.arg(QString(VER_FILE_VERSION_STR_NO_BUILD)));


		locMap.rstr = STR_PROGRAM_COPYRIGHT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Copyright (c) 2017 All rights reserved by MEDICALIP"));

		locMap.rstr = STR_MEDICALIP_ADDR;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"<a href = \"MEDICALIP\">http://www.medicalip.com</a>"));

		locMap.rstr = STR_OPENSOURCE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"<a href = \"OPENSOURCE\">Open Source</a>"));

		locMap.rstr = STR_MODULE_INFO;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"<a href = \"MODULEINFO\">Module Info</a>"));

		locMap.rstr = STR_PROGRAM_DESC;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Developed by MEDICALIP Co., Ltd. <strong>MEDIP은 의료 영상 처리 소프트웨어입니다.</strong>"
			"MEDIP의 인터페이스는 사용자 편의성에 중점을 두고 설계되었습니다. "
			"세그멘테이션과 분석 기능은 어떠한 의료 영상에서도 효율적으로 동작하게끔 되어있습니다. (CT, MRI, etc). <br/>"
			"MEDIP이 지닌 강력한 기능들;  Region of interest(ROI) 선택, 3D image 퀄리티 향상, "
			"그리고 다양한 장기 분할 세그멘테이션. A segmentation process can be easily applied by using this software. <br/>"
			"In addition, it is possible to export \"STL\" file format for printing with any 3D printer in the world."));

		locMap.rstr = STR_LICENSE_WARN;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"\n라이센스 발급 이후 사용 가능한 동작입니다."));

		locMap.rstr = STR_APP_LICENSE_FAIL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"올바르지 않은 라이센스를 사용하고 있습니다.\n라이센스를 발급받은 PC에서 사용하십시오."));

		locMap.rstr = STR_APP_PASSWORD_FAIL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"올바르지 않은 비밀번호입니다."));

		locMap.rstr = STR_PASSED_MSG;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"<strong>기능 확장된 버전을 사용하고 계십니다.</strong><br><br>저희 프로그램을 사용해주셔서 감사합니다."));

		//locMap.rstr = STR_ALREADY_EXECUTE;
		//strList.insert(locMap.rstr, QString::fromWCharArray(L"실행된 프로그램이 존재합니다."));


		locMap.rstr = STR_HISTOGRAM_DESC;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"window width/level 히스토그램 확인"));

		locMap.rstr = ERR_DU_1016_STR_ALREADY_EXIST_THREAD;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"동작하고 있는 기능이 있습니다. 잠시 후 다시 시도해주십시오."));
		locMap.rstr = STR_WORK_THREAD;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"동작 중인 기능..."));
		locMap.rstr = STR_WORK_THREAD_CANCEL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"기능 동작이 취소되었습니다."));
		locMap.rstr = STR_WORK_THREAD_FAIL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"메모리 부족으로 인해 기능 동작이 실패하였습니다."));

		locMap.rstr = STR_SEED_POINT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Seed point 지정"));
		locMap.rstr = STR_SEED_POINT_SHOW;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Seed point 표시"));
		locMap.rstr = STR_SEED_POINT_HIDE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Seed point 감추기"));
		locMap.rstr = STR_ANNO_LIST;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Annotation 리스트"));

		locMap.rstr = STR_ANNO_TEXT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Text"));
		locMap.rstr = STR_ANNO_LENGTH;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Length"));
		locMap.rstr = STR_ANNO_ANGLE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Angle"));
		locMap.rstr = STR_ANNO_OVAL;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Oval"));
		locMap.rstr = STR_ANNO_ARROW;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Arrow"));
		locMap.rstr = STR_ANNO_RECTANGLE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Rectangle"));
		locMap.rstr = STR_ANNO_PROF_DRAW;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Draw"));
		locMap.rstr = STR_ANNO_PROF_POLY;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Polygon"));

		locMap.rstr = STR_2D_SCREEN;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"2D Screen"));
		locMap.rstr = STR_3D_VOLUME;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"3D Volume"));

		locMap.rstr = STR_LAYER_HISTOGRAM;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"View histogram"));

		locMap.rstr = STR_SHOW_ANI_POINT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Annotation path points 표시"));
		locMap.rstr = STR_HIDE_ANI_POINT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Annotation path points 감추기"));

		locMap.rstr = STR_VR_POINT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"클릭한 지점 VR로 확인"));

		locMap.rstr = STR_3D_TO_2D;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"클릭한 지점 2D View에서 확인"));

		locMap.rstr = STR_SVIEW;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Cross-section"));

		locMap.rstr = STR_GAMMA;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Gamma"));

		//locMap.rstr = STR_ALLOC_FAIL;
		//strList.insert(locMap.rstr, QString::fromWCharArray(L"로딩에 필요한 메모리가 부족합니다.\n"
		//	"파일 크롭 작업을 통해 로딩하십시오."));

		locMap.rstr = STR_EXPORT_VTK;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"VTK 파일 추출"));

		locMap.rstr = STR_EXPORT_NII;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"NII 파일 추출"));

		locMap.rstr = STR_MEDIPWINDOW_TITLE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"MEDIP - Medical Imaging & Printing"));

		locMap.rstr = STR_CONNECT_HOMEPAGE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Connect to MedicalIP"));

		locMap.rstr = STR_TABLIST_BTN_TOOLTIP;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Tab list 표시"));

		locMap.rstr = STR_IMAGELIST_BTN_TOOLTIP;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Image list 표시"));

		locMap.rstr = STR_MESSAGE_FILE_LOAD_FAILED;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"파일이 로드되지 않았습니다."));

		locMap.rstr = STR_MESSAGE_TITLE_CROPOPEN_FILE_FAILED;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"열 수 없음 ::"));

		locMap.rstr = STR_MESSAGE_TEXT_CROPOPEN_FILE_FAILED;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"DICOM 파일만 크롭 가능합니다."));

		locMap.rstr = STR_CROP_WINDOW_TILE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Crop the dicom"));

		locMap.rstr = STR_CROP_MEMORY_INFO;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"메모리 정보"));

		locMap.rstr = STR_CROP_LENGTH_INFO;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Length Info"));

		locMap.rstr = STR_CROP_DEPTH_INFO;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Z 축 정보"));

		locMap.rstr = STR_CAPTURE_TAB;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Capture"));

		locMap.rstr = STR_IMAGE_IMPORT_TAB;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"가져오기"));

		locMap.rstr = STR_BTN_TA_FEATURE_EXTRACT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"추출"));

		locMap.rstr = STR_BTN_TA_FEATURE_ALL_EXTRACT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"전체추출"));

		locMap.rstr = STR_LOGIN_ID;
		strList.insert(locMap.rstr, QString("아이디"));
		locMap.rstr = STR_LOGIN_PWD;
		strList.insert(locMap.rstr, QString("패스워드"));
		locMap.rstr = STR_LOGIN;
		strList.insert(locMap.rstr, QString("로그인"));
		locMap.rstr = STR_LOGIN_IDSAVE;
		strList.insert(locMap.rstr, QString("아이디 저장"));
		locMap.rstr = STR_LOGIN_AUTO;
		strList.insert(locMap.rstr, QString("자동 로그인"));
		
		locMap.rstr = STR_UID_SECONDARY_CAPTUREIMAGE_STORAGE;
		strList.insert(locMap.rstr, QString("1.2.840.10008.5.1.4.1.1.7"));
		break;
	}
	case ENGLISH:
	{

		locMap.rstr = STR_EXPORT_SPLIT_FILE;
		strList.insert(locMap.rstr, QString("Split to files"));

		locMap.rstr = STR_TA;
		//strList.insert(locMap.rstr, QString("Texture Analysis"));
		strList.insert(locMap.rstr, QString("Radiomics"));

		locMap.rstr = STR_CUSTOM;
		strList.insert(locMap.rstr, QString("Custom"));

		locMap.rstr = STR_SAVE_CAPTURE;
		strList.insert(locMap.rstr, QString("Capture image save"));

		locMap.rstr = STR_EXIST_TEMP_FILE;
		strList.insert(locMap.rstr, QString("Temporary file already exists. Please delete the file and try again."));

		locMap.rstr = STR_SOFT_TISSUE;
		strList.insert(locMap.rstr, QString("Soft tissue"));

		locMap.rstr = STR_AIR;
		strList.insert(locMap.rstr, QString("Air"));

		locMap.rstr = STR_FAT;
		strList.insert(locMap.rstr, QString("Fat"));

		locMap.rstr = STR_FLUIDS;
		strList.insert(locMap.rstr, QString("Fluids"));

		locMap.rstr = STR_PRESET;
		strList.insert(locMap.rstr, QString("Preset"));

		locMap.rstr = STR_ANNOTATION;
		strList.insert(locMap.rstr, QString("Annotation"));

		locMap.rstr = STR_LEVEL_SET;
		strList.insert(locMap.rstr, QString("Level set"));

		locMap.rstr = STR_PASSWORD;
		strList.insert(locMap.rstr, QString("Input Password"));

		locMap.rstr = STR_DRAW_CUT;
		strList.insert(locMap.rstr, QString("Image Draw Cut"));


		locMap.rstr = STR_RECOMMEND_1;
		strList.insert(locMap.rstr, QString("Recommend 1"));
		locMap.rstr = STR_RECOMMEND_2;
		strList.insert(locMap.rstr, QString("Recommend 2"));

		locMap.rstr = STR_ISOTROPIC;
		strList.insert(locMap.rstr, QString("Iso-Z"));

		locMap.rstr = STR_SAVE_FILE;
		strList.insert(locMap.rstr, QString("Save"));
		locMap.rstr = STR_SAVE_FILE_DESC;
		strList.insert(locMap.rstr, QString("Do you want to save changes to the file?"));

		locMap.rstr = STR_OPEN;
		strList.insert(locMap.rstr, QString("Open"));
		locMap.rstr = STR_MEDIP_FILE_SAVE;
		strList.insert(locMap.rstr, QString("Medical IP file save"));
		locMap.rstr = STR_MEDIP_FILE_OPEN;
		strList.insert(locMap.rstr, QString("Medical IP file open"));
		locMap.rstr = STR_CROP;
		strList.insert(locMap.rstr, QString("Open cropped file"));

		locMap.rstr = STR_SAVE_AS;
		strList.insert(locMap.rstr, QString("Save as"));
		locMap.rstr = STR_UPLOAD_PACS;
		strList.insert(locMap.rstr, QString("Upload to PACS"));
		locMap.rstr = STR_LOAD;
		strList.insert(locMap.rstr, QString("Load"));

		locMap.rstr = STR_UPLOAD;
		strList.insert(locMap.rstr, QString("Send file to server"));
		locMap.rstr = STR_UPLOAD_SUCCESS;
		strList.insert(locMap.rstr, QString("Send project file succeeded."));
		locMap.rstr = STR_UPLOAD_FAIL;
		strList.insert(locMap.rstr, QString("Send project file failed."));

		locMap.rstr = STR_REMOTE_FILE;
		strList.insert(locMap.rstr, QString("Remote file upload"));
		locMap.rstr = STR_REMOTE_FILE_DELAYED;
		strList.insert(locMap.rstr, QString("Remote file uploading is delayed."));


		locMap.rstr = STR_SAVE_OBJ_FILE;
		strList.insert(locMap.rstr, QString("Export to OBJ file"));

		locMap.rstr = STR_SHOW_ANNO;
		strList.insert(locMap.rstr, QString("Show annotations"));
		locMap.rstr = STR_HIDE_ANNO;
		strList.insert(locMap.rstr, QString("Hide annotations"));


		locMap.rstr = STR_FILE_TRANSFER;
		strList.insert(locMap.rstr, QString("File transfer"));
		locMap.rstr = STR_TRANSFER_DESC;
		strList.insert(locMap.rstr, QString("Would you like to upload the current file to \"Medical IP\" server?"));

		locMap.rstr = STR_CONTACT;
		strList.insert(locMap.rstr, QString("Contact us"));
		locMap.rstr = STR_SEGMENT;
		strList.insert(locMap.rstr, QString("Segmentation"));
		locMap.rstr = STR_MEASUREMENT;
		strList.insert(locMap.rstr, QString("Measurement"));
		locMap.rstr = STR_MESH_EDITING;
		strList.insert(locMap.rstr, QString("Mesh editing"));
		locMap.rstr = STR_REPORT;
		strList.insert(locMap.rstr, QString("Report"));
		locMap.rstr = STR_PLANNING;
		strList.insert(locMap.rstr, QString("Planning"));
		


		locMap.rstr = STR_ENHANCED;
		strList.insert(locMap.rstr, QString("Enhancement"));
		locMap.rstr = STR_ROI;
		strList.insert(locMap.rstr, QString("ROI"));
		locMap.rstr = STR_INFO;
		strList.insert(locMap.rstr, QString("Information"));
		locMap.rstr = STR_INFO_DESC;
		strList.insert(locMap.rstr, QString("Information / logout"));

		locMap.rstr = STR_WINDOW_WL;
		strList.insert(locMap.rstr, QString("Window width & level"));
		locMap.rstr = STR_DEFAULT;
		strList.insert(locMap.rstr, QString("Default"));
		locMap.rstr = STR_BONE;
		strList.insert(locMap.rstr, QString("Bone (CT)"));
		locMap.rstr = STR_LUNG;
		strList.insert(locMap.rstr, QString("Lung (CT)"));
		locMap.rstr = STR_MEDIASTINAL;
		strList.insert(locMap.rstr, QString("Mediastinal (CT)"));
		locMap.rstr = STR_BRAIN;
		strList.insert(locMap.rstr, QString("Brain (CT)"));
		locMap.rstr = STR_ABD;
		strList.insert(locMap.rstr, QString("ABD (CT)"));
		locMap.rstr = STR_LIVER;
		strList.insert(locMap.rstr, QString("Liver (CT)"));
		locMap.rstr = STR_KIDNEY;
		strList.insert(locMap.rstr, QString("Kidney (CT)"));
		locMap.rstr = STR_URINARY_TRACK_BLADDER;
		strList.insert(locMap.rstr, QString("Urinary track + bladder (CT)"));

		locMap.rstr = STR_WIDTH;
		strList.insert(locMap.rstr, QString("Width"));
		locMap.rstr = STR_HEIGHT;
		strList.insert(locMap.rstr, QString("Height"));
		locMap.rstr = STR_LEVEL;
		strList.insert(locMap.rstr, QString("Level"));
		locMap.rstr = STR_WORK;
		strList.insert(locMap.rstr, QString("Working.."));
		locMap.rstr = STR_OK;
		strList.insert(locMap.rstr, QString("OK"));
		locMap.rstr = STR_CANCEL;
		strList.insert(locMap.rstr, QString("Cancel"));
		locMap.rstr = STR_AUTO;
		strList.insert(locMap.rstr, QString("Auto"));
		locMap.rstr = STR_RESET;
		strList.insert(locMap.rstr, QString("Reset"));
		locMap.rstr = STR_APPLY;
		strList.insert(locMap.rstr, QString("Apply"));
		locMap.rstr = STR_Z_FLIP;
		strList.insert(locMap.rstr, QString("Z-Flip"));
		locMap.rstr = STR_IMG_ENHANCED;
		strList.insert(locMap.rstr, QString("Image enhancement"));
		locMap.rstr = STR_ORG_IMG;
		strList.insert(locMap.rstr, QString("Original image"));
		locMap.rstr = STR_IMG_CIRCUMFERENCE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Image circumference"));

		locMap.rstr = STR_RG;
		strList.insert(locMap.rstr, QString("Region growing"));
		locMap.rstr = STR_NOT_SELECTED;
		strList.insert(locMap.rstr, QString("Not selected seed"));
		locMap.rstr = STR_SELECT_SEED;
		strList.insert(locMap.rstr, QString("Select seed"));
		locMap.rstr = STR_IRP;
		strList.insert(locMap.rstr, QString("Inside region proposal"));
		locMap.rstr = STR_SEED_APPLY;
		strList.insert(locMap.rstr, QString("Seed apply"));
		locMap.rstr = STR_ALL_SELECT;
		strList.insert(locMap.rstr, QString("Range apply"));
		locMap.rstr = STR_RG_ALL;
		strList.insert(locMap.rstr, QString("Region all select"));
		locMap.rstr = STR_ROI_LIST;
		strList.insert(locMap.rstr, QString("ROI list"));

		locMap.rstr = STR_CLR_ROI;
		strList.insert(locMap.rstr, QString("Clear layer"));
		locMap.rstr = STR_NEW;
		strList.insert(locMap.rstr, QString("New"));
		locMap.rstr = STR_RENAME;
		strList.insert(locMap.rstr, QString("Rename"));
		locMap.rstr = STR_DELETE;
		strList.insert(locMap.rstr, QString("Delete"));
		locMap.rstr = STR_COPY;
		strList.insert(locMap.rstr, QString("Duplicate"));
		locMap.rstr = STR_INVERSE;
		strList.insert(locMap.rstr, QString("Inverse"));
		locMap.rstr = STR_FFS_ROI;
		strList.insert(locMap.rstr, QString("First=First-Second"));
		locMap.rstr = STR_INTERSECTION_ROI;
		strList.insert(locMap.rstr, QString("Intersection"));
		locMap.rstr = STR_MERGE;
		strList.insert(locMap.rstr, QString("Merge"));
		locMap.rstr = STR_SPLIT_REGION;
		strList.insert(locMap.rstr, QString("Split region"));
		locMap.rstr = STR_EROSION_ROI;
		strList.insert(locMap.rstr, QString("Erosion"));
		locMap.rstr = STR_DILATION_ROI;
		strList.insert(locMap.rstr, QString("Dilation"));
		locMap.rstr = STR_MOVE_MASK_ROI;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Move mask"));
		locMap.rstr = STR_SET_SEED_POINT;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Set seed point"));
		locMap.rstr = STR_HOLEFILLING_ROI;
		strList.insert(locMap.rstr, QString("Hole filling(3D)"));
		locMap.rstr = STR_HOLEFILLING2D_ROI;
		strList.insert(locMap.rstr, QString("Hole filling(2D)"));
		locMap.rstr = STR_COMPONENT_ROI;
		strList.insert(locMap.rstr, QString("Component Choice"));
		locMap.rstr = STR_3D_INTERPOLATION;
		strList.insert(locMap.rstr, QString("3D Interpolation"));

		locMap.rstr = STR_VESSELNESS_ROI;
		strList.insert(locMap.rstr, QString("Vesselness"));
		locMap.rstr = STR_FISSURENESS_ROI;
		strList.insert(locMap.rstr, QString("Fissureness"));

		locMap.rstr = STR_ANTI_MULTIPLE;
		strList.insert(locMap.rstr, QString("Does not support action for multiple selection."));
		locMap.rstr = STR_ONE_SELECT;
		strList.insert(locMap.rstr, QString("Please select one item to "));
		
		locMap.rstr = STR_IMG_EROSION;
		strList.insert(locMap.rstr, QString("Image erosion"));
		locMap.rstr = STR_IMG_DILATION;
		strList.insert(locMap.rstr, QString("Image dilation"));
		locMap.rstr = STR_IMG_MOVE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Image move"));
		locMap.rstr = STR_IMG_COMPONENT_CHOISE;
		strList.insert(locMap.rstr, QString("Component Choice"));
		locMap.rstr = STR_SELECT_2_WARN;
		strList.insert(locMap.rstr, QString("Only select 2 layer!!"));
		locMap.rstr = STR_SELECT_MORE;
		strList.insert(locMap.rstr, QString("Select more layer!!"));
		locMap.rstr = STR_SELECT_L3;
		strList.insert(locMap.rstr, QString("Select L3 Layer"));
		locMap.rstr = STR_SELECT_ABDOMINAL_WAIST;
		strList.insert(locMap.rstr, QString("Select Abdominal Waist Layer"));
		locMap.rstr = STR_WARN;
		strList.insert(locMap.rstr, QString("Warning"));
		locMap.rstr = STR_SELECT_SEED_FIRST;
		strList.insert(locMap.rstr, QString("Please press the \"Select seed\" button and select the pixel in the image."));
		locMap.rstr = STR_SELECT_RANGE_FIRST;
		strList.insert(locMap.rstr, QString("Please input the \"Level\" range value first."));

		locMap.rstr = STR_INVALID_NUMBER;
		strList.insert(locMap.rstr, QString("Invalid number format. (Lower number ~ Upper number)"));
		locMap.rstr = STR_LOAD_FIRST;
		strList.insert(locMap.rstr, QString("Load dicom image first."));
		locMap.rstr = STR_LOAD_FILE;
		strList.insert(locMap.rstr, QString("File loading"));
		locMap.rstr = STR_CROPPING_DICOM_DATA;
		strList.insert(locMap.rstr, QString("DICOM Data Cropping in progress..."));
		locMap.rstr = STR_LOAD_DICOM_FILE;
		strList.insert(locMap.rstr, QString("Loading DICOM file"));
		locMap.rstr = STR_LOAD_DICOM_FILE_FAIL;
		strList.insert(locMap.rstr, QString("DICOM file loading failed"));

		locMap.rstr = STR_DELETE_FIRST;
		strList.insert(locMap.rstr, QString("Mask list is full. Delete item first."));
		locMap.rstr = STR_DIFFER_SIZE;
		strList.insert(locMap.rstr, QString("Different size between image and File."));

		locMap.rstr = STR_PREVIEW_SURFACE;
		strList.insert(locMap.rstr, QString("Preview surface"));
		locMap.rstr = STR_EXPORT_STL;
		strList.insert(locMap.rstr, QString("Export stl file"));
		locMap.rstr = STR_EXPORT_OBJ;
		strList.insert(locMap.rstr, QString("Export obj file"));
		locMap.rstr = STR_EXPORT_3MF;
		strList.insert(locMap.rstr, QString("Export 3mf file"));
		locMap.rstr = STR_EXPORT_USD;
		strList.insert(locMap.rstr, QString("Export usd file"));
		locMap.rstr = STR_EXPORT_RAW;
		strList.insert(locMap.rstr, QString("Export raw file (mask)"));

		locMap.rstr = STR_EXPORT_HURAW;
		strList.insert(locMap.rstr, QString("Export raw file (HU)"));

		locMap.rstr = STR_EXPORT_TXT;
		strList.insert(locMap.rstr, QString("Export TXT file"));

		locMap.rstr = STR_FORWARD_SEED;
		strList.insert(locMap.rstr, QString("Copy to foreseed"));
		locMap.rstr = STR_BACKWARD_SEED;
		strList.insert(locMap.rstr, QString("Copy to backseed"));

		locMap.rstr = STR_Y_FLIP;
		strList.insert(locMap.rstr, QString("Y-flip"));
		locMap.rstr = STR_X_FLIP;
		strList.insert(locMap.rstr, QString("X-flip"));

		locMap.rstr = STR_FAILED_TO;
		strList.insert(locMap.rstr, QString("Failed to "));

		locMap.rstr = STR_MODALITY;
		strList.insert(locMap.rstr, QString("Modality"));
		locMap.rstr = STR_MACHINE_MODEL;
		strList.insert(locMap.rstr, QString("Machine model"));
		locMap.rstr = STR_STUDY_DESC;
		strList.insert(locMap.rstr, QString("Study description"));
		locMap.rstr = STR_SERIES_DESC;
		strList.insert(locMap.rstr, QString("Series description"));
		locMap.rstr = STR_ID;
		strList.insert(locMap.rstr, QString("ID"));
		locMap.rstr = STR_UID;
		strList.insert(locMap.rstr, QString("Series UID"));
		locMap.rstr = STR_IMG_POS;
		strList.insert(locMap.rstr, QString("Image position"));
		locMap.rstr = STR_IMG_ORIENTATION;
		strList.insert(locMap.rstr, QString("Image orientation"));
		locMap.rstr = STR_EXAM_DATE;
		strList.insert(locMap.rstr, QString("Exam date"));
		locMap.rstr = STR_NAME;
		strList.insert(locMap.rstr, QString("Patient name"));
		locMap.rstr = STR_SLICE;
		strList.insert(locMap.rstr, QString("Slice"));
		locMap.rstr = STR_ACQUISITION_TIME;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"Acquisition Time"));
		locMap.rstr = STR_SLICE_THICK;
		strList.insert(locMap.rstr, QString("Slice thickness"));
		locMap.rstr = STR_SEX;
		strList.insert(locMap.rstr, QString("Sex"));
		locMap.rstr = STR_KVP;
		strList.insert(locMap.rstr, QString("Kvp"));
		locMap.rstr = STR_AGE;
		strList.insert(locMap.rstr, QString("Age"));
		locMap.rstr = STR_WEIGHT;
		strList.insert(locMap.rstr, QString("Weight"));
		locMap.rstr = STR_MAS;
		strList.insert(locMap.rstr, QString("mAs"));
		locMap.rstr = STR_WXH;
		strList.insert(locMap.rstr, QString("W x H x D"));
		locMap.rstr = STR_X_SPACING;
		strList.insert(locMap.rstr, QString("X spacing"));
		locMap.rstr = STR_Y_SPACING;
		strList.insert(locMap.rstr, QString("Y spacing"));
		locMap.rstr = STR_Z_SPACING;
		strList.insert(locMap.rstr, QString("Z spacing"));
		locMap.rstr = STR_KERNEL;
		strList.insert(locMap.rstr, QString("Kernel"));
		locMap.rstr = STR_MANUFACTURER;
		strList.insert(locMap.rstr, QString("Manufacturer"));
		locMap.rstr = STR_MANUFACTURER_MODEL;
		strList.insert(locMap.rstr, QString("Manufacturer's model name"));
		locMap.rstr = STR_STUDY_INSTANCE_UID;
		strList.insert(locMap.rstr, QString("Study Instance UID"));
		locMap.rstr = STR_SERIES_INSTANCE_UID;
		strList.insert(locMap.rstr, QString("Series Instance UID"));
		locMap.rstr = STR_EXAM_ID;
		strList.insert(locMap.rstr, QString("Exam ID"));
		locMap.rstr = STR_ACCESSION_NUMBER;
		strList.insert(locMap.rstr, QString("Accession Number"));
		locMap.rstr = STR_ECHO_TIME;
		strList.insert(locMap.rstr, QString("Echo Time"));
		locMap.rstr = STR_REPETITION_TIME;
		strList.insert(locMap.rstr, QString("Repetition Time"));
		locMap.rstr = STR_MAGNETIC_FIELD_STRENGTH;
		strList.insert(locMap.rstr, QString("Magnetic Field Strength"));
		locMap.rstr = STR_FILP_ANGLE;
		strList.insert(locMap.rstr, QString("Flip Angle"));

		locMap.rstr = STR_NONE_DEEPDRAW_PACK;
		strList.insert(locMap.rstr, QString("You can use this function by adding the AI Pack.<a href='http://www.medicalip.com/Pricing\'><br>Please contact medicalip.</a>"));

		locMap.rstr = STR_SCALE_NOT_MATCHED;
		strList.insert(locMap.rstr, QString("This file scale is not matched."));


		locMap.rstr = STR_SMOOTHING_LEVEL;
		strList.insert(locMap.rstr, QString("Smooth"));
		locMap.rstr = STR_COLOR_CHANGE;
		strList.insert(locMap.rstr, QString("Text color"));
		locMap.rstr = STR_TEXT_SIZE;
		strList.insert(locMap.rstr, QString("Text size "));

		locMap.rstr = STR_INIT_FAIL;
		strList.insert(locMap.rstr, QString("Init fail"));
		//locMap.rstr = STR_DX_INIT_FAIL;
		//strList.insert(locMap.rstr, QString("OpenGL initializing error."));
		locMap.rstr = STR_APP_INIT_FAIL;
		strList.insert(locMap.rstr, QString("Application init fail."));
		//locMap.rstr = STR_RC_INIT_FAIL;
		//strList.insert(locMap.rstr, QString("Not found resource file."));
		locMap.rstr = STR_APP_SYSTEMDATE_INVALID;
		strList.insert(locMap.rstr, QString("System time is invalid. try again after synchronizing current time."));
		//locMap.rstr = STR_APP_MEDIPVERSIONINFO_FILE_LOAD_FAIL;
		//strList.insert(locMap.rstr, QString("\"medipVersionInfo.dat\" file with product information failed to load."));

		locMap.rstr = STR_INFO_DIALOG;
		strList.insert(locMap.rstr, QString("Information dialog"));

		locMap.rstr = STR_LOGIN_DIALOG;
		strList.insert(locMap.rstr, QString("Login dialog"));

		locMap.rstr = STR_OFFLINELICENSEREG_DIALOG;
		strList.insert(locMap.rstr, QString("Offline license dialog"));

		locMap.rstr = STR_COPY_CODE;
		strList.insert(locMap.rstr, QString("Copy dode"));

		locMap.rstr = STR_ACTIVATE;
		strList.insert(locMap.rstr, QString("Activate"));

		locMap.rstr = STR_PROGRAM_VERSION;
#if defined(COVID19_VER)
		strList.insert(locMap.rstr, QString("Version %1 (%2)").arg(QString(VER_FILE_VERSION_STR_NO_BUILD)).arg(RELEASE_DATE));
#else
		strList.insert(locMap.rstr, QString("Version %1 (%2)").arg(QString(VER_FILE_VERSION_STR)).arg(RELEASE_DATE));
#endif

		locMap.rstr = STR_PROGRAM_COPYRIGHT;
		strList.insert(locMap.rstr, QString("Copyright (c) All rights reserved by MEDICALIP"));

		locMap.rstr = STR_MEDICALIP_ADDR;
		strList.insert(locMap.rstr, QString("<a href = \"MEDICALIP\">http://www.medicalip.com</a>"));

		locMap.rstr = STR_OPENSOURCE;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"<a href = \"OPENSOURCE\">Open Source</a>"));
				
		locMap.rstr = STR_MODULE_INFO;
		strList.insert(locMap.rstr, QString::fromWCharArray(L"<a href = \"MODULEINFO\">Module Info</a>"));

		locMap.rstr = STR_LICENSE_WARN;
		strList.insert(locMap.rstr, QString("\nExecution failed, need a license key."));

		locMap.rstr = STR_APP_LICENSE_FAIL;
		strList.insert(locMap.rstr, QString("You are using a disabled license.\nRun the program on the activated PC."));

		locMap.rstr = STR_APP_PASSWORD_FAIL;
		strList.insert(locMap.rstr, QString("Incorrect password."));

		locMap.rstr = STR_PASSED_MSG;
		strList.insert(locMap.rstr, QString("Thank you for using our program."));

		//locMap.rstr = STR_ALREADY_EXECUTE;
		//strList.insert(locMap.rstr, QString("The executed program exists."));


		locMap.rstr = STR_HISTOGRAM_DESC;
		strList.insert(locMap.rstr, QString("Threshold range"));
			
		locMap.rstr = STR_WORK_THREAD;
		strList.insert(locMap.rstr, QString("The working thread"));
		locMap.rstr = STR_WORK_THREAD_CANCEL;
		strList.insert(locMap.rstr, QString("The working thread canceled"));
		locMap.rstr = STR_WORK_THREAD_FAIL;
		strList.insert(locMap.rstr, QString("The working thread failed : Out of memory"));

		locMap.rstr = STR_SEED_POINT;
		strList.insert(locMap.rstr, QString("Set seed point"));
		locMap.rstr = STR_SEED_POINT_SHOW;
		strList.insert(locMap.rstr, QString("Seed point show"));
		locMap.rstr = STR_SEED_POINT_HIDE;
		strList.insert(locMap.rstr, QString("Seed point hide"));
		locMap.rstr = STR_ANNO_LIST;
		strList.insert(locMap.rstr, QString("Annotation list"));

		locMap.rstr = STR_ANNO_TEXT;
		strList.insert(locMap.rstr, QString("Text"));
		locMap.rstr = STR_ANNO_LENGTH;
		strList.insert(locMap.rstr, QString("Length"));
		locMap.rstr = STR_ANNO_ANGLE;
		strList.insert(locMap.rstr, QString("Angle"));
		locMap.rstr = STR_ANNO_OVAL;
		strList.insert(locMap.rstr, QString("Oval"));
		locMap.rstr = STR_ANNO_ARROW;
		strList.insert(locMap.rstr, QString("Arrow"));
		locMap.rstr = STR_ANNO_RECTANGLE;
		strList.insert(locMap.rstr, QString("Rectangle"));
		locMap.rstr = STR_ANNO_PROF_DRAW;
		strList.insert(locMap.rstr, QString("Draw"));
		locMap.rstr = STR_ANNO_PROF_POLY;
		strList.insert(locMap.rstr, QString("Polygon"));

		locMap.rstr = STR_2D_SCREEN;
		strList.insert(locMap.rstr, QString("2D Screen"));
		locMap.rstr = STR_3D_VOLUME;
		strList.insert(locMap.rstr, QString("3D Volume"));

		locMap.rstr = STR_LAYER_HISTOGRAM;
		strList.insert(locMap.rstr, QString("View histogram"));

		locMap.rstr = STR_SHOW_ANI_POINT;
		strList.insert(locMap.rstr, QString("Show annotation path points"));
		locMap.rstr = STR_HIDE_ANI_POINT;
		strList.insert(locMap.rstr, QString("Hide annotation path points"));

		locMap.rstr = STR_VR_POINT;
		strList.insert(locMap.rstr, QString("View clicked point in VR mode"));

		locMap.rstr = STR_3D_TO_2D;
		strList.insert(locMap.rstr, QString("View clicked point in 2D"));

		locMap.rstr = STR_SVIEW;
		strList.insert(locMap.rstr, QString("Cross-section"));

		locMap.rstr = STR_GAMMA;
		strList.insert(locMap.rstr, QString("Gamma"));

		//locMap.rstr = STR_ALLOC_FAIL;
		//strList.insert(locMap.rstr, QString("Your computer is low on memory.\n"
		//	"You must perform a croppping operation."));

		locMap.rstr = STR_EXPORT_VTK;
		strList.insert(locMap.rstr, QString("Export to VTK file"));

		locMap.rstr = STR_EXPORT_NII;
		strList.insert(locMap.rstr, QString("Export to NII file"));

		locMap.rstr = STR_MEDIPWINDOW_TITLE;
		strList.insert(locMap.rstr, QString("MEDIP - Medical Imaging & Printing"));

		locMap.rstr = STR_CONNECT_HOMEPAGE;
		strList.insert(locMap.rstr, QString("Connect to MedicalIP"));

		locMap.rstr = STR_TABLIST_BTN_TOOLTIP;
		strList.insert(locMap.rstr, QString("Show/hide tab list"));

		locMap.rstr = STR_IMAGELIST_BTN_TOOLTIP;
		strList.insert(locMap.rstr, QString("Show/hide image list"));

		locMap.rstr = STR_MESSAGE_FILE_LOAD_FAILED;
		strList.insert(locMap.rstr, QString("File not loaded !!"));

		locMap.rstr = STR_MESSAGE_TITLE_CROPOPEN_FILE_FAILED;
		strList.insert(locMap.rstr, QString("Can not open"));

		locMap.rstr = STR_MESSAGE_TEXT_CROPOPEN_FILE_FAILED;
		strList.insert(locMap.rstr, QString("Only DCM files can be opened."));

		locMap.rstr = STR_CROP_WINDOW_TILE;
		strList.insert(locMap.rstr, QString("Cropping window"));

		locMap.rstr = STR_CROP_MEMORY_INFO;
		strList.insert(locMap.rstr, QString("Memory info"));

		locMap.rstr = STR_CROP_LENGTH_INFO;
		strList.insert(locMap.rstr, QString("Length info"));

		locMap.rstr = STR_CROP_DEPTH_INFO;
		strList.insert(locMap.rstr, QString("Depth info"));

		locMap.rstr = STR_CAPTURE_TAB;
		strList.insert(locMap.rstr, QString("Capture"));

		locMap.rstr = STR_IMAGE_IMPORT_TAB;
		strList.insert(locMap.rstr, QString("Import"));

		locMap.rstr = STR_BTN_TA_FEATURE_EXTRACT;
		strList.insert(locMap.rstr, QString("Extract"));

		locMap.rstr = STR_BTN_TA_FEATURE_ALL_EXTRACT;
		strList.insert(locMap.rstr, QString("Calculate"));

		// radiomics feature string
		// major
		locMap.rstr = STR_TA_MAJOR_SIZE_AND_SHAPE_BASED_FEATURES;
		strList.insert(locMap.rstr, QString("Size and shape based - features"));

		locMap.rstr = STR_TA_MAJOR_HISTOGRAM_BASED_FEATURES;
		strList.insert(locMap.rstr, QString("Histogram based-features"));

		locMap.rstr = STR_TA_MAJOR_RELATIONSHIPS_BETWEEN_IMAGE_VOXELS_BASED_FEATURES;
		strList.insert(locMap.rstr, QString("Relationships between image voxels based-features"));

		// middle
		// 1
		locMap.rstr = STR_TA_MIDDLE_SHAPE_BASED_3D;
		strList.insert(locMap.rstr, QString("Shape-based(3D)"));

		locMap.rstr = STR_TA_MIDDLE_SHAPE_BASED_2D;
		strList.insert(locMap.rstr, QString("Shape-based(2D)"));
		// 2
		locMap.rstr = STR_TA_MIDDLE_FIRST_ORDER_STATISTICS;
		strList.insert(locMap.rstr, QString("First order statistics"));
		// 3
		locMap.rstr = STR_TA_MIDDLE_GRAY_LEVEL_COOCCURRENCE_MATRIX;
		strList.insert(locMap.rstr, QString("Gray level cooccurrence matrix"));

		locMap.rstr = STR_TA_MIDDLE_GRAY_LEVEL_RUN_LENGTH_MATRIX;
		strList.insert(locMap.rstr, QString("Gray level run length matrix"));

		locMap.rstr = STR_TA_MIDDLE_GRAY_LEVEL_SIZE_ZONE_MATRIX;
		strList.insert(locMap.rstr, QString("Gray level size zone matrix"));

		locMap.rstr = STR_TA_MIDDLE_NEIGHBOURING_GRAY_TONE_DIFFERENCE_MATRIX;
		strList.insert(locMap.rstr, QString("Neighbouring gray tone difference matrix"));

		locMap.rstr = STR_TA_MIDDLE_GRAY_LEVEL_DEPENDENCE_MATRIX;
		strList.insert(locMap.rstr, QString("Gray level dependence matrix"));

		// minor
		// 1-1
		locMap.rstr = STR_TA_SB3D_MESH_VOLUME;
		strList.insert(locMap.rstr, QString("Mesh volume"));

		locMap.rstr = STR_TA_SB3D_VOXEL_VOLUME;
		strList.insert(locMap.rstr, QString("Voxel volume"));

		locMap.rstr = STR_TA_SB3D_SURFACE_AREA;
		strList.insert(locMap.rstr, QString("Surface area"));

		locMap.rstr = STR_TA_SB3D_SURFACE_AREA_TO_VOLUME_RATIO;
		strList.insert(locMap.rstr, QString("Surface area to volume ratio"));

		locMap.rstr = STR_TA_SB3D_SPHERICITY;
		strList.insert(locMap.rstr, QString("Sphericity"));

		/*locMap.rstr = STR_TA_SB3D_COMPACTNESS_1;
		strList.insert(locMap.rstr, QString("Compactness 1"));*/

		/*locMap.rstr = STR_TA_SB3D_COMPACTNESS_2;
		strList.insert(locMap.rstr, QString("Compactness 2"));*/

		/*locMap.rstr = STR_TA_SB3D_SPHERICAL_DISPROPORTION;
		strList.insert(locMap.rstr, QString("Spherical Disproportion"));*/

		locMap.rstr = STR_TA_SB3D_MAXIMUM_3D_DIAMETER;
		strList.insert(locMap.rstr, QString("Maximum 3D diameter"));

		locMap.rstr = STR_TA_SB3D_MAXIMUM_2D_DIAMETER_SLICE;
		strList.insert(locMap.rstr, QString("Maximum 2D diameter (Slice)"));

		locMap.rstr = STR_TA_SB3D_MAXIMUM_2D_DIAMETER_COLUMN;
		strList.insert(locMap.rstr, QString("Maximum 2D diameter (Column)"));

		locMap.rstr = STR_TA_SB3D_MAXIMUM_2D_DIAMETER_ROW;
		strList.insert(locMap.rstr, QString("Maximum 2D diameter (Row)"));

		locMap.rstr = STR_TA_SB3D_MAJOR_AXIS_LENGTH;
		strList.insert(locMap.rstr, QString("Major axis length"));

		locMap.rstr = STR_TA_SB3D_MINOR_AXIS_LENGTH;
		strList.insert(locMap.rstr, QString("Minor axis length"));

		locMap.rstr = STR_TA_SB3D_LEAST_AXIS_LENGTH;
		strList.insert(locMap.rstr, QString("Least axis length"));

		locMap.rstr = STR_TA_SB3D_ELONGATION;
		strList.insert(locMap.rstr, QString("Elongation"));

		locMap.rstr = STR_TA_SB3D_FLAATNESS;
		strList.insert(locMap.rstr, QString("Flatness"));

		// 1-2
		locMap.rstr = STR_TA_SB2D_MESH_SURFACE;
		strList.insert(locMap.rstr, QString("Mesh surface"));

		locMap.rstr = STR_TA_SB2D_PIXEL_SURFACE;
		strList.insert(locMap.rstr, QString("Pixel surface"));

		locMap.rstr = STR_TA_SB2D_PERIMETER;
		strList.insert(locMap.rstr, QString("Perimeter"));

		locMap.rstr = STR_TA_SB2D_PERIMETER_TO_SURFACE_RATIO;
		strList.insert(locMap.rstr, QString("Perimeter to surface ratio"));

		locMap.rstr = STR_TA_SB2D_SPHERICITY;
		strList.insert(locMap.rstr, QString("Sphericity"));

		/*locMap.rstr = STR_TA_SB2D_SPHERICAL_DISPROPORTION;
		strList.insert(locMap.rstr, QString("Spherical Disproportion"));*/

		locMap.rstr = STR_TA_SB2D_MAXIMUM_2D_DIAMETER;
		strList.insert(locMap.rstr, QString("Maximum 2D diameter"));

		locMap.rstr = STR_TA_SB2D_MAJOR_AXIS_LENGTH;
		strList.insert(locMap.rstr, QString("Major axis length"));

		locMap.rstr = STR_TA_SB2D_MINOR_AXIS_LENGTH;
		strList.insert(locMap.rstr, QString("Minor axis length"));

		locMap.rstr = STR_TA_SB2D_ELONGATION;
		strList.insert(locMap.rstr, QString("Elongation"));

		// 2-1
		locMap.rstr = STR_TA_FOS_ENERGY;
		strList.insert(locMap.rstr, QString("Energy"));

		locMap.rstr = STR_TA_FOS_TOTAL_ENERGY;
		strList.insert(locMap.rstr, QString("Total energy"));

		locMap.rstr = STR_TA_FOS_ENTROPY;
		strList.insert(locMap.rstr, QString("Entropy"));

		locMap.rstr = STR_TA_FOS_MINIMUM;
		strList.insert(locMap.rstr, QString("Minimum"));

		locMap.rstr = STR_TA_FOS_10TH_PERCENTILE;
		strList.insert(locMap.rstr, QString("10th percentile"));

		locMap.rstr = STR_TA_FOS_90TH_PERCENTILE;
		strList.insert(locMap.rstr, QString("90th percentile"));

		locMap.rstr = STR_TA_FOS_MAXIMUM;
		strList.insert(locMap.rstr, QString("Maximum"));

		locMap.rstr = STR_TA_FOS_MEAN;
		strList.insert(locMap.rstr, QString("Mean"));

		locMap.rstr = STR_TA_FOS_MEDIAN;
		strList.insert(locMap.rstr, QString("Median"));

		locMap.rstr = STR_TA_FOS_INTERQUARTILE_RANGE;
		strList.insert(locMap.rstr, QString("Interquartile range"));

		locMap.rstr = STR_TA_FOS_RANGE;
		strList.insert(locMap.rstr, QString("Range"));

		locMap.rstr = STR_TA_FOS_MEAN_ABSOLUTE_DEVIATION_MAD;
		strList.insert(locMap.rstr, QString("Mean absolute deviation (MAD)"));

		locMap.rstr = STR_TA_FOS_ROBUST_MEAN_ABSOLUTE_DEVIATION_rMAD;
		strList.insert(locMap.rstr, QString("Robust mean absolute deviation (rMAD)"));

		locMap.rstr = STR_TA_FOS_ROOT_MEAN_SQUARED_RMS;
		strList.insert(locMap.rstr, QString("Root mean squared (RMS)"));

		locMap.rstr = STR_TA_FOS_STANDARD_DEVIATION;
		strList.insert(locMap.rstr, QString("Standard deviation"));

		locMap.rstr = STR_TA_FOS_SKEWNESS;
		strList.insert(locMap.rstr, QString("Skewness"));

		locMap.rstr = STR_TA_FOS_KURTOSIS;
		strList.insert(locMap.rstr, QString("Kurtosis"));

		locMap.rstr = STR_TA_FOS_VARIANCE;
		strList.insert(locMap.rstr, QString("Variance"));

		locMap.rstr = STR_TA_FOS_UNIFORMITY;
		strList.insert(locMap.rstr, QString("Uniformity"));

		// 3-1
		locMap.rstr = STR_TA_GLCM_AUTOCORRELATION;
		strList.insert(locMap.rstr, QString("Autocorrelation"));

		locMap.rstr = STR_TA_GLCM_JOINT_AVERAGE;
		strList.insert(locMap.rstr, QString("Joint average"));

		locMap.rstr = STR_TA_GLCM_CLUSTER_PROMINENCE;
		strList.insert(locMap.rstr, QString("Cluster prominence"));

		locMap.rstr = STR_TA_GLCM_CLUSTER_SHADE;
		strList.insert(locMap.rstr, QString("Cluster shade"));

		locMap.rstr = STR_TA_GLCM_CLUSTER_TENDENCY;
		strList.insert(locMap.rstr, QString("Cluster tendency"));

		locMap.rstr = STR_TA_GLCM_CONTRAST;
		strList.insert(locMap.rstr, QString("Contrast"));

		locMap.rstr = STR_TA_GLCM_CORRELATION;
		strList.insert(locMap.rstr, QString("Correlation"));

		locMap.rstr = STR_TA_GLCM_DIFFERENCE_AVERAGE;
		strList.insert(locMap.rstr, QString("Difference average"));

		locMap.rstr = STR_TA_GLCM_DIFFERENCE_ENTROPY;
		strList.insert(locMap.rstr, QString("Difference entropy"));

		locMap.rstr = STR_TA_GLCM_DIFFERENCE_VARIANCE;
		strList.insert(locMap.rstr, QString("Difference variance"));

		locMap.rstr = STR_TA_GLCM_JOINT_ENERGY;
		strList.insert(locMap.rstr, QString("Joint energy"));

		locMap.rstr = STR_TA_GLCM_JOINT_ENTROPY;
		strList.insert(locMap.rstr, QString("Joint entropy"));

		locMap.rstr = STR_TA_GLCM_INFORMATIONAL_MEASURE_OF_CORRELATION_IMC_1;
		strList.insert(locMap.rstr, QString("Informational measure of correlation (IMC) 1"));

		locMap.rstr = STR_TA_GLCM_INFORMATIONAL_MEASURE_OF_CORRELATION_IMC_2;
		strList.insert(locMap.rstr, QString("Informational measure of correlation (IMC) 2"));

		locMap.rstr = STR_TA_GLCM_INVERSE_DIFFERENCE_MOMENT_IDM;
		strList.insert(locMap.rstr, QString("Inverse difference moment (IDM)"));

		locMap.rstr = STR_TA_GLCM_MAXIMAL_CORRELATION_COEFFICIENT_MCC;
		strList.insert(locMap.rstr, QString("Maximal dorrelation coefficient (MCC)"));

		locMap.rstr = STR_TA_GLCM_INVERSE_DIFFERENCE_MOMENT_NORMALIZED_IDMN;
		strList.insert(locMap.rstr, QString("Inverse difference moment normalized (IDMN)"));

		locMap.rstr = STR_TA_GLCM_INVERSE_DIFFERENCE_ID;
		strList.insert(locMap.rstr, QString("Inverse difference (ID)"));

		locMap.rstr = STR_TA_GLCM_INVERSE_DIFFERENCE_NORMALIZED_IDN;
		strList.insert(locMap.rstr, QString("Inverse difference normalized (IDN)"));

		locMap.rstr = STR_TA_GLCM_INVERSE_VARIANCE;
		strList.insert(locMap.rstr, QString("Inverse variance"));

		locMap.rstr = STR_TA_GLCM_MAXIMUM_PROBABILITY;
		strList.insert(locMap.rstr, QString("Maximum probability"));

		locMap.rstr = STR_TA_GLCM_SUM_AVERAGE;
		strList.insert(locMap.rstr, QString("Sum average"));

		locMap.rstr = STR_TA_GLCM_SUM_ENTROPY;
		strList.insert(locMap.rstr, QString("Sum entropy"));

		locMap.rstr = STR_TA_GLCM_SUM_OF_SQUARES;
		strList.insert(locMap.rstr, QString("Sum of squares"));

		// 3-2
		locMap.rstr = STR_TA_GLSZM_SMALL_AREA_EMPHASIS_SAE;
		strList.insert(locMap.rstr, QString("Small area emphasis (SAE)"));

		locMap.rstr = STR_TA_GLSZM_LARGE_AREA_EMPHASIS_LAE;
		strList.insert(locMap.rstr, QString("Large area emphasis (LAE)"));

		locMap.rstr = STR_TA_GLSZM_GRAY_LEVEL_NON_UNIFORMITY_GLN;
		strList.insert(locMap.rstr, QString("Gray level non-uniformity (GLN)"));

		locMap.rstr = STR_TA_GLSZM_GRAY_LEVEL_NON_UNIFORMITY_NORMALIZED_GLNN;
		strList.insert(locMap.rstr, QString("Gray level non-uniformity normalized (GLNN)"));

		locMap.rstr = STR_TA_GLSZM_SIZE_ZONE_NON_UNIFORMITY__SZN;
		strList.insert(locMap.rstr, QString("Size-zone non-uniformity (SZN)"));

		locMap.rstr = STR_TA_GLSZM_SIZE_ZONE_NON_UNIFORMITY_NORMALIZED_SZNN;
		strList.insert(locMap.rstr, QString("Size-zone non-uniformity normalized (SZNN)"));

		locMap.rstr = STR_TA_GLSZM_ZONE_PERCENTAGE_ZP;
		strList.insert(locMap.rstr, QString("Zone percentage (ZP)"));

		locMap.rstr = STR_TA_GLSZM_GRAY_LEVEL_VARIANCE_GLV;
		strList.insert(locMap.rstr, QString("Gray level variance (GLV)"));

		locMap.rstr = STR_TA_GLSZM_ZONE_VARIANCE_ZV;
		strList.insert(locMap.rstr, QString("Zone variance (ZV)"));

		locMap.rstr = STR_TA_GLSZM_ZONE_ENTROPY_ZE;
		strList.insert(locMap.rstr, QString("Zone entropy (ZE)"));

		locMap.rstr = STR_TA_GLSZM_LOW_GRAY_LEVEL_ZONE_EMPHASIS_LGLZE;
		strList.insert(locMap.rstr, QString("Low gray level zone emphasis (LGLZE)"));

		locMap.rstr = STR_TA_GLSZM_HIGH_GRAY_LEVEL_ZONE_EMPHASIS_HGLZE;
		strList.insert(locMap.rstr, QString("High gray level zone emphasis (HGLZE)"));

		locMap.rstr = STR_TA_GLSZM_SMALL_AREA_LOW_GRAY_LEVEL_EMPHASIS_SALGLE;
		strList.insert(locMap.rstr, QString("Small area low gray level emphasis (SALGLE)"));

		locMap.rstr = STR_TA_GLSZM_SMALL_AREA_HIGH_GRAY_LEVEL_EMPHASIS_SAHGLE;
		strList.insert(locMap.rstr, QString("Small area high gray level emphasis (SAHGLE)"));

		locMap.rstr = STR_TA_GLSZM_LARGE_AREA_LOW_GRAY_LEVEL_EMPHASIS_LALGLE;
		strList.insert(locMap.rstr, QString("Large area low gray level emphasis (LALGLE)"));

		locMap.rstr = STR_TA_GLSZM_LARGE_AREA_HIGH_GRAY_LEVEL_EMPHASIS_LAHGLE;
		strList.insert(locMap.rstr, QString("Large area high gray level emphasis (LAHGLE)"));

		// 3-3, GLRLM
		locMap.rstr = STR_TA_GLRLM_SHORT_RUN_EMPHASIS_SRE;
		strList.insert(locMap.rstr, QString("Short run emphasis (SRE)"));

		locMap.rstr = STR_TA_GLRLM_LONG_RUN_EMPHASIS_LRE;
		strList.insert(locMap.rstr, QString("Long run emphasis (LRE)"));

		locMap.rstr = STR_TA_GLRLM_GRAY_LEVEL_NON_UNIFORMITY_GLN;
		strList.insert(locMap.rstr, QString("Gray level non-uniformity (GLN)"));

		locMap.rstr = STR_TA_GLRLM_GRAY_LEVEL_NON_UNIFORMITY_NORMALIZED_GLNN;
		strList.insert(locMap.rstr, QString("Gray level non-uniformity normalized (GLNN)"));

		locMap.rstr = STR_TA_GLRLM_RUN_LENGTH_NON_UNIFORMITY_RLN;
		strList.insert(locMap.rstr, QString("Run length non-uniformity (RLN)"));

		locMap.rstr = STR_TA_GLRLM_RUN_LENGTH_NON_UNIFORMITY_NORMALIZED_RLNN;
		strList.insert(locMap.rstr, QString("Run length non-uniformity normalized (RLNN)"));

		locMap.rstr = STR_TA_GLRLM_RUN_PERCENTAGE_RP;
		strList.insert(locMap.rstr, QString("Run percentage (RP)"));

		locMap.rstr = STR_TA_GLRLM_GRAY_LEVEL_VARIANCE_GLV;
		strList.insert(locMap.rstr, QString("Gray level variance (GLV)"));

		locMap.rstr = STR_TA_GLRLM_RUN_VARIANCE_RV;
		strList.insert(locMap.rstr, QString("Run variance (RV)"));

		locMap.rstr = STR_TA_GLRLM_RUN_ENTROPY_RE;
		strList.insert(locMap.rstr, QString("Run entropy (RE)"));

		locMap.rstr = STR_TA_GLRLM_LOW_GRAY_LEVEL_RUN_EMPHASIS_LGLRE;
		strList.insert(locMap.rstr, QString("Low gray level run emphasis (LGLRE)"));

		locMap.rstr = STR_TA_GLRLM_HIGH_GRAY_LEVEL_RUN_EMPHASIS_HGLRE;
		strList.insert(locMap.rstr, QString("High gray level run emphasis (HGLRE)"));

		locMap.rstr = STR_TA_GLRLM_SHORT_RUN_LOW_GRAY_LEVEL_EMPHASIS_SRLGLE;
		strList.insert(locMap.rstr, QString("Short run low gray level emphasis (SRLGLE)"));

		locMap.rstr = STR_TA_GLRLM_SHORT_RUN_HIGH_GRAY_LEVEL_EMPHASIS_SRHGLE;
		strList.insert(locMap.rstr, QString("Short run high gray level emphasis (SRHGLE)"));

		locMap.rstr = STR_TA_GLRLM_LONG_RUN_LOW_GRAY_LEVEL_EMPHASIS_LRLGLE;
		strList.insert(locMap.rstr, QString("Long run low gray level emphasis (LRLGLE)"));

		locMap.rstr = STR_TA_GLRLM_LONG_RUN_HIGH_GRAY_LEVEL_EMPHASIS_LRHGLE;
		strList.insert(locMap.rstr, QString("Long run high gray level emphasis (LRHGLE)"));

		// 3-4
		locMap.rstr = STR_TA_NGTDM_COARSENESS;
		strList.insert(locMap.rstr, QString("Coarseness"));

		locMap.rstr = STR_TA_NGTDM_CONTRAST;
		strList.insert(locMap.rstr, QString("Contrast"));

		locMap.rstr = STR_TA_NGTDM_BUSYNESS;
		strList.insert(locMap.rstr, QString("Busyness"));

		locMap.rstr = STR_TA_NGTDM_COMPLEXITY;
		strList.insert(locMap.rstr, QString("Complexity"));

		locMap.rstr = STR_TA_NGTDM_STRENGTH;
		strList.insert(locMap.rstr, QString("Strength"));

		// 3-5
		locMap.rstr = STR_TA_GLDM_SMALL_DEPENDENCE_EMPHASIS_SDE;
		strList.insert(locMap.rstr, QString("Small dependence emphasis (SDE)"));

		locMap.rstr = STR_TA_GLDM_LARGE_DEPENDENCE_EMPHASIS_LDE;
		strList.insert(locMap.rstr, QString("Large dependence emphasis (LDE)"));

		locMap.rstr = STR_TA_GLDM_GRAY_LEVEL_NON_UNIFORMITY_GLN;
		strList.insert(locMap.rstr, QString("Gray level non-uniformity (GLN)"));

		locMap.rstr = STR_TA_GLDM_DEPENDENCE_NON_UNIFORMITY_DN;
		strList.insert(locMap.rstr, QString("Dependence non-uniformity (DN)"));

		locMap.rstr = STR_TA_GLDM_DEPENDENCE_NON_UNIFORMITY_NORMALIZED_DNN;
		strList.insert(locMap.rstr, QString("Dependence non-uniformity normalized (DNN)"));

		locMap.rstr = STR_TA_GLDM_GRAY_LEVEL_VARIANCE_GLV;
		strList.insert(locMap.rstr, QString("Gray level variance (GLV)"));

		locMap.rstr = STR_TA_GLDM_DEPENDENCE_VARIANCE_DV;
		strList.insert(locMap.rstr, QString("Dependence variance (DV)"));

		locMap.rstr = STR_TA_GLDM_DEPENDENCE_ENTROPY_DE;
		strList.insert(locMap.rstr, QString("Dependence entropy (DE)"));

		locMap.rstr = STR_TA_GLDM_LOW_GRAY_LEVEL_EMPHASIS_LGLE;
		strList.insert(locMap.rstr, QString("Low gray level emphasis (LGLE)"));

		locMap.rstr = STR_TA_GLDM_HIGH_GRAY_LEVEL_EMPHASIS_HGLE;
		strList.insert(locMap.rstr, QString("High gray level emphasis (HGLE)"));

		locMap.rstr = STR_TA_GLDM_SMALL_DEPENDENCE_LOW_GRAY_LEVEL_EMPHASIS_SDLGLE;
		strList.insert(locMap.rstr, QString("Small dependence low gray level emphasis (SDLGLE)"));

		locMap.rstr = STR_TA_GLDM_SMALL_DEPENDENCE_HIGH_GRAY_LEVEL_EMPHASIS_SDHGLE;
		strList.insert(locMap.rstr, QString("Small dependence high gray level emphasis (SDHGLE)"));

		locMap.rstr = STR_TA_GLDM_LARGE_DEPENDENCE_LOW_GRAY_LEVEL_EMPHASIS_LDLGLE;
		strList.insert(locMap.rstr, QString("Large dependence low gray level emphasis (LDLGLE)"));

		locMap.rstr = STR_TA_GLDM_LARGE_DEPENDENCE_HIGH_GRAY_LEVEL_EMPHASIS_SDHGLE;
		strList.insert(locMap.rstr, QString("Large dependence high gray level emphasis (LDHGLE)"));

		locMap.rstr = STR_LOGIN_ID;
		strList.insert(locMap.rstr, QString("ID"));
		locMap.rstr = STR_LOGIN_PWD;
		strList.insert(locMap.rstr, QString("Password"));
		locMap.rstr = STR_LOGIN;
		strList.insert(locMap.rstr, QString("Login"));
		locMap.rstr = STR_LOGIN_IDSAVE;
		strList.insert(locMap.rstr, QString("ID save"));
		locMap.rstr = STR_LOGIN_AUTO;
		strList.insert(locMap.rstr, QString("Auto login"));

		// For Visual Printing by Lim Young Il
		locMap.rstr = STR_VISUAL_PRINT;
		strList.insert(locMap.rstr, QString("Visual print"));		

		locMap.rstr = STR_VISUAL_PRINT_WEB_LINK;
//		strList.insert(locMap.rstr, QString("Web Link"));
		strList.insert(locMap.rstr, QString("3D web view"));

		locMap.rstr = STR_VISUAL_PRINT_DIALOG;
		strList.insert(locMap.rstr, QString("Visual print dialog"));

		locMap.rstr = STR_SELECT_SLICE_DIRECTIVE_SINGLE;
		strList.insert(locMap.rstr, QString("Select a single slice"));

		locMap.rstr = STR_SELECT_SLICE_DIRECTIVE_MULTI;
		strList.insert(locMap.rstr, QString("Select a multi slice"));

		locMap.rstr = STR_NONMANIFOLD_HEDGE_CASE;
		strList.insert(locMap.rstr, QString("This case is non-manifold half-edge case!"));
	
		// DU-1
		locMap.rstr = ERR_DU_1001;
		strList.insert(locMap.rstr, QString("DeepCatch prediction must be preceded before ""Make report"".") + QString("\n") + QString("[DU-1001]"));

		locMap.rstr = ERR_DU_1002;
		strList.insert(locMap.rstr, QString("There are not enough layers to create. Please delete 9 layers.") + QString("\n") + QString("[DU-1002]"));

		locMap.rstr = ERR_DU_1003;
		strList.insert(locMap.rstr, QString("Failed to predict. Try again.") + QString("\n") + QString("[DU-1003]"));

		locMap.rstr = ERR_DU_1004;
		strList.insert(locMap.rstr, QString("Failed to create CSV file.") + QString("\n") + QString("[DU-1004]"));

		locMap.rstr = ERR_DU_1005;
		strList.insert(locMap.rstr, QString("Please select annotations to delete.") + QString("\n") + QString("[DU-1005]"));

		locMap.rstr = ERR_DU_1006;
		strList.insert(locMap.rstr, QString("Please select a single mask to duplicate.") + QString("\n") + QString("[DU-1006]"));

		locMap.rstr = ERR_DU_1007;
		strList.insert(locMap.rstr, QString("Please select a single mask.") + QString("\n") + QString("[DU-1007]"));

		locMap.rstr = ERR_DU_1008;
		strList.insert(locMap.rstr, QString("Your segmentation results will be deleted. Would you like to continue?") + QString("\n") + QString("[DU-1008]"));

		locMap.rstr = ERR_DU_1009;		
		strList.insert(locMap.rstr, QString("There is no usable credit.\nPlease email to (<a href = 'mailto:contact@medicalip.com'>contact@medicalip.com</a>)") + QString("\n") + QString("[DU-1009]"));

		locMap.rstr = ERR_DU_1010;
		strList.insert(locMap.rstr, QString("This program is already running. ") + QString("\n") + QString("[DU-1010]"));

		locMap.rstr = ERR_DU_1011;
		strList.insert(locMap.rstr, QString("Please wait. We're updating.") + QString("\n") + QString("[DU-1011]"));

		locMap.rstr = ERR_DU_1012;
		strList.insert(locMap.rstr, QString("The file 'file name' doesn't exist at this location.") + QString("\n") + QString("[DU-1012]"));

		locMap.rstr = ERR_DU_1013;
		strList.insert(locMap.rstr, QString("Failed to make report. Please save or close ""exertReport.csv"" file.") + QString("\n") + QString("[DU-1013]"));

		locMap.rstr = ERR_DU_1014;
		strList.insert(locMap.rstr, QString("Failed to read DCM data. Try again.") + QString("\n") + QString("[DU-1014]"));

		locMap.rstr = ERR_DU_1015;
		strList.insert(locMap.rstr, QString("The network is not connected.") + QString("\n") + QString("[DU-1015]"));

		locMap.rstr = ERR_DU_1016_STR_ALREADY_EXIST_THREAD;
		strList.insert(locMap.rstr, QString("The working thread exists. Please wait.") + QString("\n") + QString("[DU-1016]"));

		locMap.rstr = ERR_DU_1017;
		strList.insert(locMap.rstr, QString("Program initialization failed. Run the program again.") + QString("\n") + QString("[DU-1017]"));

		locMap.rstr = ERR_DU_1018;
		strList.insert(locMap.rstr, QString("System time is invalid. Try again after synchronizing current time.") + QString("\n") + QString("[DU-1018]"));

		locMap.rstr = ERR_DU_1019;
		strList.insert(locMap.rstr, QString("The demo or paid version has expired.\nPlease email to <a href = 'mailto:contact@medicalip.com'>contact@medicalip.com</a> for purchase.") + QString("\n") + QString("[DU-1019]"));

		// DU-0
		locMap.rstr = ERR_DU_0001;
		strList.insert(locMap.rstr, QString("Invalid ID. Try again.") + QString("\n") + QString("[DU-0001]"));

		locMap.rstr = ERR_DU_0002;
		strList.insert(locMap.rstr, QString("Failed to read the series of information.") + QString("\n") + QString("[DU-0002]"));

		locMap.rstr = ERR_DU_0003;
		strList.insert(locMap.rstr, QString("Failed to load volume data.") + QString("\n") + QString("[DU-0003]"));

		locMap.rstr = ERR_DU_0004;
		strList.insert(locMap.rstr, QString("It is not a standard dicom file format.") + QString("\n") + QString("[DU-0004]"));

		locMap.rstr = ERR_DU_0005;
		strList.insert(locMap.rstr, QString("Macro setting file error.Check row % 1, column % 2") + QString("\n") + QString("[DU-0005]"));

		locMap.rstr = ERR_DU_0006;
		strList.insert(locMap.rstr, QString("%1 service is not allowed to you. please subscribe first.") + QString("\n") + QString("[DU-0006]"));

		locMap.rstr = ERR_DU_0007;
		strList.insert(locMap.rstr, QString("Invalid Password. Try again.") + QString("\n") + QString("[DU-0007]"));

		// DE-0
		locMap.rstr = ERR_DE_0001;
		strList.insert(locMap.rstr, QString("Failed to update. Please check network connection or firewall settings.") + QString("\n") + QString("[DE-0001]"));

		locMap.rstr = ERR_DE_0002;
		strList.insert(locMap.rstr, QString("Failed to update. Please check network connection or firewall settings.") + QString("\n") + QString("[DE-0002]"));

		locMap.rstr = ERR_DE_0003;
		strList.insert(locMap.rstr, QString("Failed to update. Please check network connection or firewall settings.") + QString("\n") + QString("[DE-0003]"));

		locMap.rstr = ERR_DE_0004;
		strList.insert(locMap.rstr, QString("Failed to update. Please check network connection or firewall settings.") + QString("\n") + QString("[DE-0004]"));

		locMap.rstr = ERR_DE_0005;
		strList.insert(locMap.rstr, QString("Failed to update. Please check network connection or firewall settings.") + QString("\n") + QString("[DE-0005]"));

		locMap.rstr = ERR_DE_0006;
		strList.insert(locMap.rstr, QString("Failed to update. Please check network connection or firewall settings.") + QString("\n") + QString("[DE-0006]"));

		locMap.rstr = ERR_DE_0007;
		strList.insert(locMap.rstr, QString("Network error occurred. Please check network connection and try again.") + QString("\n") + QString("[DE-0007]"));

		locMap.rstr = ERR_DE_0008;
		strList.insert(locMap.rstr, 
			QString("Another user(IP Address=%1) logged in with the same ID and the program will be logged out. After that, we will guide you to the steps of saving your work.") + 
			QString("\n") + 
			QString("[DE-0008]"));

		locMap.rstr = ERR_DE_0009;
		strList.insert(locMap.rstr,
			QString("The program will be logged out due to connection failure to the server. there could be an unstable network connection or an internet service problem. After that, we will guide you to the steps of saving your work.") +
			QString("\n") +
			QString("[DE-0009]"));

		// DH-0
		locMap.rstr = ERR_DH_0001;
		strList.insert(locMap.rstr, QString("OS - Microsoft Windows 7(64bit) or higher, \n"
			"CPU - Intel i5 or higher, RAM - 8GB or higher,\n"
			"GPU - NVIDIA Geforce 1000series or higher(with 3GB(2D UNET) or 8GB(3D UNET) GPU memory or higher)\n"
			"If not, operating the network takes more than one hour per case.") + QString("\n") + QString("[DH-0001]"));

		locMap.rstr = ERR_DH_0002;
		strList.insert(locMap.rstr, QString("Your computer is low on GPU memory.\n"
			"GPU - NVIDIA Geforce 1000series or higher(with 3GB(2D UNET) or 8GB(3D UNET) GPU memory or higher)") + QString("\n") + QString("[DH-0002]"));

		locMap.rstr = ERR_DH_0003;
		strList.insert(locMap.rstr, QString("Your computer is low on memory.\n You must perform a croppping operation.") + QString("\n") + QString("[DH-0003]"));

		locMap.rstr = ERR_DH_0004;
		strList.insert(locMap.rstr, QString("Failed to create Cropping window.") + QString("\n") + QString("[DH-0004]"));

		locMap.rstr = ERR_DH_0005;
		strList.insert(locMap.rstr, QString("Out of memory. Please close other programs and try again. ") + QString("\n") + QString("[DH-0005]"));

		locMap.rstr = ERR_DH_0006;
		strList.insert(locMap.rstr, QString("GPU processor not detected. Please ensure the NVIDA graphics card\n and the latest drivers(<a href='https://www.nvidia.co.kr/Download/index.aspx?lang=en\'>Link</a>) are installed.") + QString("\n") + QString("[DH-0006]"));

		locMap.rstr = ERR_DH_0007;
		strList.insert(locMap.rstr, QString("Volume Rendering requires GPU Graphics card.") + QString("\n") + QString("[DH-0007]"));

		locMap.rstr = ERR_DH_0008;
		strList.insert(locMap.rstr, QString("OpenGL initializing error. Please update  your graphics card driver.") + QString("\n") + QString("[DH-0008]"));
		
		// DR-0
		locMap.rstr = ERR_DR_0001;
		strList.insert(locMap.rstr, QString("Histogram file doesn't exist. Please re-install DeepCatch.") + QString("\n") + QString("[DR-0001]"));

		locMap.rstr = ERR_DR_0002;
		strList.insert(locMap.rstr, QString("""medipVersionInfo.dat"" file doesn't exist. Please re-install DeepCatch.") + QString("\n") + QString("[DR-0002]"));

		locMap.rstr = ERR_DR_0003;
		strList.insert(locMap.rstr, QString("Resource file doesn't exist. Please re-install DeepCatch.") + QString("\n") + QString("[DR-0003]"));

		locMap.rstr = ERR_DR_0004;
		strList.insert(locMap.rstr, QString("Report template doesn't exist. Please re-install DeepCatch.") + QString("\n") + QString("[DR-0004]"));

		locMap.rstr = STR_UID_SECONDARY_CAPTUREIMAGE_STORAGE;
		strList.insert(locMap.rstr, QString("1.2.840.10008.5.1.4.1.1.7"));
		break;
	}
	default:
		return false;
	}
	
	return true;
}

QString StringManager::getString(RSTRING str_type, LOCALE loc)
{
	locMap.loc = loc;

	if (!init()) 
		return QString("");
	
	QMap<RSTRING, QString>::iterator result;

	locMap.rstr = str_type;
	result = strList.find(locMap.rstr);

	if (result != strList.end())
		return result.value();

	return QString("");
}

QString StringManager::getStringFormat(RSTRING str_type, QStringList args, LOCALE loc)
{
	QString stringWithFormat = getString(str_type, loc);

	for (QString arg : args)
	{
		stringWithFormat = stringWithFormat.arg(arg);
	}

	return stringWithFormat;
}

QString StringManager::GetSessionName() const
{
	return m_sessionName;
}

StringManager * StringManager::getSingleton()
{
	static StringManager instance;

	return &instance;
}

