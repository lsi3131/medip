#pragma once

#ifndef VOLUMEVIEW_H
#define VOLUMEVIEW_H

#include "define.h"
#include "glWidget.h"
#include "math/math.h"
#include "mipEngine/Camera.h"
#include "Math/Transform.h"
#include <QBasicTimer>
#include <QWidget>
#include <QIcon>

#ifdef _M_IX86
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkShortArray.h>
#include <vtkPointData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkDICOMImageReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkLODActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkPolyDataNormals.h>
#include <vtkContourFilter.h>
#include <vtkStripper.h>
#include <vtkLookupTable.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkSTLReader.h>
#include <vtkReverseSense.h>
//////////// Volume rendering
#include <vtkOpenGLGPUVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkImageChangeInformation.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkObjectFactory.h>
//////////// axis widget
#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include <vtkPropAssembly.h>
#include <vtkCommand.h>
#include <vtkSTLWriter.h>
#include <vtkPolyDataWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkFillHolesFilter.h>
#include <vtkAutoInit.h>
#endif

enum AREA_STATE
{
	AS_NONE = 0,

	AS_DCUT_START = 100,
	AS_DCUT_ING,
	AS_DCUT_END,
	AS_DCUT_CUTTING,
};

struct VOLUMEVIEW_RESET_UI_INFO
{
	mip::SCAMERA camera;
	float fCameraXZAngle;
	mip::TRANSFORM trVolumModel;
	mip::TRANSFORM trPlaneCut;
	mip::TRANSFORM trPlaneImage;
	bool bResetUIExcuted;
};

class QLabel;
class QPushButton;
class QImage;
class QSlider;
class QCheckBox;
class QMenu;
class QAction;
class QFocusEvent;
class QVBoxLayout;
class QAbstractButton;

namespace mip
{
	class MeshTopology;
}

class VolumeView : public OpenGLWidget
{
	Q_OBJECT

public:
	VolumeView(QWidget *parent = 0);
	virtual ~VolumeView();
	mip::QUATERNION getVolRotation() { return m_trVolumModel.rotation; }
	mip::QUATERNION getImgRotation() { return m_trPlaneImage.rotation; }
	mip::VECTOR3 getVolTranslation() { return m_trVolumModel.translation; }
	mip::VECTOR3 getImgTranslation() { return m_trPlaneImage.translation; }
	float getZoom() { return m_camera.getZoom(); }
	mip::VECTOR3 getScreenToLocal(muint32 screen_x, muint32 screen_y, float depth);
	virtual mip::VECTOR3 getLocalToScreen(float x, float y, float z);
	mip::VECTOR3 getLocalToVolume(float x, float y, float z);
	virtual mip::VECTOR3 getScreenToProj(float x, float y);
	virtual mip::MATRIX44 getWVP();
	void TransformRotate(mip::TRANSFORM & tr, const mip::SCAMERA & camera, mip::MATRIX44 * trasform = NULL);
	void TransformRotate(mip::TRANSFORM &tr, int type);
	void setVolRotation(mip::QUATERNION rotation) { m_trVolumModel.zero(); m_trVolumModel.addRotate(rotation); }
	void setImgRotation(mip::QUATERNION rotation) { m_trPlaneImage.zero(); m_trPlaneImage.addRotate(rotation); }
	void setVolTranslation(mip::VECTOR3 translation) { m_trVolumModel.addTranslate(translation); TransformRotate(m_trVolumModel, m_camera);}
	void setImgTranslation(mip::VECTOR3 translation);
	void setZoom(float zoom) { m_camera.setZoom(zoom); }

	virtual void hideControls();
	virtual void showControls();
	void enableControls(bool bEnable) override;

	void initInScreenRightMenu();
	void updateScreenLeftMenu(int width, int height);

	void setContextMenu();
	void setWorkMode(WORK_MODE mode, bool iconRefresh=false);
	void resetDisplayMode() { slot_OnStopDisplay(); }

	void bringVolume();

	void	syncFullscreen();
	mip::SCAMERA getCamera() { return m_camera; }
	mip::TRANSFORM getWorld() { return m_trVolumModel; }

	void useSkipRender(int index, bool res = true);

private:
	QMenu						*m_ContextMenu;
	QAction						*m_ActVR;
	QAction						*m_Act2DPoint;
	QAction						*m_ActRightRotate = nullptr;
	QAction						*m_ActLeftRotate = nullptr;
	QAction						*m_ActRotateStop = nullptr;
	bool						m_bPickingSagittal;
	bool						m_bPickingCoronal;

	bool						m_bMultislice;
	bool						m_bPickingModel;
	bool						m_bPickingManipulator;

	mip::VECTOR3				m_worldCoord;
	mip::VECTOR3				m_indexCoord;

	mip::VECTOR3				m_pointControl;

	std::vector<mip::VECTOR3> m_dragVectorList;
	std::vector<mip::VECTOR2> m_dragVectorScreen;

	int xminmax[2];
	int yminmax[2];

	mip::TRANSFORM				m_trPlaneImage;
	mip::TRANSFORM				m_trPlaneCut;

	mip::TRANSFORM					m_trVolumModel;
	mip::VECTOR3 m_vecCenter;

	AREA_STATE m_areaState;

	QPolygon					m_polyLine;		// screen 좌표계
	float						m_fElapsedTime;
	float						m_fAccumulatedTime;
	muint32						m_lAccumulatedFrame;

	QPushButton *m_btnRegionROI = nullptr;
	QPushButton *m_btnCurveSplit = nullptr;
	QPushButton *m_btnPlaneSplit = nullptr;
	QPushButton *m_btnAngleSplit = nullptr;

