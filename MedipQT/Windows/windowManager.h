#pragma once

#include "define.h"
#include "WindowBase.h"
#include "graphics/volumedata.h"
#include "graphics/volumedatapet.h"
#include "graphics/AnnoDeclare.h"
#include "Renderer/VolumeRender.h"
#include "Renderer/texture.h"
#include "Renderer/Mesh.h"
#include "Renderer/color.h"
#include "DicomReader.h"
#include "Windows/ImageManagementTab.h"
#include "Windows/ReportWidget.h"
#include "Windows/Main/MainVisualPrintMeshWidget.h"
#include "SelectSliceDlg.h"
#include "Windows/MessageBox/MessageBoxBase.h"

#include <vtkSmartPointer.h>
#include <QMutex>
#include <QVector>
#include <QMap>
#include <vector>
#include <utility>
#include <qdom.h>

#define ELEMENT_FILE "FileConfig"
#define ELEMENT_PRESET "PresetConfig"
#define ELEMENT_LICENSE "LicenseConfig"

class MedipQT;
class QWidget;
class QTabWidget;
class QHBoxLayout;

class vtkDICOMImageReader;
class vtkAlgorithm;
class vtkImageData;
class vtkVolume;
class vtkFixedPointVolumeRayCastMapper;
class vtkColorTransferFunction;
class vtkPiecewiseFunction;
class vtkVolumeProperty;
class vtkBoxWidget;
class vtkDataArray;
class QVBoxLayout;
class QProgressBar;
class QOpenGLContext;
class QOpenGLPaintDevice;
class QStatusBar;
class MainSegmentWidget;
class MainMeshWidget;
class MainAnalWidget;
class MainTAWidget;
class ReportWidget;

class QTimer;
class TabWindow;
class CroppingDialog;

class CMeshViewBtn3DScene;
class CMeshModelViewManager;

class ActionManager;
class ProductManager;

namespace fm
{
	class FileManagerImportData;
}
namespace mip
{
	class DcmtkVolumeReader;
	class Renderer;
	class MeshTopology;
	class SCAMERA;
}

class WindowManagerFileLoader;

class WindowManager
{
public:
	friend WindowManagerFileLoader;

public:
	enum eModalityType
	{
		Modality_HU,
		Modality_SI,
	};

public:
	const int m_undoBasic;
	const int m_maxVolumeAlpha = 200;

public:
	static WindowManager* getSingleton();
	static mip::Renderer* GetSingletonRenderer();
private:

public:
	WindowManager();
	WindowManager(DataContext* pDataContext, ActionManager* pActionManager, mip::Renderer* pRenderer);
	~WindowManager();

public:
	bool IsImportListEmpty() const;
	void ClearFileData();
	
	bool fileOpen(const QString& filepath, bool bReverse = true);
	bool fileOpen_PET_DCM(const QString& filename);
	bool fileMultiOpen(const QStringList& filePathList);
	bool fileMultiImport(const QStringList& filePathList);

	bool loadDicomData(const mint16* HU, DcmtkSeriesInfo& dcmSeriesInfo, DicomVolumeInfo& dcmVolumeInfo);

	bool fileSave(const QString& filename, bool exit = false);
	bool lastestPathGet(QString& outFilepath, bool onlyPath = false);
	void lastestPathSave(const QString& filepath);
	void setLowSpecOptions();
	void setAdvancedConfig();
	bool IsSecondaryImage(DcmtkSeriesInfo* pDcmInfo);

	void useSkipRender(int index, bool res = true);

	LAYER_RG_SHORTCUT getRGType();
	void setRGType(LAYER_RG_SHORTCUT type);
	int	getAnimationGroup();
	int	getAniStartPoint(int _group = 0);
	bool isContainAniPoint(QVector3D, int group = 0);

	void setLatestActiveViewType(WINDOW_TYPE type);
	WINDOW_TYPE getLatestActiveViewType();
	void setLatestActiveViewDepth(uint depth);
	uint getLatestActiveViewDepth();
	float getLatestActiveViewZoomX();
	float getLatestActiveViewZoomY();
	void setLatestActiveViewZoomX(float x);
	void setLatestActiveViewZoomY(float y);

	void setWheelZoom(bool set);
	void setSliderMid(WINDOW_TYPE type);
	bool getWheelZoom() { return m_wheelZoom; }

	void setColor2DMode(bool set);
	bool getColor2DMode() { return m_color2D; }

