#include "stdafx.h"
#include "VolumeView.h"
#include "windowManager.h"
#include "DicomReader.h"
#include "mipEngine/geometry.h"
#include "mipEngine/intersect.h"
#include "Renderer/Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/MeshTopology.h"
#include "System/resourceManager.h"
#include "System/styleManager.h"
#include "System/stringManager.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/Tabwindow.h"

#ifdef _M_IX86
#include <QVBoxLayout>
#endif

#include "UI/CustomHistogram.h"
#include "Actions/ActionManager.h"

#include "Math/Math.h"
#include "math/MipMath.h"
#include "mip/Log.h"
#include "MedipQT.h"
#include "DataContext.h"

#ifdef _DEBUG
#include "TimeAPI.h"
#endif

VolumeView::VolumeView(QWidget* parent)
	:OpenGLWidget(parent)
	, m_btnVolume(0)
	, m_btnLayer(0)
	, m_btnRegionROI(0)
	, m_btnPlaneSplit(0)
	, m_btnCurveSplit(0)
	, m_btnAngleSplit(0)
	, m_btnStopDisplay(0)
	, m_btnFullScreen(0)
	, m_mouseMove(false)
	, m_bVRPoint(false)
	, m_bWorkMode(false)
{
	m_camera.setZoom(20.0f);
	m_camera.setPos(mip::VECTOR3(0, 0, -15.0f));
	m_camera.setAt(mip::VECTOR3(0, 0, 0));
	m_pointControl.set(0, 0, 0);

	m_LbuttonDown = false;
	m_RbuttonDown = false;
	m_MbuttonDown = false;

	m_bAxial = false;

	m_bRightRotate = false;
	m_bLeftRotate = false;

	m_firstControlPressed = false;
	m_secondControlPressed = false;

	for (int i = 0; i < 4; i++)
		m_bSkip[i] = false;

	m_labelImage = 0;
	m_cullingTexture = NULL;
	m_fAccumulatedTime = 0;
	m_lAccumulatedFrame = 0;
	m_guidebox.reset();

	m_nIconSize = WIN_MANAGER->mainWindow->IconSize;

	m_ActVR = new QAction(STRING_MANAGER->getString(STR_VR_POINT), this);
	connect(m_ActVR, &QAction::triggered, this, &VolumeView::slot_OnVRMode);
	m_Act2DPoint = new QAction(STRING_MANAGER->getString(STR_3D_TO_2D), this);
	connect(m_Act2DPoint, &QAction::triggered, this, &VolumeView::slot_OnTo2DPoint);

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Rendering_3DVolumeViewer))
	{
		m_ActRightRotate = new QAction("Right rotation for display", this);
		m_ActLeftRotate = new QAction("Left rotation for display", this);
		m_ActRotateStop = new QAction("Stop rotation for display", this);

		connect(m_ActRightRotate, &QAction::triggered, this, &VolumeView::slot_OnRightDisplay);
		connect(m_ActLeftRotate, &QAction::triggered, this, &VolumeView::slot_OnLeftDisplay);
		connect(m_ActRotateStop, &QAction::triggered, this, &VolumeView::slot_OnStopDisplay);
	}

	m_ContextMenu = new QMenu(this);
	setContextMenu();
	//setMouseTracking(true);
	setAcceptDrops(true);
	//setFocusPolicy(Qt::StrongFocus);
	//setAttribute(Qt::WA_PaintOnScreen);
	//setAttribute(Qt::WA_NativeWindow, true);
	//setAutoFillBackground(false);
	//m_useOpenGL = true;

#ifdef _M_IX86
	m_pVTKWidget = NULL;
	m_pMainLayout = NULL;
	renderWindowInteractor = nullptr;
	renderWindowVTK = nullptr;

	m_pVTKWidget = new QWidget();
	m_pVTKWidget->setContentsMargins(0, 0, 0, 0);
	m_pVTKWidget->hide();

	m_pMainLayout = new QVBoxLayout(this);
	m_pMainLayout->setContentsMargins(0, 0, 0, 0);
	m_pMainLayout->addWidget(m_pVTKWidget);
	setLayout(m_pMainLayout);

	initVTK();
#endif

	WIN_MANAGER->displayTimer = new QTimer(this);

	if (WIN_MANAGER->displayTimer)
		WIN_MANAGER->displayTimer->stop();

	connect(WIN_MANAGER->displayTimer, SIGNAL(timeout()), this, SLOT(slot_OnDisplay()));

	m_WheelTimer = new QTimer(this);

	if (m_WheelTimer)
	{
		m_WheelTimer->stop();
		m_WheelTimer->setSingleShot(true);
		connect(m_WheelTimer, SIGNAL(timeout()), this, SLOT(slot_OnWheelTimeout()));
	}

	HoverWidget = NULL;
	memset(&m_resetUIInfo, 0x00, sizeof(VOLUMEVIEW_RESET_UI_INFO));

}

VolumeView::~VolumeView()
{
}

void VolumeView::setImgTranslation(mip::VECTOR3 translation)
{
	mip::MATRIX44 matVolume = m_trVolumModel.getMatrix();

	m_trPlaneImage.addTranslate(translation);

	TransformRotate(m_trPlaneImage, m_camera, &(matVolume));
}

void VolumeView::hideControls()
{
	if (m_btnRegionROI) m_btnRegionROI->hide();
	if (m_btnStopDisplay) m_btnStopDisplay->hide();
	if (m_btnPlaneSplit) m_btnPlaneSplit->hide();
	if (m_btnCurveSplit) m_btnCurveSplit->hide();
	if (m_btnAngleSplit) m_btnAngleSplit->hide();
	if (m_btnFullScreen)
	{
		if (!m_btnFullScreen->isHidden())
		{
			m_btnFullScreen->hide();
			renderLater();
		}
	}
}

void VolumeView::showControls()
{
	setWorkMode(WIN_MANAGER->getWorkMode());
	bool isEnable = g_Renderer->isAvailableVolumeRender();

#ifndef _M_IX86
	if (m_btnRegionROI)
	{
		if (isEnable)
			m_btnRegionROI->show();
		else
			m_btnRegionROI->hide();
	}

	if (m_btnPlaneSplit)
	{
		if (isEnable)
			m_btnPlaneSplit->show();
		else
			m_btnPlaneSplit->hide();
	}

	if (m_btnCurveSplit)
	{
		if (isEnable)
			m_btnCurveSplit->show();
		else
			m_btnCurveSplit->hide();
	}

	if (m_btnAngleSplit)
	{
		if (isEnable)
			m_btnAngleSplit->show();
		else
			m_btnAngleSplit->hide();
	}

#endif
	if ((m_bLeftRotate || m_bRightRotate) && m_btnStopDisplay)
	{
		m_btnStopDisplay->show();
		if (!isEnable)
			m_btnStopDisplay->hide();
	}

	if (m_btnFullScreen)
		m_btnFullScreen->show();

}

void VolumeView::enableControls(bool bEnable)
{
	//	throw std::logic_error("The method or operation is not implemented.");
	if (m_btnRegionROI) m_btnRegionROI->setEnabled(bEnable);
	if (m_btnStopDisplay) m_btnStopDisplay->setEnabled(bEnable);
	if (m_btnPlaneSplit) m_btnPlaneSplit->setEnabled(bEnable);
	if (m_btnCurveSplit) m_btnCurveSplit->setEnabled(bEnable);
	if (m_btnAngleSplit) m_btnAngleSplit->setEnabled(bEnable);
	//	if (m_btnFullScreen) m_btnFullScreen->setEnabled(bEnable);
}

void VolumeView::reserveInit(HWND hwnd)
{
	m_hwnd = hwnd;
}

void VolumeView::resetUI()
{
	memset(&m_resetUIInfo, 0x00, sizeof(VOLUMEVIEW_RESET_UI_INFO));
	m_resetUIInfo.bResetUIExcuted = true;
	m_resetUIInfo.camera = m_camera;
	m_resetUIInfo.fCameraXZAngle = m_fCameraXZAngle;
	m_resetUIInfo.trVolumModel = m_trVolumModel;
	m_resetUIInfo.trPlaneCut = m_trPlaneCut;
	m_resetUIInfo.trPlaneImage = m_trPlaneImage;

	if (DATA_CONTEXT->volume_data.isValidate())
	{
		float size = DATA_CONTEXT->volume_data.getSizeX() * 0.5f;
		if (size < DATA_CONTEXT->volume_data.getSizeY() * 0.5f) size = DATA_CONTEXT->volume_data.getSizeY() * 0.5f;
		if (size < DATA_CONTEXT->volume_data.getSizeZ() * 0.5f) size = DATA_CONTEXT->volume_data.getSizeZ() * 0.5f;

		mip::VECTOR3 cameraPos = mip::VECTOR3(-3.2299f, 2.0845f, size * 2);

#ifdef USE_RIGHT_HAND
		m_camera.setPos(cameraPos);
#else
		m_camera.setPos(cameraPos);
#endif
		m_camera.setZoomMax(size * 3);
		m_camera.setZoom(size * 2.0f);
		//m_camera.setZoom(10.0f);
		//m_camera.setNearFar(0.1f, size * 4);
		m_camera.setNearFar(0.1f, size * 8);

		cameraPos.y = 0.0f;
		cameraPos.normalize();
		m_fCameraXZAngle = acosf(cameraPos.dot(mip::VECTOR3(0.0f, 0.0f, 1.0f)));
	}
	else
	{
		mip::VECTOR3 cameraPos = mip::VECTOR3(-3.2299f, 2.0845f, 15.0f);

		m_camera.setPos(cameraPos);
		m_camera.setZoom(20.0f);

		cameraPos.y = 0.0f;
		cameraPos.normalize();
		m_fCameraXZAngle = acosf(cameraPos.dot(mip::VECTOR3(0.0f, 0.0f, 1.0f)));
	}

	m_camera.setAt(mip::VECTOR3(0, 0, 0));

	m_trVolumModel.zero();
	m_trVolumModel.addRotateX(3.14f); //180도
	m_trPlaneCut.zero();
	m_trPlaneImage.zero();

	renderLater();
}

void VolumeView::resetUICancel()
{
	if (m_resetUIInfo.bResetUIExcuted)
	{
		m_resetUIInfo.bResetUIExcuted = false;
		m_camera = m_resetUIInfo.camera;
		m_fCameraXZAngle = m_resetUIInfo.fCameraXZAngle;
		m_trVolumModel = m_resetUIInfo.trVolumModel;
		m_trPlaneCut = m_resetUIInfo.trPlaneCut;
		m_trPlaneImage = m_resetUIInfo.trPlaneImage;
		renderLater();
	}
}

void VolumeView::setClipPrefer()
{
	WIN_MANAGER->setClipOpt();
}

void VolumeView::resizeEvent(QResizeEvent* event)
{
	if (event == NULL) return;

	int width = event->size().width();
	int height = event->size().height();

#ifdef _M_IX86
	if (m_pVTKWidget)
	{
		m_pVTKWidget->setGeometry(0, 0, width, height);

		if (renderWindowVTK)
		{
			renderWindowVTK->SetSize(m_pVTKWidget->width(), m_pVTKWidget->height());
		}
	}
#endif

	initInScreenRightMenu();
	updateScreenLeftMenu(width, height);
	m_camera.setScreenSize(width, height);

	m_camera.updateOrtho();

	g_Renderer->setProj(m_camera.getProj());
	renderLater();
}

mip::VECTOR3 VolumeView::getScreenToProj(float x, float y)
{
	return mip::geom::Screen2Proj(x, y, this->width(), this->height());
}

mip::MATRIX44 VolumeView::getWVP()
{
	return m_trVolumModel.getMatrix() * m_camera.getView() * m_camera.getProj();
}

mip::VECTOR3 VolumeView::getScreenToLocal(muint32 screen_x, muint32 screen_y, float depth)
{
	return m_camera.getWorldPoint(screen_x, screen_y, depth, &m_trVolumModel.getMatrix());
}

mip::VECTOR3 VolumeView::getLocalToScreen(float x, float y, float z)
{
	return m_camera.getScreenPoint(mip::VECTOR3(x, y, z), &m_trVolumModel.getMatrix());
}

mip::VECTOR3 VolumeView::getLocalToVolume(float x, float y, float z)
{
	mip::VECTOR3 basisVec;
	mip::VECTOR3 LocalVec;
	mip::VECTOR3 volumeVec;

	basisVec.set((-((float)DATA_CONTEXT->volume_data.getCX() / 2)) * DATA_CONTEXT->volume_data.getSpaceX(),
		(-((float)DATA_CONTEXT->volume_data.getCY() / 2)) * DATA_CONTEXT->volume_data.getSpaceY(),
		(-((float)DATA_CONTEXT->volume_data.getCZ() / 2)) * DATA_CONTEXT->volume_data.getSpaceZ());

	LocalVec.set(x, y, z);

	volumeVec.set((LocalVec - basisVec));

	volumeVec.x /= DATA_CONTEXT->volume_data.getSpaceX();
	volumeVec.y /= DATA_CONTEXT->volume_data.getSpaceY();
	volumeVec.z /= DATA_CONTEXT->volume_data.getSpaceZ();

	return volumeVec;
}

void VolumeView::setContextMenu()
{
	m_ContextMenu->clear();
	m_ContextMenu->setStyleSheet("background: rgba(48, 48, 48, 255);");
	m_ContextMenu->setContextMenuPolicy(Qt::DefaultContextMenu);

	m_ContextMenu->addAction(m_ContextMenu->addSeparator());
	if (m_bVRPoint)
	{
		m_ContextMenu->addAction(m_ActVR);
		m_ContextMenu->addAction(m_Act2DPoint);
		m_ContextMenu->addAction(m_ContextMenu->addSeparator());
	}

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Rendering_3DVolumeViewer))
	{
		if (!m_bLeftRotate)
			m_ContextMenu->addAction(m_ActLeftRotate);
		if (!m_bRightRotate)
			m_ContextMenu->addAction(m_ActRightRotate);
		if (m_bRightRotate || m_bLeftRotate)
			m_ContextMenu->addAction(m_ActRotateStop);
	}
}



