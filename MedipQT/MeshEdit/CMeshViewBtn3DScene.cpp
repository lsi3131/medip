#include "stdafx.h"
#include "CMeshViewBtn3DScene.h"
#include "MedipQT.h"
#include "WindowManager.h"
#include "Main/MainMeshWidget.h"
#include "MEVolumeView.h"
#include "ActionManager.h"
#include "styleManager.h"
#include "stringManager.h"

#include "Dialogs/SizeDialog.h"

#include "CMeshWorkManager.h"
#include "CMeshCutManager.h"
#include "CMeshModelViewManager.h"
#include "CMeshDlgManager.h"
#include "CMeshDistancMananager.h"
#include "CMeshHoleFillManager.h"
#include "CManipulator.h"

#include "DataContext.h"

#include <QAction>
#include <QWidget>

/*
@brief
*/
CMeshViewBtn3DScene::CMeshViewBtn3DScene()
	: m_BtnZero(0),
	m_BtnSphere(0),
	m_BtnCube(0),
	m_BtnCylin(0),
	m_BtnDistance(0),
	m_BtnCut(0),
	m_BtnPlane(0),
	m_BtnPoly(0),
	//m_BtnColor(0),
	m_SliderTrans(0),
	HoverWidget(0)
{
}

/*
@brief
*/
CMeshViewBtn3DScene::~CMeshViewBtn3DScene()
{

}

CMeshViewBtn3DScene* CMeshViewBtn3DScene::getInstance()
{
	static CMeshViewBtn3DScene instance;
	return &instance;
}

void CMeshViewBtn3DScene::Init(DataContext* pDataContext, MEVolumeView* pViewer)
{
	Init(pDataContext,
		WIN_MANAGER,
		pViewer,
		MESH_CUT_MANAGER,
		MESH_DIALOG_MANAGER,
		MESH_WORK_MANAGER,
		PLANE_MANIPULATOR,
		MESH_MODELVIEW_MANAGER,
		MESH_DISTANCE_MANAGER,
		MESH_HOLE_MANAGER
	);
}

void CMeshViewBtn3DScene::Init(
	DataContext* pDataContext,
	WindowManager* pWinManager,
	MEVolumeView* pViewer,
	CMeshCutManager* pMeshCutManager,
	CMeshDlgManager* pDlgManager,
	CMeshWorkManager* pWorkManager,
	CPlaneManiplator* pPlaneManipulator,
	CMeshModelViewManager* pModelViewManager,
	CMeshDistancMananager* pDistanceManager,
	CMeshHoleFillManager* pHoleFillManager)
{
	m_pDataContext = pDataContext;
	m_pWinManager = pWinManager;
	m_pViewer = pViewer;

	m_pMeshCutManager = pMeshCutManager;
	m_pDlgManager = pDlgManager;
	m_pWorkManager = pWorkManager;
	m_pPlaneManipulator = pPlaneManipulator;
	m_pModelViewManager = pModelViewManager;
	m_pDistanceManager = pDistanceManager;
	m_pHoleFillManager = pHoleFillManager;

	m_ActZero = new QAction(QString("Recalc Zero"), m_pViewer);

	connect(m_ActZero, &QAction::triggered, this, &CMeshViewBtn3DScene::OnReCalcZero);

	initInScreenRightMenu();
}