	void setInvertingLUTMode(bool set);
	bool getInvertingLUTMode() { return m_invertingLUT; }
	void setBoundaryMode(bool b) { m_BoundaryMode = b; }
	bool getBoundaryMode() { return m_BoundaryMode; }
	void setShowClippingBox(bool show) { m_showClippingBox = show; }
	bool getShowClippingBox() { return m_showClippingBox; }
	void setShowViewAxises(bool b) { m_showViewAxises = b; }
	bool getShowViewAxises() { return m_showViewAxises; }

	void setEdgeMode(bool val) { m_edge_mode = val; }
	bool getEdgeMode() { return m_edge_mode; }
	void setLayer2DAlphaValue(int val, bool config = true, bool render = true);
	float getLayer2DAlphaValue();

	/*mip::MeshTopology*	getMainMesh(int uid);*/

	void showControls(WINDOW_TYPE type);
	void hideControls();

	void InitViewControlsState();
	bool IsEnableViewControls();
	void SetEnableViewControls(bool bEnable);

	void setSharedPrefer(MAINTAB_TYPE preType);
	void setClipOpt();
	void setMaskingmode(bool);
	bool getVolumeMixMode() { return m_maskingmode; }
	int getVolumeAlphaMax() { return m_maxVolumeAlpha; }
	float getVolumeAlpha(bool isOrigin = false);
	void setVolumeAlpha(int);
	void resetVolumeAlpha();
	float getLayerAlpha(bool isOrigin = false);
	void setLayerAlpha(int);
	float getGamma() { return m_Gamma; }
	void setGamma(float gamma) { m_Gamma = gamma; }

	void setBrushThickness(int thickness) { m_BrushThickness = thickness; }
	int getBrushThickness() { return m_BrushThickness; }

	void setBrushOpacity(int opacity) { m_BrushOpacity = opacity; }
	int getBrushOpacity() { return m_BrushOpacity; }

	void setVisibleMesh(bool set);
	bool getVisibleMesh() { return m_showMesh; }
	void setCullingMesh(bool set);
	bool getCullingMesh() { return m_clipMesh; }
	void setVisibleMaskVolume(bool set) { m_showMaskVol = set; }
	bool getVisibleMaskVolume() { return m_showMaskVol; }
	void setCullingMaskVolume(bool set);
	bool getCullingMaskVolume() { return m_clipMaskVol; }
	void setClip2DPlane(bool set);
	bool getClip2DPlane() { return m_clip2DPlane; }

	void selectSeedMode();
	void selectPatchyMode();
	void moveRegionMode();

	float GetAniLength(int index, bool isSpiral = false, bool onlyLength = false, bool isMM = true);
	float CalcLength(QVector3D, QVector3D, bool isVolume = true); //false ==> world coord
	void setRegionGrowingSeed(muint32 x, muint32 y, muint32 z, mint16 Hu, bool layerRG = false);
	void setPatchyPoint(bool updateInfo = false);
	void enableTOIAll();

	void buildRenderBufferTopology(mip::MeshTopology* _p_mesh, mip::SRenderBufferParams* _parmas = nullptr);
	void moveMeshCenterScene(int _idx);
	void moveMeshCenterScene(bool _b_all = false);

	void renderLater_3DView();
	void renderLater_All(bool volumeupdate = true);
	void renderLater_SubView();
	void renderLater_GridView(bool volumeupdate = true);

	void renderLater_AnalView(bool volumeupdate = true);

	void forceUpdate2DViewData(bool updateImage = true, bool updateMask = false);
	void forceUpdateRadiomicsView();
	void forceUpdateSegmentationWidget();

	void deleteKeyProcess();
	void escKeyProcess();
	void openBracketProcess();
	void closeBracketProcess();

	void resetResource();
	void resetUI();
	void updateUI(bool isSelection = false, int selection = -1);
	void updateUIMultiSelect(const std::vector<muint32>& vecMultiMaskUID);
	void updateMeshUI(bool refresh = true, int selIndex = -1, bool _b_clear = true, bool b_close_dlg = true);
	void updateVisualPrintMeshUI(bool refresh = true, int selIndex = -1, bool _b_clear = true);

	void updateSeed();
	void updateSummary();
	bool isDicomSeriesInfoShouldBeUpdated();
	bool importDicomFile_And_UpdateSummary();
	void applyVoxelToUI(int, bool seed = false);
	void resetRGValue();
	void setAIRange(bool init = true);
	//void setAIDepth(int, int,bool _init=false);
	void setAIDepth(BoundingBoxI box);
	void settingOutset(void);
	void updateAnnotation();
	void clearFeatureList();
	mask getSelectedMask();
	int getSelectedMaskByteIndex();
	int getSelectedMaskUID();
	QColor getSelectedMaskColor();

