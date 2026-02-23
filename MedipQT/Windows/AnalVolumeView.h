#pragma once

#ifndef ANALMPRVOLUMEVIEW_H
#define ANALMPRVOLUMEVIEW_H

#include "define.h"
#include "glWidget.h"
#include "math/math.h"
#include "mipEngine/Camera.h"
#include "Math/Transform.h"

#include "DataContext.h"

#include <qimage.h>
#include <qicon.h>
#include <QBasicTimer>
#include <qwidget.h>

class QLabel;
class QImage;
class QSlider;
class QPushButton;
class QToolButton;
class QFocusEvent;
class QAbstractButton;
class Annotation;

class AnalVolumeView : public OpenGLWidget
{
	Q_OBJECT

public:
	AnalVolumeView(DataContext* pDataContext, QWidget *parent = 0);
	virtual ~AnalVolumeView();

	virtual void hideControls();
	virtual void showControls();
	void enableControls(bool bEnable) override;

	virtual mip::VECTOR3 getLocalToScreen(float x, float y, float z);
	virtual mip::VECTOR3 getScreenToProj(float x, float y);
	virtual mip::MATRIX44 getWVP();

public:
	void	setHomePosition(HOME_POSITION index);

	void    initInScreenMenu();
	void	updateScreenLeftMenu(int width, int height);
	void    reserveInit(HWND hwnd);
	void	resetUI() override;
	void	reInit() override;
	void	renderWaterMark();
	void	syncFullscreen();
	void	setClipPrefer(uchar _clip, bool optOnly = false);
	//	void	renderCoordLine();

protected:
	void resizeEvent(QResizeEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;
	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void keyPressEvent(QKeyEvent *e) override;
	void keyReleaseEvent(QKeyEvent *e) override;

	void wheelEvent(QWheelEvent *event) override;
	//	bool eventFilter(QObject * target, QEvent * e) override;
	virtual void render(QPainter *painter);
	void drawAnnotations(QPainter *p);
	void drawMouseWork(QPainter *p);

	int m_nIconSize = 0;

	virtual void dropEvent(QDropEvent* event) override;
	virtual void dragEnterEvent(QDragEnterEvent* event) override;
public:
	muint32 createTextureFromQImage(QImage * image);

	void setWorkMode(ANAL_WORK_MODE mode, bool iconRefresh = false);

	QImage	getCaptureImage(bool, bool);
	mip::SCAMERA getCamera() { return m_camera; }
	mip::TRANSFORM getWorld() { return m_trVolumModel; }

	void setVolRotation(mip::QUATERNION rotation) { m_trVolumModel.zero(); m_trVolumModel.addRotate(rotation); }
	void setImgRotation(mip::QUATERNION rotation) { m_trPlaneImage.zero(); m_trPlaneImage.addRotate(rotation); }
	void setVolTranslation(mip::VECTOR3 translation) { m_trVolumModel.addTranslate(translation); TransformRotate(&m_trVolumModel, &m_camera); }
	void setImgTranslation(mip::VECTOR3 translation);

	void setZoom(float zoom) { m_camera.setZoom(zoom); }

	mip::QUATERNION getVolRotation() { return m_trVolumModel.rotation; }
	mip::QUATERNION getImgRotation() { return m_trPlaneImage.rotation; }
	mip::VECTOR3 getVolTranslation() { return m_trVolumModel.translation; }
	mip::VECTOR3 getImgTranslation() { return m_trPlaneImage.translation; }

	float getZoom() { return m_camera.getZoom(); }
	uchar getClipPrefer() { return m_nClip; }
	void clip2DOff();
	bool getClip2DPlane();
	void useSkipRender(int index, bool res = true);
	bool	pickingMask(const mip::VECTOR2 & point, /*out*/mip::VECTOR3 & result, std::vector<std::pair<int, mask8>> &vecHitMaskBit = std::vector<std::pair<int, mask8>>());

protected:
	bool eventFilter(QObject * target, QEvent * e) override;
	void focusInEvent(QFocusEvent *) override;

private:
	void TransformRotate(mip::TRANSFORM * tr, const mip::SCAMERA * camera, mip::MATRIX44 * trasform = NULL);

	void renderScene(QPainter *p, bool skip_mode = false);
	void renderVolume(bool skip_mode);
	void renderMeshModel(mip::MATRIX44 & world, mip::MATRIX44 & view, mip::MATRIX44 & proj, bool culling = true);

	void mouseMove_Annotation_NormalMode();
	void mousePress_InNormalMode();
	void mouseRelease_EditSelectedAnnotation();
	
	bool commitToUpdateNewAnnotationStatus(Annotation* ano, int index);

	virtual void touchRelease();
	virtual void touchMove(mint32 preX, mint32 preY, mint32 currX, mint32 currY);
	virtual void touchZoom(float dt);

	QString getMoveMode();
	void updateToolButtonIcon_As_WorkMode(QPushButton* button, ANAL_WORK_MODE targetWorkMode, ANAL_WORK_MODE mode, QIcon icon[], bool iconRefresh);

private:
	HWND	m_hwnd;

	QPoint m_MousePos;
	QPoint m_MousePos_Pressed;
	QPoint m_preMousePos;
	bool m_mouseMove;

	bool m_RbuttonDown;
	bool m_LbuttonDown;
	bool m_MbuttonDown;

	QLabel *m_labelImage;
	QLabel *m_labelWater;
	QSize	m_size;
	QSize	m_waterSize;
	QSlider * m_slidebar;

	std::vector<mip::VECTOR3>		m_planeList;
	muint32 m_cullingTexture;
	mip::TRANSFORM					m_trVolumModel;
	mip::TRANSFORM					m_trPlaneImage;
	mip::TRANSFORM					m_trPlaneCut;
	mip::SCAMERA					m_camera;

	bool	m_bSkip[4]; /*0:wl change, 1: object move, 2: object zoom, 3: alpha change*/

	QToolButton *m_btn2D;
	QPushButton *m_btnFullScreen;

	QPushButton	*m_btnFreeDrawROI;
	QPushButton	*m_btnSplit;
	QPushButton *m_btnPlanecut;
	QPushButton	*m_btnDistance;


	QAction *actAxial;
	QAction *actCoronal;
	QAction *actSagittal;

	uchar m_nClip;
	const uchar CT_NONE = 0;
	const uchar CT_AXIAL = 1;
	const uchar CT_CORONAL = 1 << 1;
	const uchar CT_SAGITTAL = 1 << 2;
	QImage m_img;

	QPolygon				m_polyLine;
	bool m_bWorkMode;
	QAbstractButton *HoverWidget;
	QIcon LeaveIcon;

	std::unique_ptr<Annotation>		m_pVolumeDistAnnotation;
	std::vector<mip::VECTOR3>		m_vecVolumePoint;	// measurement tab calculate dist 관련 리스트

	std::unique_ptr<Annotation> m_pOriginAnnotation;
	
	DataContext* m_pDataContext;

private slots:
	void slot_OnAxial();
	void slot_OnCoronal();
	void slot_OnSagittal();

	void slot_OnFullScreen();
	void slot_OnWheelTimeout();


	void slot_OnPlanecut(bool checked = false);
	void slot_OnCurveSplit();
	void slot_OnFreeDrawROI();
	void slot_OnCalcDistance();
};
#endif
