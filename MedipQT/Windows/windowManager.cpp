#include "stdafx.h"
#include "windowManager.h"
#include "WindowBase.h"
#include "VolumeView.h"
#include "Main/MainSegmentWidget.h"
#include "Main/MainMeshWidget.h"
#include "Main/MainAnalWidget.h"
#include "Main/MainTAWidget.h"
#include "Actions/ActionManager.h"

#include "System/stringManager.h"
#include "System/styleManager.h"
#include "System/ShortcutManager.h"
#include "System/VolumeCalculator.h"
#include "System/FileManager.h"
#include "System/File/FileWriterMesh.h"
#include "System/ProductManager.h"

#include "Renderer/Mesh.h"
#include "Renderer/model.h"

#include "mip/Log.h"
#include "mipEngine/Camera.h"

#include "Windows/VolumeView.h"
#include "Windows/SegmentationView.h"
#include "Windows/AnalVolumeView.h"
#include "Windows/AnalMPRPlaneView.h"
#include "Windows/MEVolumeView.h"
#include "Windows/RadiomicsView.h"
#include "Windows/Tab/MeshTab.h"
#include "Windows/Tab/AISegTabDeepCatch.h"
#include "Windows/File/WindowManagerFileLoader.h"

#include "Renderer/Renderer.h"
#include "Renderer/VolumeComputer.h"

#include "Dialogs/DicomSeriesSelectDialog.h"
#include "Dialogs/CroppingDialog.h"
#include "Dialogs/ClinicalInformationDlg.h"

#include "UI/AlphaColorMap.h"
#include "UI/HoverPoints.h"
#include "UI/CustomHistogram.h"

#include "Tabwindow.h"

#include "MedipQT.h"

#include "graphics/CRSpline.h"

#include "LicenseManager.h"
#include "Network/Network.h"

#include "medipmipencoder.h"
#include "ActionImageIsotropic.h"

#include "UI/VisualPrint/VisualPrintMeshListWidget.h"

#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshDlgManager.h"
#include "MeshEdit/CMeshModelViewManager.h"
#include "MeshEdit/CMeshViewBtn3DScene.h"

#include "DataContext.h"

#include "3MF/C3MFLoader.h"

#include <strsafe.h>

WindowManager* WindowManager::getSingleton()
{
	static WindowManager instance;
	return &instance;
}

mip::Renderer* WindowManager::GetSingletonRenderer()
{
	static mip::Renderer m_render;
	return &m_render;
}

WindowManager::WindowManager() :
	WindowManager(DATA_CONTEXT, ACTION_MANAGER, g_Renderer)
{
}

WindowManager::WindowManager(DataContext* pDataContext, ActionManager* pActionManager, mip::Renderer* pRenderer) :
	m_pDataContext(pDataContext),
	m_pActionManager(pActionManager),
	m_pRenderer(pRenderer),
	m_undoBasic(30),
	m_BrushThickness(1),
	m_BrushOpacity(70),
	mainWindow(nullptr),
	mainSegmentWidget(nullptr),
	mainMeshWidget(nullptr),
	pVisualPrintMeshWidget(nullptr),
	mainAnalWidget(nullptr),
	mainReportWidget(nullptr),
	mainTAWidget(nullptr)
{
	m_pProductManager = PRODUCT_MANAGER;
	m_pMeshViewBtn3DScene = MESH_BTN_SCENE_MANAGER;
	m_pMeshModelViewManager = MESH_MODELVIEW_MANAGER;

	m_pFileLoader = std::make_shared<WindowManagerFileLoader>(this);
	m_pMessageBox = MessageBoxBase::NewForGUI();

	m_nTexVolume = 0;
	m_nTexMask = 0;
	m_nTexTF = 0;
	m_nTex2DTF = 0;
	m_nTexTFAlpha = 0;
	m_nTexFont = 0;

	m_nTexLowVolumeAxial = 0;
	m_nTexLowVolumeCoronal = 0;
	m_nTexLowVolumeSagittal = 0;
	bExit = false;
	bExitForceLogoutCase = false;
	m_ShortcutRG = LAYER_RG_NONE;
	m_nTex2DAxial = 0;
	m_nTex2DCoronal = 0;
	m_nTex2DSagittal = 0;
#ifdef DEV_VER
	bHighlight = false;
#endif
	patchyPoint = QVector3D(-1, -1, -1);
	bThrePreview = false;
	m_ani_mode = SINGLE_MODE;
	//	device = nullptr;
	context = nullptr;
	//vtkBoxWidget *box = vtkBoxWidget::New();

	m_window_level = 41;
	m_window_width = 85;
	CirculSize_A = CirculSize_S = AngularSize_A = AngularSize_S = 10;
	pencil_S = pencil_A = false;
	m_latest_active_window_type = WT_AXIAL;
	m_latest_active_window_depth = 50;
	m_DPI = 5.0f;

	regionGrowingX = -1;
	mainMeshWidget = nullptr;
	pVisualPrintMeshWidget = nullptr;
	mainAnalWidget = nullptr;
	mainReportWidget = nullptr;
	mainTAWidget = nullptr;
	m_tabWindow = nullptr;
	imgTabList = nullptr;
	m_BoundaryMode = false;
	m_showViewAxises = false;
	m_showClippingBox = false;
	m_showMesh = true;
	m_clipMesh = true;
	m_showMaskVol = true;
	m_clipMaskVol = true;

	m_clip2DPlane = false;
	bCaptureDel = false;
	bImportDel = false;

	m_renderType = RT_VOLUME_MASK_SURFACE;

	m_save_file = true;

	m_isLicensePass = false;
	m_renderable = false;
	m_move_focus = true;
	bStartAni = false;
	bCreateAni = false;
	m_bSeedTip = RT_SHOW;
	bShowAniPoint = false;
	bShowAnnoText = true;
	bShowAnnoLength = true;
	bShowAnnoAngle = true;
	bShowAnnoOval = true;
	bShowAnnoArrow = true;
	bShowAnnoRectangle = true;
	bShowAnnoName = true;
	isLayerFlip = false;

	m_maskingmode = false;
	m_VolumeAlpha = 100;
	m_LayerAlpha = 200;
	m_workMode = WORK_NONE;
	m_analworkMode = ANAL_WORK_NONE;
	m_thresholdWorkModes = THRESHOLD_WORK_NONE;
	//m_meshworkMode = MESH_WORK_NONE;
	annoColor = QColor(255, 0, 0);
	m_seedColor[0] = COLOR(255, 0, 0, 200);
	m_seedColor[1] = COLOR(0, 0, 255, 200);
	m_coord_type = INVALID_TYPE;
	m_window2D_postprocess_filter = W2PPF_NONE;
	m_undoLimit = m_undoBasic;
	m_2D_Direction = false;
	m_DCutShape = DRAW_CUT_SHAPE_ANGULAR;

	QScreen* screen = QGuiApplication::primaryScreen();
	QRect  screenGeometry = screen->geometry();
	ScreenWidth = screenGeometry.width();
	ScreenHeight = screenGeometry.height();

	bVisibleCoordinate = true;

	QString strVal;

	if (!getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_gamma, strVal))
	{
		m_use_Gamma = false;
		m_Gamma = 1.0f;
	}
	else if (0.0f >= strVal.toFloat())
	{
		m_use_Gamma = false;
		m_Gamma = 1.0f;
	}
	else
	{
		m_use_Gamma = true;
		m_Gamma = strVal.toFloat();
	}

	if (!getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_Layer2DAlpha, strVal))
	{
		setLayer2DAlphaValue(200);
	}
	else
	{
		setLayer2DAlphaValue(strVal.toInt());
	}

	if (!getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_wheelZoom, strVal))
		m_wheelZoom = true;
	else if (FALSE == strVal.toInt())
	{
		m_wheelZoom = false;
	}
	else
		m_wheelZoom = true;


	if (!getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_rePos, strVal))
		m_bRepositioning = true;
	else if (-1 == strVal.toInt())
	{
		m_bRepositioning = false;
	}
	else
		m_bRepositioning = true;

	if (!getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_backfaceCulling, strVal))
		m_bBackfaceCulling = true;
	else if (-1 == strVal.toInt())
	{
		m_bBackfaceCulling = false;
	}
	else
		m_bBackfaceCulling = true;

	if (!getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_shader_quality, strVal))
		m_shader_quality = SQ_LOW; // low
	else
	{
		int val = strVal.toInt() + 1;

		m_shader_quality = (SHADER_QUALITY)val;
	}

	if (!getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_3dAnno, strVal))
		m_3d_anno = true;
	else
	{
		int val = strVal.toInt();

		if (val == 0)
			m_3d_anno = false;
		else
			m_3d_anno = true;
	}

	if (!getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_color2D, strVal))
		m_color2D = false;
	else
	{
		int val = strVal.toInt();

		if (val == 0)
			m_color2D = false;
		else
			m_color2D = true;
	}

	if (!getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_invertingLUT, strVal))
		m_invertingLUT = false;
	else
	{
		int val = strVal.toInt();

		if (val == 0)
			m_invertingLUT = false;
		else
			m_invertingLUT = true;
	}

	if (getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_VolumeOpacity3D, strVal))
	{
		int val = strVal.toInt();
		m_VolumeAlpha = val;
	}

	m_VolumeActivate = true;
	matMoveCameraPos.set(0, 0, 0);

	for (int i = 0; i < 3; i++)
		analSlideOffset[i] = analSlideOffset[i].Zero;

	m_edge_mode = false;

	for (int i = 0; i < 2; i++)
	{
		m_drawingSeed[i].uid = 0;
		m_drawingSeed[i]._m = VM_MASK1 >> i;
	}//default

	//	displayTimer = nullptr;

	//	fillMaskCount = 0;
	m_bOnCropping = false;
	tempReportData.clear();
	importList.clear();

	//////////////////////////////////////////////////////////////////////////
	//
	m_bTargetedDrawCut = false;

	bRadiomicsDataChangeFlag[WT_AXIAL - WT_AXIAL] = false;
	bRadiomicsDataChangeFlag[WT_CORONAL - WT_AXIAL] = false;
	bRadiomicsDataChangeFlag[WT_SAGITTAL - WT_AXIAL] = false;

	{
		QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
		QString folder_name = "Temp";
		QString save_path = current_path + "\\" + folder_name;

		if (QDir(save_path).exists())
		{
			QDir(save_path).removeRecursively();
		}

		if (!QDir(save_path).exists())
		{
			QDir().mkdir(save_path);
		}
	}

	m_bEnableViewControls = true;

	if (getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_BrushThickness, strVal))
	{
		int val = strVal.toInt();
		m_BrushThickness = val;
	}

	if (getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_BrushOpacity, strVal))
	{
		int val = strVal.toInt();
		m_BrushOpacity = val;
	}
}

WindowManager::~WindowManager()
{
	resetResource();

	SAFE_DELETE(context);
	//SAFE_DELETE(device);

	QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
	//QString current_path = QDir::currentPath();
	QString folder_name = "Temp";
	QString save_path = current_path + "\\" + folder_name;
	if (QDir(save_path).exists())
	{
		QDir dir(save_path);
		dir.removeRecursively();
	}
}

void WindowManager::resetResource()
{
	makeCurrent();

	resetTextures();
	for (auto a = anotationList.begin(); a != anotationList.end(); ++a)
	{
		SAFE_DELETE((*a));
	}
	anotationList.clear();

	for (auto cp = captureList.begin(); cp != captureList.end(); cp++)
	{
		SAFE_DELETE(*cp);
	}

	captureList.clear();

	doneCurrent();
}

void WindowManager::updateVolumeTextureGL(short* BuffData)
{
	m_pRenderer->updateVolumeTextureGL(m_nTexVolume, BuffData);
}

void WindowManager::getAnnotationColor()
{
	static int index = 1;
	static COLOR colorTable[] = { COLOR(170,0,0,255), COLOR(66,255,255,255), COLOR(59,255,56,255),
		COLOR(255,33,244,255), COLOR(165,127,231,255), COLOR(214,117,130,255),
		COLOR(51,135,214,255), COLOR(214,168,127,255) };

	if (index >= 8)
		index = index % 8;
	annoColor = QColor(colorTable[index].r, colorTable[index].g,
		colorTable[index].b);

	index++;
}
bool WindowManager::setVolumeDataToCropWindow(std::wstring& dir_path, std::vector<SliceInfo>& fileslice, DicomVolumeInfo& dcm_volume_info)
{
	setCropOnOff(true);

	CroppingDialog dlgCropping(&m_pDataContext->volume_data, mainWindow);

	try
	{
		dlgCropping.initData(dcm_volume_info);
	}
	catch (...)
	{
		return false;
	}

	m_pActionManager->action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_DICOM_FILE));
	m_pActionManager->action_ProgressUpdate(0);
	m_pActionManager->clear();

	bool bLoadDCMData = false;
	if (fileslice.size() > 0)//&& pDCMVolumeInfo->bytesPerVoxel == 2)
	{
		bLoadDCMData = mip::DcmtkVolumeReader::LoadVolumeData(
			fileslice, dir_path, dcm_volume_info, DCM_VOLUME_DATA_TYPE_NORMAL, 0.1f, 1,
			dlgCropping.getHUDataPoint(), 0, m_pActionManager->action_ProgressDlgGet(), 100);
	}

	m_pActionManager->action_ProgressEnd();

	mip::DcmtkVolumeReader::UnloadDicomFiles();
	if (bLoadDCMData == false)
	{
		m_pMessageBox->warning(mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1014));
		return false;
	}

	dlgCropping.initWidget();
	//croppingWindow->show();
	int result = dlgCropping.exec();
	if (result == QDialog::Accepted)
	{
		setCropOnOff(false);
	}

	return true;
}

void WindowManager::initMedipUIData(int width, int level)
{
	VOLUME_DATA* pVolume = &m_pDataContext->volume_data;
	if (pVolume)
	{
		pVolume->updateVolumeMinMax();
		pVolume->createMaskInfo();
		applyVoxelToUI(0, true);
		pVolume->forceUpdateVolume();
		pVolume->forceUpdateMaskVolume();
	}

	int HuMin = pVolume->getHuMin();
	int HuMax = pVolume->getHuMax();

	if (!(HuMin <= level && HuMax >= level))
	{
		level = (HuMin + HuMax) / 2;
	}

	if (width <= 0)
	{
		width = abs(HuMax - HuMin);
	}

	m_pDataContext->m_MeshData.ClearMeshInfo();

	setDefaultLevel(level);
	setWindowLevel(level);
	setVolumeLevel(level);

	setDefaultWidth(width);
	setWindowWidth(width);
	setVolumeWidth(width);

	setPreset(SP_DEFAULT);
	setVolumePreset(SP_DEFAULT);
	setCoordType(INVALID_TYPE);
	clearFeatureList();
	resetUI();
	resetResource();

	m_pActionManager->clear();

	if (mainSegmentWidget)
	{
		mainSegmentWidget->resetResource();
		mainSegmentWidget->setWorkMode(WORK_NONE);
		mainSegmentWidget->createHUHisto();
		mainSegmentWidget->SetWidthLine();
		mainSegmentWidget->SetWindowRGSlider();
		mainSegmentWidget->setBeginScreen();
	}

	if (pVolume)
	{
		saveOrginVolumeData();
	}

	if (mainAnalWidget)
	{
		mainAnalWidget->setBeginScreen();
	}

	if (mainTAWidget)
	{
		mainTAWidget->resetResource();
		mainTAWidget->setWorkMode(RADIOMICS_WORK_NONE);
		mainTAWidget->setBeginScreen();
	}

	if (mainReportWidget)
	{
		mainReportWidget->setPatientData();
	}

	if (mainMeshWidget)
	{
		mainMeshWidget->loadMesh(m_pDataContext->m_MeshData.GetCurrentMeshIndex(), true);
	}

	if (pVisualPrintMeshWidget)
	{
		pVisualPrintMeshWidget->loadMesh(m_pDataContext->m_MeshData.GetCurrentMeshIndex(), true);
	}

	aniLine.clear();
	aniCount.clear();
	aniCount.push_back(0);
	patchyPoint = QVector3D(-1, -1, -1);
	if (m_tabWindow)
	{
		resetRGValue();
		updateSeed();
		updateAnnotation();
		updateMeshUI();
		clearFeatureList();
		setPatchyPoint();
		setVolumeAlphaSlider(getVolumeAlpha(true));
		setLayerAlphaSlider(getLayerAlpha(true));
		setAIRange();
		setAIDepth(m_pDataContext->volume_data.getBoundingBox());
	}

	//NON GPU일 때 볼륨 알파값 0.1로 고정
	if (m_pRenderer->isAvailableVolumeRender() == false)
	{
		resetVolumeAlpha();
	}

	setSaveState(false);

	//MEDIP Volume 정보가 새로 Load되었을 떄 Volume_Calculator 업데이트
	VOLUME_CALCULATOR->Initialize(&m_pDataContext->volume_data);

	renderLater_SubView();
	renderLater_GridView(true);
}

void WindowManager::renderVolumeVTK()
{
#if 0
	vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	/*vtkSmartPointer<vtkRenderWindow> renderWindowVTK;
	vtkSmartPointer<vtkRenderer> rendererVTK;*/
	renderWindowVTK = vtkSmartPointer<vtkRenderWindow>::New();
	rendererVTK = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> interactorStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	vtkSmartPointer<vtkPiecewiseFunction> gradientOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	vtkSmartPointer<vtkPiecewiseFunction> scalarOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	vtkSmartPointer<vtkColorTransferFunction> color = vtkSmartPointer<vtkColorTransferFunction>::New();
	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();

	//HWND hwnd = (HWND)pMainWidget->winId();
	/*HWND hwnd = (HWND)m_pVTKWidget->winId();
	renderWindowVTK->SetParentId(hwnd);*/
	//renderWindow->SetParentId(pMainWidget->windowHandle());

	QString s = "c:\\Qt\\test_dicom";
	reader->SetDirectoryName(s.toStdString().c_str());
	reader->Update();
	imageData->ShallowCopy(reader->GetOutput());

	rendererVTK->SetBackground(0.1, 0.2, 0.3);

	renderWindowVTK->SetSize(250, 250);
	renderWindowVTK->SetPosition(120, 120);
	renderWindowVTK->AddRenderer(rendererVTK);

	renderWindowInteractor->SetInteractorStyle(interactorStyle);
	renderWindowInteractor->SetRenderWindow(renderWindowVTK);

	//volumeMapper->SetBlendModeToComposite();
	//volumeMapper->SetRequestedRenderModeToGPU();
	volumeMapper->SetInputData(imageData);

	volumeProperty->ShadeOff();
	//volumeProperty->SetInterpolationTypeToLinear();
	volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

	volumeProperty->SetAmbient(0.1);
	volumeProperty->SetDiffuse(0.9);
	volumeProperty->SetSpecular(0.2);
	volumeProperty->SetSpecularPower(10.0);

	gradientOpacity->AddPoint(0.0, 0.0);
	gradientOpacity->AddPoint(2000.0, 1.0);
	volumeProperty->SetGradientOpacity(gradientOpacity);

	scalarOpacity->AddPoint(-800.0, 0.0);
	scalarOpacity->AddPoint(-750.0, 1.0);
	scalarOpacity->AddPoint(-350.0, 1.0);
	scalarOpacity->AddPoint(-300.0, 0.0);
	scalarOpacity->AddPoint(-200.0, 0.0);
	scalarOpacity->AddPoint(-100.0, 1.0);
	scalarOpacity->AddPoint(1000.0, 0.0);
	scalarOpacity->AddPoint(2750.0, 0.0);
	scalarOpacity->AddPoint(2976.0, 1.0);
	scalarOpacity->AddPoint(3000.0, 0.0);
	volumeProperty->SetScalarOpacity(scalarOpacity);

	color->AddRGBPoint(-750.0, 0.08, 0.05, 0.03);
	color->AddRGBPoint(-350.0, 0.39, 0.25, 0.16);
	color->AddRGBPoint(-200.0, 0.80, 0.80, 0.80);
	color->AddRGBPoint(2750.0, 0.70, 0.70, 0.70);
	color->AddRGBPoint(3000.0, 0.35, 0.35, 0.35);
	volumeProperty->SetColor(color);

	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	rendererVTK->AddVolume(volume);
	rendererVTK->ResetCamera();

	renderWindowVTK->Render();
	//renderWindowVTK->SetWindowName("asdfasdf");
	renderWindowInteractor->Start();
#endif
}

bool WindowManager::buildup3DVolumeBuffer(void)
{
	return true;
}

void WindowManager::setSeedColor(QColor _col, int index)
{
	static QColor initCol[2] = { QColor(255,0,0,200), QColor(0,0,255,200) };
	if (index < 0 || index > 1)
	{
		return;
	}

	if (_col.alpha() == 0)
	{
		setSeedColor(initCol[index], index);
		return;
	}

	m_seedColor[index] = COLOR(_col.red(), _col.green(), _col.blue(), 200);
}

void WindowManager::setSeedColor(COLOR _col, int index)
{
	static COLOR initCol[2] = { COLOR(255,0,0,200), COLOR(0,0,255,200) };

	if (index < 0 || index > 1)
	{
		return;
	}

	if (_col.a == 0)
	{
		setSeedColor(initCol[index], index);
		return;
	}

	m_seedColor[index] = _col;
}

/*@function getSeedColor
 *@brief drawcut seed color return
 *@param index 0 : backseed, 1: foreseed
 *@param return color
*/
COLOR WindowManager::getSeedColor(int index)
{
	if (index < 0 || index > 1)
	{
		return COLOR(255, 0, 0, 200);
	}

	return m_seedColor[index];
}

COLOR WindowManager::getDrawingSeedColor(bool isLeft)
{
	int chkLeft = isLeft ? 0 : 1;
	UIDMask chk;

	chk.uid = 0;

	for (int i = 0; i < 2; i++)
	{
		chk._m = VM_MASK0 << i;

		if (chk.uid == m_drawingSeed[chkLeft].uid &&
			chk._m == m_drawingSeed[chkLeft]._m)
		{
			return getSeedColor(i);
		}
	}


	MaskInfo* info = m_pDataContext->volume_data.findMaskInfo(m_drawingSeed[chkLeft].uid, m_drawingSeed[chkLeft]._m);

	if (info)
	{
		return info->color;
	}

	return COLOR();
}

bool WindowManager::loadCroppingDicomFile(QString& filename)
{
	return m_pFileLoader->loadCroppingDicomFile(filename);
}

bool WindowManager::loadCroppingDicomData(const mint16* pHUdata, const DcmtkSeriesInfo& dcmSeriesInfo, const DicomVolumeInfo& dcmVolumeInfo)
{
	return m_pFileLoader->loadCroppingDicomData(pHUdata, dcmSeriesInfo, dcmVolumeInfo);
}

void WindowManager::invisibleModeUpdate(WINDOW_TYPE type)
{
	if (mainSegmentWidget == nullptr)
	{
		return;
	}

	if (getShaderQuality() == SQ_INVISIBLE || m_pRenderer->isAvailableVolumeRender() == false)
	{
		m_pDataContext->volume_data.forceUpdateVolume();

		mip::VECTOR3 dv;
		mip::AABB box = m_pDataContext->volume_data.getBoundingBox3DAABB();
		int depth = static_cast<int>(mainSegmentWidget->getWindow(type)->getDepth());

		if (type == WT_AXIAL)
		{
			int cz = m_pDataContext->volume_data.getCZ();
			float dz = (box.max.z - box.min.z) / static_cast<float>(m_pDataContext->volume_data.getCZ());

			for each (mip::VECTOR3 & var in m_pDataContext->volume_data.planeVertex[type])
			{
				var.z = dz * static_cast<float>(depth - (cz / 2));
			}
		}
		else if (type == WT_CORONAL)
		{
			int cy = m_pDataContext->volume_data.getCY();
			float dy = (box.max.y - box.min.y) / static_cast<float>(m_pDataContext->volume_data.getCY());

			for each (mip::VECTOR3 & var in m_pDataContext->volume_data.planeVertex[type])
			{
				var.y = dy * static_cast<float>(depth - (cy / 2));
			}
		}
		else if (type == WT_SAGITTAL)
		{
			int cx = m_pDataContext->volume_data.getCX();
			float dx = (box.max.x - box.min.x) / static_cast<float>(m_pDataContext->volume_data.getCX());

			for each (mip::VECTOR3 & var in m_pDataContext->volume_data.planeVertex[type])
			{
				var.x = dx * static_cast<float>(depth - (cx / 2));;
			}
		}
		mainSegmentWidget->getViewVolume()->renderLater();
	}
}

void WindowManager::updatePlaneData_all()
{
	if (mainSegmentWidget)
	{
		mainSegmentWidget->updatePlaneData(WT_AXIAL);
		mainSegmentWidget->updatePlaneData(WT_CORONAL);
		mainSegmentWidget->updatePlaneData(WT_SAGITTAL);
	}

	if (mainTAWidget)
	{
		mainTAWidget->updatePlaneData(WT_AXIAL);
		mainTAWidget->updatePlaneData(WT_CORONAL);
		mainTAWidget->updatePlaneData(WT_SAGITTAL);
	}

	if (mainAnalWidget)
	{
		mainAnalWidget->updatePlaneData(WT_AXIAL);
		mainAnalWidget->updatePlaneData(WT_CORONAL);
		mainAnalWidget->updatePlaneData(WT_SAGITTAL);
	}

	//if (mainTAWidget)
	//{
	//	mainTAWidget->updatePlaneData(WT_AXIAL);
	//	mainTAWidget->updatePlaneData(WT_CORONAL);
	//	mainTAWidget->updatePlaneData(WT_SAGGITAL);
	//}
}

