#pragma once

#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include <Windows.h>
#include <vector>
#include <map>
#include <qvector.h>
#include <qlistwidget.h>
#include <qlist.h>
#include <memory>
#include <qmutex.h>
#include <functional>

#include "define.h"

#include "graphics/color.h"
#include "graphics/BoundingBox.h"
#include "graphics/volumedata.h"
#include "graphics/Mesh/MeshPrimitiveBuilder.h"

#include "Renderer/Slice.h"
#include "algorithm/MagicCut.h"
#include "UI/TASubClassResultCtrl.h"

#include "defineMipEncoder.h"

#include "Actions/ActionDeepCatchReport.h"
#include "Actions/ActionCOVIDReport.h"
#include "Actions/ActionCuda.h"
#include "Actions/ActionWorkThreadRunner.h"
#include "Actions/ActionDefinitions.h"
#include "Actions/ActionThreadArgument.h"
#include "Actions/ActionResult.h"
//#include "Actions/ActionThreadEndDeepDrawPredict.h"

#include "DataContext.h"
#include "FunctionLevel.h"

namespace mip
{
	class MeshTopology;

	namespace TA {
		struct TextureFeatureValues;
	};
};

class QUndoStack;
class QPolygon;
class QPoint;
class QVector3D;
class QProgressDialog;
class QString;
class QThread;
class QObject;
class QTreeWidgetItem;
class QSlider;
class QColor;

class BoundingBoxI;
class MaskInfo;
class Annotation;
class MaskListWidget;
class VolumeView;
class OpenGLWidget;
class ConfusionMatrixInfoDialog;
class AnalMPRPlaneView;
class DrawcutTab;
class WorkMeshRemesh;
class AnnoArrow;
class AnnoAngle;
class AnnoLength;
class AnnoCurveLength;
class AnnoRectangle;
class AnnoString;

class WindowManager;
class MEVolumeView;
class MedipQT;
class WorkBase;
class WindowBase;

class ActionObserver
{
public:
	virtual void onActionStarted(ACTION_PROCESSING id, void* pData) = 0;
	virtual void onActionFinished(ACTION_PROCESSING id, void* pData) = 0;
};

class ActionManager : public QObject
{
	Q_OBJECT

public:
	//friend ActionThreadEndDeepDrawPredict;

public:
	static ActionManager* getSingleton();

public:
	ActionManager();
	~ActionManager();

public:
	void initProgress(bool _b_cancel = false);
	void initProgress(QWidget* pParentWidget, bool _b_cancel = false);

	void action_ProgressBegin(bool _b_cancel = false);
	void action_ProgressBegin(QString str, bool _b_cancel = false);
	void action_ProgressBegin(const QString& str, QWidget* pParentWidget, bool _b_cancel = false);

	void action_ProgressUpdate(int value, QString strWorkName = "");
	void action_ProgressUpdateWithRange(int value, int start, int end, QString strWorkName = "");
	bool action_ProgressIsCanceled();
	void action_ProgressEnd();
	void action_ThreadExit();
	QProgressDialog* action_ProgressDlgGet();
	mip::TA::TextureFeatureValues* getTextureFeatureVals();
	COVID_REPORT* getCOVID19AnalysisVals();
	mip::MeshTopology* getTempMesh();

	void setUndoStack(QUndoStack* stack);
	QUndoStack* getUndoStack();
	void UndoStack_clear();
	void clear();

	bool setUndoStack_VisualPrint(QUndoStack* stack);
	QUndoStack* getUndoStack_VisualPrint();
	void VisualPrint_UndoStack_clear();

	void addObserver(ActionObserver* pObserver);

