#pragma once


#include "graphics/color.h"
#include "graphics/BoundingBox.h"

#include "Math/Math.h"
#include "math/Quaternion.h"
#include "math/Vector.h"
#include "Math/MipMath.h"

//#include <QVector3D>

namespace MIP_ENCODER
{
	//#define NEXT_VER_MIP_FILE_VERSION 1000000004
#define TEXT_LENGTH_MAX 50

	enum ERROR_MESSAGE
	{
		NON_ERROR = 0,
		FAILED_FILE_OPEN = -1,
		WRONG_PROJECT_TYPE = -2,
		SAVE_ERROR = -3,
		READ_ERROR = -4,
		HIGH_VERSION = -5,
		DIFF_PROJECT_TYPE = -6,	
		DIFF_FILE_SIZE = -7,

	};

	enum PROJ_TYPE
	{
		PT_MIP,		// MEDIP 파일
		PT_MIPD,	// DeepCatch 파일
		PT_MIPA,		// MEDIP AI 파일
	};

	enum PROJ_CHUNK_WINDOW_INFO
	{
		PCWI_CORONAL = 0,
		PCWI_AXIAL,
		PCWI_SAGGITAL,
	};
	
	////////////////////////////////////////////////////////////////////////////////////
	//PADDING 필요 없는 STRUCT (파일 WRITE/READ에 STRUCT 자체가 쓰이지 않음. 추가 X)//////
	////각 STRUCT의 멤버 하나씩 READ/WRITE됨////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	struct AnimationInfo
	{
		mint32 nPoint; /*animation point count*/
		//float *points; /*animation point x,y,z*/
		std::vector<float> vecPoints;
		mint32 reserve[10];
	};

	struct MeshTextureInfo
	{
		int width;
		int height;
		int nrChannels;
		int tileStyle_1;
		int tileStyle_2;
		int dataSize;
		std::vector<uchar> data;
	};
	
	struct SurfaceInfo
	{
		bool	InfoType;	//connected : 0, not connected : 1(true)
		mint32	infoIndex;	//NOT CONNECTED : -1, CONNECTED : LAYER UID		
		std::wstring wstrName;
		bool	selected;
		bool	isHidden;
		COLOR	color;
		muint8	NameLength;
		bool	upScale;		

		mint32	nPoint;		// vertex
		muint32	TrisCount;	//
		muint32 NorCount;	//
		muint32	nUVCount;		// vertex
		muint32	textureIDCount;	//
		muint32 textureInfoCount;	//

		std::vector<mip::VECTOR3> verts;
		std::vector<muint32> tris;
		std::vector<mip::VECTOR3> normals;

		//texture
		std::vector<mip::VECTOR2> uv;
		std::vector<float> textureID;
		std::vector<MeshTextureInfo> textureInfo;

		mip::MATRIX44 worldMat;

		bool	subInfo;		// mip210 부터 안씀
		muint8	UID;		//subInfo UID, mip210 부터 안씀, infoIndex로 대체함
		mint8	reserve[22];
	};

	struct SurfaceSubInfo
	{
		muint8		UID;		//0~63 (cnt : 64)
		wchar_t		*MeshName;	//according to surfaceinfo.namelength (max : 255)
	};

	struct ImageInfo
	{
		mint32 width;
		mint32 height;
		mint32 size;
		mint32 nameLength;
		std::string name;
		std::vector<uchar> data;
	};

	struct CaptureImgInfo
	{
		int width;
		int height;
		int size;
		std::vector<uchar> data;
	};

	struct AI_ResultInfo //20210729_byPHS_ADD_AI_Result
	{
		mint32						nUse_AIInfo;
		muint32						uid;
		mint32						outSetVal;
		muint32						AI_result_Size;
		std::vector<uchar>			AI_result;
	};


#pragma pack(push, 1)
	struct ProjectHead //36byte -> 36byte (4byte 기준)
	{
		mint32 version;
		mint32 filetype;
		mint32 detailSize;
		mint32 fileSize;
		mint32 reserve[5];
	};

	//padding -> 최대 변수형 크기 기준.
	//[X64] ===> DATA1 [+PADDING] [+DATA2] ==> (8 * N) BYTE 

