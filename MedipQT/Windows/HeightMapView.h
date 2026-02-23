#ifndef HEIGHTMAPVIEW_H
#define HEIGHTMAPVIEW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "HeightMapInfo.h"

#include "math/math.h"


class HeightMapView : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	HeightMapView(QWidget *parent);
	virtual ~HeightMapView();
	
	void setDataObj(CHeightMapInfo *pObj) { m_pROI_Info = pObj; }
	void setColorTable(QImage img) { m_ColorTableImg = img;  }
	

public slots:
	void slot_cleanup();

protected:
	void initializeGL() override;	
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	
	void wheelEvent(QWheelEvent *event);

	void makeObject(void);
	void makeNormals(void);
	void makeTexture(void);

private:
 	//QOpenGLBuffer m_vbo;
 	QOpenGLShaderProgram *m_program;
 	QOpenGLTexture *m_texture;
	
	GLuint m_posAttr;
	GLuint m_textcoordAttr;
	GLuint m_normalAttr;

	GLuint m_matWorldUniform;
	GLuint m_matViewUniform;
	GLuint m_matProjUniform;

	vector<GLfloat> m_vecVertices;
	vector<GLfloat> m_vecNormals;
	vector<GLfloat> m_vecTexCoord;
	//////////////////////////////
	QVector2D m_MousePos;
	QVector3D m_rotationAxis;
	QQuaternion m_quatRot;

	QMatrix4x4 m_matProj;
	float		m_fZoom;

	//////////////////////////////
	bool m_core;
	static bool m_transparent;

	int m_frame;

	CHeightMapInfo *m_pROI_Info;
	QImage m_ColorTableImg;

	//////////////////////////////

	

	
	
};

#endif // HEIGHTMAPVIEW_H
