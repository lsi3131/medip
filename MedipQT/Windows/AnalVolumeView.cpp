#include "stdafx.h"
#include "AnalVolumeView.h"
#include "windowManager.h"
#include "Renderer/Renderer.h"
#include "System/resourceManager.h"
#include "System/styleManager.h"
#include "System/stringManager.h"

#include "UI/CustomHistogram.h"
#include "mipEngine/intersect.h"
#include "mipEngine/geometry.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/Main/MainAnalWidget.h"
#include "Windows/Tabwindow.h"
#include "Actions/ActionManager.h"
#include "MedipQT.h"
#include "VolumeView.h"
#include "AnnotationFactory.h"
#include "AnalMPRPlaneView.h"
#include "DataContext.h"

#include "MeshEdit/CMeshViewRenderManager.h"

#include <QApplication>

AnalVolumeView::AnalVolumeView(DataContext* pDataContext, QWidget* parent)
	:OpenGLWidget(parent)
	, m_slidebar(0)
	, m_btn2D(0)
	, m_btnFullScreen(0)
	, actAxial(0)
	, actCoronal(0)
	, actSagittal(0)
	, m_bWorkMode(false)
	, m_pOriginAnnotation(nullptr)
	, m_pDataContext(pDataContext)
{
	m_labelImage = 0;
	m_slidebar = 0;

	m_RbuttonDown = false;
	m_LbuttonDown = false;
	m_MbuttonDown = false;
	m_mouseMove = false;

	for (int i = 0; i < 4; i++)
		m_bSkip[i] = false;


	m_nClip = CT_NONE;

	m_nIconSize = WIN_MANAGER->mainWindow->IconSize;

	float size = m_pDataContext->volume_data.getSizeX()*0.5f;
	if (size < m_pDataContext->volume_data.getSizeY()*0.5f) size = m_pDataContext->volume_data.getSizeY()*0.5f;
	if (size < m_pDataContext->volume_data.getSizeZ()*0.5f) size = m_pDataContext->volume_data.getSizeZ()*0.5f;

#ifdef USE_RIGHT_HAND
	m_camera.setPos(mip::VECTOR3(-3.2299f, 2.0845f, size * 2));
#else
	m_camera.setPos(mip::VECTOR3(-3.2299f, 2.0845f, size * 2));
#endif

	m_camera.setZoomMax(size * 3);
	m_camera.setZoom(size*2.0f);
	m_camera.setNearFar(0.1f, size * 8);
	m_camera.setAt(mip::VECTOR3(0, 0, 0));

	m_trVolumModel.zero();
	m_trVolumModel.addRotateX(3.14f);
	m_windowType = WT_VOLUME;

	m_cullingTexture = NULL;

	m_WheelTimer = new QTimer(this);

	if (m_WheelTimer)
	{
		m_WheelTimer->stop();
		m_WheelTimer->setSingleShot(true);
		connect(m_WheelTimer, SIGNAL(timeout()), this, SLOT(slot_OnWheelTimeout()));
	}


	HoverWidget = NULL;
	m_MousePos_Pressed = QPoint(0, 0);
	initInScreenMenu();

	setAcceptDrops(true);
}

AnalVolumeView::~AnalVolumeView()
{

}

void AnalVolumeView::hideControls()
{
	if (m_btn2D)
	{
		m_btn2D->hide();

		if (m_btnFullScreen)
		{
			if (!m_btnFullScreen->isHidden())
			{
				m_btnFullScreen->hide();
				renderLater();
			}
		}
		if (m_btnFreeDrawROI) m_btnFreeDrawROI->hide();
		if (m_btnDistance) m_btnDistance->hide();
		if (m_btnPlanecut) m_btnPlanecut->hide();
		if (m_btnSplit) m_btnSplit->hide();
	}
}

void AnalVolumeView::showControls()
{
	bool isEnable = true;
	if (g_Renderer->isAvailableVolumeRender() == false)
		isEnable = false;

	setWorkMode(WIN_MANAGER->getAnalWorkMode());
	if (m_btn2D)
	{
		if (m_btn2D)
		{
			m_btn2D->show();
			if (!isEnable)
				m_btn2D->hide();
		}

		if (m_btnFullScreen)m_btnFullScreen->show();
#ifndef _M_IX86
		if (m_btnFreeDrawROI)
		{
			if (isEnable)
				m_btnFreeDrawROI->show();
			else
				m_btnFreeDrawROI->hide();
		}

		if (m_btnPlanecut)
		{
#ifdef DEV_VER
			if (isEnable)
				m_btnPlanecut->show();
			else
#endif
				m_btnPlanecut->hide();
		}

		if (m_btnSplit)
		{
#ifdef DEV_VER
			if (isEnable)
				m_btnSplit->show();
			else
#endif
				m_btnSplit->hide();
		}

#endif
		if (m_btnDistance)
		{
			if (isEnable)
				m_btnDistance->show();
			else
				m_btnDistance->hide();
		}

	}
}

void AnalVolumeView::enableControls(bool bEnable)
{
//	throw std::logic_error("The method or operation is not implemented.");
	if (m_btn2D)
	{
	//	m_btn2D->setEnabled(bEnable);
	//	if (m_btnFullScreen) m_btnFullScreen->setEnabled(bEnable);
		if (m_btnFreeDrawROI) m_btnFreeDrawROI->setEnabled(bEnable);
		if (m_btnPlanecut) m_btnPlanecut->setEnabled(bEnable);
		if (m_btnSplit) m_btnSplit->setEnabled(bEnable);
	}
}

void AnalVolumeView::reserveInit(HWND hwnd)
{
	m_hwnd = hwnd;
}

void AnalVolumeView::setHomePosition(HOME_POSITION index)
{
	if (!m_pDataContext->volume_data.isValidate()) return;

	if (!((WIN_MANAGER->mainTabType == MAINTAB_MEASUREMENT || WIN_MANAGER->mainTabType == MAINTAB_TA)
		&& (this == QApplication::focusWidget()))) return;

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
	m_trPlaneImage.zero();
	m_trPlaneCut.zero();

	renderLater();
}

void AnalVolumeView::initInScreenMenu()
{
	if (NULL == m_btn2D)
	{		
		QMenu *clipMenu = new QMenu(this);
		clipMenu->installEventFilter(this);
		actAxial = new QAction(RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize), "    Axial Clip", this);
		actAxial->setCheckable(true);
		actAxial->setChecked(false);
		actCoronal = new QAction(RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize), "    Coronal Clip", this);
		actCoronal->setCheckable(true);
		actCoronal->setChecked(false);
		actSagittal = new QAction(RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize), "    Sagittal Clip", this);
		actSagittal->setCheckable(true);
		actSagittal->setChecked(false);

		connect(actAxial, &QAction::triggered, this, &AnalVolumeView::slot_OnAxial);
		connect(actCoronal, &QAction::triggered, this, &AnalVolumeView::slot_OnCoronal);
		connect(actSagittal, &QAction::triggered, this, &AnalVolumeView::slot_OnSagittal);

		clipMenu->addAction(actAxial);
		clipMenu->addAction(actCoronal);
		clipMenu->addAction(actSagittal);

		m_btn2D = new QToolButton(this);
		m_btn2D->setPopupMode(QToolButton::InstantPopup);
		m_btn2D->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize));
		m_btn2D->setMenu(clipMenu);
		m_btn2D->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btn2D->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btn2D->setMouseTracking(true);
		m_btn2D->installEventFilter(this);
		m_btn2D->setStyleSheet("color: black;");
		m_btn2D->setToolTip("2D View on 3D");
		m_btn2D->hide();


		m_btnFreeDrawROI = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize), "", this);
		m_btnFreeDrawROI->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnFreeDrawROI->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnFreeDrawROI->setCheckable(true);
		m_btnFreeDrawROI->setMouseTracking(true);
		m_btnFreeDrawROI->installEventFilter(this);
		m_btnFreeDrawROI->hide();
		m_btnFreeDrawROI->setStyleSheet("color: black;");
		m_btnFreeDrawROI->setToolTip("Region Selection\n(Only use for Right-click : deletion)");
		connect(m_btnFreeDrawROI, &QPushButton::clicked, this, &AnalVolumeView::slot_OnFreeDrawROI);

		m_btnDistance = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_LENGTH, m_nIconSize, m_nIconSize), "", this);
		m_btnDistance->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnDistance->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnDistance->setCheckable(true);
		m_btnDistance->setMouseTracking(true);
		m_btnDistance->installEventFilter(this);
		m_btnDistance->hide();
		m_btnDistance->setStyleSheet("color: black;");
		m_btnDistance->setToolTip("Calculate Distance");
		connect(m_btnDistance, &QPushButton::clicked, this, &AnalVolumeView::slot_OnCalcDistance);

		m_btnPlanecut = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_PLANE_CUT, m_nIconSize, m_nIconSize), "", this);
		m_btnPlanecut->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnPlanecut->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnPlanecut->setCheckable(true);
		m_btnPlanecut->setMouseTracking(true);
		m_btnPlanecut->installEventFilter(this);
		m_btnPlanecut->hide();
		m_btnPlanecut->setStyleSheet("color: black;");
		m_btnPlanecut->setToolTip("Plane cut(Splitting)\n(Apply : Right-click)");
		connect(m_btnPlanecut, &QPushButton::clicked, this, &AnalVolumeView::slot_OnPlanecut);

		m_btnSplit = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SPLIT_CURVE, m_nIconSize, m_nIconSize), "", this);
		m_btnSplit->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnSplit->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnSplit->setCheckable(true);
		m_btnSplit->setMouseTracking(true);
		m_btnSplit->installEventFilter(this);
		m_btnSplit->hide();
		m_btnSplit->setStyleSheet("color: black;");
		m_btnSplit->setToolTip("Split curve");
		connect(m_btnSplit, &QPushButton::clicked, this, &AnalVolumeView::slot_OnCurveSplit);

		m_btnFullScreen = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FULL_SCREEN, m_nIconSize, m_nIconSize), "", this);
		m_btnFullScreen->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnFullScreen->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnFullScreen->setCheckable(true);
		m_btnFullScreen->setMouseTracking(true);
		m_btnFullScreen->installEventFilter(this);
		m_btnFullScreen->hide();
		m_btnFullScreen->setStyleSheet("color: black;");
		m_btnFullScreen->setToolTip("Full screen");
		connect(m_btnFullScreen, &QPushButton::clicked, this, &AnalVolumeView::slot_OnFullScreen);

	}
}