bool WindowManager::IsCropOn() const
{
	return m_bOnCropping;
}

void WindowManager::setCropOnOff(bool val)
{
	m_bOnCropping = val;
}

void WindowManager::DrawCutOnOff(bool val)
{
	DrawcutTab* tab = m_tabWindow->getDrawTab();

	if (tab)
	{
		tab->drawCutOnOff(val);
	}
}

void WindowManager::volumeLayerAlphaSet()
{
	m_tabWindow->get3DTab()->setLayerAlpha(getLayerAlpha(true));
}

void WindowManager::setVolumeAlphaSlider(int val)
{
	m_tabWindow->get3DTab()->setVolumeAlpha(val);
}

void WindowManager::setLayerAlphaSlider(int val)
{
	m_tabWindow->get3DTab()->setLayerAlpha(val);
}

bool WindowManager::isClipMode()
{
	return m_tabWindow->get3DTab()->isClipMode();
}

void WindowManager::clipOnOff(bool val)
{
	m_tabWindow->get3DTab()->clipOnOff(val);
}

void WindowManager::clip2DOff()
{
	if (mainAnalWidget)
	{
		mainAnalWidget->getViewVolume()->clip2DOff();
	}
}

void WindowManager::OnVRMode()
{
	m_tabWindow->get3DTab()->OnVRMode();
}

void WindowManager::setThumbnail(QImage img)
{
	if (img == QImage())
	{
		m_Thumbnail = QImage();
	}

	m_Thumbnail = QImage(100, 100, QImage::Format_RGBA8888);

	QImage tmpImg = img.scaled(QSize(100, 100), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	QPainter p(&m_Thumbnail);
	p.drawImage(0, 0, tmpImg);
	p.end();
}

QImage WindowManager::getThumbnail()
{
	return m_Thumbnail;
}

void WindowManager::set3DAnnoState(bool set)
{
	m_3d_anno = set;

	QString strVal = QString::number(m_3d_anno);

	setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_3dAnno, strVal);

	if (m_pDataContext->volume_data.isValidate())
	{
		renderLater_3DView();
	}
}

bool WindowManager::get3DAnnoState()
{
	return m_3d_anno;
}

SLICE_PRESET WindowManager::getSelectedPreset()
{
	if (m_tabWindow)
	{
		Visualize2DTab* tab = m_tabWindow->get2DTab();

		if (tab)
			return tab->getPresetType();
	}

	return SP_DEFAULT;
}

SLICE_PRESET WindowManager::getSelectedVolumePreset()
{
	if (m_tabWindow)
	{
		Visualize3DTab* tab = m_tabWindow->get3DTab();

		if (tab)
			return tab->getPresetType();
	}

	return SP_DEFAULT;
}

int WindowManager::getSelectedCustomPreset(int type)
{
	if (m_tabWindow)
	{
		if (CL_2D == type)
		{
			Visualize2DTab* tab = m_tabWindow->get2DTab();

			if (tab)
				return tab->getCusPresetType();
		}
		else
		{
			Visualize3DTab* tab = m_tabWindow->get3DTab();

			if (tab)
				return tab->getCusPresetType();
		}
	}

	return -1;
}

void WindowManager::setPresetName(QString name, int index, bool isImport, bool bAppDataPath)
{
	if (!IsLicensePass())
	{
		return;
	}

	QFile file;
	if (bAppDataPath)
	{
		file.setFileName(STRING_MANAGER->localPresetFilePath + "/" + name + ".prd");
	}
	else
	{
		file.setFileName(STRING_MANAGER->presetFilePath + "/" + name + ".prd");
	}

	if (isImport)
	{
		if (!file.exists())
		{
			return;
		}
	}
	else
	{
		if (file.exists())
		{
			file.remove();
		}
	}


	if (m_presetNameList.size() <= index)
	{
		m_presetNameList.append(name);
		if (bAppDataPath)
		{
			m_presetPathTypeList.append(eCFLAppDataLocalPath);
		}
		else
		{
			m_presetPathTypeList.append(eCFLApplicationPah);
		}
		HoverPoints::AddPresetAlpha(index, isImport);
		AlphaColorMap::addPreset(index, isImport);

		if (m_pDataContext->volume_data.isValidate())
		{
			updatePresetTexture(SP_DEFAULT, index);
			update2DPresetTexture(SP_DEFAULT, index);
		}

		if (m_tabWindow)
		{
			{
				Visualize2DTab* tab = m_tabWindow->get2DTab();

				if (tab)
					tab->addCustomPreset(index, isImport, bAppDataPath);
			}

			{
				Visualize3DTab* tab = m_tabWindow->get3DTab();

				if (tab)
					tab->addCustomPreset(index, isImport, bAppDataPath);
			}

		}
	}

	if (bAppDataPath)
	{
		QString nameList = "";
		for (int i = 0; i < m_presetNameList.size(); i++)
		{
			if (m_presetPathTypeList.at(i) == eCFLAppDataLocalPath)
			{
				nameList.append(m_presetNameList.at(i));
				if (i != (m_presetNameList.size() - 1))
					nameList.append(":");
			}
		}
		setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_cusPreset_AppData, nameList);
	}
	else
	{
		QString nameList = m_presetNameList.size() != 0 ? m_presetNameList.at(0) : "";
		for (int i = 1; i < m_presetNameList.size(); i++)
		{
			nameList.append(":");
			nameList.append(m_presetNameList.at(i));
		}
		setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_cusPreset, nameList);
	}
}

bool WindowManager::delPresetName(int index, bool bAppDataPath)
{
	if (index < 0 || index >= m_presetNameList.size())
	{
		return false;
	}

	int res;
	if ((res = m_pMessageBox->warning(mainWindow, "Custom preset deletion",
		"Do you also delete the preset file(*.prd)?",
		QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel)) == QMessageBox::Ok)
	{
		QFile file(STRING_MANAGER->presetFilePath + "/" + m_presetNameList.at(index) + ".prd");

		file.remove();
	}

	m_presetNameList.remove(index);
	m_presetPathTypeList.remove(index);

	HoverPoints::DelPresetAlpha(index);
	AlphaColorMap::delPreset(index);

	if (m_pDataContext->volume_data.isValidate())
	{
		updatePresetTexture(SP_DEFAULT, index, true);
		update2DPresetTexture(SP_DEFAULT, index, true);
	}

	if (m_tabWindow)
	{
		{
			Visualize2DTab* tab = m_tabWindow->get2DTab();

			if (tab)
				tab->delCustomPreset(index);
		}

		{
			Visualize3DTab* tab = m_tabWindow->get3DTab();

			if (tab)
				tab->delCustomPreset(index);
		}

	}

	if (bAppDataPath)
	{
		QString nameList = "";
		for (int i = 0; i < m_presetNameList.size(); i++)
		{
			if (m_presetPathTypeList.at(i) == eCFLAppDataLocalPath)
			{
				nameList.append(m_presetNameList.at(i));
				if (i != (m_presetNameList.size() - 1))
					nameList.append(":");
			}
		}
		setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_cusPreset_AppData, nameList);
	}
	else
	{
		QString nameList = m_presetNameList.size() != 0 ? m_presetNameList.at(0) : "";
		for (int i = 1; i < m_presetNameList.size(); i++)
		{
			nameList.append(":");
			nameList.append(m_presetNameList.at(i));
		}
		setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_cusPreset, nameList);
	}

	return true;
}

eConfigFileLocation WindowManager::getPresetPathType(int index)
{
	if (index < 0 || index >= m_presetPathTypeList.size())
	{
		return eCFLApplicationPah;
	}

	return m_presetPathTypeList.at(index);
}

int	WindowManager::getPresetCount()
{
	return m_presetNameList.count();
}

QString WindowManager::getPresetName(int index)
{
	if (index < 0 || index >= m_presetNameList.size())
	{
		return QString();
	}

	return m_presetNameList.at(index);
}

bool WindowManager::getRGLock()
{
	return m_seedLock.try_lock();
}

void WindowManager::setRGUnlock()
{
	setRGType(LAYER_RG_NONE);
	m_seedLock.unlock();
}

bool WindowManager::getRGDrawLock(bool _del)
{
	if (!_del)
		return m_seedDrawLock.tryLock(10);

	m_seedDrawLock.lock();

	return true;
}

void WindowManager::setRGDrawUnlock()
{
	m_seedDrawLock.unlock();
}

void WindowManager::updateTAState(bool needUpdate, int uid)
{
	if (m_tabWindow)
	{
		ROITab2* tab = m_tabWindow->getROITab();

		if (tab)
		{
			tab->updateTAState(needUpdate, uid);
		}
	}
}

void WindowManager::setUndoLimit(int val)
{
	m_undoLimit = val;

	setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_undoLimit, QString::number(val));
}

int	WindowManager::getUndoLimit()
{
	return m_undoLimit;
}

void WindowManager::setDPIValue(double val)
{
	if (val == m_DPI)
	{
		return;
	}

	m_DPI = val;
}

double WindowManager::getDPIValue()
{
	return m_DPI;
}