	void setPreset(SLICE_PRESET type, bool reset = false, int cusPre = -1);
	void setVolumePreset(SLICE_PRESET type, bool reset = false, int cusPre = -1);
	void setAutoScaleSlope(float);
	void setWindowWidth(int width);
	void setWindowLevel(int level);
	void setVolumeWidth(int width);
	void setVolumeLevel(int level);
	void addVolumeWidth(int width);
	void addVolumeLevel(int level);

	void setVolumeWidthLevel(int width, int level);
	void setWindowWidthLevel(int width, int level);
	int getWindowWidth() { return m_window_width; }
	int getWindowLevel() { return m_window_level; }

	int getVolumeWidth() { return m_volume_width; }
	int getVolumeLevel() { return m_volume_level; }
	SHADER_QUALITY	getShaderQuality() { return m_shader_quality; }
	void setShaderQuality(SHADER_QUALITY value);

	bool saveOrginVolumeData();
	bool resetTextures();
	bool resetLowSpecTextures();
	bool resetMaskTexture();
	bool initPresetTexture();
	bool updatePresetTexture(SLICE_PRESET type = SP_DEFAULT, int cusType = -1, bool delCus = false);
	bool update2DPresetTexture(SLICE_PRESET type = SP_DEFAULT, int cusType = -1, bool delCus = false);
	bool preRenderProcess();
	bool preRenderProcessMPR();

	mip::COLOR getVolumeColor(QVector<int>, QVector<QColor>, int index, int preset = 0);
	muint32 getVolumeTexture() { return m_nTexVolume; }
	muint32 getMaskTexture() { return m_nTexMask; }
	muint32 get3DPresetTexture() { return m_nTexTF; }
	muint32 getAlphaPresetTexture() { return m_nTexTFAlpha; }
	muint32 get2DPresetTexture() { return m_nTex2DTF; }

	muint32 get2DMPR_Axial() { return m_nTex2DAxial; }
	muint32 get2DMPR_Coronal() { return m_nTex2DCoronal; }
	muint32 get2DMPR_Sagittal() { return m_nTex2DSagittal; }
	muint32 get2DMPR(WINDOW_TYPE type);
	muint32 getLowSpec2DMPR(WINDOW_TYPE type);

	muint32 getDefaultWidth() { return m_nDefaultWidth; }
	mint32 getDefaultLevel() { return m_nDefaultLevel; }
	void setDefaultWidth(muint32 width) { m_nDefaultWidth = width; }
	void setDefaultLevel(mint32 level) { m_nDefaultLevel = level; }

	int getRenderType() { return m_renderType; }
	void setRenderType(int type);

	bool saveRAW(QString& mPath, mask _m, int _mI = 0);
	bool saveHURAW(QString& mPath, mask _m = 0, int _mI = 0, bool _patchy = false, BoundingBoxI _box = BoundingBoxI());
	void saveROI(QString& mPath);
	bool saveVTK(QString& filename, muint32 index, mask _m = 0, int _mI = 0);

	bool readROIFile(unsigned char* image, QString& filename);
	void readROINIIFile(unsigned char* image, vtkDataArray* scalar);

	void loadNextFiles();
	bool loadFiles(const QString& filepath);
	bool loadPRD(const QString& filepath);

	//bool loadRAW(const QString& mPath, bool reverse = true);
	//bool loadROI(const QString& filepath);
	//bool loadNII(const QString& filepath);
	//bool loadTXT(const QString& filepath);

	//bool loadSTL(const QString& filepath);
	//bool loadOBJ(const QString& filepath);
	//bool loadVTK(const QString& filepath);
	//bool loadUSD(const QString& filepath);
	//bool load3MF(const QString& filepath);

	bool loadImage(const QString& path);
	bool previewSurface(mint32 layerUID, mask _m, int mI = 0);
	bool previewSurface(mint32 layerUID, mask _m, int mI, bool bEnableProgress);
	bool previewSurfaceVisualPrint(mint32 layerUID, mask _m, int mI, bool bEnableProgress);
	bool previewSurface(mint32 layerUID, mask _m, int mI, bool bEnableProgress, QWidget* pParentWidget);
	bool hideSurface(muint32 index);