void AnalVolumeView::updateScreenLeftMenu(int width, int height)
{
	if (m_btn2D != NULL)
	{
		int _offset = 0;
		m_btn2D->move(width - m_btn2D->width(), _offset);
		_offset += m_btn2D->height();
		m_btnFreeDrawROI->move(width - m_btnFreeDrawROI->width(), _offset);
		_offset += m_btnFreeDrawROI->height();
		m_btnDistance->move(width - m_btnDistance->width(), _offset);
		_offset += m_btnDistance->height();
		m_btnSplit->move(width - m_btnSplit->width(), _offset);
		_offset += m_btnSplit->height();
		m_btnPlanecut->move(width - m_btnPlanecut->width(), _offset);
		_offset += m_btnPlanecut->height();

		m_btnFullScreen->move(0, height - m_btnFullScreen->height());
	}
}

void AnalVolumeView::resetUI()
{
	reInit();
}

void AnalVolumeView::reInit()
{
	float size = m_pDataContext->volume_data.getSizeX()*0.5f;
	if (size < m_pDataContext->volume_data.getSizeY()*0.5f) size = m_pDataContext->volume_data.getSizeY()*0.5f;
	if (size < m_pDataContext->volume_data.getSizeZ()*0.5f) size = m_pDataContext->volume_data.getSizeZ()*0.5f;

#ifdef USE_RIGHT_HAND
	m_camera.setPos(mip::VECTOR3(-3.2299f, 2.0845f, size * 2));
#else
	m_camera.setPos(mip::VECTOR3(-3.2299f, 2.0845f, size * 2));
#endif

	m_camera.setZoomMax(size * 3);
	m_camera.setZoom(size*2.0f);
	m_camera.setNearFar(0.1f, size * 8);
	m_camera.setAt(mip::VECTOR3(0, 0, 0));
	m_trVolumModel.zero();
	m_trPlaneImage.zero();
	m_trPlaneCut.zero();
	m_trVolumModel.addRotateX(3.14f);
}

void AnalVolumeView::syncFullscreen()
{
	slot_OnFullScreen();
}

void AnalVolumeView::setClipPrefer(uchar _clip, bool optOnly)
{
	if (!m_pDataContext->volume_data.isValidate()) return;

	if (!optOnly)
	{
		bool res = false;
		if (_clip & CT_AXIAL)
			res = true;
		actAxial->setChecked(res);

		res = false;
		if (_clip & CT_CORONAL)
			res = true;
		actCoronal->setChecked(res);

		res = false;
		if (_clip & CT_SAGITTAL)
			res = true;
		actSagittal->setChecked(res);

		slot_OnAxial();
		slot_OnCoronal();
		slot_OnSagittal();
	}

	WIN_MANAGER->setClipOpt();
}

void AnalVolumeView::resizeEvent(QResizeEvent *e)
{
	if (e == NULL) return;

	int width = e->size().width();
	int height = e->size().height();

	updateScreenLeftMenu(width, height);

	if (m_slidebar)
		m_slidebar->move(width / 2 - m_slidebar->size().width() / 2, height - 30);

	renderLater();
}

void AnalVolumeView::keyPressEvent(QKeyEvent *e)
{
	checkModifiers(e);
}

void AnalVolumeView::keyReleaseEvent(QKeyEvent * e)
{
	//	OpenGLWidget::keyReleaseEvent(e);

	checkModifiers(e, false);

	if (e->key() == Qt::Key_Space)
		slot_OnFullScreen();

}

void AnalVolumeView::mouseMoveEvent(QMouseEvent *e)
{
	checkModifiers(e);

	//	if (WIN_MANAGER->getMoveFocus())
	//	{
	this->setFocus();
	//	}

	if (m_pDataContext->volume_data.isValidate() == false) return;

	m_mouseMove = true;
	WIN_MANAGER->setLatestActiveViewType(WT_VOLUME);
	m_preMousePos = m_MousePos;
	m_MousePos = e->pos();

	m_camera.setScreenXY(m_MousePos.x(), m_MousePos.y());

	switch (WIN_MANAGER->getAnalWorkMode())
	{
	case ANAL_WORK_CAPTURE:
		if (m_LbuttonDown)
		{
			bool mode = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureMode();

			if (!mode) //rectangle
			{
				if (m_polyLine.size() >= 2)
					m_polyLine.pop_back();

				m_polyLine.push_back(m_MousePos);
			}
			renderLater();
		}
		break;
	case ANAL_WORK_SEGMENT_FREEDRAW:
	case ANAL_WORK_3D_CURVE_SPLIT:
	{
		bool res = WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_SEGMENT_FREEDRAW ? m_RbuttonDown : m_LbuttonDown;
		if (res)
		{
			if (g_Renderer->isAvailableVolumeRender() == false)
				break;
			if (!m_polyLine.contains(m_MousePos))
				m_polyLine.append(m_MousePos);

			renderLater();
		}
	}
	break;
	case ANAL_WORK_SEGMENT_CALC_DISTANCE:
	case ANAL_WORK_3D_PLANE_SPLIT:
	default:
	{
		bool bSelected = false;
		for (auto ano = WIN_MANAGER->anotationList.begin(); ano != WIN_MANAGER->anotationList.end(); ++ano)
		{
			if ((*ano)->isSelected())
				bSelected = true;
		}

		if (m_touchZoomEvent == false && !bSelected)
		{
			mip::MATRIX44 matVolume = m_trVolumModel.getMatrix();
			const bool planecut = WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_3D_PLANE_SPLIT;
			if (m_MbuttonDown)
			{
				mip::VECTOR3 v1;
				mip::VECTOR3 v2;
				useSkipRender(1);

				if ((!m_ctrl) && (WIN_MANAGER->isClipMode() || planecut/*|| m_checkCoronal->isChecked() || m_checkSaggital->isChecked()*/))
				{
					v1 = m_camera.getWorldPoint(0, &matVolume);
					v2 = m_camera.getPreWorldPoint(0, &matVolume);

					if (planecut)
						m_trPlaneCut.addTranslate((v1 - v2));
					else
						m_trPlaneImage.addTranslate((v1 - v2));
				}
				else
				{
					v1 = m_camera.getWorldPoint();
					v2 = m_camera.getPreWorldPoint();

					m_trVolumModel.addTranslate((v1 - v2));
				}

				renderLater();
			}

			if (m_LbuttonDown && !m_ctrl)
			{
				useSkipRender(1);

				if ((!m_ctrl) && (WIN_MANAGER->isClipMode() || planecut /*|| m_checkCoronal->isChecked() || m_checkSaggital->isChecked()*/))
				{
					if (planecut)
						TransformRotate(&m_trPlaneCut, &m_camera, &(matVolume));
					else
						TransformRotate(&m_trPlaneImage, &m_camera, &(matVolume));
				}
				else
				{
					TransformRotate(&m_trVolumModel, &m_camera);
				}

				renderLater();
			}
		}

		
		mouseMove_Annotation_NormalMode();
		break;
	}
		
	}



	m_preMousePos = e->pos();
	m_camera.setPreScreenXY(m_preMousePos.x(), m_preMousePos.y());
}


bool AnalVolumeView::eventFilter(QObject * target, QEvent * e)
{
	static QIcon clipIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_PLANE_CUT, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_PLANE_CUT_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon fullIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FULL_SCREEN, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_FULL_SCREEN_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon TDIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_2D_VIEW_ON_3D_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon freeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_VIEW_FREEDRAWROI_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon splitIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_SPLIT_CURVE, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_SPLIT_CURVE_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon planeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_PLANE_CUT, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_PLANE_CUT_HOVER, m_nIconSize, m_nIconSize) };
	static QIcon distIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_LENGTH, m_nIconSize, m_nIconSize) ,		
		RESOURCE_MANAGER->getIcon(ICON_ANNO_LENGTH_HOVER, m_nIconSize, m_nIconSize),
		RESOURCE_MANAGER->getIcon(ICON_ANNO_LENGTH, m_nIconSize, m_nIconSize)};
	

	if (target == NULL)
		return QWidget::eventFilter(target, e);

	if (!target->inherits("QMenu") && !target->inherits("QPushButton")
		&& !target->inherits("QToolButton"))
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
		QMouseEvent *evt = dynamic_cast<QMouseEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
			}
		}
	}
	if (1 != res)
	{
		QHoverEvent *evt = dynamic_cast<QHoverEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
			}
		}
	}
	if (1 != res)
	{
		QWheelEvent *evt = dynamic_cast<QWheelEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
					res = 1;
			}
		}
	}

	if (res == 1)
	{
		if (target == m_btnFreeDrawROI && !m_btnFreeDrawROI->isChecked())
		{
			if (g_Renderer->isAvailableVolumeRender() == false) return QWidget::eventFilter(target, e);

			m_btnFreeDrawROI->setIcon(freeIcon[res]);
			HoverWidget = m_btnFreeDrawROI;
			LeaveIcon = freeIcon[0];
		}
		else if (target == m_btnDistance && !m_btnDistance->isChecked())
		{
			if (g_Renderer->isAvailableVolumeRender() == false) return QWidget::eventFilter(target, e);			
			m_btnDistance->setIcon(distIcon[res]);
			HoverWidget = m_btnDistance;
			LeaveIcon = distIcon[0];
		}
		else if (target == m_btnSplit && !m_btnSplit->isChecked())
		{
			if (g_Renderer->isAvailableVolumeRender() == false) return QWidget::eventFilter(target, e);

			m_btnSplit->setIcon(splitIcon[res]);
			HoverWidget = m_btnSplit;
			LeaveIcon = splitIcon[0];
		}
		else if (target == m_btnPlanecut && !m_btnPlanecut->isChecked())
		{
			if (g_Renderer->isAvailableVolumeRender() == false) return QWidget::eventFilter(target, e);

			m_btnPlanecut->setIcon(planeIcon[res]);
			HoverWidget = m_btnPlanecut;
			LeaveIcon = planeIcon[0];
		}
		else if (target == m_btnFullScreen && !m_fullscreen)
		{
			m_btnFullScreen->setIcon(fullIcon[res]);
			HoverWidget = m_btnFullScreen;
			LeaveIcon = fullIcon[0];
		}
		else if (target == m_btn2D && (m_nClip == CT_NONE))
		{
			m_btn2D->setIcon(TDIcon[res]);
			HoverWidget = m_btn2D;
			LeaveIcon = TDIcon[0];
		}
	}

	return QWidget::eventFilter(target, e);
}

void AnalVolumeView::focusInEvent(QFocusEvent *)
{
	setWorkMode(WIN_MANAGER->getAnalWorkMode());
}