void WindowManager::InitViewControlsState(void)
{
	if (!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(m_pProductManager->m_strMedipDeepCatch)
		|| !(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(m_pProductManager->m_strMedipDeepCatchV2))
	{
		m_isPredictComplete = false;
	}
	else
	{
		m_isPredictComplete = true;
	}

	SetEnableViewControls(m_isPredictComplete);

	if (m_tabWindow)
	{
		AIKernelConversionTab* pKernelConversiontab = m_tabWindow->getAIKernelConversionTab();
		if (pKernelConversiontab)
		{
			pKernelConversiontab->SetVisibleInterpolation(false);
		}

		AILowdoseCTReconstuctionTab* pLowdosCTReconTab = m_tabWindow->getAILowdoseCTReconTab();
		if (pLowdosCTReconTab)
		{
			pLowdosCTReconTab->SetVisibleInterpolation(false);
		}
	}
}

bool WindowManager::IsEnableViewControls()
{
	if (IsAIOutsetMaskSelected())
	{
		return false;
	}
	return m_bEnableViewControls;
}

void WindowManager::SetEnableViewControls(bool bEnable)
{
	/* AI Mask가 Selected되어 있으면 항상 Diable*/
	if (IsAIOutsetMaskSelected())
	{
		bEnable = false;
		if (!LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH) || !LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH_V2))
		{
			if (!m_pDataContext->volume_data.isMaskEditable())
			{
				bEnable = false;
			}
			else
			{
				bEnable = true;
			}
		}
	}

	if (m_bEnableViewControls != bEnable)
	{
		m_bEnableViewControls = bEnable;

		/* Viewer Widget Drawing 기능 Enable/Disable */
		if (mainSegmentWidget)
		{
			VolumeView* vView = dynamic_cast<VolumeView*>(mainSegmentWidget->getViewVolume());
			if (vView)
			{
				vView->enableControls(m_bEnableViewControls);
			}

			mainSegmentWidget->getWindow(WT_AXIAL)->enableControls(m_bEnableViewControls);
			mainSegmentWidget->getWindow(WT_CORONAL)->enableControls(m_bEnableViewControls);
			mainSegmentWidget->getWindow(WT_SAGITTAL)->enableControls(m_bEnableViewControls);

			if (bEnable == false)
			{
				setSegmentationWorkMode(WORK_NONE);
			}
		}

		if (mainAnalWidget)
		{
			AnalVolumeView* vView = mainAnalWidget->getViewVolume();
			if (vView)
			{
				vView->enableControls(m_bEnableViewControls);
			}

			mainAnalWidget->getWindow(WT_AXIAL)->enableControls(m_bEnableViewControls);
			mainAnalWidget->getWindow(WT_CORONAL)->enableControls(m_bEnableViewControls);
			mainAnalWidget->getWindow(WT_SAGITTAL)->enableControls(m_bEnableViewControls);

			if (IsAIOutsetMaskSelected())
			{
				mainAnalWidget->getWindow(WT_AXIAL)->enableAnnoControls(true);
				mainAnalWidget->getWindow(WT_CORONAL)->enableAnnoControls(true);
				mainAnalWidget->getWindow(WT_SAGITTAL)->enableAnnoControls(true);
			}

			if (bEnable == false)
			{
				setMeasurementWorkMode(ANAL_WORK_NONE);
			}
		}

		if (mainTAWidget)
		{
			AnalVolumeView* vView = mainTAWidget->getViewVolume();
			if (vView)
			{
				vView->enableControls(m_bEnableViewControls);
			}

			mainTAWidget->getWindow(WT_AXIAL)->enableControls(m_bEnableViewControls);
			mainTAWidget->getWindow(WT_CORONAL)->enableControls(m_bEnableViewControls);
			mainTAWidget->getWindow(WT_SAGITTAL)->enableControls(m_bEnableViewControls);

			if (bEnable == false)
			{
				setRadiomicsWorkMode(RADIOMICS_WORK_NONE);
			}
		}

		/* Tab Control Widget 기능 Enable/Disable*/
		DrawcutTab* tabDrawTab = m_tabWindow->getDrawTab();
		if (tabDrawTab)
		{
			tabDrawTab->SetContainerEnable(bEnable);
		}

		ThreSholdTab* tabThreshold = m_tabWindow->getThreSholdTab();
		if (tabThreshold)
		{
			tabThreshold->SetContainerEnable(bEnable);
		}

		/* Mask Layer 관련 기능 Enable/Disable*/
		SetEnable_Action(SHORTCUT_MANAGER->Action_ManualDrawing_PolygonSelection(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_ManualDrawing_RegionSelection(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_ManualDrawing_PixelWisedSelection(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_ManualDrawing_OvalSelection(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_ManualDrawing_AngularSelection(), bEnable);

		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Erosion_Left(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Erosion_Right(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Erosion_Anterior(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Erosion_Posterior(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Erosion_Inferior(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Erosion_Superior(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Erosion_6_Connectivity(), bEnable);

		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Dilation_Left(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Dilation_Right(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Dilation_Anterior(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Dilation_Posterior(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Dilation_Inferior(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Dilation_Superior(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Dilation_6_Connectivity(), bEnable);

		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Inverse(), bEnable);

		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_3DHoleFilling(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_2DHoleFilling_AxialPlane(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_2DHoleFilling_CoronalPlane(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_2DHoleFilling_SagittalPlane(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_2DHoleFilling_WholePlane(), bEnable);

		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Move_Mask_Left(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Move_Mask_Right(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Move_Mask_Anterior(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Move_Mask_Posterior(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Move_Mask_Inferior(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_Layer_Move_Mask_Superior(), bEnable);

		SetEnable_Action(SHORTCUT_MANAGER->Action_SemiAutoSeg_DrawCutMode(), bEnable);
		SetEnable_Action(SHORTCUT_MANAGER->Action_SemiAutoSeg_DrawCutApply(), bEnable);

	}
}

bool WindowManager::IsAIOutsetMaskSelected(int* outValue)
{
	// DeepCatch는 예외.
	//if (LICENSE_DATA->getProductType() == PRODUCT_NAME_MEDIP_DEEPCATCH) //PHS_TEST
	//	return false;

	MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(m_pDataContext->volume_data.getCurrentMaskInfoID());
	for (auto it = m_pDataContext->volume_data.m_vecAIOutset.begin(); it != m_pDataContext->volume_data.m_vecAIOutset.end(); ++it)
	{
		if (info && it->first == info->uid)
		{
			if (outValue)
			{
				*outValue = it->second;
			}
			return true;
		}
	}

	return false;
}

void WindowManager::FreezeProject_InAISegProcessing(bool isFreeze)
{
	bool enable = !isFreeze;

	SetEnableViewControls(enable);

	/* ROI Tab 관련 Control Enable/Disable*/
	ROITab2* tabROI = m_tabWindow->getROITab();
	if (tabROI)
	{
		tabROI->SetContainerEnable(enable);
	}

	IMGProcessTab* tabImg = m_tabWindow->getImgTab();
	if (tabImg)
	{
		tabImg->SetContainerEnable(enable);
	}

	VOLProcessTab* tabVol = m_tabWindow->getVolTab();
	if (tabVol)
	{
		tabVol->SetContainerEnable(enable);
	}

	/* AI Tab 관련 Control Enable/Disable*/
	AISegTab* tabAISeg = m_tabWindow->getAITab();
	if (tabAISeg)
	{
		tabAISeg->FreezeProject(isFreeze);
	}
}

void WindowManager::SelectSliceNum(SELECT_TYPE eType, QString strSliceNum)
{
	DEEPCATCH_REPORT_PREDICT_INFO* pPredictedInfo = m_pActionManager->getDeepCatch_PredictedInfo();
	if (pPredictedInfo == nullptr)
	{
		return;
	}

	AISegTabDeepCatch* pDeepCatchtab = (AISegTabDeepCatch*)m_tabWindow->getAITab();
	QString strSliceTopic;

	std::vector<int> vecSliceInput;
	WINDOW_TYPE eWinType = WT_CORONAL;
	if (eType == ST_SINGLE)
	{
		if (pPredictedInfo->nAxialDepth > 0)
		{
			vecSliceInput.push_back(pPredictedInfo->nAxialDepth);
		}
		eWinType = WT_SAGITTAL;
		strSliceTopic = strSliceNum;
	}
	else if (eType == ST_MULTIPLE)
	{
		if (pPredictedInfo->nStartAxialDepth > 0)
		{
			vecSliceInput.push_back(pPredictedInfo->nStartAxialDepth);
		}
		else
		{
			vecSliceInput.push_back(0);
		}

		if (pPredictedInfo->nEndAxialDepth > 0)
		{
			vecSliceInput.push_back(pPredictedInfo->nEndAxialDepth);
		}
		else
		{
			vecSliceInput.push_back(0);
		}

		eDeepcatchAWSelectType eAWSelectType;
		if (pDeepCatchtab)
		{
			eAWSelectType = (eDeepcatchAWSelectType)pDeepCatchtab->getAWPredict();
		}

		if (eAWSelectType == eDAWTSagittal)
		{
			pPredictedInfo->stPredictOpt.AWConfirmType = eDAWTSagittal;
			eWinType = WT_SAGITTAL;
		}
		else if (eAWSelectType == eDAWTCoronal)
		{
			pPredictedInfo->stPredictOpt.AWConfirmType = eDAWTCoronal;
			eWinType = WT_CORONAL;
		}

		if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
		{
			if (pDeepCatchtab != nullptr && pDeepCatchtab->getBoneAnalysis() == eDCVTNone)
				strSliceTopic = STRING_MANAGER->getString(STR_SELECT_SLICE_DIRECTIVE_MULTI) + "(Abdominal Waist)";
			else
				strSliceTopic = STRING_MANAGER->getString(STR_SELECT_SLICE_DIRECTIVE_MULTI) + "(" + strSliceNum + ")";
		}
	}


	SelectSliceDlg ssDlg(eType, eWinType, vecSliceInput, strSliceTopic);

	QString strPosX, strPosY;
	getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_SelectionDlg_PositionX, strPosX);
	getConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_SelectionDlg_PositionY, strPosY);

	ssDlg.move(strPosX.toInt(), strPosY.toInt());

	if (ssDlg.exec() == QDialog::Accepted)
	{
		strPosX = strPosX.number(ssDlg.pos().x());
		strPosY = strPosY.number(ssDlg.pos().y());

		setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_SelectionDlg_PositionX, strPosX);
		setConfigValue(ELEMENT_FILE, STRING_MANAGER->m_config_DeepCatch_SelectionDlg_PositionY, strPosY);

		std::vector<int> vecSliceOut;
		ssDlg.GetSliceNumber(vecSliceOut);

		if (eType == ST_SINGLE && vecSliceOut.size() > 0)
		{
			pPredictedInfo->nAxialDepth = vecSliceOut[0];
			if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
			{
				MaskInfo* pNewMaskInfo = m_pDataContext->volume_data.findMaskInfo(strSliceNum, true);
				if (pNewMaskInfo)
					FillSpecificSliceMask(strSliceNum, vecSliceOut);
				else if (pDeepCatchtab != nullptr && pDeepCatchtab->getBoneAnalysis() == eDCVTNone)
					FillSpecificSliceMask(DEEPCATCH_WHOLEBODY_MASKNAME_L3, vecSliceOut);
			}
			else
			{
				FillSpecificSliceMask(DEEPCATCH_WHOLEBODY_MASKNAME_L3, vecSliceOut);
			}

			if (ssDlg.IsSelectAgain())
			{
				pPredictedInfo->stPredictOpt.singleSliceType = eDSliceSelectUserDefine;
			}
			else
			{
				pPredictedInfo->stPredictOpt.singleSliceType = eDSliceSelectAuto;
			}
			qDebug() << "eL3Auto" << pPredictedInfo->stPredictOpt.singleSliceType;
		}
		else if (eType == ST_MULTIPLE && vecSliceOut.size() > 1)
		{
			if (m_pProductManager->IsAvailableFunctionLevel(MFL_Product_DeepCatch_DeepCatchV2))
			{
				pPredictedInfo->nStartAxialDepth = vecSliceOut[0];
				pPredictedInfo->nEndAxialDepth = vecSliceOut[1];
				MaskInfo* pNewMaskInfo = m_pDataContext->volume_data.findMaskInfo(strSliceNum, true);
				if (pNewMaskInfo)
					FillSpecificSliceMask(strSliceNum, vecSliceOut);
				else if (pDeepCatchtab != nullptr && pDeepCatchtab->getBoneAnalysis() == eDCVTNone)
					FillSpecificSliceMask(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST, vecSliceOut);
			}
			else
			{
				pPredictedInfo->nStartAxialDepth = vecSliceOut[0];
				pPredictedInfo->nEndAxialDepth = vecSliceOut[1];
				FillSpecificSliceMask(DEEPCATCH_WHOLEBODY_MASKNAME_ABDOMINAL_WAIST, vecSliceOut);
			}

			if (ssDlg.IsSelectAgain())
			{
				pPredictedInfo->stPredictOpt.multiSliceType = eDSliceSelectUserDefine;
			}
			else
			{
				pPredictedInfo->stPredictOpt.multiSliceType = eDSliceSelectAuto;
			}
			qDebug() << "eAWAuto" << pPredictedInfo->stPredictOpt.multiSliceType;
		}
	}
}

void WindowManager::FillSpecificSliceMask(QString strMaskName, std::vector<int> vecFillNum)
{
	MaskInfo* pNewMaskInfo = m_pDataContext->volume_data.findMaskInfo(strMaskName, true);

	if (pNewMaskInfo)
	{
		int nByteIdx = m_pDataContext->volume_data.GetMaskByteIndex(pNewMaskInfo->uid);
		mask8 maskBit = m_pDataContext->volume_data.getMask(pNewMaskInfo->uid);

		m_pDataContext->volume_data.clearMaskData(maskBit, nByteIdx);

		int cx, cy, cz;
		cx = m_pDataContext->volume_data.getCX();
		cy = m_pDataContext->volume_data.getCY();
		cz = m_pDataContext->volume_data.getCZ();

		for (int i = 0; i < vecFillNum.size(); i++)
		{
			if (vecFillNum[i] > -1 && vecFillNum[i] < cz)
			{
				int nMaskCount = 0;
				for (int y = 0; y < cy; ++y)
				{
					for (int x = 0; x < cx; ++x)
					{

						m_pDataContext->volume_data.setMaskBit(x, y, cz - 1 - vecFillNum[i], maskBit, nByteIdx);
						++nMaskCount;

					}
				}

				m_pDataContext->volume_data.setVoxelCount(pNewMaskInfo->uid, nMaskCount, true);
				m_pDataContext->volume_data.updateUIDBoundingBox(pNewMaskInfo->uid, true);
				m_pDataContext->volume_data.forceUpdateMaskVolume();

				updateUI();
				renderLater_GridView(true);
				setSaveState(false);
			}
		}
	}
}

void WindowManager::FillSpecificSliceMask(int nUID, std::vector<int> vecFillNum)
{
	MaskInfo* pNewMaskInfo = m_pDataContext->volume_data.getMaskInfo(nUID, true);

	if (pNewMaskInfo)
	{
		int nByteIdx = m_pDataContext->volume_data.GetMaskByteIndex(pNewMaskInfo->uid);
		mask8 maskBit = m_pDataContext->volume_data.getMask(pNewMaskInfo->uid);

		m_pDataContext->volume_data.clearMaskData(maskBit, nByteIdx);

		int cx, cy, cz;
		cx = m_pDataContext->volume_data.getCX();
		cy = m_pDataContext->volume_data.getCY();
		cz = m_pDataContext->volume_data.getCZ();

		for (int i = 0; i < vecFillNum.size(); i++)
		{
			if (vecFillNum[i] > -1 && vecFillNum[i] < cz)
			{
				int nMaskCount = 0;
				for (int y = 0; y < cy; ++y)
				{
					for (int x = 0; x < cx; ++x)
					{

						m_pDataContext->volume_data.setMaskBit(x, y, cz - 1 - vecFillNum[i], maskBit, nByteIdx);
						++nMaskCount;

					}
				}

				m_pDataContext->volume_data.setVoxelCount(pNewMaskInfo->uid, nMaskCount, true);
				m_pDataContext->volume_data.updateUIDBoundingBox(pNewMaskInfo->uid, true);
				m_pDataContext->volume_data.forceUpdateMaskVolume();

				updateUI();
				renderLater_GridView(true);
				setSaveState(false);
			}
		}
	}
}

void WindowManager::CopyAbdominalSkin(QString strOriginMask, QString strCopyMask, int nSliceNum)
{
	MaskInfo* pOriginMaskInfo = m_pDataContext->volume_data.findMaskInfo(strOriginMask, true);
	MaskInfo* pCopyMaskInfo = m_pDataContext->volume_data.findMaskInfo(strCopyMask, true);

	if (pOriginMaskInfo && pCopyMaskInfo)
	{
		int nByteIdxOrigin = m_pDataContext->volume_data.GetMaskByteIndex(pOriginMaskInfo->uid);
		mask8 maskBitOrigin = m_pDataContext->volume_data.getMask(pOriginMaskInfo->uid);

		int nByteIdxCopy = m_pDataContext->volume_data.GetMaskByteIndex(pCopyMaskInfo->uid);
		mask8 maskBitCopy = m_pDataContext->volume_data.getMask(pCopyMaskInfo->uid);

		m_pDataContext->volume_data.clearMaskData(maskBitCopy, nByteIdxCopy);

		int cx, cy, cz;
		cx = m_pDataContext->volume_data.getCX();
		cy = m_pDataContext->volume_data.getCY();
		cz = m_pDataContext->volume_data.getCZ();

		qDebug() << "cz - 1 - nSliceNum" << cz - 1 - nSliceNum;
		qDebug() << "pOriginMaskInfo->uid" << pOriginMaskInfo->uid;
		qDebug() << "pCopyMaskInfo->uid" << pCopyMaskInfo->uid;


		int nMaskCount = 0;
		for (int y = 0; y < cy; ++y)
		{
			for (int x = 0; x < cx; ++x)
			{
				if (m_pDataContext->volume_data.isMaskBit(x, y, cz - 1 - nSliceNum, maskBitOrigin, nByteIdxOrigin))
				{
					m_pDataContext->volume_data.setMaskBit(x, y, cz - 1 - nSliceNum, maskBitCopy, nByteIdxCopy);
					++nMaskCount;
				}
			}
		}

		m_pDataContext->volume_data.setVoxelCount(pCopyMaskInfo->uid, nMaskCount, true);
		m_pDataContext->volume_data.updateUIDBoundingBox(pCopyMaskInfo->uid, true);
		m_pDataContext->volume_data.forceUpdateMaskVolume();

		updateUI();
		renderLater_GridView(true);
		setSaveState(false);
	}

}

void WindowManager::setSegmentationWorkMode(WORK_MODE mode)
{
	if (mainSegmentWidget)
	{
		WORK_MODE prMode = getWorkMode();

		/* 이전과 같은 Mode이면 WORK_NONE모드로 Toggle한다. */
		switch (mode)
		{
		case WORK_POLYROI:
		case WORK_REGION_ROI:
		case WORK_PICKERROI:
		case WORK_OVALROI:
		case WORK_RECTROI:
		case WORK_ANNOTATION_TEXT:
			if (mode == prMode)
			{
				mode = WORK_NONE;
			}
			break;
		}
		((VolumeView*)mainSegmentWidget->getViewVolume())->setWorkMode(mode, true);
		((SegmentationView*)mainSegmentWidget->getWindow(WT_AXIAL))->setWorkMode(mode, true);
		((SegmentationView*)mainSegmentWidget->getWindow(WT_CORONAL))->setWorkMode(mode, true);
		((SegmentationView*)mainSegmentWidget->getWindow(WT_SAGITTAL))->setWorkMode(mode, true);
	}
}

void WindowManager::setMeasurementWorkMode(ANAL_WORK_MODE mode)
{
	if (mainAnalWidget)
	{
		/* 이전과 같은 Mode이면 ANAL_WORK_NONE모드로 Toggle한다. */
		ANAL_WORK_MODE prMode = getAnalWorkMode();

		switch (mode)
		{
		case ANAL_WORK_ANNO_LENGTH:
		case ANAL_WORK_ANNO_TEXT:
		case ANAL_WORK_ANNO_ANGLE:
		case ANAL_WORK_ANNO_ARROW:
		case ANAL_WORK_ANNO_RECTANGLE:
		case ANAL_WORK_ANNO_PROFILE_LINE:
		case ANAL_WORK_SEGMENT_FREEDRAW:
		case ANAL_WORK_SEGMENT_POLY:
		case ANAL_WORK_SEGMENT_PICKER:
		case ANAL_WORK_SEGMENT_RECT:
		case ANAL_WORK_SEGMENT_OVAL:
			if (mode == prMode)
			{
				mode = ANAL_WORK_NONE;
			}
			break;
		}
		mainAnalWidget->getViewVolume()->setWorkMode(mode, true);
		mainAnalWidget->getWindow(WT_AXIAL)->setWorkMode(mode, true);
		mainAnalWidget->getWindow(WT_CORONAL)->setWorkMode(mode, true);
		mainAnalWidget->getWindow(WT_SAGITTAL)->setWorkMode(mode, true);
	}
}

void WindowManager::setBoundingBoxEdge(bool value)
{
	if (mainSegmentWidget)
	{
		mainSegmentWidget->getWindow(WT_AXIAL)->setBoundingBoxEdgeMode(value);
		mainSegmentWidget->getWindow(WT_CORONAL)->setBoundingBoxEdgeMode(value);
		mainSegmentWidget->getWindow(WT_SAGITTAL)->setBoundingBoxEdgeMode(value);
	}

	if (mainTAWidget)
	{
		mainTAWidget->getWindow(WT_AXIAL)->setBoundingBoxEdgeMode(value);
		mainTAWidget->getWindow(WT_CORONAL)->setBoundingBoxEdgeMode(value);
		mainTAWidget->getWindow(WT_SAGITTAL)->setBoundingBoxEdgeMode(value);
	}
}

void WindowManager::setShowBoundingBox(bool value)
{
	if (mainSegmentWidget)
	{
		mainSegmentWidget->getWindow(WT_AXIAL)->setShowBoundingBoxMode(value);
		mainSegmentWidget->getWindow(WT_CORONAL)->setShowBoundingBoxMode(value);
		mainSegmentWidget->getWindow(WT_SAGITTAL)->setShowBoundingBoxMode(value);
	}

	if (mainTAWidget)
	{
		mainTAWidget->getWindow(WT_AXIAL)->setShowBoundingBoxMode(value);
		mainTAWidget->getWindow(WT_CORONAL)->setShowBoundingBoxMode(value);
		mainTAWidget->getWindow(WT_SAGITTAL)->setShowBoundingBoxMode(value);
	}
}

void WindowManager::setTargetedDrawCutMode(bool bMode)
{
	m_bTargetedDrawCut = bMode;
}

bool WindowManager::getTargetedDrawCutMode()
{
	return m_bTargetedDrawCut;
}

bool WindowManager::ShouldCheckSaveStatus()
{
	return !m_pActionManager->m_IsMacroMode && !getSaveState();
}

QString WindowManager::ConvertText_VoxelPosToSliceImagePos_WithTotalSliceImageCount(int x, int y, int z)
{
	// 0~cx-1 => 1~cx  : 범위표시 변경
	int view_x = x + 1;
	int view_y = y + 1;
	int view_z = (m_pDataContext->volume_data.getCZ() - (z + 1)) + 1;
	return QString("%1/%2, %3/%4, %5/%6")
		.arg(view_x).arg(m_pDataContext->volume_data.getCX())
		.arg(view_y).arg(m_pDataContext->volume_data.getCY())
		.arg(view_z).arg(m_pDataContext->volume_data.getCZ());
}

QString WindowManager::ConvertText_VoxelPosToSliceImagePos(int x, int y, int z)
{
	// 0~cx-1 => 1~cx  : 범위표시 변경
	int view_x = x + 1;
	int view_y = y + 1;
	int view_z = (m_pDataContext->volume_data.getCZ() - (z + 1)) + 1;
	return QString("%1, %2, %3")
		.arg(view_x)
		.arg(view_y)
		.arg(view_z);
}

void WindowManager::setExport3DMeshMethod(EXPORT_3D_SURFACE_MESH_METHOD method)
{
	m_export3DMeshMethod = method;
}

EXPORT_3D_SURFACE_MESH_METHOD WindowManager::getExport3DMeshMethod()
{
	return m_export3DMeshMethod;
}

mip::Renderer* WindowManager::GetCurrentRenderer() const
{
	return m_pRenderer;
}

void WindowManager::forceUpdate2DViewData(bool updateImage, bool updateMask)
{
	if (mainSegmentWidget)
	{
		if (updateImage)
		{
			mainSegmentWidget->forceUpdate2DViewData();
		}

		if (updateMask)
		{
			mainSegmentWidget->updateLayerState();
		}
	}

	if (mainAnalWidget)
	{
		if (updateImage)
			m_pDataContext->volume_data.forceUpdateMPR();

		if (updateMask)
		{
		}
	}

	if (mainTAWidget)
	{
		if (updateImage)
		{
			mainTAWidget->getWindow(WT_AXIAL)->forceUpdateSliceColorData();
			mainTAWidget->getWindow(WT_CORONAL)->forceUpdateSliceColorData();
			mainTAWidget->getWindow(WT_SAGITTAL)->forceUpdateSliceColorData();

			mainTAWidget->getWindow(WT_AXIAL)->update();
			mainTAWidget->getWindow(WT_CORONAL)->update();
			mainTAWidget->getWindow(WT_SAGITTAL)->update();
		}

		if (updateMask)
		{
			mainTAWidget->updateLayerState();
		}
	}
}

void WindowManager::forceUpdateRadiomicsView()
{
	if (mainTAWidget)
	{
		mainTAWidget->getWindow(WT_AXIAL)->update();
		mainTAWidget->getWindow(WT_CORONAL)->update();
		mainTAWidget->getWindow(WT_SAGITTAL)->update();
	}
}

void WindowManager::forceUpdateSegmentationWidget()
{
	if (mainSegmentWidget)
	{
		mainSegmentWidget->getWindow(WT_AXIAL)->update();
		mainSegmentWidget->getWindow(WT_CORONAL)->update();
		mainSegmentWidget->getWindow(WT_SAGITTAL)->update();
	}
}

QString WindowManager::getWorkModeString(bool view3D, bool measure)
{
	QString res = "Work mode : ";

	if (measure)
	{
		bool bType = (m_analworkMode == ANAL_WORK_SEGMENT_RECT || getDCutShape(true) == DRAW_CUT_SHAPE_ANGULAR);
		bool bDraw = bType ? pencil_A : oval_A;
		int bSize = bDraw ? 0 : bType ? AngularSize_A : CirculSize_A;

		switch (m_analworkMode)
		{
		case ANAL_WORK_PATH_3D_PLAY:
			return view3D ? res.append("Play Anno-Path Mode") : QString();
			break;
		case ANAL_WORK_PATH:
			return view3D ? res.append("Anno-Path Mode") : QString();
			break;
		case ANAL_WORK_ANNO_LENGTH:
			return res.append("Length Mode");
			break;
		case ANAL_WORK_ANNO_TEXT:
			return res.append("Text Mode");
			break;
		case ANAL_WORK_ANNO_ANGLE:
			return res.append("Angle Mode");
			break;
		case ANAL_WORK_SEGMENT_OVAL:
			return res.append("Oval selection Mode %1").arg(oval_A ? QString("(custom-mode)") : QString("(size:%1)").arg(CirculSize_A));
			break;
		case ANAL_WORK_ANNO_ARROW:
			return res.append("Arrow Mode");
			break;
		case ANAL_WORK_ANNO_RECTANGLE:
			return res.append("Rectangle Mode");
			break;
		case ANAL_WORK_ANNO_PROFILE_LINE:
			return res.append("Line-histogram Mode");
			break;
		case ANAL_WORK_SEGMENT_FREEDRAW:
			return res.append("Region selection Mode");
			break;
		case ANAL_WORK_SEGMENT_CALC_DISTANCE:
			return res.append("Calculate Distance Mode");
			break;
		case ANAL_WORK_SEGMENT_POLY:
			return res.append("Polygon selection Mode");
			break;
		case ANAL_WORK_CAPTURE:
			return res.append("Capture mode");
			break;
		case ANAL_WORK_SEGMENT_PICKER:
			return res.append("Pixel-wised selection Mode");
			break;
		case ANAL_WORK_SEGMENT_RECT:
			return res.append("Rect selection Mode %1").arg(pencil_A ? QString("(pencil-mode)") : QString("(size:%1)").arg(AngularSize_A));
			break;
		case ANAL_WORK_SKETCHDRAWSEGMENTATION:
			return res.append("Draw-cut Mode %1").arg(bSize == 0 ? (bType ? QString("(pencil-mode)") : QString("(custom-mode)")) : QString("(size:%1)").arg(bSize));
			break;
		case ANAL_WORK_3D_CURVE_SPLIT:
			return view3D ? res.append("Curve split Mode") : QString();
			break;
		case ANAL_WORK_3D_PLANE_SPLIT:
			return view3D ? res.append("Plane cut Mode") : QString();
		default:
			break;
		}
	}
	else
	{
		bool bType = (m_workMode == WORK_RECTROI || getDCutShape() == DRAW_CUT_SHAPE_ANGULAR);
		bool bDraw = bType ? pencil_S : oval_S;
		int bSize = bDraw ? 0 : (bType ? AngularSize_S : CirculSize_S);

		switch (m_workMode)
		{
		case WORK_RECTROI:
			return res.append("Rect selection Mode %1").arg(pencil_S ? QString("(pencil-mode)") : QString("(size:%1)").arg(AngularSize_S));
			break;
		case WORK_OVALROI:
			return res.append("Oval selection Mode %1").arg(oval_S ? QString("(custom-mode)") : QString("(size:%1)").arg(CirculSize_S));
			break;
		case WORK_POLYROI:
			return res.append("Polygon selection Mode");
			break;
		case WORK_REGION_ROI:
			return res.append("Region selection Mode");
			break;
		case WORK_PICKERROI:
			return res.append("Pixel-wised selection Mode");
			break;
		case WORK_SKETCHDRAWSEGMENTATION:
			return res.append("Draw-cut Mode %1").arg(bSize == 0 ? (bType ? QString("(pencil-mode)") : QString("(custom-mode)")) : QString("(size:%1)").arg(bSize));
			break;
		case WORK_CAPTURE:
			return res.append("Capture mode");
			break;
		case WORK_WORKING_REGION_MOVE:
			return res.append("Move working region mode");
			break;
		case WORK_PATCHY_SELECT:
			return res.append("Select patchy point mode");
			break;
		case WORK_SEED_SELECT:
			return res.append("Select seed Mode");
			break;
		case WORK_ANNOTATION_TEXT:
			return res.append("Anno-Text Mode");
			break;
		case WORK_ANNOTATION_PATH:
			return res.append("Anno-Path Mode");
			break;
		case WORK_ANNOTATION_PATH_PLAY:
			return res.append("Play Anno-Path Mode");
			break;
		case WORK_3D_CURVE_SPLIT:
			return view3D ? res.append("Curve split Mode") : QString();
			break;
		case WORK_3D_PLANE_SPLIT:
			return view3D ? res.append("Plane cut Mode") : QString();
		default:
			break;
		}
	}
	return QString("Unknown Mode");
}

QString WindowManager::getUnitString()
{
	if (m_modality == eModalityType::Modality_HU)
	{
		return QString("HU");
	}
	else
	{
		return QString("SI");
	}
}

void WindowManager::setFlipMode()
{
	isLayerFlip = !isLayerFlip;

	if (m_tabWindow)
	{
		VOLProcessTab* tab = m_tabWindow->getVolTab();

		if (tab)
		{
			tab->setFlipmode();
		}
	}
}


bool WindowManager::lastestPathGet(QString& outFilepath, bool onlyPath)
{
#if 0
	QFile file(STRING_MANAGER->configFileName);

	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		return false;
	}

	QXmlStreamReader reader(&file);
	reader.setDevice(&file);

	while (!reader.atEnd())
	{
		reader.readNext();

		if (reader.isStartElement())
		{
			if (reader.name() == STRING_MANAGER->config_LatestPath)
			{
				filepath = reader.readElementText();
				return true;
			}
		}
	}

	file.close();
	return false;
#else
	bool res = getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_LatestPath, outFilepath);

	QFileInfo curr(outFilepath);

#ifdef OPEN_MIP_IN_DEEPCATCH
	QString strExt = curr.completeSuffix();
	if ((!m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(m_pProductManager->m_strMedipDeepCatch)
		|| !m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(m_pProductManager->m_strMedipDeepCatchV2))
		&& !strExt.compare("mipd"))
	{
		res = true;
	}
	else if ((!m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(m_pProductManager->m_strMedipDeepCatch)
		|| !m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY).compare(m_pProductManager->m_strMedipDeepCatchV2))
		&& !strExt.compare("mip"))
	{
		res = true;
	}
	else
	{
		res = false;
	}

#endif
	if (onlyPath && res)
	{
		outFilepath = curr.absoluteDir().absolutePath();
	}

	if (!res)
	{
		outFilepath = "";
	}

	return res;
#endif
}

void WindowManager::lastestPathSave(const QString& filepath)
{
#if 0
	QFile file(STRING_MANAGER->configFileName);

	if (file.open(QIODevice::WriteOnly))
	{
		QXmlStreamWriter writer(&file);

		writer.setDevice(&file);
		writer.setAutoFormatting(true);
		writer.writeStartDocument();
		writer.writeStartElement("FilePath");
		writer.writeAttribute("version", VER_FILEVERSION_STR);
		writer.writeTextElement(STRING_MANAGER->config_LatestPath, filepath);
		writer.writeEndElement();
		writer.writeEndDocument();

		file.close();
	}
#else
	setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_LatestPath, filepath);
#endif
	QString title = QString(" [%1]").arg(filepath);
	mainWindow->setMainWindowTitle(title);
	mainWindow->AddRecentFile(filepath);
}

void WindowManager::setLowSpecOptions()
{
	if (m_latest_active_window_type == WT_VOLUME)
	{
		if (mainTabType == MAINTAB_MEASUREMENT)
		{
			if (mainAnalWidget)
			{
				mainAnalWidget->getViewVolume()->showControls();
			}
		}
		else if (mainTabType == MAINTAB_SEGMENTATION)
		{
			VolumeView* view = dynamic_cast<VolumeView*>(mainSegmentWidget->getViewVolume());
			if (view)
				view->showControls();
		}
		else if (mainTabType == MAINTAB_TA)
		{
			if (mainTAWidget)
			{
				mainTAWidget->getViewVolume()->showControls();
			}
		}
	}

	m_pDataContext->volume_data.forceUpdateVolume();

	if (mainAnalWidget)
	{
		{
			AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(mainAnalWidget->getWindow(WT_AXIAL));
			if (view)
				view->setLowSpecOptions();
		}
		{
			AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(mainAnalWidget->getWindow(WT_CORONAL));
			if (view)
				view->setLowSpecOptions();
		}
		{
			AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(mainAnalWidget->getWindow(WT_SAGITTAL));
			if (view)
				view->setLowSpecOptions();
		}
	}

	//if (mainTAWidget)
	//{
	//	{
	//		RadiomicsView * view = dynamic_cast<RadiomicsView*>(mainTAWidget->getWindow(WT_AXIAL));
	//		if (view)
	//			view->setLowSpecOptions();
	//	}
	//	{
	//		RadiomicsView * view = dynamic_cast<RadiomicsView*>(mainTAWidget->getWindow(WT_CORONAL));
	//		if (view)
	//			view->setLowSpecOptions();
	//	}
	//	{
	//		RadiomicsView * view = dynamic_cast<RadiomicsView*>(mainTAWidget->getWindow(WT_SAGGITAL));
	//		if (view)
	//			view->setLowSpecOptions();
	//	}
	//}

}

void WindowManager::setAdvancedConfig()
{
	QString strVal;
	if (!getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_undoLimit, strVal))
		m_undoLimit = m_undoBasic;
	else
	{
		if (!IsLicensePass())
			m_undoLimit = m_undoBasic;
		else
		{
			int val = strVal.toInt();

			if (val <= m_undoBasic)
				m_undoLimit = m_undoBasic;
			else if (val > 100)
				m_undoLimit = 100;
			else
				m_undoLimit = val;
		}
	}
}

bool WindowManager::IsSecondaryImage(DcmtkSeriesInfo* pDcmInfo)
{
	QString SOPClassUID = QString::fromStdString(pDcmInfo->sopClassUid);

	if (SOPClassUID == STRING_MANAGER->getString(STR_UID_SECONDARY_CAPTUREIMAGE_STORAGE))
		return true;

	return false;
}

/*@function useSkipRender
**@brief render skip mode setting
**@param index skip mode type (0:wl change, 1: object move, 2: object zoom, 3: alpha change)
**@param res set/unset (default : set(true))
*/
void WindowManager::useSkipRender(int index, bool res)
{
	if (mainSegmentWidget)
	{
		VolumeView* view = dynamic_cast<VolumeView*>(mainSegmentWidget->getViewVolume());

		if (view)
			view->useSkipRender(index, res);
	}

	if (mainAnalWidget)
	{
		AnalVolumeView* view = dynamic_cast<AnalVolumeView*>(mainAnalWidget->getViewVolume());

		if (view)
			view->useSkipRender(index, res);
	}

	if (mainTAWidget)
	{
		AnalVolumeView* view = dynamic_cast<AnalVolumeView*>(mainTAWidget->getViewVolume());

		if (view)
			view->useSkipRender(index, res);
	}
}

LAYER_RG_SHORTCUT WindowManager::getRGType()
{
	return m_ShortcutRG;
}

void WindowManager::setRGType(LAYER_RG_SHORTCUT type)
{
	m_ShortcutRG = type;
}

int WindowManager::getAnimationGroup()
{
	AnimationTab* tab = m_tabWindow->getAniTab();
	if (tab)
	{
		return tab->getCurrentGroup();
	}
	return 0;
}

int WindowManager::getAniStartPoint(int _group)
{
	int start = 0;
	for (int i = 0; i < _group; i++)
	{
		if (i >= aniCount.size())
		{
			break;
		}
		start += aniCount.at(i);
	}
	return start;
}

bool WindowManager::isContainAniPoint(QVector3D vec, int group)
{
	if (aniCount.at(group) == 0)
	{
		return false;
	}

	int start = 0;

	for (int i = 0; i < group; i++)
	{
		start += aniCount.at(i);
	}

	start += aniCount.at(group);
	start--;

	if (aniLine.at(start) == vec)
	{
		return true; //recursive 가능하게
	}

	return false;
}

void WindowManager::setGammaMode(bool val, bool rst, bool updateTab)
{
	if (val != m_use_Gamma)
	{
		m_use_Gamma = val;
	}

	if (!updateTab)
	{
		setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_gamma, m_use_Gamma ? QString::number(m_Gamma) : QString("0"));
		return;
	}

	if (m_tabWindow)
	{
		OptionalTab* tab = m_tabWindow->getOptionalTab();
		if (tab)
		{
			tab->setGammaMode(val);
		}

		if (rst)
		{
			tab->slot_OnReset();
		}
	}
}

bool WindowManager::isGammaMode()
{
	return m_use_Gamma;
}

void WindowManager::setConfigValue(QString element, QString node, QString value, int presetIndex)
{
	QFile file(STRING_MANAGER->configFileName);
	if (element == ELEMENT_PRESET)
	{
		if (-1 >= presetIndex)
			file.setFileName(STRING_MANAGER->presetFileName);
		else
		{
			QString preName = getPresetName(presetIndex);

			if (preName.length() == 0)
			{
				value = "";
				return;
			}

			file.setFileName(STRING_MANAGER->presetFilePath + QString("/%1.prd").arg(preName));
		}
	}

	if (file.open(QIODevice::ReadWrite | QIODevice::Text))
	{
		QDomDocument doc;
		QDomElement root;
		QDomElement Firstchild;
		QDomElement NodeTag;
		QDomElement newNodeTag;
		QString errmsg;

		if (!doc.setContent(&file, &errmsg))
		{
			file.resize(0);
			doc.setContent(&file, &errmsg);
			QTextStream out(&file);
			QDomNode xmlNode = doc.createProcessingInstruction
			("xml", "version=\"1.0\" encoding=\"UTF-8\"");
			doc.insertBefore(xmlNode, doc.firstChild());

			doc.save(out, 4);
			out.setCodec("UTF-8");
			out << "<" << element << ">\n"
				<< "<" << node << ">" << value
				<< "</" << node << ">\n"
				<< "</" << element << ">\n";
			//		printf("%s", errmsg.toUtf8().constData());
			file.close();
			return;
		}

		Firstchild = doc.firstChildElement(element);

		if (Firstchild.isNull())
		{
			Firstchild = doc.createElement(element);
			doc.appendChild(Firstchild);

			Firstchild = doc.firstChildElement(element);
		}

		NodeTag = Firstchild.firstChildElement(node);
		newNodeTag = doc.createElement(node);
		QDomText text = doc.createTextNode(value);
		newNodeTag.appendChild(text);

		if (NodeTag.isNull())
			Firstchild.appendChild(newNodeTag);
		else
			Firstchild.replaceChild(newNodeTag, NodeTag);

		file.resize(0);

		QTextStream out(&file);
		doc.save(out, 4);

		file.close();
	}
}

bool WindowManager::getConfigValue(QString element, QString node, QString& value, int presetIndex)
{
	QFile file(STRING_MANAGER->configFileName);
	bool res = false;
	value = "";
	if (element == ELEMENT_PRESET)
	{
		if (-1 >= presetIndex)
			file.setFileName(STRING_MANAGER->presetFileName);
		else
		{
			QString preName = getPresetName(presetIndex);

			if (preName.length() == 0)
			{
				value = "";
				return false;
			}

			file.setFileName(STRING_MANAGER->presetFilePath + QString("/%1.prd").arg(preName));
		}
	}

	if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QDomDocument doc;
		QDomElement root;
		QDomElement Firstchild;
		QDomElement NodeTag;
		QDomElement newNodeTag;
		QString errmsg;

		if (!doc.setContent(&file, &errmsg))
		{
			file.close();
			return res;
		}
		file.close();
		Firstchild = doc.firstChildElement(element);

		if (Firstchild.isNull())
		{
			return res;
		}

		NodeTag = Firstchild.firstChildElement(node);

		if (!NodeTag.isNull())
		{
			value = NodeTag.text();
			//	printf("%s", value.toUtf8().constData());
			res = true;
		}
	}

	return res;
}

bool WindowManager::getConfigWL(QString node, int& nWidth, int& nLevel, int presetIndex)
{
	QString resStr;
	QStringList resList;

	if (!getConfigValue(ELEMENT_PRESET, node, resStr, presetIndex))
		return false;

	resList = resStr.split("/");

	nWidth = resList.at(0).toInt();
	nLevel = resList.at(1).toInt();

	return true;
}

void WindowManager::setConfigWL(QString node, int nWidth, int nLevel, int presetIndex)
{
	QString resStr;

	resStr = QString("%1/%2").arg(nWidth).arg(nLevel);

	setConfigValue(ELEMENT_PRESET, node, resStr, presetIndex);
}

bool WindowManager::getConfigPreset(int index, int type, QString& value, bool isColor)
{
	QString node = "SCREEN";

	if (CL_3D == type)
		node = "VOLUME";

	if (isColor)
		node.append("_CUSTOM");

	return getConfigValue(ELEMENT_PRESET, node, value, index);
}

void WindowManager::setConfigPreset(int index, int type, QString value, bool isColor)
{
	QString node = "SCREEN";

	if (CL_3D == type)
		node = "VOLUME";

	if (isColor)
		node.append("_CUSTOM");

	setConfigValue(ELEMENT_PRESET, node, value, index);
}


void WindowManager::getWidthLevelValue(bool bLevel, int& value, QPoint start, QPoint end)
{
#if 0
	const int rangeValue = 70;
#else
	int rangeValue;
	rangeValue = abs(start.x() - end.x());
	rangeValue *= 10;
#endif
	if (!bLevel)
	{
		if ((start.x() - end.x()) < 0)
		{
			//width val 증가
			if ((value + rangeValue) <= abs(m_pDataContext->volume_data.getHuMax() - m_pDataContext->volume_data.getHuMin()))
				value += rangeValue;
			else
				value = abs(m_pDataContext->volume_data.getHuMax() - m_pDataContext->volume_data.getHuMin());
		}
		else if ((start.x() - end.x()) > 0)
		{
			//width val 감소
			if ((value - rangeValue) >= 1)
				value -= rangeValue;
			else
				value = 1;
		}
	}
	else
	{
		rangeValue = abs(start.y() - end.y());
		rangeValue *= 10;
		if ((end.y() - start.y()) < 0)
		{
			//levelVal val 증가
			if ((value + rangeValue) <= m_pDataContext->volume_data.getHuMax())
				value += rangeValue;
			else
				value = m_pDataContext->volume_data.getHuMax();
		}
		else if ((end.y() - start.y()) > 0)
		{
			//levelVal val 감소
			if ((value - rangeValue) >= m_pDataContext->volume_data.getHuMin())
				value -= rangeValue;
			else
				value = m_pDataContext->volume_data.getHuMin();
		}
	}
}

/*
	Segmentation, Radiomics, Measurement TAB의 좌표를 공유한다.
	TODO :
		- 현재 Segmenation,Radiomics / Measurement TAB 사이 좌표를 공유하지 않음.
		- 추후 같은 좌표를 공유하도록 수정할 것
*/
void WindowManager::setSharedPrefer(MAINTAB_TYPE prevTabType)
{
	AnalVolumeView* pMeasurementVolumeView = nullptr;
	VolumeView* pSegmentationVolumeView = nullptr;
	AnalVolumeView* pRadiomicsVolumeView = nullptr;

	if (mainAnalWidget)
	{
		pMeasurementVolumeView = mainAnalWidget->getViewVolume();
	}

	if (mainSegmentWidget)
	{
		pSegmentationVolumeView = static_cast<VolumeView*>(mainSegmentWidget->getViewVolume());
	}

	if (mainTAWidget)
	{
		pRadiomicsVolumeView = static_cast<AnalVolumeView*>(mainTAWidget->getViewVolume());
	}

	if (prevTabType == MAINTAB_MEASUREMENT)
	{
		/* Measurement -> Segmentation, Radiomics 상태 동기화 */
		copyVolumeGeometryInfo(pMeasurementVolumeView, pSegmentationVolumeView);
		copyVolumeGeometryInfo(pMeasurementVolumeView, pRadiomicsVolumeView);
	}
	else if (prevTabType == MAINTAB_SEGMENTATION)
	{
		/* Segmentation -> Measurement, Radiomics 상태 동기화 */
		copyVolumeGeometryInfo(pSegmentationVolumeView, pMeasurementVolumeView);
		copyVolumeGeometryInfo(pSegmentationVolumeView, pRadiomicsVolumeView);

		copy_Segmentation_To_Radiomics_MPRPlaneGeometryInfos(this, m_pDataContext, mainSegmentWidget, mainTAWidget);
	}
	else if (prevTabType == MAINTAB_TA)
	{
		/* Radiomics -> Segmentation, Measurement 상태 동기화 */
		copyVolumeGeometryInfo(pRadiomicsVolumeView, pMeasurementVolumeView);
		copyVolumeGeometryInfo(pRadiomicsVolumeView, pSegmentationVolumeView);

		copy_Radiomics_To_Segmentation_MPRPlaneGeometryInfos(this, m_pDataContext, mainTAWidget, mainSegmentWidget);
	}
}

void WindowManager::setClipOpt()
{
	Visualize3DTab* tab = m_tabWindow->get3DTab();

	if (tab)
	{
		tab->setClipOpt();
	}
}

float WindowManager::getVolumeAlpha(bool isOrigin)
{
	const float firstMax = 0.1;
	const float secondMax = 1.0 - 0.1;
	int min, max;

	if (isOrigin)
		return m_VolumeAlpha;

	max = m_maxVolumeAlpha / 2;
	if (m_VolumeAlpha <= max)
		return ((float)m_VolumeAlpha / max) * firstMax;
	else
	{
		min = m_maxVolumeAlpha / 2 + 1;
		max = m_maxVolumeAlpha;

		return 0.1 + ((float)(m_VolumeAlpha - min) / (max - min)) * secondMax;
	}
}

void WindowManager::setVisibleMesh(bool set)
{
	m_showMesh = set;
	renderLater_GridView();
}

void WindowManager::setCullingMesh(bool set)
{
	m_clipMesh = set;
	renderLater_GridView();
}

void WindowManager::setCullingMaskVolume(bool set)
{
	m_clipMaskVol = set;
	renderLater_GridView();
}

void WindowManager::setClip2DPlane(bool set)
{
	m_clip2DPlane = set;

	renderLater_GridView();
}

void WindowManager::setMaskingmode(bool type)
{
	m_maskingmode = type;

	renderLater_GridView();
}

void WindowManager::setVolumeAlpha(int value)
{
	m_VolumeAlpha = value;
	setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_VolumeOpacity3D, QString::number(value));
}

void WindowManager::resetVolumeAlpha()
{
	m_VolumeAlpha = m_maxVolumeAlpha / 2;

	setVolumeAlphaSlider(m_VolumeAlpha);
}


/**@function getLayerAlpha
  *@brief 3D View layer alpha value change
  *@param isOrigin UI value returns (default : false)
  *@return isOrigin ? UI value : 0-255
*/
float WindowManager::getLayerAlpha(bool isOrigin)
{
	// 211223 허건 과장 주석처리
	// 다른 탭에서 MipFile 새로 로딩하는 경우 Layer가 화면상에서 사라지게 됨
	//if (mainTabType != MAINTAB_SEGMENTATION &&
	//	mainTabType != MAINTAB_MEASUREMENT &&
	//	mainTabType != MAINTAB_TA &&
	//	mainTabType != MAINTAB_PLANNING) return 0.0f;

	const int maxVal = 200;
	const float firstMax = 20.0;
	const float secondMax = 255.0;
	int min, max;

	if (isOrigin)
		return m_LayerAlpha;

	max = maxVal / 2;
	if (m_LayerAlpha <= max)
		return ((float)m_LayerAlpha / max) * firstMax;
	else
	{
		min = maxVal / 2 + 1; //101
		max = maxVal; //200

		//101-200 to 21-255
		float val = m_LayerAlpha;
		val -= min;
		val /= (max - min);
		val *= (secondMax - 21);
		val += 21;

		return val;
	}
}

void WindowManager::setLayerAlpha(int value)
{
	m_LayerAlpha = value;
}

bool WindowManager::saveRAW(QString& mPath, mask _m, int _mI)
{
	m_pActionManager->action_FileWork_Export_RawFile(mPath, _m, _mI);

	return true;
}

bool WindowManager::saveHURAW(QString& mPath, mask _m, int _mI, bool _patchy, BoundingBoxI _box)
{
	m_pActionManager->action_FileWork_Export_HURawFile(mPath, _m, _mI, _patchy, _box);

	return true;
}

void WindowManager::saveROI(QString& mPath)
{
	int height = m_pDataContext->volume_data.getCY();
	int width = m_pDataContext->volume_data.getCX();
	int cnt = m_pDataContext->volume_data.getCZ();

	FILE* fp;
	fp = fopen(mPath.toLocal8Bit().toStdString().c_str(), "w");

	//2차안
	//width height slice 저장
	fprintf_s(fp, "%d\t%d\t%d\n", width, height, cnt);

	unsigned short* mResultMask;
	unsigned short mValue = 0;
	mResultMask = new unsigned short[width * height * cnt];
	for (long k = 0; k < width * height * cnt; k++)
	{
		/*if (m_ROIClass->m_Label_Mask[k] != 0)
			mResultMask[k] = (m_ROIClass->m_Label_Mask[k] * 10) + m_ROIClass->m_ROI_Mask[k];
		else
			mResultMask[k] = 0;*/
	}
	mValue = mResultMask[0];
	long vcnt = 1;//마스크0에 대한 카운트
	for (long k = 1; k < width * height * cnt; k++)
	{
		if (mValue != mResultMask[k])
		{
			fprintf_s(fp, "%d\t%ld\n", mValue, vcnt);
			mValue = mResultMask[k];
			vcnt = 1;//0으로 초기화후 현재 값카운트
		}
		else
		{
			vcnt++;
		}
	}

	delete[] mResultMask;

	fclose(fp);
}

bool WindowManager::saveVTK(QString& filename, muint32 index, mask _m, int _mI)
{
	m_pActionManager->action_FileWork_Export_VTKFile(m_pDataContext, index, _m, filename, _mI);

	return true;
}

int WindowManager::readRawFile(unsigned char* image, int size, QString& filename)
{
	QFile file(filename);
	if (!file.exists() || !file.open(QIODevice::ReadOnly))
	{
		return -1;
	}

	file.read((char*)image, size);
	file.close();

	int voxel_cnt = 0;
	for (int i = 0; i < size; i++)
	{
		if (image[i] != 0) voxel_cnt++;
	}

	return voxel_cnt;
}

bool WindowManager::readROIFile(unsigned char* image, QString& filename)
{
	bool r = false;
	FILE* fp;
	fopen_s(&fp, filename.toLocal8Bit().toStdString().c_str(), "r");
	//width,height,slice읽어들여와 현재 이미지의 width,height,slice와 비교
	int rwidth, rheight, rcnt;

	int height = m_pDataContext->volume_data.getCY();
	int width = m_pDataContext->volume_data.getCX();
	int cnt = m_pDataContext->volume_data.getCZ();
	memset(image, 0, cnt * height * width * sizeof(uchar));

	//	memset(m_ROIClass->m_Label_Mask, 0, width*height*slice);
	//	memset(m_ROIClass->m_ROI_Mask, 0, width*height*slice);

	fscanf_s(fp, "%d\t%d\t%d\n", &rwidth, &rheight, &rcnt);

	if (rwidth != width || rheight != height || rcnt != cnt)
	{
		m_pMessageBox->warning(nullptr, STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD),
			STRING_MANAGER->getString(STR_DIFFER_SIZE));
	}
	else
	{
		//ROI 읽어들이기
		unsigned short mValue = 0;
		unsigned short lv = 0, rv = 0;
		long vcnt = 0;
		int k = 0;
		while (fscanf_s(fp, "%hd\t%ld\n", &mValue, &vcnt) != EOF)
		{
			lv = mValue / 10;
			rv = mValue % 10;

			for (int s = 0; s < vcnt; s++)
			{
				if (lv != 0)
				{
					image[k] = 1;
				}
				k++;
				//	volume_data.setMaskData(k, rv > 0 ? VM_MASK2 : 0);
				//	k++;
			}
		}

		r = true;
	}

	fclose(fp);

	return r;
}

void WindowManager::readROINIIFile(unsigned char* image, vtkDataArray* scalar)
{
	int height = m_pDataContext->volume_data.getCY();
	int width = m_pDataContext->volume_data.getCX();
	int slice = m_pDataContext->volume_data.getCZ();
	int dataLenth = m_pDataContext->volume_data.getVolumeDataLength();

	int zVal;
	for (int z = 0; z < slice; z++)
	{
		zVal = slice - z - 1;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				muint32 index = (z * width * height) + (y * width) + x;

				if (index > (dataLenth - 1)) continue;

				if ((uchar)scalar->GetTuple1(zVal * height * width + y * width + x) != 0)
					image[index] = 1;
			}
		}
	}
}

void WindowManager::loadNextFiles()
{
	m_pFileLoader->loadNextFiles();
}

bool WindowManager::loadFiles(const QString& filepath)
{
	return m_pFileLoader->loadFiles(filepath);
}

bool WindowManager::loadImage(const QString& path)
{
	return m_pFileLoader->loadImage(path);
}

bool WindowManager::IsImportListEmpty() const
{
	return importList.empty();;
}

void WindowManager::ClearFileData()
{
	if (!importList.empty())
	{
		importList.clear();
	}

	if (!exportList.empty())
	{
		exportList.clear();
	}
}

bool WindowManager::fileOpen(const QString& filepath, bool bReverse)
{
	return m_pFileLoader->fileOpen(filepath, bReverse);
}

bool WindowManager::fileOpen_PET_DCM(const QString& filename)
{
	return m_pFileLoader->fileOpen_PET_DCM(filename);
}

bool WindowManager::fileMultiOpen(const QStringList& filePathList)
{
	if (filePathList.isEmpty())
	{
		return false;
	}
	m_pActionManager->SetAfterThread(THREAD_IMPORT_FILES);
	importList.reserve(filePathList.size());

	for (QString fileName : filePathList)
	{
		importList.push_back(fileName);
	}

	loadFiles(importList.takeFirst());

	return true;
}

bool WindowManager::fileMultiImport(const QStringList& filePathList)
{
	if (filePathList.isEmpty())
	{
		return false;
	}
	m_pActionManager->SetAfterThread(THREAD_IMPORT_FILES);
	importList.reserve(filePathList.size());

	for (QString fileName : filePathList)
	{
		importList.push_back(fileName);
	}

	if (loadFiles(importList.takeFirst()) == false)
	{
		/* 실패 시 THREAD_NONE으로 설정하여 side effect 방지 */
		m_pActionManager->SetAfterThread(THREAD_NONE);
		return false;
	}

	return true;
}

bool WindowManager::loadDicomData(const mint16* HU, DcmtkSeriesInfo& dcmSeriesInfo, DicomVolumeInfo& dcmVolumeInfo)
{
	return m_pFileLoader->loadDicomData(HU, dcmSeriesInfo, dcmVolumeInfo);
}

bool WindowManager::fileSave(const QString& filename, bool exit)
{
	QString ext = filename.section('.', -1).toLower();

	if (ext == "mip" && !LICENSE_DATA->getProductType().compare(PRODUCT_NAME_MEDIP))
	{
		if (m_tabWindow->getCaptureTab())
		{
			m_tabWindow->getCaptureTab()->CaptureFull(false);
		}

		if (nullptr != mainReportWidget)
		{
			mainReportWidget->setTempReportData(filename);
		}

		//m_pActionManager->action_FileWork_Export_MipFile(filename, exit);
		m_pActionManager->action_FileWork_Export_MipFile(filename, MIP_ENCODER::PT_MIP);

		return true;
	}
	else if (ext == "mipd" && (!LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH) || !LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH_V2)))
	{
		if (m_tabWindow->getCaptureTab())
		{
			m_tabWindow->getCaptureTab()->CaptureFull(false);
		}

		if (nullptr != mainReportWidget)
		{
			mainReportWidget->setTempReportData(filename);
		}

		m_pActionManager->action_FileWork_Export_MipFile(filename, MIP_ENCODER::PT_MIPD);
		return true;
	}
	else if (ext == "mipa" && !LICENSE_DATA->getProductType().compare(PRODUCT_NAME_MEDIP_AI))
	{
		if (m_tabWindow->getCaptureTab())
		{
			m_tabWindow->getCaptureTab()->CaptureFull(false);
		}

		if (nullptr != mainReportWidget)
		{
			mainReportWidget->setTempReportData(filename);
		}

		m_pActionManager->action_FileWork_Export_MipFile(filename, MIP_ENCODER::PT_MIPA);
		return true;
	}
	else if (ext == "nii")
	{
		if (!IsLicensePass() || !m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_NII_wholeHU))
		{
			m_pMessageBox->warning(nullptr, STRING_MANAGER->getString(STR_EXPORT_NII),
				STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_EXPORT_NII) +
				STRING_MANAGER->getString(STR_LICENSE_WARN));

			return false;
		}

		return saveNII(filename);
	}
	else if (ext == "json")
	{
		// global clinical info.
		QVector<QPair<QString, QString>> clinicalInfo;
		ClinicalInformationDlg dlg;
		if (dlg.exec() == QDialog::Accepted)
		{
			clinicalInfo = dlg.getClinicalInfo();
		}

		// local clinical info.
		QVector<QPair<QString, int>> maskNameToVoxelCountVec;
		for (int i = 0; i < m_pDataContext->volume_data.getMaskInfoListCnt(); i++)
		{
			MaskInfo* pMaskInfo = m_pDataContext->volume_data.getMaskInfo(i);
			if (pMaskInfo)
			{
				maskNameToVoxelCountVec.push_back(QPair<QString, int>(QString::fromWCharArray(pMaskInfo->maskName), m_pDataContext->volume_data.getVoxelCount(pMaskInfo->uid)));
			}
		}
		ClinicalInformationDlg dlg_local(eCIDMLocal);
		dlg_local.setBasicTableData(maskNameToVoxelCountVec);
		QVector<sLocalClinicalInfo> localClinicalInfo;
		if (dlg_local.exec() == QDialog::Accepted)
		{
			localClinicalInfo = dlg_local.getLocalClinicalInfo();
		}

		if (m_tabWindow->getCaptureTab())
		{
			m_tabWindow->getCaptureTab()->CaptureFull(false);
		}

		if (nullptr != mainReportWidget)
		{
			mainReportWidget->setTempReportData(filename);
		}
		m_pActionManager->action_FileWork_Export_JsonFile(filename, clinicalInfo, localClinicalInfo);
		return true;
	}

	return false;
}

void WindowManager::setLatestActiveViewType(WINDOW_TYPE type)
{
	static bool bOnce = true;
	if (m_latest_active_window_type != type)
	{
		if (m_pDataContext->volume_data.isValidate())
		{
			m_latest_active_window_type = type;

			if (mainSegmentWidget)
			{
				mainSegmentWidget->hideControls();
				mainSegmentWidget->showControls(type);
			}
			if (mainAnalWidget)
			{
				mainAnalWidget->hideControls();
				mainAnalWidget->showControls(type);
			}
			if (mainTAWidget)
			{
				mainTAWidget->hideControls();
				mainTAWidget->showControls(type);
			}
			if (mainTabType == MAINTAB_SEGMENTATION)
			{
				if (WT_VOLUME != type)
				{
					WindowBase* win = mainSegmentWidget->getWindow(type);

					if (win)
					{
						setLatestActiveViewZoomX(win->getZoomFactorX());
						setLatestActiveViewZoomY(win->getZoomFactorY());
						setLatestActiveViewDepth(win->getLatestDepth());
					}

				}
				else
				{
					VolumeView* vView = static_cast<VolumeView*>(mainSegmentWidget->getViewVolume());
					if (vView)
						vView->setWorkMode(m_workMode);
				}

				mainSegmentWidget->renderLater_SubView();
			}
			else if (mainTabType == MAINTAB_MEASUREMENT)
			{

			}
		}
	}
	else
	{
		if (bOnce && type == WT_AXIAL)
		{
			if (m_pDataContext->volume_data.isValidate())
			{
				bOnce = false;
				mainSegmentWidget->hideControls();
				if (mainTabType == MAINTAB_SEGMENTATION)
				{
					mainSegmentWidget->showControls(type);
					mainSegmentWidget->renderLater_SubView();
				}
			}
		}
	}
}

WINDOW_TYPE WindowManager::getLatestActiveViewType()
{
	return m_latest_active_window_type;
}

void WindowManager::setLatestActiveViewDepth(uint depth)
{
	m_latest_active_window_depth = depth;
}

uint WindowManager::getLatestActiveViewDepth()
{
	return m_latest_active_window_depth;
}

float WindowManager::getLatestActiveViewZoomX()
{
	return m_latest_active_window_zoomFactorX;
}

float WindowManager::getLatestActiveViewZoomY()
{
	return m_latest_active_window_zoomFactorY;
}

void WindowManager::setLatestActiveViewZoomX(float x)
{
	m_latest_active_window_zoomFactorX = x;
}

void WindowManager::setLatestActiveViewZoomY(float y)
{
	m_latest_active_window_zoomFactorY = y;
}

void WindowManager::setWheelZoom(bool set)
{
	m_wheelZoom = set;

	{
		SegmentationView* view = dynamic_cast<SegmentationView*>(mainSegmentWidget->getWindow(WT_AXIAL));
		if (view)
			view->InitWheelSliderFunc(set);
	}
	{
		SegmentationView* view = dynamic_cast<SegmentationView*>(mainSegmentWidget->getWindow(WT_CORONAL));
		if (view)
			view->InitWheelSliderFunc(set);
	}
	{
		SegmentationView* view = dynamic_cast<SegmentationView*>(mainSegmentWidget->getWindow(WT_SAGITTAL));
		if (view)
			view->InitWheelSliderFunc(set);
	}
	//TODO anal

	if (mainAnalWidget)
	{
		{
			AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(mainAnalWidget->getWindow(WT_AXIAL));

			if (view)
				view->setWheelSliderFunc(set);
		}

		{
			AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(mainAnalWidget->getWindow(WT_CORONAL));

			if (view)
				view->setWheelSliderFunc(set);
		}

		{
			AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(mainAnalWidget->getWindow(WT_SAGITTAL));

			if (view)
				view->setWheelSliderFunc(set);
		}

		mainAnalWidget->renderLater_All();
	}


	if (mainTAWidget)
	{
		{
			RadiomicsView* view = dynamic_cast<RadiomicsView*>(mainTAWidget->getWindow(WT_AXIAL));

			if (view)
				view->InitWheelSliderFunc(set);
		}

		{
			RadiomicsView* view = dynamic_cast<RadiomicsView*>(mainTAWidget->getWindow(WT_CORONAL));

			if (view)
				view->InitWheelSliderFunc(set);
		}

		{
			RadiomicsView* view = dynamic_cast<RadiomicsView*>(mainTAWidget->getWindow(WT_SAGITTAL));

			if (view)
				view->InitWheelSliderFunc(set);
		}

		mainTAWidget->renderLater_All();
	}

	setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_wheelZoom, QString::number(set));

}