void VolumeView::setWorkMode(WORK_MODE mode, bool iconRefresh)
{
	static QIcon freeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_FREEDRAWROI, m_nIconSize, m_nIconSize) };
	static QIcon planeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_PLANE_CUT, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_PLANE_CUT, m_nIconSize, m_nIconSize) };
	static QIcon splitIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_SPLIT_CURVE, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_SPLIT_CURVE, m_nIconSize, m_nIconSize) };
	static QIcon angleSplitIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_3D_ANGLE_SPLIT, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_3D_ANGLE_SPLIT_DOWN, m_nIconSize, m_nIconSize) };

	WORK_MODE prMode = WIN_MANAGER->getWorkMode();
	WIN_MANAGER->setWorkMode(mode);

	m_polyLine.clear();

	if (prMode != mode)
	{
		switch (prMode)
		{
		case WORK_CAPTURE:
			if (WIN_MANAGER->GetTab()->getCaptureTab())
				WIN_MANAGER->GetTab()->getCaptureTab()->cancelCapture();
			break;
		case WORK_REGION_ROI:
			break;
		case WORK_3D_CURVE_SPLIT:
			break;
		case WORK_3D_PLANE_SPLIT:
			WIN_MANAGER->setClipOpt();
			break;
		case WORK_3D_ANGLE_SPLIT:
			break;
		case WORK_NONE:
			WIN_MANAGER->layerRGLocation.clear();
			break;
		}
	}

	m_cursor = Qt::ArrowCursor;

	updateToolButtonIcon_As_WorkMode(m_btnRegionROI, WORK_REGION_ROI, mode, freeIcon, iconRefresh);

	bool chkMode = false;
	bool chkRefresh = false;
	if (m_btnPlaneSplit)
	{
		if (chkRefresh = ((mode == WORK_3D_PLANE_SPLIT) != (chkMode = m_btnPlaneSplit->isChecked())))
		{
			chkMode = !chkMode;
			m_btnPlaneSplit->setChecked(chkMode);
			m_btnPlaneSplit->setIcon(planeIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnPlaneSplit->setIcon(planeIcon[chkMode]);
	}

	if (m_btnCurveSplit)
	{
		if (chkRefresh = ((mode == WORK_3D_CURVE_SPLIT) != (chkMode = m_btnCurveSplit->isChecked())))
		{
			chkMode = !chkMode;
			m_btnCurveSplit->setChecked(chkMode);
			m_btnCurveSplit->setIcon(splitIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnCurveSplit->setIcon(splitIcon[chkMode]);
	}

	if (m_btnAngleSplit)
	{
		if (chkRefresh = ((mode == WORK_3D_ANGLE_SPLIT) != (chkMode = m_btnAngleSplit->isChecked())))
		{
			chkMode = !chkMode;
			m_btnAngleSplit->setChecked(chkMode);
			m_btnAngleSplit->setIcon(angleSplitIcon[chkMode]);
		}
		else if (!chkRefresh && iconRefresh)
			m_btnAngleSplit->setIcon(angleSplitIcon[chkMode]);
	}

	switch (mode)
	{
	case WORK_CAPTURE:
	{
		bool _mode = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureMode();

		if (_mode)
			m_cursor = Qt::PointingHandCursor;
		else
			m_cursor = Qt::CrossCursor;
	}
	break;
	case WORK_REGION_ROI:
		break;
	case WORK_3D_CURVE_SPLIT:
		break;
	case WORK_3D_PLANE_SPLIT:
		break;
	case WORK_ANNOTATION_PATH:
		m_cursor = Qt::CrossCursor;
	default:
		m_bWorkMode = false;
		break;
	}

	this->setCursor(m_cursor);

	WIN_MANAGER->renderLater_GridView(true);
}


void VolumeView::bringVolume()
{
	//mip::MATRIX44 matWorld = g_Renderer->getWorld();
	//matWorld.identity();
	//mip::MATRIX44 camera = getHMDViewMatrix();
	//mip::VECTOR4 look = mip::VECTOR4(0.0f, 0.0f, 1.0f, 0.0f);
	//mip::VECTOR4 pos = mip::VECTOR4(0.0f, 0.0f, 0.0f, 1.0f);

	//look = (camera * look) * 3.5;
	//pos = camera * pos;

	//pos = pos + look;

	//mip::VECTOR3 vec(pos.x, pos.y, pos.z);

	//m_trVolumModel.zero();
	//m_trVolumModel.addRotateX(3.14f);
	//m_trVolumModel.addTranslate(vec);
}

void VolumeView::syncFullscreen()
{
	slot_OnFullScreen();
}


/*@function useSkipRender
**@brief render skip mode setting
**@param index skip mode type (0:wl change, 1: object move, 2: object zoom, 3: alpha change)
**@param res set/unset (default : set(true))
*/
void VolumeView::useSkipRender(int index, bool res /*= true*/)
{
	if (m_bSkip[index] != res)
		m_bSkip[index] = res;
}

void VolumeView::keyPressEvent(QKeyEvent* e)
{
	checkModifiers(e);

	/*
		if (e->key() == Qt::Key_Right)
		{
			if (!m_updateFrameSet)
				revolveCamera(true);
		}
		else if (e->key() == Qt::Key_Left)
		{
			if (!m_updateFrameSet)
				revolveCamera(false);
		}*/

	TransformRotate(m_trVolumModel, e->key());

	renderLater();
}

void VolumeView::keyReleaseEvent(QKeyEvent* e)
{
	checkModifiers(e, false);

	bool chkRG = false;

	if (WIN_MANAGER->getRGType() == LAYER_RG_CTRL)
	{
		if (!m_ctrl)
			chkRG = true;
	}
	else if (WIN_MANAGER->getRGType() == LAYER_RG_ALT)
	{
		if (!m_alt)
			chkRG = true;
	}
	else if (WIN_MANAGER->getRGType() == LAYER_RG_SHIFT)
	{
		if (!m_shift)
			chkRG = true;
	}
	if (chkRG && WIN_MANAGER->getRGLock())
	{
		if ((!WIN_MANAGER->layerRGLocation.isEmpty()))
		{
			if (WIN_MANAGER->GetTab())
			{
				ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();

				if (tab)
				{
					tab->ApplyRegionGrowing();
				}
			}
			if (WIN_MANAGER->getRGDrawLock(true))
			{
				WIN_MANAGER->layerRGLocation.clear();
				WIN_MANAGER->setRGDrawUnlock();
			}
		}
		WIN_MANAGER->setRGUnlock();
	}

	if (e->key() == Qt::Key_Space)
	{
		slot_OnFullScreen();
	}

	renderLater();
}

void VolumeView::mouseMoveEvent(QMouseEvent* event)
{
	checkModifiers(event);

	if (WIN_MANAGER->getMoveFocus())
	{
		this->setFocus();
	}

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	m_mouseMove = true;
	WIN_MANAGER->setLatestActiveViewType(WT_VOLUME);
	m_preMousePos = m_MousePos;
	m_MousePos = event->pos();
	m_camera.setScreenXY(m_MousePos.x(), m_MousePos.y());

	bool default_state = true;
	switch (WIN_MANAGER->getWorkMode())
	{
	case WORK_CAPTURE:
		if (m_LbuttonDown)
		{
			default_state = false;
			bool mode = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureMode();

			if (!mode) //rectangle
			{
				if (m_polyLine.count() >= 2)
				{
					m_polyLine.replace(1, m_MousePos);
				}
				else
					m_polyLine.push_back(m_MousePos);
			}
			renderLater();
		}
		break;
	case WORK_REGION_ROI:
	case WORK_3D_CURVE_SPLIT:
	{
		bool res = WIN_MANAGER->getWorkMode() == WORK_REGION_ROI ? m_RbuttonDown : m_LbuttonDown;
		if (res)
		{
			default_state = false;
			if (g_Renderer->isAvailableVolumeRender() == false)
				break;
			if (!m_polyLine.contains(m_MousePos))
				m_polyLine.append(m_MousePos);

			renderLater();
		}
	}
	break;
	case WORK_3D_PLANE_SPLIT:
		break;
	default:
	{
		bool chkRG = false;

		if (WIN_MANAGER->getRGType() == LAYER_RG_CTRL)
		{
			if (!m_ctrl)
				chkRG = true;
		}
		else if (WIN_MANAGER->getRGType() == LAYER_RG_ALT)
		{
			if (!m_alt)
				chkRG = true;
		}
		else if (WIN_MANAGER->getRGType() == LAYER_RG_SHIFT)
		{
			if (!m_shift)
				chkRG = true;
		}

		if (chkRG && WIN_MANAGER->getRGLock())
		{
			//	
			if ((!WIN_MANAGER->layerRGLocation.isEmpty()))
			{
				if (WIN_MANAGER->GetTab())
				{
					ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();

					if (tab)
					{
						tab->ApplyRegionGrowing();
					}
				}
				if (WIN_MANAGER->getRGDrawLock(true))
				{
					WIN_MANAGER->layerRGLocation.clear();
					WIN_MANAGER->setRGDrawUnlock();
				}
			}
			WIN_MANAGER->setRGUnlock();
		}

		if (WIN_MANAGER->isClipMode() /*|| m_checkCoronal->isChecked() || m_checkSaggital->isChecked()*/)
		{
			m_mouseMove = false;
			renderLater();
		}

	}
	break;
	}

	if (default_state == true && m_touchZoomEvent == false)
	{
		mip::MATRIX44 matVolume = m_trVolumModel.getMatrix();

		double offsetDPI = m_updateFrameSet ? WIN_MANAGER->getDPIValue() : 1.f;

		bool planecut = WIN_MANAGER->getWorkMode() == WORK_3D_PLANE_SPLIT;
		if (m_MbuttonDown)
		{
			mip::VECTOR3 v1;
			mip::VECTOR3 v2;

			if ((!m_ctrl) && (WIN_MANAGER->isClipMode() || planecut/*|| m_checkCoronal->isChecked() || m_checkSaggital->isChecked()*/))
			{
				if (m_updateFrameSet == true) // VR Mode
				{
					mip::MATRIX44 view = getHMDViewMatrix(0, &matVolume);
					mip::MATRIX44 proj = getHMDProjMatrix(0);
					v1 = m_camera.getWorldPoint(view, proj, 0, &matVolume);
					v2 = m_camera.getPreWorldPoint(view, proj, 0, &matVolume);

					if (planecut)
						m_trPlaneCut.addTranslate((v1 - v2) * offsetDPI);
					else
						m_trPlaneImage.addTranslate((v1 - v2) * offsetDPI);


				}
				else
				{
					useSkipRender(1);
					v1 = m_camera.getWorldPoint(0, &matVolume);
					v2 = m_camera.getPreWorldPoint(0, &matVolume);
					if (planecut)
						m_trPlaneCut.addTranslate((v1 - v2));
					else
						m_trPlaneImage.addTranslate((v1 - v2));
				}
			}
			else
			{
				if (m_updateFrameSet == true)  // VR Mode
				{
					mip::MATRIX44 view = getHMDViewMatrix();
					mip::MATRIX44 proj = getHMDProjMatrix();
					v1 = m_camera.getWorldPoint(view, proj, 0.5f);
					v2 = m_camera.getPreWorldPoint(view, proj, 0.5f);

					m_trVolumModel.addTranslate((v1 - v2) * offsetDPI);
					/*v1 = m_camera.getWorldPoint();
					v2 = m_camera.getPreWorldPoint();

					m_trVolumModel.addTranslate((v1 - v2));*/
				}
				else
				{
					useSkipRender(1);

					v1 = m_camera.getWorldPoint();
					v2 = m_camera.getPreWorldPoint();

					m_trVolumModel.addTranslate((v1 - v2));
				}
			}

			renderLater();
		}

		if (m_LbuttonDown)
		{
			useSkipRender(1);
			if ((!m_ctrl) && (WIN_MANAGER->isClipMode() || planecut /*|| m_checkCoronal->isChecked() || m_checkSaggital->isChecked()*/))
			{
				if (planecut)
					TransformRotate(m_trPlaneCut, m_camera, &(matVolume));
				else
					TransformRotate(m_trPlaneImage, m_camera, &(matVolume));
			}
			else
			{
				TransformRotate(m_trVolumModel, m_camera);
			}

			renderLater();
		}
	}

	m_preMousePos = event->pos();
	m_camera.setPreScreenXY(m_preMousePos.x(), m_preMousePos.y());
}

void VolumeView::mousePressEvent(QMouseEvent* event)
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	m_MousePos = event->pos();
	m_preMousePos = event->pos();
	m_mouseMove = false;
	m_camera.setScreenXY(m_MousePos.x(), m_MousePos.y());
	m_camera.setPreScreenXY(m_MousePos.x(), m_MousePos.y());

	switch (event->buttons())
	{
	case Qt::LeftButton:
		m_LbuttonDown = true;
		m_bVRPoint = false;
		break;
	case Qt::MiddleButton:
		m_MbuttonDown = true;
		m_bVRPoint = false;
		break;
	case Qt::RightButton:
		m_RbuttonDown = true;
		break;
	default:
		break;
	}


	switch (WIN_MANAGER->getWorkMode())
	{
	case WORK_CAPTURE:
	{
		if (m_LbuttonDown)
		{
			//TODO rectangle & window click
			bool mode = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureMode();
			bool type = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureType();
			m_polyLine.clear();
			if (mode)
			{
				/*QImage *img = getCaptureImage(mode, type);

				if(img)
					WIN_MANAGER->captureList.push_back(img);
*/
				QImage img = getCaptureImage(mode, type);

				if (img != QImage())
					ACTION_MANAGER->action_Capture_image_Add(img);

				setWorkMode(WORK_NONE);
			}
			else //rectangle
			{
				m_bWorkMode = true;
				m_polyLine.push_back(m_MousePos);
			}
		}
	}
	break;
	case WORK_REGION_ROI:
	case WORK_3D_CURVE_SPLIT:
	{
		bool res = WIN_MANAGER->getWorkMode() == WORK_REGION_ROI ? m_RbuttonDown : m_LbuttonDown;
		if (res)
		{
			m_bWorkMode = true;
			m_bVRPoint = false;
			if (g_Renderer->isAvailableVolumeRender() == false)
				return;

			m_polyLine.append(m_MousePos);
		}

	}
	break;
	case WORK_3D_PLANE_SPLIT:
		if (m_RbuttonDown)
		{
			m_bWorkMode = true;
			//todo planecut action
			mip::MATRIX44 matOffset = mip::MATRIX44::Identity;
			matOffset.translation(-DATA_CONTEXT->volume_data.getSizeX() * 0.5f, -DATA_CONTEXT->volume_data.getSizeY() * 0.5f, -DATA_CONTEXT->volume_data.getSizeZ() * 0.5f);

			mip::MATRIX44 matinvOffset;
			mip::math::MatrixInverse(&matinvOffset, &matOffset);

			mip::MATRIX44 matPI = m_trPlaneCut.getMatrix() * matinvOffset;
			std::vector<mip::VECTOR3> planePointList;
			std::vector<mip::VECTOR3> planeCoordList;
			planePointList.clear();
			planeCoordList.clear();
			DATA_CONTEXT->volume_data.getPlaneSurface(WT_AXIAL, planePointList, planeCoordList, &matPI);

			mip::PLANE plane(planePointList[0], planePointList[1], planePointList[2]);

			ACTION_MANAGER->action_Split3D(this, QPolygon(), WIN_MANAGER->getSelectedMask(),
				WIN_MANAGER->getSelectedMaskByteIndex(), DATA_CONTEXT->volume_data.getCurrentMaskInfo()->uid, WORK_3D_PLANE_SPLIT, plane);
		}
		break;
	case WORK_3D_ANGLE_SPLIT:
	{
		if (m_LbuttonDown)
		{
			mip::VECTOR3 v3HitPoint;
			bool ret = pickingMask(mip::VECTOR2(m_MousePos.x(), m_MousePos.y()), v3HitPoint);
			if (ret)
			{
				WIN_MANAGER->vecAngleSplitPoint.push_back(v3HitPoint);
				if (WIN_MANAGER->vecAngleSplitPoint.size() >= 3)
				{
					/* Color Table 업데이트*/
					WIN_MANAGER->getAnnotationColor();

					COLOR color = toCOLOR(WIN_MANAGER->annoColor);
					AnnoAngle* annoAngle = new AnnoAngle(
						WIN_MANAGER->vecAngleSplitPoint[0],
						WIN_MANAGER->vecAngleSplitPoint[1],
						WIN_MANAGER->vecAngleSplitPoint[2],
						Annotation::DRAWING_FINISHED,
						color);

					ACTION_MANAGER->action_Annotation_Angle_Add(annoAngle);

					qDebug() << "v3HitPoint anno  p1 - x:" << WIN_MANAGER->vecAngleSplitPoint[0].x << " y:" << WIN_MANAGER->vecAngleSplitPoint[0].y << " z:" << WIN_MANAGER->vecAngleSplitPoint[0].z;
					qDebug() << "v3HitPoint anno  p2 - x:" << WIN_MANAGER->vecAngleSplitPoint[1].x << " y:" << WIN_MANAGER->vecAngleSplitPoint[1].y << " z:" << WIN_MANAGER->vecAngleSplitPoint[1].z;
					qDebug() << "v3HitPoint anno  p3 - x:" << WIN_MANAGER->vecAngleSplitPoint[2].x << " y:" << WIN_MANAGER->vecAngleSplitPoint[2].y << " z:" << WIN_MANAGER->vecAngleSplitPoint[2].z;
					WIN_MANAGER->vecAngleSplitPoint.clear();
					WIN_MANAGER->anotationList.last()->setSelect();
					WIN_MANAGER->renderLater_3DView();

				}
				else
				{
					for (auto it = WIN_MANAGER->anotationList.begin(); it != WIN_MANAGER->anotationList.end(); ++it)
					{
						(*it)->clearSelect();
					}
				}
			}
		}

		if (m_RbuttonDown)
		{
			mip::VECTOR3 p1, p2, p3;

			AnnoAngle* annoAngle = nullptr;
			for (auto it = WIN_MANAGER->anotationList.begin(); it != WIN_MANAGER->anotationList.end(); ++it)
			{
				if ((*it)->isSelected() && (*it)->getType() == AT_ANGLE)
				{
					annoAngle = static_cast<AnnoAngle*>(*it);
					p1 = annoAngle->getV1();
					p2 = annoAngle->getV2();
					p3 = annoAngle->getV3();
				}
			}

			if (annoAngle)
			{
				// matrix
				mip::MATRIX44 matOffset = mip::MATRIX44::Identity;
				matOffset.translation(-DATA_CONTEXT->volume_data.getSizeX() * 0.5f, -DATA_CONTEXT->volume_data.getSizeY() * 0.5f, -DATA_CONTEXT->volume_data.getSizeZ() * 0.5f);

				mip::MATRIX44 matinvOffset;
				mip::math::MatrixInverse(&matinvOffset, &matOffset);

				// plane
				mip::VECTOR3 vec1 = p1 - p2;
				mip::VECTOR3 vec2 = p3 - p2;
				mip::VECTOR3 vPerpendicular = vec1.cross(vec2);
				vPerpendicular.normalize();
				vPerpendicular += p2;
				//ACTION_MANAGER->action_Annotation_Angle_Add(p1, p2, vPerpendicular, QColor(0, 255, 0, 255));				

				p1.transform(matinvOffset);
				p2.transform(matinvOffset);
				p3.transform(matinvOffset);
				vPerpendicular.transform(matinvOffset);

				mip::PLANE plane_1(p1, p2, vPerpendicular);
				mip::PLANE plane_2(p3, p2, vPerpendicular);


				ACTION_MANAGER->action_Split3D(this, QPolygon(), WIN_MANAGER->getSelectedMask(),
					WIN_MANAGER->getSelectedMaskByteIndex(), DATA_CONTEXT->volume_data.getCurrentMaskInfo()->uid, WORK_3D_ANGLE_SPLIT, plane_1, plane_2);
				// 				ACTION_MANAGER->action_Split3D(this, QPolygon(), WIN_MANAGER->getSelectedMask(),
				// 					WIN_MANAGER->getSelectedMaskByteIndex(), DATA_CONTEXT->volume_data.getCurrentMaskInfo()->uid, WORK_3D_PLANE_SPLIT, plane_1);

				annoAngle->clearSelect();
			}
		}
	}
	break;
	case WORK_ANNOTATION_PATH:
	{
		if (m_LbuttonDown)
		{
			mip::VECTOR3 v3HitPoint;
			std::vector<std::pair<int, mask8 >>  vecHitMaskBit;

			bool ret = false;
			ret = pickingMask(mip::VECTOR2(m_MousePos.x(), m_MousePos.y()), v3HitPoint, vecHitMaskBit);
			if (ret)
			{
#if 1
				// reposition
				mip::VECTOR3 vVoxelPos(0, 0, 0);
				WIN_MANAGER->volume_renderer.getVoxelIndex(v3HitPoint, &vVoxelPos);

				int group = WIN_MANAGER->getAnimationGroup();

				QVector3D vec(vVoxelPos.x, vVoxelPos.y, vVoxelPos.z);
				if (!WIN_MANAGER->isContainAniPoint(vec, group))
					ACTION_MANAGER->action_Annotation_Path_Add(vec, group);
#endif
			}
		}

	}
	break;
	case	WORK_NONE:
		if (m_LbuttonDown)
		{
			WIN_MANAGER->vecAngleSplitPoint.clear();
			///////////////////////////////
			mip::VECTOR3 v3HitPoint;
			std::vector<std::pair<int, mask8 >>  vecHitMaskBit;
			bool ret = pickingMask(mip::VECTOR2(m_MousePos.x(), m_MousePos.y()), v3HitPoint, vecHitMaskBit);

			if (ret == true)
			{
				WIN_MANAGER->vecAngleSplitPoint.push_back(v3HitPoint);
				// reposition
				mip::VECTOR3 vVoxelPos;
				WIN_MANAGER->volume_renderer.getVoxelIndex(v3HitPoint, &vVoxelPos);

				/* 공용 MPRInfo 값 저장 */
				float HU = DATA_CONTEXT->volume_data.getData(vVoxelPos.x, vVoxelPos.y, vVoxelPos.z);
				WIN_MANAGER->mainSegmentWidget->setMPRViewInfo(
					QVector3D(vVoxelPos.x, vVoxelPos.y, vVoxelPos.z),
					HU
				);

				WindowBase* pAxial = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);
				WindowBase* pCoronal = WIN_MANAGER->mainSegmentWidget->getWindow(WT_CORONAL);
				WindowBase* pSagittal = WIN_MANAGER->mainSegmentWidget->getWindow(WT_SAGITTAL);

				pAxial->setDepth((int)vVoxelPos.z);
				pCoronal->setDepth((int)vVoxelPos.y);
				pSagittal->setDepth((int)vVoxelPos.x);

				// select mask
				//vecHitMaskBit
				DATA_CONTEXT->volume_data.setMultiSelectUIDByMaskBit(vecHitMaskBit);
				for (auto i = 0; i < vecHitMaskBit.size(); ++i)
				{
					MaskInfo* pMaskInfo = DATA_CONTEXT->volume_data.findMaskInfo(vecHitMaskBit[i].first, vecHitMaskBit[i].second);
					if (pMaskInfo)
					{
						WIN_MANAGER->updateUIMultiSelect(DATA_CONTEXT->volume_data.getMultiSelectMaskUID());
					}
				}
			}
			/////////////////////////////////////////
		}
		break;
	default:
		break;
	}
}

void VolumeView::mouseReleaseEvent(QMouseEvent* event)
{
	useSkipRender(1, false);

	switch (WIN_MANAGER->getWorkMode())
	{
	case WORK_CAPTURE:
	{
		m_bWorkMode = false;
		if (m_LbuttonDown && m_polyLine.count() >= 2)
		{
			bool type = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureType();

			//TODO
			/*QImage *img = getCaptureImage(false, type);

			if (img)
				WIN_MANAGER->captureList.push_back(img);
*/
			QImage img = getCaptureImage(false, type);

			if (img != QImage())
				ACTION_MANAGER->action_Capture_image_Add(img);

			m_polyLine.clear();
		}
		setWorkMode(WORK_NONE);
	}
	break;
	case WORK_REGION_ROI:
	case WORK_3D_CURVE_SPLIT:
	{
		m_bWorkMode = false;
		WORK_MODE mode = WIN_MANAGER->getWorkMode();
		bool res = mode == WORK_REGION_ROI ? m_RbuttonDown : m_LbuttonDown;
		if (res)
		{
			// delete
			if (g_Renderer->isAvailableVolumeRender() == false)
			{
				m_polyLine.clear();
				return;
			}
			if (m_polyLine.size() >= 2)
			{
				if (WORK_REGION_ROI == mode)
					ACTION_MANAGER->action_FreeDrawROI3D(m_polyLine, this, WIN_MANAGER->getSelectedMask(), true, false,
						WIN_MANAGER->getSelectedMaskByteIndex(), DATA_CONTEXT->volume_data.getCurrentMaskInfo()->uid);
				else
					ACTION_MANAGER->action_Split3D(this, m_polyLine, WIN_MANAGER->getSelectedMask(),
						WIN_MANAGER->getSelectedMaskByteIndex(), DATA_CONTEXT->volume_data.getCurrentMaskInfo()->uid);

				m_polyLine.clear();
			}
			m_polyLine.clear();
		}
	}
	break;
	case WORK_3D_ANGLE_SPLIT:
	case WORK_3D_PLANE_SPLIT:
		if (m_RbuttonDown)
		{
			m_bWorkMode = false;
		}
		break;
	default:
		/*if (!m_mouseMove)
		{*/
		if (m_RbuttonDown)
		{
			if (!getUpdateFrameState())
			{
				setContextMenu();
				m_ContextMenu->exec(event->globalPos());
			}
		}
		//}
		break;
	}

	m_LbuttonDown = false;
	m_RbuttonDown = false;
	m_MbuttonDown = false;

	renderLater();
}

void VolumeView::focusInEvent(QFocusEvent*)
{
	setWorkMode(WIN_MANAGER->getWorkMode());
}

void VolumeView::wheelEvent(QWheelEvent* event)
{
	if (DATA_CONTEXT->volume_data.isValidate() == false || m_bWorkMode) return;

	bool checkView = false;

	if (m_WheelTimer)
	{
		m_WheelTimer->stop();
		if (!m_updateFrameSet)
		{
			useSkipRender(2);
			m_WheelTimer->start(500);
		}
	}

	bool planecut = WIN_MANAGER->getWorkMode() == WORK_3D_PLANE_SPLIT;

	if (WIN_MANAGER->isClipMode() || planecut/*|| m_checkCoronal->isChecked() || m_checkSaggital->isChecked()*/)
	{
		mip::MATRIX44 matPlaneImage = planecut ? m_trPlaneCut.getMatrix() : m_trPlaneImage.getMatrix();
		mip::VECTOR3 dir = matPlaneImage.getScaledZaxis();

		if (!m_ctrl)
		{
			if (planecut)
				m_trPlaneCut.addTranslate(dir * 0.001f * event->delta());
			else
				m_trPlaneImage.addTranslate(dir * 0.001f * event->delta());
		}
		else
			m_camera.wheelZoom(event->delta() * 0.01f);
	}
	else
	{
		if (m_updateFrameSet)
		{

		}
		else
			m_camera.wheelZoom(event->delta() * 0.01f);
	}
	renderLater();
}

void VolumeView::renderScene(QPainter* p, bool skip_mode)
{
	m_camera.setScreenSize(this->width(), this->height());
	m_camera.updateOrtho();
	m_camera.updateLookAtCamera();

	mip::MATRIX44 matWorld = m_trVolumModel.getMatrix();
	mip::MATRIX44 matView = m_camera.getView();
	mip::MATRIX44 matProj = m_camera.getProj();
	mip::MATRIX44 matModelWorld;

	g_Renderer->setWorld(matWorld);
	g_Renderer->setView(matView);
	g_Renderer->setProj(matProj);

	QSize window_size = this->size();

	if (!g_Renderer->beginRender(0, this->size().width(), this->size().height()))
	{
		return;
	}
	g_Renderer->clear(mip::COLOR(30, 30, 30));

	if (g_Renderer->beginScene())
	{
		if (DATA_CONTEXT->volume_data.isValidate())
		{
			g_Renderer->setViewPort(0, 0, window_size.width(), window_size.height());

			mip::VolumeRenderer::DrawBG(g_Renderer, (mip::BGTYPE)6);

			int render_type = WIN_MANAGER->getRenderType();

			renderModel(matWorld, matView, matProj, WIN_MANAGER->getCullingMesh());

			if (render_type != RT_SURFACE)
			{
				g_Renderer->setWorld(matWorld);
				renderVolume(skip_mode, matWorld, matView, matProj);
			}

#ifdef _DEBUG
			if (m_guidebox.isNull() == false)
			{
				//			g_Renderer->setWorld(matWorld);
				std::vector<mip::VECTOR3> linelist;
				//linelist.push_back(mip::VECTOR3(0,0,0));
				//linelist.push_back(m_guidebox.min);
				//linelist.push_back(mip::VECTOR3(0, 0, 0));
				//linelist.push_back(m_guidebox.max);
				m_guidebox.getLineList(linelist);
				g_Renderer->renderLineList(linelist, mip::COLOR(255, 255, 255));
			}
#endif						
			muint32 texFont = WIN_MANAGER->getFontTexture();
			if (texFont)
			{
				g_Renderer->setWorld(matWorld);
				//g_Renderer->drawFont("KLMNOPQRSTUVWXYZ", texFont);
				//g_Renderer->drawFont("abcdefghijklmnopqrstuvwxyz", texFont);
			}

#if SUPPORT_COORDLINE
			WIN_MANAGER->volume_renderer.RenderCoordLine(g_Renderer);
#endif

#ifdef _DEBUG
			//std::string str = mip::Format("                 fps : %4.1f", (float)m_lAccumulatedFrame / m_fAccumulatedTime);
			//g_Renderer->makeText(str.c_str());
			//g_Renderer->drawText(&mip::COLOR(255,0,0));
#endif

			g_Renderer->setViewPort(window_size.width() - 100, 10, 90, 90);
#ifndef _M_IX86
			if (WIN_MANAGER->bVisibleCoordinate)
				WIN_MANAGER->volume_renderer.RenderGuideBox(g_Renderer, WIN_MANAGER->getFontTexture());
#endif
		}

		g_Renderer->endScene();
		//g_Renderer->present();
		void* rData = g_Renderer->getBackBuffer(0, 0, this->width(), this->height());
		if (rData != NULL)
		{
			QImage image((uchar*)rData, this->width(), this->height(), QImage::Format_RGB32);

			//p->setRenderHint(QPainter::Antialiasing);

			/*bool mode = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureMode();

			if (mode)
				image.save(STRING_MANAGER->cacheFilePath + "/a.png");
*/
			p->drawImage(0, 0, image);
		}
	}
	g_Renderer->endRender();
}

void VolumeView::updateToolButtonIcon_As_WorkMode(QPushButton* button, WORK_MODE targetWorkMode, WORK_MODE mode, QIcon icon[], bool iconRefresh)
{
	bool chkModeNotChanged = false;

	if (button)
	{
		/* WORK_NONE 모드일 경우 버튼을 초기상태로 변경*/
		if (mode == WORK_NONE)
		{
			button->setChecked(false);
			button->setIcon(icon[false]);
		}
		else
		{
			chkModeNotChanged = (mode == targetWorkMode);
			if (chkModeNotChanged)
			{
				button->setChecked(true);
				button->setIcon(icon[true]);
			}
			else if ((chkModeNotChanged == false) && iconRefresh)
			{
				button->setChecked(false);
				button->setIcon(icon[false]);
			}
		}
	}
}

void VolumeView::renderModel(mip::MATRIX44& world, mip::MATRIX44& view, mip::MATRIX44& proj, bool culling)
{
	if (WIN_MANAGER->getVisibleMesh() == false) return;

	mip::MATRIX44 prWorld = world;
	std::vector<mip::VECTOR3> clipped_line;

	std::vector<mip::VECTOR3> planePointList;
	std::vector<mip::VECTOR3> planeCoordList;

	mip::MATRIX44 matOffset = mip::MATRIX44::Identity;
	matOffset.translation(-DATA_CONTEXT->volume_data.getSizeX() * 0.5f, -DATA_CONTEXT->volume_data.getSizeY() * 0.5f, -DATA_CONTEXT->volume_data.getSizeZ() * 0.5f);
	int meshCount = DATA_CONTEXT->m_MeshData.GetMeshCount();
	if (DATA_CONTEXT->volume_data.isValidate() && meshCount > 0)
	{
		mip::MATRIX44 matinvOffset;
		mip::math::MatrixInverse(&matinvOffset, &matOffset);
		bool res = false;
		bool planecut = WIN_MANAGER->getWorkMode() == WORK_3D_PLANE_SPLIT;
		if (meshCount > 0)
		{
			if (WIN_MANAGER->isClipMode() || planecut)
			{
				res = planecut ? false : WIN_MANAGER->getClip2DPlane();

				DWORD clipValue = 0;
				DWORD clipValueCnt = 0;
				mip::MATRIX44 matPI = (planecut ? m_trPlaneCut.getMatrix() : m_trPlaneImage.getMatrix()) * matinvOffset;

				if (WIN_MANAGER->isClipMode() || planecut)
				{
					planePointList.clear();
					planeCoordList.clear();
					DATA_CONTEXT->volume_data.getPlaneSurface(WT_AXIAL, planePointList, planeCoordList, &matPI);

					mip::PLANE plane(planePointList[1], planePointList[2], planePointList[0]);
					g_Renderer->setClipPlane(clipValueCnt, (float*)&plane);

					clipValue |= RSCLIPPLANE0 << clipValueCnt;
					clipValueCnt++;
				}

				if (culling == true)
				{
					g_Renderer->setRenderState(mip::RS_CLIPPLANEENABLE, clipValue);
				}
			}
		}
		if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION)
		{
			res = false;
#if 0
			for (auto n = WIN_MANAGER->meshMap.begin(); n != WIN_MANAGER->meshMap.end(); ++n)
			{
				mip::MeshTopology* mesh = n.value();
				MaskInfo* minfo = DATA_CONTEXT->volume_data.getMaskInfo(n.key(), true);
				if (mesh && mesh->isLoaded() && minfo && minfo->meshConnected)
				{
					res = true;
					mip::MATRIX44 mat = (matOffset * world);
					mesh->renderBegin(g_Renderer, 0, WIN_MANAGER->getBackfaceCullingMode());
					mesh->renderPosition(&mat, view, proj, &mip::COLOR(minfo->color.r, minfo->color.g, minfo->color.b, //m_SurfaceAlpha)); 
						1.0f));

					if (m_updateFrameSet)
					{
						mesh->setLight2(g_Renderer->m_matfirstControl.getScaledZaxis() - WIN_MANAGER->matMoveCameraPos, -WIN_MANAGER->matMoveCameraPos, &mat);
					}
					mesh->render(false);
					mesh->renderEnd(g_Renderer);
				}
			}
#else
			MeshInfo* mInfo = nullptr;
			mip::MeshTopology* mesh = nullptr;
			for (int i = 0; i < meshCount; i++)
			{
				mInfo = DATA_CONTEXT->m_MeshData.GetMeshInfo(i);

				//if (mInfo && (mInfo->uid) != -1)
				if (mInfo)
				{
					if (!mInfo->show) continue;

					mesh = DATA_CONTEXT->m_MeshData.GetMesh(i);

					if (mesh && mesh->isLoaded() && mesh->m_TextureDataList.size() < 1) //20210330_byPHS_3mf_VolumeView에서 랜더링 안되게 수정
					{
						res = true;
						mip::MATRIX44 mat = (mesh->getMatrix() * matOffset * world);
						mesh->renderBegin(g_Renderer, 0, WIN_MANAGER->getBackfaceCullingMode());
						//mesh->renderPosition(&mat, view, proj, &mip::COLOR(mInfo->color.r, mInfo->color.g, mInfo->color.b, //m_SurfaceAlpha)); 
						//	1.0f));
						mesh->renderPosition(&mat, view, proj, &mip::COLOR(mInfo->color.r, mInfo->color.g, mInfo->color.b));

						if (m_updateFrameSet)
						{
							mesh->setLight2(g_Renderer->m_matfirstControl.getScaledZaxis() - WIN_MANAGER->matMoveCameraPos, -WIN_MANAGER->matMoveCameraPos, &mat);
						}

						mesh->render(false);
						mesh->renderEnd(g_Renderer);
					}
				}
			}
#endif

			if (res) res = WIN_MANAGER->isClipMode();
		}

		if (!g_Renderer->isAvailableVolumeRender() && res)
		{
			std::vector<mip::VECTOR3> tempList;
			std::vector<mip::VECTOR2> cList;
			mip::MATRIX44 matPI = planecut ? m_trPlaneCut.getMatrix() : m_trPlaneImage.getMatrix();
			DATA_CONTEXT->volume_data.getCullingPlaneSurface(tempList, cList, 0.5f, &matPI);

			if (m_cullingTexture == 0)
			{
				QImage* image = RESOURCE_MANAGER->getTexture_CullingPlane();
				if (image != NULL)
				{
					m_cullingTexture = WIN_MANAGER->createTextureFromQImage(image);
				}
			}

			if (m_cullingTexture != 0)
			{
				g_Renderer->drawPlane(tempList, cList, m_cullingTexture);
			}
		}
	}//surface only

	g_Renderer->setWorld(prWorld);
	g_Renderer->setRenderState(mip::RS_CLIPPLANEENABLE, false);

#ifdef MESH_TEST
	if (clipped_line.size())
	{
		mip::MATRIX44 mat = (matOffset * world);
		g_Renderer->setWorld(mat);

		g_Renderer->renderLineList(clipped_line, mip::COLOR(0, 255, 0, 255));
	}
#endif
}
void VolumeView::renderVolume(bool skip_mode, mip::MATRIX44& matWorld, mip::MATRIX44& matView, mip::MATRIX44& matProj, bool vr_mode, mip::MATRIX44* vr_view)
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;
	muint32 shader_quality = WIN_MANAGER->getShaderQuality();

	float HuMin = WIN_MANAGER->getWindowLevel() - WIN_MANAGER->getWindowWidth() / 2;
	if (HuMin < DATA_CONTEXT->volume_data.getHuMin())
		HuMin = DATA_CONTEXT->volume_data.getHuMin();

	float HuMax = WIN_MANAGER->getWindowLevel() + WIN_MANAGER->getWindowWidth() / 2;
	if (HuMax > DATA_CONTEXT->volume_data.getHuMax())
		HuMax = DATA_CONTEXT->volume_data.getHuMax();

	if (g_Renderer->isAvailableVolumeRender() == false ||
		shader_quality == 0)
	{
		std::vector<mip::VECTOR3> lineList;
		std::vector<mip::VECTOR2> coordList;
		mip::MATRIX44 matPI = matWorld;

		float xLength = 0.0f, yLength = 0.0f;
		float xSpace = 0.0f, ySpace = 0.0f;
		float gamma = WIN_MANAGER->getGamma();
		float alpha = WIN_MANAGER->getVolumeAlpha() * 10;
		xLength = DATA_CONTEXT->volume_data.getCY();
		yLength = DATA_CONTEXT->volume_data.getCZ();
		xSpace = DATA_CONTEXT->volume_data.getSpaceY();
		ySpace = DATA_CONTEXT->volume_data.getSpaceZ();

		lineList.clear();
		coordList.clear();
		DATA_CONTEXT->volume_data.getSegmentationPlaneSurfaceList2D(WT_AXIAL, lineList, coordList);
		int index = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
		bool res = index <= -1 ? true : false;
		if (WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneBegin2D(g_Renderer, WIN_MANAGER->getVolumeMixMode() ? 2 : 0, WIN_MANAGER->get2DPresetTexture(),
			HuMin, HuMax, xLength, yLength, xSpace, ySpace, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT)))
		{
			WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneRender2D(g_Renderer, WIN_MANAGER->getLowSpec2DMPR(WT_AXIAL), 0, lineList, coordList, 1, alpha, gamma);

			WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneEnd(g_Renderer);
		}

		xLength = DATA_CONTEXT->volume_data.getCX();
		yLength = DATA_CONTEXT->volume_data.getCY();
		xSpace = DATA_CONTEXT->volume_data.getSpaceX();
		ySpace = DATA_CONTEXT->volume_data.getSpaceY();

		lineList.clear();
		coordList.clear();
		DATA_CONTEXT->volume_data.getSegmentationPlaneSurfaceList2D(WT_SAGITTAL, lineList, coordList);

		if (WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneBegin2D(g_Renderer, WIN_MANAGER->getVolumeMixMode() ? 2 : 0, WIN_MANAGER->get2DPresetTexture(),
			HuMin, HuMax, xLength, yLength, xSpace, ySpace, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT)))
		{
			WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneRender2D(g_Renderer, WIN_MANAGER->getLowSpec2DMPR(WT_SAGITTAL), 0, lineList, coordList, 1, alpha, gamma);

			WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneEnd(g_Renderer);
		}


		xLength = DATA_CONTEXT->volume_data.getCX();
		yLength = DATA_CONTEXT->volume_data.getCZ();
		xSpace = DATA_CONTEXT->volume_data.getSpaceX();
		ySpace = DATA_CONTEXT->volume_data.getSpaceZ();

		lineList.clear();
		coordList.clear();

		DATA_CONTEXT->volume_data.getSegmentationPlaneSurfaceList2D(WT_CORONAL, lineList, coordList);

		if (WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneBegin2D(g_Renderer, WIN_MANAGER->getVolumeMixMode() ? 2 : 0, WIN_MANAGER->get2DPresetTexture(),
			HuMin, HuMax, xLength, yLength, xSpace, ySpace, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT)))
		{
			WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneRender2D(g_Renderer, WIN_MANAGER->getLowSpec2DMPR(WT_CORONAL), 0, lineList, coordList, 1, alpha, gamma);

			WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneEnd(g_Renderer);
		}

		/*lineList.clear();
		box3D.getLineList(lineList);
		g_Renderer->renderLineList(lineList, mip::COLOR(255, 255, 255));*/

		//g_Renderer->drawFont("Volume rendering is disabled.", WIN_MANAGER->getFontTexture());
		return;
	}

	if (WIN_MANAGER->getVolumeTexture() != 0)
	{
		for (int n = 0; n < DATA_CONTEXT->volume_data.getMaskInfoListCnt(); n++)
		{
			MaskInfo* minfo = DATA_CONTEXT->volume_data.getMaskInfo(n);
			COLOR color = minfo->color;
			float per = (float)minfo->layerAlpha / 255.0;
			float layerAlpha = minfo->show ? pow(per, 3) * WIN_MANAGER->getLayerAlpha() : 0;
			if (layerAlpha > 0 && layerAlpha < 1)
				layerAlpha = 1.f;

			g_Renderer->setMaskColor(minfo->uid + 2, mip::COLOR(color.r, color.g, color.b, layerAlpha));
		}

		float slice = mip::math::Max(mip::math::Max(DATA_CONTEXT->volume_data.getCX(), DATA_CONTEXT->volume_data.getCY()), DATA_CONTEXT->volume_data.getCZ());
		slice *= 1.5f;

		for (int i = 0; i < 4; i++)
		{
			if (skip_mode)
				break;
			if (m_bSkip[i])
			{
				skip_mode = true;
				break;
			}
		}

		if (skip_mode == true) slice *= 0.3f;


		mip::AABB box3D = DATA_CONTEXT->volume_data.getBoundingBox3DAABB();
		DWORD clipValue = 0;
		int RayResult = 0;
		DWORD clipValueCnt = 0;
		std::vector<mip::VECTOR3> tempList;
		std::vector<mip::VECTOR3> lineList;
		std::vector<mip::VECTOR3> coordList;


		bool planecut = WIN_MANAGER->getWorkMode() == WORK_3D_PLANE_SPLIT;
		if (WIN_MANAGER->isClipMode() || planecut)
		{
			//	m_bVRPoint = false;
			mip::MATRIX44 matPI = planecut ? m_trPlaneCut.getMatrix() : m_trPlaneImage.getMatrix();

			bool plane2D = planecut ? false : WIN_MANAGER->getClip2DPlane();

			mip::PLANE axialP, coronalP, saggitalP;



			lineList.clear();

			tempList.clear();
			DATA_CONTEXT->volume_data.getPlaneSurface(WT_AXIAL, tempList, coordList, &matPI);

			if (!plane2D)
			{
#ifdef USE_RIGHT_HAND
				lineList.push_back(tempList[0]);
				lineList.push_back(tempList[2]);
				lineList.push_back(tempList[2]);
				lineList.push_back(tempList[1]);
				lineList.push_back(tempList[1]);
				lineList.push_back(tempList[3]);
				lineList.push_back(tempList[3]);
				lineList.push_back(tempList[0]);
#else
				lineList.push_back(tempList[0]);
				lineList.push_back(tempList[1]);
				lineList.push_back(tempList[1]);
				lineList.push_back(tempList[5]);
				lineList.push_back(tempList[5]);
				lineList.push_back(tempList[2]);
				lineList.push_back(tempList[2]);
				lineList.push_back(tempList[0]);
#endif
			}
			else
			{
				for (int n = 0; n < tempList.size(); ++n)
					lineList.push_back(tempList[n]);
			}

			axialP.fromTri(tempList[1], tempList[2], tempList[0]);
			g_Renderer->setClipPlane(clipValueCnt, (float*)&axialP);

			clipValue |= RSCLIPPLANE0 << clipValueCnt;
			clipValueCnt++;


			if (lineList.size() > 0)
			{
				if (m_RbuttonDown)
				{
					mip::VECTOR3 currntPoint1 = m_camera.getWorldPoint(0, &matWorld);
					mip::VECTOR3 currntPoint2 = m_camera.getWorldPoint(1, &matWorld);
					if (WIN_MANAGER->isClipMode())
					{
						RayResult = mip::geom::Intersect_LinePlane(axialP, currntPoint1, currntPoint2, &m_pointControl);
						if (RayResult > 0)
						{
							if (box3D.checkPoint(m_pointControl) == true)
							{
								m_bVRPoint = true;
#ifdef _DEBUG
								std::vector<mip::VECTOR3> point_line;
								point_line.push_back(mip::VECTOR3(0, 0, 0));
								point_line.push_back(m_pointControl);
								g_Renderer->renderLineList(point_line, mip::COLOR(0, 0, 255));
#endif
							}
							else
								m_bVRPoint = false;
						}
						else
							m_bVRPoint = false;
					}
					else
						m_bVRPoint = false;
				}

				if (!plane2D)
				{
					if (0)
					{
						g_Renderer->renderLineList(lineList, mip::COLOR(255, 0, 0));
					}
					else
					{
						tempList.clear();
						std::vector<mip::VECTOR2> cList;
						DATA_CONTEXT->volume_data.getCullingPlaneSurface(tempList, cList, 0.5f, &matPI);

						if (m_cullingTexture == 0)
						{
							QImage* image = RESOURCE_MANAGER->getTexture_CullingPlane();
							if (image != NULL)
							{
								m_cullingTexture = WIN_MANAGER->createTextureFromQImage(image);
							}
						}

						if (m_cullingTexture != 0)
						{
							g_Renderer->drawPlane(tempList, cList, m_cullingTexture);
						}
					}
				}
				else
				{
					int index = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
					bool res = index <= -1 ? true : false;
					WIN_MANAGER->volume_renderer.drawVolumePlane(g_Renderer, lineList, coordList, WIN_MANAGER->getVolumeMixMode() ? 2 : 0,
						WIN_MANAGER->getVolumeTexture(), WIN_MANAGER->getMaskTexture(), WIN_MANAGER->get2DPresetTexture(),
						HuMin, HuMax, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT), WIN_MANAGER->getVolumeAlpha(), WIN_MANAGER->getGamma());
				}
			}
		}

		if (1) // near far
		{
			mip::PLANE plane = m_camera.getNearPlane(&matWorld);
			g_Renderer->setClipPlane(3, (float*)&plane);

			plane = m_camera.getFarPlane(&matWorld);
			g_Renderer->setClipPlane(4, (float*)&plane);

			clipValue |= RSCLIPPLANE3 | RSCLIPPLANE4;
		}