void AnalVolumeView::TransformRotate(mip::TRANSFORM * tr, const mip::SCAMERA * camera, mip::MATRIX44 * trasform)
{
	mip::VECTOR3 v1 = camera->getWorldPoint(0.5f, trasform);
	mip::VECTOR3 v2 = camera->getPreWorldPoint(0.5f, trasform);

	mip::VECTOR3 rV = (v1 - v2);
	int cx, cy, px, py;
	camera->getScreenSize(cx, cy);
	camera->getScreenXY(px, py);

	float dy = rV.y / (float)(cy);
	float dx = rV.x / (float)(cx);

	mip::MATRIX44 matInvCamera;
	matInvCamera = trasform ? (*trasform) * camera->getView() : camera->getView();
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

		tr->addRotate(q);
	}
	else if (px > (cx* 0.9f))
	{
		q.setRotationAxis(Z, dy * 60.0f);
		tr->addRotate(q);
	}
	else if (px < (cx * 0.1f))
	{
		q.setRotationAxis(Z, -dy * 60.0f);
		tr->addRotate(q);
	}
}



void AnalVolumeView::mousePressEvent(QMouseEvent *e)
{
	if (m_pDataContext->volume_data.isValidate() == false) return;

	if (e->buttons() & Qt::RightButton)
		m_RbuttonDown = true;
	else if (e->buttons() & Qt::LeftButton)
		m_LbuttonDown = true;
	else if (e->buttons() & Qt::MidButton)
		m_MbuttonDown = true;

	m_MousePos = e->pos();
	m_preMousePos = e->pos();
	m_mouseMove = false;

	/*m_camera.setScreenXY(m_MousePos.x(), m_MousePos.y());
	m_camera.setPreScreenXY(m_MousePos.x(), m_MousePos.y());*/

	printf_s("\n AnalVolumeView - m_MousePos.x %d /  m_MousePos.y %d", m_MousePos.x(), m_MousePos.y());

	switch (WIN_MANAGER->getAnalWorkMode())
	{
	case ANAL_WORK_CAPTURE:
		if (m_LbuttonDown)
		{
			bool mode = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureMode();
			bool type = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureType();

			m_polyLine.clear();

			if (mode)
			{
				/*QImage *img = getCaptureImage(mode, type);
				if (img)
					WIN_MANAGER->captureList.push_back(img);*/

				QImage img = getCaptureImage(mode, type);

				if (img != QImage())
					ACTION_MANAGER->action_Capture_image_Add(img);

				setWorkMode(ANAL_WORK_NONE);
			}
			else //rectangle
			{
				m_bWorkMode = true;
				m_polyLine.push_back(e->pos());
			}
		}
		break;
	case ANAL_WORK_SEGMENT_FREEDRAW:
	case ANAL_WORK_3D_CURVE_SPLIT:
	{
		bool res = WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_SEGMENT_FREEDRAW ? m_RbuttonDown : m_LbuttonDown;
		if (res)
		{
			m_polyLine.clear();
			m_bWorkMode = true;
			if (g_Renderer->isAvailableVolumeRender() == false)
				return;

			m_polyLine.append(m_MousePos);
		}
	}
	break;
	case ANAL_WORK_3D_PLANE_SPLIT:
		if (m_RbuttonDown)
		{
			m_bWorkMode = true;
			//todo planecut action
			mip::MATRIX44 matOffset = mip::MATRIX44::Identity;
			matOffset.translation(-m_pDataContext->volume_data.getSizeX()*0.5f, -m_pDataContext->volume_data.getSizeY()*0.5f, -m_pDataContext->volume_data.getSizeZ()*0.5f);

			mip::MATRIX44 matinvOffset;
			mip::math::MatrixInverse(&matinvOffset, &matOffset);

			mip::MATRIX44 matPI = m_trPlaneCut.getMatrix()*matinvOffset;
			std::vector<mip::VECTOR3> planePointList;
			std::vector<mip::VECTOR3> planeCoordList;
			planePointList.clear();
			planeCoordList.clear();
			m_pDataContext->volume_data.getPlaneSurface(WT_AXIAL, planePointList, planeCoordList, &matPI);

			mip::PLANE plane(planePointList[0], planePointList[1], planePointList[2]);

			ACTION_MANAGER->action_Split3D(this, QPolygon(), WIN_MANAGER->getSelectedMask(),
				WIN_MANAGER->getSelectedMaskByteIndex(), m_pDataContext->volume_data.getCurrentMaskInfo()->uid, WORK_3D_PLANE_SPLIT, plane);
		}
		break;
	case ANAL_WORK_SEGMENT_CALC_DISTANCE:
		if (m_LbuttonDown)
		{
			/* Text Type일 경우 Dialog에서 Text 초기화 */
			QString text;
			muint16 fontSize;
			QColor qcolor = WIN_MANAGER->annoColor;
			COLOR color = toCOLOR(qcolor);

			mip::VECTOR3 v3HitPoint;
			std::vector<std::pair<int, mask8 >>  vecHitMaskBit;

			bool ret = false;
			ret = pickingMask(mip::VECTOR2(m_MousePos.x(), m_MousePos.y()), v3HitPoint, vecHitMaskBit);
			if (ret)
			{
				m_vecVolumePoint.push_back(v3HitPoint);
				printf_s("\n Point Size - %zd / v3HitPoint [x : %f]  [y : %f] [z : %f]", m_vecVolumePoint.size(), v3HitPoint.x, v3HitPoint.y, v3HitPoint.z);

				int _1st = 0; int _2nd = 1;
				if (m_vecVolumePoint.size() > 1)
				{
#if 1
					mip::VECTOR3 pt_1st = m_vecVolumePoint[_1st];
					mip::VECTOR3 pt_2nd = m_vecVolumePoint[_2nd];
					m_pVolumeDistAnnotation = AnnotationFactory::createInstance(AT_LEN, this, m_vecVolumePoint, color, text, fontSize);

					AnnoLength* annoLength = (AnnoLength*)m_pVolumeDistAnnotation.get();
					ACTION_MANAGER->action_Annotation_Len_Add(annoLength);
					WIN_MANAGER->getAnnotationColor();
					setWorkMode(ANAL_WORK_NONE);

					// reposition
					mip::VECTOR3 vVoxelPos(0,0,0);
					WIN_MANAGER->volume_renderer.getVoxelIndex(v3HitPoint, &vVoxelPos);

					/* 공용 MPRInfo 값 저장 */
					float HU = m_pDataContext->volume_data.getData(vVoxelPos.x, vVoxelPos.y, vVoxelPos.z);

					AnalMPRPlaneView* pAxial = WIN_MANAGER->mainAnalWidget->getWindow(WT_AXIAL);
					AnalMPRPlaneView* pCoronal = WIN_MANAGER->mainAnalWidget->getWindow(WT_CORONAL);
					AnalMPRPlaneView* pSagittal = WIN_MANAGER->mainAnalWidget->getWindow(WT_SAGITTAL);

					pAxial->depthChanged((int)vVoxelPos.z);
					pCoronal->depthChanged((int)vVoxelPos.y);
					pSagittal->depthChanged((int)vVoxelPos.x);
#endif

					m_vecVolumePoint.clear();
				}
				
			}
		}
		break;

	case ANAL_WORK_NONE:	
		mousePress_InNormalMode();
		break;
	default:
		break;
	}


}

void AnalVolumeView::mouseReleaseEvent(QMouseEvent *e)
{
	useSkipRender(1, false);

	switch (WIN_MANAGER->getAnalWorkMode())
	{
	case ANAL_WORK_CAPTURE:
	{
		m_bWorkMode = false;
		if (m_LbuttonDown && m_polyLine.size() >= 2)
		{
			bool type = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureType();

			QImage img = getCaptureImage(false, type);

			if (img != QImage())
				ACTION_MANAGER->action_Capture_image_Add(img);

			m_polyLine.clear();
		}
		setWorkMode(ANAL_WORK_NONE);
	}
	break;
	case ANAL_WORK_SEGMENT_FREEDRAW:
	case ANAL_WORK_3D_CURVE_SPLIT:
	{
		m_bWorkMode = false;
		ANAL_WORK_MODE mode = WIN_MANAGER->getAnalWorkMode();
		bool res = mode == ANAL_WORK_SEGMENT_FREEDRAW ? m_RbuttonDown : m_LbuttonDown;
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
				if (ANAL_WORK_SEGMENT_FREEDRAW == mode)
					ACTION_MANAGER->action_FreeDrawROI3D(m_polyLine, this, WIN_MANAGER->getSelectedMask(), true, false,
						WIN_MANAGER->getSelectedMaskByteIndex(), m_pDataContext->volume_data.getCurrentMaskInfo()->uid);
				else
					ACTION_MANAGER->action_Split3D(this, m_polyLine, WIN_MANAGER->getSelectedMask(),
						WIN_MANAGER->getSelectedMaskByteIndex(), m_pDataContext->volume_data.getCurrentMaskInfo()->uid);

				m_polyLine.clear();
			}
			m_polyLine.clear();
		}
	}
	break;
	case ANAL_WORK_3D_PLANE_SPLIT:
		if (m_RbuttonDown)
			m_bWorkMode = false;
		break;

	case ANAL_WORK_NONE:
		if (m_LbuttonDown)
		{
			mouseRelease_EditSelectedAnnotation();
		}
		break;
	default:

		break;
	}



	renderLater();
	m_RbuttonDown = false;
	m_LbuttonDown = false;
	m_MbuttonDown = false;
}