	bool saveFiles(QString& filename, muint32 uid, mask _m, int _mI, EXPORT_FILES _fType = EX_FILES_NONE, bool bPatientCoordinate = false, bool appendType = true, bool chkDupFile = true);
	bool SaveMeshFiles(QString& filename, muint8 UID, mint32 lUID, EXPORT_FILES _fType = EX_FILES_NONE, bool bPatientCoordinate = false, bool appendType = false, bool bEnableProgress = true, QString qstrProgressTitle = "");
	bool saveMeshFilesVisualPrint(QString& filename, muint8 UID, mint32 lUID, EXPORT_FILES _fType = EX_FILES_NONE, bool appendType = true, bool bEnableProgress = true, QString qstrProgressTitle = "");
	bool CheckDupFile(QString& filename, bool onlyCheck = false);
	bool saveSTL(QString& filename, muint32 index, mask _m, int _mI = 0);
	bool saveOBJ(QString& filename, muint32 index, mask _m, int _mI = 0);

	bool viveOn();
	bool saveJSON(const QString& mPath);
	bool saveNII(const QString& mPath, bool _b_use_thread = true, bool patch = false, BoundingBoxI box = BoundingBoxI());
	bool saveROINII(const QString& fileName, mask _m = 0, int _mI = 0);
	bool saveHUNII(const QString& fileName, mask _m = 0, int _mI = 0);
	bool saveTXT(const QString& fileName, muint32 index, mask _m = 0, int _mI = 0);

	bool saveNRRD(const QString& mPath, bool b_use_thread = true, bool patch = false, BoundingBoxI box = BoundingBoxI());

	bool IsLicensePass();
	void SetLicensePass(bool value);

	bool getRenderable() { return m_renderable; }
	void setRenderable(bool value) { m_renderable = value; }

	bool getMoveFocus() { return m_move_focus; }
	void setMoveFocus(bool move) { m_move_focus = move; }

	void setSaveState(bool state) { m_save_file = state; }
	bool getSaveState() { return m_save_file; }

	void setGammaMode(bool val, bool rst = false, bool updateTab = true);
	bool isGammaMode();

	void setConfigValue(QString element, QString node, QString value, int presetIndex = -1);
	bool getConfigValue(QString element, QString node, QString& value, int presetIndex = -1);
	bool getConfigWL(QString node, int&, int&, int presetIndex = -1);
	void setConfigWL(QString node, int, int, int presetIndex = -1);
	bool getConfigPreset(int index, int type, QString& value, bool isColor = true); //color & alpha points
	void setConfigPreset(int index, int type, QString value, bool isColor = true);//color & alpha points

	void getWidthLevelValue(bool, int&, QPoint, QPoint);

	void setSeedShow(int value) { m_bSeedTip = value; }
	int getSeedShow() { return m_bSeedTip; }

	void setThreUpper(int val) { m_Threshold_Upper = val; }
	void setThreLower(int val) { m_Threshold_Lower = val; }
	int getThreUpper() { return m_Threshold_Upper; }
	int getThreLower() { return m_Threshold_Lower; }

	void setDirectionMode(bool val) { m_2D_Direction = val; }
	bool getDirectionMode() { return m_2D_Direction; }

	void setRepositioningMode(bool val) { m_bRepositioning = val; }
	bool getRepositioningMode() { return m_bRepositioning; }

	void setBackfaceCullingMode(bool val) { m_bBackfaceCulling = val; }
	bool getBackfaceCullingMode() { return m_bBackfaceCulling; }

	void setDrawcutMode();
	void setDelMaskView(int index, bool isClear = false);

	muint32 getRawSize(QString fileName);

	short Get16(int index);

	void setPlaymode(PLAYBACK_MODE mode) { m_ani_mode = mode; }
	PLAYBACK_MODE getPlaymode() { return m_ani_mode; }

	WORK_MODE getWorkMode() { return m_workMode; }
	void setWorkMode(WORK_MODE mode) { m_workMode = mode; }

	ANAL_WORK_MODE getAnalWorkMode() { return m_analworkMode; }
	void setAnalWorkMode(ANAL_WORK_MODE mode) { m_analworkMode = mode; }

	THRESHOLD_WORK_MODES getThresholdWorkModes() { return m_thresholdWorkModes; }
	void setThresholdWorkModes(THRESHOLD_WORK_MODES modes) { m_thresholdWorkModes = modes; }
	void setThresholdWorkModeOn(THRESHOLD_WORK_MODE mode) { m_thresholdWorkModes |= mode; }
	void setThresholdWorkModeOff(THRESHOLD_WORK_MODE mode) { m_thresholdWorkModes &= ~mode; }
	bool isTheadholdWorkModeOn(THRESHOLD_WORK_MODE mode) { return ((m_thresholdWorkModes & mode) != 0); }

