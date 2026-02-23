#pragma once

#include "glWidget.h"
#include "graphics/color.h"
#include "graphics/BoundingBox.h"
#include "ColorTable.h"
#include <QVector3d>
#include <QIcon>
#include <QSlider>

class QMenu;
class AnnoControlDlg;
class QAbstractButton;
class DataContext;

struct ViewerButtonInfo
{
	QPushButton* Button;
	QIcon IconNormal;
	QIcon IconHover;
	QIcon IconPressed;
	WORK_MODE WorkMode;
};

struct MPRViewSharedInfo
{
	QVector3D VolumeVoxelPosition;
	mint16 HU = 0;

	WINDOW_TYPE winType;
	uint depth;
};

class WindowBase : public OpenGLWidget
{
	Q_OBJECT

public:
	WindowBase(QWidget* parent, MPRViewSharedInfo* pViewerSharedInfo = nullptr);
	virtual ~WindowBase();

public:
	void init(DataContext* pDataContext);
	void resetResource();

	void syncFullscreen();
	void InitWheelSliderFunc(bool set);
	QImage getCaptureImage(bool, bool);

	void renderHeatMap(QPainter* p);
	void recon2DMap(std::vector<unsigned char>& vecInData, std::vector<unsigned char>& vecOutData, unsigned int c2d_width, unsigned int c2d_height, unsigned int c2d_depth);

	bool isFullScreen();
	void setFullScreenDelegate(QWidget* root);

	WINDOW_TYPE getType();
	void setType(WINDOW_TYPE type);

	QRect getPosition();
	mint16 getData(const QPoint& point, muint32 depth);
	QPoint screenToVolumePosition(const QPoint* point);
	QPointF screenToVolumePositionF(const QPointF* point);
	QVector3D screenToVolumePosition3(const QPoint* point);
	QVector3D screenToCoord3(const QPoint* point);
	void moveSlicePosition(int dx, int dy);

	void createCirclePointList(muint32 sizeEllipse);
	void createCursorShape(bool _auto = false);

	void drawMeshOutline(QPainter* p);
	void drawAniLine(QPainter* p);
	void drawSlice(QPainter* p);
	void drawMask(QPainter* p, unsigned char m, int mI = 0, COLOR color = COLOR(0, 255, 0, 255));
	void drawMask(QPainter* p);
	void drawMaskAll(QPainter* p, bool forceUpdate = false);
	void drawSquareLine(QPainter* p, bool drawcut = false);
	void drawUnloaded(QPainter* p);
	void drawBoundLine(QPainter* p);
	void drawMouseWork(QPainter* p);
	void drawCoordLine(QPainter* p);
	void drawAnnotations(QPainter* p);
	void drawSeedTip(QPainter* p);
	void drawDirection(int startWidth, QPainter* p);
	void drawBoundPoints(QPainter* p);
	void drawPreview(QPainter* p);
	void drawText(QPainter* p, QPoint pt, QColor color, QString text, QRect* pOutDrawRectArea = nullptr);
	void drawCoordAndUnitTypeText(QPainter* p, QPoint pt, QColor color, QVector3D vec, mint16 HU, QString unitText, QRect* pOutDrawRectArea = nullptr, QString appendText = "");
	void drawWorkModeText(QPainter* p, QPoint pt, QColor color, QRect* pOutDrawRectArea = nullptr);
	void drawThresholdModeText(QPainter* p, QPoint pt, QColor color, QRect* pOutDrawRectArea = nullptr);
	void appendTextWithComma(QString* pOutAppendText, const QString& appendText);
	void calcVolumePreview();
	QString getDepthString(bool isFront = true);

	void updateMouseActionMode();

	uint getDepth(bool flip = false);
	uint getLatestDepth();
	uint getAniDepth(int index, bool isLine = true, bool isSpiral = true);
	float getfDepth(bool flip = false);

	void updatePlaneDatas();
	void updateSliderPosition();

	void setShowBoundingBoxMode(bool value);
	void setBoundingBoxEdgeMode(bool value);

	void InitZoomFactor(float x, float y, bool isUpdate = false);
	void setZoomFactor(float x, float y);
	void SetZoomFactorByStepNum(float fStepNumX, float fStepNumY);
	void GetZoomStepNum(float& fStepNumX, float& fStepNumY);

	void resetSlicePosition();
	void setSlicePosition(int x, int y);

	int getSlicePositionX();
	int getSlicePositionY();
	float getZoomFactorX();
	float getZoomFactorY();
	float getFactorX();
	float getFactorY();
	QVector3D getLatestClickPos();

	bool getFlipY();
	bool getFlipX();

	bool isAnnoEditMode();
	void setAnnoEditMode(bool editmode);
	void checkVolumeArea(QVector3D& area);
	QVector3D volumeToScreenPosition(int volx, int voly, int volz, bool scale = true);
	QVector3D volumeToScreenPositionAuto(muint32 x, muint32 y, muint32 depth, bool scale = true);
	int volumeToScreenDepth(QVector3D);

	void setDutyCheck();
	void forceUpdateSliceColorData();

	void setShowBounding(bool show);

	void calcMesh2DOutline();

	void updateMeshOutline();

public:
	virtual void setDepth(uint depth);
	virtual void setfDepth(float depth);
	virtual void preDepth();
	virtual void nextDepth();

protected:
	virtual void initInScreenMenu();
	virtual void initInScreenRightTopMenu();
	virtual void initInScreenRightBottomMenu();
	virtual void initInScreenLeftBottomMenu();
	virtual void updateScreenMenu(int width, int height);

	virtual QVector3D getShaderSliderDepthPosition() { return QVector3D(); }
public:
	virtual void hideControls() override;
	virtual void showControls() override;
	virtual void enableControls(bool bEnable);

protected:
	virtual bool eventFilter(QObject* target, QEvent* e) override;