void CMeshViewBtn3DScene::initInScreenRightMenu()
{
	if (m_pWinManager->mainWindow == nullptr)
	{
		return;
	}

	if (NULL == m_BtnZero)
	{
		int sz_icon = m_pWinManager->mainWindow->IconSize;

		m_BtnZero = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_MESH_ZERO, sz_icon, sz_icon), "", m_pViewer);
		m_BtnZero->setIconSize(QSize(34, 34));
		m_BtnZero->setFixedSize(QSize(34, 34));
		m_BtnZero->setCheckable(true);
		m_BtnZero->setMouseTracking(true);
		m_BtnZero->installEventFilter(m_pViewer);
		//	m_BtnZero->hide();
		m_BtnZero->setStyleSheet("color: black;");
		m_BtnZero->setToolTip("Pivot point to center of object");

		connect(m_BtnZero, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnReCalcZero);

		m_BtnCube = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_MESH_CUBE, sz_icon, sz_icon), "", m_pViewer);
		m_BtnCube->setIconSize(QSize(34, 34));
		m_BtnCube->setFixedSize(QSize(34, 34));
		m_BtnCube->setCheckable(true);
		m_BtnCube->setMouseTracking(true);
		m_BtnCube->installEventFilter(m_pViewer);
		//m_BtnCube->hide();
		m_BtnCube->setStyleSheet("color: black;");
		m_BtnCube->setToolTip("Create cube mesh");

		connect(m_BtnCube, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnCreateCube);

		m_BtnCylin = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_MESH_CYLINDER, sz_icon, sz_icon), "", m_pViewer);
		m_BtnCylin->setIconSize(QSize(34, 34));
		m_BtnCylin->setFixedSize(QSize(34, 34));
		m_BtnCylin->setCheckable(true);
		m_BtnCylin->setMouseTracking(true);
		m_BtnCylin->installEventFilter(m_pViewer);
		//m_BtnCylin->hide();
		m_BtnCylin->setStyleSheet("color: black;");
		m_BtnCylin->setToolTip("Create cylinder mesh");

		connect(m_BtnCylin, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnCreateCylinder);

		m_BtnSphere = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_MESH_SPHERE, sz_icon, sz_icon), "", m_pViewer);
		m_BtnSphere->setIconSize(QSize(34, 34));
		m_BtnSphere->setFixedSize(QSize(34, 34));
		m_BtnSphere->setCheckable(true);
		m_BtnSphere->setMouseTracking(true);
		m_BtnSphere->installEventFilter(m_pViewer);
		//m_BtnSphere->hide();
		m_BtnSphere->setStyleSheet("color: black;");
		m_BtnSphere->setToolTip("Create sphere mesh");

		connect(m_BtnSphere, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnCreateSphere);

		QMenu* clipMenu = new QMenu(m_pViewer);
		clipMenu->installEventFilter(m_pViewer);

		auto act_plane = m_pMeshCutManager->getActionPtr(MESH_WORK_PLANE_CUT);
		auto act_polygon = m_pMeshCutManager->getActionPtr(MESH_WORK_POLYGON_CUT);
		auto act_polyline = m_pMeshCutManager->getActionPtr(MESH_WORK_POLYLINE_CUT);
		auto act_free_poly = m_pMeshCutManager->getActionPtr(MESH_WORK_FREEPOLYLINE_CUT);

		clipMenu->addAction(act_plane);
		clipMenu->addAction(act_polygon);
		clipMenu->addAction(act_polyline);
		clipMenu->addAction(act_free_poly);

		m_BtnCut = new QToolButton(m_pViewer);
		m_BtnCut->setPopupMode(QToolButton::InstantPopup);
		m_BtnCut->setIcon(RESOURCE_MANAGER->getIcon(ICON_NON_PLANE_CUT, sz_icon, sz_icon));
		m_BtnCut->setMenu(clipMenu);
		m_BtnCut->setIconSize(QSize(34, 34));
		m_BtnCut->setFixedSize(QSize(34, 34));
		m_BtnCut->setMouseTracking(true);
		m_BtnCut->installEventFilter(m_pViewer);
		m_BtnCut->setStyleSheet("QToolButton::menu-indicator { image: none; }");
		m_BtnCut->setToolTip("Mesh cut functions (Plane, Polygon, Polyline)");

		m_BtnPlane = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_PLANE_CUT, sz_icon, sz_icon), "", m_pViewer);
		m_BtnPlane->setIconSize(QSize(34, 34));
		m_BtnPlane->setFixedSize(QSize(34, 34));
		m_BtnPlane->setCheckable(true);
		m_BtnPlane->setMouseTracking(true);
		m_BtnPlane->installEventFilter(m_pViewer);
		m_BtnPlane->hide();
		m_BtnPlane->setStyleSheet("color: black;");
		m_BtnPlane->setToolTip("Plane cut");

		connect(m_BtnPlane, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnPlanecut);

		m_BtnPoly = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYROI, sz_icon, sz_icon), "", m_pViewer);
		m_BtnPoly->setIconSize(QSize(34, 34));
		m_BtnPoly->setFixedSize(QSize(34, 34));
		m_BtnPoly->setCheckable(true);
		m_BtnPoly->setMouseTracking(true);
		m_BtnPoly->installEventFilter(m_pViewer);
		m_BtnPoly->hide();
		m_BtnPoly->setStyleSheet("color: black;");
		m_BtnPoly->setToolTip("Polygon cut");

		connect(m_BtnPoly, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnPolyInnercut);

		m_BtnLine = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_PROF_LINE, sz_icon, sz_icon), "", m_pViewer);
		m_BtnLine->setIconSize(QSize(34, 34));
		m_BtnLine->setFixedSize(QSize(34, 34));
		m_BtnLine->setCheckable(true);
		m_BtnLine->setMouseTracking(true);
		m_BtnLine->installEventFilter(m_pViewer);
		m_BtnLine->hide();
		m_BtnLine->setStyleSheet("color: black;");
		m_BtnLine->setToolTip("Poly-Line cut");

		connect(m_BtnLine, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnPolyLinecut);

		m_BtnPolyFree = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYFREE, sz_icon, sz_icon), "", m_pViewer);
		m_BtnPolyFree->setIconSize(QSize(34, 34));
		m_BtnPolyFree->setFixedSize(QSize(34, 34));
		m_BtnPolyFree->setCheckable(true);
		m_BtnPolyFree->setMouseTracking(true);
		m_BtnPolyFree->installEventFilter(m_pViewer);
		m_BtnPolyFree->hide();
		m_BtnPolyFree->setStyleSheet("color: black;");
		m_BtnPolyFree->setToolTip("Poly-Free cut");

		connect(m_BtnPolyFree, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnPolyFreecut);

		m_BtnDistance = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_LENGTH, sz_icon, sz_icon), "", m_pViewer);
		m_BtnDistance->setIconSize(QSize(34, 34));
		m_BtnDistance->setFixedSize(QSize(34, 34));
		m_BtnDistance->setCheckable(true);
		m_BtnDistance->setMouseTracking(true);
		m_BtnDistance->installEventFilter(m_pViewer);
		//m_BtnPoly->hide();
		m_BtnDistance->setStyleSheet("color: black;");
		m_BtnDistance->setToolTip("Length between two points of mesh");
		m_BtnDistance->setVisible(false); //20210112_byPHS_들어가있지 않은 기능 버튼 Hide

		connect(m_BtnDistance, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnDistance);

		//m_BtnColor = new QPushButton(QString("C"), m_pViewer);
		//m_BtnColor->setFixedSize(QSize(34, 34));
		//m_BtnColor->setCheckable(false);
		//m_BtnColor->setMouseTracking(true);
		//m_BtnColor->installEventFilter(m_pViewer);
		//m_BtnColor->setStyleSheet(STYLE_MANAGER->buttonBehind);
		//m_BtnColor->hide();
		//m_BtnColor->setStyleSheet("color: black;");
		//m_BtnColor->setToolTip("Change mesh color");

		//connect(m_BtnColor, &QPushButton::clicked, MESH_MODELVIEW_MANAGER, &CMeshModelViewManager::OnColorChange);

		m_SliderTrans = new QSlider(Qt::Horizontal, m_pViewer);
		m_SliderTrans->setRange(0, 100);
		m_SliderTrans->setValue(100);
		m_SliderTrans->hide();
		m_SliderTrans->setStyleSheet("background-color: rgba(125, 125, 125, 0%)");
		connect(m_SliderTrans, &QSlider::valueChanged, this, &CMeshViewBtn3DScene::OnOpacityChange);

		m_pLabelOpacity = new QLabel("Opacity", m_pViewer);
		m_pLabelOpacity->setStyleSheet("background-color: rgba(125, 125, 125, 0%)");
		m_pLabelOpacity->setAlignment(Qt::AlignLeft);
		m_pLabelOpacity->hide();

		m_BtnSculpt = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_BRUSH_SCULPT, sz_icon, sz_icon), "", m_pViewer);
		m_BtnSculpt->setIconSize(QSize(34, 34));
		m_BtnSculpt->setFixedSize(QSize(34, 34));
		m_BtnSculpt->setCheckable(true);
		m_BtnSculpt->setMouseTracking(true);
		m_BtnSculpt->installEventFilter(m_pViewer);
		m_BtnSculpt->setStyleSheet("color: black;");
		m_BtnSculpt->setToolTip("Brush Sculpt");

		connect(m_BtnSculpt, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnSculpt);

		m_BtnSelect = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_BRUSH_SELECT, sz_icon, sz_icon), "", m_pViewer);
		m_BtnSelect->setIconSize(QSize(34, 34));
		m_BtnSelect->setFixedSize(QSize(34, 34));
		m_BtnSelect->setCheckable(true);
		m_BtnSelect->setMouseTracking(true);
		m_BtnSelect->installEventFilter(m_pViewer);
		m_BtnSelect->setStyleSheet("color: black;");
		m_BtnSelect->setToolTip("Brush Select");

		connect(m_BtnSelect, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnSelect);

		m_BtnFillHole = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_HOLEFILL, sz_icon, sz_icon), "", m_pViewer);
		m_BtnFillHole->setIconSize(QSize(34, 34));
		m_BtnFillHole->setFixedSize(QSize(34, 34));
		m_BtnFillHole->setCheckable(true);
		m_BtnFillHole->setMouseTracking(true);
		m_BtnFillHole->installEventFilter(m_pViewer);
		m_BtnFillHole->setStyleSheet("color: black;");
		m_BtnFillHole->setToolTip("Fill hole");

		connect(m_BtnFillHole, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnFillHoleSelected);