	void action_OvalROI(const QPolygon& list, WindowBase* view, mask _mask, int _mI, bool del = false);
	void action_InterDraw(AnalMPRPlaneView* view, QPolygon list, mask _m, int _mI,
		bool _del = false, bool _oval = false, bool _line = false, bool _circle = false);
	void action_Marking(const QPolygon& list, WindowBase* view, bool drawCutMarking, bool merge = false, mask _mask = VM_MASK2, int maskIndex = 0);
	void action_FreeDrawROI(const QPolygon& list, WindowBase* view, bool merge = false, mask _mask = VM_MASK2, int maskIndex = 0);
	void action_FreeDrawROI_Del(const QPolygon& list, WindowBase* view, bool merge = false, mask _mask = VM_MASK2, int maskIndex = 0);
	void action_FreeDrawROI3D(const QPolygon& list, OpenGLWidget* view, mask _mask, bool del, bool merge = false, int maskIndex = 0, int uid = 0);
	void action_FreeDrawROI_Del3D(const QPolygon& list, OpenGLWidget* view, bool merge = false, mask _mask = VM_MASK2, int maskIndex = 0);
	void action_Split3D(OpenGLWidget* view, QPolygon list = QPolygon(), mask _mask = VM_MASK2, int _mI = 0, int uid = 0, WORK_MODE eType = WORK_MODE::WORK_3D_CURVE_SPLIT, mip::PLANE p1 = mip::PLANE(), mip::PLANE p2 = mip::PLANE());
	void action_SketchDrawSegmentation(WindowBase* view, bool merge = false, mask _mask = VM_MASK2, int maskIndex = 0, double GPC_lambda = 50.0);
	void action_Seed_color(DrawcutTab* tab, int index, QColor col);
	void action_Seed_clear(int index);
	void action_PolyROI(const QPolygon& list, WindowBase* view, bool merge = false, mask _mask = VM_MASK2, int _maskIndex = 0);
	void action_PolyROI_Del(const QPolygon& list, WindowBase* view, bool merge = false, mask _mask = VM_MASK2, int _maskIndex = 0);
	void action_MarkingSquare(QPainterPath screen_region, WindowBase* view, mask _m = VM_MASK2, int _mI = 0, bool merge = false, bool drawcut = false, bool brush = false, int startHU = 0, int endHU = 0);
	void action_MarkingInterSquare(QPainterPath screen_region, AnalMPRPlaneView* view, mask _m = VM_MASK2, int _mI = 0, bool _del = false, bool merge = false);
	void action_UnMarking(const QPolygon& list, WindowBase* view, bool merge = false, mask _mask = VM_MASK2, int _maskIndex = 0);
	void action_UnMarkingAround(QVector<QPoint> screen_point, const QPolygon& list, WindowBase* view, bool merge = false, mask _mask = VM_MASK2, int _maskIndex = 0);
	void action_UnMarkingSquare(QPainterPath screen_region, WindowBase* view, mask _m = VM_MASK2, int _mI = 0, bool merge = false);
#if 0
	void action_UnMarkingAround2(const QPoint& screen_point, const QPolygon& list, WindowBase* view, bool merge = false, mask _mask1 = VM_MASK0, mask _mask2 = VM_MASK1);
#endif
	void action_DrawSeedFormMask(mask _mask, bool fore_seed = true, int _maskIndex = 0);

	void action_RegionGrowing(mint32 x, mint32 y, mint32 z, mint32 upper, muint32 lower, bool underROI,
		mask _mask = VM_MASK2, int _maskIndex = 0, bool connect_6 = false, LAYER_RG_SHORTCUT ctrlApply = LAYER_RG_NONE);
	void action_ThresholdSelect(mint32 upper, mint32 lower, bool m_bWithin = false, mask _mask = VM_MASK2, int _maskIndex = 0, bool bEnableProgress = true);
	void action_RandomSelect(int _count, bool _pointunit, int _x_diameter_min, int _x_diameter_max, int _y_diameter_min, int _y_diameter_max, int _z_diameter_min, int _z_diameter_max, int _mean_min_HU, int _mean_max_HU, int _boundary_min_HU, int _boundary_max_HU, bool _cavitymode, int _cavityboundary, int _uid);
	void action_ImageErosion(mask _mask = VM_MASK2, int _maskIndex = 0, uchar _diection = 0);
	void action_ImageDilation(mask _mask = VM_MASK2, int _maskIndex = 0, uchar _diection = 0);
	void action_ImageMove(mask _mask = VM_MASK2, int _maskIndex = 0, uchar direction = 0);
	void action_Image_Hole_Filling(int uid, mask _mask = VM_MASK2, int _maskIndex = 0, mip::HoleFilling::Mode _mode = mip::HoleFilling::Mode::Mode3D, bool bEnableProgress = true);
	void action_ImageVesselness(mask _mask = VM_MASK2, int _maskIndex = 0);
	void action_ImageFissureness(mask _mask = VM_MASK2, int _maskIndex = 0);
	void action_ImageFeatureExtractor_Add(mask _mask = VM_MASK2, int _maskIndex = 0, muint32	uid = 0, bool _report = false);
	void action_ImageFeatureExtractor_Del(muint32 uid);
	///////// radiomics(Texture Analysis)
	void action_TA_CalculateFeature(const QString& strGroupName, mint32 nROIIdx, TASubClassResultCtrl* pSubClassResult, bool bShape3DWithoutDiameter = false);
	void action_TA_Calculate2DMap(const QString& strGroupName, float* p3DVolumeMap, mint32 nROIIdx, std::vector<std::pair<float, float>>* pColorCategory);

	void action_MaskExtractGMM(QString& projectName, int nCluster = 2); // // 220315 허 건 과장(심장 솔루션)
	void action_MaskExtractGMM(QWidget* pWidget, MaskInfo* pMaskInfo, int nCluster); // 220315 허 건 과장(심장 솔루션)
	void action_MaskExtractGMM_Add(void* ExtractMaskInfo); // 220315 허 건 과장(심장 솔루션)