void AnalVolumeView::wheelEvent(QWheelEvent *event)
{
	if (m_touchEvent == true) return;

	if (m_pDataContext->volume_data.isValidate() == false || m_bWorkMode) return;


	if (m_WheelTimer)
	{
		m_WheelTimer->stop();
		useSkipRender(2);
		m_WheelTimer->start(500);
	}
	bool planecut = WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_3D_PLANE_SPLIT;
	if ((WIN_MANAGER->isClipMode() || planecut/*|| m_checkCoronal->isChecked() || m_checkSaggital->isChecked()*/))
	{
		mip::MATRIX44 matPlaneImage = planecut ? m_trPlaneCut.getMatrix() : m_trPlaneImage.getMatrix();
		mip::VECTOR3 dir = matPlaneImage.getScaledZaxis();

		if (!m_ctrl)
			planecut ? m_trPlaneCut.addTranslate(dir * 0.001f * event->delta())
			: m_trPlaneImage.addTranslate(dir * 0.001f * event->delta());
		else
			m_camera.wheelZoom(event->delta() * 0.01f);
	}
	else
	{
		m_camera.wheelZoom(event->delta() * 0.01f);
	}
	renderLater();

	//mip::VECTOR3 * planes = NULL;
	//bool checkView = false;
	//float dt = event->delta() * 0.001f;
	//if (actAxial->isChecked())
	//{
	//	checkView = true;
	//	planes = &m_pDataContext->volume_data.axialPPlane[0];
	//	if (planes == NULL) return;

	//	mip::VECTOR3 v = ((planes[0] - planes[1]) ^ (planes[2] - planes[1])).normalize();

	//	v *= dt;

	//	for (int n = 0; n < 4; ++n)
	//	{
	//		planes[n] += v;
	//	}

	//}
	//if (actCoronal->isChecked())
	//{
	//	checkView = true;
	//	planes = &m_pDataContext->volume_data.coronalPPlane[0];
	//	if (planes == NULL) return;
	//	mip::VECTOR3 v = ((planes[0] - planes[1]) ^ (planes[2] - planes[1])).normalize();

	//	v *= dt;

	//	for (int n = 0; n < 4; ++n)
	//	{
	//		planes[n] += v;
	//	}
	//}
	//if (actSagittal->isChecked())
	//{
	//	checkView = true;
	//	planes = &m_pDataContext->volume_data.saggitalPPlane[0];
	//	if (planes == NULL) return;
	//	mip::VECTOR3 v = ((planes[0] - planes[1]) ^ (planes[2] - planes[1])).normalize();

	//	v *= dt;

	//	for (int n = 0; n < 4; ++n)
	//	{
	//		planes[n] += v;
	//	}
	//}
	////TODO if / else 조건 (a/c/s check 확인 조건)
	//
	//if (!checkView)
	//{
	//	m_camera.wheelZoom(event->delta() * 0.01f);
	//	renderLater();
	//}
	//else
	//	WIN_MANAGER->renderLater_AnalView();

}

void AnalVolumeView::dropEvent(QDropEvent* event)
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

void AnalVolumeView::dragEnterEvent(QDragEnterEvent* event)
{
	//	throw std::logic_error("The method or operation is not implemented.");
	event->accept();
}

void AnalVolumeView::renderScene(QPainter *p, bool skip_mode)
{
	m_camera.setScreenSize(this->width(), this->height());
	m_camera.updateOrtho();
	m_camera.updateLookAtCamera();

	mip::MATRIX44 matWorld = m_trVolumModel.getMatrix();
	mip::MATRIX44 matView = m_camera.getView();
	mip::MATRIX44 matProj = m_camera.getProj();

	g_Renderer->setWorld(matWorld);
	g_Renderer->setView(matView);
	g_Renderer->setProj(matProj);

	if (!g_Renderer->beginRender(0, this->size().width(), this->size().height())) return;

	g_Renderer->clear(mip::COLOR(30, 30, 30));

	if (g_Renderer->beginScene())
	{
		if (m_pDataContext->volume_data.isValidate())
		{
			g_Renderer->setViewPort(0, 0, this->width(), this->height());

			mip::VolumeRenderer::DrawBG(g_Renderer, (mip::BGTYPE)6);

			int render_type = WIN_MANAGER->getRenderType();

			renderMeshModel(matWorld, matView, matProj, WIN_MANAGER->getCullingMesh());

#ifdef _M_IX86
			if (render_type != RT_SURFACE)
			{
				renderVolume(skip_mode);
			}
#else
			renderVolume(skip_mode);
#endif

#if SUPPORT_COORDLINE
			WIN_MANAGER->volume_renderer.RenderCoordLine(g_Renderer);
#endif 

			g_Renderer->setViewPort(this->width() - 100, 10, 90, 90);
#ifndef _M_IX86
			if(WIN_MANAGER->bVisibleCoordinate)
				WIN_MANAGER->volume_renderer.RenderGuideBox(g_Renderer, WIN_MANAGER->getFontTexture());
#endif
		}

		g_Renderer->endScene();

		void * rData = g_Renderer->getBackBuffer(0, 0, this->width(), this->height());
		if (rData != NULL)
		{
			QImage image((uchar*)rData, this->width(), this->height(), QImage::Format_RGB32);
			m_img = QImage(this->width(), this->height(), QImage::Format_RGB32);
			QPainter p2(&m_img);
			p2.drawImage(0, 0, image);
			p2.end();


			//p->setRenderHint(QPainter::Antialiasing);

			p->drawImage(0, 0, image);
		}
	}

	g_Renderer->endRender();
}