#if SUPPORT_STAMP3D == 1
		m_BtnStamp3d = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_BRUSH_SELECT, sz_icon, sz_icon), "", m_pViewer);
		m_BtnStamp3d->setIconSize(QSize(34, 34));
		m_BtnStamp3d->setFixedSize(QSize(34, 34));
		m_BtnStamp3d->setCheckable(true);
		m_BtnStamp3d->setMouseTracking(true);
		m_BtnStamp3d->installEventFilter(m_pViewer);
		m_BtnStamp3d->setStyleSheet("color: black;");
		m_BtnStamp3d->setToolTip("Stamp3D");

		connect(m_BtnStamp3d, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnStamp3D);
#endif
#if SUPPORT_MATCAP == 1
		m_BtnShader = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_BRUSH_SELECT, sz_icon, sz_icon), "", m_pViewer);
		m_BtnShader->setIconSize(QSize(34, 34));
		m_BtnShader->setFixedSize(QSize(34, 34));
		m_BtnShader->setCheckable(true);
		m_BtnShader->setMouseTracking(true);
		m_BtnShader->installEventFilter(m_pViewer);
		m_BtnShader->setStyleSheet("color: black;");
		m_BtnShader->setToolTip("Shader");

		connect(m_BtnShader, &QPushButton::clicked, this, &CMeshViewBtn3DScene::OnShader);
#endif
	}
}

void CMeshViewBtn3DScene::updateScreenLeftMenu(int _width, int _height)
{
	int offsetRY = 0;
	int offsetLY = 0;
	if (m_BtnZero)
	{
		m_BtnCube->move(_width - m_BtnCube->width(), offsetRY);
		offsetRY += m_BtnCube->height();
		m_BtnCylin->move(_width - m_BtnCylin->width(), offsetRY);
		offsetRY += m_BtnCylin->height();
		m_BtnSphere->move(_width - m_BtnSphere->width(), offsetRY);
		offsetRY += m_BtnSphere->height();

		offsetRY += 5;

		m_BtnCut->move(_width - m_BtnCut->width(), offsetRY);
		offsetRY += m_BtnCut->height();

		m_BtnPlane->move(_width - m_BtnPlane->width(), offsetRY);
		if (!m_BtnPlane->isHidden()) offsetRY += m_BtnPlane->height();
		m_BtnPoly->move(_width - m_BtnPoly->width(), offsetRY);
		if (!m_BtnPoly->isHidden()) offsetRY += m_BtnPoly->height();
		m_BtnLine->move(_width - m_BtnLine->width(), offsetRY);
		if (!m_BtnLine->isHidden()) offsetRY += m_BtnLine->height();
		m_BtnPolyFree->move(_width - m_BtnPolyFree->width(), offsetRY);
		if (!m_BtnPolyFree->isHidden()) offsetRY += m_BtnPolyFree->height();

		//20210112_byPHS_들어가있지 않은 기능 주석
		//offsetRY += 5;
		//m_BtnDistance->move(_width - m_BtnDistance->width(), offsetRY);
		//offsetRY += m_BtnDistance->height();

		offsetRY += 5;
		m_BtnSculpt->move(_width - m_BtnSculpt->width(), offsetRY);
		offsetRY += m_BtnSculpt->height();

		offsetRY += 5;
		m_BtnFillHole->move(_width - m_BtnFillHole->width(), offsetRY);
		offsetRY += m_BtnFillHole->height();

		offsetRY += 5;
		m_BtnSelect->move(_width - m_BtnSelect->width(), offsetRY);
		offsetRY += m_BtnSelect->height();

#if SUPPORT_STAMP3D == 1
		offsetRY += 5;
		m_BtnStamp3d->move(_width - m_BtnStamp3d->width(), offsetRY);
		offsetRY += m_BtnStamp3d->height();
#endif

#if SUPPORT_MATCAP == 1
		offsetRY += 5;
		m_BtnShader->move(_width - m_BtnShader->width(), offsetRY);
		offsetRY += m_BtnShader->height();
#endif

		int Right = 1;
		m_pDlgManager->MoveMeshDialog();

		m_BtnZero->move(_width - m_BtnZero->width(), _height - m_BtnZero->height());

		//m_BtnColor->move(0, offsetLY);
		//if (!m_BtnColor->isHidden()) offsetLY += m_BtnColor->height();

		int offset_rx;
		offset_rx = _width - m_SliderTrans->width() + 1;
		offsetRY += 10;
		m_pLabelOpacity->move(offset_rx, offsetRY);

		offsetRY += 20;
		offset_rx = _width - m_SliderTrans->width();
		m_SliderTrans->move(offset_rx, offsetRY);
		offsetLY += m_SliderTrans->height();

	}
}

