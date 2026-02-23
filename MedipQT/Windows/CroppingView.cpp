#include "stdafx.h"
#include "CroppingView.h"


CroppingView::CroppingView(QWidget* parent, WINDOW_TYPE eWinType)
	:QWidget(parent),
	m_depth(0),
	m_cx(0),
	m_cy(0),
	m_cz(0),
	m_imageWidth(0),
	m_imageHeight(0),
	m_imageDepth(0),
	m_pTexture(nullptr),
	m_pData_HU(nullptr)
{
	this->setMouseTracking(true);

	m_eWinType = eWinType;

	m_window_width = 85;
	m_window_level = 41;

	m_zoomFactorX = 1.0f;
	m_zoomFactorY = 1.0f;

	m_boundingBox.reset();
	m_beforeClicked.reset();
	m_mouse_action_mode = MAM_NONE;

	m_LbuttonDown = false;
}

CroppingView::~CroppingView()
{
	destroyData();
}

void CroppingView::paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);

	render(&painter);

	painter.end();
}

void CroppingView::wheelEvent(QWheelEvent *ev)
{
	this->setFocus();

	if (ev->angleDelta().ry() > 0)
	{
		if (m_zoomFactorX + m_fZoomStepX <= m_fMaxZoomFactorX && m_zoomFactorY + m_fZoomStepY <= m_fMaxZoomFactorY)
		{
			m_zoomFactorX += m_fZoomStepX;
			m_zoomFactorY += m_fZoomStepY;
		}
	}
	else
	{
		if (m_zoomFactorX - m_fZoomStepX >= m_fMinZoomFactorX && m_zoomFactorY - m_fZoomStepY >= m_fMinZoomFactorY)
		{
			m_zoomFactorX -= m_fZoomStepX;
			m_zoomFactorY -= m_fZoomStepY;
		}
	}
	renderLater();
}

void CroppingView::mousePressEvent(QMouseEvent *e)
{
	if (e->buttons() == Qt::LeftButton)
	{
		m_beforeClicked = m_boundingBox;
		m_LbuttonDown = true;

		if (m_mouse_action_mode & MAM_BOUNDING_MAXX_OVER)
			m_mouse_action_mode |= MAM_BOUNDING_MAXX_MOVE;
		else if (m_mouse_action_mode & MAM_BOUNDING_MINX_OVER)
			m_mouse_action_mode |= MAM_BOUNDING_MINX_MOVE;

		if (m_mouse_action_mode & MAM_BOUNDING_MAXY_OVER)
			m_mouse_action_mode |= MAM_BOUNDING_MAXY_MOVE;
		else if (m_mouse_action_mode & MAM_BOUNDING_MINY_OVER)
			m_mouse_action_mode |= MAM_BOUNDING_MINY_MOVE;
	}
}