void AnalVolumeView::renderVolume(bool skip_mode)
{
	if (m_pDataContext->volume_data.isValidate() == false) return;

	muint32 shader_quality = WIN_MANAGER->getShaderQuality();

	if (g_Renderer->isAvailableVolumeRender() == false ||
		shader_quality == 0)
	{
		std::vector<mip::VECTOR3> lineList;
		std::vector<mip::VECTOR2> coordList;
		mip::MATRIX44 matPI = g_Renderer->getWorld();

		float HuMin = WIN_MANAGER->getWindowLevel() - WIN_MANAGER->getWindowWidth() / 2;
		if (HuMin < m_pDataContext->volume_data.getHuMin())
			HuMin = m_pDataContext->volume_data.getHuMin();

		float HuMax = WIN_MANAGER->getWindowLevel() + WIN_MANAGER->getWindowWidth() / 2;
		if (HuMax > m_pDataContext->volume_data.getHuMax())
			HuMax = m_pDataContext->volume_data.getHuMax();

		float xLength = 0.0f, yLength = 0.0f;
		float xSpace = 0.0f, ySpace = 0.0f;
		float gamma = WIN_MANAGER->getGamma();
		float alpha = WIN_MANAGER->getVolumeAlpha() * 10;

		xLength = m_pDataContext->volume_data.getCY();
		yLength = m_pDataContext->volume_data.getCZ();
		xSpace = m_pDataContext->volume_data.getSpaceY();
		ySpace = m_pDataContext->volume_data.getSpaceZ();

		lineList.clear();
		coordList.clear();
		m_pDataContext->volume_data.getMPRPlaneSurfaceList2D(WT_AXIAL, lineList, coordList);
		int index = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
		bool res = index <= -1 ? true : false;
		if (WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneBegin2D(g_Renderer, WIN_MANAGER->getVolumeMixMode() ? 2 : 0,
			WIN_MANAGER->get2DPresetTexture(), HuMin, HuMax, xLength, yLength, xSpace, ySpace, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT)))
		{
			WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneRender2D(g_Renderer, WIN_MANAGER->get2DMPR_Axial(), 0, lineList, coordList, 1, alpha, gamma);

			WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneEnd(g_Renderer);
		}

		xLength = m_pDataContext->volume_data.getCX();
		yLength = m_pDataContext->volume_data.getCY();
		xSpace = m_pDataContext->volume_data.getSpaceX();
		ySpace = m_pDataContext->volume_data.getSpaceY();

		lineList.clear();
		coordList.clear();
		m_pDataContext->volume_data.getMPRPlaneSurfaceList2D(WT_SAGITTAL, lineList, coordList);

		if (WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneBegin2D(g_Renderer, WIN_MANAGER->getVolumeMixMode() ? 2 : 0, WIN_MANAGER->get2DPresetTexture(),
			HuMin, HuMax, xLength, yLength, xSpace, ySpace, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT)))
		{
			WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneRender2D(g_Renderer, WIN_MANAGER->get2DMPR_Sagittal(), 0, lineList, coordList, 1, alpha, gamma);

			WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneEnd(g_Renderer);
		}


		xLength = m_pDataContext->volume_data.getCX();
		yLength = m_pDataContext->volume_data.getCZ();
		xSpace = m_pDataContext->volume_data.getSpaceX();
		ySpace = m_pDataContext->volume_data.getSpaceZ();

		lineList.clear();
		coordList.clear();
		m_pDataContext->volume_data.getMPRPlaneSurfaceList2D(WT_CORONAL, lineList, coordList);

		if (WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneBegin2D(g_Renderer, WIN_MANAGER->getVolumeMixMode() ? 2 : 0, WIN_MANAGER->get2DPresetTexture(),
			HuMin, HuMax, xLength, yLength, xSpace, ySpace, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT)))
		{
			WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneRender2D(g_Renderer, WIN_MANAGER->get2DMPR_Coronal(), 0, lineList, coordList, 1, alpha, gamma);

			WIN_MANAGER->volume_renderer.drawVolumeAdvPlaneEnd(g_Renderer);
		}

		return;
	}

	if (WIN_MANAGER->getVolumeTexture() != 0)
	{
		for (int n = 0; n < m_pDataContext->volume_data.getMaskInfoListCnt(); n++)
		{
			MaskInfo * minfo = m_pDataContext->volume_data.getMaskInfo(n);
			COLOR color = minfo->color;
			g_Renderer->setMaskColor(minfo->uid + 2, mip::COLOR(color.r, color.g, color.b, minfo->show ? WIN_MANAGER->getLayerAlpha() : 0));
		}

		float slice = mip::math::Max(mip::math::Max(m_pDataContext->volume_data.getCX(), m_pDataContext->volume_data.getCY()), m_pDataContext->volume_data.getCZ()) * 1.5f;

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

		float HuMin = WIN_MANAGER->getWindowLevel() - WIN_MANAGER->getWindowWidth() / 2;
		float HuMax = WIN_MANAGER->getWindowLevel() + WIN_MANAGER->getWindowWidth() / 2;

		std::vector<mip::VECTOR3> lineList;
		std::vector<mip::VECTOR3> coordList;
		mip::AABB box3D = m_pDataContext->volume_data.getBoundingBox3DAABB();
		DWORD clipValue = 0;
		int RayResult = 0;
		DWORD clipValueCnt = 0;
		std::vector<mip::VECTOR3> tempList;

		if (1)
		{
			const bool planecut = WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_3D_PLANE_SPLIT;
			bool plane2D = planecut ? false : WIN_MANAGER->getClip2DPlane();
			if (WIN_MANAGER->isClipMode() || planecut)
			{
				tempList.clear();
				lineList.clear();
				coordList.clear();
				mip::MATRIX44 matPI = planecut ? m_trPlaneCut.getMatrix() : m_trPlaneImage.getMatrix();
				m_pDataContext->volume_data.getPlaneSurface(WT_AXIAL, tempList, coordList, &matPI);

				for (int n = 0; n < tempList.size(); ++n)
					lineList.push_back(tempList[n]);

				mip::PLANE pl;

				pl.fromTri(tempList[1], tempList[2], tempList[0]);
				g_Renderer->setClipPlane(clipValueCnt, (float*)&pl);

				clipValue |= RSCLIPPLANE0 << clipValueCnt;
				clipValueCnt++;

				if (!plane2D)
				{
					tempList.clear();
					std::vector<mip::VECTOR2> cList;
					m_pDataContext->volume_data.getCullingPlaneSurface(tempList, cList, 0.5f, &matPI);

					if (m_cullingTexture == 0)
					{
						QImage * image = RESOURCE_MANAGER->getTexture_CullingPlane();
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
				else
				{
					if (HuMin < m_pDataContext->volume_data.getHuMin())
						HuMin = m_pDataContext->volume_data.getHuMin();
					if (HuMax > m_pDataContext->volume_data.getHuMax())
						HuMax = m_pDataContext->volume_data.getHuMax();
					int index = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
					bool res = index <= -1 ? true : false;
					WIN_MANAGER->volume_renderer.drawVolumePlane(g_Renderer, lineList, coordList, WIN_MANAGER->getVolumeMixMode() ? 2 : 0,
						WIN_MANAGER->getVolumeTexture(), WIN_MANAGER->getMaskTexture(), WIN_MANAGER->get2DPresetTexture(),
						HuMin, HuMax, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT), WIN_MANAGER->getVolumeAlpha(), WIN_MANAGER->getGamma());
				}
			}


			mip::PLANE axialP, coronalP, saggitalP;

			{
				tempList.clear();
				lineList.clear();
				coordList.clear();

				plane2D = planecut ? false : actAxial->isChecked();

				if (!plane2D)
				{					
					m_pDataContext->volume_data.getMPRPPlanes(WT_AXIAL, tempList);

					lineList.push_back(tempList[0]);
					lineList.push_back(tempList[1]);
					lineList.push_back(tempList[1]);
					lineList.push_back(tempList[2]);
					lineList.push_back(tempList[2]);
					lineList.push_back(tempList[3]);
					lineList.push_back(tempList[3]);
					lineList.push_back(tempList[0]);

					g_Renderer->renderLineList(lineList, mip::COLOR(80, 152, 205));
				}
				else
				{					
					m_pDataContext->volume_data.getMPRPlaneSurface(WT_AXIAL, tempList, coordList);

					for (int n = 0; n < tempList.size(); ++n)
						lineList.push_back(tempList[n]);

				}

				axialP.fromTri(tempList[1], tempList[2], tempList[0]);


				if (lineList.size() > 0 && plane2D)
				{
					/*g_Renderer->setClipPlane(clipValueCnt, (float*)&axialP);

					clipValue |= RSCLIPPLANE0 << clipValueCnt;
					clipValueCnt++;*/

					if (HuMin < m_pDataContext->volume_data.getHuMin())
						HuMin = m_pDataContext->volume_data.getHuMin();
					if (HuMax > m_pDataContext->volume_data.getHuMax())
						HuMax = m_pDataContext->volume_data.getHuMax();
					int index = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
					bool res = index <= -1 ? true : false;
					WIN_MANAGER->volume_renderer.drawVolumePlane(g_Renderer, lineList, coordList, WIN_MANAGER->getVolumeMixMode() ? 2 : 0,
						WIN_MANAGER->getVolumeTexture(), WIN_MANAGER->getMaskTexture(), WIN_MANAGER->get2DPresetTexture(),
						HuMin, HuMax, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT), WIN_MANAGER->getVolumeAlpha(), WIN_MANAGER->getGamma());
				}

			}

			{
				tempList.clear();
				lineList.clear();
				coordList.clear();
				plane2D = planecut ? false : actCoronal->isChecked();

				if (!plane2D)
				{
					m_pDataContext->volume_data.getMPRPPlanes(WT_CORONAL, tempList);

					lineList.push_back(tempList[0]);
					lineList.push_back(tempList[1]);
					lineList.push_back(tempList[1]);
					lineList.push_back(tempList[2]);
					lineList.push_back(tempList[2]);
					lineList.push_back(tempList[3]);
					lineList.push_back(tempList[3]);
					lineList.push_back(tempList[0]);

					g_Renderer->renderLineList(lineList, mip::COLOR(228, 97, 117));
				}
				else
				{
					m_pDataContext->volume_data.getMPRPlaneSurface(WT_CORONAL, tempList, coordList);

					for (int n = 0; n < tempList.size(); ++n)
						lineList.push_back(tempList[n]);
				}

				coronalP.fromTri(tempList[1], tempList[2], tempList[0]);

				if (lineList.size() > 0 && plane2D)
				{
					/*g_Renderer->setClipPlane(clipValueCnt, (float*)&coronalP);

					clipValue |= RSCLIPPLANE0 << clipValueCnt;
					clipValueCnt++;*/

					if (HuMin < m_pDataContext->volume_data.getHuMin())
						HuMin = m_pDataContext->volume_data.getHuMin();

					if (HuMax > m_pDataContext->volume_data.getHuMax())
						HuMax = m_pDataContext->volume_data.getHuMax();

					int index = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
					bool res = index <= -1 ? true : false;

					WIN_MANAGER->volume_renderer.drawVolumePlane(g_Renderer, lineList, coordList, WIN_MANAGER->getVolumeMixMode() ? 2 : 0,
						WIN_MANAGER->getVolumeTexture(), WIN_MANAGER->getMaskTexture(), WIN_MANAGER->get2DPresetTexture(),
						HuMin, HuMax, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT), WIN_MANAGER->getVolumeAlpha(), WIN_MANAGER->getGamma());
				}
			}

			{
				tempList.clear();
				lineList.clear();
				coordList.clear();

				plane2D = planecut ? false : actSagittal->isChecked();

				if (!plane2D)
				{
					m_pDataContext->volume_data.getMPRPPlanes(WT_SAGITTAL, tempList);

					lineList.push_back(tempList[0]);
					lineList.push_back(tempList[1]);
					lineList.push_back(tempList[1]);
					lineList.push_back(tempList[2]);
					lineList.push_back(tempList[2]);
					lineList.push_back(tempList[3]);
					lineList.push_back(tempList[3]);
					lineList.push_back(tempList[0]);

					g_Renderer->renderLineList(lineList, mip::COLOR(94, 179, 119));
				}
				else
				{
					m_pDataContext->volume_data.getMPRPlaneSurface(WT_SAGITTAL, tempList, coordList);

					for (int n = 0; n < tempList.size(); ++n)
						lineList.push_back(tempList[n]);

				}

				saggitalP.fromTri(tempList[1], tempList[2], tempList[0]);

				if (lineList.size() > 0 && plane2D)
				{
					/*g_Renderer->setClipPlane(clipValueCnt, (float*)&saggitalP);

					clipValue |= RSCLIPPLANE0 << clipValueCnt;
					clipValueCnt++;*/

					if (HuMin < m_pDataContext->volume_data.getHuMin())
						HuMin = m_pDataContext->volume_data.getHuMin();

					if (HuMax > m_pDataContext->volume_data.getHuMax())
						HuMax = m_pDataContext->volume_data.getHuMax();

					int index = WIN_MANAGER->getSelectedCustomPreset(CL_2D);
					bool res = index <= -1 ? true : false;
					WIN_MANAGER->volume_renderer.drawVolumePlane(g_Renderer, lineList, coordList, WIN_MANAGER->getVolumeMixMode() ? 2 : 0,
						WIN_MANAGER->getVolumeTexture(), WIN_MANAGER->getMaskTexture(), WIN_MANAGER->get2DPresetTexture(),
						HuMin, HuMax, res ? WIN_MANAGER->getSelectedPreset() : (index + SP_COUNT), WIN_MANAGER->getVolumeAlpha(), WIN_MANAGER->getGamma());
				}
			}

		}

		if (WIN_MANAGER->getCullingMaskVolume() == true)
		{
			g_Renderer->setRenderState(mip::RS_CLIPPLANEENABLE, clipValue);
		}

		HuMin = WIN_MANAGER->getVolumeLevel() - WIN_MANAGER->getVolumeWidth() / 2;
		HuMax = WIN_MANAGER->getVolumeLevel() + WIN_MANAGER->getVolumeWidth() / 2;

		muint32 renderpass = 0;
		if (WIN_MANAGER->getVolumeMixMode()) renderpass |= 1;

		float alpha = WIN_MANAGER->getVolumeAlpha();

		int index = WIN_MANAGER->getSelectedCustomPreset(CL_3D);
		bool res = index <= -1 ? true : false;

		WIN_MANAGER->volume_renderer.drawVolume(g_Renderer, renderpass, WIN_MANAGER->getVolumeTexture(), WIN_MANAGER->getMaskTexture(), WIN_MANAGER->get3DPresetTexture(),
			m_pDataContext->volume_data.getSizeX(), m_pDataContext->volume_data.getSizeY(), m_pDataContext->volume_data.getSizeZ(),
			HuMin, HuMax, res ? WIN_MANAGER->getSelectedVolumePreset() : (index + SP_COUNT),
			alpha, slice /*m_rotateModel ? 100.0f : 320.0f*/, 1.0f, WIN_MANAGER->getGamma(), WIN_MANAGER->getShaderQuality(), false, NULL, NULL);
		//					3, 1.0f, slice /*m_rotateModel ? 100.0f : 320.0f*/);
	}

	g_Renderer->setRenderState(mip::RS_CLIPPLANEENABLE, false);
}