void WindowManager::setSliderMid(WINDOW_TYPE type)
{
	if (!getWheelZoom())
	{
		return;
	}

	if (mainAnalWidget)
	{
		switch (type)
		{
		case WT_AXIAL:
		{
			AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(mainAnalWidget->getWindow(WT_AXIAL));

			if (view)
				view->setSliderMid();
		}
		break;
		case WT_CORONAL:
		{
			AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(mainAnalWidget->getWindow(WT_CORONAL));

			if (view)
				view->setSliderMid();
		}
		break;
		case WT_SAGITTAL:
		{
			AnalMPRPlaneView* view = dynamic_cast<AnalMPRPlaneView*>(mainAnalWidget->getWindow(WT_SAGITTAL));

			if (view)
				view->setSliderMid();
		}
		break;
		}
	}
}

void WindowManager::setColor2DMode(bool set)
{
	m_color2D = set;
	setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_wheelZoom, QString::number(set));

	forceUpdate2DViewData();

	if (mainSegmentWidget)
	{
		mainSegmentWidget->renderLater_GridView(false);
	}

	if (mainAnalWidget)
	{
		mainAnalWidget->renderLater_GridView(false);
	}

	if (mainTAWidget)
	{
		mainTAWidget->renderLater_GridView(false);
	}
}