	struct WindowInfo //52byte -> 52byte (4byte 기준)
	{
		mint32 windowType;
		mint32 posX;
		mint32 posY;
		float zoomFactorX;
		float zoomFactorY;
		float factorX;
		float factorY;
		float depth;
		mint32 reserve[5];
	};

	struct AnnoTextInfo//136byte ->136byte (4byte 기준)
	{
		float posX;
		float posY;
		float posZ;
		WCHAR text[TEXT_LENGTH_MAX];//100BYTE

		mint16 size;
		mint16 type;
		muint32 color;

		muint8  isHidden;
		mint8  reserve[15];
	};

	typedef struct AnnoLengthInfo //44byte -> 44byte(4byte 기준)
	{
		float posX1;
		float posY1;
		float posZ1;
		float posX2;
		float posY2;
		float posZ2;

		muint32 color;
		muint8  isHidden;
		mint8  reserve_1;
		mint8  reserve_2;
		mint8  reserve_3;
		mint32 reserve[3];
	}AnnoOvalInfo;

	typedef struct AnnoLengthInfo AnnoArrowInfo;

	struct AnnoAngleInfo//56byte -> 56byte(4byte 기준)
	{
		float posX1;
		float posY1;
		float posZ1;
		float posX2;
		float posY2;
		float posZ2;
		float posX3;
		float posY3;
		float posZ3;

		muint32 color;
		muint8  isHidden;
		mint8  reserve_1;
		mint8  reserve_2;
		mint8  reserve_3;
		mint32 reserve[3];
	};

	//////////////////////
	struct ProjectMaskInfo//90byte -> 92byte(4byte 기준)
	{
		muint32	uid; // 0 ~ 6
		COLOR	color;
		mask	mask_id; // MASK2 ~ MASK7
		bool	show;
		WCHAR	maskName[MASKINFO_TEXT_LENGTH_MAX];
		bool	meshConnected;	//surfaceshow -> deprecated  ==> surface connected
		mask	mask_id2;
		bool	layerCustom;
		muint8	layerAlpha;
		mint8	reserve_char;
		bool	reserve_bool[3];
		short	padding1; //with reserve_bool array [can use as reserve]
		int		reserve[8];
	};

	struct ReportInfo//8byte -> 8byte(4byte 기준)
	{
		mint32  imageCount;
		mint32	htmlSize;
		//	ImageInfo* imageInfo; //DETAIL의 PADDING으로 추가 + 4byte
	};

	struct PredictOpt
	{
		// DeepCatch predict 완료 후에만 manual drawing 허용 flag
		bool bIsPredictComplete = false;

		mint8 nProjectType = -1;		// 0:Abdomen, 1:Chesh ........
		mint8 nUNET = -1;				// 0:2D,		1:3D
		mint8 nContrast = -1;			// 0:Non,		1:Con
		mint8 nAWPredict = -1;			// 0:coronal 1:sagittal

		// report
		mint8 nGender = -1;				// 0:Female	1:Male
		mint32 nHeight = 0;
		mint32 nWeight = 0;
		mint8 nIsL3Auto = -1;			// 0:Auto,	1:User Define
		mint8 nIsAWAuto = -1;			// 0:Auto,	1:User Define
		mint8 nReportSliceType = -1;	// 0:L3,		1:AW

		mint32 nL3SliceNum = -1;
		mint32 nAWSliceStartNum = -1;
		mint32 nAWSliceEndNum = -1;
		
		mint16 nPredictYear = 0;
		mint16 nPredictMonth = 0;
		mint16 nPredictDay = 0;

		// 2021.09.16 추가
		mint8 nAdditionalOptions = -1;	// 0: None, 1: Only Trunk 2: QCT
		mint8 nIOClassification = -1;	// 0: false, 1: true
		mint8 nMuscleQualityMap = -1;	// 0: false, 1: true
	};