void AnalVolumeView::renderMeshModel(
										mip::MATRIX44 & world, 
										mip::MATRIX44 & view, 
										mip::MATRIX44 & proj, 
										bool culling 
)
{
	mip::MATRIX44 matOffset = mip::MATRIX44::Identity;
	matOffset.translation(-m_pDataContext->volume_data.getSizeX()*0.5f, -m_pDataContext->volume_data.getSizeY()*0.5f, -m_pDataContext->volume_data.getSizeZ()*0.5f);
	int meshCount = m_pDataContext->m_MeshData.GetMeshCount();

	if (m_pDataContext->volume_data.isValidate() && meshCount > 0)
	{
		mip::MATRIX44 matinvOffset;
		mip::math::MatrixInverse(&matinvOffset, &matOffset);
		bool res = false;
		bool planecut = WIN_MANAGER->getWorkMode() == WORK_3D_PLANE_SPLIT;
		if (meshCount > 0)
		{
			MeshInfo* mInfo = nullptr;
			mip::MeshTopology* mesh = nullptr;
			for (int i = 0; i < meshCount; i++)
			{
				mInfo = m_pDataContext->m_MeshData.GetMeshInfo(i);

				if (mInfo && (mInfo->uid) != -1)
				{
					if (!mInfo->show) continue;

					mesh = m_pDataContext->m_MeshData.GetMesh(i);

					if (mesh && mesh->isLoaded() && mesh->m_TextureDataList.size() < 1) //20210330_byPHS_3mf_VolumeView에서 랜더링 안되게 수정
					{
						res = true;
						mip::MATRIX44 mat = (mesh->getMatrix() * matOffset*world);
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
		}
	}
}

void AnalVolumeView::mouseMove_Annotation_NormalMode()
{
	for (auto ano = WIN_MANAGER->anotationList.begin(); ano != WIN_MANAGER->anotationList.end(); ++ano)
	{
		if ((*ano)->isSelected() && m_LbuttonDown)
		{
			mip::VECTOR3 v3HitPoint;
			std::vector<std::pair<int, mask8 >>  vecHitMaskBit;

			bool ret = false;
			ret = pickingMask(mip::VECTOR2(m_MousePos.x(), m_MousePos.y()), v3HitPoint, vecHitMaskBit);

			(*ano)->movePosition_Measurement_Vol(this, m_MousePos.x(), m_MousePos.y(), v3HitPoint);

			// reposition
			mip::VECTOR3 vVoxelPos(0, 0, 0);
			WIN_MANAGER->volume_renderer.getVoxelIndex(v3HitPoint, &vVoxelPos);

			/* 공용 MPRInfo 값 저장 */
			float HU = m_pDataContext->volume_data.getData(vVoxelPos.x, vVoxelPos.y, vVoxelPos.z);

			AnalMPRPlaneView* pAxial = WIN_MANAGER->mainAnalWidget->getWindow(WT_AXIAL);
			AnalMPRPlaneView* pCoronal = WIN_MANAGER->mainAnalWidget->getWindow(WT_CORONAL);
			AnalMPRPlaneView* pSagittal = WIN_MANAGER->mainAnalWidget->getWindow(WT_SAGITTAL);

			pAxial->depthChanged((int)vVoxelPos.z);
			pCoronal->depthChanged((int)vVoxelPos.y);
			pSagittal->depthChanged((int)vVoxelPos.x);
		}
		else
		{
			(*ano)->setHover_Measurement(this, m_MousePos.x(), m_MousePos.y());
		}
		renderLater();
		
	}
}

void AnalVolumeView::mouseRelease_EditSelectedAnnotation()
{
	int index = 0;
	for (auto ano = WIN_MANAGER->anotationList.begin(); ano != WIN_MANAGER->anotationList.end(); ++ano)
	{
		if ((*ano)->isSelected())
		{
			if (m_MousePos != m_MousePos_Pressed)
			{
				if (commitToUpdateNewAnnotationStatus(*ano, index))
				{
					break;
				}
			}
		}
		index++;
	}

	/* Annotation Select를 진행 */
	if (WIN_MANAGER->anotationList.size() != 0)
	{
		/* Annotation Select 상태 초기화 */
		for (int i = WIN_MANAGER->anotationList.size() - 1; i >= 0; i--)
		{
			Annotation *ano = WIN_MANAGER->anotationList.at(i);
			(*ano).clearSelect();
		}
	}
}

void AnalVolumeView::mousePress_InNormalMode()
{
	if (m_LbuttonDown)
	{
		/* Annotation Select를 진행 */
		if (WIN_MANAGER->anotationList.size() != 0)
		{
			/* Annotation Select 상태 초기화 */
			for (int i = WIN_MANAGER->anotationList.size() - 1; i >= 0; i--)
			{
				Annotation *ano = WIN_MANAGER->anotationList.at(i);
				(*ano).clearSelect();
			}
		}
	}

	if (m_LbuttonDown )
	{
		/* Annotation Select를 진행 */
		if (WIN_MANAGER->anotationList.size() != 0)
		{
			for (int i = WIN_MANAGER->anotationList.size() - 1; i >= 0; i--)
			{
				Annotation *ano = WIN_MANAGER->anotationList.at(i);

				bool preSelect = ano->isSelected();
				bool curSelect = ano->setSelect_Measurement(this, m_MousePos.x(), m_MousePos.y());

				/* 새로 선택되었을 경우 화면 업데이트*/
				if (preSelect != curSelect)
				{
					/* 원본 Annotation 정보 저장 */
					WIN_MANAGER->renderLater_AnalView();
				}

				/* 선택이 되었다면 원본 Annotation 객체 생성 */
				if (curSelect)
				{
					if (ano->getType() == AT_LEN)
					{
						m_pOriginAnnotation = std::make_unique<AnnoLength>(*(AnnoLength*)ano);
					}
					else if (ano->getType() == AT_ARROW)
					{
						m_pOriginAnnotation = std::make_unique<AnnoArrow>(*(AnnoArrow*)ano);
					}
					else if (ano->getType() == AT_ANGLE)
					{
						m_pOriginAnnotation = std::make_unique<AnnoAngle>(*(AnnoAngle*)ano);
					}
					else if (ano->getType() == AT_RECTANGLE)
					{
						m_pOriginAnnotation = std::make_unique<AnnoRectangle>(*(AnnoRectangle*)ano);
					}
					else if (ano->getType() == AT_TEXT)
					{
						m_pOriginAnnotation = std::make_unique<AnnoString>(*(AnnoString*)ano);
					}

					break;
				}
			}
		}
	}
}


bool AnalVolumeView::commitToUpdateNewAnnotationStatus(Annotation* ano, int index)
{
	if (ano->getType() == AT_LEN)
	{
		AnnoLength *pAnnoOrigin = (AnnoLength*)m_pOriginAnnotation.get();
		AnnoLength *pAnnoNew = (AnnoLength*)ano;
		ACTION_MANAGER->action_Annotation_Len_Edit(index, pAnnoOrigin, pAnnoNew);

		return true;
	}
	else if (ano->getType() == AT_ARROW)
	{
		AnnoArrow *pAnnoOrigin = (AnnoArrow*)m_pOriginAnnotation.get();
		AnnoArrow *pAnnoNew = (AnnoArrow*)ano;
		ACTION_MANAGER->action_Annotation_Arrow_Edit(index, pAnnoOrigin, pAnnoNew);

		return true;
	}
	else if (ano->getType() == AT_ANGLE)
	{
		AnnoAngle *pAnnoOrigin = (AnnoAngle*)m_pOriginAnnotation.get();
		AnnoAngle *pAnnoNew = (AnnoAngle*)ano;
		ACTION_MANAGER->action_Annotation_Angle_Edit(index, pAnnoOrigin, pAnnoNew);

		return true;
	}
	else if (ano->getType() == AT_TEXT)
	{
		AnnoString *pAnnoOrigin = (AnnoString*)m_pOriginAnnotation.get();
		AnnoString *pAnnoNew = (AnnoString*)ano;
		ACTION_MANAGER->action_Annotation_Text_Edit(NULL, index, pAnnoOrigin, pAnnoNew);

		return true;
	}
	else if (ano->getType() == AT_RECTANGLE)
	{
		AnnoRectangle *pAnnoOrigin = (AnnoRectangle*)m_pOriginAnnotation.get();
		AnnoRectangle *pAnnoNew = (AnnoRectangle*)ano;
		ACTION_MANAGER->action_Annotation_Rectangle_Edit(index, pAnnoOrigin, pAnnoNew);

		return true;
	}

	return false;
}


void AnalVolumeView::touchRelease()
{
	useSkipRender(1, false);
	useSkipRender(2, false);

	renderLater();
}

void AnalVolumeView::slot_OnAxial()
{	
	static QIcon prIcon = RESOURCE_MANAGER->getIcon(ICON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize);
	static QIcon reIcon = RESOURCE_MANAGER->getIcon(ICON_NON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize);
	if (g_Renderer->isAvailableVolumeRender() == false)
	{
		actAxial->setChecked(!actAxial->isChecked());
		return;
	}

	if (actAxial->isChecked())
	{
		actAxial->setIcon(RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE, m_nIconSize, m_nIconSize));

		WIN_MANAGER->clipOnOff(false);

		m_nClip |= CT_AXIAL;
	}
	else
	{
		actAxial->setIcon(RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize));
		m_nClip &= (~CT_AXIAL);
	}

	if (m_nClip != CT_NONE)
		m_btn2D->setIcon(prIcon);
	else
		m_btn2D->setIcon(reIcon);

	renderLater();
}

void AnalVolumeView::slot_OnCoronal()
{
	static QIcon prIcon = RESOURCE_MANAGER->getIcon(ICON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize);
	static QIcon reIcon = RESOURCE_MANAGER->getIcon(ICON_NON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize);

	if (g_Renderer->isAvailableVolumeRender() == false)
	{
		actCoronal->setChecked(!actCoronal->isChecked());
		return;
	}

	if (actCoronal->isChecked())
	{
		actCoronal->setIcon(RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE, m_nIconSize, m_nIconSize));

		WIN_MANAGER->clipOnOff(false);

		m_nClip |= CT_CORONAL;
	}
	else
	{
		actCoronal->setIcon(RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize));
		m_nClip &= (~CT_CORONAL);
	}

	if (m_nClip != CT_NONE)
		m_btn2D->setIcon(prIcon);
	else
		m_btn2D->setIcon(reIcon);

	renderLater();
}

void AnalVolumeView::slot_OnSagittal()
{
	static QIcon prIcon = RESOURCE_MANAGER->getIcon(ICON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize);
	static QIcon reIcon = RESOURCE_MANAGER->getIcon(ICON_NON_2D_VIEW_ON_3D, m_nIconSize, m_nIconSize);

	if (g_Renderer->isAvailableVolumeRender() == false)
	{
		actSagittal->setChecked(!actSagittal->isChecked());
		return;
	}

	if (actSagittal->isChecked())
	{
		actSagittal->setIcon(RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE, m_nIconSize, m_nIconSize));

		WIN_MANAGER->clipOnOff(false);

		m_nClip |= CT_SAGITTAL;
	}
	else
	{
		actSagittal->setIcon(RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE, m_nIconSize, m_nIconSize));
		m_nClip &= (~CT_SAGITTAL);
	}

	if (m_nClip != CT_NONE)
		m_btn2D->setIcon(prIcon);
	else
		m_btn2D->setIcon(reIcon);

	renderLater();
}