void WindowManager::setInvertingLUTMode(bool set)
{
	m_invertingLUT = set;
	setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_wheelZoom, QString::number(set));

	forceUpdate2DViewData();

	if (mainSegmentWidget)
		mainSegmentWidget->renderLater_GridView(false);

	if (mainAnalWidget)
		mainAnalWidget->renderLater_GridView(false);

	if (mainTAWidget)
		mainTAWidget->renderLater_GridView(false);
}

void WindowManager::renderLater_3DView()
{
	if (mainTabType == MAINTAB_SEGMENTATION)
	{
		if (mainSegmentWidget->getViewVolume())
			mainSegmentWidget->getViewVolume()->renderLater();
	}
	else if (mainTabType == MAINTAB_MEASUREMENT)
	{
		if (mainAnalWidget->getViewVolume())
			mainAnalWidget->getViewVolume()->renderLater();
	}
	else if (mainTabType == MAINTAB_TA)
	{
		if (mainTAWidget->getViewVolume())
			mainTAWidget->getViewVolume()->renderLater();
	}
	else if (mainTabType == MAINTAB_MESH_EDITING)
	{
		if (mainMeshWidget->getMainView())
			mainMeshWidget->getMainView()->renderLater();
	}
	else if (mainTabType == MAINTAB_VISUAL_PRINT_MESH_EDITING)
	{
		if (pVisualPrintMeshWidget->getMainView())
			pVisualPrintMeshWidget->getMainView()->renderLater();
	}
}

void WindowManager::renderLater_All(bool volumeupdate)
{
	switch (mainTabType)
	{
	case MAINTAB_SEGMENTATION:
		if (mainSegmentWidget)
		{
			mainSegmentWidget->renderLater_GridView(volumeupdate);
			mainSegmentWidget->renderLater_SubView();
			break;
		}
	case MAINTAB_MESH_EDITING:
		if (mainMeshWidget)
		{
			mainMeshWidget->renderLater();
		}
		break;
	case MAINTAB_VISUAL_PRINT_MESH_EDITING:
		if (pVisualPrintMeshWidget)
		{
			pVisualPrintMeshWidget->renderLater();
			break;
		}
	case MAINTAB_MEASUREMENT:
		if (mainAnalWidget)
		{
			mainAnalWidget->renderLater_GridView(volumeupdate);
			break;
		}
	case MAINTAB_TA:
		if (mainTAWidget)
		{
			mainTAWidget->renderLater_GridView(volumeupdate);
			break;
		}
	}
}

void WindowManager::renderLater_AnalView(bool volumeupdate)
{
	if (mainAnalWidget)
	{
		mainAnalWidget->renderLater_GridView(volumeupdate);
	}
}

void WindowManager::renderLater_GridView(bool volumeupdate)
{
	if (mainTabType == MAINTAB_SEGMENTATION)
	{
		if (mainSegmentWidget)
		{
			mainSegmentWidget->renderLater_GridView(volumeupdate);
		}
	}
	else if (mainTabType == MAINTAB_MEASUREMENT)
	{
		if (mainAnalWidget)
		{
			mainAnalWidget->renderLater_GridView(volumeupdate);
		}
	}
	else if (mainTabType == MAINTAB_TA)
	{
		if (mainTAWidget)
		{
			mainTAWidget->renderLater_GridView(volumeupdate);
		}
	}
}

void WindowManager::renderLater_SubView()
{
	if (mainSegmentWidget)
	{
		mainSegmentWidget->renderLater_SubView();
	}
}

void WindowManager::buildRenderBufferTopology(mip::MeshTopology* _p_mesh, mip::SRenderBufferParams* _parmas)
{
	//MEVolumeView* view = mainMeshWidget->getMainView();
	//view->blockSignals(true);
	m_pRenderer->makeCurrent();
	if (_parmas)
	{
		_p_mesh->buildRenderBufferTopology(_parmas);
	}
	else
	{
		_p_mesh->buildRenderBufferTopology();
	}
	m_pRenderer->doneCurrent();

	//view->blockSignals(false);
}

void WindowManager::moveMeshCenterScene(bool _b_all)
{
	if (!mainMeshWidget)
	{
		return;
	}

	//m_pMeshModelViewManager->MoveScreenCenterMesh(_b_all);
}

void WindowManager::moveMeshCenterScene(int _idx)
{
	if (!mainMeshWidget)
	{
		return;
	}

	//m_pMeshModelViewManager->MoveScreenCenterMesh(_idx);
}

void WindowManager::selectSeedMode()
{
	mainSegmentWidget->setWorkMode(WORK_NONE);

	mainSegmentWidget->setWorkMode(WORK_SEED_SELECT);
}
void WindowManager::selectPatchyMode()
{
	mainSegmentWidget->setWorkMode(WORK_NONE);

	mainSegmentWidget->setWorkMode(WORK_PATCHY_SELECT);
}

void WindowManager::moveRegionMode()
{
	mainSegmentWidget->setWorkMode(WORK_NONE);

	mainSegmentWidget->setWorkMode(WORK_WORKING_REGION_MOVE);
}

float WindowManager::GetAniLength(int index, bool isSpiral, bool onlyLength, bool isMM)
{
	float fLength = 0.f;
	float fMM = isMM ? 10.f : 1.f;

	if (aniCount.size() < index || aniCount.at(index) < 2)
	{
		if (!onlyLength)
		{
			spiralLine.clear();
			spiralPos.clear();
		}
	}
	else
	{
		const int cnt = 10 * aniCount.at(index);
		int start, end;
		end = start = getAniStartPoint(index);
		end += aniCount.at(index);

		if (isSpiral)
		{
			QVector<QVector3D>* Lines = nullptr;
			QMap<int, int>* tmpPos = nullptr;
			if (onlyLength)
			{
				Lines = new QVector<QVector3D>();
				tmpPos = new QMap<int, int>();
			}
			else
			{
				Lines = &spiralLine;
				tmpPos = &spiralPos;
			}

			Lines->clear();
			tmpPos->clear();

			CRSpline sp;

			for (int i = start; i < end; i++)
				sp.AddSplinePoint(aniLine.at(i));

			int _pos = start;
			tmpPos->insert(_pos, 0);
			bool isContinue = true;
			for (float i = 0; i < cnt; i++)
			{
				float t = i / cnt;

				if (!isContinue)
				{
					break;
				}

				QVector3D line = sp.GetInterpolatedSplinePoint(t, _pos, isContinue);

				Lines->push_back(line);

				/*TODO::get length*/
				if (i != 0)
				{
					tmpPos->insert((_pos + start), i);

					QVector3D _v1, _v2;
					_v1 = Lines->at(i - 1);
					_v2 = Lines->at(i);
					fLength += CalcLength(_v1, _v2);
				}
			}

			if (onlyLength)
			{
				SAFE_DELETE(Lines);
				SAFE_DELETE(tmpPos);
			}
		}
		else
		{
			for (int i = (start + 1); i < end; i++)
			{
				QVector3D _v1, _v2;
				_v1 = aniLine.at(i - 1);
				_v2 = aniLine.at(i);
				fLength += CalcLength(_v1, _v2);
			}
		}
	}
	return (fMM * fLength);
}

float WindowManager::CalcLength(QVector3D vec1, QVector3D vec2, bool isVolume)
{
	if (isVolume)
	{
		//vec1, vec2 to world coord
		vec1.setX(m_pDataContext->volume_data.getSpaceX() * vec1.x() - m_pDataContext->volume_data.getSizeX() * 0.5f);
		vec1.setY(m_pDataContext->volume_data.getSpaceY() * vec1.y() - m_pDataContext->volume_data.getSizeY() * 0.5f);
		vec1.setZ(m_pDataContext->volume_data.getSpaceZ() * vec1.z() - m_pDataContext->volume_data.getSizeZ() * 0.5f);

		vec2.setX(m_pDataContext->volume_data.getSpaceX() * vec2.x() - m_pDataContext->volume_data.getSizeX() * 0.5f);
		vec2.setY(m_pDataContext->volume_data.getSpaceY() * vec2.y() - m_pDataContext->volume_data.getSizeY() * 0.5f);
		vec2.setZ(m_pDataContext->volume_data.getSpaceZ() * vec2.z() - m_pDataContext->volume_data.getSizeZ() * 0.5f);

	}

	return (vec1 - vec2).length();
}

void WindowManager::setLayer2DAlphaValue(int val, bool config, bool render)
{
	if (val < 0)
	{
		val = 0;
	}
	else if (val > 200)
	{
		val = 200;
	}

	if (val != m_Layer2DAlpha)
	{
		m_Layer2DAlpha = val;
	}

	if (config)
	{
		setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_Layer2DAlpha, QString::number(m_Layer2DAlpha));
	}

	if (render)
	{
		if (!m_pDataContext->volume_data.isValidate())
		{
			return;
		}
		//if (false == m_edge_mode) // 220714 허 건 과장(Boundary Mode 수행 시, 렌더링 업데이트가 안됨)
		{
			forceUpdate2DViewData(false, true);
			renderLater_GridView(false);
		}
	}
}

float WindowManager::getLayer2DAlphaValue()
{
	float val = m_Layer2DAlpha;

	val /= 200.0f;

	return val; //returns 0.0f~1.0f
}

void WindowManager::showControls(WINDOW_TYPE type)
{
	if (MAINTAB_MEASUREMENT == mainTabType)
	{
		mainAnalWidget->hideControls();
		mainAnalWidget->showControls(type);
	}
	else if (MAINTAB_SEGMENTATION == mainTabType)
	{
		mainSegmentWidget->hideControls();
		mainSegmentWidget->showControls(type);
	}
	else if (MAINTAB_TA == mainTabType)
	{
		mainTAWidget->hideControls();
		mainSegmentWidget->showControls(type);
	}
}

void WindowManager::hideControls()
{
	if (MAINTAB_MEASUREMENT == mainTabType)
	{
		mainAnalWidget->hideControls();
	}
	else if (MAINTAB_SEGMENTATION == mainTabType)
	{
		mainSegmentWidget->hideControls();
	}
	else if (MAINTAB_TA == mainTabType)
	{
		mainTAWidget->hideControls();
	}
}
void WindowManager::setDrawcutMode()
{
	//mainSegmentWidget->setWorkMode(WORK_NONE);

	mainSegmentWidget->setWorkMode(WORK_SKETCHDRAWSEGMENTATION);
	if (mainAnalWidget)
	{
		mainAnalWidget->setWorkMode(ANAL_WORK_SKETCHDRAWSEGMENTATION);
	}
	DrawCutOnOff(true);
}

void WindowManager::setDelMaskView(int index, bool isClear)
{
	MaskInfo* info = m_pDataContext->volume_data.getMaskInfo(index);

	if (info)
	{
		int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	}

	if (mainSegmentWidget)
	{
		mainSegmentWidget->delMaskView();
	}
}

muint32 WindowManager::getRawSize(QString fileName)
{
	QFile file(fileName);
	muint32 size = 0;
	if (file.exists() && file.open(QIODevice::ReadOnly))
	{
		size = file.size();
	}

	return size;
}

void WindowManager::setRegionGrowingSeed(muint32 x, muint32 y, muint32 z, mint16 Hu, bool layerRG)
{
	regionGrowingX = x;
	regionGrowingY = y;
	regionGrowingZ = z;
	regionGrowingHu = Hu;

	if (m_tabWindow && (!layerRG))
	{
		ThreSholdTab* tab = m_tabWindow->getThreSholdTab();

		if (tab)
		{
			tab->setRegionGrowing();
		}
	}

	//	mainSegmentWidget->setSeedText(x,y,z,Hu);
}

void WindowManager::setPatchyPoint(bool updateInfo)
{
	if (m_tabWindow)
	{
		WorkingRegionTab* tab = m_tabWindow->getPatchyTab();

		if (tab)
		{
			if (!updateInfo)
			{
				tab->SelectPoint();
			}
			else
			{
				tab->setRegionInfo(false);
			}
		}
	}
}

void WindowManager::deleteKeyProcess()
{
	if (bCaptureDel || bImportDel)
	{
		if (imgTabList)
		{
			ImageListTab* tab = imgTabList->getImgTab();

			if (tab)
			{
				tab->slot_OnDelete();
			}
		}

		bCaptureDel = false;
		bImportDel = false;
		return;
	}

	//mainSegmentWidget->getWindow(getLatestActiveViewType());
	Annotation* anno;
	for (mint32 n = 0; n < anotationList.size(); n++)
	{
		anno = anotationList[n];
		if (anno->isSelected())
		{
			m_pActionManager->action_Annotation_Del(nullptr, n);
			return;
		}
	}

}

void WindowManager::escKeyProcess()
{
	if (!m_pDataContext->volume_data.isValidate())
	{
		return;
	}

	if (mainTabType == MAINTAB_SEGMENTATION)
		mainSegmentWidget->setWorkMode(WORK_NONE);
	else if (mainTabType == MAINTAB_MEASUREMENT)
		mainAnalWidget->setWorkMode(ANAL_WORK_NONE);
	else if (mainTabType == MAINTAB_TA)
		mainTAWidget->setWorkMode(RADIOMICS_WORK_NONE);
	else if (mainTabType == MAINTAB_MESH_EDITING)
		mainMeshWidget->setWorkMode(MESH_WORK_NONE);
	else if (mainTabType == MAINTAB_VISUAL_PRINT_MESH_EDITING)
		pVisualPrintMeshWidget->setWorkMode(VISUAL_PRINT_MESH_WORK_NONE);
}

void WindowManager::openBracketProcess()
{
	const int minSize_S = 1;
	const int minSize_A = 10;

	if (!m_pDataContext->volume_data.isValidate())
	{
		return;
	}

	bool chg = false;

	if (mainTabType == MAINTAB_SEGMENTATION)
	{
		bool bType = (m_workMode == WORK_RECTROI || getDCutShape() == DRAW_CUT_SHAPE_ANGULAR);
		if (bType) //todo
		{
			if (minSize_S < AngularSize_S)
			{
				AngularSize_S--;
				chg = true;
			}
			else if (!pencil_S)
			{
				pencil_S = true;
				chg = true;
			}
		}
		else if (m_workMode == WORK_OVALROI || getDCutShape() == DRAW_CUT_SHAPE_CIRCULAR)
		{
			if (minSize_S < CirculSize_S)
			{
				CirculSize_S--;
				chg = true;
			}
			else if (!oval_S)
			{
				oval_S = true;
				chg = true;
			}
		}
		if (chg)
		{
			mainSegmentWidget->changeShapeSize();
			mainSegmentWidget->renderLater_GridView(false);
		}
	}
	else if (mainTabType == MAINTAB_MEASUREMENT)
	{
		bool bType = (m_analworkMode == ANAL_WORK_SEGMENT_RECT || getDCutShape() == DRAW_CUT_SHAPE_ANGULAR);

		if (bType)//todo
		{
			if (minSize_A < AngularSize_A)
			{
				AngularSize_A--;
				chg = true;
			}
			else if (!pencil_A)
			{
				pencil_A = true;
				chg = true;
			}

		}
		else if (m_analworkMode == ANAL_WORK_SEGMENT_OVAL || getDCutShape() == DRAW_CUT_SHAPE_CIRCULAR)
		{
			if (minSize_A < CirculSize_A)
			{
				CirculSize_A--;
				chg = true;
			}
			else if (!oval_A)
			{
				oval_A = true;
				chg = true;
			}
		}

		if (chg)
		{
			mainAnalWidget->changeShapeSize();
			mainAnalWidget->renderLater_GridView(false);
		}
	}
}

void WindowManager::closeBracketProcess()
{
	const int maxSize_S = 500;
	const int maxSize_A = 300;

	if (!m_pDataContext->volume_data.isValidate())
	{
		return;
	}
	bool chg = false;

	if (mainTabType == MAINTAB_SEGMENTATION)
	{
		bool bType = (m_workMode == WORK_RECTROI || getDCutShape() == DRAW_CUT_SHAPE_ANGULAR);
		if (bType) //todo
		{
			if (pencil_S)
			{
				pencil_S = false;
				chg = true;
			}
			else if (maxSize_S > AngularSize_S)
			{
				AngularSize_S++;
				chg = true;
			}

		}
		else if (m_workMode == WORK_OVALROI || getDCutShape() == DRAW_CUT_SHAPE_CIRCULAR)
		{
			if (oval_S)
			{
				oval_S = false;
				chg = true;
			}
			else if (maxSize_S > CirculSize_S)
			{
				CirculSize_S++;
				chg = true;
			}
		}
		if (chg)
		{
			mainSegmentWidget->changeShapeSize();
			mainSegmentWidget->renderLater_GridView(false);
		}
	}
	else if (mainTabType == MAINTAB_MEASUREMENT)
	{
		bool bType = (m_analworkMode == ANAL_WORK_SEGMENT_RECT || getDCutShape() == DRAW_CUT_SHAPE_ANGULAR);

		if (bType)//todo
		{
			if (pencil_A)
			{
				pencil_A = false;
				chg = true;
			}
			else if (maxSize_A > AngularSize_A)
			{
				AngularSize_A++;
				chg = true;
			}
		}
		else if (m_analworkMode == ANAL_WORK_SEGMENT_OVAL || getDCutShape() == DRAW_CUT_SHAPE_CIRCULAR)
		{
			if (oval_A)
			{
				oval_A = false;
				chg = true;
			}
			else if (maxSize_A > CirculSize_A)
			{
				CirculSize_A++;
				chg = true;
			}
		}
		if (chg)
		{
			mainAnalWidget->changeShapeSize();
			mainAnalWidget->renderLater_GridView(false);
		}
	}
}

void WindowManager::enableTOIAll()
{
	mainSegmentWidget->enableTOIAll();
}

void WindowManager::updateUI(bool isSelection, int selection)
{
	if (m_tabWindow)
	{
		ROITab2* tab = m_tabWindow->getROITab();
		if (tab)
		{
			tab->updateROIList(isSelection, selection);
		}
	}

	if (mainSegmentWidget)
	{
		mainSegmentWidget->initWindows();
	}
}

void WindowManager::updateUIMultiSelect(const std::vector<muint32>& vecMultiMaskUID)
{
	if (m_tabWindow)
	{
		ROITab2* tab = m_tabWindow->getROITab();
		if (tab)
		{
			tab->updateROIListMultiMask(vecMultiMaskUID);
		}
	}

	mainSegmentWidget->initWindows();
}

void WindowManager::updateMeshUI(bool refresh, int selIndex, bool _b_clear, bool b_close_dlg)
{
	if (m_tabWindow)
	{
		MeshTab* tab = m_tabWindow->getMeshTab();
		if (tab)
		{
			tab->Update(refresh, selIndex, _b_clear, b_close_dlg);
		}

		if (b_close_dlg)
		{
			if (m_pDataContext->m_MeshData.GetMeshCount() > 0)
			{
				if (tab)
				{
					tab->setOpenWidget(true);
				}
			}
		}
	}
}

void WindowManager::updateVisualPrintMeshUI(bool refresh, int selIndex, bool _b_clear)
{
	if (m_tabWindow)
	{
		auto tab = m_tabWindow->getVisualPrintMeshTab();
		if (tab)
		{
			tab->Update(refresh, selIndex);
		}
	}
}

void WindowManager::updateSeed()
{
	DrawcutTab* tab = m_tabWindow->getDrawTab();

	if (tab)
		tab->UpdateSeed(true);
}

void WindowManager::updateSummary()
{
	if (m_tabWindow)
	{
		SummaryTab* tab = m_tabWindow->getSummaryTab();

		if (tab)
			tab->setSummary();
	}
}

bool WindowManager::isDicomSeriesInfoShouldBeUpdated()
{
	DcmtkSeriesInfo* pDicomSeriesInfo = GetDicomInfo();
	return
		pDicomSeriesInfo->strStudyUID.empty() ||
		pDicomSeriesInfo->accessionNumber.empty() ||
		pDicomSeriesInfo->examID.empty();
}

bool WindowManager::importDicomFile_And_UpdateSummary()
{
#ifdef DEV_FILE_MANAGER
	DcmtkSeriesInfo* pDicomSeriesInfo = GetDicomInfo();

	/* Upload 필수 정보가 없을 경우 업데이트 진행 */
	QString strFilter;
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_DICOM))
	{
		strFilter += QString("DICOM File(*.dcm; *.DCM);;");
	}

	QString filepath = ImportFileDialog(mainWindow,
		STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN), "", "",
		strFilter);

	if (filepath.isEmpty())
	{
		return false;
	}

	DcmtkSeriesInfo newDcmSeriesInfo;
	if (mip::DcmtkVolumeReader::LoadSeriesInfo(filepath.toStdWString(), newDcmSeriesInfo) == false)
	{
		/* ERROR. invalid dicom file.*/
		return false;
	}

	/* DICOM 정보 업데이트 */
	pDicomSeriesInfo->strStudyUID = newDcmSeriesInfo.strStudyUID;
	pDicomSeriesInfo->strSeriesUID = newDcmSeriesInfo.strSeriesUID;
	pDicomSeriesInfo->studyDate_ = newDcmSeriesInfo.studyDate_;
	pDicomSeriesInfo->studyTime_ = newDcmSeriesInfo.studyTime_;
	pDicomSeriesInfo->modality_ = newDcmSeriesInfo.modality_;
	pDicomSeriesInfo->seriesDate = newDcmSeriesInfo.seriesDate;
	pDicomSeriesInfo->accessionNumber = newDcmSeriesInfo.accessionNumber;
	pDicomSeriesInfo->examID = newDcmSeriesInfo.examID;
	pDicomSeriesInfo->patientId_ = newDcmSeriesInfo.patientId_;
	//pDicomSeriesInfo->patientsName_ = newDcmSeriesInfo.patientsName_;
	pDicomSeriesInfo->age_ = newDcmSeriesInfo.age_;
	pDicomSeriesInfo->sex_ = newDcmSeriesInfo.sex_;

	updateSummary();

	return true;
#endif
}

void WindowManager::applyVoxelToUI(int index, bool seed)
{
	if (m_tabWindow)
	{
		if (!seed)
		{
			ROITab2* tab = m_tabWindow->getROITab();

			if (tab)
				tab->applyVoxel(index);
		}
#ifndef MULTI_DRAWCUT_MODE
		else
#endif
		{
			DrawcutTab* tab = m_tabWindow->getDrawTab();

			if (tab)
				tab->UpdateVoxel();
		}
	}
	//	mainSegmentWidget->applyVoxelToUI(index);
}

void WindowManager::resetRGValue()
{
	if (m_tabWindow)
	{
		ThreSholdTab* tab = m_tabWindow->getThreSholdTab();

		if (tab)
			tab->Init();
	}
	regionGrowingHu = regionGrowingX = regionGrowingY = regionGrowingZ = -1;
}

void WindowManager::setAIRange(bool init)
{
	if (m_tabWindow)
	{
		AISegTab* tab = m_tabWindow->getAITab();
		// 		if (tab)
		// 		{
		// 			tab->setAIRange(init);
		// 		}
	}
}

/*@function setAIDepth
 *@brief depth range set
 *@param start depth start point
 *@param to depth end point
 *@param _init initial setting (default true), false --> only change automatically checked
 */
void WindowManager::setAIDepth(BoundingBoxI box)
{
	if (m_tabWindow)
	{
		AISegTab* tab = m_tabWindow->getAITab();

		if (tab)
		{
			//tab->m_tabSet->setDepth(true);								
			tab->UpdateDepth(true);
		}
	}
}

void WindowManager::settingOutset()
{
	if ((!LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH) ||
		!LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH_V2)) && !m_isPredictComplete)
	{
		return;
	}

	AISegTab* tab = m_tabWindow->getAITab();

	int sliderOutVal = 0;
	if (IsAIOutsetMaskSelected(&sliderOutVal))
	{
		if (tab && tab->m_tabSet)
		{
			tab->m_tabSet->setEnableObject(true);
			tab->m_tabSet->setSliderOutVal(sliderOutVal);
		}

		if ((!LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH) ||
			!LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH_V2)) && !m_pDataContext->volume_data.isMaskEditable())
			SetEnableViewControls(true);
		else
			SetEnableViewControls(false);
	}
	else
	{
		if (tab && tab->m_tabSet)
		{
			tab->m_tabSet->setEnableObject(false);
		}
		SetEnableViewControls(true);
	}
}

void WindowManager::updateAnnotation()
{
	if (m_tabWindow)
	{
		AnnotationTab2* annotationTab = m_tabWindow->getAnnoTab();

		if (annotationTab)
		{
			annotationTab->UpdateAnnoList(mainTabType);
		}

		AnimationTab* animationTab = m_tabWindow->getAniTab();

		if (animationTab)
		{
			animationTab->UpdateAniList();
		}
	}

	if (imgTabList)
	{
		ImageListTab* tab = imgTabList->getImgTab();

		if (tab)
		{
			tab->UpdateList();
		}
	}

}

