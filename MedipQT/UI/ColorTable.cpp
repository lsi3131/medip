#include "stdafx.h"
#include "ColorTable.h"
#include "define.h"

ColorTable::ColorTable(eColorGradientPostion eImgPos, QWidget *parent)
	: QWidget(parent)
{
	m_eImagePos = eImgPos;
}

ColorTable::~ColorTable()
{
	m_vecCategoryValue.clear();
	m_vecLabelManager.clear();
}

void ColorTable::setStops(bool bInputSelect, QGradientStops stops)
{
	if (bInputSelect == true)
	{
		{ m_stops = stops; }
	}
	else
	{
		m_stops = getSettingValue();
	}
}

void ColorTable::setGradientImage(void)
{
	if (!m_stops.empty())
	{
		drawGradient(0, 0, 0, m_nHeight, m_displayImg);
	}
}

void ColorTable::setGradientCategoryValue(std::vector<std::pair<float, float>> vecVal)
{
	m_vecCategoryValue.clear();
	m_vecCategoryValue = std::move(vecVal);
	
	for (int i=0; i<m_vecCategoryValue.size(); ++i)
	{
		int size = m_vecLabelManager.size() - 1;
		if (size < i)
		{
			QLabel *label = new QLabel(this);
			label->setFixedWidth(m_nCategoryTextWidth);
			label->setFixedHeight(15);

			if (m_eImagePos == eColorGradientPostion::eRightPos)
			{
				label->setText(QString::number(m_vecCategoryValue[i].second, 'f', 2) + " -");
				label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
				int labelHeight = label->height();
				label->move(0, m_vecCategoryValue[i].first*(m_nHeight - (m_nHeightPadding * 2)) + m_nHeightPadding - (labelHeight / 2));
			}
			else if (m_eImagePos == eColorGradientPostion::eLeftPos)
			{
				label->setText("- " +QString::number(m_vecCategoryValue[i].second, 'f', 2));
				label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
				int labelHeight = label->height();
				label->move(m_nWidth- m_nCategoryTextWidth+1, m_vecCategoryValue[i].first*(m_nHeight - (m_nHeightPadding * 2)) + m_nHeightPadding - (labelHeight / 2));
			}			

			label->setStyleSheet("QLabel{ background-color: rgba(0, 0, 0, 255);}");		

			m_vecLabelManager.push_back(label);
		}
		else
		{
			if (m_eImagePos == eColorGradientPostion::eRightPos)
			{
				m_vecLabelManager[i]->setText(QString::number(m_vecCategoryValue[i].second, 'f', 2) + " -");
				int labelHeight = m_vecLabelManager[i]->height();
				m_vecLabelManager[i]->move(0, m_vecCategoryValue[i].first*(m_nHeight - (m_nHeightPadding * 2)) + m_nHeightPadding - (labelHeight / 2));
			}
			else if (m_eImagePos == eColorGradientPostion::eLeftPos)
			{
				m_vecLabelManager[i]->setText("- " + QString::number(m_vecCategoryValue[i].second, 'f', 2));
				int labelHeight = m_vecLabelManager[i]->height();
				m_vecLabelManager[i]->move(m_nWidth - m_nCategoryTextWidth + 1, m_vecCategoryValue[i].first*(m_nHeight - (m_nHeightPadding * 2)) + m_nHeightPadding - (labelHeight / 2));
			}
		}
	}
}

bool ColorTable::isEmptyCategoryMap(void)
{
	if (m_vecCategoryValue.empty())
		return true;
	else
		return false;
}

