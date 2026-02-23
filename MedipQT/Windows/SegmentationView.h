#ifndef SEGMENTATIONVIEW_H
#define SEGMENTATIONVIEW_H

#include "WindowBase.h"
#include <QObject>

class SegmentationView : public WindowBase
{
	Q_OBJECT

public:
	SegmentationView(QWidget* parent, WINDOW_TYPE eType, MPRViewSharedInfo* pViewerSharedInfo = nullptr);
	virtual ~SegmentationView();
public:
	virtual void resetUI() override;
	virtual void setDepth(uint depth) override;

	virtual void hideControls() override;
	virtual void showControls() override;
	virtual void enableControls(bool bEnable) override;

	void setWorkMode(WORK_MODE mode, bool iconRefresh = false);

protected:
	virtual void initInScreenMenu() override;
	virtual void initInScreenRightTopMenu() override;
	virtual void initInScreenRightBottomMenu() override;
	virtual void initInScreenLeftBottomMenu() override;
	virtual QVector3D getShaderSliderDepthPosition() override;

	virtual void updateScreenMenu(int width, int height) override;

	virtual void render(QPainter* p);
	virtual void wheelEvent(QWheelEvent* ev) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	virtual void mouseDoubleClickEvent(QMouseEvent* e) override;

	virtual void focusInEvent(QFocusEvent*) override;
	virtual void focusOutEvent(QFocusEvent*) override;

	virtual void preDepth() override;
	virtual void nextDepth() override;

protected:
	bool eventFilter(QObject* target, QEvent* e) override;

protected:
	void processWork_MouseMove();
	void processWork_MousePress();
	void processWork_MouseRelease();
	void processWork_MouseDoubleClick(QMouseEvent* e);

private:
	void updateToolButtonIcon_As_WorkMode(QPushButton* button, WORK_MODE targetWorkMode, WORK_MODE mode, QIcon icon[], bool iconRefresh);
	void checkColorMap();

private slots:
	void	slot_OnPolyROI();
	void	slot_OnFreeDrawROI();
	void	slot_OnPickerROI();
	void	slot_OnAnotationText();
	void	slot_OnOvalROI();
	void	slot_OnRectROI();

private:
	QPushButton* m_btnPolyROI = nullptr;
	QPushButton* m_btnRegionROI = nullptr;
	QPushButton* m_btnPickerROI = nullptr;
	QPushButton* m_btnRectROI = nullptr;
	QPushButton* m_btnOvalROI = nullptr;
	QPushButton* m_btnAnoText = nullptr;
};

#endif // SEGMENTATIONVIEW_H