#ifndef DEV_VER
		if (WIN_MANAGER->getShowClippingBox())
#endif
		{
			// multi mask 선택
			std::vector<muint32> vecCurrentMaskUID = DATA_CONTEXT->volume_data.getMultiSelectMaskUID();
			for (auto i = 0; i < vecCurrentMaskUID.size(); ++i)
			{
				bool isEmpty = DATA_CONTEXT->volume_data.isEmptyMaskVoxel(vecCurrentMaskUID[i], false, true);
				if (isEmpty == false)
				{
					MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(vecCurrentMaskUID[i], true);
					if (info != NULL)
					{
						BoundingBoxI& layerbox = DATA_CONTEXT->volume_data.getBoundingBox(info->uid);//DATA_CONTEXT->volume_data.boundingBoxROI[info->uid];
						mip::AABB layeraabb = DATA_CONTEXT->volume_data.getAABB(layerbox);

						lineList.clear();
						layeraabb.getLineList(lineList);

						g_Renderer->renderLineList(lineList, mip::COLOR(info->color.r, info->color.g, info->color.b));
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////
		}

		if (WIN_MANAGER->getCullingMaskVolume() == true)
		{
			g_Renderer->setRenderState(mip::RS_CLIPPLANEENABLE, clipValue);
		}

		HuMin = WIN_MANAGER->getVolumeLevel() - WIN_MANAGER->getVolumeWidth() / 2;
		HuMax = WIN_MANAGER->getVolumeLevel() + WIN_MANAGER->getVolumeWidth() / 2;

		muint32 renderpass = 0;
		if (WIN_MANAGER->getVolumeMixMode())
		{
			renderpass |= 1;
		}
#ifdef DEV_VER
		if (WIN_MANAGER->mainSegmentWidget->isHighLight())
		{
			renderpass |= 2;
		}
#endif

		float alpha = WIN_MANAGER->getVolumeAlpha();
		if (m_updateFrameSet && alpha >= 1.0f)
		{
			alpha = 0.95f;
		}

#ifndef DEV_VER
		if (alpha >= 1.0f)
		{
			alpha = 0.95f;
		}
#endif
		int index = WIN_MANAGER->getSelectedCustomPreset(CL_3D);
		bool res = index <= -1 ? true : false;
		index += SP_COUNT;

		WIN_MANAGER->volume_renderer.drawVolume(g_Renderer, renderpass, WIN_MANAGER->getVolumeTexture(), WIN_MANAGER->getMaskTexture(), WIN_MANAGER->get3DPresetTexture(),
			DATA_CONTEXT->volume_data.getSizeX(), DATA_CONTEXT->volume_data.getSizeY(), DATA_CONTEXT->volume_data.getSizeZ(),
			HuMin, HuMax, res ? WIN_MANAGER->getSelectedVolumePreset() : (index),
			alpha, slice /*m_rotateModel ? 100.0f : 320.0f*/, 1.0f, WIN_MANAGER->getGamma(), shader_quality, vr_mode, vr_view, RayResult > 0 ? &m_pointControl : NULL);
		//					3, 1.0f, slice /*m_rotateModel ? 100.0f : 320.0f*/);
	}

	g_Renderer->setRenderState(mip::RS_CLIPPLANEENABLE, false);
}

#ifdef _M_IX86
void VolumeView::initVTK()
{
	volume = nullptr;
	volumeMapper = nullptr;
	volumeProperty = nullptr;
	gradientOpacity = nullptr;
	scalarOpacity = nullptr;
	color = nullptr;
	trackballStyle = nullptr;

	if (rendererVTK == NULL)
	{
		rendererVTK = vtkSmartPointer<vtkRenderer>::New();
	}

	if (renderWindowVTK == NULL)
	{
		renderWindowVTK = vtkSmartPointer<vtkRenderWindow>::New();

		HWND hwnd = (HWND)m_pVTKWidget->winId();
		renderWindowVTK->SetParentId(hwnd);
		renderWindowVTK->SetSize(m_pVTKWidget->width(), m_pVTKWidget->height());
		renderWindowVTK->SetPosition(0, 0);
	}

	if (renderWindowInteractor == NULL)
	{
		renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	}
}

void VolumeView::renderVolumeVTKData(vtkSmartPointer<vtkImageData> imageData, vtkSmartPointer<vtkShortArray> scalars)
{
	initVTK();

	volume = vtkSmartPointer<vtkVolume>::New();
	volumeMapper = vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper>::New();
	volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	gradientOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	scalarOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	color = vtkSmartPointer<vtkColorTransferFunction>::New();

	trackballStyle = vtkSmartPointer<vtkInteractorStyleSwitch>::New();

	imageData->AllocateScalars(VTK_SHORT, 1);
	imageData->GetPointData()->SetScalars(scalars);

	volumeMapper->SetInputData(imageData);

	volumeProperty->ShadeOff();
	volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

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

	// setting color
	color->AddRGBPoint(-750.0, 0.08, 0.05, 0.03);
	//color->AddRGBPoint(-350.0, 0.39, 0.25, 0.16); // brown
	color->AddRGBPoint(-350.0, 0.30, 0.25, 0.30); //
	color->AddRGBPoint(-200.0, 0.80, 0.80, 0.80);
	color->AddRGBPoint(2750.0, 0.70, 0.70, 0.70);
	color->AddRGBPoint(3000.0, 0.35, 0.35, 0.35);

	volumeProperty->SetScalarOpacity(scalarOpacity);
	volumeProperty->SetColor(color);

	gradientOpacity->AddPoint(0.0, 0.0);
	gradientOpacity->AddPoint(2000.0, 1.0);
	volumeProperty->SetGradientOpacity(gradientOpacity);

	volumeProperty->SetDiffuse(0.1);
	volumeProperty->SetAmbient(0.9);
	volumeProperty->SetSpecular(0.2);
	volumeProperty->SetSpecularPower(10.0);

	volume->SetProperty(volumeProperty);
	volume->SetMapper(volumeMapper);

	//rendererVTK->UnRegister(nullptr);
	rendererVTK->AddViewProp(volume);
	rendererVTK->ResetCamera();
	rendererVTK->SetBackground(0, 0, 0);
	rendererVTK->ResetCameraClippingRange();
	rendererVTK->LightFollowCameraOn();
	rendererVTK->UpdateLightsGeometryToFollowCamera();
	rendererVTK->TwoSidedLightingOn();

	trackballStyle->SetCurrentStyleToTrackballCamera();

	renderWindowVTK->AddRenderer(rendererVTK);
	renderWindowInteractor->SetInteractorStyle(trackballStyle);
	renderWindowInteractor->SetRenderWindow(renderWindowVTK);

	renderWindowVTK->Render();
	renderWindowInteractor->Initialize();

	qDebug() << "VTK Render - Start" << endl;
	renderWindowInteractor->Start();
	qDebug() << "VTK Render - End" << endl;
}

void VolumeView::renderVolumeVTKDICOMReader()
{
	volume = nullptr;
	volumeMapper = nullptr;
	volumeProperty = nullptr;
	gradientOpacity = nullptr;
	scalarOpacity = nullptr;
	color = nullptr;
	trackballStyle = nullptr;

	volume = vtkSmartPointer<vtkVolume>::New();
	volumeMapper = vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper>::New();
	volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	gradientOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	scalarOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	color = vtkSmartPointer<vtkColorTransferFunction>::New();

	trackballStyle = vtkSmartPointer<vtkInteractorStyleSwitch>::New();

	//
	vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	QString s = "c:\\Qt\\test_dicom_bak";
	reader->SetDirectoryName(s.toStdString().c_str());
	reader->Update();
	imageData->ShallowCopy(reader->GetOutput());
	//
	volumeMapper->SetInputData(imageData);

	volumeProperty->ShadeOff();
	volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

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

	// setting color
	color->AddRGBPoint(-750.0, 0.08, 0.05, 0.03);
	//color->AddRGBPoint(-350.0, 0.39, 0.25, 0.16); // brown
	color->AddRGBPoint(-350.0, 0.30, 0.25, 0.30); //
	color->AddRGBPoint(-200.0, 0.80, 0.80, 0.80);
	color->AddRGBPoint(2750.0, 0.70, 0.70, 0.70);
	color->AddRGBPoint(3000.0, 0.35, 0.35, 0.35);

	volumeProperty->SetScalarOpacity(scalarOpacity);
	volumeProperty->SetColor(color);

	gradientOpacity->AddPoint(0.0, 0.0);
	gradientOpacity->AddPoint(2000.0, 1.0);
	volumeProperty->SetGradientOpacity(gradientOpacity);

	volumeProperty->SetDiffuse(0.1);
	volumeProperty->SetAmbient(0.9);
	volumeProperty->SetSpecular(0.2);
	volumeProperty->SetSpecularPower(10.0);

	volume->SetProperty(volumeProperty);
	volume->SetMapper(volumeMapper);

	//rendererVTK->UnRegister(nullptr);
	rendererVTK->AddViewProp(volume);
	rendererVTK->ResetCamera();
	rendererVTK->SetBackground(0, 0, 0);
	rendererVTK->ResetCameraClippingRange();
	rendererVTK->LightFollowCameraOn();
	rendererVTK->UpdateLightsGeometryToFollowCamera();
	rendererVTK->TwoSidedLightingOn();

	trackballStyle->SetCurrentStyleToTrackballCamera();

	renderWindowVTK->AddRenderer(rendererVTK);
	renderWindowInteractor->SetInteractorStyle(trackballStyle);
	renderWindowInteractor->SetRenderWindow(renderWindowVTK);

	renderWindowVTK->Render();
	renderWindowInteractor->Initialize();

	qDebug() << "VTK Render - Start" << endl;
	renderWindowInteractor->Start();
	qDebug() << "VTK Render - End" << endl;
}

void VolumeView::clearVTK()
{
	renderWindowVTK->Finalize();
	renderWindowInteractor->TerminateApp();

	renderWindowVTK = nullptr;
	rendererVTK = nullptr;
	renderWindowInteractor = nullptr;
}
#endif

bool VolumeView::eventFilter(QObject* target, QEvent* e)
{
	static QIcon freeHover = RESOURCE_MANAGER->getIcon(ICON_VIEW_FREEDRAWROI_HOVER, m_nIconSize, m_nIconSize);
	static QIcon freeLeave = RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize);

	static QIcon stopHover = RESOURCE_MANAGER->getIcon(ICON_STOP_HOVER, m_nIconSize, m_nIconSize);
	static QIcon stopLeave = RESOURCE_MANAGER->getIcon(ICON_NON_STOP, m_nIconSize, m_nIconSize);

	static QIcon cullingHover = RESOURCE_MANAGER->getIcon(ICON_VIEW_FREEDRAWROI_HOVER, m_nIconSize, m_nIconSize);
	static QIcon cullingLeave = RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize);

	static QIcon fullHover = RESOURCE_MANAGER->getIcon(ICON_VIEW_FULL_SCREEN_HOVER, m_nIconSize, m_nIconSize);
	static QIcon fullRelease = RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FULL_SCREEN, m_nIconSize, m_nIconSize);

	static QIcon splitHover = RESOURCE_MANAGER->getIcon(ICON_SPLIT_CURVE_HOVER, m_nIconSize, m_nIconSize);
	static QIcon splitLeave = RESOURCE_MANAGER->getIcon(ICON_NON_SPLIT_CURVE, m_nIconSize, m_nIconSize);

	static QIcon planeHover = RESOURCE_MANAGER->getIcon(ICON_PLANE_CUT_HOVER, m_nIconSize, m_nIconSize);
	static QIcon planeLeave = RESOURCE_MANAGER->getIcon(ICON_NON_PLANE_CUT, m_nIconSize, m_nIconSize);

	static QIcon angleHover = RESOURCE_MANAGER->getIcon(ICON_3D_ANGLE_SPLIT_HOVER, m_nIconSize, m_nIconSize);
	static QIcon angleLeave = RESOURCE_MANAGER->getIcon(ICON_NON_3D_ANGLE_SPLIT, m_nIconSize, m_nIconSize);


	if (target == NULL)
		return QWidget::eventFilter(target, e);

	if (!target->inherits("QPushButton"))
		return QWidget::eventFilter(target, e);


	QEvent::Type _type = e->type();

	int res = 0;

	if (_type == QEvent::HoverEnter)
		this->unsetCursor();
	else if (_type == QEvent::HoverLeave ||
		_type == QEvent::Leave || _type == QEvent::Show)
	{
		if (HoverWidget)
		{
			if (!HoverWidget->isChecked())
				HoverWidget->setIcon(LeaveIcon);
		}

		this->setCursor(m_cursor);
	}

	QWidget* w = dynamic_cast<QWidget*>(target);
	{
		QMouseEvent* evt = dynamic_cast<QMouseEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
			}
		}
	}
	{
		QHoverEvent* evt = dynamic_cast<QHoverEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
			}
		}
	}
	{
		QWheelEvent* evt = dynamic_cast<QWheelEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
			}
		}
	}


	if (target == m_btnRegionROI && !m_btnRegionROI->isChecked())
	{
		if (g_Renderer->isAvailableVolumeRender() == false) return QWidget::eventFilter(target, e);

		if (res == 1)
		{
			m_btnRegionROI->setIcon(freeHover);
			HoverWidget = m_btnRegionROI;
			LeaveIcon = freeLeave;
		}
	}
	else if (target == m_btnCurveSplit && !m_btnCurveSplit->isChecked())
	{
		if (g_Renderer->isAvailableVolumeRender() == false) return QWidget::eventFilter(target, e);

		if (res == 1)
		{
			m_btnCurveSplit->setIcon(splitHover);
			HoverWidget = m_btnCurveSplit;
			LeaveIcon = splitLeave;
		}
	}
	else if (target == m_btnPlaneSplit && !m_btnPlaneSplit->isChecked())
	{
		if (g_Renderer->isAvailableVolumeRender() == false) return QWidget::eventFilter(target, e);

		if (res == 1)
		{
			m_btnPlaneSplit->setIcon(planeHover);
			HoverWidget = m_btnPlaneSplit;
			LeaveIcon = planeLeave;
		}
	}
	else if (target == m_btnAngleSplit && !m_btnAngleSplit->isChecked())
	{
		if (g_Renderer->isAvailableVolumeRender() == false) return QWidget::eventFilter(target, e);

		if (res == 1)
		{
			m_btnAngleSplit->setIcon(angleHover);
			HoverWidget = m_btnAngleSplit;
			LeaveIcon = angleLeave;
		}
	}
	else if (target == m_btnStopDisplay)
	{
		if (res == 1)
		{
			m_btnStopDisplay->setIcon(stopHover);
			HoverWidget = m_btnStopDisplay;
			LeaveIcon = stopLeave;
		}
	}
	else if (target == m_btnFullScreen && !m_fullscreen)
	{
		if (res == 1)
		{
			m_btnFullScreen->setIcon(fullHover);
			HoverWidget = m_btnFullScreen;
			LeaveIcon = fullRelease;
		}
	}

	return QWidget::eventFilter(target, e);
}