	QPushButton *m_btnStopDisplay = nullptr;
	QPushButton *m_btnFullScreen = nullptr;
public:
	mip::SCAMERA					m_camera;
	std::vector<mip::VECTOR3> m_planeList;

	void    reserveInit(HWND hwnd);
	void	resetUI();
	void	resetUICancel();
	void	setClipPrefer();
	void	renderModel(mip::MATRIX44 & world, mip::MATRIX44 & view, mip::MATRIX44 & proj, bool culling = true);
	void	renderVolume(bool skip_mode, mip::MATRIX44 & world, mip::MATRIX44 & view, mip::MATRIX44 & proj, bool vr_mode = false, mip::MATRIX44 * vr_view = NULL);
#ifdef _M_IX86
	void	initVTK();
	void	renderVolumeVTKData(vtkSmartPointer<vtkImageData> imageData, vtkSmartPointer<vtkShortArray> da);
	void	renderVolumeVTKDICOMReader(); // vtkDICOMImageReader 이용한 test 함수
	void	clearVTK();
#endif
	void	renderGuideBox(mip::MATRIX44 world, mip::MATRIX44 view, mip::MATRIX44 proj);
	void	renderCoordLine();
	void	drawMouseWork(QPainter *p);
	void	drawSeedPoints(QPainter *p);
	void	drawAnnotations(QPainter *p);
	bool	pickingMask(const mip::VECTOR2 & point, /*out*/mip::VECTOR3 & result, std::vector<std::pair<int, mask8>> &vecHitMaskBit = std::vector<std::pair<int, mask8>>());

	mip::MATRIX44 getHMDViewMatrix(muint32 ueye = 0, mip::MATRIX44 * matWorld = NULL);
	mip::MATRIX44 getHMDProjMatrix(muint32 ueye = 0);

	void	revolveCamera(bool right);
	QImage	getCaptureImage(bool, bool);
	QImage	getCaptureImage(QPolygon &polyLine);	// 입력된 사각형 사이즈로 캡쳐.

protected:
	//QPaintEngine *paintEngine() const { return 0; }

	void resizeEvent(QResizeEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void keyPressEvent(QKeyEvent *e) override;
	void keyReleaseEvent(QKeyEvent *e) override;

	void focusInEvent(QFocusEvent *) override;
	void wheelEvent(QWheelEvent *event) override;	
	bool eventFilter(QObject * target, QEvent * e) override;
	virtual void render(QPainter *painter);

	virtual void touchRelease();
	virtual void touchMove(mint32 preX, mint32 preY, mint32 currX, mint32 currY);
	virtual void touchZoom(float dt);

	int m_nIconSize = 0;

	virtual void dropEvent(QDropEvent *event) override;
	virtual void dragEnterEvent(QDragEnterEvent *event) override;

public:

	void setHomePosition(HOME_POSITION index, bool bForce = false);
public slots:
	void slot_OnVolume();
	void slot_OnLayer();
	void slot_OnFreeDrawROI();
	void slot_OnFullScreen();
	void slot_OnStopDisplay();
	void slot_OnRightDisplay();
	void slot_OnLeftDisplay();//
	void slot_OnPlaneSplit(bool checked = false);
	void slot_OnCurveSplit();
	void slot_OnAngleSplit();


public:
	QString getMoveMode();
#ifdef _M_IX86
	void showVTKWidget(bool show);
#endif

private:
	void renderScene(QPainter *p, bool skip_mode = false);
	void updateToolButtonIcon_As_WorkMode(QPushButton* button, WORK_MODE targetWorkMode, WORK_MODE mode, QIcon icon[], bool iconRefresh);

private:
	HWND	m_hwnd;

	QBasicTimer timer;
	QString text;
	int step;

	bool m_LbuttonDown;
	bool m_RbuttonDown;
	bool m_MbuttonDown;
	bool m_bVRPoint;
	bool m_bAxial;
	bool m_firstControlPressed;
	bool m_secondControlPressed;
	bool m_firstControlDpadPressed;

	bool m_bRightRotate;
	bool m_bLeftRotate;

	bool m_mouseMove;

	QPoint m_MousePos;
	QPoint m_preMousePos;
	QLabel *m_labelImage;
	QLabel *m_labelWater;
	QSize	m_size;

	QPushButton *m_btnVolume;
	QPushButton *m_btnLayer;

	float m_fCameraXZAngle;
	bool m_bWorkMode;

	bool	m_bSkip[4]; /*0 : ww, wl change, 1: object move, 2: object zoom, 3: alpha change*/
	QAbstractButton *HoverWidget;
	QIcon LeaveIcon;
#ifdef _M_IX86
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
	vtkSmartPointer<vtkRenderer> rendererVTK;
	vtkSmartPointer<vtkRenderWindow> renderWindowVTK;

	vtkSmartPointer<vtkVolumeProperty> volumeProperty;
	vtkSmartPointer<vtkPiecewiseFunction> gradientOpacity;
	vtkSmartPointer<vtkPiecewiseFunction> scalarOpacity;
	vtkSmartPointer<vtkColorTransferFunction> color;

	vtkSmartPointer<vtkVolume> volume;
	vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper> volumeMapper;

	vtkSmartPointer<vtkInteractorStyleSwitch> trackballStyle;

	QWidget*		m_pVTKWidget;
	QVBoxLayout*	m_pMainLayout;
#endif

	VOLUMEVIEW_RESET_UI_INFO m_resetUIInfo;

public:
	muint32 m_cullingTexture;

	mip::AABB	m_guidebox;

private slots:
	void slot_OnTo2DPoint();
	void slot_OnChecked();
	void slot_OnVRMode();
	void slot_OnDisplay();
	void slot_OnWheelTimeout();
	void slot_OnUpdateFrame();
};
#endif