void CroppingView::mouseMoveEvent(QMouseEvent *e)
{
	this->setFocus();

	QRect rect = getPostion();

	if (m_mouse_action_mode >= MAM_BOUNDING_MOVE)
	{
		if (m_zoomFactorX == 0 || m_zoomFactorY == 0)
			return;

		int x = (e->pos().x() - rect.x()) / m_zoomFactorX;
		int y = (e->pos().y() - rect.y()) / m_zoomFactorY;

		if (m_eWinType == WT_AXIAL)
		{
			if (m_mouse_action_mode & MAM_BOUNDING_MAXX_MOVE)
				m_boundingBox.maxX = x;
			else if (m_mouse_action_mode & MAM_BOUNDING_MINX_MOVE)
				m_boundingBox.minX = x;

			if (m_mouse_action_mode & MAM_BOUNDING_MAXY_MOVE)
				m_boundingBox.maxY = y;
			else if (m_mouse_action_mode & MAM_BOUNDING_MINY_MOVE)
				m_boundingBox.minY = y;
		}
		else if (m_eWinType == WT_CORONAL)
		{
			if (m_mouse_action_mode & MAM_BOUNDING_MAXX_MOVE)
				m_boundingBox.maxX = x;
			else if (m_mouse_action_mode & MAM_BOUNDING_MINX_MOVE)
				m_boundingBox.minX = x;

			if (m_mouse_action_mode & MAM_BOUNDING_MAXY_MOVE)
				m_boundingBox.maxZ = y;
			else if (m_mouse_action_mode & MAM_BOUNDING_MINY_MOVE)
				m_boundingBox.minZ = y;
		}
		else if (m_eWinType == WT_SAGITTAL)
		{
			if (m_mouse_action_mode & MAM_BOUNDING_MAXX_MOVE)
				m_boundingBox.maxY = x;
			else if (m_mouse_action_mode & MAM_BOUNDING_MINX_MOVE)
				m_boundingBox.minY = x;

			if (m_mouse_action_mode & MAM_BOUNDING_MAXY_MOVE)
				m_boundingBox.maxZ = y;
			else if (m_mouse_action_mode & MAM_BOUNDING_MINY_MOVE)
				m_boundingBox.minZ = y;
		}
	}
	else
	{
		BoundingBoxI box = m_boundingBox;

		if (m_eWinType == WT_AXIAL)
		{
			box.minX = rect.x() + m_boundingBox.minX * m_zoomFactorX;
			box.maxX = rect.x() + m_boundingBox.maxX * m_zoomFactorX;
			box.minY = rect.y() + m_boundingBox.minY * m_zoomFactorY;
			box.maxY = rect.y() + m_boundingBox.maxY * m_zoomFactorY;
		}
		else if (m_eWinType == WT_CORONAL)
		{
			box.minX = rect.x() + m_boundingBox.minX * m_zoomFactorX;
			box.maxX = rect.x() + m_boundingBox.maxX * m_zoomFactorX;
			box.minY = rect.y() + m_boundingBox.minZ * m_zoomFactorY;
			box.maxY = rect.y() + m_boundingBox.maxZ * m_zoomFactorY;
		}
		else if (m_eWinType == WT_SAGITTAL)
		{
			box.minX = rect.x() + m_boundingBox.minY * m_zoomFactorX;
			box.maxX = rect.x() + m_boundingBox.maxY * m_zoomFactorX;
			box.minY = rect.y() + m_boundingBox.minZ * m_zoomFactorY;
			box.maxY = rect.y() + m_boundingBox.maxZ * m_zoomFactorY;
		}

		m_mouse_action_mode = MAM_NONE;

		if (e->pos().x() > (box.maxX - PEN_SIZE) && e->pos().x() < (box.maxX + PEN_SIZE))
			m_mouse_action_mode |= MAM_BOUNDING_MAXX_OVER;
		else if (e->pos().x() > (box.minX - PEN_SIZE) && e->pos().x() < (box.minX + PEN_SIZE))
			m_mouse_action_mode |= MAM_BOUNDING_MINX_OVER;

		if (e->pos().y() > (box.maxY - PEN_SIZE) && e->pos().y() < (box.maxY + PEN_SIZE))
			m_mouse_action_mode |= MAM_BOUNDING_MAXY_OVER;
		else if (e->pos().y() > (box.minY - PEN_SIZE) && e->pos().y() < (box.minY + PEN_SIZE))
			m_mouse_action_mode |= MAM_BOUNDING_MINY_OVER;
	}
	renderLater();
}

