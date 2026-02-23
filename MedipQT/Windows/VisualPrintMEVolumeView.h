#pragma once

#ifndef VISUALPRINT_MEVOLUMEVIEW_H
#define VISUALPRINT_MEVOLUMEVIEW_H

#include "define.h"
#include <qwidget.h>
#include "glWidget.h"
#include "qmap.h"
#include "math/math.h"
#include "mipEngine/Camera.h"
#include "Renderer/color.h"
#include "Math/Transform.h"
#include <qrect.h>
#include <qicon.h>
#include "Renderer/Mesh.h"

#include "DataContext.h"

namespace mip
{
	class MeshCore;
};

class QLabel;
class QImage;
class QMenu;
class QAction;
class QAbstractButton;
class QToolButton;


class VisualPrintMEVolumeView : public OpenGLWidget
{
	Q_OBJECT

public:
	VisualPrintMEVolumeView(DataContext* pDataContext, QWidget *parent = 0);
	virtual ~VisualPrintMEVolumeView();

	void	resetUI();

	void	updatePckModelIndex(int _idx);
	void	updateGeometry(bool bUpdate) {
		m_bUpdateGeometryCnt = bUpdate;
	};

private slots:
	void slot_OnAxial();
	void slot_OnCoronal();
	void slot_OnSagittal();

private:
	// Event Func
	////////////////////////////////////////////////////////////////////////
	void	resizeEvent(QResizeEvent *event) override;
	void	mouseMoveEvent(QMouseEvent *event) override;
	void	mousePressEvent(QMouseEvent *event) override;
	void	mouseReleaseEvent(QMouseEvent *event) override;
	void	keyPressEvent(QKeyEvent *e) override;
	void	keyReleaseEvent(QKeyEvent *e) override;
	void	wheelEvent(QWheelEvent *event) override;
	bool	eventFilter(QObject * target, QEvent * e) override;

	void	processMouseMove();
	void	processMousePress(QMouseEvent *e);
	void	processMouseRelease(QMouseEvent * e);
	////////////////////////////////////////////////////////////////////////

	// render Func
	////////////////////////////////////////////////////////////////////////
	virtual void	render(QPainter *painter);
	
	void			renderScene(QPainter *p, bool skip_mode = false);
	void			drawPckInfo(QPainter *p);
	void			renderVolume(bool skip_mode);
	////////////////////////////////////////////////////////////////////////

	// view direction
		////////////////////////////////////////////////////////////////////
	void	MeshFrontView();
	void	MeshBackView();
	void	MeshTopView();
	void	MeshBottomView();
	void	MeshLeftView();
	void	MeshRightView();
	mip::VECTOR3 getCetnerMeshes();
	////////////////////////////////////////////////////////////////////////
	
	// etc..
	////////////////////////////////////////////////////////////////////////
	void			initInScreenMenu();
	void			updateScreenLeftMenu(int width, int height);

	void			TransformRotate(mip::TRANSFORM * tr, const mip::SCAMERA * camera, mip::MATRIX44 * trasform = NULL);
	mip::MATRIX44   calcMatrixRotateFromPoint(mip::VECTOR3& _vec, mip::VECTOR3& _pt);

	void			updatePivotPoint();
	void			updatePivotPoint(int _idx);
	void			SetPivotPoint(mip::VECTOR3 _point);

	bool			getClip2DPlane();

	mip::VECTOR3	getWorldCenter(bool init = true);
	mip::VECTOR3	getSceneCenter3d();

	bool			updateGeometryCount(mip::MeshTopology * pMT, int & oVertCnt, int & oTriCnt);
	bool			getGeometryCount(mip::MeshTopology* pMT, int & _vCnt, int & _tCnt);
	
	void			resetPosition();
	////////////////////////////////////////////////////////////////////////


	// Variables
	////////////////////////////////////////////////////////////////////////
	uchar					m_nClip;

	int						m_nIconSize;

	mip::TRANSFORM			m_trObject;
	mip::TRANSFORM			m_trPlaneImage;
	mip::TRANSFORM			m_trPlaneCut;

	mip::SCAMERA			m_camera;

	mip::VECTOR3			m_worldCenter;

	QPoint					m_MousePos;
	QPoint					m_preMousePos;

	bool					m_RbuttonDown;
	bool					m_LbuttonDown;
	bool					m_MbuttonDown;

	bool					m_wireFrame;
	bool					m_backface;

	bool					m_bSkip[4]; /*0:wl change, 1: object move, 2: object zoom, 3: alpha change*/
	
	QToolButton*			m_btn2D;

	QAction*				m_actAxial;
	QAction*				m_actCoronal;
	QAction*				m_actSagittal;

	QAbstractButton*		m_HoverWidget;
	QIcon					m_LeaveIcon;

	muint32					m_cullingTexture;

	mip::VECTOR3			m_PivotPoint;
	bool					m_bPivotPoint;
	int						m_pckID;
	int						m_pckID_Old;

	bool					m_bUpdateGeometryCnt;
	int						m_VertCnt;
	int						m_TriCnt;
	float					m_TotalSize;

	DataContext*			m_pDataContext;
	
	////////////////////////////////////////////////////////////////////////
};
#endif