bool CMeshViewBtn3DScene::eventFilter(QObject* target, QEvent* e)
{
	if (m_pWinManager->mainWindow == nullptr)
	{
		return QObject::eventFilter(target, e);
	}

	int sz_icon = m_pWinManager->mainWindow->IconSize;

	static QIcon cubeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_MESH_CUBE, sz_icon, sz_icon) ,
 RESOURCE_MANAGER->getIcon(ICON_MESH_CUBE_HOVER, sz_icon, sz_icon) };
	static QIcon sphereIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_MESH_SPHERE, sz_icon, sz_icon) ,
 RESOURCE_MANAGER->getIcon(ICON_MESH_SPHERE_HOVER, sz_icon, sz_icon) };
	static QIcon cylinIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_MESH_CYLINDER, sz_icon, sz_icon) ,
 RESOURCE_MANAGER->getIcon(ICON_MESH_CYLINDER_HOVER, sz_icon, sz_icon) };
	static QIcon planeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_PLANE_CUT, sz_icon, sz_icon) ,
 RESOURCE_MANAGER->getIcon(ICON_PLANE_CUT_HOVER, sz_icon, sz_icon) };
	static QIcon polyIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYROI, sz_icon, sz_icon) ,
 RESOURCE_MANAGER->getIcon(ICON_VIEW_POLYROI_HOVER, sz_icon, sz_icon) };
	static QIcon lineIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_PROF_LINE, sz_icon, sz_icon) ,
 RESOURCE_MANAGER->getIcon(ICON_ANNO_PROF_LINE_HOVER, sz_icon, sz_icon) };
	static QIcon disIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_ANNO_LENGTH, sz_icon, sz_icon) ,
 RESOURCE_MANAGER->getIcon(ICON_ANNO_LENGTH_HOVER, sz_icon, sz_icon) };
	static QIcon ZeroIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_MESH_ZERO, sz_icon, sz_icon) ,
 RESOURCE_MANAGER->getIcon(ICON_MESH_ZERO_HOVER, sz_icon, sz_icon), RESOURCE_MANAGER->getIcon(ICON_MESH_ZERO, sz_icon, sz_icon) };
	static QIcon polyFreeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_POLYFREE, sz_icon, sz_icon) ,
 RESOURCE_MANAGER->getIcon(ICON_VIEW_POLYROI_HOVER, sz_icon, sz_icon) };
	static QIcon sculptIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_BRUSH_SCULPT, sz_icon, sz_icon) ,
 RESOURCE_MANAGER->getIcon(ICON_BRUSH_SCULPT_HOVER, sz_icon, sz_icon) };
	static QIcon fillholeIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_SCULPT_OP_HOLEFILL, sz_icon, sz_icon) ,
 RESOURCE_MANAGER->getIcon(ICON_SCULPT_OP_HOLEFILL_HOVER, sz_icon, sz_icon) };
	static QIcon selectIcon[] = { RESOURCE_MANAGER->getIcon(ICON_NON_BRUSH_SELECT, sz_icon, sz_icon) ,
 RESOURCE_MANAGER->getIcon(ICON_BRUSH_SELECT_HOVER, sz_icon, sz_icon) };

	if (target == NULL)
	{
		return false;
	}

	if (!target->inherits("QPushButton") && !target->inherits("QToolButton"))
	{
		return false;
	}

	QEvent::Type _type = e->type();

	int res = 0;
	bool press = false;

	if (_type == QEvent::HoverLeave ||
		_type == QEvent::Leave || _type == QEvent::Show)
	{
		if (HoverWidget)
		{
			if (!HoverWidget->isChecked())
				HoverWidget->setIcon(LeaveIcon);
		}

		//	this->setCursor(m_cursor);
	}

	QWidget* w = dynamic_cast<QWidget*>(target);
	{
		QMouseEvent* evt = dynamic_cast<QMouseEvent*>(e);
		if (evt)
		{
			if (w)
			{
				if (w->rect().contains(evt->pos()))
				{
					res = 1;
					press = e->type() == QEvent::MouseButtonPress;
				}
			}
		}
	}
	if (1 != res)
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
	if (1 != res)
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

	if (res == 1)
	{
		auto cut_flag = m_pMeshCutManager->getCutFlag();

		if (target == m_BtnCube && !m_BtnCube->isChecked())
		{
			m_BtnCube->setIcon(cubeIcon[res]);
			HoverWidget = m_BtnCube;
			LeaveIcon = cubeIcon[0];
		}
		else if (target == m_BtnCylin && !m_BtnCylin->isChecked())
		{
			m_BtnCylin->setIcon(cylinIcon[res]);
			HoverWidget = m_BtnCylin;
			LeaveIcon = cylinIcon[0];
		}
		else if (target == m_BtnSphere && !m_BtnSphere->isChecked())
		{
			m_BtnSphere->setIcon(sphereIcon[res]);
			HoverWidget = m_BtnSphere;
			LeaveIcon = sphereIcon[0];
		}
		else if (target == m_BtnDistance && !m_BtnDistance->isChecked())
		{
			m_BtnDistance->setIcon(disIcon[res]);
			HoverWidget = m_BtnDistance;
			LeaveIcon = disIcon[0];
		}
		else if (target == m_BtnSculpt && !m_BtnSculpt->isChecked())
		{
			m_BtnSculpt->setIcon(sculptIcon[res]);
			HoverWidget = m_BtnSculpt;
			LeaveIcon = sculptIcon[0];
		}
		else if (target == m_BtnSelect && !m_BtnSelect->isChecked())
		{
			m_BtnSelect->setIcon(selectIcon[res]);
			HoverWidget = m_BtnSelect;
			LeaveIcon = selectIcon[0];
		}
		else if (target == m_BtnPlane && !m_BtnPlane->isChecked())
		{
			m_BtnPlane->setIcon(planeIcon[res]);
			HoverWidget = m_BtnPlane;
			LeaveIcon = planeIcon[0];
		}
		else if (target == m_BtnPoly && !m_BtnPoly->isChecked())
		{
			m_BtnPoly->setIcon(polyIcon[res]);
			HoverWidget = m_BtnPoly;
			LeaveIcon = polyIcon[0];
		}
		else if (target == m_BtnLine && !m_BtnLine->isChecked())
		{
			m_BtnLine->setIcon(lineIcon[res]);
			HoverWidget = m_BtnLine;
			LeaveIcon = lineIcon[0];
		}
		else if (target == m_BtnPoly && !m_BtnPoly->isChecked())
		{
			m_BtnPoly->setIcon(polyFreeIcon[res]);
			HoverWidget = m_BtnPoly;
			LeaveIcon = polyFreeIcon[0];
		}
		else if (target == m_BtnZero)
		{
			if (m_BtnZero->isChecked())
			{
				m_BtnZero->setChecked(false);
			}

			m_BtnZero->setIcon(ZeroIcon[res + press]);
			HoverWidget = m_BtnZero;
			LeaveIcon = ZeroIcon[0];
		}
		else if (target == m_BtnCut && !(*cut_flag))
		{
			m_BtnCut->setIcon(planeIcon[res]);
			HoverWidget = m_BtnCut;
			LeaveIcon = planeIcon[0];
		}
		else if (target == m_BtnFillHole && !m_BtnFillHole->isChecked())
		{
			m_BtnFillHole->setIcon(fillholeIcon[res]);
			HoverWidget = m_BtnFillHole;
			LeaveIcon = fillholeIcon[0];
		}
#if SUPPORT_STAMP3D == 1
		else if (target == m_BtnStamp3d && !m_BtnStamp3d->isChecked())
		{
			m_BtnStamp3d->setIcon(selectIcon[res]);
			HoverWidget = m_BtnStamp3d;
			LeaveIcon = selectIcon[0];
		}
#endif
#if SUPPORT_MATCAP == 1
		else if (target == m_BtnShader && !m_BtnShader->isChecked())
		{
			m_BtnShader->setIcon(selectIcon[res]);
			HoverWidget = m_BtnShader;
			LeaveIcon = selectIcon[0];
		}
#endif
	}

	return true;
}

