#pragma once

#ifndef ANALMPRPLANEVIEW_H
#define ANALMPRPLANEVIEW_H

#include "define.h"
#include "glWidget.h"
#include "Renderer/Slice.h"
#include "Renderer/color.h"
#include <qwidget.h>
#include <qicon.h>
#include <qvector3d.h>
#include <qimage.h>
#include <memory>

#include "DataContext.h"

class QLabel;
class QImage;
class QSlider;
class QMenu;
class QAction;
class QFocusEvent;
class QComboBox;
class Annotation;
class WindowManager;

class AnalMPRPlaneView : public OpenGLWidget
{
	Q_OBJECT

public:
	AnalMPRPlaneView(WindowManager* pWinManager, WINDOW_TYPE windowType, DataContext* pDataContext, QWidget* parent = 0);
	AnalMPRPlaneView(WINDOW_TYPE windowType, DataContext* pDataContext, QWidget *parent = 0);
	virtual ~AnalMPRPlaneView();

public:
	virtual void enableControls(bool bEnable);
	void enableAnnoControls(bool bEnable);

public:
	virtual void resetUI() override;
	virtual void reInit() override;

public:
	void setWorkMode(ANAL_WORK_MODE mode, bool iconRefresh=false);
	bool isWorkMode();

	mip::Slice& getCamera() { return m_sliceCamera; }
	float getThickness() { return m_thickness; }
	void updatePlaneDatas();
	void updatePlaneDepth();

	void syncFullscreen();
	
	QImage getCaptureImage(bool, bool);
	void createCursorShape(bool _auto=false);

	mip::VECTOR3 getVolumeToWorld(mip::VECTOR3);
	mip::VECTOR3 getVolumeToWorldAuto(mip::VECTOR3);
	mip::VECTOR3 getWorldToVolume(float x, float y, float z);

	bool getCurrentCameraToPlaneIntersectedPoint(mip::VECTOR3* pOutPoint);
	bool getCameraToPlaneIntersectedPoint(mip::VECTOR3* pOutPoint, int screenX, int screenY);
	bool getCameraToPlaneIntersectedPoint(mip::Slice& sliceCamera, mip::PLANE& plane, mip::VECTOR3* pOutPoint, int screenX, int screenY);

	void hideControls();
	void showControls();

	void preDepth();
	void nextDepth();
	void setThickness(float _thick);
	void setWheelSliderFunc(bool is_SliderSlice_WheelZoom);

	void setLowSpecOptions();
	void setSliderMid();
	void depthChanged(int val);

	mint16 getHU_Point_Screen(int screenX, int screenY, OUT mip::VECTOR3* pVolumePoint = nullptr);

	void updatePlaneState();
	void updateMeshOutline();
protected:
	virtual void render(QPainter *p) override;

	virtual bool eventFilter(QObject * target, QEvent * e) override;
	virtual void keyPressEvent(QKeyEvent *e) override;
	virtual void keyReleaseEvent(QKeyEvent *e) override;

	virtual void mouseMoveEvent(QMouseEvent *e) override;
	virtual void mousePressEvent(QMouseEvent *e) override;
	virtual void mouseReleaseEvent(QMouseEvent *e) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *e)override;

	virtual void resizeEvent(QResizeEvent *e) override;
	virtual void wheelEvent(QWheelEvent *event) override;

	virtual void touchMove(mint32 preX, mint32 preY, mint32 currX, mint32 currY) override;
	virtual void touchZoom(float dt) override;

	virtual void dragEnterEvent(QDragEnterEvent* ev) override;
	virtual void dropEvent(QDropEvent* event);
protected:
	void processMouseDoubleClick(QMouseEvent *e);
	void processMousePress(QMouseEvent *e);
	void processMouseMove();
	void processMouseRelease();
	
	bool processRotate();
	void processSlideZ(float dt, bool isSlider=false, bool isRotate=false);
	void processResize(muint32 width, muint32 height);

	bool isSelectedView();
	void focusInEvent(QFocusEvent *) override;

	void slide(WINDOW_TYPE type, mip::VECTOR3 & dir);
	
	mip::VECTOR3 getLocalToVolume(float x, float y, float z);

	QCursor getBitmapCursor();

	void setImageData(muint32 ip_mode, QImage & image, mip::COLOR * data);

	void initInScreenMenu();
	void initInScreenRightMenu();
	void updateScreenRightMenu(int width, int height);

	void renderScene(QPainter *p, bool skip_mode = false);
	void renderScene2D(QPainter *p);

	void drawPlaneLine(QPainter *p);
	void drawAnnotations(QPainter *p);
	void drawMouseWork(QPainter *p);

	void drawSquareLine(QPainter *p, bool drawcut=false);
	void drawRuler(QPainter *p);
	void drawThickness(QPainter *p, int* pOutOffset);
	void drawCoordHU(QPainter *p, int offset);
	void drawWorkMode(QPainter *p);
	void drawMeshOutline(QPainter* p);

	void setContextMenu();