void AnalVolumeView::slot_OnFullScreen()
{
	if (!m_pDataContext->volume_data.isValidate()) return;
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


void AnalVolumeView::slot_OnWheelTimeout()
{
	useSkipRender(2, false);

	renderLater();
}

void AnalVolumeView::slot_OnPlanecut(bool checked)
{
	if ((!m_pDataContext->volume_data.isValidate()) || g_Renderer->isAvailableVolumeRender() == false)
	{
		if (m_btnPlanecut)
			m_btnPlanecut->setChecked(!checked);
		return;
	}

	if (m_btnPlanecut->isChecked())
	{
		setWorkMode(ANAL_WORK_3D_PLANE_SPLIT,true);
		WIN_MANAGER->clipOnOff(false);
		WIN_MANAGER->setClipOpt();
	}
	else
		setWorkMode(ANAL_WORK_NONE, true);
}

void AnalVolumeView::slot_OnCurveSplit()
{
	if ((!m_pDataContext->volume_data.isValidate()) || g_Renderer->isAvailableVolumeRender() == false)
	{
		if (m_btnSplit)
			m_btnPlanecut->setChecked(!m_btnSplit->isChecked());
		return;
	}

	if (m_btnSplit->isChecked())
		setWorkMode(ANAL_WORK_3D_CURVE_SPLIT, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);
}

void AnalVolumeView::slot_OnFreeDrawROI()
{
	if ((!m_pDataContext->volume_data.isValidate()) || g_Renderer->isAvailableVolumeRender() == false)
	{
		m_btnFreeDrawROI->setChecked(!m_btnFreeDrawROI->isChecked());
		return;
	}

	if (m_btnFreeDrawROI->isChecked())
		setWorkMode(ANAL_WORK_SEGMENT_FREEDRAW, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);
}

void AnalVolumeView::slot_OnCalcDistance()
{
	if (m_btnDistance->isChecked())
		setWorkMode(ANAL_WORK_SEGMENT_CALC_DISTANCE, true);
	else
		setWorkMode(ANAL_WORK_NONE, true);
}

//bool AnalVolumeView::eventFilter(QObject * target, QEvent * e)
//{
//	return QWidget::eventFilter(target, e);
//}

void AnalVolumeView::render(QPainter *p)
{
	if (p == NULL) 
		return;

	// Update OpenGL Resource
	WIN_MANAGER->preRenderProcess();

	if (m_pDataContext->volume_data.isValidate() == false || WIN_MANAGER->getRenderable() == false || WIN_MANAGER->IsCropOn())
	{
		QColor background = RESOURCE_MANAGER->getUnloadBackGroundColor();

		if (m_labelImage == NULL)
		{
			m_labelImage = new QLabel(this);

			QImage * unloadImage = RESOURCE_MANAGER->getUnloadImage(WT_VOLUME);

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

		return;
	}

	if (m_labelImage != NULL) m_labelImage->hide();

	renderScene(p);

	drawMouseWork(p);

	drawAnnotations(p);

	// TODO Draw Painter ( GDI )
	QPen prePen = p->pen();

	p->setPen(QPen(QColor(62, 137, 219)));
	QFontMetrics fontMet(p->font());
	QString str = getMoveMode();
	QRect rect = fontMet.boundingRect(str);
	int textX = 0;
	if (!m_btnFullScreen->isHidden())
		textX += m_btnFullScreen->width();

	p->drawText(textX, this->height() - rect.height(), str);

	p->setPen(prePen);


	if (!WIN_MANAGER->IsLicensePass())
	{
		QImage * WaterImage = RESOURCE_MANAGER->getWaterMark();
		if (WaterImage != NULL)
		{
			p->drawImage(QPoint(this->width() / 2 - WaterImage->size().width() / 2, (this->height() / 2) + 100), *WaterImage);
		}
	}

	// mesh rendering
}

void AnalVolumeView::drawAnnotations(QPainter * p)
{
	mip::MATRIX44 matView = m_camera.getView();
	mip::MATRIX44 matProj = m_camera.getProj();
	mip::MATRIX44 matWorld = m_trVolumModel.getMatrix();

	std::vector<mip::VECTOR3> list;

	if (WIN_MANAGER->get3DAnnoState())
	{
		for (auto ano = WIN_MANAGER->anotationList.begin(); ano != WIN_MANAGER->anotationList.end(); ++ano)
		{
			if ((*ano)->isAnnoHidden()) 
				continue;

			if ((*ano)->getType() == AT_LEN)
			{
				if (!WIN_MANAGER->bShowAnnoLength) 
					continue;

				AnnoLength *annoLen = static_cast<AnnoLength*>(*ano);
				annoLen->drawAnno(this, p, ADV_MEASUREMENT_VOLUME_VIEWER);
			}
			else if ((*ano)->getType() == AT_TEXT)
			{
				if (!WIN_MANAGER->bShowAnnoText) 
					continue;

				AnnoString *annoStr = static_cast<AnnoString*>(*ano);
				annoStr->drawAnno(this, p, ADV_MEASUREMENT_VOLUME_VIEWER);
			}
			else if ((*ano)->getType() == AT_ANGLE)
			{
				if (!WIN_MANAGER->bShowAnnoAngle) 
					continue;

				AnnoAngle *annoAngle = static_cast<AnnoAngle*>(*ano);
				annoAngle->drawAnno(this, p, ADV_MEASUREMENT_VOLUME_VIEWER);
			}
			else if ((*ano)->getType() == AT_ARROW)
			{
				if (!WIN_MANAGER->bShowAnnoArrow) 
					continue;
				AnnoArrow *annoArrow = static_cast<AnnoArrow*>(*ano);
				annoArrow->drawAnno(this, p, ADV_MEASUREMENT_VOLUME_VIEWER);
			}
			else if ((*ano)->getType() == AT_RECTANGLE)
			{
				if (!WIN_MANAGER->bShowAnnoRectangle)
					continue;
				AnnoRectangle *annoRect = static_cast<AnnoRectangle*>(*ano);
				annoRect->drawAnno(this, p, ADV_MEASUREMENT_VOLUME_VIEWER);
			}
		}
	}

	if (m_vecVolumePoint.size() != 0)
	{
		mip::MATRIX44 matView = getCamera().getView();
		mip::MATRIX44 matProj = getCamera().getProj();
		mip::MATRIX44 matWorld = getWorld().getMatrix();

		QColor qcolor = WIN_MANAGER->annoColor;

		QBrush preBrush = p->brush();
		QPen prePen = p->pen();
		for (int i = 0; i < m_vecVolumePoint.size(); i++)
		{
			mip::VECTOR3 v1 = mip::geom::WorldToScreen(m_vecVolumePoint[i], this->width(), this->height(), matView, matProj, &matWorld);
			p->setPen(qcolor);
			p->setBrush(qcolor);
			p->drawEllipse(QPoint(v1.x, v1.y), 2, 2);
		}
		p->setPen(prePen);
		p->setBrush(preBrush);
	}
}

//void AnalVolumeView::drawProfiles(QPainter * p)
//{
//	if (NULL == p) return;
//
//	mip::MATRIX44 matView = m_camera.getView();
//	mip::MATRIX44 matProj = m_camera.getProj();
//	mip::MATRIX44 matWorld = m_trVolumModel.getMatrix();
//
//	std::vector<mip::VECTOR3> list;
//
//	for (auto pro = WIN_MANAGER->profileList.begin(); pro != WIN_MANAGER->profileList.end(); ++pro)
//	{
//		if ((*pro)->isProHidden()) continue;
//
//		if ((*pro)->getType() == AT_PT_DRAW || (*pro)->getType() == AT_PT_POLY)
//		{
//			ProDraw *proDraw = dynamic_cast<ProDraw*>(*pro);
//
//			if (proDraw)
//				proDraw->drawProfile(this, p, 1);
//		}
//	}
//
//}

void AnalVolumeView::drawMouseWork(QPainter *p)
{
	if (NULL == p) return;

	bool focusWin = WIN_MANAGER->getLatestActiveViewType() == m_windowType;

	switch (WIN_MANAGER->getAnalWorkMode())
	{

	case ANAL_WORK_PATH:
	case ANAL_WORK_PATH_3D_PLAY:
	{
		int _group = WIN_MANAGER->getAnimationGroup();
		if (WIN_MANAGER->aniCount.at(_group) <= 0) return;
		if (m_updateFrameSet) return;
		bool bInArea = false;

		VOLUME_DATA & vd = m_pDataContext->volume_data;
		mip::MATRIX44 matView = m_camera.getView();
		mip::MATRIX44 matProj = m_camera.getProj();
		mip::MATRIX44 matWorld = getWorld().getMatrix();
		float cx, cy, cz;
		AnimationTab* tab = WIN_MANAGER->GetTab()->getAniTab();
		bool bSpiral = tab ? tab->isSpiral() : false;
		QVector<QVector3D> *Lines = bSpiral ? &(WIN_MANAGER->spiralLine) : &(WIN_MANAGER->aniLine);
		QPolygonF polyLines;
		p->setPen(QPen(Qt::darkRed, 2));
		for (int i = 0; i < Lines->count(); i++)
		{
			bInArea = false;

			QVector3D vec = Lines->at(i);

			cx = vd.getSpaceX()*vec.x() - vd.getSizeX()*0.5f;
			cy = vd.getSpaceY()*vec.y() - vd.getSizeY()*0.5f;
			cz = vd.getSpaceZ()*vec.z() - vd.getSizeZ()*0.5f;

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

			cx = vd.getSpaceX()*vec.x() - vd.getSizeX()*0.5f;
			cy = vd.getSpaceY()*vec.y() - vd.getSizeY()*0.5f;
			cz = vd.getSpaceZ()*vec.z() - vd.getSizeZ()*0.5f;

			vec = QVector3D(cx, cy, cz);

			mip::VECTOR3 _v1 = mip::VECTOR3(vec.x(), vec.y(), vec.z());
			mip::VECTOR3 v1 = mip::geom::WorldToScreen(_v1, this->width(), this->height(), matView, matProj, &matWorld);
			p->drawEllipse(QPointF(v1.x, v1.y), 2, 2);
		}
		if (WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_PATH_3D_PLAY)
		{
			p->setPen(QPen(Qt::green, 3));
			QVector3D pos = WIN_MANAGER->LinePos;
			cx = vd.getSpaceX()*pos.x() - vd.getSizeX()*0.5f;
			cy = vd.getSpaceY()*pos.y() - vd.getSizeY()*0.5f;
			cz = vd.getSpaceZ()*pos.z() - vd.getSizeZ()*0.5f;

			pos = QVector3D(cx, cy, cz);
			mip::VECTOR3 _v1 = mip::VECTOR3(pos.x(), pos.y(), pos.z());
			mip::VECTOR3 v1 = mip::geom::WorldToScreen(_v1, this->width(), this->height(), matView, matProj, &matWorld);

			p->drawEllipse(QPointF(v1.x, v1.y), 2, 2);
		}
	}
	break;
	case ANAL_WORK_SEGMENT_FREEDRAW:
	case ANAL_WORK_3D_CURVE_SPLIT:
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
	case ANAL_WORK_CAPTURE:
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

		if (m_polyLine.size() >= 2)
		{
			p->setPen(QPen(Qt::red, 1.5));

			QRect rect;

			rect.setCoords(m_polyLine.at(0).x(), m_polyLine.at(0).y(),
				m_polyLine.at(1).x(), m_polyLine.at(1).y());

			p->drawRect(rect);

		}
	}
	break;
	}
}

muint32 AnalVolumeView::createTextureFromQImage(QImage * image)
{
	int xLenth = image->width();
	int yLenth = image->height();

	mip::COLOR * pTextureBuffer = new mip::COLOR[yLenth*xLenth];
	mip::COLOR * pRowTexture = NULL;
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

	muint32 imageTexture = g_Renderer->createTextureGL(xLenth, yLenth, mip::TEXT_USER, pTextureBuffer);

	SAFE_DELETES(pTextureBuffer);

	return imageTexture;
}

mip::VECTOR3 AnalVolumeView::getLocalToScreen(float x, float y, float z)
{
	return m_camera.getScreenPoint(mip::VECTOR3(x, y, z), &m_trVolumModel.getMatrix());
}

mip::VECTOR3 AnalVolumeView::getScreenToProj(float x, float y)
{
	return mip::geom::Screen2Proj(x, y, this->width(), this->height());
}

mip::MATRIX44 AnalVolumeView::getWVP()
{
	return m_trVolumModel.getMatrix() * m_camera.getView() * m_camera.getProj();
}



void AnalVolumeView::setWorkMode(ANAL_WORK_MODE mode, bool iconRefresh)
{
	static QIcon freeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FREEDRAWROI, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_VIEW_FREEDRAWROI, m_nIconSize, m_nIconSize) };
	static QIcon planeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_PLANE_CUT, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_PLANE_CUT, m_nIconSize, m_nIconSize) };
	static QIcon splitIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_SPLIT_CURVE, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_SPLIT_CURVE, m_nIconSize, m_nIconSize) };
	static QIcon distIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_LENGTH, m_nIconSize, m_nIconSize) ,
		RESOURCE_MANAGER->getIcon(ICON_ANNO_LENGTH, m_nIconSize, m_nIconSize) };

	ANAL_WORK_MODE prMode = WIN_MANAGER->getAnalWorkMode();

	WIN_MANAGER->setAnalWorkMode(mode);

	m_polyLine.clear();

	if (prMode != mode)
	{
		switch (prMode)
		{
		case ANAL_WORK_SEGMENT_FREEDRAW:
		case ANAL_WORK_3D_CURVE_SPLIT:
			break;
		case ANAL_WORK_3D_PLANE_SPLIT:
			WIN_MANAGER->setClipOpt();
			break;
		case ANAL_WORK_CAPTURE:
			if (WIN_MANAGER->GetTab()->getCaptureTab())
				WIN_MANAGER->GetTab()->getCaptureTab()->cancelCapture();
			break;
		}
	}

	m_cursor = Qt::ArrowCursor;

	bool chkMode = false;
	bool chkRefresh = false;

	if (chkRefresh = ((mode == ANAL_WORK_SEGMENT_FREEDRAW) != (chkMode = m_btnFreeDrawROI->isChecked())))
	{
		chkMode = !chkMode;
		m_btnFreeDrawROI->setChecked(chkMode);
		m_btnFreeDrawROI->setIcon(freeIcon[chkMode]);
	}
	else if (!chkRefresh && iconRefresh)
		m_btnFreeDrawROI->setIcon(freeIcon[chkMode]);
	
	updateToolButtonIcon_As_WorkMode(m_btnFreeDrawROI, ANAL_WORK_SEGMENT_FREEDRAW, mode, freeIcon, iconRefresh);

	if (chkRefresh = ((mode == ANAL_WORK_SEGMENT_CALC_DISTANCE) != (chkMode = m_btnDistance->isChecked())))
	{
		chkMode = !chkMode;
		m_btnDistance->setChecked(chkMode);
		m_btnDistance->setIcon(distIcon[chkMode]);
	}
	else if (!chkRefresh && iconRefresh)
		m_btnDistance->setIcon(distIcon[chkMode]);

	if (chkRefresh = ((mode == ANAL_WORK_3D_PLANE_SPLIT) != (chkMode = m_btnPlanecut->isChecked())))
	{
		chkMode = !chkMode;
		m_btnPlanecut->setChecked(chkMode);
		m_btnPlanecut->setIcon(planeIcon[chkMode]);
	}
	else if (!chkRefresh && iconRefresh)
		m_btnPlanecut->setIcon(planeIcon[chkMode]);
	
	if (chkRefresh = ((mode == ANAL_WORK_3D_CURVE_SPLIT) != (chkMode = m_btnSplit->isChecked())))
	{
		chkMode = !chkMode;
		m_btnSplit->setChecked(chkMode);
		m_btnSplit->setIcon(splitIcon[chkMode]);
	}
	else if (!chkRefresh && iconRefresh)
		m_btnSplit->setIcon(splitIcon[chkMode]);

	switch (mode)
	{
	case ANAL_WORK_CAPTURE:
	{
		bool _mode = WIN_MANAGER->GetTab()->getCaptureTab()->getCaptureMode();

		if (_mode)
			m_cursor = Qt::PointingHandCursor;
		else
			m_cursor = Qt::CrossCursor;
	}
	break;
	case ANAL_WORK_SEGMENT_FREEDRAW:
	case ANAL_WORK_SEGMENT_CALC_DISTANCE:
	case ANAL_WORK_3D_PLANE_SPLIT:
	case ANAL_WORK_3D_CURVE_SPLIT:
		break;
	default:
		m_bWorkMode = false;
		break;
	}

	this->setCursor(m_cursor);

	WIN_MANAGER->renderLater_GridView(true);
}