	/////////
	void action_ImageCircumference(mask* pMaskDataPointTmp, mask* pMaskDataPoint, mask maskBit, int nSliceNum, bool bEnableProgress = true);
	void action_ImageEnhance(int level, mask _mask = VM_MASK2);
	void action_ImageGaussian(int index);
	void action_ImageLaplacian(int index);
	void action_imageDrawCut(mask _mask = VM_MASK2);
	void action_imageBoundary(WINDOW_TYPE type, muint32 depth, mask _mask = VM_MASK2, int _maskIndex = 0);
	void action_ImageIsotropic();
	void action_ImageIsotropic_Modification();

	void action_ftpUpload(QString& filename);
	void action_ImageScaling(bool isDownscale = true);
	void action_VolumeCropping();

	// AI
	bool action_Deepdraw_train(QString projName, QVector<MaskInfo*> projInfo, bool useGPU, bool isLoad);
	bool action_Deepdraw_predict(
		QString strInputPath, 
		QString strWeightPath, 
		eMEDIP_FUNCTION_LEVEL weightFunctionLevel, 
		QString projName, 
		BoundingBoxI box, 
		bool useGPU, 
		eDeepPredictAICopyMask copyMask,
		bool bEnableProgress = true, 
		int nFilterUID = -1
	);

	void action_Deepdraw_outset(int outset, int _uid, bool isUndo);
	bool action_Translation_predict(QString projName, bool useGPU, int start = 0, int end = 0);
	void action_AI_ContrastSynthesis(eAIContrastSynthesisOption option);
	void action_AI_Kernel_Continous_Conversion(void);
	void action_AI_Kernel_Neutralization(void);
	void action_AI_Lowdose_Reconstruction(void);
	void action_AI_CT_Quantization(void);

	bool action_VB_IO_predict(QString strInputPath, QString strWeightPath, int nWeightType, QString projName, QString targetMaskName, int nProcessing, BoundingBoxI box, bool useGPU, bool bEnableProgress = true, int nFilterUID = -1);
	bool action_calculate_slice_position(std::vector<int> targetMaskNames, int selectType, float fOffset = 3.0f, bool bEnableProgress = true); //fOffset - cm	

//SUPPORT_L_T_CHECK	
	bool action_Find_Pick_MaskName(std::vector<QString>& targetMaskName, QVector3D nSelectPos, bool bUseYaxis = true, bool bOnlyVB = true); //

// SUPPORT_DEEPCATCH_VERSION_2
	bool action_IO_Aorta_predict(QString strInputPath, QString strWeightPath, int nWeightType, QString projName, int nProcessing, BoundingBoxI box, bool useGPU, bool bEnableProgress = true, int nFilterUID = -1);

	//SUPPORT_DEEPCATCH_V2_DOCTORANSWER
	bool action_HCC_Prediction(
		float& age,			// "Age" from UI;
		float& gender,		// "Gender" from UI; NOTE: (Male: 1.f, Female: 0.f)
		float& agent,		// "Antivirals agent" from UI; NOTE: (Entecavir: 1.f, Tenofovir: 0.f)
		float& cirr,		// "Cirrhosis, baseline" from UI; NOTE: (Yes: 1.f, No: 0.f)
		float& hbeag,		// "Presence of HBeAg, baseline" from UI; NOTE: (Yes: 1.f, No: 0.f)
		float& plt,			// "Platelet, baseline" from UI;
		float& alb,			// "Albumin, baseline" from UI;
		float& tb,			// "Total bilirubin, baseline" from UI;
		float& alt,			// "ALT, baseline" from UI;
		float& hbv,			// "HBV DNA, baseline" from UI;
		float& liver,		// "Liver Volume" from DeepCatch;
		float& spleen,
		bool bEnableProgress = 1
	);



	// COVID
	bool action_COVID19_predict(QString strInputPath, QString strWeightPath, QString projName, int start, int end, bool useGPU);
	void action_COVID19_FeatureExtract();
	/// DeepCatch Custom
	bool action_Classification_predict(QString projName, int start, int end, bool useGPU);

	void action_ImageComponentChoise(mask _mask = VM_MASK2, int _maskIndex = 0, int _componentNum = 0, bool bEnableProgress = true);
	// DeepCatch
	void action_DeepCatch_L3_predict(QString strInputPath, QString strWeightPath, int nWeightType, QString projName, BoundingBoxI box, bool useGPU, bool bEnableProgress = true);
	void action_MultiConnect_predict(
		QString strMsg,
		QString strInputPath, 
		QString strWeightPath, 
		eMEDIP_FUNCTION_LEVEL nWeightType, 
		QString projName, 
		BoundingBoxI box, 
		bool useGPU);
	void action_DeepCatch_FeatureExtract(int funLevel, bool bEnableProgress = true);
	void action_DeepCatch_Abdominal_waist_range(VOLUME_DATA* pVolData, bool bEnableProgress = true);
	void action_DeepCatch_Abdominal_waist_range_Predict(QString strInputPath, QString strWeightPath, int nWeightType, QString projName, BoundingBoxI box, bool useGPU, bool bEnableProgress = true);

