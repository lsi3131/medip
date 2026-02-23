#include "stdafx.h"
#include "HeightMapView.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
#define PROGRAM_NORMAL_ATTRIBUTE 2

bool HeightMapView::m_transparent = false;

static const char *vertexShader =
"attribute vec4 posAttr;\n"
"attribute vec4 texCoord;\n"
"attribute vec3 normalAttr;\n"

"uniform mat4 matWorld;\n"
"uniform mat4 matView;\n"
"uniform mat4 matProj;\n"
"uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);\n"

"varying mediump vec4 texc;\n"

"varying vec3 N;\n"
"varying vec3 L;\n"
"varying vec3 V;\n"

"void main()\n"
"{\n"
"	texc = texCoord;\n"
"	// SB6 frament-phong\n"
"	// Calculate view-space coordinate\n"
"	mat4 matMV = matView * matWorld;\n"
"	vec4 P = matMV * posAttr;\n"

"	// // Calculate normal in view-space\n"
"	N = mat3(matMV) * normalAttr;\n"

"	// Calculate light vector\n"
"	L = light_pos - P.xyz;\n"

"	// // Calculate view vector\n"
"	gl_Position = matProj * matView * matWorld * posAttr;\n"
"}\n";

static const char* fragmentShader =
"uniform sampler2D texture;\n"
"uniform vec3 diffuse_albedo = vec3(-0.9, -0.9, -0.9);\n"
"uniform vec3 specular_albedo = vec3(0.5);\n"
"uniform float specular_power = 128.0;\n"
"uniform vec3 ambient = vec3(-0.1, -0.1, -0.1);\n"
"//varying vec3 color;\n"

"varying mediump vec4 texc;\n"
"varying vec3 N;\n"
"varying vec3 L;\n"
"varying vec3 V;\n"

"void main()\n"
"{\n"
"	// SB6 frament-phong\n"
"	// Normalize the incoming N, L and V vectors\n"
"	vec3 NN = normalize(N);\n"
"	vec3 LN = normalize(L);\n"
"	vec3 VN = normalize(V);\n"

"	// Calculate R locally\n"
"	vec3 R = reflect(-LN, NN);\n"

"	// Compute the diffuse and specular components for each fragment\n"
"	vec3 diffuse = max(dot(NN, LN), 0.0) * diffuse_albedo;\n"
"	vec3 specular = pow(max(dot(R, VN), 0.0), specular_power) * specular_albedo;\n"

"	// Write final color to the framebuffer\n"
"	vec4 color = vec4(ambient + diffuse + specular, 1.0);\n"

"	gl_FragColor = texture2D(texture, texc.xy) + color;\n"
"}\n";

HeightMapView::HeightMapView(QWidget *parent)
	: QOpenGLWidget(parent)
	, m_program(nullptr)
	, m_texture(nullptr)
	, m_frame(0)
	, m_pROI_Info(nullptr)
	, m_MousePos(QPoint(0, 0))
	, m_fZoom(-2.f)
{

	m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
	if (m_transparent) 
	{
		QSurfaceFormat fmt = format();
		fmt.setAlphaBufferSize(8);
		setFormat(fmt);
	}

	m_matProj.setToIdentity();
	m_matProj.perspective(45.0f, width() / height(), 0.01f, 100.0f);
	// init rotation
	//m_quatRot = QQuaternion::fromAxisAndAngle(QVector3D(1.f, 0.f, 0.f), -45.f);
}

HeightMapView::~HeightMapView()
{
	//qDebug() << " Height Map View Destructor" << endl;
	slot_cleanup();
}

void HeightMapView::slot_cleanup()
{
	if (m_program == nullptr)
		return;
	makeCurrent();

	if(m_texture)
		delete m_texture;

	if(m_program)
		delete m_program;
	m_program = 0;
	doneCurrent();
}