void CMeshViewBtn3DScene::OnPlanecut()
{
	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));

		m_BtnPlane->setChecked(false);

		auto act_plane = m_pMeshCutManager->getActionPtr(MESH_WORK_PLANE_CUT);

		if (act_plane)
		{
			act_plane->setChecked(false);
		}

		return;
	}

	/*if (m_ActPlane)
	m_ActPlane->setText(QString("Plane cut mode %1").arg(m_ActPlane->isChecked() ? QString("OFF") : QString("ON")));*/
	//if ( (*(m_pModelViewManager->getMeshpckIDPtr()) == -1) || (m_pModelViewManager->pckMeshModelCount() != 1) )
	//{
	//	m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);

	//	auto view = m_pWinManager->mainMeshWidget->getMainView();
	//	QMessageBox::warning(view, QString("Action for single mesh"), QString("Pick single mesh to apply this function."));
	//	return;
	//}

	//mip::MeshTopology * m = m_pDataContext->m_MeshData.GetMesh(*(m_pModelViewManager->getMeshpckIDPtr()));
	mip::MeshTopology* m = m_pDataContext->m_MeshData.GetCurrentMesh();


	auto act_plane = m_pMeshCutManager->getActionPtr(MESH_WORK_PLANE_CUT);

	if (act_plane->isChecked() && m)
	{
		MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();

		CMeshCutDlg* dlg = static_cast<CMeshCutDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_MESHCUT));

		bool b_select_flag = false;
		if (dlg && dlg->isVisible())
		{
			dlg->updateComboboxIndex(1, false);
		}
		else
		{
			//mip::MeshTopology* p_mesh = m_pDataContext->volume_data..GetCurrentMesh();
			//if (p_mesh && mip::mesh_control::isTriFlag(p_mesh, mip::SELECTED))
			//{
			//	b_select_flag = true;
			//}

			//if (mode == MESH_WORK_MANIFULATE || b_select_flag)
			//{
			//	m_pDlgManager->closeDialog();
			//}
			//else
			//{
			//	m_pDlgManager->rejectDialog();
			//}

			m_pDlgManager->closeDialog();

			m_pWorkManager->UpdateWorkMode(MESH_WORK_PLANE_CUT, true);

			if (!b_select_flag)
			{
				m_pDlgManager->makeMeshDialog(MESH_WORK_PLANE_CUT);
			}
		}

		ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_PLANE_CUT, mode);

		if (b_select_flag)
		{
			m_pWinManager->mainMeshWidget->getMainView()->clearSelectFlag(true);

			m_pDlgManager->makeMeshDialog(MESH_WORK_PLANE_CUT);
		}

		m_pPlaneManipulator->UpdatePosition(m);
	}
	else
	{
		MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();
		CMeshCutDlg* dlg = static_cast<CMeshCutDlg*>(m_pDlgManager->getMeshDialog(mode));
		if (dlg && dlg->isVisible())
		{
			dlg->OnCancel();
		}

		m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
	}

	m_pMeshCutManager->clearPlaneCutParam();
}

void CMeshViewBtn3DScene::OnPolyInnercut()
{
	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));

		m_BtnPoly->setChecked(false);

		auto act_polygon = m_pMeshCutManager->getActionPtr(MESH_WORK_POLYGON_CUT);

		if (act_polygon)
		{
			act_polygon->setChecked(false);
		}

		return;
	}

	/*if (m_ActPolygon)
	m_ActPolygon->setText(QString("Inner cut mode %1").arg(m_ActPolygon->isChecked() ? QString("OFF") : QString("ON")));*/
	//if (*(m_pModelViewManager->getMeshpckIDPtr()) == -1)
	if (m_pDataContext->m_MeshData.GetCurrentMeshIndex() == -1)
	{
		m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		//QMessageBox::warning(this, QString("Action for single mesh"), QString("Pick the mesh to apply this function."));
		return;
	}

	//const mip::MeshTopology * m = m_pDataContext->m_MeshData.GetMesh(*(m_pModelViewManager->getMeshpckIDPtr()));
	const mip::MeshTopology* m = m_pDataContext->m_MeshData.GetCurrentMesh();

	auto act_polygon = m_pMeshCutManager->getActionPtr(MESH_WORK_POLYGON_CUT);

	if (act_polygon->isChecked() && m)
	{
		MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();

		CMeshCutDlg* dlg = static_cast<CMeshCutDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_MESHCUT));

		bool b_select_flag = false;
		if (dlg && dlg->isVisible())
		{
			dlg->updateComboboxIndex(1, false);
		}
		else
		{
			//mip::MeshTopology* p_mesh = m_pDataContext->volume_data..GetCurrentMesh();
			//if (p_mesh && mip::mesh_control::isTriFlag(p_mesh, mip::SELECTED))
			//{
			//	b_select_flag = true;
			//}

			//if (mode == MESH_WORK_MANIFULATE || b_select_flag)
			//{
			//	m_pDlgManager->closeDialog();
			//}
			//else
			//{
			//	m_pDlgManager->rejectDialog();
			//}

			m_pDlgManager->closeDialog();

			m_pWorkManager->UpdateWorkMode(MESH_WORK_POLYGON_CUT, true);

			m_pDlgManager->makeMeshDialog(MESH_WORK_POLYGON_CUT);
		}

		ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_POLYGON_CUT, mode);

		if (b_select_flag)
		{
			m_pWinManager->mainMeshWidget->getMainView()->clearSelectFlag(true);
		}
	}
	else
	{
		m_pDlgManager->rejectDialog();

		m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
	}

	//20210615_byPHS_MeshEdit uv 작업 전까지 3mf파일을 기본 쉐이더로 
#if 0
	int   	meshCnt = m_pDataContext->volume_data..GetMeshCount();
	int   	pickCnt = m_pWinManager->vt_pckID.size();
	mip::MeshTopology* p_mesh = nullptr;
	auto view = m_pWinManager->mainMeshWidget->getMainView();

	for (int i = 0; i < meshCnt; ++i)
	{
		auto  pck_id = m_pWinManager->vt_pckID[i];
		if ((-1 == pck_id) || !m_pWinManager->vt_pckID[i]) continue;

		p_mesh = m_pDataContext->volume_data..GetMesh(i);
		p_mesh->initShader(mip::SHADERTYPE::SHADER_PONG);
	}
#endif
	//m_pWinManager->mainMeshWidget->getMainView()->clearSelectFlag();
	//m_pMeshCutManager->clearPlaneCutParam();

	//m_pMeshCutManager->setPolyProcessFlag(true);
}