void VolumeView::render(QPainter* p)
{
	if (p == NULL)
		return;

	// Update OpenGL Resource
	WIN_MANAGER->preRenderProcess();

	if (DATA_CONTEXT->volume_data.isValidate() == false || WIN_MANAGER->getRenderable() == false || WIN_MANAGER->IsCropOn())
	{
		QColor background = RESOURCE_MANAGER->getUnloadBackGroundColor();

		if (m_labelImage == NULL)
		{
			m_labelImage = new QLabel(this);

			QImage* unloadImage = RESOURCE_MANAGER->getUnloadImage(WT_VOLUME);

			if (unloadImage != NULL)
			{
				m_labelImage->setPixmap(QPixmap::fromImage(*unloadImage));
				m_size = QSize(unloadImage->width(), unloadImage->height());
			}
		}

		QSize size = (this->size() / 2) - (m_size / 2);
		m_labelImage->setGeometry(QRect(size.width(), size.height(), m_size.width(), m_size.height()));
		m_labelImage->show();

		p->fillRect(0, 0, width(), height(), RESOURCE_MANAGER->getUnloadBackGroundColor());
#ifdef _M_IX86
		showVTKWidget(false);
#endif

		return;
	}
	if (m_labelImage)
		m_labelImage->hide();

#ifdef _M_IX86
	if (m_pVTKWidget->isVisible() == true)
	{
		return;
	}
#endif

	if (WIN_MANAGER->getRenderable() == false)
	{
		return;
	}

#ifdef _DEBUG
	static float s_fLastTime = (float)(0.001f * timeGetTime());
	float fCurrentTime = 0.001f * timeGetTime();
	m_fElapsedTime = fCurrentTime - s_fLastTime;	// 인터벌 타임계산
	s_fLastTime = fCurrentTime;
	m_fAccumulatedTime += m_fElapsedTime;						// 누적시간 계산
#endif

	if (m_updateFrameSet == true) // Draw VR
	{
#ifndef _M_IX86 // VR
		mip::HMDcontrolState first, second;
		bool r = g_Renderer->handleInput(&first, &second);
		m_firstControlPressed = first.PressButtonA;
		m_secondControlPressed = second.PressButtonA;

		//if (second.PressDLeft)
		//{
		//	WIN_MANAGER->addVolumeLevel(-10);
		//}
		m_firstControlDpadPressed = false;
		if (first.PressDLeft)
			m_firstControlDpadPressed = true;

		if (first.PressButtonGrip)
		{
			bringVolume();
		}

		if (second.PressButtonGrip)
		{
			resetUI();
		}

		mip::MATRIX44 matInvWorld = m_trVolumModel.getMatrix();
		matInvWorld.inverse();
		mip::VECTOR3 firstControl, secondControl;
		mip::QUATERNION firstRot, secondRot;

		bool planecut = WIN_MANAGER->getWorkMode() == WORK_3D_PLANE_SPLIT;
		double offsetDPI = m_updateFrameSet ? (WIN_MANAGER->getDPIValue() / 10.f) : 1.f;
		g_Renderer->makeControllerAxes(matInvWorld, &firstControl, &secondControl, &firstRot, &secondRot);

		if (m_firstControlPressed)
		{
			if (planecut)
			{
				m_trPlaneCut.addTranslate((firstControl)*offsetDPI);
				m_trPlaneCut.addRotate(firstRot);
			}
			else
			{
				m_trPlaneImage.addTranslate((firstControl)*offsetDPI);
				m_trPlaneImage.addRotate(firstRot);
			}
		}

		if (second.PressDLeft)
		{
			if (mip::math::Abs(secondControl.x) > mip::math::Abs(secondControl.y))
			{
				if (secondControl.x >= 0)
					WIN_MANAGER->addVolumeLevel(10);
				else
					WIN_MANAGER->addVolumeLevel(-10);
			}
			else
			{
				if (secondControl.y >= 0)
					WIN_MANAGER->addVolumeWidth(10);
				else
					WIN_MANAGER->addVolumeWidth(-10);
			}
		}
		else if (m_secondControlPressed)
		{
			m_trVolumModel.addTranslate((secondControl)*offsetDPI);
			m_trVolumModel.addRotate(secondRot);
		}

		slot_OnUpdateFrame();
#endif
	}
	else // Draw OpenGL
	{
		renderScene(p);
		if (WIN_MANAGER->getSeedShow() == RT_SHOW)
			drawSeedPoints(p);

		drawAnnotations(p);
	}

	QFontMetrics fontMet(p->font());
	QString str;
	QRect rect;
#ifdef _DEBUG
	p->setPen(QColor(255, 0, 0));

	str = QString("fps : %4.1f").arg((float)m_lAccumulatedFrame / m_fAccumulatedTime);
	rect = fontMet.boundingRect(str);
	p->drawText(QPoint(100, rect.height() + 5), str);

	if (m_fAccumulatedTime > 3.0f)
	{
		m_fAccumulatedTime = 0.0f;
		m_lAccumulatedFrame = 0;
	}
	m_lAccumulatedFrame++;
#endif

	if (DATA_CONTEXT->volume_data.isValidate())
	{
		QPen prePen = p->pen();
		int stopDisplayHeight = 0;
		int textX = 0;
		if (!m_btnFullScreen->isHidden())
			textX += m_btnFullScreen->width();
		p->setPen(QPen(QColor(62, 137, 219)));

		if (!m_btnStopDisplay->isHidden())
		{
			textX = 0;
			stopDisplayHeight += m_btnFullScreen->width();
		}

		if (WIN_MANAGER->getWorkMode() == WORK_REGION_ROI ||
			WIN_MANAGER->getWorkMode() == WORK_CAPTURE)
		{
			str = QString("%1").arg(WIN_MANAGER->getWorkModeString(true));
			rect = fontMet.boundingRect(str);
			p->drawText(textX, this->height() - stopDisplayHeight, str);

			stopDisplayHeight += rect.height();

			str = getMoveMode();
			rect = fontMet.boundingRect(str);
			stopDisplayHeight += 2;
			p->drawText(textX, this->height() - stopDisplayHeight, str);
		}
		else
		{
			str = getMoveMode();
			rect = fontMet.boundingRect(str);
			stopDisplayHeight += rect.height();
			p->drawText(textX, this->height() - stopDisplayHeight, str);
		}

		p->setPen(prePen);
	}

	drawMouseWork(p);

	if (g_Renderer->isAvailableVolumeRender() == false)
	{
		p->setPen(Qt::red);
		QFontMetrics fontMet(p->font());

		//QString str = QString("Volume Rendering requires");
		//QRect rect = fontMet.boundingRect(str);

		//p->drawText(QPoint((this->width() / 2) - (rect.width() / 2), 10 + rect.height()), str);


		//str = QString("GPU Graphic card.");
		QString strMsg = STRING_MANAGER->getString(ERR_DH_0007);
		QRect rect2 = fontMet.boundingRect(strMsg);

		p->drawText(QPoint((this->width() / 2) - (rect.width() / 2), 10 + rect.height() + rect2.height()), str);
	}

	if (!WIN_MANAGER->IsLicensePass())
	{
		QImage* WaterImage = RESOURCE_MANAGER->getWaterMark();
		if (WaterImage != NULL)
		{
			p->drawImage(QPoint(this->width() / 2 - WaterImage->size().width() / 2, (this->height() / 2) + 100), *WaterImage);
		}
	}
}