	struct DicomElementInfo
	{
		std::string StudyInstanceUID;
		std::string SeriesInstanceUID;
		std::string ExamID;
		std::string AccessionNumber;
		std::string StudyDate;
		std::string StudyTime;
		std::string PatientID;
		std::string PatientName;
		std::string PatientSex;
		std::string PatientAge;
		std::string PatientWeight;
		std::string AcquisitionNumber;
		std::string Units;
		std::string RadionuclideTotalDose;
		std::string RadionuclideHalfLife;
		std::string RadiopharmaceuticalStartTime;
		std::string AcquisitionTime;
		std::string FrameOfReferenceUID;
		std::string SeriesDate;
		std::string SeriesTime;
	};

#pragma pack(pop)

	// DLL <-> MEDIP 간 interface
	struct ProjectDataInfo
	{
		//detail
		mint32	dataCX = 0;
		mint32	dataCY = 0;
		mint32	dataCZ = 0;
		float   spaceX = 0;
		float   spaceY = 0;
		float   spaceZ = 0;
		mint32	dataLenth = 0;		// volume length
		mint32	maskSize = 0;		// 3D 마스크 한 개의 크기
		mint32	volumeSize = 0;		// dataLength 와 겹칩, 둘 중 하나 삭제 예정
		BoundingBoxI box;
		mint32	maskInfoSize = 0;

		mint32	windowWidth = 0;	// 2d tab
		mint32	windowLevel = 0;
		mint32	SelectedPreSet = 0;
		mint32	volumeWidth = 0;	// 3d tab
		mint32	volumeLevel = 0;
		mint32	SelectedvolumePreSet = 0;
		mint32	volumeRenderType = 0;
		muint32	nDefaultWidth = 0;
		mint32	nDefaultLevel = 0;
		short	volumeHUMin = 0;	// volume info
		short	volumeHUMax = 0;
		short	downScaleCnt = 0;

		mip::QUATERNION volumeRotation;
		mip::VECTOR3 volumeTranslation;
		float volumeZoom = 0;

		muint8	right_hand_coord = 0;
		bool	isGamma = false;
		float	fGamma = 0.f;

		mint8	modality_type = 0;	// DCM header 정보
		mint8	subStudySize = 0;		// origin mip에서만 씀
		mint8	subSeriesSize = 0;

		bool	SelectedCustomPreset = false;	// 2d tab
		mint8	CuspreVal = -1;				// custom preset number
		bool	SelectedCustomVolumePreset = false;	// 3d tab
		mint8	CusVolumepreVal = -1;

		muint32 seedColor[2];	// draw cut 

		mint32	ThumbSize = 0;	// mipd22, mip21 이후 버전에서는 사용 안함 // 
		mint32	CaptureSize = 0;	// mipd22, mip21 이후 버전에서는 사용 안함 // 

		// 
		mip::VECTOR3	imgOrientation[2];	// xAxis, yAxis
		float	imgPos[3];		// dicom image position
		int nKVP = 0;
		int nmA = 0;

		bool	updateReport = false;	// report
		ReportInfo reportInfo;

		PredictOpt stPredictOptInfo;

		// cunkdata
		std::vector<std::vector<mask>> vecMaskData;
		std::vector<mint16> vecHUData;
		std::vector<AnimationInfo> vecAniLine;
		std::vector<WindowInfo> vecWindowInfo;
		std::vector<AnnoTextInfo> vecAnnoTextInfo;
		std::vector<ProjectMaskInfo> vecMaskInfo;
		std::vector<SurfaceInfo> vecSurfaceInfo;
		std::vector<AnnoLengthInfo> vecAnnoLengthInfo;
		std::vector<AnnoAngleInfo> vecAnnoAngleInfo;
		std::vector<AnnoOvalInfo> vecAnnoOvalInfo;
		std::vector<AnnoArrowInfo> vecAnnoArrowInfo;
		std::vector<uchar> vecThumbnail;		// 안씀
		std::vector<CaptureImgInfo> vecCaptureImgInfo;
		std::string strStudy_desc;
		std::string strSeries_desc;
		std::vector<ImageInfo> vecImageInfo;		// report info
		std::vector<char> vecReportHtml;
		std::map<std::string, int> mapPredictResultUID;	// DeepCatch predict (maskName : UID) 매칭용
		std::string strKernel;
		std::string strManufacturer;
		std::string strManufacturerModel;
		std::vector<AI_ResultInfo> vecAIResultInfo;
		DicomElementInfo dicomElementInfo;
	};
}