	RADIOMICS_WORK_MODE getRadiomicsWorkMode() { return m_radiomicsWorkMode; }
	void setRadiomicsWorkMode(RADIOMICS_WORK_MODE mode) { m_radiomicsWorkMode = mode; }

	void setVisualPrintWorkMode(VISUAL_PRINT_WORK_MODE mode) { m_VisualPrintWorkMode = mode; }
	VISUAL_PRINT_WORK_MODE getVisualPrintWorkMode() { return m_VisualPrintWorkMode; }

	void makeCurrent();
	void doneCurrent();
	void getOpenGLError(std::string str);
	void MEViewRenderLater();
	void setDCutShape(DRAW_CUT_SHAPE value);
	DRAW_CUT_SHAPE getDCutShape(bool Tabtype = false);

	QString getWorkModeString(bool view3D = false, bool measure = false);
	QString getUnitString(); //HU or SI return

	void setCoordType(WINDOW_COORD_TYPE val) { m_coord_type = val; }
	WINDOW_COORD_TYPE getCoordType() { return m_coord_type; }
	void setFlipMode();

	muint32 createTextureFromQImage(QImage* image);
	bool setFontTexture(QImage* image);
	muint32 getFontTexture() { return m_nTexFont; }

	void setWindow2D_PostProcess_Filter(WINDOW2D_POSTPROCESS_FILTER fileter) { m_window2D_postprocess_filter = fileter; }
	WINDOW2D_POSTPROCESS_FILTER getWindow2D_PostProcess_Filter() { return m_window2D_postprocess_filter; }

	void setDrawingSeed(bool isLeft, mask _m, int _mI);
	mask getDrawingSeedMask(bool isLeft);
	int getDrawingSeedIndex(bool isLeft);

	bool ChangeUID(mint32 pre_mask_uid, mint32 mask_uid);
	bool ChangeUID_Ext(VOLUME_DATA* pVolumeData, mint32 pre_mask_uid, mint32 mask_uid);
	void setTempReportData(const QString& fileName, const std::vector<std::pair<QImage, QString>>& image_list, const QString& html);

	void updatePckModel(int _idx);
	void updateMeshTablist(int _idx, bool _clear = true);
	void updateMeshTablist();
	void UpdateAllMeshTabList_NotPicked();
	void UpdateAllMeshTabList();
	void UpdateMeshTabByInfo(const MeshInfo& info);
	void clearMeshTabSelection();
	void clearMeshTabList();
	//////////////////////////////////////////////////////////////////////////////////////////////

	// Visual Print
	//////////////////////////////////////////////////////////////////////////////////////////////
	//	bool VisualPrintingSTLUpload(QString & filename);
	void VisualPrintingWebLink(DataContext* pDataContext);
	void VisualPrintFromMeshList();
	void setVisualPrintParams(int _val_smooth, int _val_reduce);
	void setVisualPrintSmoothVal(int _val_smooth);
	void setVisualPrintReduceVal(int _val_reduce);
	void updateVisualPrintPckModel(int _index);
	//////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////////
	void enableMeshList(bool _b_enable);
	void enableROIList(bool _b_enable);
	//////////////////////////////////////////////////////////////////////////////////////////////

	// 20201112_byPHS - Dialog Reject
	void rejectMEViewDlg(int old_idx = -1);
	void showMeshDlg(int mode);
	void MoveMeshDialog(int fixed = 0); //default - 0, fixed Right - 1 , fixed Left - 2

	void initMedipUIData(int width, int level);
	void updateVolumeTextureGL(short*);
	void getAnnotationColor();

	void setSeedColor(QColor, int);
	void setSeedColor(COLOR, int);
	COLOR getSeedColor(int);
	COLOR getDrawingSeedColor(bool isLeft);

	bool loadCroppingDicomFile(QString& filename);
	bool loadCroppingDicomData(const mint16* pHUdata, const DcmtkSeriesInfo& dcmSeriesInfo, const DicomVolumeInfo& dcmVolumeInfo);
	void invisibleModeUpdate(WINDOW_TYPE type);
	void updatePlaneData_all();
	bool IsCropOn() const;
	void setCropOnOff(bool val);

	void DrawCutOnOff(bool val);
	void volumeLayerAlphaSet();
	void setVolumeAlphaSlider(int val);
	void setLayerAlphaSlider(int val);
	bool isClipMode();
	void clipOnOff(bool val);
	void clip2DOff();
	void OnVRMode();
	void setThumbnail(QImage img);
	QImage getThumbnail();