	void action_BoundingBox_Modify(BoundingBoxI& box);

	void action_MaskList_clear_one(muint32 index, int maskIndex = 0);
	void action_MaskList_add();
	void action_MaskList_copy(muint32 index);
	void action_MaskList_copy_ex(VOLUME_DATA* pVolumeData, muint32 index);
	void action_MaskList_del(muint32 index, int maskIndex = 0);
	void action_MaskList_del_list(std::vector<muint32>& list, bool bDeleteAll = false);
	void action_MaskList_del_ex(VOLUME_DATA* pVolumeData, muint32 index, int maskIndex = 0);
	void action_MaskList_del_list_ex(VOLUME_DATA* pVolumeData, std::vector<int>& deleteIndexList);
	void action_MaskList_intersection(MaskInfo& maskinfoA, MaskInfo& maskinfoB);
	void action_MaskList_merge(DataContext* pDataContext, std::vector<muint32>& list);
	void action_MaskList_compute_comfusionMtx(const std::vector<muint32>& list, ConfusionMatrixInfoDialog& confusionInfo);
	void action_MaskList_color(MaskListWidget* maskList, muint32 index, COLOR color);
	void action_MaskList_text_change(MaskListWidget* maskList, muint32 data_index, QString& str);
	void action_MaskList_move(MaskListWidget* maskList, muint32 source_index, muint32 dest_index);
	void action_MaskList_Invert(MaskInfo& maskinfo, int maskIndex = 0);
	void action_MaskList_DifferenceAB(MaskInfo& maskinfoA, MaskInfo& maskinfoB);
	void action_MaskList_Levelset(mask _m, int _mI, int _iter, double _lambda, int _radious);
	void action_MaskList_ZFlip(MaskInfo& maskinfo, int maskIndex = 0);
	void action_MaskList_YFlip(MaskInfo& maskinfo, int maskIndex = 0);
	void action_MaskList_XFlip(MaskInfo& maskinfo, int maskIndex = 0);
	bool action_MaskList_add_LiverResult(int ww, int wl, int start, int to);
	bool action_MaskList_addRawFile(const QString& fileName, bool reverse);
	bool action_MaskList_addROIFile(const QString& fileName);
	bool action_MaskList_addTXTFile(const QString& fileName);
	void action_MaskList_mesh_connect(DataContext* pDataContext, muint32 lUID);
	void action_MaskList_mesh_connect(DataContext* pDataContext, bool show);
	void action_MaskList_mesh_connect(DataContext* pDataContext, std::vector <muint32>& list, bool Conn = false);
	void action_MaskList_As_Drawseed(int uid);
	void action_MaskList_Cancel_Drawseed(int uid);

	void action_MaskList_splitRegion(bool bEnableProgress = true);
	void action_MaskList_3D_Interpolation(int direction = 1, bool bEnableProgress = true); //1 = Axial

	void action_MaskList_ImageCalculate(mint16* pOriginVolumeData);

	void action_Window_Width(int width);
	void action_Window_Level(int level);
	void action_Volume_Width(int width);
	void action_Volume_Level(int level);
	void action_LevelWidth_Auto(int prelevel, int level, int prewidth, int width, float prescalescope, float scalescope);
	void action_Preset(int prelevel, int prewidth, SLICE_PRESET prePreset, SLICE_PRESET preset, int cusPre = -1);
	void action_VolumePreset(int prelevel, int prewidth, SLICE_PRESET prePreset, SLICE_PRESET preset, int cusPre = -1);

	void action_MeshList_add(DataContext* pDataContext, mint32 layerUID, QString strName, mip::MeshTopology* mesh, bool upScale = false);
	void action_MeshList_add_Multi(DataContext* pDataContext, std::vector<mint32>& _vt_layer_uid, std::vector<QString>& _vt_file_name, std::vector<mip::MeshTopology*>& _vt_mesh, bool _b_upscale = false);
	void action_MeshList_add_VisualPrint(mint32 layerUID, QString strName, mip::MeshTopology* mesh, bool upScale = false);
	void action_MeshList_delete(DataContext* pDataContext, mint8 mUID, mint32 lUID);
	void action_MeshList_delete(DataContext* pDataContext, std::vector<mint8> mUID);
	void action_MeshList_update(DataContext* pDataContext, mint8 mUID, mip::MeshTopology* newMesh);
	void action_UndoRedo_update(DataContext* pDataContext, MESH_WORK_MODE _work_mode = MESH_WORK_NONE, MESH_WORK_MODE _work_prev_mode = MESH_WORK_NONE);
	void action_MeshList_split(DataContext* pDataContext, mint8 mUID, mip::MeshTopology* m1);
	void action_MeshList_color_change(DataContext* pDataContext, mint8 mUID, QColor col);
	void action_MeshList_name_change(DataContext* pDataContext, mint8 mUID, QString newName);
	void action_MeshList_UID_change(DataContext* pDataContext, mint8 oldUID, mint8 newUID);
	void action_MeshList_Scale_change(DataContext* pDataContext, mint8 mUID, float* scal);
	void action_MeshList_Translation_change(DataContext* pDataContext, mint8 mUID, mip::VECTOR3 tran);
	void action_MeshList_Rotation_change(DataContext* pDataContext, mint8 mUID, float* fRot);
	void action_MeshList_create_polygon(DataContext* pDataContext, eMeshPrimitiveType type);
	void action_MeshList_create_polygon(DataContext* pDataContext, eMeshPrimitiveType type, mip::VECTOR3 vecSize, QString strName, int* resolution = nullptr);
	void action_MeshList_align_change(DataContext* pDataContext, muint8 mUID, int Stype = 0, int Dtype = 0);