void HeightMapView::initializeGL()
{
	if (!m_pROI_Info)
		return;

	connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &HeightMapView::slot_cleanup);

	initializeOpenGLFunctions();
	glClearColor(0, 0, 0, m_transparent ? 0 : 1);
	
	glEnable(GL_DEPTH_TEST);

	m_program = new QOpenGLShaderProgram(this);
	m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader);
	m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader);
	//m_program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, "shader/vsHeightMap.glsl");
	//m_program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, "shader/fsHeightMap.glsl");
// 	m_program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
// 	m_program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	m_program->link();
	m_posAttr = m_program->attributeLocation("posAttr");
	//m_colAttr = m_program->attributeLocation("colAttr");
	m_textcoordAttr = m_program->attributeLocation("texCoord");
	m_normalAttr = m_program->attributeLocation("normalAttr");

	m_program->setUniformValue("texture", 0);
	m_matWorldUniform = m_program->uniformLocation("matWorld");
	m_matViewUniform = m_program->uniformLocation("matView");
	m_matProjUniform = m_program->uniformLocation("matProj");
		
	makeObject();
	makeNormals();
	makeTexture();
	
}

void HeightMapView::resizeGL(int w, int h)
{
	if (!m_pROI_Info)
		return;

	//QTransform
	m_matProj.setToIdentity();
	m_matProj.perspective(45.0f, w / float(h), 0.01f, 100.0f);
}

void HeightMapView::paintGL()
{
	if (!m_pROI_Info)
		return;
	if (m_vecVertices.size() == 0)
		return;

	// Draw the scene:
	const qreal retinaScale = devicePixelRatio();

	glViewport(0, 0, width() * retinaScale, height() * retinaScale);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_program->bind();

	// Calculate model view transformation	
	QMatrix4x4 matView;
	matView.translate(0.f, 0.f, m_fZoom);
	matView.rotate(m_quatRot);

	// object의 중앙으로 이동
	QMatrix4x4 matWorld;
	matWorld.translate(-0.5f, 0.5f, -0.5f);

	m_program->setUniformValue(m_matWorldUniform, matWorld);
	m_program->setUniformValue(m_matViewUniform, matView);
	m_program->setUniformValue(m_matProjUniform, m_matProj);

// 	vector<GLfloat> vecColors(m_vecVertices.size(), 0);
// 	for (int i = 0; i < m_vecVertices.size(); i++)
// 	{
// 		if (i % 9 == 0 || i % 9 == 4 || i % 9 == 8)
// 		{
// 			vecColors[i] = 1.f;
// 		}
// 	}

	// vbo
// 	m_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
// 	m_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
// 	m_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
// 	m_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));


	glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, &m_vecVertices[0]);
	glVertexAttribPointer(m_textcoordAttr, 2, GL_FLOAT, GL_FALSE, 0, &m_vecTexCoord[0]);
	glVertexAttribPointer(m_normalAttr, 3, GL_FLOAT, GL_FALSE, 0, &m_vecNormals[0]);

	glEnableVertexAttribArray(PROGRAM_VERTEX_ATTRIBUTE);
	glEnableVertexAttribArray(PROGRAM_TEXCOORD_ATTRIBUTE);
	glEnableVertexAttribArray(PROGRAM_NORMAL_ATTRIBUTE);

	m_texture->bind();
	glDrawArrays(GL_TRIANGLES, 0, m_vecVertices.size()/3);
// 
	glDisableVertexAttribArray(PROGRAM_NORMAL_ATTRIBUTE);
	glDisableVertexAttribArray(PROGRAM_TEXCOORD_ATTRIBUTE);
	glDisableVertexAttribArray(PROGRAM_VERTEX_ATTRIBUTE);

	m_program->release();
	
	++m_frame;
	
	update();
}


void HeightMapView::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() == Qt::LeftButton)
	{
		QVector2D preMousePos = m_MousePos;
		m_MousePos = QVector2D(event->pos());

		QVector2D diff = m_MousePos - preMousePos;
		QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

		qreal acc = diff.length();// / 100.0;

		m_rotationAxis = (m_rotationAxis + n * acc).normalized();

		m_quatRot = QQuaternion::fromAxisAndAngle(m_rotationAxis, acc) * m_quatRot;
	}
}