void VolumeView::initInScreenRightMenu()
{
	if (m_btnRegionROI == nullptr)
	{
		//#if !defined(COVID19_VER)
		m_btnRegionROI = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize), "", this);
		m_btnRegionROI->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnRegionROI->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnRegionROI->setCheckable(true);
		m_btnRegionROI->setMouseTracking(true);
		m_btnRegionROI->installEventFilter(this);
		m_btnRegionROI->hide();
		m_btnRegionROI->setStyleSheet("color: black;");
		m_btnRegionROI->setToolTip("Region Selection\n(Only use for Right-click : deletion)");
		connect(m_btnRegionROI, &QPushButton::clicked, this, &VolumeView::slot_OnFreeDrawROI);

		//m_btnSplit = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SPLIT_CURVE), "", this);
		m_btnCurveSplit = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Manualdrawingtools_3DViewManualsplitfunction, this);
		if (m_btnCurveSplit)
		{
			m_btnCurveSplit->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_SPLIT_CURVE, m_nIconSize, m_nIconSize));
			m_btnCurveSplit->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnCurveSplit->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnCurveSplit->setCheckable(true);
			m_btnCurveSplit->setMouseTracking(true);
			m_btnCurveSplit->installEventFilter(this);
			m_btnCurveSplit->hide();
			m_btnCurveSplit->setStyleSheet("color: black;");
			m_btnCurveSplit->setToolTip("Curve Split");
			connect(m_btnCurveSplit, &QPushButton::clicked, this, &VolumeView::slot_OnCurveSplit);
		}

		//m_btnPlanecut = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_PLANE_CUT), "", this);
		m_btnPlaneSplit = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Manualdrawingtools_3DViewManualsplitfunction, this);
		if (m_btnPlaneSplit)
		{
			m_btnPlaneSplit->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_PLANE_CUT, m_nIconSize, m_nIconSize));
			m_btnPlaneSplit->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnPlaneSplit->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnPlaneSplit->setCheckable(true);
			m_btnPlaneSplit->setMouseTracking(true);
			m_btnPlaneSplit->installEventFilter(this);
			m_btnPlaneSplit->hide();
			m_btnPlaneSplit->setStyleSheet("color: black;");
			m_btnPlaneSplit->setToolTip("Plane Split\n(Apply : Right-click)");
			connect(m_btnPlaneSplit, &QPushButton::clicked, this, &VolumeView::slot_OnPlaneSplit);
		}

		m_btnAngleSplit = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Manualdrawingtools_3DViewManualsplitfunction, this);
		if (m_btnAngleSplit)
		{
			m_btnAngleSplit->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_3D_ANGLE_SPLIT, m_nIconSize, m_nIconSize));
			m_btnAngleSplit->setIconSize(QSize(m_nIconSize, m_nIconSize));
			m_btnAngleSplit->setFixedSize(QSize(m_nIconSize, m_nIconSize));
			m_btnAngleSplit->setCheckable(true);
			m_btnAngleSplit->setMouseTracking(true);
			m_btnAngleSplit->installEventFilter(this);
			m_btnAngleSplit->hide();
			m_btnAngleSplit->setStyleSheet("color: black;");
			m_btnAngleSplit->setToolTip("Angle Split\n(Apply : Right-click)");
			connect(m_btnAngleSplit, &QPushButton::clicked, this, &VolumeView::slot_OnAngleSplit);
		}

		//#endif
		m_btnStopDisplay = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_STOP, m_nIconSize, m_nIconSize), "", this);
		m_btnStopDisplay->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnStopDisplay->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnStopDisplay->setMouseTracking(true);
		m_btnStopDisplay->installEventFilter(this);
		m_btnStopDisplay->hide();
		m_btnStopDisplay->setStyleSheet("color: black;");
		m_btnStopDisplay->setToolTip("Stop rotation for display");
		connect(m_btnStopDisplay, &QPushButton::clicked, this, &VolumeView::slot_OnStopDisplay);

		m_btnFullScreen = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FULL_SCREEN, m_nIconSize, m_nIconSize), "", this);
		m_btnFullScreen->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnFullScreen->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnFullScreen->setCheckable(true);
		m_btnFullScreen->setMouseTracking(true);
		m_btnFullScreen->installEventFilter(this);
		m_btnFullScreen->hide();
		m_btnFullScreen->setStyleSheet("color: black;");
		m_btnFullScreen->setToolTip("Full screen");
		connect(m_btnFullScreen, &QPushButton::clicked, this, &VolumeView::slot_OnFullScreen);

	}
}