void CMeshViewBtn3DScene::OnPolyLinecut()
{
	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));

		m_BtnLine->setChecked(false);

		auto act_polyline = m_pMeshCutManager->getActionPtr(MESH_WORK_POLYLINE_CUT);

		if (act_polyline)
		{
			act_polyline->setChecked(false);
		}

		return;
	}

	//if (m_ActPolyline)
	//	m_ActPolyline->setText(QString("Outer cut mode %1").arg(m_ActPolyline->isChecked() ? QString("OFF") : QString("ON")));
	//if (*(m_pModelViewManager->getMeshpckIDPtr()) == -1)
	if (m_pDataContext->m_MeshData.GetCurrentMeshIndex() == -1)
	{
		m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		//QMessageBox::warning(this, QString("Action for single mesh"), QString("Pick the mesh to apply this function."));
		return;
	}

	//const mip::MeshTopology * m = m_pDataContext->m_MeshData.GetMesh(*(m_pModelViewManager->getMeshpckIDPtr()));
	const mip::MeshTopology* m = m_pDataContext->m_MeshData.GetCurrentMesh();

	auto act_polyline = m_pMeshCutManager->getActionPtr(MESH_WORK_POLYLINE_CUT);

	if (act_polyline->isChecked() && m)
	{
		MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();

		CMeshCutDlg* dlg = static_cast<CMeshCutDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_MESHCUT));

		bool b_select_flag = false;
		if (dlg && dlg->isVisible())
		{
			dlg->updateComboboxIndex(1, false);
		}
		else
		{
			//mip::MeshTopology* p_mesh = m_pDataContext->volume_data..GetCurrentMesh();
			//if (p_mesh && mip::mesh_control::isTriFlag(p_mesh, mip::SELECTED))
			//{
			//	b_select_flag = true;
			//}

			//if (mode == MESH_WORK_MANIFULATE || b_select_flag)
			//{
			//	m_pDlgManager->closeDialog();
			//}
			//else
			//{
			//	m_pDlgManager->rejectDialog();
			//}

			m_pDlgManager->closeDialog();

			m_pWorkManager->UpdateWorkMode(MESH_WORK_POLYLINE_CUT, true);

			m_pDlgManager->makeMeshDialog(MESH_WORK_POLYLINE_CUT);
		}

		ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_POLYLINE_CUT, mode);

		if (b_select_flag)
		{
			m_pWinManager->mainMeshWidget->getMainView()->clearSelectFlag(true);
		}
	}
	else
	{
		m_pDlgManager->rejectDialog();

		m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
	}

#if 0 
	//20210615_byPHS_MeshEdit uv 작업 전까지 3mf파일을 기본 쉐이더로 
	int   	meshCnt = m_pDataContext->volume_data..GetMeshCount();
	int   	pickCnt = m_pWinManager->vt_pckID.size();
	mip::MeshTopology* p_mesh = nullptr;
	auto view = m_pWinManager->mainMeshWidget->getMainView();

	for (int i = 0; i < meshCnt; ++i)
	{
		auto  pck_id = m_pWinManager->vt_pckID[i];
		if ((-1 == pck_id) || !m_pWinManager->vt_pckID[i]) continue;

		p_mesh = m_pDataContext->volume_data..GetMesh(i);
		p_mesh->initShader(mip::SHADERTYPE::SHADER_PONG);
	}
#endif
	//m_pWinManager->mainMeshWidget->getMainView()->clearSelectFlag();

	//m_pMeshCutManager->clearPlaneCutParam();

	//m_pMeshCutManager->setPolyProcessFlag(true);
}

void CMeshViewBtn3DScene::OnPolyFreecut()
{
	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));

		m_BtnPolyFree->setChecked(false);

		auto act_polygon_free = m_pMeshCutManager->getActionPtr(MESH_WORK_FREEPOLYLINE_CUT);

		if (act_polygon_free)
		{
			act_polygon_free->setChecked(false);
		}

		return;
	}

	//if (m_ActPolyFree)
	//	m_ActPolyFree->setText(QString("Outer cut mode %1").arg(m_ActPolyline->isChecked() ? QString("OFF") : QString("ON")));
	//if (*(m_pModelViewManager->getMeshpckIDPtr()) == -1)
	if (m_pDataContext->m_MeshData.GetCurrentMeshIndex() == -1)
	{
		m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);

		auto view = m_pWinManager->mainMeshWidget->getMainView();
		QMessageBox::warning(view, QString("Action for single mesh"), QString("Pick the mesh to apply this function."));
		return;
	}

	//const mip::MeshTopology * m = m_pDataContext->m_MeshData.GetMesh(*(m_pModelViewManager->getMeshpckIDPtr()));
	const mip::MeshTopology* m = m_pDataContext->m_MeshData.GetCurrentMesh();

	auto act_polyfree = m_pMeshCutManager->getActionPtr(MESH_WORK_FREEPOLYLINE_CUT);

	if (act_polyfree->isChecked() && m)
	{
		MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();

		CMeshCutDlg* dlg = static_cast<CMeshCutDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_MESHCUT));

		bool b_select_flag = false;
		if (dlg && dlg->isVisible())
		{
			dlg->updateComboboxIndex(1, false);
		}
		else
		{
			//mip::MeshTopology* p_mesh = m_pDataContext->volume_data..GetCurrentMesh();
			//if (p_mesh && mip::mesh_control::isTriFlag(p_mesh, mip::SELECTED))
			//{
			//	b_select_flag = true;
			//}

			//if (mode == MESH_WORK_MANIFULATE || b_select_flag)
			//{
			//	m_pDlgManager->closeDialog();
			//}
			//else
			//{
			//	m_pDlgManager->rejectDialog();
			//}

			m_pDlgManager->closeDialog();

			m_pWorkManager->UpdateWorkMode(MESH_WORK_FREEPOLYLINE_CUT, true);

			m_pDlgManager->makeMeshDialog(MESH_WORK_FREEPOLYLINE_CUT);
		}

		ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_FREEPOLYLINE_CUT, mode);

		if (b_select_flag)
		{
			m_pWinManager->mainMeshWidget->getMainView()->clearSelectFlag(true);
		}
	}
	else
	{
		m_pDlgManager->rejectDialog();

		m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
	}

#if 0
	//20210615_byPHS_MeshEdit uv 작업 전까지 3mf파일을 기본 쉐이더로 
	int   	meshCnt = m_pDataContext->volume_data..GetMeshCount();
	int   	pickCnt = m_pWinManager->vt_pckID.size();
	mip::MeshTopology* p_mesh = nullptr;
	auto view = m_pWinManager->mainMeshWidget->getMainView();

	for (int i = 0; i < meshCnt; ++i)
	{
		auto  pck_id = m_pWinManager->vt_pckID[i];
		if ((-1 == pck_id) || !m_pWinManager->vt_pckID[i]) continue;

		p_mesh = m_pDataContext->volume_data..GetMesh(i);
		p_mesh->initShader(mip::SHADERTYPE::SHADER_PONG);
	}
#endif
	//m_pWinManager->mainMeshWidget->getMainView()->clearSelectFlag();

	//m_pMeshCutManager->clearPlaneCutParam();

	//m_pMeshCutManager->setPolyProcessFlag(true);
}