	// Visual Print
	/////////////////////////////////////////////////////////////////////////////////////
	void action_VisualPrint_MeshList_name_change(DataContext* pDataContext, mint8 mUID, QString newName);
	void action_VisualPrint_MeshList_color_change(DataContext* pDataContext, mint8 mUID, QColor col);
	void action_MeshList_update_VisualPrint(DataContext* pDataContext, mint8 mUID, mip::MeshTopology* newMesh);
	void action_UndoRedo_update_VisualPrint(DataContext* pDataContext);
	/////////////////////////////////////////////////////////////////////////////////////


	void action_Annotation_Del(WindowBase* view, muint32 annoIndex);
	void action_Annotation_Text_Add(AnnoString* pAnnoText);
	void action_Annotation_Text_Edit(WindowBase* view, muint32 annoIndex, AnnoString* pOrigin, AnnoString* pNew);

	void action_Annotation_Len_Add(AnnoLength* pAnnoLength);
	void action_Annotation_Len_Edit(muint32 annoIndex, AnnoLength* pOrigin, AnnoLength* pNew);

	void action_Capture_image_Add(QImage);
	void action_Capture_image_Del(int);

	void action_ImageList_Del(int index);
	void action_ImageList_Import(const QImage& img);

	void action_Annotation_Angle_Add(AnnoAngle* pAnnoAngle);
	void action_Annotation_Angle_Edit(muint32 annoIndex, AnnoAngle* pOrigin, AnnoAngle* pNew);

	void action_Annotation_Arrow_Add(AnnoArrow* pAnnoArrow);
	void action_Annotation_Arrow_Edit(muint32 annoIndex, AnnoArrow* pOrigin, AnnoArrow* pNew);

	void action_Annotation_Rectangle_Add(AnnoRectangle* pAnnoRect);
	void action_Annotation_Rectangle_Edit(muint32 annoIndex, AnnoRectangle* pOrigin, AnnoRectangle* pNew);

	void action_Annotation_Path_Add(QVector3D vec, int group = 0);
	void action_Annotation_Path_Del(int index, int group = 0);
	void action_Annotation_Path_Clear(int group = 0);
	void action_Annotation_Path_new();

	void action_FileWork_Preview_Surface(DataContext* pDataContext, mint32 layerUID, mask m, int mI = 0);
	void action_FileWork_Preview_Surface(DataContext* pDataContext, mint32 layerUID, mask m, int mI, bool bEnableProgress, QString qstrProgressTitle = "");
	void action_FileWork_Preview_SurfaceVisualPrint(mint32 layerUID, mask m, int mI, bool bEnableProgress, QString qstrProgressTitle = "");
	void action_FileWork_Preview_Surface(DataContext* pDataContext, mint32 layerUID, mask m, int mI, bool bEnableProgress, QString qstrProgressTitle, QWidget* pParentWidget);
	void action_FileWork_Export_STLFile(DataContext* pDataContext, muint32 index, mask m, QString& strFilename, bool bPatientCoordinate = false, int mI = 0);
	void action_FileWork_Export_Mesh_To_STLFile_Single(DataContext* pDataContext, QString& strFilename, mip::MeshTopology* m, bool upScale, bool bEnableProgress, QString qstrProgressTitle, bool bPatientCoordinate = false);
	void action_FileWork_Export_Mesh_To_STLFile_Multi(DataContext* pDataContext, QString& filename, bool bPatientCoordinate = false);
	void action_FileWork_Export_OBJFile(DataContext* pDataContext, muint32 index, mask m, QString& strFilename, bool bPatientCoordinate = false, int mI = 0);
	void action_FileWork_Export_OBJFile(DataContext* pDataContext, QString& strFilename, mip::MeshTopology* m, bool upScale, bool bEnableProgress, QString qstrProgressTitle, bool bPatientCoordinate = false);
	//	void action_FileWork_Export_OBJFile(QString & strFilename, mip::MeshCore * m, bool upScale, bool bEnableProgress = true, QString qstrProgressTitle = "");
	void action_FileWork_Export_OBJFile(DataContext* pDataContext, QString& strFilename, mip::MeshTopology* m, bool upScale = false, bool bPatientCoordinate = false);
	void action_FileWork_Export_VTKFile(DataContext* pDataContext, muint32 index, mask m, QString& strFilename, bool bPatientCoordinate = false, int mI = 0);
	void action_FileWork_Export_VTKFile(DataContext* pDataContext, QString& strFilename, mip::MeshTopology* m, bool upScale = false, bool bPatientCoordinate = false);
	void action_FileWork_Export_3MFFile(DataContext* pDataContext, QString& strFilename, mip::MeshTopology* m, bool upScale = false, bool bPatientCoordinate = false);
	void action_FileWork_Export_USDFile(DataContext* pDataContext, const QString& filePath, const std::vector<int>& indexList);