void VolumeView::updateScreenLeftMenu(int width, int height)
{
	int _offset = 0;

	if (m_btnRegionROI)
	{
		m_btnRegionROI->move(width - m_btnRegionROI->width(), _offset);
		_offset += m_btnRegionROI->height();
	}

	if (m_btnCurveSplit)
	{
		m_btnCurveSplit->move(width - m_btnCurveSplit->width(), _offset);
		_offset += m_btnCurveSplit->height();
	}

	if (m_btnPlaneSplit)
	{
		m_btnPlaneSplit->move(width - m_btnPlaneSplit->width(), _offset);
		_offset += m_btnPlaneSplit->height();
	}

	if (m_btnAngleSplit)
	{
		m_btnAngleSplit->move(width - m_btnAngleSplit->width(), _offset);
		_offset += m_btnAngleSplit->height();
	}

	if (m_btnFullScreen)
		m_btnFullScreen->move(0, height - m_btnFullScreen->height());

	if (m_btnStopDisplay)
		m_btnStopDisplay->move(m_btnFullScreen->width(), height - m_btnStopDisplay->height());

}

void VolumeView::slot_OnVolume()
{
	static bool visible = true;
	static QIcon actIcon = RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE, m_nIconSize, m_nIconSize);
	static QIcon inactIcon = RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize);
	int nType = 0;
	visible = !visible;
	WIN_MANAGER->setVolumeAlpha(!WIN_MANAGER->getVolumeAlpha());
	m_btnVolume->setIcon(visible ? actIcon : inactIcon);

	/*code*/
	if (WIN_MANAGER->getVolumeAlpha())
		nType |= RT_VOLUME;
	if (WIN_MANAGER->getLayerAlpha())
		nType |= RT_MASK;
	//if (m_VolumeMixMode)
	//	nType |= RT_SURFACE;

	WIN_MANAGER->setRenderType(nType);
	WIN_MANAGER->renderLater_GridView(true);
}

void VolumeView::slot_OnLayer()
{
	static bool visible = true;
	static QIcon actIcon = RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE, m_nIconSize, m_nIconSize);
	static QIcon inactIcon = RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize);
	int nType = 0;
	visible = !visible;
	WIN_MANAGER->setLayerAlpha(!WIN_MANAGER->getLayerAlpha());

	m_btnLayer->setIcon(visible ? actIcon : inactIcon);

	/*code*/
	if (WIN_MANAGER->getVolumeAlpha())
		nType |= RT_VOLUME;
	if (WIN_MANAGER->getLayerAlpha())
		nType |= RT_MASK;
	//if (m_VolumeMixMode)
	//	nType |= RT_SURFACE;

	WIN_MANAGER->setRenderType(nType);
	WIN_MANAGER->renderLater_GridView(true);
}

void VolumeView::slot_OnFreeDrawROI()
{
	if ((!DATA_CONTEXT->volume_data.isValidate()) || g_Renderer->isAvailableVolumeRender() == false)
	{
		m_btnRegionROI->setChecked(!m_btnRegionROI->isChecked());
		if (m_btnRegionROI->isChecked())
		{
			WIN_MANAGER->setSegmentationWorkMode(WORK_NONE);
		}
		else
		{
			WIN_MANAGER->setSegmentationWorkMode(WORK_REGION_ROI);
		}
		return;
	}

	WIN_MANAGER->setSegmentationWorkMode(WORK_REGION_ROI);
}

void VolumeView::slot_OnFullScreen()
{
	if (!DATA_CONTEXT->volume_data.isValidate()) return;
	if (m_bWorkMode == true) return;

	m_fullscreen = !m_fullscreen;

	static QIcon prIcon = RESOURCE_MANAGER->getIcon(ICON_VIEW_FULL_SCREEN, m_nIconSize, m_nIconSize);
	static QIcon reIcon = RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FULL_SCREEN, m_nIconSize, m_nIconSize);

	if (m_fullscreen)
		m_btnFullScreen->setIcon(prIcon);
	else
		m_btnFullScreen->setIcon(reIcon);

	emit setFullScreen(m_fullscreen == true ? this : NULL);
}

void VolumeView::slot_OnDisplay()
{
	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		m_bLeftRotate = false;
		m_bRightRotate = false;
		WIN_MANAGER->displayTimer->stop();
		//TODO - STOP BUTTON HIDE
		if (m_btnStopDisplay)
			m_btnStopDisplay->hide();
		return;
	}

	if (!(m_bRightRotate || m_bLeftRotate))
	{
		WIN_MANAGER->displayTimer->stop();
		if (m_btnStopDisplay)
			m_btnStopDisplay->hide();
		return;
	}

	if (m_bRightRotate)
	{
		revolveCamera(true);
	}
	else if (m_bLeftRotate)
	{
		revolveCamera(false);
	}

	renderLater();
}

void VolumeView::slot_OnWheelTimeout()
{
	useSkipRender(2, false);

	renderLater();
}

void VolumeView::slot_OnStopDisplay()
{
	m_bLeftRotate = false;
	m_bRightRotate = false;

	if (WIN_MANAGER->displayTimer)
		WIN_MANAGER->displayTimer->stop();

	if (m_btnStopDisplay)
		m_btnStopDisplay->hide();
}

void VolumeView::slot_OnRightDisplay()
{
	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	m_bLeftRotate = false;
	m_bRightRotate = true;

	if (WIN_MANAGER->displayTimer)
	{
		if (!WIN_MANAGER->displayTimer->isActive())
		{
			WIN_MANAGER->displayTimer->start(50);
			m_btnStopDisplay->show();
		}
	}
}

void VolumeView::slot_OnLeftDisplay()
{
	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	m_bRightRotate = false;
	m_bLeftRotate = true;

	if (WIN_MANAGER->displayTimer)
	{
		if (!WIN_MANAGER->displayTimer->isActive())
		{
			WIN_MANAGER->displayTimer->start(50);
			m_btnStopDisplay->show();
		}
	}
}

void VolumeView::slot_OnPlaneSplit(bool checked)
{
	if ((!DATA_CONTEXT->volume_data.isValidate()) || g_Renderer->isAvailableVolumeRender() == false)
	{
		if (m_btnPlaneSplit)
			m_btnPlaneSplit->setChecked(!checked);
		return;
	}

	if (m_btnPlaneSplit->isChecked())
	{
		setWorkMode(WORK_3D_PLANE_SPLIT, true);
		WIN_MANAGER->clipOnOff(false);
		WIN_MANAGER->setClipOpt();
	}
	else
		setWorkMode(WORK_NONE, true);
}

void VolumeView::slot_OnCurveSplit()
{
	if ((!DATA_CONTEXT->volume_data.isValidate()) || g_Renderer->isAvailableVolumeRender() == false)
	{
		if (m_btnCurveSplit)
			m_btnCurveSplit->setChecked(!m_btnCurveSplit->isChecked());
		return;
	}

	if (m_btnCurveSplit->isChecked())
		setWorkMode(WORK_3D_CURVE_SPLIT, true);
	else
		setWorkMode(WORK_NONE, true);
}

void VolumeView::slot_OnAngleSplit()
{
	if ((!DATA_CONTEXT->volume_data.isValidate()) || g_Renderer->isAvailableVolumeRender() == false)
	{
		if (m_btnAngleSplit)
			m_btnAngleSplit->setChecked(!m_btnAngleSplit->isChecked());
		return;
	}

	if (m_btnAngleSplit->isChecked())
		setWorkMode(WORK_3D_ANGLE_SPLIT, true);
	else
	{
		setWorkMode(WORK_NONE, true);

		for (auto it = WIN_MANAGER->anotationList.begin(); it != WIN_MANAGER->anotationList.end(); ++it)
		{
			(*it)->clearSelect();
		}
	}
}

QString VolumeView::getMoveMode()
{
	QString str;
	bool chk = m_btnPlaneSplit ? m_btnPlaneSplit->isChecked() : false;

	if (WIN_MANAGER->isClipMode() || chk)
	{
		if (m_ctrl)
			str.append("(Ctrl) Object Move mode");
		else
			str.append("(Non-ctrl) Plane Move mode");
	}
	else
		str = "Object Move mode";

	return str;
}

#ifdef _M_IX86
void VolumeView::showVTKWidget(bool show)
{
	if (m_pVTKWidget)
	{
		m_pVTKWidget->setVisible(show);
		m_labelImage->setVisible(!show);
	}
}
#endif

void VolumeView::slot_OnTo2DPoint()
{
	mip::VECTOR3 vec = getLocalToVolume(m_pointControl.x, m_pointControl.y, m_pointControl.z);

	WindowBase* winA, * winC, * winS;

	winA = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);
	winC = WIN_MANAGER->mainSegmentWidget->getWindow(WT_CORONAL);
	winS = WIN_MANAGER->mainSegmentWidget->getWindow(WT_SAGITTAL);

	winA->setDepth(vec.z);
	winC->setDepth(vec.y);
	winS->setDepth(vec.x);

	WIN_MANAGER->renderLater_GridView(false);
}

void VolumeView::slot_OnChecked()
{
	renderLater();
}

void VolumeView::TransformRotate(mip::TRANSFORM& tr, const mip::SCAMERA& camera, mip::MATRIX44* trasform)
{
	mip::VECTOR3 v1 = camera.getWorldPoint(0.5f, trasform);
	mip::VECTOR3 v2 = camera.getPreWorldPoint(0.5f, trasform);

	mip::VECTOR3 rV = (v1 - v2);
	int cx, cy, px, py;
	camera.getScreenSize(cx, cy);
	camera.getScreenXY(px, py);

	float dy = rV.y / (float)(cy);
	float dx = rV.x / (float)(cx);

	mip::MATRIX44 matInvCamera;
	if (m_updateFrameSet == true)
	{
		matInvCamera = getHMDViewMatrix(0, trasform);
		matInvCamera = trasform ? (*trasform) * matInvCamera : matInvCamera;
	}
	else
	{
		matInvCamera = trasform ? (*trasform) * camera.getView() : camera.getView();
	}

	matInvCamera.inverse();

	mip::QUATERNION	q;
#ifdef USE_RIGHT_HAND
	mip::VECTOR3 Z = -matInvCamera.getScaledZaxis().normalize();
	if (trasform == NULL)
	{
		dy = -dy;
	}
#else
	mip::VECTOR3 Z = matInvCamera.getScaledZaxis().normalize();
#endif
	if (px < (cx * 0.9f) && px >(cx * 0.1f))
	{
		mip::VECTOR3 rotV = rV;
		rotV.normalize();

		rotV = (rotV ^ Z).normalize();

		q.setRotationAxis(rotV, rV.length() * 0.1f);

		tr.addRotate(q);
	}
	else if (px > (cx * 0.9f))
	{
		q.setRotationAxis(Z, dy * 60.0f);
		tr.addRotate(q);
	}
	else if (px < (cx * 0.1f))
	{
		q.setRotationAxis(Z, -dy * 60.0f);
		tr.addRotate(q);
	}
}

void VolumeView::TransformRotate(mip::TRANSFORM& tr, int type)
{
	mip::SCAMERA camera = m_camera;

	int cx, cy, px, py;
	double offsetDPI = m_updateFrameSet ? (WIN_MANAGER->getDPIValue()) * 2.f : 15.f;

	camera.getScreenSize(cx, cy);

	px = cx / 2;
	py = cy / 2;

	switch (type)
	{
	case Qt::Key_Right:
	case Qt::Key_Left:
		px += type == Qt::Key_Right ? offsetDPI : -offsetDPI;
		break;
	case Qt::Key_Up:
	case Qt::Key_Down:
		py += type == Qt::Key_Up ? offsetDPI : -offsetDPI;
		break;
	default:
		break;
	}

	mip::VECTOR3 v1 = camera.getWorldPoint(px, py, 0.5f);
	mip::VECTOR3 v2 = camera.getWorldPoint(cx / 2, cy / 2, 0.5f);

	mip::VECTOR3 rV = (v1 - v2);

	float dy = rV.y / (float)(cy);
	float dx = rV.x / (float)(cx);

	mip::MATRIX44 matInvCamera;
	if (m_updateFrameSet == true)
		matInvCamera = getHMDViewMatrix(0);
	else
		matInvCamera = camera.getView();

	matInvCamera.inverse();

	mip::QUATERNION	q;
#ifdef USE_RIGHT_HAND
	mip::VECTOR3 Z = -matInvCamera.getScaledZaxis().normalize();
	dy = -dy;
#else
	mip::VECTOR3 Z = matInvCamera.getScaledZaxis().normalize();
#endif
	if (px < (cx * 0.9f) && px >(cx * 0.1f))
	{
		mip::VECTOR3 rotV = rV;
		rotV.normalize();

		rotV = (rotV ^ Z).normalize();

		q.setRotationAxis(rotV, rV.length() * 0.1f);

		tr.addRotate(q);
	}
	else if (px > (cx * 0.9f))
	{
		q.setRotationAxis(Z, dy * 60.0f);
		tr.addRotate(q);
	}
	else if (px < (cx * 0.1f))
	{
		q.setRotationAxis(Z, -dy * 60.0f);
		tr.addRotate(q);
	}
}