	virtual void render(QPainter* p) override;
	virtual void dropEvent(QDropEvent* event) override;
	virtual void dragEnterEvent(QDragEnterEvent* ev) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	virtual void mouseDoubleClickEvent(QMouseEvent* e)override;
	virtual void focusInEvent(QFocusEvent*) override;
	virtual void focusOutEvent(QFocusEvent*) override;
	virtual void keyPressEvent(QKeyEvent* e) override;
	virtual void keyReleaseEvent(QKeyEvent* e) override;

	virtual void resizeEvent(QResizeEvent* event) override;

	virtual void touchMove(float dx, float dy) override;
	virtual void touchZoom(float dt) override;

	virtual void processSlideZ(float dt, bool isSlider = false, bool isRotate = false);

protected:
	void zoom(float dt, float factorX, float factorY);
	void zoomSlider(int val, float factorX, float factorY);
	void updateSliceColorData(bool forceUpdate = false);
	QCursor getBitmapCursor();
	void updateSliceMaskData(unsigned char m, int mI, COLOR _color, BoundingBoxI& maskArea, bool forceUpdate = false, COLOR* buffer = NULL);
	void setContextMenu();
	bool isShowUsage();
	void setOtherMPRPlaneDepth_InMouseEvent(MAINTAB_TYPE type);
	bool isMouseEventAndContainPos(QObject* target, QEvent* e);
	bool checkButtonHoverStatus(QObject* target, QPushButton* button);
	void setButtonHoverStatus(QPushButton* button, const QIcon& iconNormal, const QIcon& iconHover);
	void registerViewerButtonInfo(QPushButton* button, const QIcon& iconNormal, const QIcon& iconHover, const QIcon& iconPressed);

	void updateViewerShaderedInfo();
private:
	void updateZoomInfo();

signals:
	void setFullScreen(WindowBase* window);
	void PlayAnimate(bool);

public slots:
	void slot_OnFullScreen();
	void slot_OnShowBounding();
	void slot_OnShowBoundaryEdge();

	void slot_OnDepthSlideChanged(int value);
	void slot_OnDepthSlideReleased();

protected slots:
	void slot_OnL3Modify();
	void slot_OnAWModify();

private slots:
	void slot_OnContextSeed();
	void slot_OnContextSeedShow();
	void slot_OnContextAnnoList();
	void slot_OnContextSeedApply();
	void slot_OnContextRangeApply();
	void slot_OnContextAniPoints();
	void slot_OnContextAnnoText();
	void slot_OnWheelTimeout();

protected:
	static bool m_bWorkMode;

	QPoint m_globalPos;
	QPoint m_preMousePos;
	QPoint m_MousePos;
	QPoint m_moveOffset;
	QPoint m_preMovePos;
	QVector3D m_latestClickPos;
	bool m_LbuttonDown;
	bool m_MbuttonDown;
	bool m_RbuttonDown;
	bool m_mouseDown;
	bool m_annoEditMode;
	bool m_dutyCheck;
	WINDOW_TYPE m_windowType;
	QWidget* m_rootWidget;

	QPolygon m_polyLine;
	QPainterPath m_rectRegion;

	bool m_focus;
	bool m_fullscreen;
	QString m_key;

	uint m_depth;
	uint m_preDepth;
	uint m_latestClickDepth;
	float m_fdepth;
	float m_zoomFactorX; // spacing 을 기반으로한 확대축소 배율값(zoom값)
	float m_zoomFactorY;
	float m_fZoomStepX;  // zoom의 단위 step 값
	float m_fZoomStepY;

	float m_fMinZoomFactorX;
	float m_fMinZoomFactorY;
	float m_fMaxZoomFactorX;
	float m_fMaxZoomFactorY;
	int m_nZoomStepCnt = 100;
	int m_nMinZoomNum = 5;

	float m_fZoomLow = 0.0f;

	QPoint m_SlicePosition;
	bool m_polyProcessCheck;

	QPolygon m_list;

	MOUSE_ACTION_MODE m_mouse_action_mode;
	MOUSE_ACTION_MODE m_pre_mouse_action_mode;

	bool m_flipY;
	bool m_flipX;
	bool m_showRightMenu;

	QPushButton* m_btnFullScreen = nullptr;
	QPushButton* m_btnBoundaryEdge = nullptr;
	QPushButton* m_btnShowBounding = nullptr;

	// DeepCatch 전용
	QPushButton* m_btnL3Modify = nullptr;
	QPushButton* m_btnAbdominalWaistModify = nullptr;

	QMenu* m_pContextMenu;
	QAction* m_pActSeedPoint;
	QAction* m_pActSeedShow;
	QAction* m_pActAnnoList;
	QAction* m_pActSeedApply;
	QAction* m_pActRangeApply;
	QAction* m_pActShowAniPoint;
	QAction* m_pActShowAnnoText;

	bool m_bMoveMouse;

	COLOR* m_colorData;
	COLOR* m_maskData;

	BoundingBoxI m_stOrgBox;

	QSlider* m_slidebar;

	ColorTable* m_pColorTable = nullptr;
	QImage m_heightMapColorTable;

	int m_nIconSize = 0;

	QAbstractButton* m_pHoverWidget;
	QIcon m_IconLeave;

	bool m_bIsMaskDraw = true;

	QPoint m_StartPos;
	QPoint m_MovePos;

	std::vector<ViewerButtonInfo> m_viewerButtonInfos;
	MPRViewSharedInfo* m_pViewerSharedInfo;

	DataContext* m_pDataContext;

private:
	float m_fSpacingX = 0.f;
	float m_fSpacingY = 0.f;
	bool m_bUpdateMeshOutline;

};