void WindowManager::clearFeatureList()
{
	if (m_tabWindow)
	{
		AnalysisTab* tab = m_tabWindow->getAnalysisTab();
		if (tab)
		{
			tab->ClearResult();
		}
	}

	pRadiomics3DVolume = nullptr;
	std::vector<std::pair<float, float>>().swap(vecRadiomicsColorCategory);
	bRadiomicsDataChangeFlag[0] = false;
	bRadiomicsDataChangeFlag[1] = false;
	bRadiomicsDataChangeFlag[2] = false;

}

void WindowManager::resetUI()
{
	if (mainSegmentWidget)
	{
		mainSegmentWidget->resetUI();
	}
	if (mainAnalWidget)
	{
		mainAnalWidget->resetUI();
	}
	if (mainTAWidget)
	{
		mainTAWidget->resetUI();
	}
	if (m_tabWindow)
	{
		m_tabWindow->ResetUI();
	}
}

mask WindowManager::getSelectedMask()
{
	MaskInfo* info = m_pDataContext->volume_data.getCurrentMaskInfo();
	if (info == 0)
		return VM_MASK2;

	if (info->uid >= MASK_SECOND_MAX)
		return info->mask_id2;

	return info->mask_id;
}

int WindowManager::getSelectedMaskByteIndex()
{
	MaskInfo* info = m_pDataContext->volume_data.getCurrentMaskInfo();
	if (info == 0) return 0;

	if (info->uid >= MASK_SECOND_MAX)
		return (info->uid - MASK_SECOND_MAX) / 8 + 1;

	return 0;
}

int WindowManager::getSelectedMaskUID()
{
	MaskInfo* info = m_pDataContext->volume_data.getCurrentMaskInfo();
	if (info == 0) return -1;

	return info->uid;
}

QColor WindowManager::getSelectedMaskColor()
{
	MaskInfo* info = m_pDataContext->volume_data.getCurrentMaskInfo();
	if (info == 0) return QColor(Qt::red);

	return QColor(info->color.r, info->color.g, info->color.b);
}

void WindowManager::setPreset(SLICE_PRESET type, bool reset, int cusPre)
{
	if (m_tabWindow)
	{
		Visualize2DTab* tab = m_tabWindow->get2DTab();

		if (tab)
			tab->setPreset(type, reset, cusPre);
	}
}

void WindowManager::setVolumePreset(SLICE_PRESET type, bool reset, int cusPre)
{
	if (m_tabWindow)
	{
		Visualize3DTab* tab = m_tabWindow->get3DTab();

		if (tab)
			tab->setPreset(type, reset, cusPre);
	}
}

void WindowManager::setAutoScaleSlope(float val)
{
	if (m_tabWindow)
	{
		Visualize2DTab* tab = m_tabWindow->get2DTab();

		if (tab)
			tab->setAutoScaleSlope(val);
	}
}

void WindowManager::setWindowWidth(int width)
{
	m_window_width = width;

	if (m_tabWindow)
	{
		Visualize2DTab* tab = m_tabWindow->get2DTab();

		if (tab)
			tab->setWidth(width);
	}

	//	mainSegmentWidget->setUIWidth(width);
}

void WindowManager::setWindowLevel(int level)
{
	m_window_level = level;

	if (m_tabWindow)
	{
		Visualize2DTab* tab = m_tabWindow->get2DTab();

		if (tab)
			tab->setLevel(level);
	}

	//	mainSegmentWidget->setUILevel(level);
}

void WindowManager::setVolumeWidth(int width)
{
	m_volume_width = width;
	/**/
	if (m_tabWindow)
	{
		Visualize3DTab* tab = m_tabWindow->get3DTab();

		if (tab)
			tab->setWidth(width);
	}
	//	mainSegmentWidget->setVolumeUIWidth(width);
}

void WindowManager::setVolumeLevel(int level)
{
	m_volume_level = level;
	/**/
	if (m_tabWindow)
	{
		Visualize3DTab* tab = m_tabWindow->get3DTab();

		if (tab)
			tab->setLevel(level);
	}
	//	mainSegmentWidget->setVolumeUILevel(level);
}

void WindowManager::addVolumeWidth(int width)
{
	m_volume_width += width;

	if (m_tabWindow)
	{
		Visualize3DTab* tab = m_tabWindow->get3DTab();

		if (tab)
			tab->setWidth(m_volume_width);
	}

	//	mainSegmentWidget->setVolumeUIWidth(width);
}

void WindowManager::addVolumeLevel(int level)
{
	m_volume_level += level;

	if (m_tabWindow)
	{
		Visualize3DTab* tab = m_tabWindow->get3DTab();

		if (tab)
			tab->setLevel(m_volume_level);
	}
	//	mainSegmentWidget->setVolumeUILevel(level);
}

void WindowManager::setVolumeWidthLevel(int width, int level)
{
	setVolumeWidth(width);
	setVolumeLevel(level);
}

void WindowManager::setWindowWidthLevel(int width, int level)
{
	setWindowWidth(width);
	setWindowLevel(level);
}

void WindowManager::rejectMEViewDlg(int old_idx)
{
	if (mainMeshWidget && MESH_DIALOG_MANAGER->isMeshDialog())
	{
		if (mainTabType != MAINTAB_MESH_EDITING)
			MESH_DIALOG_MANAGER->rejectDialog(old_idx, true);
		else
			MESH_DIALOG_MANAGER->rejectDialog(old_idx, false);
	}
}

void WindowManager::showMeshDlg(int mode)
{
	MAINTAB_TYPE type = mainTabType;
	if (type == MAINTAB_SEGMENTATION || type == MAINTAB_MEASUREMENT)
	{
		mainWindow->callMainTabchange((int)MAINTAB_MESH_EDITING - 1);
	}

	if (mainMeshWidget)
		MESH_DIALOG_MANAGER->makeMeshDialog(mode);
}

void WindowManager::MoveMeshDialog(int fixed)
{
	if (mainMeshWidget && MESH_DIALOG_MANAGER->isMeshDialog())
		MESH_DIALOG_MANAGER->MoveMeshDialog(fixed);
}

bool WindowManager::buildup3DVolumeTexture(progUpdatefunc update, void* data)
{
	if (m_pRenderer->isAvailableVolumeRender() == false || getShaderQuality() == SQ_INVISIBLE)
	{
		return buildupLowSpecVolume(data);
	}

	muint32 xlen = m_pDataContext->volume_data.getCX();
	muint32 ylen = m_pDataContext->volume_data.getCY();
	muint32 zlen = m_pDataContext->volume_data.getCZ();

	if (m_nTexVolume != 0)
	{
		m_pRenderer->updateVolumeTextureGL(m_nTexVolume, m_pDataContext->volume_data.getHUDataPoint());
	}
	else
	{
		m_nTexVolume = m_pRenderer->createVolumeTextureGL(xlen, ylen, zlen, mip::TEXTYPE::TEXT_VOLUME, mip::TEXFORMAT::TEXF_R16I, m_pDataContext->volume_data.getHUDataPoint());
	}


	if (m_nTexVolume == 0)
	{
		//mip::LogDebug("Failed Texture Loading !!");
		return false;
	}

	return true;
}

bool WindowManager::buildup2DMPR(progUpdatefunc update, void* data)
{
	muint32 xlen = m_pDataContext->volume_data.getCX();
	muint32 ylen = m_pDataContext->volume_data.getCY();
	muint32 zlen = m_pDataContext->volume_data.getCZ();

	mint16 level = getWindowLevel();
	muint16 width = getWindowWidth() / 2;
	int min = level - width;
	int max = level + width;

	float rangef = (max - min) == 0 ? 1 : max - min;

	float rate = 1.0f / rangef;
	float ColOne;
	QColor _color;
	muint32 size = 0;

	if (m_pDataContext->volume_data.axialPlaneData[1])
	{
		size = xlen * ylen;
		mip::COLOR* color = new mip::COLOR[size];

		for (int n = 0; n < size; n++)
		{
			ColOne = rate * float(m_pDataContext->volume_data.axialPlaneData[1][n] - min);
			ColOne *= 255;

			if (ColOne < 0)
				ColOne = 0;
			else if (ColOne > 255)
				ColOne = 255;

			if (m_color2D)
				_color = m_pDataContext->volume_data.get2Dcolor(ColOne);
			else
			{
				if (m_invertingLUT)
					_color = QColor(255 - ColOne, 255 - ColOne, 255 - ColOne);
				else
					_color = QColor(ColOne, ColOne, ColOne);
			}

			//	color[n] = mip::COLOR(mip::FCOLOR(ColOne, ColOne, ColOne,1.0f));
			color[n] = mip::COLOR(_color.blue(), _color.green(), _color.red(), 1.0f);
		}

		if (m_nTex2DAxial != 0)
		{
			m_pRenderer->updateTextureGL(m_nTex2DAxial, color);
		}
		else
		{
			m_nTex2DAxial = m_pRenderer->createTextureGL(xlen, ylen, mip::TEXTYPE::TEXT_USER, color);
		}

		SAFE_DELETES(color);

		if (m_nTex2DAxial == 0)
		{
			mip::LogDebug("Failed Texture Loading !!");
			return false;
		}
	}

	if (m_pDataContext->volume_data.coronalPlaneData[1])
	{
		size = xlen * zlen;
		mip::COLOR* color = new mip::COLOR[size];

		for (int n = 0; n < size; n++)
		{
			ColOne = rate * float(m_pDataContext->volume_data.coronalPlaneData[1][n] - min);
			ColOne *= 255;

			if (ColOne < 0)
				ColOne = 0;
			else if (ColOne > 255)
				ColOne = 255;

			if (m_color2D)
				_color = m_pDataContext->volume_data.get2Dcolor(ColOne);
			else
			{
				if (m_invertingLUT)
					_color = QColor(255 - ColOne, 255 - ColOne, 255 - ColOne);
				else
					_color = QColor(ColOne, ColOne, ColOne);
			}

			//	color[n] = mip::COLOR(mip::FCOLOR(ColOne, ColOne, ColOne,1.0f));
			color[n] = mip::COLOR(_color.blue(), _color.green(), _color.red(), 1.0f);
		}

		if (m_nTex2DCoronal != 0)
		{
			m_pRenderer->updateTextureGL(m_nTex2DCoronal, color);
		}
		else
		{
			m_nTex2DCoronal = m_pRenderer->createTextureGL(xlen, zlen, mip::TEXTYPE::TEXT_USER, color);
		}

		SAFE_DELETES(color);

		if (m_nTex2DCoronal == 0)
		{
			mip::LogDebug("Failed Texture Loading !!");
			return false;
		}
	}


	if (m_pDataContext->volume_data.sagittalPlaneData[1])
	{
		size = ylen * zlen;
		mip::COLOR* color = new mip::COLOR[size];

		for (int n = 0; n < size; n++)
		{
			ColOne = rate * float(m_pDataContext->volume_data.sagittalPlaneData[1][n] - min);
			ColOne *= 255;

			if (ColOne < 0)
				ColOne = 0;
			else if (ColOne > 255)
				ColOne = 255;

			if (m_color2D)
				_color = m_pDataContext->volume_data.get2Dcolor(ColOne);
			else
			{
				if (m_invertingLUT)
					_color = QColor(255 - ColOne, 255 - ColOne, 255 - ColOne);
				else
					_color = QColor(ColOne, ColOne, ColOne);
			}

			//	color[n] = mip::COLOR(mip::FCOLOR(ColOne, ColOne, ColOne,1.0f));
			color[n] = mip::COLOR(_color.blue(), _color.green(), _color.red(), 1.0f);
		}

		if (m_nTex2DSagittal != 0)
		{
			m_pRenderer->updateTextureGL(m_nTex2DSagittal, color);
		}
		else
		{
			m_nTex2DSagittal = m_pRenderer->createTextureGL(ylen, zlen, mip::TEXTYPE::TEXT_USER, color);
		}

		SAFE_DELETES(color);

		if (m_nTex2DSagittal == 0)
		{
			mip::LogDebug("Failed Texture Loading !!");
			return false;
		}
	}


	return true;
}

muint32 WindowManager::get2DMPR(WINDOW_TYPE type)
{
	switch (type)
	{
	case WT_CORONAL:
		return get2DMPR_Coronal();
	case WT_SAGITTAL:
		return get2DMPR_Sagittal();
	default:
		return get2DMPR_Axial();
	}
}

muint32 WindowManager::getLowSpec2DMPR(WINDOW_TYPE type)
{
	switch (type)
	{
	case WT_CORONAL:
		return m_nTexLowVolumeCoronal;
	case WT_SAGITTAL:
		return m_nTexLowVolumeSagittal;
	default:
		return m_nTexLowVolumeAxial;
	}
}

bool WindowManager::createLowSpecTexture(WINDOW_TYPE type, muint32& texLowVolume, int width, int height)
{
	mint16 window_level = getWindowLevel();
	muint16 window_width = getWindowWidth() / 2;

	int min = window_level - window_width;
	int max = window_level + window_width;

	float rate = 1.0f / float(max - min);

	mip::COLOR* color = new mip::COLOR[width * height];
	uint nSlice = mainSegmentWidget->getWindow(type)->getDepth();

	for (int h = 0; h < height; ++h)
	{
		for (int w = 0; w < width; ++w)
		{
			float ColOne = 0.0f;

			if (type == WT_AXIAL)
				ColOne = rate * float(m_pDataContext->volume_data.getData(w, h, nSlice) - min);
			else if (type == WT_CORONAL)
				ColOne = rate * float(m_pDataContext->volume_data.getData(w, nSlice, h) - min);
			else if (type == WT_SAGITTAL)
				ColOne = rate * float(m_pDataContext->volume_data.getData(nSlice, w, h) - min);

			ColOne *= 255;
			QColor _color = m_pDataContext->volume_data.get2Dcolor(ColOne);
			//	color[n] = mip::COLOR(mip::FCOLOR(ColOne, ColOne, ColOne,1.0f));
			color[h * width + w] = mip::COLOR(_color.blue(), _color.green(), _color.red(), 1.0f);

			//	color[h * width + w] = mip::COLOR(mip::FCOLOR(ColOne, ColOne, ColOne, 1.0f));
		}
	}

	if (texLowVolume != 0)
		m_pRenderer->updateTextureGL(texLowVolume, color);
	else
		texLowVolume = m_pRenderer->createTextureGL(width, height, mip::TEXTYPE::TEXT_USER, color);
	SAFE_DELETES(color);

	if (texLowVolume == 0)
	{
		mip::LogDebug("Failed Texture Loading !!");
		return false;
	}
	return true;
}

bool WindowManager::buildupLowSpecVolume(void* data)
{
	muint32 xlen = m_pDataContext->volume_data.getCX();
	muint32 ylen = m_pDataContext->volume_data.getCY();
	muint32 zlen = m_pDataContext->volume_data.getCZ();

	if (createLowSpecTexture(WT_AXIAL, m_nTexLowVolumeAxial, xlen, ylen) == false) return false;
	if (createLowSpecTexture(WT_CORONAL, m_nTexLowVolumeCoronal, xlen, zlen) == false) return false;
	if (createLowSpecTexture(WT_SAGITTAL, m_nTexLowVolumeSagittal, ylen, zlen) == false) return false;

	return true;
}

bool WindowManager::buildup3DMaskTexture(progUpdatefunc update, void* data)
{
	if (m_pRenderer->isAvailableVolumeRender() == false)
	{
		return false;
	}

	muint32 xlen = m_pDataContext->volume_data.getCX();
	muint32 ylen = m_pDataContext->volume_data.getCY();
	muint32 zlen = m_pDataContext->volume_data.getCZ();

	static int dataType = 0;
	mip::TEXFORMAT type;
	int infoSize;

	if (m_pDataContext->volume_data.isValidate())
		infoSize = m_pDataContext->volume_data.getMaskInfoListCnt();
	else
		infoSize = 0;

	if (MASK_SECOND_MAX >= infoSize)
		type = mip::TEXF_R8UI;
	else if (14 >= infoSize)
		type = mip::TEXF_R16UI;
	else
		type = mip::TEXF_R32UI;;


	void* BuffData = create3DMaskTexture(update, data, type);

	if (BuffData != nullptr)
	{
		if (m_nTexMask != 0)
		{
			if (type == dataType)
			{
				m_pRenderer->updateVolumeTextureGL(m_nTexMask, BuffData);
			}
			else
			{
				dataType = type;
				m_pRenderer->deleteTextureLater(m_nTexMask);
				m_nTexMask = m_pRenderer->createVolumeTextureGL(xlen, ylen, zlen, mip::TEXTYPE::TEXT_MASK, (mip::TEXFORMAT)dataType, BuffData);
			}
		}
		else
		{
			dataType = type;
			m_nTexMask = m_pRenderer->createVolumeTextureGL(xlen, ylen, zlen, mip::TEXTYPE::TEXT_MASK, (mip::TEXFORMAT)dataType, BuffData);
		}
	}
	else
	{
		mip::LogDebug(" Failed Texture Update !!");
	}

	if (type > mip::TEXF_R8UI) SAFE_DELETES(BuffData);

	if (m_nTexMask == 0)
	{
		//mip::LogDebug("Failed Texture Loading !!");
		return false;
	}

	return true;
}

void* WindowManager::create3DMaskTexture(progUpdatefunc update, void* data, int type)
{
	muint32 xlen = m_pDataContext->volume_data.getCX();
	muint32 ylen = m_pDataContext->volume_data.getCY();
	muint32 zlen = m_pDataContext->volume_data.getCZ();
	if (type == mip::TEXF_R8UI)
	{
		return m_pDataContext->volume_data.getMaskDataPoint();
	}

	else if (type == mip::TEXF_R16UI)
	{
		muint16* BuffData = nullptr;
		try
		{
			BuffData = new muint16[xlen * ylen * zlen];
		}
		catch (...)
		{
			SAFE_DELETES(BuffData);
			return nullptr;
		}

		memset(BuffData, 0, sizeof(muint16) * (xlen * ylen * zlen));
		for (int z = 0; z < zlen; z++)
		{
			int yr = 0;
			for (int y = 0; y < ylen; y++, yr++)
			{
				for (int x = 0; x < xlen; x++)
				{
					int n = (z) * (xlen * ylen) + y * xlen + x;

					muint16 val = m_pDataContext->volume_data.getMaskData(n);
					muint16 val2 = m_pDataContext->volume_data.getMaskData(n, 1) << 8;

					BuffData[n] = val | val2;

				}
			}

			if (update)
			{
				update(100 * z / (float)zlen, data);
			}
		}

		return BuffData;
	}
	else
	{
		muint32* BuffData = nullptr;

		try
		{
			BuffData = new muint32[xlen * ylen * zlen];
		}
		catch (...)
		{
			SAFE_DELETES(BuffData);
			return nullptr;
		}

		memset(BuffData, 0, sizeof(muint32) * (xlen * ylen * zlen));
		for (int z = 0; z < zlen; z++)
		{
			int yr = 0;
			for (int y = 0; y < ylen; y++, yr++)
			{
				for (int x = 0; x < xlen; x++)
				{
					int n = (z) * (xlen * ylen) + y * xlen + x;

					muint32 val = m_pDataContext->volume_data.getMaskData(n);
					muint32 val2 = m_pDataContext->volume_data.getMaskData(n, 1) << 8;
					muint32 val3 = m_pDataContext->volume_data.getMaskData(n, 2) << 16;
					muint32 val4 = m_pDataContext->volume_data.getMaskData(n, 3) << 24;
					BuffData[n] = val | val2 | val3 | val4;
				}
			}

			if (update)
				update(100 * z / (float)zlen, data);
		}

		return BuffData;
	}
}

bool WindowManager::resetTextures()
{
	resetLowSpecTextures();

	m_pRenderer->deleteTextureLater(m_nTexMask);
	m_pRenderer->deleteTextureLater(m_nTexVolume);

	m_pRenderer->deleteTextureLater(m_nTex2DAxial);
	m_pRenderer->deleteTextureLater(m_nTex2DCoronal);
	m_pRenderer->deleteTextureLater(m_nTex2DSagittal);
	//	m_pRenderer->deleteTextureLater(nTex2DTF);

	m_nTexMask = 0;
	m_nTexVolume = 0;

	m_nTex2DAxial = 0;
	m_nTex2DCoronal = 0;
	m_nTex2DSagittal = 0;

	return true;
}

bool WindowManager::resetLowSpecTextures()
{
	m_pRenderer->deleteTextureLater(m_nTexLowVolumeAxial);
	m_pRenderer->deleteTextureLater(m_nTexLowVolumeCoronal);
	m_pRenderer->deleteTextureLater(m_nTexLowVolumeSagittal);

	m_nTexLowVolumeAxial = 0;
	m_nTexLowVolumeCoronal = 0;
	m_nTexLowVolumeSagittal = 0;

	return true;
}

bool WindowManager::resetMaskTexture()
{
	//m_pRenderer->deleteTexture(nTexMask);
	//nTexMask = 0;

	return true;
}

bool WindowManager::initPresetTexture()
{
	if (m_nTexTF != 0)
		m_pRenderer->deleteTexture(m_nTexTF);

	if (m_nTex2DTF != 0)
		m_pRenderer->deleteTexture(m_nTex2DTF);

	mip::COLOR* pTextureBuffer = new mip::COLOR[512 * 512];
	{
		int yr = 0;
		for (int y = 0; y < 512; y++, yr++)
		{
			for (int x = 0; x < 512; x++)
			{
				if (y < 3) // 0,1,2
					pTextureBuffer[x + (y * 512)] = mip::COLOR(x / 2, x / 2, x / 2, x / 2); // BGRA
				//	pTextureBuffer[x+(y*512)] = mip::COLOR(0,0,x,x); // BGRA
				else if (y < 6) // 3,4,5 
					pTextureBuffer[x + (y * 512)] = mip::COLOR(x / 2, 0, 0, x / 2); // BGRA
				else if (y < 9)
					pTextureBuffer[x + (y * 512)] = mip::COLOR(0, x / 2, 0, x / 2); // BGRA
				else
				{
					pTextureBuffer[x + (y * 512)] = mip::COLOR(x / 2, x / 2, x / 2, x / 2); // BGRA
					//		pTextureBuffer[x + (y * 512)] = getVolumeColor(x);
				}
			}
		}
	}

	m_nTexTF = m_pRenderer->createTextureGL(512, 512, mip::TEXT_USER2, pTextureBuffer);

	SAFE_DELETES(pTextureBuffer);

	pTextureBuffer = new mip::COLOR[512 * 512];
	{
		int yr = 0;
		for (int y = 0; y < 512; y++, yr++)
		{
			for (int x = 0; x < 512; x++)
			{
				if (y < 3) // 0,1,2
					pTextureBuffer[x + (y * 512)] = mip::COLOR(x / 2, x / 2, x / 2, x / 2); // BGRA
				//	pTextureBuffer[x+(y*512)] = mip::COLOR(0,0,x,x); // BGRA
				else if (y < 6) // 3,4,5 
					pTextureBuffer[x + (y * 512)] = mip::COLOR(x / 2, 0, 0, x / 2); // BGRA
				else if (y < 9)
					pTextureBuffer[x + (y * 512)] = mip::COLOR(0, x / 2, 0, x / 2); // BGRA
				else
				{
					pTextureBuffer[x + (y * 512)] = mip::COLOR(x / 2, x / 2, x / 2, x / 2); // BGRA
					//		pTextureBuffer[x + (y * 512)] = getVolumeColor(x);
				}
			}
		}
	}

	m_nTex2DTF = m_pRenderer->createTextureGL(512, 512, mip::TEXT_USER2, pTextureBuffer);

	SAFE_DELETES(pTextureBuffer);


	return true;
}