void VolumeView::slot_OnVRMode()
{
	WIN_MANAGER->OnVRMode();
	WIN_MANAGER->matMoveCameraPos.set(m_pointControl.x, m_pointControl.y, m_pointControl.z);
}

void VolumeView::drawMouseWork(QPainter* p)
{
	bool focusWin = WIN_MANAGER->getLatestActiveViewType() == m_windowType;
	switch (WIN_MANAGER->getWorkMode())
	{
	case WORK_ANNOTATION_PATH:
	case WORK_ANNOTATION_PATH_PLAY:
	{
		int _group = WIN_MANAGER->getAnimationGroup();
		if (WIN_MANAGER->aniCount.at(_group) <= 0) return;
		if (m_updateFrameSet) return;
		bool bInArea = false;

		VOLUME_DATA& vd = DATA_CONTEXT->volume_data;
		mip::MATRIX44 matView = m_camera.getView();
		mip::MATRIX44 matProj = m_camera.getProj();
		mip::MATRIX44 matWorld = getWorld().getMatrix();
		float cx, cy, cz;
		AnimationTab* tab = WIN_MANAGER->GetTab()->getAniTab();
		bool bSpiral = tab ? tab->isSpiral() : false;
		QVector<QVector3D>* Lines = bSpiral ? &(WIN_MANAGER->spiralLine) : &(WIN_MANAGER->aniLine);
		QPolygonF polyLines;
		p->setPen(QPen(Qt::darkRed, 2));
		for (int i = 0; i < Lines->count(); i++)
		{
			bInArea = false;

			QVector3D vec = Lines->at(i);

			cx = vd.getSpaceX() * vec.x() - vd.getSizeX() * 0.5f;
			cy = vd.getSpaceY() * vec.y() - vd.getSizeY() * 0.5f;
			cz = vd.getSpaceZ() * vec.z() - vd.getSizeZ() * 0.5f;

			vec = QVector3D(cx, cy, cz);

			mip::VECTOR3 _v1 = mip::VECTOR3(vec.x(), vec.y(), vec.z());
			mip::VECTOR3 v1 = mip::geom::WorldToScreen(_v1, this->width(), this->height(), matView, matProj, &matWorld);
			polyLines.push_back(QPointF(v1.x, v1.y));

		}
		p->drawPolyline(polyLines);
		p->setPen(QPen(Qt::red, 3));
		int nStart, nEnd;
		nEnd = nStart = WIN_MANAGER->getAniStartPoint(_group);
		nEnd += WIN_MANAGER->aniCount.at(_group);
		Lines = &(WIN_MANAGER->aniLine);
		for (int i = nStart; i < nEnd; i++)
		{
			bInArea = false;

			QVector3D vec = Lines->at(i);

			cx = vd.getSpaceX() * vec.x() - vd.getSizeX() * 0.5f;
			cy = vd.getSpaceY() * vec.y() - vd.getSizeY() * 0.5f;
			cz = vd.getSpaceZ() * vec.z() - vd.getSizeZ() * 0.5f;

			vec = QVector3D(cx, cy, cz);

			mip::VECTOR3 _v1 = mip::VECTOR3(vec.x(), vec.y(), vec.z());
			mip::VECTOR3 v1 = mip::geom::WorldToScreen(_v1, this->width(), this->height(), matView, matProj, &matWorld);
			p->drawEllipse(QPointF(v1.x, v1.y), 2, 2);
		}
		if (WIN_MANAGER->getWorkMode() == WORK_ANNOTATION_PATH_PLAY)
		{
			p->setPen(QPen(Qt::green, 3));
			QVector3D pos = WIN_MANAGER->LinePos;
			cx = vd.getSpaceX() * pos.x() - vd.getSizeX() * 0.5f;
			cy = vd.getSpaceY() * pos.y() - vd.getSizeY() * 0.5f;
			cz = vd.getSpaceZ() * pos.z() - vd.getSizeZ() * 0.5f;

			pos = QVector3D(cx, cy, cz);
			mip::VECTOR3 _v1 = mip::VECTOR3(pos.x(), pos.y(), pos.z());
			mip::VECTOR3 v1 = mip::geom::WorldToScreen(_v1, this->width(), this->height(), matView, matProj, &matWorld);

			p->drawEllipse(QPointF(v1.x, v1.y), 2, 2);
		}
	}
	break;
	case WORK_CAPTURE:
	{
		if (!focusWin) break;

		bool mode = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureMode();

		if (mode)
		{
			QPoint p1, p2;

			p1 = QPoint(0, 0);
			p2 = QPoint(this->width() - 1, 0);

			p->setPen(Qt::red);
			p->drawLine(p1, p2);

			p2 = QPoint(0, this->height() - 1);
			p->drawLine(p1, p2);

			p1 = QPoint(this->width() - 1, this->height() - 1);
			p->drawLine(p1, p2);

			p2 = QPoint(this->width() - 1, 0);
			p->drawLine(p1, p2);
		}

		if (m_polyLine.count() >= 2)
		{
			p->setPen(QPen(Qt::red, 1.5));

			QRect rect;

			rect.setCoords(m_polyLine.at(0).x(), m_polyLine.at(0).y(),
				m_polyLine.at(1).x(), m_polyLine.at(1).y());

			p->drawRect(rect);
		}
	}
	break;
	case WORK_REGION_ROI:
	case WORK_3D_CURVE_SPLIT:
		if (m_polyLine.size() > 1)
		{
			QColor col = WIN_MANAGER->getSelectedMaskColor();
			p->setPen(QPen(col, 2.3));
			p->drawPolyline(m_polyLine);
			/*for (auto pl = m_polyLine.begin(); pl != m_polyLine.end(); ++pl)
			{
			p->drawPoint(*pl);
			}*/
		}
		break;
	default:
		break;
	}
}

void VolumeView::drawSeedPoints(QPainter* p)
{
	if (NULL == p)return;
	mip::MATRIX44 matView = m_camera.getView();
	mip::MATRIX44 matProj = m_camera.getProj();
	mip::MATRIX44 matWorld = getWorld().getMatrix();
	QPen pen = p->pen();
	p->setPen(QPen(QColor(255, 0, 0), 2));

	for (int i = 0; i < WIN_MANAGER->seedLocation.size(); i++)
	{
		QVector3D vec = WIN_MANAGER->seedLocation.at(i);

		float cx, cy, cz;
		VOLUME_DATA& vd = DATA_CONTEXT->volume_data;

		cx = cy = cz = 0;

		cx = vd.getSpaceX() * vec.x() - vd.getSizeX() * 0.5f;
		cy = vd.getSpaceY() * vec.y() - vd.getSizeY() * 0.5f;
		cz = vd.getSpaceZ() * vec.z() - vd.getSizeZ() * 0.5f;

		vec = QVector3D(cx, cy, cz);

		mip::VECTOR3 _v1 = mip::VECTOR3(vec.x(), vec.y(), vec.z());
		mip::VECTOR3 v1 = mip::geom::WorldToScreen(_v1, this->width(), this->height(), matView, matProj, &matWorld);

		p->drawEllipse(QPointF(v1.x, v1.y), 2, 2);
	}

	if (WIN_MANAGER->getRGDrawLock())
	{
		for (int i = 0; i < WIN_MANAGER->layerRGLocation.size(); i++)
		{
			QVector3D vec = WIN_MANAGER->layerRGLocation.at(i);

			float cx, cy, cz;
			VOLUME_DATA& vd = DATA_CONTEXT->volume_data;

			cx = cy = cz = 0;

			cx = vd.getSpaceX() * vec.x() - vd.getSizeX() * 0.5f;
			cy = vd.getSpaceY() * vec.y() - vd.getSizeY() * 0.5f;
			cz = vd.getSpaceZ() * vec.z() - vd.getSizeZ() * 0.5f;

			vec = QVector3D(cx, cy, cz);

			mip::VECTOR3 _v1 = mip::VECTOR3(vec.x(), vec.y(), vec.z());
			mip::VECTOR3 v1 = mip::geom::WorldToScreen(_v1, this->width(), this->height(), matView, matProj, &matWorld);

			p->drawEllipse(QPointF(v1.x, v1.y), 2, 2);
		}
		WIN_MANAGER->setRGDrawUnlock();
	}

	p->setPen(pen);
}

void VolumeView::drawAnnotations(QPainter* p)
{
	bool pointDraw = true;

	if (WIN_MANAGER->vecAngleSplitPoint.size() != 0 && pointDraw)
	{
		mip::MATRIX44 matView = getCamera().getView();
		mip::MATRIX44 matProj = getCamera().getProj();
		mip::MATRIX44 matWorld = getWorld().getMatrix();

		QBrush preBrush = p->brush();
		QPen prePen = p->pen();
		for (int i = 0; i < WIN_MANAGER->vecAngleSplitPoint.size(); i++)
		{
			mip::VECTOR3 v1 = mip::geom::WorldToScreen(WIN_MANAGER->vecAngleSplitPoint[i], this->width(), this->height(), matView, matProj, &matWorld);
			p->setPen(QColor(255, 255, 255, 255));
			p->setBrush(QColor(255, 255, 255, 255));
			p->drawEllipse(QPoint(v1.x, v1.y), 2, 2);
		}
		p->setPen(prePen);
		p->setBrush(preBrush);
	}

	if (WIN_MANAGER->get3DAnnoState())
	{
		for (auto ano = WIN_MANAGER->anotationList.begin(); ano != WIN_MANAGER->anotationList.end(); ++ano)
		{
			if ((*ano)->isAnnoHidden()) continue;

			if ((*ano)->getType() == AT_LEN)
			{
				if (!WIN_MANAGER->bShowAnnoLength)
					continue;

				AnnoLength* annoLen = static_cast<AnnoLength*>(*ano);
				annoLen->drawAnno(this, p, ADV_VOLUME_VIEWER);
			}
			else if ((*ano)->getType() == AT_TEXT)
			{
				if (!WIN_MANAGER->bShowAnnoText)
					continue;

				AnnoString* annoStr = static_cast<AnnoString*>(*ano);
				annoStr->drawAnno(this, p, ADV_VOLUME_VIEWER);
			}
			else if ((*ano)->getType() == AT_ANGLE)
			{
				if (!WIN_MANAGER->bShowAnnoAngle)
					continue;

				AnnoAngle* annoAngle = static_cast<AnnoAngle*>(*ano);
				annoAngle->drawAnno(this, p, ADV_VOLUME_VIEWER);
			}
			else if ((*ano)->getType() == AT_ARROW)
			{
				if (!WIN_MANAGER->bShowAnnoArrow)
					continue;

				AnnoArrow* annoArrow = static_cast<AnnoArrow*>(*ano);
				annoArrow->drawAnno(this, p, ADV_VOLUME_VIEWER);
			}
			else if ((*ano)->getType() == AT_NAME) // 210817 허 건 과장 추가
			{
				if (!WIN_MANAGER->bShowAnnoName)
					continue;

				AnnoName* annoName = static_cast<AnnoName*>(*ano);
				annoName->drawAnno(this, p, ADV_VOLUME_VIEWER);
			}
		}
	}

	/*if (WIN_MANAGER->bShowAnnoText)
	{
		for (auto ano = WIN_MANAGER->anotationList.begin(); ano != WIN_MANAGER->anotationList.end(); ano++)
		{
			AnnoString * anno = static_cast<AnnoString *>(*ano);
			mip::VECTOR3 pos(anno->getX(), anno->getY(), anno->getZ());
			mip::VECTOR3 realPos = mip::geom::WorldToScreen(pos, this->width(), this->height(), matView, matProj);
			p->setPen(QPen(QColor(anno->color.r, anno->color.g, anno->color.b)));
			p->drawText(QPoint(realPos.x, realPos.y), anno->str);
		}
	}*/
}

bool VolumeView::pickingMask(const mip::VECTOR2& point, /*out*/ mip::VECTOR3& result, std::vector<std::pair<int, mask8>>& vecHitMaskBit)
{
	mip::MATRIX44 matWorld = m_trVolumModel.getMatrix();

	mip::RAY ray = mip::geom::ScreenToRay(point, this->size().width(), this->size().height(),
		m_camera.getView(), m_camera.getProj(), &matWorld);

	mip::VECTOR3 vHitPoint;
	bool bPickingMask = false;

	std::vector<mask8> vecMaskBitAll, vecMaskShowFlag;
	int nMaskCnt = DATA_CONTEXT->volume_data.getMaskIndexCnt();
	for (int i = 0; i < nMaskCnt; ++i)
	{
		vecMaskShowFlag.push_back(0);
		mask8 maskBit = 0x01;
		for (int j = 0; j < 8; ++j)
		{
			MaskInfo* pMaskInfo = DATA_CONTEXT->volume_data.findMaskInfo(i, maskBit);
			if (pMaskInfo)
			{
				if (pMaskInfo->show)
				{
					vecMaskShowFlag[i] |= maskBit;
				}
			}
			maskBit = maskBit << 1;
		}
	}

	bPickingMask = WIN_MANAGER->volume_renderer.ray_MaskCheck(ray, DATA_CONTEXT->volume_data.getAllMaskDataPoint(), vecMaskShowFlag
		, vecMaskBitAll, &vHitPoint);

	if (bPickingMask)
	{
		result = vHitPoint;
		//vecHitMaskBit = vecMaskBitAll;
		// mask bit 별로 쪼갠다.
		for (int i = 0; i < vecMaskShowFlag.size(); ++i)
		{
			unsigned char maskBit = 0x01;
			for (int j = 0; j < 8; ++j)	// bit 수
			{
				if (vecMaskBitAll[i] & maskBit)
					vecHitMaskBit.push_back(pair<int, mask8>(i, maskBit));

				maskBit = maskBit << 1;
			}
		}


		qDebug() << QString("ray (x:%1, y:%2, z:%3)").arg(ray.org.x).arg(ray.org.y).arg(ray.org.z);
		qDebug() << QString("hit point (x:%1, y:%2, z:%3)").arg(result.x).arg(result.y).arg(result.z);

		return true;
	}
	else
	{
		qDebug() << QString("ray (x:%1, y:%2, z:%3)").arg(ray.org.x).arg(ray.org.y).arg(ray.org.z);
		qDebug() << "No hit";
	}

	return false;
}

mip::MATRIX44 VolumeView::getHMDViewMatrix(muint32 ueye, mip::MATRIX44* matWorld)
{
	//mip::MATRIX44 matMVP;
	mip::MATRIX44 matView;

	mip::MATRIX44 matTr;
	mip::VECTOR3 pos = -WIN_MANAGER->matMoveCameraPos;

	mip::MATRIX44 rot;
	if (matWorld)
	{
		pos.transform(*matWorld);
	}

	if (g_Renderer->isValidateHMD())
	{
		matTr.translation(pos);
		if (ueye == 0)
		{
			matView = matTr * g_Renderer->m_mat4HMDPose * g_Renderer->m_mat4eyePosLeft;
		}
		else
		{
			matView = matTr * g_Renderer->m_mat4HMDPose * g_Renderer->m_mat4eyePosRight;
		}
	}
	else
	{
		matTr.translation(pos);
		//matMVP = matWorld * matView * matProj;
		matView = m_camera.getView();
		matView._41 = 0;
		matView._42 = 0;
		matView._43 = 0;
		matView = matTr * matView;
	}

	return matView;
}

mip::MATRIX44 VolumeView::getHMDProjMatrix(muint32 ueye)
{
	mip::MATRIX44 matR;

	if (g_Renderer->isValidateHMD())
	{
		if (ueye == 0)
		{
			matR = g_Renderer->m_mat4ProjectionLeft;
		}
		else
		{
			matR = g_Renderer->m_mat4ProjectionRight;
		}
	}
	else
	{
		float size = DATA_CONTEXT->volume_data.getSizeX() * 0.5f;
		if (size < DATA_CONTEXT->volume_data.getSizeY() * 0.5f) size = DATA_CONTEXT->volume_data.getSizeY() * 0.5f;
		if (size < DATA_CONTEXT->volume_data.getSizeZ() * 0.5f) size = DATA_CONTEXT->volume_data.getSizeZ() * 0.5f;

		QSize screen = this->size();
		float aspect = screen.width() / float(screen.height());
		double offsetDPI = m_updateFrameSet ? (WIN_MANAGER->getDPIValue()) : 1.f;

		matR = mip::math::MatrixPerspectiveFovRH(MIP_PI / 4.0f, aspect, 0.1f, offsetDPI);
	}

	return matR;
}