void	CMeshViewBtn3DScene::OnFillHoleSelected()
{
	if (ACTION_MANAGER->isActionFinished())
	{
		bool checked = m_BtnFillHole->isChecked();

		MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();
		ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_HOLE_FILL_SELECTED, mode);

		if (!checked)
		{
			m_pDlgManager->closeDialog();

			m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);

			auto view = m_pWinManager->mainMeshWidget->getMainView();
			view->renderLater();

			return;
		}

		//if ((*(m_pModelViewManager->getMeshpckIDPtr()) == -1) || (m_pModelViewManager->pckMeshModelCount() != 1) )
		if (m_pDataContext->m_MeshData.GetCurrentMeshIndex() == -1 || m_pDataContext->m_MeshData.GetMeshCount() != 1)
		{
			m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);

			auto view = m_pWinManager->mainMeshWidget->getMainView();
			QMessageBox::warning(view, QString("Action for single mesh"), QString("Pick the mesh to apply this function."));
			m_BtnFillHole->setChecked(false);
			return;
		}

		m_pDlgManager->makeMeshDialog(MESH_WORK_HOLE_FILL_SELECTED);

		m_pHoleFillManager->Process(MESH_WORK_FIND_HOLE);

		auto view = m_pWinManager->mainMeshWidget->getMainView();
		view->renderLater();
	}
	else
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));

		m_BtnFillHole->setChecked(false);
	}
}


void	CMeshViewBtn3DScene::OnCreateSphere()
{
	int sz_icon = m_pWinManager->mainWindow->IconSize;

	static QIcon spherePress = RESOURCE_MANAGER->getIcon(ICON_MESH_SPHERE, sz_icon, sz_icon);
	static QIcon sphereLeave = RESOURCE_MANAGER->getIcon(ICON_NON_MESH_SPHERE, sz_icon, sz_icon);

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));

		m_BtnSphere->setChecked(false);
		m_BtnSphere->setIcon(sphereLeave);

		return;
	}

	m_pDlgManager->rejectDialog();

	m_BtnSphere->setChecked(true);
	m_BtnSphere->setIcon(spherePress);

	auto view = m_pWinManager->mainMeshWidget->getMainView();

	SizeDialog dlg(QString("Create sphere"), 2, view);
	int Ecode = dlg.exec();


	if (QDialog::Accepted == Ecode)
	{
		mip::VECTOR3 vec;
		int resol[2];

		vec.x = dlg.getWidth();
		vec.y = dlg.getHeight();
		vec.z = dlg.getDepth();

		vec /= 10.f; //downscale

		for (int i = 0; i < 2; i++)
			resol[i] = dlg.getResolution(i);

		QString name = dlg.getName();

		ACTION_MANAGER->action_MeshList_create_polygon(m_pDataContext, eMeshPrimitiveType::Sphere, vec, name, resol);

		//m_pointList3D.clear();
		//m_lineList3D.clear();

		auto p_polyline = m_pMeshCutManager->getPolyLinePtr();
		p_polyline->clear();

	}

	m_BtnSphere->setChecked(false);
	m_BtnSphere->setIcon(sphereLeave);
}

void CMeshViewBtn3DScene::OnCreateCube()
{
	int sz_icon = m_pWinManager->mainWindow->IconSize;

	static QIcon cubePress = RESOURCE_MANAGER->getIcon(ICON_MESH_CUBE, sz_icon, sz_icon);
	static QIcon cubeLeave = RESOURCE_MANAGER->getIcon(ICON_NON_MESH_CUBE, sz_icon, sz_icon);

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));

		m_BtnCube->setChecked(false);
		m_BtnCube->setIcon(cubeLeave);

		return;

	}

	m_pDlgManager->rejectDialog();

	m_BtnCube->setChecked(true);
	m_BtnCube->setIcon(cubePress);

	auto view = m_pWinManager->mainMeshWidget->getMainView();

	SizeDialog dlg(QString("Create cube"), 0, view);
	int Ecode = dlg.exec();

	if (QDialog::Accepted == Ecode)
	{
		mip::VECTOR3 vec;
		//	int resol[2];

		vec.x = dlg.getWidth();
		vec.y = dlg.getHeight();
		vec.z = dlg.getDepth();

		vec /= 10.0f; //scale

		QString name = dlg.getName();

		ACTION_MANAGER->action_MeshList_create_polygon(m_pDataContext, eMeshPrimitiveType::Cube, vec, name);

		//m_pointList3D.clear();
		//m_lineList3D.clear();

		auto p_polyline = m_pMeshCutManager->getPolyLinePtr();
		p_polyline->clear();

	}

	m_BtnCube->setChecked(false);
	m_BtnCube->setIcon(cubeLeave);
}

void CMeshViewBtn3DScene::OnCreateCylinder()
{
	if (!m_pDataContext)
	{
		return;
	}

	int sz_icon = m_pWinManager->mainWindow->IconSize;

	static QIcon cylinPress = RESOURCE_MANAGER->getIcon(ICON_MESH_CYLINDER, sz_icon, sz_icon);
	static QIcon cylinLeave = RESOURCE_MANAGER->getIcon(ICON_NON_MESH_CYLINDER, sz_icon, sz_icon);

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));

		m_BtnCylin->setChecked(false);
		m_BtnCylin->setIcon(cylinLeave);

		return;
	}

	m_pDlgManager->rejectDialog();

	m_BtnCylin->setChecked(true);
	m_BtnCylin->setIcon(cylinPress);

	auto view = m_pWinManager->mainMeshWidget->getMainView();

	SizeDialog dlg(QString("Create cylinder"), 1, view);
	int Ecode = dlg.exec();


	if (QDialog::Accepted == Ecode)
	{
		mip::VECTOR3 vec;

		vec.x = dlg.getWidth();
		vec.y = dlg.getHeight();
		vec.z = dlg.getDepth();

		vec /= 10.f; //downscale

		int resol = dlg.getResolution();

		QString name = dlg.getName();

		ACTION_MANAGER->action_MeshList_create_polygon(m_pDataContext, eMeshPrimitiveType::Cylinder, vec, name, &resol);

		//m_pointList3D.clear();
		//m_lineList3D.clear();

		auto p_polyline = m_pMeshCutManager->getPolyLinePtr();
		p_polyline->clear();
	}

	m_BtnCylin->setChecked(false);
	m_BtnCylin->setIcon(cylinLeave);
}

void CMeshViewBtn3DScene::OnDistance()
{
	if (m_BtnDistance->isChecked())
	{
		m_pDistanceManager->Process();
	}
}

void CMeshViewBtn3DScene::OnSculpt()
{
	if (!ACTION_MANAGER->isActionFinished() || !m_pDataContext || !m_pViewer)
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));

		m_BtnSelect->setChecked(false);
	}

	bool checked = m_BtnSculpt->isChecked();
	m_pDlgManager->rejectDialog();

	if (!checked) return;

	//int nSelectedCnt = 0;
	//for (int ii = 0; ii < m_pWinManager->vt_pckID.size(); ii++)
	//{
	//	if (m_pWinManager->vt_pckID[ii])	nSelectedCnt++;
	//}

	int nSelectedCnt = m_pDataContext->m_MeshData.GetSelectMeshCount();

	//if (*(m_pModelViewManager->getMeshpckIDPtr()) == -1 || nSelectedCnt > 1)
	if (m_pDataContext->m_MeshData.GetCurrentMeshIndex() == -1 || nSelectedCnt > 1)
	{
		//auto view = m_pWinManager->mainMeshWidget->getMainView();

		m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		QMessageBox::warning(m_pViewer, QString("Action for single mesh"), QString("Pick one mesh to apply this function."));
		m_BtnSculpt->setChecked(false);
		return;
	}

	MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();
	//ACTION_MANAGER->action_UndoRedo_update(MESH_WORK_BRUSH_SCULPT, mode);

	if (m_pDlgManager->makeMeshDialog(MESH_WORK_BRUSH_SCULPT))
	{
		m_pWorkManager->setWorkMode(mode);

		int nPickMesh = m_pDataContext->m_MeshData.GetCurrentMeshIndex();
		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(nPickMesh);
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetMesh(nPickMesh);

		ACTION_MANAGER->action_Create_MatchingData(m_pDataContext, mesh);
	}
}


