#pragma once

#include "WindowBase.h"
#include "DataContext.h"

class Annotation;

class RadiomicsView : public WindowBase
{
	Q_OBJECT

public:
	RadiomicsView(QWidget *parent, WINDOW_TYPE winType, DataContext* pDataContext, MPRViewSharedInfo* pViewerSharedInfo = nullptr);
	virtual ~RadiomicsView();

public:
	virtual void enableControls(bool bEnable) override;
	virtual void resetUI() override;
	virtual void setDepth(uint depth) override;

	virtual void showControls() override;
	virtual void hideControls() override;
public:
	void setWorkMode(RADIOMICS_WORK_MODE mode, bool iconRefresh = false);

protected:
	virtual void initInScreenMenu() override;
	virtual void initInScreenRightTopMenu() override;
	virtual void initInScreenRightBottomMenu() override;
	virtual void initInScreenLeftBottomMenu() override;
	virtual void updateScreenMenu(int width, int height) override;
	virtual QVector3D getShaderSliderDepthPosition() override;

	virtual void render(QPainter *p) override;

	virtual void wheelEvent(QWheelEvent *ev) override;
	virtual void mouseMoveEvent(QMouseEvent *e) override;
	virtual void mousePressEvent(QMouseEvent *e) override;
	virtual void mouseReleaseEvent(QMouseEvent *e) override;
	virtual void mouseDoubleClickEvent(QMouseEvent * e) override;

	virtual bool eventFilter(QObject * target, QEvent * e) override;

	virtual void preDepth() override;
	virtual void nextDepth() override;

protected:
	void renderHeatMap(QPainter *p);
	void recon2DMap(float* pIn3DData, float* pOutData, unsigned int c2d_width, unsigned int c2d_height, unsigned int c2d_depth);

	void drawSlice_PET(QPainter* p);
	void drawAnnotations(QPainter *p);
	
protected:
	float getData_PET_SUV(const QPoint & point, muint32 depth);
	QPoint screenToPETVolumePosition(const QPoint * point);

	void processWork_MouseMove();
	void processWork_MousePress();
	void processWork_MouseRelease();
	void processWork_MouseDoubleClick(QMouseEvent *e);

	void mousePress_Annotation_InDrawingMode(RADIOMICS_WORK_MODE mode);
	void mousePress_Annotation_NormalMode();

	void mouseMove_Annotation_InDrawingMode();
	void mouseMove_Annotation_NormalMode();

public slots:
	void slot_OnAnnoRect();
	void slot_onHeightMap();

private:
	QPushButton *m_btnHeightMap = nullptr;
	QPushButton *m_btnAnnoRect = nullptr;

	std::unique_ptr<Annotation> m_pCurAnnotation;
	std::unique_ptr<Annotation> m_pOriginAnnotation;

	DataContext* m_pDataContext;
};