private:
	void mouseDblClick_Annotation();

	void mousePress_CaptureArea();
	void mousePress_SegmentPolyLine();
	void mousePress_Segmentation_MagicCut_or_SegmentRectOval();
	void mousePress_Segmentation_Pick_or_FreeDraw();
	void mousePress_Annotation_InDrawingMode(ANAL_WORK_MODE workMode);
	void mousePress_InNormalMode();

	void mouseMove_SegmentPolyLine();
	void mouseMove_Segmentation_Pick_or_FreeDraw();
	void mouseMove_Segmentation_MagicCut_or_RectOval();
	void mouseMove_CaptureArea();
	void mouseMove_Annotation_InDrawingMode();
	void mouseMove_Annotation_NormalMode();
	
	void mouseRelease_CaptureArea();
	void mouseRelease_Animation();
	void mouseRelease_Segmentation_MagicCut_or_RectOval();
	void mouseRelease_Segmentation_Pick_or_FreeDraw();
	void mouseRelease_EditSelectedAnnotation();

	bool commitToUpdateNewAnnotationStatus(Annotation* ano, int index);

	void drawAnnotationList(QPainter* p);

	void drawMousework_Segmentation_MagicCut_or_RectOval(QPainter* p);
	void drawMousework_Segmentation_PolyLine(QPainter* p);
	void drawMousework_Segmentation_Pick_or_FreeDraw(QPainter* p);
	void drawMousework_CaptureArea(QPainter* p);
	void drawMousework_Animation_Line(QPainter *p);

	void clearWindowManagerStatus(ANAL_WORK_MODE mode);
	void updateRightMenuButtonStatus_SelectedOrNot(ANAL_WORK_MODE mode, bool iconRefresh);
	void updateRightMenuButtonStatus_HoverOrNot(QEvent::Type type, QEvent*e, QObject * target);

	void updateMouseCursor_By_EventType(QEvent::Type type);
	void updateMouseCursor(ANAL_WORK_MODE mode, bool captureMode);

	void clearSelectedAnnotation(ANAL_WORK_MODE mode, bool* pVolumeUpdate = false);
	void updateVolumeUpdateStatus(ANAL_WORK_MODE mode, ANAL_WORK_MODE prevMode, bool* pVolumeUpdate);

	bool isMousePointInBoundaryBox();
	bool isBoxAABB_ContainPoint(mip::AABB box3D, mip::VECTOR3 point);

	bool isRenderable();
	void drawUnloadedImage(QPainter* p);

	void moveDepth(int direction);

	void checkDepth();
	void calcMesh2DOutline();

private slots:
	void	slot_OnAnnoLength();
	void	slot_OnAnnoText();
	void	slot_OnAnnoAngle();
	void	slot_OnInterOval();
	void	slot_OnAnnoArrow();
	void	slot_OnAnnoRect();
	void	slot_OnShowAniPoint();


	void	slot_OnContextProfPlane();
	void	slot_OnInterPoly();
	void	slot_OnProfileLine();
	void	slot_OnInterDraw();
	void	slot_OnInterPicker();
	void	slot_OnInterRect();

	void	slot_OnShowAxises();


	void	slot_OnContextAnnoText();
	void	slot_OnContextAnnoLength();
	void	slot_OnContextAnnoAngle();
	void	slot_OnContextAnnoArrow();
	void	slot_OnContextAnnoRectangle();
	void	slot_OnContextAnnoList();

protected slots:
	void	slot_OnDepthSlideChanged(int value);
	void	slot_OnComboThicknessChanged(int);

#ifdef _DEBUG
	void	slot_OnComboFilterChanged(int);
#endif
public slots:
	void	slot_OnFullScreen();
	void	slot_OnThickness();

protected:
	WindowManager* m_pWindowManager;
	float m_Winsize;

	QPolygon	m_list;
	QComboBox * m_comboThickness;
	bool		m_CoronalSeleted;
	bool		m_AxialSeleted;
	bool		m_SaggitalSeleted;

	QPoint		m_MousePos;
	QPoint		m_MousePos_Pressed;
	QPoint		m_preMousePos;
	QPoint		m_globalPos;
	bool		m_polycheck;

	bool		m_RbuttonDown;
	bool		m_LbuttonDown;
	bool		m_MbuttonDown;

	bool		m_bMoveMouse;

	bool		m_translationMPR;
	bool		m_rotateMPR;
	
	mip::Slice	m_sliceCamera;
	mip::VECTOR2 m_planeCenter;
	mip::VECTOR3 m_initOffset;

	QPainterPath m_rectRegion;
	QPolygon	m_polyLine;

	QPushButton *m_btnAnnoLength;
	QPushButton *m_btnAnnoText;
	QPushButton *m_btnAnnoAngle;
	QPushButton *m_btnAnnoArrow;
	QPushButton *m_btnAnnoRect;

	QPushButton *m_btnSegmentPoly;
	QPushButton *m_btnSegmentOval;
	QPushButton *m_btnSegmentDraw;
	QPushButton	*m_btnSegmentPicker;
	QPushButton	*m_btnSegmentRect;

	QPushButton *m_btnProfileLine;

	QPushButton *m_btnShowAxis;
	QPushButton *m_btnFullScreen;

	QPushButton	*m_btnThickness;

	QMenu *m_contextMenu;
	QAction *m_actShowAnnoLength;
	QAction *m_actShowAnnoText;
	QAction *m_actShowAnnoAngle;
	QAction *m_actShowAnnoArrow;
	QAction *m_actShowAnnoRectangle;
	QAction *m_actAnnoList;
	QAction	*m_actShowAniPoint;

	QSlider *m_slidebar;

	int m_nIconSize = 0;

#ifdef DEV_VER
	QAction *m_actPlane;
#endif

	QImage	m_img;

	float	m_thickness;
	float	m_userThick;
	QPushButton* m_HoverWidget;
	QIcon m_LeaveIcon;

	QLabel* m_labelUnloadImage;
	QLabel* m_labelWater;
	QSize m_size;
#ifdef _DEBUG
	QComboBox * m_comboFilterList;
#endif

	std::unique_ptr<Annotation> m_pCurAnnotation;
	std::unique_ptr<Annotation> m_pOriginAnnotation;

	bool m_bUpdateMeshOutline;

	DataContext* m_pDataContext;
};
#endif