	void action_FileWork_Export_ROINIIFile(mask m, const QString& strFilename, int mI = 0);
	void action_FileWork_Export_HUNIIFile(mask m, const QString& strFilename, int mI = 0);
	void action_FileWork_Export_COORDINATENIIFile(mask m, const QString& strFilename, int mI = 0);
	void action_FileWork_Export_NIIFile(const QString& strFilename, bool _b_use_thread = true, bool patch = false, BoundingBoxI box = BoundingBoxI());
	void action_FileWork_Export_NRRDFile(const QString& strFilename, bool b_use_thread = true, bool patch = false, BoundingBoxI box = BoundingBoxI());
	void action_FileWork_Export_TXTFile(muint32 index, mask m, const QString& strFilename, int mI = 0);
	void action_FileWork_Export_HURawFile(QString& mPath, mask _m = 0, int _mI = 0, bool _patchy = false, BoundingBoxI _box = BoundingBoxI());
	void action_FileWork_Export_RawFile(QString& mPath, mask _m = 0, int _mI = 0);
	void action_FileWork_Export_MipFile(const QString& strFilename, MIP_ENCODER::PROJ_TYPE eProjType, bool bUseThread = true, bool bEnableProgress = true);
	void action_FileWork_Export_JsonFile(const QString& strFilename, QVector<QPair<QString, QString>>& clinicalInfo, QVector<sLocalClinicalInfo>& localClinicalInfo, bool bUseThread = true);
	void action_FileWork_Import_Dicom(QString& strFilename);
	void action_FileWork_Split_ToFile();
	void action_FileWork_Split_ToFile(mask _m, int _mI, int uid, int spCount, QString filePath, int fileType = 0);
	bool action_FileWork_Import_Mip(QString& strFilename, MIP_ENCODER::PROJ_TYPE eProjType, bool bUseThread = true);
	void action_FileWork_Import_NII(QString filename);
	void action_FileWork_Import_PRD(QString filename);
	void action_FileWork_Import_RAW(QString filename);
	void action_FileWork_Import_MED(QString filename);
	void action_FileWork_Import_ROI(QString filename);
	void action_FileWork_Import_TXT(QString filename);
	void action_FileWork_Import_STL(DataContext* pDataContext, QStringList filename);
	void action_FileWork_Import_3MF(DataContext* pDataContext, QStringList filename);
	void action_FileWork_Import_OBJ(DataContext* pDataContext, QStringList filename);
	void action_FileWork_Import_VTK(DataContext* pDataContext, QStringList filename);
	void action_FileWork_Import_USD(DataContext* pDataContext, const QString& filepath);
	void action_FileWork_Import_IMG(QString filename);

	void action_VisualPrintWork_Mesh_Export();

	void action_Create_MatchingData(DataContext* pDataContext, mip::MeshTopology* mesh);
	int getAction_state();

	void action_Mesh_Planecut(
		DataContext* pDataContext,
		MESH_WORK_MODE _mode,
		MESH_WORK_MODE _prev_mode,
		mip::MeshTopology* _plane_mesh,
		mip::MATRIX44& _plane_mat,
		std::vector<mip::MeshTopology*>& _vt_mesh,
		std::vector<std::vector<mip::VECTOR3>>* _vt_pt_holes,
		bool _b_fill_hole = false,
		bool _b_remesh = false,
		bool _b_smooth = false,
		bool _b_inverse_zaxis = false
	);
	void action_Mesh_Polycut(
		DataContext* pDataContext,
		MESH_WORK_MODE _mode,
		MESH_WORK_MODE _prev_mode,
		std::vector<mip::VECTOR2>& _vt_polygons
	);
	void action_Mesh_FindHole(
		mip::MeshTopology* _p_mesh,
		std::vector<std::vector<mip::VECTOR3>>& _vt_boundary_pts,
		std::vector<std::pair<std::vector<int>, bool>>& _vt_boundary_pts_idx
	);
	void action_Mesh_FillSelectedHole(
		DataContext* pDataContext,
		mip::MeshTopology* _p_mesh,
		int _hole_idx,
		bool _b_remesh,
		bool _b_smooth,
		std::vector<std::vector<mip::VECTOR3>>& _vt_boundary_pts,
		std::vector<std::pair<std::vector<int>, bool>>& _vt_boundary_pts_idx,
		bool _b_fill_hole_all = false
	);
	void action_Mesh_Boolean(DataContext* pDataContext, MESH_WORK_MODE type, muint8 mUID, muint8 mUID2);