	void set3DAnnoState(bool set);
	bool get3DAnnoState();

	SLICE_PRESET getSelectedPreset();
	SLICE_PRESET getSelectedVolumePreset();
	int getSelectedCustomPreset(int type);
	void setPresetName(QString name, int index, bool isImport = false, bool bAppDataPath = false);
	bool delPresetName(int index, bool bAppDataPath = false);
	QString getPresetName(int index);
	eConfigFileLocation getPresetPathType(int index);
	int	getPresetCount();

	bool getRGLock();
	void setRGUnlock();

	bool getRGDrawLock(bool _del = false);
	void setRGDrawUnlock();

	void updateTAState(bool, int);

	void setUndoLimit(int);
	int	getUndoLimit();

	void setDPIValue(double);
	double getDPIValue();

	bool IsAIOutsetMaskSelected(int* outValue = nullptr);

	void FreezeProject_InAISegProcessing(bool isFreeze);

	void SelectSliceNum(SELECT_TYPE eType, QString strSliceNum = "L3");
	void FillSpecificSliceMask(int nUID, std::vector<int> vecFillNum);
	void FillSpecificSliceMask(QString strMaskName, std::vector<int> vecFillNum);
	void CopyAbdominalSkin(QString strOriginMask, QString strCopyMask, int nSliceNum);

	void setSegmentationWorkMode(WORK_MODE mode);
	void setMeasurementWorkMode(ANAL_WORK_MODE mode);

	void setBoundingBoxEdge(bool value);
	void setShowBoundingBox(bool value);

	void setTargetedDrawCutMode(bool bMode);
	bool getTargetedDrawCutMode();

	bool ShouldCheckSaveStatus();

	QString ConvertText_VoxelPosToSliceImagePos_WithTotalSliceImageCount(int x, int y, int z);
	QString ConvertText_VoxelPosToSliceImagePos(int x, int y, int z);

	void setExport3DMeshMethod(EXPORT_3D_SURFACE_MESH_METHOD method);
	EXPORT_3D_SURFACE_MESH_METHOD  getExport3DMeshMethod();

	mip::Renderer* GetCurrentRenderer() const;

	void setHeatMapMode(bool bCheck);
	bool isHeatMapMode();
	bool isHeatMapEnabled();

	TabWindow* GetTab() const;
	void SetTab(TabWindow* pTabWindow);

	bool IsPredictComplete() const;
	void SetPredictComplete(bool value);

	void SetProductManager(ProductManager* pProductManager);
	void SetMessageBox(std::unique_ptr<MessageBoxBase> pMessageBox);

	DcmtkSeriesInfo* GetDicomInfo(DcmtkSeriesInfo* infoList = nullptr);
	DcmtkSeriesInfo* GetDicomInfo_PET(DcmtkSeriesInfo* infoList = nullptr);

private:
	bool buildup3DVolumeTexture(progUpdatefunc update = nullptr, void* data = nullptr);
	bool buildup3DMaskTexture(progUpdatefunc update = nullptr, void* data = nullptr);
	bool buildup2DMPR(progUpdatefunc update = nullptr, void* data = nullptr);
	bool buildupLowSpecVolume(void* data = nullptr);

	bool createLowSpecTexture(WINDOW_TYPE type, muint32& texLowVolume, int width, int height);
	void* create3DMaskTexture(progUpdatefunc update = nullptr, void* data = nullptr, int type = mip::TEXF_R8UI);
	bool setVolumeDataToCropWindow(std::wstring& dir_path, std::vector<SliceInfo>& fileslice, DicomVolumeInfo& dcm_volume_info);

	bool buildup3DVolumeBuffer(void);

#ifdef _M_IX86
	void renderVolumeVTKData();
#endif
	void renderVolumeVTK();
	void initDicomInfo(DcmtkSeriesInfo* target, DcmtkSeriesInfo* source);
	int readRawFile(unsigned char* image, int size, QString& filename);

public:
	MedipQT* mainWindow;

	MAINTAB_TYPE mainTabType;
	QWidget* mainCenterWidget;
	QHBoxLayout* mainHLayout;

	MainSegmentWidget* mainSegmentWidget;
	MainMeshWidget* mainMeshWidget;
	MainVisualPrintMeshWidget* pVisualPrintMeshWidget;
	MainAnalWidget* mainAnalWidget;
	ReportWidget* mainReportWidget;
	MainTAWidget* mainTAWidget;