void HeightMapView::mousePressEvent(QMouseEvent *event)
{
	if (event->buttons() == Qt::LeftButton)
	{
		m_MousePos = QVector2D(event->pos());
	}
}

void HeightMapView::mouseReleaseEvent(QMouseEvent *event)
{
}

void HeightMapView::wheelEvent(QWheelEvent *event)
{
	m_fZoom += 0.001 * (float)event->delta();
	
}

void HeightMapView::makeObject(void)
{
	//// build vertex
	for (int i = 0; i < m_pROI_Info->getRow(); i++)
	{
		for (int j = 0; j < m_pROI_Info->getColumn(); j++)
		{
			// vertex
			vector<GLfloat> vecSrc = m_pROI_Info->makeHeightFace(j, i);

			int nsize = m_vecVertices.size();
			m_vecVertices.resize((GLfloat)(nsize + vecSrc.size()));
			std::copy(vecSrc.begin(), vecSrc.end(), m_vecVertices.begin() + nsize);

		}
	}
	int a = 0;
}

void HeightMapView::makeNormals(void)
{
	for (int i = 0; i < m_vecVertices.size(); i += 9)
	{
		QVector3D vec3_1(m_vecVertices[i], m_vecVertices[i + 1], m_vecVertices[i + 2]);
		QVector3D vec3_2(m_vecVertices[i + 3], m_vecVertices[i + 4], m_vecVertices[i + 5]);
		QVector3D vec3_3(m_vecVertices[i + 6], m_vecVertices[i + 7], m_vecVertices[i + 8]);
		QVector3D edge_1(vec3_2 - vec3_1);
		QVector3D edge_2(vec3_3 - vec3_2);
		QVector3D edge_3(vec3_1 - vec3_3);

		QVector3D normal = QVector3D::crossProduct(edge_1, edge_2);
		normal.normalize();
		m_vecNormals.push_back(normal.x());
		m_vecNormals.push_back(normal.y());
		m_vecNormals.push_back(normal.z());

		normal = QVector3D::crossProduct(edge_2, edge_3);
		normal.normalize();
		m_vecNormals.push_back(normal.x());
		m_vecNormals.push_back(normal.y());
		m_vecNormals.push_back(normal.z());

		normal = QVector3D::crossProduct(edge_3, edge_1);
		normal.normalize();
		m_vecNormals.push_back(normal.x());
		m_vecNormals.push_back(normal.y());
		m_vecNormals.push_back(normal.z());
	}
}

void HeightMapView::makeTexture(void)
{
	// make m_vecTexCoord
	for (int i = 0; i < m_vecVertices.size(); i += 3)
	{
		float fx, fy;
		fx = m_vecVertices[i];
		fy = m_vecVertices[i + 2];

		m_vecTexCoord.push_back(fx);
		m_vecTexCoord.push_back(fy);
	}

	// make color table
	m_texture = new QOpenGLTexture(m_ColorTableImg);

// 	int m = 5;
// 	int n = 5;
// 
// 	QImage image(m, n, QImage::Format_RGB32);
// 	//QRgb value;
// 
// 	for (int i = 0; i < n; i++)
// 	{
// 		int red = 0, grn = 0, blu = 0;
// 		switch (i%n)
// 		{
// 		case 4: red = 255;    grn = 0;      blu = 0;    break;//red
// 		case 3: red = 255;	  grn = 255;    blu = 0;    break;//yellow
// 		case 2: red = 0;      grn = 255;    blu = 0;	break;//green
// 		case 1: red = 0;      grn = 255;    blu = 255;	break;//cyan
// 		case 0: red = 0;      grn = 0;      blu = 255;    break;//blue
// 		default:
// 			break;
// 		}
// 
// 		for (int j = 0; j < m; j++)
// 		{
// 			image.setPixel(j, i, qRgb(red, grn, blu));
// 		}
// 	}
// 
// 	m_texture = new QOpenGLTexture(image);

}