void CroppingView::mouseReleaseEvent(QMouseEvent *e)
{
	if (m_LbuttonDown)
	{
		if (m_boundingBox.maxX < m_boundingBox.minX || m_boundingBox.maxY < m_boundingBox.minY || m_boundingBox.maxZ < m_boundingBox.minZ)
		{
			m_boundingBox = m_beforeClicked;
		}

		if (m_eWinType == WT_AXIAL)
		{
			if (m_imageWidth <= m_boundingBox.maxX)
			{
				m_boundingBox.maxX = m_imageWidth - 1;
			}
			else if (0 > m_boundingBox.minX)
			{
				m_boundingBox.minX = 0;
			}

			if (m_imageHeight <= m_boundingBox.maxY)
			{
				m_boundingBox.maxY = m_imageHeight - 1;
			}
			else if (0 > m_boundingBox.minY)
			{
				m_boundingBox.minY = 0;
			}
		}
		else if (m_eWinType == WT_CORONAL)
		{
			if (m_imageWidth <= m_boundingBox.maxX)
			{
				m_boundingBox.maxX = m_imageWidth - 1;
			}
			else if (0 > m_boundingBox.minX)
			{
				m_boundingBox.minX = 0;
			}

			if (m_imageHeight <= m_boundingBox.maxZ)
			{
				m_boundingBox.maxZ = m_imageHeight - 1;
			}
			else if (0 > m_boundingBox.minZ)
			{
				m_boundingBox.minZ = 0;
			}
		}
		else if (m_eWinType == WT_SAGITTAL)
		{
			if (m_imageWidth <= m_boundingBox.maxY)
			{
				m_boundingBox.maxY = m_imageWidth - 1;
			}
			else if (0 > m_boundingBox.minY)
			{
				m_boundingBox.minY = 0;
			}

			if (m_imageHeight <= m_boundingBox.maxZ)
			{
				m_boundingBox.maxZ = m_imageHeight - 1;
			}
			else if (0 > m_boundingBox.minZ)
			{
				m_boundingBox.minZ = 0;
			}
		}

		m_mouse_action_mode = MAM_NONE;
		m_LbuttonDown = false;

		emit sig_BoxUpdate(m_eWinType);
	}
	renderLater();
}

void CroppingView::createData(int cx, int cy, int cz, int width, int level, mint16* pHUdata)
{
	m_cx = cx;
	m_cy = cy;
	m_cz = cz;

	if (m_eWinType == WT_AXIAL)
	{
		m_imageWidth = cx;
		m_imageHeight = cy;
		m_imageDepth = cz;
	}
	else if (m_eWinType == WT_CORONAL)
	{
		m_imageWidth = cx;
		m_imageHeight = cz;
		m_imageDepth = cy;
	}
	else if (m_eWinType == WT_SAGITTAL)
	{
		m_imageWidth = cy;
		m_imageHeight = cz;
		m_imageDepth = cx;
	}
	m_depth = m_imageDepth / 2;

	m_boundingBox.reset(cx, cy, cz);

	m_zoomFactorX = 1.0f;
	m_zoomFactorY = 1.0f;

	m_window_width = width;
	m_window_level = level;

	m_pData_HU = pHUdata;
	m_pTexture = new uchar[m_imageWidth * m_imageHeight];

	if (m_imageWidth > this->size().width() || m_imageHeight > this->size().height())
	{
		m_zoomFactorX = m_zoomFactorX / pow(2, (m_imageWidth / this->size().width()));
		m_zoomFactorY = m_zoomFactorY / pow(2, (m_imageHeight / this->size().height()));
	}
}

void CroppingView::destroyData()
{
	SAFE_DELETES(m_pTexture);
}

void CroppingView::initTexture(void)
{
	updateTexture();
	updateImageByTexture();
}