	void action_Mesh_Remesh(MESH_REMESH_TYPE type, float offset, muint8 mUID, DataContext* pDataContext, MEVolumeView* pViewer);
	void action_Mesh_Smooth(MESH_REMESH_TYPE type, float offset, muint8 mUID, DataContext* pDataContext, MEVolumeView* pViewer);

	void action_Mesh_Attach(DataContext* pDataContext, QList<muint32>& _list);
	void action_Mesh_Duplicate(DataContext* pDataContext, QList<muint32>& _list);

	void action_Mesh_MeshToMask(QList<muint32>& _list, DataContext* pDataContext);

	//20201207_byPHS
	WorkMeshRemesh* action_Mesh_Init(MESH_REMESH_TYPE type, muint8 mUID, DataContext* pDataContext, MEVolumeView* pViewer, float offset = 0.0f);
	void action_Mesh(WorkMeshRemesh* pWorkMesh);

	//SUPPORT_STAMP3D
	void action_Mesh_Stamp3D(
		mip::MeshTopology* pTargetMesh,
		mip::MeshTopology* pFontMesh,
		int  pickFaceIdx,
		float offset,
		bool bIntaglio = true	//음각
	);

	// Macro 관련.
	void action_MACRO_Start(void);
	void action_DeepCatch_MACRO_Start(void);

	void action_MEDIP_MACRO_Start();
	void action_MEDIP_MACRO_Export_Mask_Raw_Start(const QString& mipFilePath, const QString& saveDirectoryPath);
	void action_MEDIP_MACRO_Export_Mask_NII_Start(const QString& mipFilePath, const QString& saveDirectoryPath);

	void action_Omniverse_Upload_USD(const QString& ipAddress, const QString& serverPath, const QString& uploadFilePath);
	bool action_EmptyStart_WithThread(ACTION_PROCESSING action, QString* pOutErrorString, QThread** ppOutThread);

	void DeepCatchCurMacroType(QMap<QString, sDeepcatchMacroColumnData>& macroLine, unsigned short& macroType,
		bool& bMuscleQualityMap, bool& bIOClassification, bool& bVertebra, bool& bLiverSpleen, bool& bAorta, bool& bBodyComposition);
	bool SettingMacroData(QString strFilename, /*output*/int& nRow, /*output*/int& nCol);
	void SettingMacroThreadData(int MacroCommandLineIndex);
	void MacroErrorRecord(sDeepcatchMacroRowData errorMacroCommand, QString strErrorString);


	sReportOtherInfo* getReportOtherInfo() const { return m_pReportOtherInfo; }
	DEEPCATCH_REPORT* getDeepCatchAnalysisVals() const { return m_pDeepCatch_Report; }
	DEEPCATCH_REPORT_PREDICT_INFO* getDeepCatch_PredictedInfo() const { return m_pDeepCatch_PredictedInfo; }

	void action_EmptyStart(ACTION_PROCESSING eActionProcess);
	void action_ThreadEnd(DataContext* pDataContext);
	void action_ThreadEnd_DeepDrawPredict();
	void action_ThreadEnd_CopyAIResultToMask();
	void action_ThreadEnd_DeepDrawPredict_Ex(WindowManager* pWindowManager, ActionManager* pActionManager);

	void BreakLock();
	bool getThreadLock(ACTION_PROCESSING id = ACTION_PROCESSING::ACTP_NONE);

	bool isActionFinished();

	void action_VisualPrintFTPUpload(DataContext* pDataContext, QString& filename, bool bEnableProgress);
	void action_VisualPrintFTPUploadROI(DataContext* pDataContext, QString& filename);// Add For Visual Print (Upload ROI File) by Lim Young-il
	void action_VisualPrintWebLink(DataContext* pDataContext);
	void action_VisualPrintDialog(DataContext* pDataContext, QWidget* pMainWindow);

	sReportOtherInfo* ReportOtherInfoSafeCreate();
	DEEPCATCH_REPORT* DeepcatchReportInfoSafeCreate();
	DEEPCATCH_REPORT_PREDICT_INFO* DeepcatchReportPredictedInfoSafeCreate();
	void DeepcatchReportPredictedInfoSafeDelete();
	void threadQueueClear();

	//신규 인터페이스
	void action_Start_List(const std::deque<ThreadArgExtension>& argumentList, QString* pOutErrorMessage, QThread** pOutWorkThread);
	void AddActionThreadEndCallBack(std::function<void(void*)> pFunction, void* pContext);