void VolumeView::revolveCamera(bool right)
{
	mip::MATRIX44 invCamera = m_camera.getView();
	invCamera.inverse();
	const float da = RADIAN(1);

	m_fCameraXZAngle += right ? da : -da;

	if (m_fCameraXZAngle > RADIAN(360))
		m_fCameraXZAngle = RADIAN(-360);
	else if (m_fCameraXZAngle < RADIAN(-360))
		m_fCameraXZAngle = RADIAN(360);

	mip::VECTOR3 nextPos;
	float distance = mip::VECTOR3(invCamera._41, 0.0f, invCamera._43).length();
	nextPos.x = -sinf(m_fCameraXZAngle) * distance;
	nextPos.y = invCamera._42;
	nextPos.z = cosf(m_fCameraXZAngle) * distance;
	m_camera.setPos(nextPos);
}

QImage VolumeView::getCaptureImage(bool mode, bool type)
{
	WIN_MANAGER->makeCurrent();
	void* rData = g_Renderer->getBackBuffer(0, 0, this->width(), this->height());
	WIN_MANAGER->doneCurrent();

	if (rData != NULL)
	{
		QImage image((uchar*)rData, this->width(), this->height(), QImage::Format_RGB32);
		QImage img = QImage(this->width(), this->height(), QImage::Format_RGBA8888);

		QPainter p(&img);
		p.fillRect(QRect(0, 0, this->width(), this->height()), Qt::black);
		p.drawImage(0, 0, image);

		if (type)
		{
			if (WIN_MANAGER->getSeedShow() == RT_SHOW)
				drawSeedPoints(&p);

			drawAnnotations(&p);

			QFontMetrics fontMet(p.font());
			QString str = getMoveMode();
			QRect rect = fontMet.boundingRect(str);

			p.setPen(QPen(QColor(62, 137, 219)));
			p.drawText(0, this->height() - rect.height(), str);

			if (g_Renderer->isAvailableVolumeRender() == false)
			{
				p.setPen(Qt::red);
				str = QString("Volume Rendering requires");
				rect = fontMet.boundingRect(str);

				p.drawText(QPoint((this->width() / 2) - (rect.width() / 2), 10 + rect.height()), str);

				str = QString("GPU Graphic card.");
				QRect rect2 = fontMet.boundingRect(str);

				p.drawText(QPoint((this->width() / 2) - (rect.width() / 2), 10 + rect.height() + rect2.height()), str);
			}
		}

		if (mode)
		{
			p.end();
			return img;
		}
		else //rectangle
		{
			p.end();

			int w, h;

			w = abs(m_polyLine.at(0).x() - m_polyLine.at(1).x());
			w++;

			h = abs(m_polyLine.at(0).y() - m_polyLine.at(1).y());
			h++;

			if ((1 >= w) && (1 >= h))
				return QImage();

			QRect rect;

			QPoint leftTop = QPoint(m_polyLine.at(0));
			QPoint rightBottom = QPoint(m_polyLine.at(1));

			int tmpPoint;

			if (leftTop.x() > rightBottom.x())
			{
				tmpPoint = leftTop.x();
				leftTop.setX(rightBottom.x());
				rightBottom.setX(tmpPoint);
			}
			if (leftTop.y() > rightBottom.y())
			{
				tmpPoint = leftTop.y();
				leftTop.setY(rightBottom.y());
				rightBottom.setY(tmpPoint);
			}

			if (leftTop.x() < 0 || (leftTop.x() >= this->width()))
			{
				int nX = leftTop.x() > 0 ? (this->width() - 1) : 0;
				leftTop.setX(nX);
			}
			if (rightBottom.x() < 0 || (rightBottom.x() >= this->width()))
			{
				int nX = rightBottom.x() > 0 ? (this->width() - 1) : 0;
				rightBottom.setX(nX);
			}

			if (leftTop.y() < 0 || (leftTop.y() >= this->height()))
			{
				int nY = leftTop.y() > 0 ? (this->height() - 1) : 0;
				leftTop.setY(nY);
			}
			if (rightBottom.y() < 0 || (rightBottom.y() >= this->height()))
			{
				int nY = rightBottom.y() > 0 ? (this->height() - 1) : 0;
				rightBottom.setY(nY);
			}

			rect.setCoords(leftTop.x(), leftTop.y(), rightBottom.x(), rightBottom.y());

			w = rect.width();
			h = rect.height();

			if ((w <= 1) && (h <= 1))
				return QImage();


			QImage rectImg = QImage(w, h, QImage::Format_RGBA8888);
			QPainter p2(&rectImg);
			p2.drawImage(0, 0, img.copy(rect));

			/*if (!WIN_MANAGER->IsLicensePass())
			{
				QImage * WaterImage = RESOURCE_MANAGER->getWaterMark();
				if (WaterImage != NULL)
				{
					QImage drawImg = WaterImage->scaled(QSize(w, h / 3), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
					p2.drawImage(QPoint(0, h / 4), drawImg);
				}
			}*/

			p2.end();

			//	SAFE_DELETE(img);

			return rectImg;
		}
	}

	return QImage();
}

// 입력된 사각형 사이즈로 캡쳐.
QImage VolumeView::getCaptureImage(QPolygon& polyLine)
{
	WIN_MANAGER->makeCurrent();
	void* rData = g_Renderer->getBackBuffer(0, 0, this->width(), this->height());
	WIN_MANAGER->doneCurrent();

	if (rData != NULL)
	{
		QImage image((uchar*)rData, this->width(), this->height(), QImage::Format_RGB32);
		QImage img = QImage(this->width(), this->height(), QImage::Format_RGBA8888);

		QPainter p(&img);
		p.fillRect(QRect(0, 0, this->width(), this->height()), Qt::black);
		p.drawImage(0, 0, image);
		p.end();

		int w, h;

		w = abs(polyLine.at(0).x() - polyLine.at(1).x());
		w++;

		h = abs(polyLine.at(0).y() - polyLine.at(1).y());
		h++;

		if ((1 >= w) && (1 >= h))
			return QImage();

		QRect rect;

		QPoint leftTop = QPoint(polyLine.at(0));
		QPoint rightBottom = QPoint(polyLine.at(1));

		int tmpPoint;

		if (leftTop.x() > rightBottom.x())
		{
			tmpPoint = leftTop.x();
			leftTop.setX(rightBottom.x());
			rightBottom.setX(tmpPoint);
		}
		if (leftTop.y() > rightBottom.y())
		{
			tmpPoint = leftTop.y();
			leftTop.setY(rightBottom.y());
			rightBottom.setY(tmpPoint);
		}

		if (leftTop.x() < 0 || (leftTop.x() >= this->width()))
		{
			int nX = leftTop.x() > 0 ? (this->width() - 1) : 0;
			leftTop.setX(nX);
		}
		if (rightBottom.x() < 0 || (rightBottom.x() >= this->width()))
		{
			int nX = rightBottom.x() > 0 ? (this->width() - 1) : 0;
			rightBottom.setX(nX);
		}

		if (leftTop.y() < 0 || (leftTop.y() >= this->height()))
		{
			int nY = leftTop.y() > 0 ? (this->height() - 1) : 0;
			leftTop.setY(nY);
		}
		if (rightBottom.y() < 0 || (rightBottom.y() >= this->height()))
		{
			int nY = rightBottom.y() > 0 ? (this->height() - 1) : 0;
			rightBottom.setY(nY);
		}

		rect.setCoords(leftTop.x(), leftTop.y(), rightBottom.x(), rightBottom.y());

		w = rect.width();
		h = rect.height();

		if ((w <= 1) && (h <= 1))
			return QImage();


		QImage rectImg = QImage(w, h, QImage::Format_RGBA8888);
		QPainter p2(&rectImg);
		p2.drawImage(0, 0, img.copy(rect));

		/*if (!WIN_MANAGER->IsLicensePass())
		{
		QImage * WaterImage = RESOURCE_MANAGER->getWaterMark();
		if (WaterImage != NULL)
		{
		QImage drawImg = WaterImage->scaled(QSize(w, h / 3), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		p2.drawImage(QPoint(0, h / 4), drawImg);
		}
		}*/

		p2.end();

		//	SAFE_DELETE(img);

		return rectImg;
	}

	return QImage();
}

void renderHMDScene(muint32 nVREye, void* data)
{
#ifndef _M_IX86 // VR
	VolumeView* viewWindow = (VolumeView*)data;
	if (viewWindow == NULL) return;

	mip::MATRIX44 matWorld = g_Renderer->getWorld();
	mip::MATRIX44 matView = viewWindow->getHMDViewMatrix(nVREye, &matWorld);
	mip::MATRIX44 matProj = viewWindow->getHMDProjMatrix(nVREye);

	g_Renderer->setView(matView);
	g_Renderer->setProj(matProj);

	g_Renderer->clear(mip::COLOR(30, 30, 30)); // important

	if (g_Renderer->beginScene())
	{
		if (DATA_CONTEXT->volume_data.isValidate())
		{
			mip::VolumeRenderer::DrawBG(g_Renderer, (mip::BGTYPE)6);

			int render_type = WIN_MANAGER->getRenderType();

			viewWindow->renderModel(matWorld, matView, matProj, WIN_MANAGER->getCullingMesh());

			if (render_type != RT_SURFACE)
			{
				mip::MATRIX44 vrView = viewWindow->getHMDViewMatrix(0, &matWorld);

				if (g_Renderer->isValidateHMD())
					viewWindow->renderVolume(false, matWorld, matView, matProj, true, &vrView);
				else
					viewWindow->renderVolume(false, matWorld, matView, matProj, true);
			}

			g_Renderer->renderHMDController(nVREye, WIN_MANAGER->getFontTexture());
		}

		g_Renderer->endScene();
	}
#endif
}

void VolumeView::slot_OnUpdateFrame()
{
#ifndef _M_IX86 // VR
	if (!(g_Renderer->isValidateHMD() || g_Renderer->isValidateHMDShader())) return;

	m_camera.updateOrtho();
	m_camera.updateLookAtCamera();

	mip::MATRIX44 matWorld = m_trVolumModel.getMatrix();
	g_Renderer->setWorld(matWorld);

	if (g_Renderer->beginRender(0, this->size().width(), this->size().height()))
	{
		g_Renderer->renderStereoTargets(g_Renderer->getDefaultFBOID(), renderHMDScene, this);

		g_Renderer->renderCompanionWindow(this->width(), this->height(), true);

		if (getCurrentPainter() != NULL)
		{
			//g_Renderer->present();
			if (g_Renderer->isValidateHMD())
			{
				void* rData = g_Renderer->getBackBuffer(0, 0, this->width(), this->height());
				if (rData != NULL)
				{
					QImage image((uchar*)rData, this->width(), this->height(), QImage::Format_RGB32);

					//getCurrentPainter()->setRenderHint(QPainter::Antialiasing);
					getCurrentPainter()->drawImage(0, 0, image);
				}
			}
			else
			{
				void* rData = g_Renderer->getBackBuffer(0, 0, g_Renderer->getHMDWidth(), g_Renderer->getHMDHeight());
				if (rData != NULL)
				{
					QImage image((uchar*)rData, g_Renderer->getHMDWidth(), g_Renderer->getHMDHeight(), QImage::Format_RGB32);

					getCurrentPainter()->setRenderHint(QPainter::Antialiasing);

					QRect target(0, 0, this->width(), this->height());
					QRect source(0.0, 0.0, g_Renderer->getHMDWidth(), g_Renderer->getHMDHeight());
					getCurrentPainter()->drawImage(target, image, source);
					//getCurrentPainter()->drawImage(0,0, image);
				}
			}
		}

		g_Renderer->endRender();
	}

	g_Renderer->updateHMDMatrixPose();
#endif
}

void VolumeView::touchRelease()
{
	useSkipRender(1, false);
	useSkipRender(2, false);

	renderLater();
}

void VolumeView::touchMove(mint32 preX, mint32 preY, mint32 currX, mint32 currY)
{
	mip::VECTOR3 v1 = m_camera.getWorldPoint(preX, preY, 0);
	mip::VECTOR3 v2 = m_camera.getWorldPoint(currX, currY, 0);

	useSkipRender(1);

	m_trVolumModel.addTranslate(v2 - v1);

	renderLater();
}

void VolumeView::touchZoom(float dt)
{
	m_camera.wheelZoom(dt * 0.1f);
	useSkipRender(2);
	renderLater();
}

void VolumeView::dropEvent(QDropEvent* event)
{
	//	throw std::logic_error("The method or operation is not implemented.");
	const QMimeData* mimeData = event->mimeData();
	if (mimeData->text().isEmpty())
	{
		event->ignore();
		return;
	}

	QString filename;
	QUrl fileUrl = mimeData->urls().at(0);

	filename = fileUrl.toLocalFile();

	/*local/remote 구분*/
	if (-1 != filename.indexOf("//"))
	{
		//경고창
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_REMOTE_FILE),
			STRING_MANAGER->getString(STR_REMOTE_FILE_DELAYED));
	}

	//	WIN_MANAGER->fileOpen(filename);
	WIN_MANAGER->mainWindow->fileOpen(filename);

	event->acceptProposedAction();
}

void VolumeView::dragEnterEvent(QDragEnterEvent* event)
{
	//	throw std::logic_error("The method or operation is not implemented.");
	event->accept();
}

/*
 *@function setHomePosition
 *@brief 정위치 변동
 *@param index 정위치 (0~5 / A,P,R,L,S,I 매칭)
*/
void VolumeView::setHomePosition(HOME_POSITION index, bool bForce)
{
	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	if (!(WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION))
	{
		if (!bForce)
		{
			if (this != QApplication::focusWidget())
				return;
		}
	}

	// 220311 허 건 과장
	{
		if (DATA_CONTEXT->volume_data.isValidate())
		{
			float size = DATA_CONTEXT->volume_data.getSizeX() * 0.5f;
			if (size < DATA_CONTEXT->volume_data.getSizeY() * 0.5f) size = DATA_CONTEXT->volume_data.getSizeY() * 0.5f;
			if (size < DATA_CONTEXT->volume_data.getSizeZ() * 0.5f) size = DATA_CONTEXT->volume_data.getSizeZ() * 0.5f;

			mip::VECTOR3 cameraPos = mip::VECTOR3(-3.2299f, 2.0845f, size * 2);

#ifdef USE_RIGHT_HAND
			m_camera.setPos(cameraPos);
#else
			m_camera.setPos(cameraPos);
#endif
			m_camera.setZoomMax(size * 3);
			m_camera.setZoom(size * 2.0f);
			//m_camera.setZoom(10.0f);
			//m_camera.setNearFar(0.1f, size * 4);
			m_camera.setNearFar(0.1f, size * 8);

			cameraPos.y = 0.0f;
			cameraPos.normalize();
			m_fCameraXZAngle = acosf(cameraPos.dot(mip::VECTOR3(0.0f, 0.0f, 1.0f)));
		}
		else
		{
			mip::VECTOR3 cameraPos = mip::VECTOR3(-3.2299f, 2.0845f, 15.0f);

			m_camera.setPos(cameraPos);
			m_camera.setZoom(20.0f);

			cameraPos.y = 0.0f;
			cameraPos.normalize();
			m_fCameraXZAngle = acosf(cameraPos.dot(mip::VECTOR3(0.0f, 0.0f, 1.0f)));
		}

		m_camera.setAt(mip::VECTOR3(0, 0, 0));
	}

	m_trVolumModel.zero();

	switch (index)
	{
	case HP_POSTERIOR:
		m_trVolumModel.addRotateZ(M_PI);
		m_trVolumModel.addRotateX(-90. * M_PI / 180.);
		break;
	case HP_RIGHT:
		m_trVolumModel.addRotateZ(90. * M_PI / 180.);
		m_trVolumModel.addRotateX(-90. * M_PI / 180.);
		break;
	case HP_LEFT:
		m_trVolumModel.addRotateZ(-90. * M_PI / 180.);
		m_trVolumModel.addRotateX(-90. * M_PI / 180.);
		break;
	case HP_SUPERIOR:
		break;
	case HP_INFERIOR:
		m_trVolumModel.addRotateX(M_PI);
		break;
	default:
	case HP_ANTERIOR:
		m_trVolumModel.addRotateX(-90. * M_PI / 180.);
		break;
	}

	//	
	m_trPlaneCut.zero();
	m_trPlaneImage.zero();

	renderLater();
}