	QStatusBar* statusBar;
	mip::VECTOR3 analSlideOffset[3];
	mip::VECTOR3 analSlideOffsetSub[3];  // Mesh 2D Outline용(220926 허 건 과장)

	WINDOW_TYPE tempType;
	TempReportData tempReportData;

	QOpenGLContext* context;

	mip::VolumeRenderer volume_renderer;

	unsigned int AngularSize_S;
	unsigned int AngularSize_A;

	unsigned int CirculSize_S;
	unsigned int CirculSize_A;

	bool pencil_S;
	bool pencil_A;

	bool oval_S;
	bool oval_A;

	std::string directoryPath;

	mint32 regionGrowingX;
	mint32 regionGrowingY;
	mint32 regionGrowingZ;
	mint16 regionGrowingHu;

	QVector<Annotation*> anotationList;
	QVector<QImage*> captureList;
	QVector<QImage*> imgList;
	QVector<int> exportList; //roi uid list
	QVector<int> roiPreviewSurfaceList; //roi preview surface list for Visual Printing with Mesh Edit
	QVector<int> VisualPrint_roiPreviewSurfaceList; //roi preview surface list for Visual Printing with Mesh Edit
	QVector<QString> roiSaveMeshList;//roi save mesh list for Visual Printing with Mesh Edit
	QVector<QString> roiUploadList;	//roi upload stl file list for Visual Printing
	QVector<QString> roiDeleteList;	//delete roi list after roi upload for Visual Printing	
	tstring tstrUploadIDPath;	// ID Path Name member variable for Visual Printing File Upload
	tstring tstrUploadDateTimePath;	// DateTime Path Name member variable for Visual Printing File Upload
	QString qstrURL_LinkPath; // DateTime Path Name member variable for Visual Printing Link Web 3D
	QVector<QString> importList;

	QString exportPath; //exclude file name

	/*QMap<mint32, mip::MeshTopology*>	meshMap;
	std::vector<mip::MeshInfo*> meshList;*/
	QVector<QVector3D> seedLocation;
	QVector<QVector3D> layerRGLocation;

	mip::VECTOR3 matMoveCameraPos;

	QVector<QVector3D> aniLine;
	QVector3D LinePos;
	QVector<QVector3D> spiralLine;
	QVector<int> aniCount;
	QMap<int, int> spiralPos; //match to aniLine
	QVector3D patchyPoint;
	bool aniMove;
	bool bStartAni; /*play animation*/
	bool bCreateAni; /*create animation point check*/
	bool bShowAniPoint;
	bool bShowAnnoText;
	bool bShowAnnoLength;
	bool bShowAnnoAngle;
	bool bShowAnnoOval;
	bool bShowAnnoArrow;
	bool bShowAnnoRectangle;
	bool bShowAnnoName;

	bool bCaptureDel;
	bool bImportDel;
	WINDOW_TYPE AniType;

	QTimer* displayTimer;

	bool isLayerFlip;
	std::vector<mip::VECTOR3> vecAngleSplitPoint;	// segmentation탭 임시 point viewing -world 좌표계

#ifdef DEV_VER
	QVector<QVector3D> boundPoints;
#endif
#ifdef DEV_VER
	bool bHighlight;
	bool bRGPreview;
#endif
	bool bThrePreview;
	bool bExit;
	bool bExitForceLogoutCase;
	QColor annoColor;
	ImageManagementTab* imgTabList;

	// radiomics heatmap
	float* pRadiomics3DVolume = nullptr;	// -1~1 
	std::vector<std::pair<float, float>> vecRadiomicsColorCategory;
	bool bRadiomicsDataChangeFlag[3]; // 
	// AI seg set heatmap
	float* pAISegRawResult = nullptr;

	int	ScreenWidth = 0;
	int	ScreenHeight = 0;

	bool bVisibleCoordinate = true;

#if defined(DEEPCATCH_VER_MACRO)

	QString m_strMacroInputPath = "";
	QString m_strMacroOutputPath = "";
#endif

private:
	ProductManager* m_pProductManager;
	ActionManager* m_pActionManager;
	TabWindow* m_tabWindow;
	bool m_isPredictComplete = false;

	std::shared_ptr<WindowManagerFileLoader> m_pFileLoader;

	DataContext* m_pDataContext;
	mip::Renderer* m_pRenderer;
	CMeshViewBtn3DScene* m_pMeshViewBtn3DScene;
	CMeshModelViewManager* m_pMeshModelViewManager;

	std::unique_ptr<MessageBoxBase> m_pMessageBox;