void ColorTable::clearCategoryMap(void)
{	
	for (auto it = m_vecLabelManager.begin(); it != m_vecLabelManager.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	m_vecLabelManager.clear();
	m_vecCategoryValue.clear();
}

QSize ColorTable::getWidgetSize()
{
	return QSize(m_nWidth, m_nHeight);
}

void ColorTable::setWidgetSize(QSize size)
{
	m_nWidth = size.width();
	m_nHeight = size.height();
}

QImage ColorTable::getHeightMapColorTable(void)
{
	// 	int m = 1024;
	// 	int n = 1024;
	// 
	// 	QImage image(m, n, QImage::Format_RGB32);
	// 	int rVal = -1, gVal = -1, bVal = 256;
	// 	for (int i = 0; i < n; i++)
	// 	{
	// 		int red = 0, grn = 0, blu = 0;
	// 		switch (i / 256)
	// 		{
	// 		case 3: red = 255;		grn = --gVal;	blu = 0;		break;	// yellow -> red
	// 		case 2: red = ++rVal;	grn = 255;		blu = 0;		break;	// green -> yellow
	// 		case 1: red = 0;		grn = 255;		blu = --bVal;	break;	// cyan -> green
	// 		case 0: red = 0;		grn = ++gVal;	blu = 255;		break;	// blue -> cyan
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
	// 	return image;
	//////////////////////////////////////////////////////////
	vector<QColor> colortable;
	QPixmap pixmap(512, 512);
	QLinearGradient gradient;
	QBrush brush;
	int index;
	int imgWidth, imgHeight;
	QImage imgmap;
	QColor pixelColor;

	gradient.setStart(0, 512);
	gradient.setFinalStop(0, 0);
	gradient.setInterpolationMode(gradient.ComponentInterpolation);
	gradient.setStops(getSettingValue());

	brush = gradient;

	pixmap.fill(Qt::transparent);

	QPainter pixPainter(&pixmap);
	pixPainter.setPen(Qt::NoPen);
	pixPainter.fillRect(pixmap.rect(), brush);
	pixPainter.setRenderHint(QPainter::Antialiasing);
	pixPainter.end();

	imgmap = pixmap.toImage();
	imgWidth = imgmap.width();
	imgHeight = imgmap.height();

	return imgmap;
}

void ColorTable::setGradientPosition(eColorGradientPostion ePos)
{
	m_eImagePos = ePos;
}

void ColorTable::setWidth(int nWindgetWidth, int nTextWidth)
{
	m_nWidth = nWindgetWidth;
	m_nCategoryTextWidth = nTextWidth;
}

QGradientStops ColorTable::getSettingValue(void)
{
	QGradientStops gradientStops;

	gradientStops.push_back(QGradientStop(0.00, QColor(255, 0, 0)));
	gradientStops.push_back(QGradientStop(0.20, QColor(255, 255, 0)));
	gradientStops.push_back(QGradientStop(0.40, QColor(0, 255, 0)));
	gradientStops.push_back(QGradientStop(0.80, QColor(0, 255, 255)));
	gradientStops.push_back(QGradientStop(1.00, QColor(0, 0, 255)));

	return gradientStops;
}

void ColorTable::paintEvent(QPaintEvent *e)
{
	if (!m_displayImg.isNull())
	{
		QPainter painter;
		painter.begin(this);
		painter.setPen(Qt::NoPen);
	
		QRect rect;
		if (m_eImagePos == eColorGradientPostion::eRightPos)
		{
			rect = QRect(QPoint(m_nCategoryTextWidth, m_nHeightPadding), QPoint(m_nWidth, m_nHeight - m_nHeightPadding));
		}
		else if (m_eImagePos == eColorGradientPostion::eLeftPos)
		{
			rect = QRect(QPoint(0, m_nHeightPadding), QPoint(m_nWidth - m_nCategoryTextWidth, m_nHeight - m_nHeightPadding));
		}

		painter.drawImage(rect, m_displayImg);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.end();
	}
}

void ColorTable::drawGradient(int nStartX, int nStartY, int nEndX, int nEndY, QImage &pOutImg)
{
	QBrush brush;
	QLinearGradient gradient;

	gradient.setStart(nStartX, nStartY);
	gradient.setFinalStop(nEndX, nEndY);
	gradient.setInterpolationMode(gradient.ComponentInterpolation);
	gradient.setStops(m_stops);

	brush = gradient;
	
	QImage imgmap(m_nWidth - m_nCategoryTextWidth, m_nHeight - (m_nHeightPadding * 2), QImage::Format_RGB32);
	
	imgmap.fill(Qt::transparent);

	QPainter imgPainter(&imgmap);
	imgPainter.setPen(Qt::NoPen);
	imgPainter.fillRect(imgmap.rect(), brush);
	imgPainter.setRenderHint(QPainter::Antialiasing);
	imgPainter.end();

	pOutImg = imgmap;
}