//todo del preset texture,,,,etc,,,
bool WindowManager::updatePresetTexture(SLICE_PRESET type, int cusType, bool delCus)
{
	makeCurrent();

	static QVector<QColor> colortable[SP_COUNT + 1];
	static QVector<int> alphaTable[SP_COUNT + 1];
	static bool init = false;
	static QVector<QVector<QColor>> Cuscolortable;
	static QVector<QVector<int>> Cusalphatable;
	static int CusCount = m_presetNameList.size();

	if (!init)
	{
		init = true;
		for (int index = 0; index < SP_COUNT; index++)
		{
			colortable[index] = AlphaColorMap::getColorTable((SLICE_PRESET)index, CL_3D);
			alphaTable[index] = HoverPoints::getAlphaGraph(CL_3D, (SLICE_PRESET)index);
		}
		colortable[SP_COUNT] = AlphaColorMap::getColorTable(SP_DEFAULT, CL_3D, -1, true);
		alphaTable[SP_COUNT] = HoverPoints::getAlphaGraph(CL_3D, SP_DEFAULT, -1, true);

		for (int index = 0; index < getPresetCount(); index++)
		{
			Cuscolortable.insert(index, AlphaColorMap::getColorTable(SP_DEFAULT, CL_3D, index));
			Cusalphatable.insert(index, HoverPoints::getAlphaGraph(CL_3D, SP_DEFAULT, index));
		}
		CusCount = m_presetNameList.size();
	}
	else
	{
		if (!(cusType >= 0 || delCus))
		{
			colortable[type] = AlphaColorMap::getColorTable(type, CL_3D);
			alphaTable[type] = HoverPoints::getAlphaGraph(CL_3D, type);
		}

		else
		{
			if (Cuscolortable.size() > cusType)
			{
				Cuscolortable.takeAt(cusType).clear();
				Cusalphatable.takeAt(cusType).clear();
			}

			if (!delCus)
			{
				if (Cuscolortable.size() <= cusType)
				{
					Cuscolortable.append(AlphaColorMap::getColorTable(SP_DEFAULT, CL_3D, cusType));
					Cusalphatable.append(HoverPoints::getAlphaGraph(CL_3D, SP_DEFAULT, cusType));
				}
				else
				{
					Cuscolortable.insert(cusType, AlphaColorMap::getColorTable(SP_DEFAULT, CL_3D, cusType));
					Cusalphatable.insert(cusType, HoverPoints::getAlphaGraph(CL_3D, SP_DEFAULT, cusType));
				}
			}
			CusCount = m_presetNameList.size();
		}
	}

	if (m_nTexTF != 0)
		m_pRenderer->deleteTexture(m_nTexTF);

	mip::COLOR* pTextureBuffer = new mip::COLOR[512 * 512];
	{
		int yr = 0;

		int cusPreset = -1;
		for (int y = 0; y < 512; y++, yr++)
		{
			SLICE_PRESET xPreset = SP_DEFAULT;
			bool res = false;
			if ((y / 3) >= SP_COUNT)
			{
				xPreset = SP_COUNT;
				res = Cuscolortable.size() != 0;
				//todo custom preset
				if (res)
				{
					cusPreset = ((y / 3) - SP_COUNT);

					if (cusPreset >= CusCount)
						cusPreset = CusCount - 1;
				}
			}
			else
				xPreset = (SLICE_PRESET)(y / 3);

			for (int x = 0; x < 512; x++)
			{
				if (!res)
					pTextureBuffer[x + (y * 512)] = getVolumeColor(alphaTable[xPreset], colortable[xPreset], x / 2);
				else
					pTextureBuffer[x + (y * 512)] = getVolumeColor(Cusalphatable.at(cusPreset), Cuscolortable.at(cusPreset), x / 2);
			}

		}
	}

	m_nTexTF = m_pRenderer->createTextureGL(512, 512, mip::TEXT_USER2, pTextureBuffer);

	SAFE_DELETES(pTextureBuffer);
	doneCurrent();
	return true;
}

bool WindowManager::update2DPresetTexture(SLICE_PRESET type, int cusType, bool delCus)
{
	makeCurrent();

	static QVector<QColor> colortable[SP_COUNT + 1];
	static QVector<int> alphaTable[SP_COUNT + 1];
	static bool init = false;
	static QVector<QVector<QColor>> Cuscolortable;
	static QVector<QVector<int>> Cusalphatable;
	static int CusCount = m_presetNameList.size();

	if (!init)
	{
		init = true;
		for (int index = 0; index < SP_COUNT; index++)
		{
			colortable[index] = AlphaColorMap::getColorTable((SLICE_PRESET)index, CL_2D);
			alphaTable[index] = HoverPoints::getAlphaGraph(CL_2D, (SLICE_PRESET)index);
		}
		colortable[SP_COUNT] = AlphaColorMap::getColorTable(SP_DEFAULT, CL_2D, -1, true);
		alphaTable[SP_COUNT] = HoverPoints::getAlphaGraph(CL_2D, SP_DEFAULT, -1, true);

		for (int index = 0; index < getPresetCount(); index++)
		{
			Cuscolortable.insert(index, AlphaColorMap::getColorTable(SP_DEFAULT, CL_2D, index));
			Cusalphatable.insert(index, HoverPoints::getAlphaGraph(CL_2D, SP_DEFAULT, index));
		}
		CusCount = m_presetNameList.size();
	}
	else
	{
		if (!(cusType >= 0 || delCus))
		{
			colortable[type] = AlphaColorMap::getColorTable(type, CL_2D);
			alphaTable[type] = HoverPoints::getAlphaGraph(CL_2D, type);
		}

		else
		{
			if (Cuscolortable.size() > cusType)
			{
				Cuscolortable.takeAt(cusType).clear();
				Cusalphatable.takeAt(cusType).clear();
			}

			if (!delCus)
			{
				if (Cuscolortable.size() <= cusType)
				{
					Cuscolortable.append(AlphaColorMap::getColorTable(SP_DEFAULT, CL_2D, cusType));
					Cusalphatable.append(HoverPoints::getAlphaGraph(CL_2D, SP_DEFAULT, cusType));
				}
				else
				{
					Cuscolortable.insert(cusType, AlphaColorMap::getColorTable(SP_DEFAULT, CL_2D, cusType));
					Cusalphatable.insert(cusType, HoverPoints::getAlphaGraph(CL_2D, SP_DEFAULT, cusType));
				}
			}
			CusCount = m_presetNameList.size();
		}
	}

	if (m_nTex2DTF != 0)
		m_pRenderer->deleteTexture(m_nTex2DTF);

	mip::COLOR* pTextureBuffer = new mip::COLOR[512 * 512];
	{
		int yr = 0;

		int cusPreset = -1;
		for (int y = 0; y < 512; y++, yr++)
		{
			SLICE_PRESET xPreset = SP_DEFAULT;
			bool res = false;
			if ((y / 3) >= SP_COUNT)
			{
				xPreset = SP_COUNT;
				res = Cuscolortable.size() != 0;
				//todo custom preset
				if (res)
				{
					cusPreset = ((y / 3) - SP_COUNT);

					if (cusPreset >= CusCount)
						cusPreset = CusCount - 1;
				}
			}
			else
				xPreset = (SLICE_PRESET)(y / 3);

			for (int x = 0; x < 512; x++)
			{
				if (!res)
					pTextureBuffer[x + (y * 512)] = getVolumeColor(alphaTable[xPreset], colortable[xPreset], x / 2);
				else
					pTextureBuffer[x + (y * 512)] = getVolumeColor(Cusalphatable.at(cusPreset), Cuscolortable.at(cusPreset), x / 2);
			}

		}
	}

	m_nTex2DTF = m_pRenderer->createTextureGL(512, 512, mip::TEXT_USER2, pTextureBuffer);

	SAFE_DELETES(pTextureBuffer);
	doneCurrent();
	return true;
}

mip::COLOR WindowManager::getVolumeColor(QVector<int> alphatable, QVector<QColor> colortable, int index, int preset)
{
	QColor changeColor = colortable.at(index);
	//	printf("%d/%d/%d/%d\n", changeColor.red(), changeColor.green(), changeColor.blue(), alphatable.at(index));
	//	return mip::COLOR(changeColor.red(), changeColor.green(), changeColor.blue(), changeColor.alpha());
	return mip::COLOR(changeColor.blue(), changeColor.green(), changeColor.red(), alphatable.at(index));
}

bool WindowManager::hideSurface(muint32 index)
{
	//	deleteMesh(index);

	renderLater_GridView(true);

	return true;
}

//bool WindowManager::saveFiles(QString & filename, muint32 uid, mask _m, int _mI, EXPORT_FILES _fType, bool appendType)
//{
//#ifdef ONLY_FOR_FTP_UPLOAD_VER
//
//#else 
//	if (!(IsLicensePass()))
//	{
//		m_pMessageBox->warning(nullptr, "License", "The program must be licensed to activate.");
//		return false;
//	}
//
//#endif
//
//	static QString exts[] = { QString("stl"), QString("obj"), QString("vtk"), QString("raw"), QString("raw"),
//								QString("nii"), QString("nii"),QString("nii") ,QString("txt") };
//	static EXPORT_FILES fType = EX_FILES_NONE;
//
//	if (_fType != EX_FILES_NONE)
//		fType = _fType;
//
//	if (fType == EX_FILES_NONE)
//		return false;
//
//
//	if (appendType)
//		filename.append(exts[fType]);
//
//	bool chk = CheckDupFile(filename, !appendType);
//
//	if (chk)
//	{
//		QFile file(filename);
//
//		file.remove();
//	}
//
//	switch (fType)
//	{
//	case EX_FILES_STL:
//		m_pActionManager->action_FileWork_Export_STLFile(uid, _m, getSmoothLevel(), filename, _mI);
//		break;
//	case EX_FILES_OBJ:
//		m_pActionManager->action_FileWork_Export_OBJFile(uid, _m, getSmoothLevel(), filename, _mI);
//		break;
//	case EX_FILES_RAW:
//		m_pActionManager->action_FileWork_Export_RawFile(filename, _m, _mI);
//		break;
//	case EX_FILES_HU_RAW:
//		m_pActionManager->action_FileWork_Export_HURawFile(filename, _m, _mI);
//		break;
//	case EX_FILES_NII:
//		m_pActionManager->action_FileWork_Export_ROINIIFile(_m, filename, _mI);
//		break;
//	case EX_FILES_HUNII:
//		m_pActionManager->action_FileWork_Export_HUNIIFile(_m, filename, _mI);
//		break;
//	case EX_FILES_COORDINATE_NII:
//		m_pActionManager->action_FileWork_Export_COORDINATENIIFile(_m, filename, _mI);
//		break;
//	case EX_FILES_VTK:
//		m_pActionManager->action_FileWork_Export_VTKFile(uid, _m, getSmoothLevel(), filename, _mI);
//		break;
//	case EX_FILES_TXT:
//		m_pActionManager->action_FileWork_Export_TXTFile(uid, _m, filename, _mI);
//		break;
//	default:
//		return false;
//		break;
//	}
//
//	return true;
//}

bool WindowManager::saveFiles(QString& filename, muint32 uid, mask _m, int _mI, EXPORT_FILES _fType, bool bPatientCoordinate, bool appendType, bool chkDupFile)
{
#ifdef ONLY_FOR_FTP_UPLOAD_VER

#else 
	if (!(IsLicensePass()))
	{
		m_pMessageBox->warning(nullptr, "License", "The program must be licensed to activate.");
		return false;
	}

#endif

	static QString exts[] = { QString("stl"), QString("obj"), QString("vtk"), QString("3mf"), QString("raw"), QString("raw"),
	QString("nii"), QString("nii"),QString("nii") ,QString("txt") };

	static EXPORT_FILES exportFileType = EX_FILES_NONE;

	if (_fType != EX_FILES_NONE)
	{
		exportFileType = _fType;
	}

	if (exportFileType == EX_FILES_NONE)
	{
		return false;
	}

	if (appendType)
	{
		filename.append(exts[exportFileType]);
	}

	bool chk;
	if (chkDupFile)
	{
		chk = CheckDupFile(filename, !appendType);
	}
	else
	{
		chk = CheckDupFile(filename, !chkDupFile);
	}

	if (chk)
	{
		QFile file(filename);

		file.remove();
	}

	switch (exportFileType)
	{
	case EX_FILES_STL:
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileExport))
			return false;

		m_pActionManager->action_FileWork_Export_STLFile(m_pDataContext, uid, _m, filename, bPatientCoordinate, _mI);
		break;
	case EX_FILES_OBJ:
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileExport))
			return false;

		m_pActionManager->action_FileWork_Export_OBJFile(m_pDataContext, uid, _m, filename, bPatientCoordinate, _mI);
		break;
	case EX_FILES_RAW:
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_MaskExport))
			return false;

		m_pActionManager->action_FileWork_Export_RawFile(filename, _m, _mI);
		break;
	case EX_FILES_HU_RAW:
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_MaskExport))
			return false;

		m_pActionManager->action_FileWork_Export_HURawFile(filename, _m, _mI);
		break;
	case EX_FILES_NII:
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_MaskExport))
			return false;

		m_pActionManager->action_FileWork_Export_ROINIIFile(_m, filename, _mI);
		break;
	case EX_FILES_HUNII:
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Export_NII_wholeHU))
			return false;

		m_pActionManager->action_FileWork_Export_HUNIIFile(_m, filename, _mI);
		break;
	case EX_FILES_COORDINATE_NII:
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_MaskExport))
			return false;

		m_pActionManager->action_FileWork_Export_COORDINATENIIFile(_m, filename, _mI);
		break;
	case EX_FILES_VTK:
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileExport))
			return false;

		m_pActionManager->action_FileWork_Export_VTKFile(m_pDataContext, uid, _m, filename, bPatientCoordinate, _mI);
		break;
	case EX_FILES_TXT:
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_LayerOperation_MaskExport))
			return false;

		m_pActionManager->action_FileWork_Export_TXTFile(uid, _m, filename, _mI);
		break;
	case EX_FILES_3MF:
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileExport))
			return false;

		m_pActionManager->action_FileWork_Export_STLFile(m_pDataContext, uid, _m, filename, _mI);
		break;
	default:
		return false;
		break;
	}

	return true;
}

bool WindowManager::SaveMeshFiles(QString& filename, muint8 UID, mint32 lUID, EXPORT_FILES _fType, bool bPatientCoordinate, bool appendType, bool bEnableProgress, QString qstrProgressTitle)
{
	FileWriterMesh fileWriter(this, m_pDataContext, m_pActionManager);
	return fileWriter.SaveMeshFiles(filename, UID, _fType, bPatientCoordinate, appendType, bEnableProgress, qstrProgressTitle);
}

bool WindowManager::saveMeshFilesVisualPrint(QString& filename, muint8 UID, mint32 lUID, EXPORT_FILES _fType, bool appendType, bool bEnableProgress, QString qstrProgressTitle)
{
	FileWriterMesh fileWriter(this, m_pDataContext, m_pActionManager);
	return fileWriter.saveMeshFilesVisualPrint(filename, UID, _fType, appendType, bEnableProgress, qstrProgressTitle);
}

bool WindowManager::CheckDupFile(QString& filename, bool onlyCheck/*=false*/)
{
	QFile file(filename);

	bool res = file.exists();

	if (onlyCheck)
		return res;

	if (res)
	{
		QString ext = filename.section('.', -1);

		filename.replace(QString(".%1").arg(ext), QString(""));

		for (int i = 1; i < 120; i++)
		{
			QString tmpFile;
			tmpFile = filename;
			tmpFile.append(QString(" (%1).%2").arg(i).arg(ext));

			file.setFileName(tmpFile);

			if (!file.exists())
			{
				res = false;
				filename = tmpFile;
				break;
			}
		}

	}

	return res;
}


bool WindowManager::previewSurface(mint32 layerUID, mask _m, int mI)
{
	/*static bool s_bEnableProgress = true;
	if (s_bEnableProgress)
	{
		m_pActionManager->action_FileWork_Preview_Surface(layerUID, _m, getSmoothLevel(), mI, s_bEnableProgress, "Preview Surface");
		s_bEnableProgress = false;
	}
	else
	{
		m_pActionManager->action_FileWork_Preview_Surface(layerUID, _m, getSmoothLevel(), mI, s_bEnableProgress);
	}*/

	m_pActionManager->action_FileWork_Preview_Surface(m_pDataContext, layerUID, _m, mI);
	return true;
}

bool WindowManager::previewSurface(mint32 layerUID, mask _m, int mI, bool bEnableProgress)
{
	//	static bool s_bEnableProgress = true;
	if (bEnableProgress)
	{
		m_pActionManager->action_FileWork_Preview_Surface(m_pDataContext, layerUID, _m, mI, bEnableProgress, "Preview Surface");
		//		bEnableProgress = false;
	}
	else
	{
		m_pActionManager->action_FileWork_Preview_Surface(m_pDataContext, layerUID, _m, mI, bEnableProgress);
	}
	return true;
}

bool WindowManager::previewSurfaceVisualPrint(mint32 layerUID, mask _m, int mI, bool bEnableProgress)
{
	//	static bool s_bEnableProgress = true;
	if (bEnableProgress)
	{
		m_pActionManager->action_FileWork_Preview_SurfaceVisualPrint(layerUID, _m, mI, bEnableProgress, "Preview Surface");
		//		bEnableProgress = false;
	}
	else
	{
		m_pActionManager->action_FileWork_Preview_SurfaceVisualPrint(layerUID, _m, mI, bEnableProgress);
	}
	return true;
}

bool WindowManager::previewSurface(mint32 layerUID, mask _m, int mI, bool bEnableProgress, QWidget* pParentWidget)
{
	//	static bool s_bEnableProgress = true;
	if (bEnableProgress)
	{
		m_pActionManager->action_FileWork_Preview_Surface(m_pDataContext, layerUID, _m, mI, bEnableProgress, "Preview Surface", pParentWidget);
		//		bEnableProgress = false;
	}
	else
	{
		m_pActionManager->action_FileWork_Preview_Surface(m_pDataContext, layerUID, _m, mI, bEnableProgress, "Preview Surface", pParentWidget);
		//		m_pActionManager->action_FileWork_Preview_Surface(layerUID, _m, getSmoothLevel(), mI, bEnableProgress);
	}
	return true;
}

bool WindowManager::saveSTL(QString& filename, muint32 index, mask _m, int _mI)
{

	m_pActionManager->action_FileWork_Export_STLFile(m_pDataContext, index, _m, filename, _mI);
	return true;
}

bool WindowManager::saveOBJ(QString& filename, muint32 index, mask _m, int _mI)
{

	m_pActionManager->action_FileWork_Export_OBJFile(m_pDataContext, index, _m, filename, _mI);
	return true;
}

void WindowManager::setShaderQuality(SHADER_QUALITY value)
{
	int val = m_shader_quality = value;

	setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_shader_quality, QString::number(val - 1));
}

bool WindowManager::saveOrginVolumeData()
{
	QFile file(STRING_MANAGER->cacheFileOriginVolume);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write((char*)m_pDataContext->volume_data.getHUDataPoint(), m_pDataContext->volume_data.getVolumeDataLength() * sizeof(mint16));
		file.close();
	}
	return true;
}

bool WindowManager::loadPRD(const QString& mPath)
{
	return m_pFileLoader->loadPRD(mPath);
}

bool WindowManager::saveJSON(const QString& mPath)
{
	int width = m_pDataContext->volume_data.getCX();
	int height = m_pDataContext->volume_data.getCY();
	int slice = m_pDataContext->volume_data.getCZ();

	int HU_size = 0, HU_min = 10000, HU_max = -10000;
	mint16 Hu_data = 0;
	for (int z = 0; z < slice; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				Hu_data = m_pDataContext->volume_data.getData(z * height * width + y * width + x);
				if (Hu_data > HU_max) HU_max = Hu_data;

				if (Hu_data < HU_min) HU_min = Hu_data;
			}
		}
	}
	HU_size = abs(HU_min) + abs(HU_max);

	QFile wFile(mPath);
	WORD wd = 0xFEFF;   //unicode 식별자
	QString str;

	if (width * height * slice < 512 * 512 * 512)
	{
		str = QString("{\n\t\"width\":\"%1\",\n\t\"height\":\"%2\",\n\t\"slice\":\"%3\",\n\t\"HU_size\":\"%4\",\n\t\"HU_min\":\"%5\",\n\t\"HU_max\":\"%6\",\n\t\"Mask_Size\":\"%7\"\n}").arg(width).arg(height).arg(slice).arg(HU_size).arg(HU_min).arg(HU_max).arg(m_pDataContext->volume_data.getMaskInfoListCnt());
	}
	else
	{
		str = QString("{\n\t\"width\":\"%1\",\n\t\"height\":\"%2\",\n\t\"slice\":\"%3\",\n\t\"HU_size\":\"%4\",\n\t\"HU_min\":\"%5\",\n\t\"HU_max\":\"%6\",\n\t\"Mask_Size\":\"%7\"\n}").arg(width / 2).arg(height / 2).arg(slice / 2).arg(HU_size / 2).arg(HU_min / 2).arg(HU_max / 2).arg(m_pDataContext->volume_data.getMaskInfoListCnt());
	}
	if (wFile.open(QIODevice::WriteOnly))
	{
		wFile.write((const char*)&wd, 2);   //unicode 식별자 파일 처음에 쓰자
		wFile.write((const char*)str.toStdWString().c_str(), sizeof(WCHAR) * str.size());
		wFile.close();
	}
	else
	{
		return false;
	}

	return true;
}

bool WindowManager::saveROINII(const QString& fileName, mask _m, int _mI)
{
	m_pActionManager->action_FileWork_Export_ROINIIFile(_m, fileName, _mI);

	return true;
}

bool WindowManager::saveHUNII(const QString& fileName, mask _m, int _mI)
{
	m_pActionManager->action_FileWork_Export_HUNIIFile(_m, fileName, _mI);

	return true;
}

bool WindowManager::saveTXT(const QString& fileName, muint32 index, mask _m /*= 0*/, int _mI /*= 0*/)
{
	m_pActionManager->action_FileWork_Export_TXTFile(index, _m, fileName, _mI);

	return true;
}

bool WindowManager::saveNII(const QString& mPath, bool _b_use_thread, bool patch, BoundingBoxI box)
{
	m_pActionManager->action_FileWork_Export_NIIFile(mPath, _b_use_thread, patch, box);

	return true;
}

bool WindowManager::saveNRRD(const QString& mPath, bool b_use_thread, bool patch, BoundingBoxI box)
{
	m_pActionManager->action_FileWork_Export_NRRDFile(mPath, b_use_thread, patch, box);

	return true;
}

bool WindowManager::IsLicensePass()
{
	return m_isLicensePass;
}

void WindowManager::SetLicensePass(bool value)
{
	m_isLicensePass = value;
}

muint32 WindowManager::createTextureFromQImage(QImage* image)
{
	int xLenth = image->width();
	int yLenth = image->height();

	mip::COLOR* pTextureBuffer = new mip::COLOR[yLenth * xLenth];
	mip::COLOR* pRowTexture = nullptr;
	{
		int yr = 0;
		for (int y = 0; y < yLenth; y++, yr++)
		{
			// y coord inversed ( lefthand coord : rot x axis -90 )
			pRowTexture = reinterpret_cast<mip::COLOR*>(pTextureBuffer + (xLenth * yr));
			QRgb* qrgb = (QRgb*)image->scanLine(y);
			for (int x = 0; x < xLenth; x++)
			{
				pRowTexture[x] = mip::COLOR(qBlue(qrgb[x]), qGray(qrgb[x]), qRed(qrgb[x]), qAlpha(qrgb[x]));
			}
		}
	}

	muint32 imageTexture = m_pRenderer->createTextureGL(xLenth, yLenth, mip::TEXT_USER, pTextureBuffer);

	SAFE_DELETES(pTextureBuffer);

	return imageTexture;
}

bool WindowManager::setFontTexture(QImage* image)
{
	if (m_nTexFont != 0)
	{
		return false;
	}

	m_nTexFont = createTextureFromQImage(image);

	return m_nTexFont != 0 ? true : false;
}

bool WindowManager::viveOn()
{
#ifndef _M_IX86 // VR
	if (m_pRenderer->isAvailableVolumeRender() == false)
	{
		return false;
	}

	if (m_pDataContext->volume_data.isValidate())
	{
		VolumeView* volumeView = static_cast<VolumeView*>(mainSegmentWidget->getViewVolume());
		bool state = volumeView->getUpdateFrameState();

		makeCurrent();
		QSize size = volumeView->size();
		if (m_pRenderer->initHMD(m_pRenderer->getDefaultFBOID(), size.width(), size.height()) == true)
		{
			mainSegmentWidget->setVRUpdate(!state);
			if (state == false)
			{
				mip::VECTOR3 pos = mip::VECTOR3(0, 0, 0);
				mip::MATRIX44 matWorld = m_pRenderer->getWorld() * m_pRenderer->getView();
				matWorld.inverse();
				pos.transform(matWorld);
				matMoveCameraPos.set(pos);
				mainSegmentWidget->getViewVolume()->resetUI();
			}
		}
		doneCurrent();
	}
	else
	{
		m_pMessageBox->warning(nullptr, STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD)
			, STRING_MANAGER->getString(STR_LOAD_FIRST));

		return false;
	}
#endif
	return true;
}

void WindowManager::makeCurrent()
{
	m_pRenderer->makeCurrent();
}

void WindowManager::doneCurrent()
{
	m_pRenderer->doneCurrent();
}

void WindowManager::getOpenGLError(std::string str)
{
	m_pRenderer->getOpenGLError(str);
}

void WindowManager::MEViewRenderLater()
{
	if (mainMeshWidget)
	{
		mainMeshWidget->getMainView()->setUpdateFrame(false);
	}
}

void WindowManager::setDCutShape(DRAW_CUT_SHAPE value)
{
	m_DCutShape = value;
}

DRAW_CUT_SHAPE WindowManager::getDCutShape(bool Tabtype)
{
	bool res = false;
	if (Tabtype) //measurement
		res = m_analworkMode == ANAL_WORK_SKETCHDRAWSEGMENTATION;
	else
		res = m_workMode == WORK_SKETCHDRAWSEGMENTATION;

	if (res)
		return m_DCutShape;
	else
		return DRAW_CUT_SHAPE_NONE;
}

bool WindowManager::preRenderProcessMPR()
{
	bool bRenderCheck = getRenderable();

	if (bRenderCheck == true)
	{
		setRenderable(false);
	}

	makeCurrent();

	m_pRenderer->preRenderProcess();

	if (m_pDataContext->volume_data.isValidate() == true)
	{
		if (m_pDataContext->volume_data.getUpdateMPR())
		{
			m_pDataContext->volume_data.clearUpdateMPR();

			buildup2DMPR(nullptr);
		}

		if (m_pDataContext->volume_data.getUpdateMaskVolume())
		{
			m_pDataContext->volume_data.clearUpdateMaskVolume();

			buildup3DMaskTexture(nullptr);
		}
	}

	doneCurrent();

	if (bRenderCheck == true)
	{
		setRenderable(true);
	}

	return true;
}