	LAYER_RG_SHORTCUT m_ShortcutRG;
	DRAW_CUT_SHAPE m_DCutShape;
	eModalityType m_modality;
	int m_Layer2DAlpha;
	QImage m_Thumbnail;
	int m_renderType;

	WINDOW_TYPE m_latest_active_window_type;
	uint m_latest_active_window_depth;
	float m_latest_active_window_zoomFactorX;
	float m_latest_active_window_zoomFactorY;

	int m_window_level;
	int m_window_width;

	int m_volume_level;
	int m_volume_width;
	SHADER_QUALITY m_shader_quality;
	bool m_showMesh;
	bool m_clipMesh;
	bool m_showMaskVol;
	bool m_clipMaskVol;
	bool m_wheelZoom;
	bool m_color2D;
	bool m_invertingLUT;
	bool m_clip2DPlane;

	bool m_BoundaryMode;
	bool m_showViewAxises;

	bool m_showClippingBox;

	bool m_save_file;

	bool m_renderable;
	muint32 m_nTexVolume;
	muint32 m_nTexLowVolumeAxial;
	muint32 m_nTexLowVolumeCoronal;
	muint32 m_nTexLowVolumeSagittal;
	muint32 m_nTexMask;
	muint32 m_nTexTF;
	muint32 m_nTexTFAlpha;

	muint32 m_nTex2DAxial;
	muint32 m_nTex2DSagittal;
	muint32 m_nTex2DCoronal;
	muint32 m_nTex2DTF;
	muint32 m_nTexFont;

	muint32 m_nDefaultWidth;
	mint32 m_nDefaultLevel;
	bool m_isLicensePass;
	bool m_move_focus;

	bool m_3d_anno;

	int m_bSeedTip;

	bool m_VolumeActivate;
	bool m_maskingmode; ////false : all(default), true : selected layer
	float m_VolumeAlpha;
	float m_Gamma;
	float m_LayerAlpha;

	int  m_BrushThickness;
	int  m_BrushOpacity;

	WORK_MODE m_workMode;
	ANAL_WORK_MODE m_analworkMode;
	THRESHOLD_WORK_MODES m_thresholdWorkModes;
	RADIOMICS_WORK_MODE	m_radiomicsWorkMode;
	//MESH_WORK_MODE m_meshworkMode;
	VISUAL_PRINT_WORK_MODE m_VisualPrintWorkMode;
	WINDOW_COORD_TYPE m_coord_type;
	WINDOW2D_POSTPROCESS_FILTER m_window2D_postprocess_filter;

	bool m_bOnCropping;

	bool m_edge_mode; /*false: (non-edge), true: edge*/

	int m_Threshold_Upper;
	int m_Threshold_Lower;

	bool m_2D_Direction;
	bool m_bRepositioning;
	bool m_bBackfaceCulling;

	bool m_use_Gamma;
	PLAYBACK_MODE m_ani_mode;
	int m_undoLimit;

	QVector<QString> m_presetNameList;
	QVector<eConfigFileLocation> m_presetPathTypeList;

	QMutex m_seedDrawLock;
	QMutex m_seedLock;
	COLOR m_seedColor[2]; /*0 : backseed, 1: foreseed*/

	UIDMask m_drawingSeed[2]; /*0 : right, 1: left*/ //use <mask index, mask> 
	double m_DPI; //VR Controller DPI

	bool m_bEnableViewControls = true;
	bool m_bTargetedDrawCut;

	EXPORT_3D_SURFACE_MESH_METHOD m_export3DMeshMethod;
};

//============================================================
//	 Utility Function. 
//============================================================
void copyVolumeGeometryInfo(VolumeView* pFrom, AnalVolumeView* pTo);
void copyVolumeGeometryInfo(AnalVolumeView* pFrom, AnalVolumeView* pTo);
void copyVolumeGeometryInfo(AnalVolumeView* pFrom, VolumeView* pTo);

void copy_Radiomics_To_Segmentation_MPRPlaneGeometryInfos(WindowManager* pWinManager, DataContext* pDataContext, MainTAWidget* pFrom, MainSegmentWidget* pTo);
void copy_Segmentation_To_Radiomics_MPRPlaneGeometryInfos(WindowManager* pWinManager, DataContext* pDataContext, MainSegmentWidget* pFrom, MainTAWidget* pTo);
void copyMPRPlaneGeometryInfo(WindowManager* pWinManager, WindowBase* pFrom, WindowBase* pTo);

#define WIN_MANAGER (WindowManager::getSingleton())
#define g_Renderer (WindowManager::GetSingletonRenderer())