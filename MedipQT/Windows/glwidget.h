#pragma once

#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include "define.h"
#include <qwidget.h>
#include "Math/Transform.h"
//#include "math/math.h"
class QPushButton;
class QPainter;
class QEvent;
class QGestureEvent;
class QSwipeGesture;
class QPanGesture;
class QPinchGesture;
class QTimer;
class QInputEvent;

class OpenGLWidget : public QWidget
{
	Q_OBJECT

public:
	OpenGLWidget(QWidget* parent);

	WINDOW_TYPE getType();
	void setUpdateFrame(bool value);
	bool getUpdateFrameState();
	QPainter* getCurrentPainter();
	bool getFullscreen() const;

public:
	virtual void resetUI() {};
	virtual void reInit() {};
	virtual mip::VECTOR3 getLocalToScreen(float x, float y, float z) { return mip::VECTOR3(); };

	virtual void hideControls() {};
	virtual void showControls() {};
	virtual void enableControls(bool bEnable) {};

	virtual mip::VECTOR3 getScreenToProj(float x, float y) { return mip::VECTOR3(); };
	virtual mip::MATRIX44 getWVP() { return mip::MATRIX44(); };

protected:
	void drawWaterMark(QPainter* p);
	void checkModifiers(QKeyEvent* e, bool _press = true, bool* chkRender = NULL);
	void checkModifiers(QMouseEvent* e, bool* chkRender = NULL);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual bool event(QEvent* event) override;

protected:
	virtual void touchRelease() {};
	virtual void touchMove(float dx, float dy) {};
	virtual void touchMove(mint32 preX, mint32 preY, mint32 currX, mint32 currY) {};
	virtual void touchZoom(float dt) {};

	virtual void render(QPainter* p);

public slots:
	void animate();
	void renderLater();

signals:
	void setFullScreen(OpenGLWidget* window);

protected:
	WINDOW_TYPE m_windowType;
	QCursor	m_cursor;

	bool m_fullscreen;
	bool m_updateFrameSet;
	bool m_useOpenGL;
	bool m_touchEvent;
	bool m_touchZoomEvent;

	bool m_ctrl;
	bool m_alt;
	bool m_shift;

	QTimer* m_WheelTimer;

private:
	int m_elapsed;
	QPainter* m_tempPainter;

	QPointF m_preTouchPoint[3];
	QPointF m_currentTouchPoint[3];
};
#endif