bool WindowManager::preRenderProcess()
{
	bool bRenderCheck = getRenderable();

	if (bRenderCheck == true)
		setRenderable(false);

	makeCurrent();

	m_pRenderer->preRenderProcess();

	if (m_pDataContext->volume_data.isValidate() == true)
	{
		bool res = true;
		if (m_pDataContext->volume_data.getUpdateVolume())
		{
			m_pDataContext->volume_data.clearUpdateVolume();

			//m_pActionManager->action_ProgressBegin(STRING_MANAGER->getString(STR_SHOW_UPDATE_MASK));
			//m_pActionManager->action_ProgressUpdate(0);
			res = buildup3DVolumeTexture(nullptr);
			//m_pActionManager->action_ProgressEnd();

		}

		if (m_pDataContext->volume_data.getUpdateMaskVolume())
		{
			m_pDataContext->volume_data.clearUpdateMaskVolume();

			//m_pActionManager->action_ProgressBegin(STRING_MANAGER->getString(STR_SHOW_UPDATE_MASK));
			//m_pActionManager->action_ProgressUpdate(0);
			if (res)
			{
				res = buildup3DMaskTexture(nullptr);
				//m_pActionManager->action_ProgressEnd();
			}
		}

		if (m_tabWindow)
		{
			Visualize3DTab* tab = m_tabWindow->get3DTab();

			if (tab)
				tab->setDisableShader(!res);
		}


		if (m_pDataContext->volume_data.getUpdateMPR())
		{
			m_pDataContext->volume_data.clearUpdateMPR();

			buildup2DMPR(nullptr);
		}
	}
	doneCurrent();

	if (bRenderCheck == true)
		setRenderable(true);

	return true;
}

bool WindowManager::ChangeUID(mint32 pre_mask_uid, mint32 mask_uid)
{
	{
		if (m_tabWindow)
		{
			AnalysisTab* tab = m_tabWindow->getAnalysisTab();

			if (tab)
			{
				bool preState = m_pDataContext->volume_data.getTAState(pre_mask_uid);
				m_pDataContext->volume_data.setTAState(mask_uid, preState);
				tab->ChangeUID(pre_mask_uid, mask_uid);
			}
		}
	}
#ifdef SEGMENTATION_COLOR_CODING
	for (auto it = m_pDataContext->volume_data.m_vecAIResultData.begin(); it != m_pDataContext->volume_data.m_vecAIResultData.end(); ++it)
	{
		if (it->first == pre_mask_uid)
		{
			it->first = mask_uid;
			break;
		}
	}

	for (auto it = m_pDataContext->volume_data.m_vecAIOutset.begin(); it != m_pDataContext->volume_data.m_vecAIOutset.end(); ++it)
	{
		if (it->first == pre_mask_uid)
		{
			it->first = mask_uid;
			break;
		}
	}


#endif

#ifdef MULTI_DRAWCUT_MODE
	{
		DrawcutTab* tab = tabList->getDrawTab();

		if (tab)
			tab->ChangeSeed(pre_mask_uid, mask_uid);
	}
#endif

	return true;
		}

bool WindowManager::ChangeUID_Ext(VOLUME_DATA* pVolumeData, mint32 pre_mask_uid, mint32 mask_uid)
{
	{
		if (m_tabWindow)
		{
			AnalysisTab* tab = m_tabWindow->getAnalysisTab();

			if (tab)
			{
				bool preState = pVolumeData->getTAState(pre_mask_uid);
				pVolumeData->setTAState(mask_uid, preState);
				tab->ChangeUID(pre_mask_uid, mask_uid);
			}
		}
	}
#ifdef SEGMENTATION_COLOR_CODING
	for (auto it = pVolumeData->m_vecAIResultData.begin(); it != pVolumeData->m_vecAIResultData.end(); ++it)
	{
		if (it->first == pre_mask_uid)
		{
			it->first = mask_uid;
			break;
		}
	}

	for (auto it = pVolumeData->m_vecAIOutset.begin(); it != pVolumeData->m_vecAIOutset.end(); ++it)
	{
		if (it->first == pre_mask_uid)
		{
			it->first = mask_uid;
			break;
		}
	}


#endif

#ifdef MULTI_DRAWCUT_MODE
	{
		if (tabList)
		{
			DrawcutTab* tab = tabList->getDrawTab();

			if (tab)
				tab->ChangeSeed(pre_mask_uid, mask_uid);
		}
		}
#endif
	//pVolumeData->ChangeLUID(pre_mask_uid, mask_uid);

	return true;
	}


void WindowManager::setTempReportData(const QString& fileName, const std::vector<std::pair<QImage, QString>>& image_list, const QString& html)
{
	tempReportData.clear();
	tempReportData.setFileName(fileName);
	tempReportData.setImageList(image_list);
	tempReportData.setHTML(html);
}

void WindowManager::setDrawingSeed(bool isLeft, mask _m, int _mI)
{
	int chkLeft = isLeft ? 0 : 1;
	m_drawingSeed[chkLeft].uid = _mI;
	m_drawingSeed[chkLeft]._m = _m;
}


mask WindowManager::getDrawingSeedMask(bool isLeft)
{
	int chkLeft = isLeft ? 0 : 1;
	return m_drawingSeed[chkLeft]._m;
}

int WindowManager::getDrawingSeedIndex(bool isLeft)
{
	int chkLeft = isLeft ? 0 : 1;
	return m_drawingSeed[chkLeft].uid;
}

void WindowManager::setRenderType(int type)
{
	qDebug() << __FUNCTION__ << " Type:" << type << endl;
	m_renderType = type;
}

//bool WindowManager::VisualPrintingSTLUpload(QString & filename)
//{
//	m_pActionManager->action_VisualPrintFTPUpload(filename);
//	return true;
//}

void WindowManager::VisualPrintingWebLink(DataContext* pDataContext)
{
	m_pActionManager->action_VisualPrintWebLink(pDataContext);

#if 0
	int nUsableCount = network::chkFunctionUsableCount(LICENSE_DATA->getUrl(eLSPTDecreaseFunctionUsableCount), MFL_Common_VisualPrinting_VisualPrintingservice);
#else
	int nUsableCount = LICENSE_DATA->chkFunctionUsableCount((eMEDIP_FUNCTION_LEVEL)MFL_Common_VisualPrinting_VisualPrintingservice, eLSPTDecreaseFunctionUsableCount);
#endif

	if (nUsableCount >= 0)
	{
		m_tabWindow->getVisualPrintTab()->SetVisualPrintUsableCntLabel(nUsableCount);
	}
	else
	{
		m_pMessageBox->warning(nullptr, "VisualPrint Count Check Error", nullptr);
	}
}

#ifdef _M_IX86
void WindowManager::renderVolumeVTKData()
{
	VolumeView* vView = dynamic_cast<VolumeView*>(mainSegmentWidget->getViewVolume());
	if (vView == nullptr || volume_data.isValidate() == false)
		return;

	vView->clearVTK();

	vView->showVTKWidget(true);
	vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkShortArray> scalars = vtkSmartPointer<vtkShortArray>::New();
	//imageData->ReleaseData();

	int height = volume_data.getCY();
	int width = volume_data.getCX();
	int slice = volume_data.getCZ();

	imageData->SetDimensions(width, height, slice);
	imageData->SetSpacing(volume_data.getSpaceX(), volume_data.getSpaceY(), volume_data.getSpaceZ());

	qDebug() << scalars << endl;

	for (int z = 0; z < slice; z++)
	{
		int z1 = slice - z - 1;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				/*unsigned char* maskData = volume_data.getMaskDataPoint(0);
				if (maskData[z * height * width + y * width + x] > 0 && maskData[z * height * width + y * width + x] <= 5)
					scalars->InsertTuple1(z1 * height * width + y * width + x, volume_data.getData(z * height * width + y * width + x));
				else
					scalars->InsertTuple1(z1 * height * width + y * width + x, -3024);*/

				scalars->InsertTuple1(z1 * height * width + y * width + x, volume_data.getData(z * height * width + y * width + x));
	}
}
}

	vView->renderVolumeVTKData(imageData, scalars);
	}
#endif

void		WindowManager::VisualPrintFromMeshList()
{
	m_pActionManager->action_VisualPrintWork_Mesh_Export();
}

void		WindowManager::setVisualPrintParams(int _val_smooth, int _val_reduce)
{
	int n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();
	if (n_mesh < 1)
	{
		return;
	}

	auto p_mesh_list = m_tabWindow->getVisualPrintMeshTab()->GetVisualPrintMeshListWidget();

	if (p_mesh_list)
	{
		p_mesh_list->setSmoothWidgets(_val_smooth, _val_reduce);
	}

	for (int idx = 0; idx < n_mesh; ++idx)
	{
		auto p_mesh_info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(idx);

		if (p_mesh_info)
		{
			p_mesh_info->m_nReduceLevel = _val_reduce;
			p_mesh_info->m_nSmoothLevel = _val_smooth;
			p_mesh_info->m_nRemeshLevel = 0;
		}
	}
}

// 201019 허 건 대리
void		WindowManager::setVisualPrintSmoothVal(int _val_smooth)
{
	int n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();
	if (n_mesh < 1)
	{
		return;
	}

	auto p_mesh_list = m_tabWindow->getVisualPrintMeshTab()->GetVisualPrintMeshListWidget();

	if (p_mesh_list)
	{
		p_mesh_list->setSmoothWidgetsSmoothVal(_val_smooth);
	}

	for (int idx = 0; idx < n_mesh; ++idx)
	{
		auto p_mesh_info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(idx);

		if (p_mesh_info)
		{
			p_mesh_info->m_nSmoothLevel = _val_smooth;
		}
	}
}

// 201019 허 건 대리
void		WindowManager::setVisualPrintReduceVal(int _val_reduce)
{
	int n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();
	if (n_mesh < 1)
	{
		return;
	}

	auto p_mesh_list = m_tabWindow->getVisualPrintMeshTab()->GetVisualPrintMeshListWidget();

	if (p_mesh_list)
	{
		p_mesh_list->setSmoothWidgetsReduceVal(_val_reduce);
	}

	for (int idx = 0; idx < n_mesh; ++idx)
	{
		auto p_mesh_info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(idx);

		if (p_mesh_info)
		{
			p_mesh_info->m_nReduceLevel = _val_reduce;
		}
	}
}

//201021 허 건 대리
void		WindowManager::updateVisualPrintPckModel(int _index)
{
	if (pVisualPrintMeshWidget)
	{
		auto p_view = pVisualPrintMeshWidget->getMainView();

		if (p_view)
		{
			p_view->updatePckModelIndex(_index);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 201008 허건 대리
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////   리스트 위젯 비활성화 및 활성화 ////////////////////////////////////////////////////////////////////////
void		WindowManager::enableMeshList(bool _b_enable)
{
	MeshTab* p_tab = m_tabWindow->getMeshTab();

	if (p_tab)
	{
		p_tab->setEnabled(_b_enable);
	}
}

void		WindowManager::enableROIList(bool _b_enable)
{
	ROITab2* p_tab = m_tabWindow->getROITab();

	if (p_tab)
	{
		p_tab->setEnabled(_b_enable);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 200825 허건 대리
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////   Mesh View  Function//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowManager::updatePckModel(int _idx)
{
	if (!mainMeshWidget)
	{
		return;
	}

	//int n_vt_pck = vt_pckID.size();

	//if (n_vt_pck < 1)
	//{
	//	vt_pckID.resize(m_pDataContext->m_MeshData.GetMeshCount(), false);
	//}

	//if (n_vt_pck > _idx)
	//{
	//	for (int i = 0; i < n_vt_pck; ++i)
	//	{
	//		vt_pckID[i] = ((i == _idx) ? true : false);
	//	}

	//	m_pMeshModelViewManager->updatePivotPoint(_idx);

	//	if (_idx < 0)
	//	{
	//		MESH_BTN_SCENE_MANAGER->enableOpcityCtrl(false);
	//	}
	//	else
	//	{
	//		m_pMeshViewBtn3DScene->enableOpcityCtrl(true);
	//	}
	//}

	int nMesh = m_pDataContext->m_MeshData.GetMeshCount();

	if (nMesh < 1 || nMesh <= _idx)
	{
		return;
	}

	for (int i = 0; i < nMesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (pMeshInfo == nullptr)
		{
			continue;
		}

		pMeshInfo->selected = ((i == _idx) ? true : false);
	}

	m_pMeshModelViewManager->UpdatePivotPoint(_idx);

	if (_idx < 0)
	{
		m_pMeshViewBtn3DScene->enableOpcityCtrl(false);
	}
	else
	{
		m_pMeshViewBtn3DScene->enableOpcityCtrl(true);
	}

	renderLater_3DView();
}

void WindowManager::updateMeshTablist(int _idx, bool b_clear)
{
	qDebug() << "update mesh tab List. idx = " << _idx << ", clear" << b_clear;
	MeshTab* p_mesh_tab = m_tabWindow->getMeshTab();

	if (!p_mesh_tab)
	{
		return;
	}

	p_mesh_tab->Update(false, _idx, b_clear);
}

void WindowManager::updateMeshTablist()
{
	int count = 0;
	int nMesh = m_pDataContext->m_MeshData.GetMeshCount();
	//for (int i = 0; i < vt_pckID.size(); ++i)
	for (int i = 0; i < nMesh; ++i)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

		if (pMeshInfo && pMeshInfo->selected)
		{
			if (count == 0)
			{
				updateMeshTablist(i);
				count++;
			}
			else
			{
				updateMeshTablist(i, false);
			}
		}
	}
}

void WindowManager::UpdateAllMeshTabList_NotPicked()
{
	int count = m_pDataContext->m_MeshData.GetMeshCount();
	for (int i = 0; i < count; ++i)
	{
		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(i);
		if (info)
		{
			if (info->selected)
			{
				bool clear = false;
				updateMeshTablist(i, clear);
			}
		}
	}
}

void WindowManager::UpdateAllMeshTabList()
{
	int count = m_pDataContext->m_MeshData.GetMeshCount();
	for (int i = 0; i < count; ++i)
	{
		bool clear = false;
		updateMeshTablist(i, clear);
	}
}

void WindowManager::UpdateMeshTabByInfo(const MeshInfo& info)
{
	int layerIndex = 0;
	if (m_pDataContext->m_MeshData.TryGetIndexByName(&layerIndex, info.GetName().toStdString()))
	{
		MeshTab* meshTab = m_tabWindow->getMeshTab();
		if (meshTab)
		{
			MeshListWidget* meshList = meshTab->GetMeshList();
			meshList->UpdateItemByLayerIndex(layerIndex);
		}
	}
}

void WindowManager::clearMeshTabSelection()
{
	MeshTab* p_mesh_tab = m_tabWindow->getMeshTab();

	if (!p_mesh_tab)
	{
		return;
	}

	p_mesh_tab->ClearSelection();

	int nMesh = m_pDataContext->m_MeshData.GetMeshCount();
	//for (int i = 0; i < vt_pckID.size(); ++i)
	for (int i = 0; i < nMesh; ++i)
	{
		p_mesh_tab->Update(true, i);
	}
}

void WindowManager::clearMeshTabList()
{
	MeshTab* p_mesh_tab = m_tabWindow->getMeshTab();

	if (!p_mesh_tab)
	{
		return;
	}

	p_mesh_tab->Clear();
}

void WindowManager::setHeatMapMode(bool bCheck)
{
	if (m_tabWindow)
	{
		AISegTab* tab = m_tabWindow->getAITab();
		if (tab)
		{
			tab->SetHeatMapMode(bCheck);
		}
	}
}

bool WindowManager::isHeatMapMode()
{
	if (m_tabWindow)
	{
		AISegTab* tab = m_tabWindow->getAITab();
		if (tab)
		{
			return tab->IsHeatMapMode();
		}
	}

	return false;
}

bool WindowManager::isHeatMapEnabled()
{
	if (m_tabWindow)
	{
		AISegTab* tab = m_tabWindow->getAITab();
		if (tab)
		{
			return tab->IsHeatMapEnabled();
		}
	}

	return false;
}

TabWindow* WindowManager::GetTab() const
{
	return m_tabWindow;
}

void WindowManager::SetTab(TabWindow* pTabWindow)
{
	m_tabWindow = pTabWindow;
}

bool WindowManager::IsPredictComplete() const
{
	return m_isPredictComplete;
}

void WindowManager::SetPredictComplete(bool value)
{
	m_isPredictComplete = value;
}

void WindowManager::SetProductManager(ProductManager* pProductManager)
{
	m_pProductManager = pProductManager;
}

void WindowManager::SetMessageBox(std::unique_ptr<MessageBoxBase> pMessageBox)
{
	m_pMessageBox = std::move(pMessageBox);
	m_pFileLoader->SetMessageBox(m_pMessageBox.get());
}

DcmtkSeriesInfo* WindowManager::GetDicomInfo(DcmtkSeriesInfo* infoList)
{
	static DcmtkSeriesInfo info;

	if (nullptr != infoList)
	{
		initDicomInfo(&info, infoList);

		QString str = QString::fromLocal8Bit(info.modality_.c_str());
		if (str.contains(QRegularExpression("(?i)(CT|MCT|XRAY|CR|XA|PT)")))
		{
			m_modality = eModalityType::Modality_HU;
		}
		else
		{
			m_modality = eModalityType::Modality_SI;
		}
	}

	return &info;
}

DcmtkSeriesInfo* WindowManager::GetDicomInfo_PET(DcmtkSeriesInfo* infoList)
{
	static DcmtkSeriesInfo info;
	if (nullptr != infoList)
	{
		initDicomInfo(&info, infoList);
	}
	return &info;
}

void WindowManager::initDicomInfo(DcmtkSeriesInfo* target, DcmtkSeriesInfo* source)
{
	target->strSeriesUID = source->strSeriesUID;
	target->strStudyUID = source->strStudyUID;
	target->strAcquisitionNum = source->strAcquisitionNum;
	target->patientsName_ = source->patientsName_;
	target->patientId_ = source->patientId_;
	target->patientSize_ = source->patientSize_;
	target->patientWeight_ = source->patientWeight_;
	target->studyDate_ = source->studyDate_;
	target->studyTime_ = source->studyTime_;
	target->modality_ = source->modality_;
	target->numImages_ = source->numImages_;
	target->description_ = source->description_;
	target->birthday_ = source->birthday_;
	target->age_ = source->age_;
	target->kvp_ = source->kvp_;
	target->ma_ = source->ma_;
	target->sex_ = source->sex_;
	target->studyDescription = source->studyDescription;
	target->sliceThickness = source->sliceThickness;
	target->width = source->width;
	target->height = source->height;
	target->imagePosition = source->imagePosition;
	target->xySpacing = source->xySpacing;
	target->zSpacing = source->zSpacing;
	target->convolutionkernel = source->convolutionkernel;
	target->manufacturerModel = source->manufacturerModel;
	target->seriesDate = source->seriesDate;
	target->Manufacturer = source->Manufacturer;

	target->units_ = source->units_;
	target->radionuclideTotalDose_ = source->radionuclideTotalDose_;
	target->radionuclideHalfLife = source->radionuclideHalfLife;
	target->acquisitionTime_ = source->acquisitionTime_;
	target->radiopharmaceuticalStartTime_ = source->radiopharmaceuticalStartTime_;

	target->frameOfReferenceUID_ = source->frameOfReferenceUID_;
	target->accessionNumber = source->accessionNumber;
	target->examID = source->examID;

	target->echoTime = source->echoTime;
	target->repetitionTime = source->repetitionTime;
	target->magneticFieldStrength = source->magneticFieldStrength;
	target->flipAngle = source->flipAngle;
	target->sopClassUid = source->sopClassUid;
}

short WindowManager::Get16(int index)
{
	return m_pDataContext->volume_data.pData3D_HU_Flip[index];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void copyVolumeGeometryInfo(VolumeView* pFrom, AnalVolumeView* pTo)
{
	if (pFrom && pTo)
	{
		mip::QUATERNION volRotate = pFrom->getVolRotation();
		mip::QUATERNION imgRotate = pFrom->getImgRotation();
		mip::VECTOR3 volTrans = pFrom->getVolTranslation();
		mip::VECTOR3 imgTrans = pFrom->getImgTranslation();
		float zoom = pFrom->getZoom();

		pTo->setVolRotation(volRotate);
		pTo->setVolTranslation(volTrans);
		pTo->setImgRotation(imgRotate);
		pTo->setImgTranslation(imgTrans);
		pTo->setZoom(zoom);
		pTo->setClipPrefer(0, true);
		pTo->renderLater();
	}
}

void copyVolumeGeometryInfo(AnalVolumeView* pFrom, AnalVolumeView* pTo)
{
	if (pFrom && pTo)
	{
		mip::QUATERNION volRotate = pFrom->getVolRotation();
		mip::QUATERNION imgRotate = pFrom->getImgRotation();
		mip::VECTOR3 volTrans = pFrom->getVolTranslation();
		mip::VECTOR3 imgTrans = pFrom->getImgTranslation();
		float zoom = pFrom->getZoom();
		mint8 clipPrefer = pFrom->getClipPrefer();

		pTo->setVolRotation(volRotate);
		pTo->setVolTranslation(volTrans);
		pTo->setImgRotation(imgRotate);
		pTo->setImgTranslation(imgTrans);
		pTo->setZoom(zoom);
		pTo->setClipPrefer(clipPrefer);
		pTo->renderLater();
	}
}

void copyVolumeGeometryInfo(AnalVolumeView* pFrom, VolumeView* pTo)
{
	if (pFrom && pTo)
	{
		mip::QUATERNION volRotate = pFrom->getVolRotation();
		mip::QUATERNION imgRotate = pFrom->getImgRotation();
		mip::VECTOR3 volTrans = pFrom->getVolTranslation();
		mip::VECTOR3 imgTrans = pFrom->getImgTranslation();
		float zoom = pFrom->getZoom();
		mint8 clipPrefer = pFrom->getClipPrefer();

		pTo->setVolRotation(volRotate);
		pTo->setVolTranslation(volTrans);
		pTo->setImgRotation(imgRotate);
		pTo->setImgTranslation(imgTrans);
		pTo->setZoom(zoom);
		pTo->setClipPrefer();
		pTo->renderLater();
	}
}

/*
	TODO: MainTAWidget, MainSegmentWidget 사이 공통 모듈 가상 함수로 통합할 것
*/
void copy_Radiomics_To_Segmentation_MPRPlaneGeometryInfos(WindowManager* pWinManager, DataContext* pDataContext, MainTAWidget* pFrom, MainSegmentWidget* pTo)
{
	if (pFrom && pTo)
	{
		pTo->setWindowsZoomFactor(
			pDataContext->volume_data.getSpaceX(true),
			pDataContext->volume_data.getSpaceY(true),
			pDataContext->volume_data.getSpaceZ(true));

		for (int i = 0; i < 3; i++)
		{
			WindowBase* pFromWin = pFrom->getWindow((WINDOW_TYPE)(WT_AXIAL + i));
			WindowBase* pToWin = pTo->getWindow((WINDOW_TYPE)(WT_AXIAL + i));

			copyMPRPlaneGeometryInfo(pWinManager, pFromWin, pToWin);
		}
	}
}

void copy_Segmentation_To_Radiomics_MPRPlaneGeometryInfos(WindowManager* pWinManager, DataContext* pDataContext, MainSegmentWidget* pFrom, MainTAWidget* pTo)
{
	if (pFrom && pTo)
	{
		pTo->setWindowsZoomFactor(
			pDataContext->volume_data.getSpaceX(true),
			pDataContext->volume_data.getSpaceY(true),
			pDataContext->volume_data.getSpaceZ(true));

		for (int i = 0; i < 3; i++)
		{
			WindowBase* pFromWin = pFrom->getWindow((WINDOW_TYPE)(WT_AXIAL + i));
			WindowBase* pToWin = pTo->getWindow((WINDOW_TYPE)(WT_AXIAL + i));

			copyMPRPlaneGeometryInfo(pWinManager, pFromWin, pToWin);
		}
	}
}

void copyMPRPlaneGeometryInfo(WindowManager* pWinManager, WindowBase* pFrom, WindowBase* pTo)
{
	float fZoomStepNumX = 0;
	float fZoomStepNumY = 0;
	float fSlicePosX = 0.f;
	float fSlicePosY = 0.f;
	int nDepth = 0;
	if (pFrom && pTo)
	{
		pFrom->GetZoomStepNum(fZoomStepNumX, fZoomStepNumY);
		fSlicePosX = pFrom->getSlicePositionX();
		fSlicePosY = pFrom->getSlicePositionY();
		nDepth = pFrom->getDepth();

		pTo->SetZoomFactorByStepNum(fZoomStepNumX, fZoomStepNumY);
		pTo->setSlicePosition(fSlicePosX, fSlicePosY);
		pTo->setDepth(nDepth);

		if (pWinManager->getWheelZoom())
		{
			pTo->InitWheelSliderFunc(true);
		}
		else
		{
			pTo->InitWheelSliderFunc(false);
		}
	}
}