void CroppingView::updateTexture()
{
	int n3Dcx = m_cx;
	int n3Dcy = m_cy;
	int n3Dcz = m_cz;

	int n3D_X_coeff = 0, n3D_Y_coeff = 0, n3D_Z_coeff = 0;
	int dx = 0, dy = 0;
	int n2d_width = 0, n2d_height = 0;
	int nInitX = 0, nInitY = 0, nExitX = 0, nExitY = 0;

	if (m_eWinType == WT_AXIAL)
	{
		n3D_X_coeff = 1;
		n3D_Y_coeff = n3Dcx;
		n3D_Z_coeff = n3Dcy*n3Dcx;

		n2d_width = n3Dcx;
		n2d_height = n3Dcy;

		nInitX = 0;
		nInitY = 0;
		nExitX = n2d_width;
		nExitY = n2d_height;
		dx = dy = 1;
	}
	else if (m_eWinType == WT_CORONAL)
	{
		n3D_X_coeff = 1;
		n3D_Y_coeff = n3Dcy*n3Dcx;	// 방향 반대	// z방향
		n3D_Z_coeff = n3Dcx;

		n2d_width = n3Dcx;
		n2d_height = n3Dcz;

		nInitX = 0;
		nInitY = n2d_height - 1;
		nExitX = n2d_width;
		nExitY = 0;
		dx = 1;
		dy = -1;
	}
	else if (m_eWinType == WT_SAGITTAL)
	{
		n3D_X_coeff = n3Dcx;	// 방향반대
		n3D_Y_coeff = n3Dcy*n3Dcx;	// 방향반대
		n3D_Z_coeff = 1;

		n2d_width = n3Dcy;
		n2d_height = n3Dcz;

		nInitX = n2d_width - 1;
		nInitY = n2d_height - 1;
		nExitX = 0;
		nExitY = 0;
		dx = -1;
		dy = -1;
	}

	for (int y = 0, y2d = nInitY; y < n2d_height; ++y, y2d += dy)
	{
		for (int x = 0; x < n2d_width; ++x)
		{
			int index3D = m_depth * n3D_Z_coeff + y * n3D_Y_coeff + x * n3D_X_coeff;
			int index2D = y2d * n2d_width + x;

			muint8 color = 0;
			if (m_pData_HU[index3D] <= (m_window_level - m_window_width / 2))
				color = 0;
			else if (m_pData_HU[index3D] >= (m_window_level + m_window_width / 2))
				color = 255;
			else
				color = 255 * (m_pData_HU[index3D] - (m_window_level - m_window_width / 2)) / m_window_width;

			m_pTexture[index2D] = color;
		}
	}
}

void CroppingView::updateImageByTexture()
{
	QImage tmpImage = QImage((uchar*)m_pTexture, m_imageWidth, m_imageHeight, QImage::Format_Grayscale8);
	QRect rect = getPostion();

	m_image = tmpImage.scaled(rect.width(), rect.height());
}

void CroppingView::initZoomFactor(float fSpacingX, float fSpacingY, bool isUpdate)
{
	int w = this->width();
	int h = this->height();

	float fZoomX = (float)w / ((float)m_imageWidth * fSpacingX);
	float fZoomY = (float)h / ((float)m_imageHeight * fSpacingY);

	float fZoom = 0.f;
	if ((fZoomX) > (fZoomY))
		fZoom = fZoomY;
	else
		fZoom = fZoomX;


	if (isUpdate)
	{
		float fOldZoomCntX = m_zoomFactorX / m_fZoomStepX;
		float fOldZoomCntY = m_zoomFactorY / m_fZoomStepY;

		m_zoomFactorX = fZoom * fSpacingX;
		m_zoomFactorY = fZoom * fSpacingY;
		m_fZoomStepX = m_zoomFactorX  *  0.1f;
		m_fZoomStepY = m_zoomFactorY  *  0.1f;

		m_fMinZoomFactorX = m_zoomFactorX - (m_fZoomStepX * (float)m_nMinZoomNum);
		m_fMinZoomFactorY = m_zoomFactorY - (m_fZoomStepY * (float)m_nMinZoomNum);

		m_fMaxZoomFactorX = m_zoomFactorX + (m_fZoomStepX * (float)(m_nZoomStepCnt - 10));
		m_fMaxZoomFactorY = m_zoomFactorY + (m_fZoomStepY * (float)(m_nZoomStepCnt - 10));

		// init 값
		m_zoomFactorX += m_fZoomStepX * (fOldZoomCntX - (float)10);
		m_zoomFactorY += m_fZoomStepY * (fOldZoomCntY - (float)10);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//0123456789ABCDEF~~~100
		//        0      1               95  
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		int a = 0;

	}
	else
	{
		m_zoomFactorX = fZoom * fSpacingX;
		m_zoomFactorY = fZoom * fSpacingY;
		m_fZoomStepX = m_zoomFactorX  *  0.1f;
		m_fZoomStepY = m_zoomFactorY  *  0.1f;

		m_fMinZoomFactorX = m_zoomFactorX - (m_fZoomStepX * (float)m_nMinZoomNum);
		m_fMinZoomFactorY = m_zoomFactorY - (m_fZoomStepY * (float)m_nMinZoomNum);

		m_fMaxZoomFactorX = m_zoomFactorX + (m_fZoomStepX * (float)(m_nZoomStepCnt - 10));
		m_fMaxZoomFactorY = m_zoomFactorY + (m_fZoomStepY * (float)(m_nZoomStepCnt - 10));

		// init 값
		m_zoomFactorX -= m_fZoomStepX * 2;
		m_zoomFactorY -= m_fZoomStepY * 2;
	}
}