QImage AnalVolumeView::getCaptureImage(bool mode, bool type)
{
	QImage img = QImage(this->width(), this->height(), QImage::Format_RGBA8888);

	QPainter p(&img);
	p.fillRect(QRect(0, 0, this->width(), this->height()), Qt::black);
	p.drawImage(0, 0, m_img);

	if (type)
	{
		drawAnnotations(&p);

		QFontMetrics fontMet(p.font());
		QString str = getMoveMode();
		QRect rect = fontMet.boundingRect(str);

		p.setPen(QPen(QColor(62, 137, 219)));
		p.drawText(0, this->height() - rect.height(), str);
	}

	if (mode)
	{
		/*if (!WIN_MANAGER->IsLicensePass())
		{
			QImage * WaterImage = RESOURCE_MANAGER->getWaterMark();
			if (WaterImage != NULL)
			{
				QImage drawImg = WaterImage->scaled(QSize(this->width(), this->height() / 3), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				p.drawImage(QPoint(0, this->height() / 4), drawImg);
			}
		}*/

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

	return QImage();
}

void AnalVolumeView::setImgTranslation(mip::VECTOR3 translation)
{
	mip::MATRIX44 matVolume = m_trVolumModel.getMatrix();

	m_trPlaneImage.addTranslate(translation);

	TransformRotate(&m_trPlaneImage, &m_camera, &(matVolume));
}

void AnalVolumeView::clip2DOff()
{
	if (m_nClip != CT_NONE)
	{
		if (m_nClip & CT_AXIAL)
		{
			actAxial->setChecked(false);
			slot_OnAxial();
		}
		if (m_nClip & CT_CORONAL)
		{
			actCoronal->setChecked(false);
			slot_OnCoronal();
		}
		if (m_nClip & CT_SAGITTAL)
		{
			actSagittal->setChecked(false);
			slot_OnSagittal();
		}
	}
}

bool AnalVolumeView::getClip2DPlane()
{
	return m_nClip != CT_NONE;
}

/*@function useSkipRender
**@brief render skip mode setting
**@param index skip mode type (0:wl change, 1: object move, 2: object zoom, 3: alpha change)
**@param res set/unset (default : set(true))
*/
void AnalVolumeView::useSkipRender(int index, bool res)
{
	if (index < 0 || index > 3) return;

	if (m_bSkip[index] != res)
		m_bSkip[index] = res;
}


bool AnalVolumeView::pickingMask(const mip::VECTOR2 & point, /*out*/ mip::VECTOR3 & result, std::vector<std::pair<int, mask8>> &vecHitMaskBit)
{
	mip::MATRIX44 matWorld = m_trVolumModel.getMatrix();

	mip::RAY ray = mip::geom::ScreenToRay(point, this->size().width(), this->size().height(),
		m_camera.getView(), m_camera.getProj(), &matWorld);

	mip::VECTOR3 vHitPoint;
	bool bPickingMask = false;

	std::vector<mask8> vecMaskBitAll, vecMaskShowFlag;
	int nMaskCnt = m_pDataContext->volume_data.getMaskIndexCnt();
	for (int i = 0; i < nMaskCnt; ++i)
	{
		vecMaskShowFlag.push_back(0);
		mask8 maskBit = 0x01;
		for (int j = 0; j < 8; ++j)
		{
			MaskInfo* pMaskInfo = m_pDataContext->volume_data.findMaskInfo(i, maskBit);
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

	bPickingMask = WIN_MANAGER->volume_renderer.ray_MaskCheck(ray, m_pDataContext->volume_data.getAllMaskDataPoint(), vecMaskShowFlag
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

void AnalVolumeView::touchMove(mint32 preX, mint32 preY, mint32 currX, mint32 currY)
{
	mip::VECTOR3 v1 = m_camera.getWorldPoint(preX, preY, 0);
	mip::VECTOR3 v2 = m_camera.getWorldPoint(currX, currY, 0);

	useSkipRender(1, true);

	m_trVolumModel.addTranslate(v2 - v1);
	renderLater();
}

void AnalVolumeView::touchZoom(float dt)
{
	m_camera.wheelZoom(dt*0.1f);
	useSkipRender(2);
	renderLater();
}

QString AnalVolumeView::getMoveMode()
{
	QString str;

	if (WIN_MANAGER->isClipMode())
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

void AnalVolumeView::updateToolButtonIcon_As_WorkMode(QPushButton * button, ANAL_WORK_MODE targetWorkMode, ANAL_WORK_MODE mode, QIcon icon[], bool iconRefresh)
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