void CMeshViewBtn3DScene::OnSelect()
{
	if (!ACTION_MANAGER->isActionFinished() || !m_pDataContext || !m_pViewer)
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));

		m_BtnFillHole->setChecked(false);
	}

	bool checked = m_BtnSelect->isChecked();

	MESH_WORK_MODE mode = m_pWorkManager->getWorkMode();
	if (!checked)
	{
		if (mode == MESH_WORK_MANIFULATE) m_pDlgManager->closeDialog();
		else m_pDlgManager->rejectDialog();

		m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);

		return;
	}

	bool b_select_flag = false;
	mip::MeshTopology* p_mesh = m_pDataContext->m_MeshData.GetCurrentMesh();
	if (p_mesh && mip::mesh_control::isTriFlag(p_mesh, mip::SELECTED))
	{
		b_select_flag = true;
	}

	//if (mode == MESH_WORK_MANIFULATE || b_select_flag)
	//{
	//	m_pDlgManager->closeDialog();
	//}
	//else
	//{
	//	m_pDlgManager->rejectDialog();
	//}

	m_pDlgManager->closeDialog();

	//int nSelectedCnt = 0;
	//for (int ii = 0; ii < m_pWinManager->vt_pckID.size(); ii++)
	//{
	//	if (m_pWinManager->vt_pckID[ii])	nSelectedCnt++;
	//}
	int nSelectedCnt = m_pDataContext->m_MeshData.GetSelectMeshCount();

	//if (*(m_pModelViewManager->getMeshpckIDPtr()) == -1 || nSelectedCnt > 1)
	if (m_pDataContext->m_MeshData.GetCurrentMeshIndex() == -1 || nSelectedCnt > 1)
	{
		auto view = m_pWinManager->mainMeshWidget->getMainView();

		m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		QMessageBox::warning(view, QString("Action for single mesh"), QString("Pick one mesh to apply this function."));
		m_BtnSelect->setChecked(false);
		return;
	}

	ACTION_MANAGER->action_UndoRedo_update(m_pDataContext, MESH_WORK_BRUSH_SELECTION, mode);

	if (b_select_flag)
	{
		//m_pDlgManager->makeMeshDialog(MESH_WORK_BRUSH_SELECTION, false, false);
		BrushSelectDlg* dlg = static_cast<BrushSelectDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_SELECT));
		if (dlg)
		{
			if (dlg->isVisible())
			{
				dlg->showFuncBtn();
			}
			else
			{
				dlg->show();
			}
		}
	}
	else
	{
		BrushSelectDlg* dlg = static_cast<BrushSelectDlg*>(m_pDlgManager->getMeshDialog(MESH_DIALOG_SELECT));
		if (dlg)
		{
			if (!dlg->isVisible())
			{
				m_pDlgManager->makeMeshDialog(MESH_WORK_BRUSH_SELECTION);
			}
		}
	}

	m_pWorkManager->UpdateWorkMode(MESH_WORK_BRUSH_SELECTION, true);

	//20210608_byPHS
	//if (!m_pWinManager->mainMeshWidget->getMainView()->readyBrush(MESH_WORK_BRUSH_SELECTION, 1))
	//{
	//	m_pDlgManager->rejectDialog();
	//}
}

void CMeshViewBtn3DScene::OnStamp3D()
{
#if SUPPORT_STAMP3D == 1
	bool checked = m_BtnStamp3d->isChecked();
	m_pDlgManager->rejectDialog();

	if (!checked) return;

	//int nSelectedCnt = 0;
	//for (int ii = 0; ii < m_pWinManager->vt_pckID.size(); ii++)
	//{
	//	if (m_pWinManager->vt_pckID[ii])	
	//}

	int	n_mesh = m_pDataContext->m_MeshData.GetMeshCount();
	int nSelectedCnt = 0;
	for (int ii = 0; ii < n_mesh; ii++)
	{
		MeshInfo* pMeshInfo = m_pDataContext->m_MeshData.GetMeshInfo(ii);
		if (pMeshInfo && pMeshInfo->selected)
		{
			nSelectedCnt++;
		}
	}

	if (*(m_pModelViewManager->getMeshpckIDPtr()) == -1 || nSelectedCnt > 1)
	{
		auto view = m_pWinManager->mainMeshWidget->getMainView();

		m_pWorkManager->UpdateWorkMode(MESH_WORK_NONE, true);
		QMessageBox::warning(view, QString("Action for single mesh"), QString("Pick one mesh to apply this function."));
		m_BtnStamp3d->setChecked(false);
		return;
	}

	m_pDlgManager->makeMeshDialog(MESH_WORK_STAMP3D);
#endif
}

void CMeshViewBtn3DScene::OnShader()
{
#if SUPPORT_MATCAP == 1
	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));

		m_BtnShader->setChecked(false);
	}

	bool checked = m_BtnShader->isChecked();


	if (m_pWinManager->mainMeshWidget)
	{
		m_pWinManager->mainMeshWidget->showMaterialTab(checked);
	}


#endif
}

void	CMeshViewBtn3DScene::enableOpcityCtrl(bool _b_enable)
{
	if (!m_pLabelOpacity || !m_SliderTrans || !m_pDataContext || !m_pViewer)
	{
		return;
	}

	m_pLabelOpacity->setEnabled(_b_enable);
	m_SliderTrans->setEnabled(_b_enable);

	if (_b_enable)
	{
		// value update
		mip::MeshTopology* mesh = m_pDataContext->m_MeshData.GetCurrentMesh();
		if (mesh)
		{
			auto alpha = mesh->getAlphaVal();

			m_SliderTrans->setValue(alpha / 255.f * 100.f);

			m_pLabelOpacity->show();
			m_SliderTrans->show();
		}
	}
	else
	{
		m_pLabelOpacity->hide();
		m_SliderTrans->hide();
	}
}

void CMeshViewBtn3DScene::OnOpacityChange(int val)
{
	m_pModelViewManager->OnOpacityChange(val);
}

void CMeshViewBtn3DScene::OnReCalcZero()
{
	m_pModelViewManager->OnReCalcZero();
}