void CroppingView::render(QPainter *p)
{
	p->fillRect(0, 0, width(), height(), QColor(0, 0, 0));

	drawTexture(p);

	drawBoundLine(p);
}

void CroppingView::drawTexture(QPainter *p)
{
	//rotate 
	QMatrix rm;
	rm.rotate(m_rotAngle);
	int pxw = m_image.width();
	int pxh = m_image.height();
	QImage tmpImg = m_image.transformed(rm);
	QImage drawImg = tmpImg.copy((tmpImg.width() - pxw) / 2, (tmpImg.height() - pxh) / 2, pxw, pxh);

	QRect rect = getPostion();
	drawImg = drawImg.scaled(QSize(rect.width(), rect.height()));

	// 음영
	for (int y = 0; y < drawImg.height(); ++y)
	{
		for (int x = 0; x < drawImg.width(); ++x)
		{
			int index = y * m_imageWidth + x;
			QColor color = drawImg.pixelColor(x, y);
			uchar tmp = color.red();
			if (m_eWinType == WT_AXIAL)
			{
				if (x < round(m_boundingBox.minX*m_zoomFactorX) || x > round(m_boundingBox.maxX*m_zoomFactorX) || y <  round(m_boundingBox.minY* m_zoomFactorY) || y >  round(m_boundingBox.maxY*m_zoomFactorY))
				{
					tmp /= 5;
					color.setRgb(tmp, tmp, tmp);
				}
			}
			else if (m_eWinType == WT_CORONAL)
			{
				if (x < round(m_boundingBox.minX*m_zoomFactorX) || x >round(m_boundingBox.maxX*m_zoomFactorX) || y < round(m_boundingBox.minZ*m_zoomFactorY) || y > round(m_boundingBox.maxZ*m_zoomFactorY))
				{
					tmp /= 5;
					color.setRgb(tmp, tmp, tmp);
				}
			}
			else if (m_eWinType == WT_SAGITTAL)
			{
				if (x < round(m_boundingBox.minY*m_zoomFactorX) || x > round(m_boundingBox.maxY*m_zoomFactorX) || y < round(m_boundingBox.minZ*m_zoomFactorY) || y > round(m_boundingBox.maxZ*m_zoomFactorY))
				{
					tmp /= 5;
					color.setRgb(tmp, tmp, tmp);
				}
			}

			drawImg.setPixelColor(x, y, color);

		}
	}

	p->setRenderHint(QPainter::Antialiasing);

	p->drawImage(rect, drawImg);
}