	void SendActionFinished(int id);
	void SendActionProcessFinished(ACTION_PROCESSING actionProcessingID);

	std::vector<QUndoCommand*> GetReservationWorkList() const;
	void ClearReservationWorkList();
	void AddReservationWork(QUndoCommand* pWork);

public:
	// 매크로 쓰레드 순서 queue
// 구조체로 만들어서 결과를 shared_ptr, void* 로 넘김
// use ex
// current thread 에서 front의 pThreadResult 입력
// thread end에서 bAfterthread = m_qThreadNext->eNextThread
// next threa에서 front()의 pThreadResult 사용 후 reset
// m_qThreadNext.pop();
	void NextThreadSetting(void);

	int CreateL3AWPredictInput(int nUID, WINDOW_TYPE eProjectionType, QString strDataPath, QString strFileName);
	void CreateAWPredictInput(int nUID, WINDOW_TYPE eProjectionType, QString strDataPath, QString strFileName);

	void SetAfterThread(eAfterTHREAD value);
	eAfterTHREAD GetAfterThread() const;

	void SetCurrentThread(eAfterTHREAD value);
	eAfterTHREAD GetCurrentThread() const;
private:
	void setThreadUnlock();

Q_SIGNALS:
	void sig_actionFinished(int id);
	void sig_actionProcessFinished(int actionProcessingID);

public:
	std::deque<ActionThreadArgument> m_qThreadNext;
	int m_nMultiThreadTotalCount = 0;
	int m_nCurrentThreadCount = 0;

	// thread temporay 변수로 사용
	// volumedata에 있는 각종 temp 변수를 대체하기 위해서 만듬
	std::unordered_map<std::string, std::shared_ptr<void>> m_hashThreadResult;

	// DeepCatch MACRO MODE
	bool m_IsMacroMode = false;
	bool m_IsMultiSeries = false;
	QList<sDeepcatchMacroRowData> m_ListMacroCommandLine;
	int m_SeriesInfoIndex = 0;

	ActionResult actionResult;
	WorkBase* m_pCurrentWork;
	QThread* m_pCurrentThread;

	std::vector<MaskInfo> m_newGenerateAIMaskInfos;
private:
	MedipQT* m_pMainWindow;

	ACTION_PROCESSING m_currentActionID = ACTION_PROCESSING::ACTP_NONE;
	eAfterTHREAD m_eAfterThread = THREAD_NONE;
	eAfterTHREAD m_eCurThread = THREAD_NONE;

	QMutex m_threadLock;
	bool m_bActionFinished;

	QUndoStack* m_pUndoStack;
	QUndoStack* m_pUndoStack_VisualPrint;


	sReportOtherInfo* m_pReportOtherInfo = nullptr;
	DEEPCATCH_REPORT* m_pDeepCatch_Report = nullptr;
	DEEPCATCH_REPORT_PREDICT_INFO* m_pDeepCatch_PredictedInfo = nullptr;

	std::vector<ActionObserver*> m_actionObservers;
	std::vector<QUndoCommand*> m_vecReservationWork;

private:
	std::function<void(void*)> m_pActionThreadEndCallBack;
	void* m_pActionThreadEndCallBackContext;


private:
	unsigned int m_progressValue;
	unsigned int m_threadID;

	QObject* m_psWorker;
	QThread* m_spThread;

	ACTION_PROCESSING m_state;
	mask m_mask;		// mask bit
	int m_maskIndex;	// mask array index
	mip::MeshTopology* m_tempMesh;
	mint32 m_tempIndex;			// mask layer index로 사용

	// thread 결과값 저장용
	QString m_actionText;
	mip::TA::TextureFeatureValues* m_pTextureFeatureVals;

	QProgressDialog* m_pProgressDlg;
	COVID_REPORT* m_pCovid_Report;
};

template <typename T>
void InsertActionThreadResult(ActionManager* pActionManager, ACTION_PROCESSING key, const T& data)
{
	std::string keyStr = QString::number(key).toStdString();
	std::shared_ptr<T> pData = std::make_shared<T>(data);
	pActionManager->m_hashThreadResult.insert(std::make_pair(keyStr, pData));
}

template <typename T>
std::shared_ptr<T> PopActionThreadResult(ActionManager* pActionManager, ACTION_PROCESSING key)
{
	std::string keyStr = QString::number(key).toStdString();
	auto it = pActionManager->m_hashThreadResult.find(keyStr);
	if (it == pActionManager->m_hashThreadResult.end())
	{
		return nullptr;
	}
	std::shared_ptr<void> pData = (*it).second;
	pActionManager->m_hashThreadResult.erase(keyStr);
	return std::static_pointer_cast<T>(pData);
}

#define ACTION_MANAGER ActionManager::getSingleton()

#endif