void CroppingView::drawBoundLine(QPainter *p)
{
	QRect rect = getPostion();

	int nX, nY;
	bool selX, selY;
	selX = selY = false;

	int minX, maxX, minY, maxY;

	if (m_eWinType == WT_AXIAL)
	{
		minX = m_boundingBox.minX;
		maxX = m_boundingBox.maxX;
		minY = m_boundingBox.minY;
		maxY = m_boundingBox.maxY;
	}
	else if (m_eWinType == WT_CORONAL)
	{
		minX = m_boundingBox.minX;
		maxX = m_boundingBox.maxX;
		minY = m_boundingBox.minZ;
		maxY = m_boundingBox.maxZ;
	}
	else if (m_eWinType == WT_SAGITTAL)
	{
		minX = m_boundingBox.minY;
		maxX = m_boundingBox.maxY;
		minY = m_boundingBox.minZ;
		maxY = m_boundingBox.maxZ;
	}

	if ((MAM_BOUNDING_MINX_OVER & m_mouse_action_mode) || (MAM_BOUNDING_MINX_MOVE & m_mouse_action_mode))
	{
		p->setPen(QPen(Qt::blue, PEN_SIZE));
		selX = true;
		nX = rect.x() + minX * m_zoomFactorX;
	}
	else
		p->setPen(QPen(Qt::white, 1));

	p->drawLine(rect.x() + minX * m_zoomFactorX, 0, rect.x() + minX * m_zoomFactorX, this->size().height());

	if ((MAM_BOUNDING_MAXX_OVER & m_mouse_action_mode) || (MAM_BOUNDING_MAXX_MOVE & m_mouse_action_mode))
	{
		p->setPen(QPen(Qt::blue, PEN_SIZE));
		selX = true;
		nX = rect.x() + maxX * m_zoomFactorX;
	}
	else
		p->setPen(QPen(Qt::white, 1));

	p->drawLine(rect.x() + round((maxX)* m_zoomFactorX), 0, rect.x() + round((maxX)* m_zoomFactorX), this->size().height());


	if ((MAM_BOUNDING_MINY_OVER & m_mouse_action_mode) || (MAM_BOUNDING_MINY_MOVE & m_mouse_action_mode))
	{
		p->setPen(QPen(Qt::blue, PEN_SIZE));
		selY = true;
		nY = rect.y() + minY * m_zoomFactorY;
	}
	else
		p->setPen(QPen(Qt::white, 1));

	p->drawLine(0, rect.y() + minY * m_zoomFactorY, this->size().width(), rect.y() + round(minY * m_zoomFactorY));


	if ((MAM_BOUNDING_MAXY_OVER & m_mouse_action_mode) || (MAM_BOUNDING_MAXY_MOVE & m_mouse_action_mode))
	{
		p->setPen(QPen(Qt::blue, PEN_SIZE));
		selY = true;
		nY = rect.y() + maxY * m_zoomFactorY;
	}
	else
		p->setPen(QPen(Qt::white, 1));

	p->drawLine(0, rect.y() + round((maxY)* m_zoomFactorY), this->size().width(), rect.y() + round((maxY)* m_zoomFactorY));

	if (selX && selY)
	{
		p->setPen(Qt::red);
		p->setBrush(Qt::red);

		p->drawEllipse(QPoint(nX, nY), PEN_SIZE, PEN_SIZE);
	}
}

QRect CroppingView::getPostion()
{
	// 	QPoint scaled(qRound(float(m_cx) * (m_zoomFactorX*cos(m_rot*M_PI / 180) + m_zoomFactorY*sin(m_rot*M_PI / 180)))
	// 		, qRound(float(m_cy) * (m_zoomFactorX*sin(m_rot*M_PI / 180) + m_zoomFactorY*cos(m_rot*M_PI/180))));
	QPoint scaled(qRound(float(m_imageWidth) * m_zoomFactorX), qRound(float(m_imageHeight) *m_zoomFactorY));
	//QPoint scaled(qRound(float(m_cx) * 1.0), qRound(float(m_cy) *1.0));
	QPoint imageZero = (QPoint(this->size().width(), this->size().height()) / 2) - (scaled / 2);

	return QRect(imageZero.x(), imageZero.y(), scaled.x(), scaled.y());
}

void CroppingView::renderLater()
{
	update();
}

void CroppingView::reset()
{
	BoundingBoxI originBox;
	originBox.reset(m_cx, m_cy, m_cz);
	m_boundingBox = originBox;
	m_rotAngle = 0.f;
}

void CroppingView::setBoundingBox(BoundingBoxI box) 
{
	m_boundingBox = box; 
};

BoundingBoxI CroppingView::getBouningBox() 
{
	return m_boundingBox; 
};

MOUSE_ACTION_MODE CroppingView::getMouseAction() 
{
	return m_mouse_action_mode; 
};

void CroppingView::setRotationAngle(float fAngle)
{
	if (m_rotAngle != fAngle)
	{
		m_rotAngle = fAngle;
		emit sig_angleUpdate(m_eWinType);
	}
}

float CroppingView::getRotationAngle()
{
	return m_rotAngle;
}

int CroppingView::getMaxDepth()
{
	return m_imageDepth - 1;
}

void CroppingView::setDepth(int depth)
{
	if (depth >= 0 && depth <= getMaxDepth())
	{
		m_depth = depth;
		updateTexture();
		updateImageByTexture();
		update();
	}
}

int CroppingView::getDepth()
{
	return m_depth;
}
