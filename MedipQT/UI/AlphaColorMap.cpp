#include "stdafx.h"
#include "AlphaColorMap.h"
#include "CustomHistogram.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "System/resourceManager.h"

static QGradientStops defaultVolume;
static QGradientStops defaultScreen;
static QGradientStops presetStops[SP_COUNT - 1];
static QGradientStops VolumePresetStops[SP_COUNT - 1];
static QVector<QGradientStops> custom2DStops;
static QVector<QGradientStops> custom3DStops;

AlphaColorMap::AlphaColorMap(QWidget* parent /* = Q_NULLPTR */, int type /* = 1*/, bool colorChange /* = false */) : 
	QWidget(parent)
{
	static bool initial = false;

	this->m_colorClick = colorChange;
	this->m_colorChange = false;
	this->m_type = type;

	if (CL_2D == type)
		m_preset = WIN_MANAGER->getSelectedPreset();
	else
		m_preset = WIN_MANAGER->getSelectedVolumePreset();

	m_custom = WIN_MANAGER->getSelectedCustomPreset(type);

	if (!initial)
	{
		initial = true;
		custom2DStops.reserve(PRESET_MAX);
		custom3DStops.reserve(PRESET_MAX);
	}

	setMouseTracking(true);
}

AlphaColorMap::~AlphaColorMap()
{
}

void AlphaColorMap::setPreset(SLICE_PRESET preset, bool reset, int cusIndex)
{
	if (-1 >= cusIndex)
	{
		m_preset = preset;
		m_custom = -1;
	}
	else
		m_custom = cusIndex;

	QGradientStops stops = getPresetStops(preset, m_type, reset, cusIndex);

	if (m_stops != stops)
	{
		/*if (reset)
			this->m_colorChange = false;
*/
		m_stops = stops;
		setColorPoints();
		emit sig_AddPoints();
		this->repaint();

		this->m_colorChange = true;
	}
}

void AlphaColorMap::addPreset(int index, bool isImport)
{
	QGradientStops stops;

	if (!isImport) //custom
	{
		stops = getPresetStops(SP_CUSTOM, CL_2D);
		custom2DStops.append(stops);

		QString points;

		for (int i = 0; i < stops.size(); i++)
		{
			QGradientStop stop = stops.at(i);
			if (0 != i)
				points.append(QString(";%1,%2,%3,%4").arg(stop.first)
					.arg(stop.second.red()).arg(stop.second.green()).arg(stop.second.blue()));
			else
				points.append(QString("%1,%2,%3,%4").arg(stop.first)
					.arg(stop.second.red()).arg(stop.second.green()).arg(stop.second.blue()));
		}

		WIN_MANAGER->setConfigPreset(index, CL_2D, points);

		stops.clear();

		stops = getPresetStops(SP_CUSTOM, CL_3D);
		custom3DStops.append(stops);
		points = "";

		for (int i = 0; i < stops.size(); i++)
		{
			QGradientStop stop = stops.at(i);
			if (0 != i)
				points.append(QString(";%1,%2,%3,%4").arg(stop.first)
					.arg(stop.second.red()).arg(stop.second.green()).arg(stop.second.blue()));
			else
				points.append(QString("%1,%2,%3,%4").arg(stop.first)
					.arg(stop.second.red()).arg(stop.second.green()).arg(stop.second.blue()));
		}

		WIN_MANAGER->setConfigPreset(index, CL_3D, points);

		stops.clear();
	}
	else
	{
		QString points;
		QStringList point;
		bool res = WIN_MANAGER->getConfigPreset(index, CL_2D, points);

		QGradientStops stops;
		if (res)
		{
			point = points.split(";");

			for (int i = 0; i < point.size(); i++)
			{
				QStringList color = point.at(i).split(",");
				stops << QGradientStop(color.at(0).toFloat(), QColor(color.at(1).toInt(), color.at(2).toInt(), color.at(3).toInt()));
			}
		}
		else
		{
			stops = setGrayScale();
		}
		if (custom2DStops.size() <= index)
			custom2DStops.append(stops);
		else
			custom2DStops.replace(index, stops);

		stops.clear();
		points = "";

		res = WIN_MANAGER->getConfigPreset(index, CL_3D, points);

		if (res)
		{
			point = points.split(";");

			for (int i = 0; i < point.size(); i++)
			{
				QStringList color = point.at(i).split(",");
				stops << QGradientStop(color.at(0).toFloat(), QColor(color.at(1).toInt(), color.at(2).toInt(), color.at(3).toInt()));
			}
		}
		else
		{
			stops = setGrayScale();
		}
		if (custom3DStops.size() <= index)
			custom3DStops.append(stops);
		else
			custom3DStops.replace(index, stops);

		stops.clear();
	}
}

bool AlphaColorMap::delPreset(int index)
{
	if (index < 0 ||
		index >= custom2DStops.size())
		return false;

	custom2DStops.remove(index);

	if (index >= custom3DStops.size())
		return false;

	custom3DStops.remove(index);

	return true;
}

void AlphaColorMap::setPresetStops(QGradientStops stops, int type)
{
	m_stops = stops;
	setColorPoints();

	if (-1 >= m_custom)
	{
		switch (m_preset)
		{
		case SP_RECOMMAND_1:
		case SP_RECOMMAND_2:
		case SP_BONE:
		case SP_LUNG:
		case SP_MEDIASTINAL:
		case SP_BRAIN:
		case SP_ABD:
		case SP_LIVER:
		case SP_KIDNEY:
		case SP_URINARY_TRACK_BLADDER:
		case SP_CUSTOM:
			if (CL_2D == type)
				presetStops[m_preset - 1] = stops;
			else
				VolumePresetStops[m_preset - 1] = stops;
			break;
		case SP_DEFAULT:
		default:
			if (CL_2D == type)
				defaultScreen = stops;
			else
				defaultVolume = stops;
			break;
		}
	}
	else
	{
		if (CL_2D == type)
			custom2DStops.replace(m_custom, stops);
		else
			custom3DStops.replace(m_custom, stops);
	}

	emit sig_AddPoints();
	this->repaint();
}

void AlphaColorMap::applyCustomStops()
{
	QString points;

	for (int i = 0; i < m_stops.size(); i++)
	{
		QGradientStop stop = m_stops.at(i);
		if (0 != i)
			points.append(QString(";%1,%2,%3,%4").arg(stop.first)
				.arg(stop.second.red()).arg(stop.second.green()).arg(stop.second.blue()));
		else
			points.append(QString("%1,%2,%3,%4").arg(stop.first)
				.arg(stop.second.red()).arg(stop.second.green()).arg(stop.second.blue()));
	}

	if (-1 >= m_custom)
	{
		switch (m_preset)
		{
		case SP_RECOMMAND_1:
		case SP_RECOMMAND_2:
		case SP_BONE:
		case SP_LUNG:
		case SP_MEDIASTINAL:
		case SP_BRAIN:
		case SP_ABD:
		case SP_LIVER:
		case SP_KIDNEY:
		case SP_URINARY_TRACK_BLADDER:
		case SP_CUSTOM:
			if (CL_2D == m_type)
			{
				presetStops[m_preset - 1] = m_stops;
				WIN_MANAGER->setConfigValue(ELEMENT_PRESET, QString("SCREEN_CUSTOM%1").arg(m_preset), points);
			}
			else
			{
				VolumePresetStops[m_preset - 1] = m_stops;
				WIN_MANAGER->setConfigValue(ELEMENT_PRESET, QString("VOLUME_CUSTOM%1").arg(m_preset), points);
			}
			break;
		case SP_DEFAULT:
		default:
			if (CL_2D == m_type)
			{
				defaultScreen = m_stops;
				WIN_MANAGER->setConfigValue(ELEMENT_PRESET, "SCREEN_CUSTOM", points);
			}
			else
			{
				defaultVolume = m_stops;
				WIN_MANAGER->setConfigValue(ELEMENT_PRESET, "VOLUME_CUSTOM", points);
			}
			break;
		}
	}
	else
	{
		if (CL_2D == m_type)
			custom2DStops.replace(m_custom, m_stops);
		else
			custom3DStops.replace(m_custom, m_stops);

		WIN_MANAGER->setConfigPreset(m_custom, m_type, points);
	}
}

QGradientStops AlphaColorMap::getPresetStops(SLICE_PRESET preset, int type, bool reset, int cusIndex)
{
	static bool initial = false;

	if (!initial)
	{
		initial = true;

		QString points;
		QStringList point;
		bool res = WIN_MANAGER->getConfigValue(ELEMENT_PRESET, "SCREEN_CUSTOM", points);

		if (res)
		{
			point = points.split(";");

			for (int i = 0; i < points.split(";").length(); i++)
			{
				QStringList color = point.at(i).split(",");
				defaultScreen << QGradientStop(color.at(0).toFloat(), QColor(color.at(1).toInt(), color.at(2).toInt(), color.at(3).toInt()));
			}
		}
		else
			defaultScreen = setGrayScale();

		res = WIN_MANAGER->getConfigValue(ELEMENT_PRESET, "VOLUME_CUSTOM", points);

		if (res)
		{
			point = points.split(";");

			for (int i = 0; i < points.split(";").length(); i++)
			{
				QStringList color = point.at(i).split(",");
				defaultVolume << QGradientStop(color.at(0).toFloat(), QColor(color.at(1).toInt(), color.at(2).toInt(), color.at(3).toInt()));
			}
		}
		else
			defaultVolume = setGrayScale();

		QLinearGradient presetGradient[SP_COUNT - 1];
		QLinearGradient VolumePresetGradient[SP_COUNT - 1];
		QImage* presetImg;
		QColor pixelColor;
		float pos;
		int value;
		int cusCnt = WIN_MANAGER->getPresetCount();


		for (int index = 0; index < SP_COUNT - 1; index++)
		{
			res = WIN_MANAGER->getConfigValue(ELEMENT_PRESET, QString("SCREEN_CUSTOM%1").arg(index + 1), points);

			if (res)
			{
				point = points.split(";");

				for (int i = 0; i < points.split(";").length(); i++)
				{
					QStringList color = point.at(i).split(",");
					presetStops[index] << QGradientStop(color.at(0).toFloat(), QColor(color.at(1).toInt(), color.at(2).toInt(), color.at(3).toInt()));
				}
			}
			else
				presetStops[index] = setGrayScale();
		}

		for (int index = 0; index < SP_COUNT - 1; index++)
		{
			res = WIN_MANAGER->getConfigValue(ELEMENT_PRESET, QString("VOLUME_CUSTOM%1").arg(index + 1), points);

			if (res)
			{
				point = points.split(";");

				for (int i = 0; i < points.split(";").length(); i++)
				{
					QStringList color = point.at(i).split(",");
					VolumePresetStops[index] << QGradientStop(color.at(0).toFloat(), QColor(color.at(1).toInt(), color.at(2).toInt(), color.at(3).toInt()));
				}
			}
			else
				VolumePresetStops[index] = setGrayScale();
		}

		for (int index = 0; index < cusCnt; index++)
		{
			res = WIN_MANAGER->getConfigPreset(index, CL_2D, points);

			QGradientStops stops;
			if (res)
			{
				point = points.split(";");

				for (int i = 0; i < points.split(";").length(); i++)
				{
					QStringList color = point.at(i).split(",");
					stops << QGradientStop(color.at(0).toFloat(), QColor(color.at(1).toInt(), color.at(2).toInt(), color.at(3).toInt()));
				}
			}
			else
			{
				stops = setGrayScale();
			}
			if (custom2DStops.size() <= index)
				custom2DStops.append(stops);
			else
				custom2DStops.replace(index, stops);
		}

		points = "";

		for (int index = 0; index < cusCnt; index++)
		{
			res = WIN_MANAGER->getConfigPreset(index, CL_3D, points);

			QGradientStops stops;
			if (res)
			{
				point = points.split(";");

				for (int i = 0; i < points.split(";").length(); i++)
				{
					QStringList color = point.at(i).split(",");
					stops << QGradientStop(color.at(0).toFloat(), QColor(color.at(1).toInt(), color.at(2).toInt(), color.at(3).toInt()));
				}
			}
			else
			{
				stops = setGrayScale();
			}
			if (custom3DStops.size() <= index)
				custom3DStops.append(stops);
			else
				custom3DStops.replace(index, stops);
		}

	}

	if (-1 >= cusIndex)
	{
		switch (preset)
		{
		case SP_RECOMMAND_1:
		case SP_RECOMMAND_2:
		case SP_BONE:
		case SP_LUNG:
		case SP_MEDIASTINAL:
		case SP_BRAIN:
		case SP_ABD:
		case SP_LIVER:
		case SP_KIDNEY:
		case SP_URINARY_TRACK_BLADDER:
		case SP_CUSTOM:
			if (CL_2D == type)
				return presetStops[preset - 1];
			else
				return VolumePresetStops[preset - 1];
			break;
		case SP_DEFAULT:
		default:
			if (reset)
				return setGrayScale();

			if (CL_2D == type)
				return defaultScreen;

			return defaultVolume;
			break;
		}
	}
	else
	{
		if (CL_2D == type)
			return custom2DStops.at(cusIndex);
		else
			return custom3DStops.at(cusIndex);
	}
}

QGradientStops AlphaColorMap::setGrayScale()
{
	QGradientStops defaultStops;

	defaultStops << QGradientStop(0.00, QColor(0, 0, 0));
	defaultStops << QGradientStop(1.00, QColor(255, 255, 255));

	return defaultStops;
}

QColor AlphaColorMap::getColorPoint(int pos)
{
	QGradientStop curStop = m_stops.at(pos);

	return curStop.second;
}

void AlphaColorMap::removeColorPoint(int pos)
{
	m_stops.remove(pos);
	m_points.remove(pos);
	this->m_colorChange = true;
	emit sig_AddPoints();
	this->repaint();
}

void AlphaColorMap::moveColorPoint(int oldPos, qreal newPos)
{
	const qreal interval = 0.01f;
	if ((newPos <= 0.0f) || (newPos >= 1.0f))
		return;

	if ((0 == oldPos) || ((m_points.size() - 1) == oldPos))
		return;

	QGradientStop oldStop = m_stops.at(oldPos);
	QGradientStop newStop = QGradientStop(newPos, oldStop.second);

	int pos = -1;

	if (((m_points.at(oldPos - 1) + interval) < newPos) && ((m_points.at(oldPos + 1) - interval) > newPos))
		pos = oldPos;

	if (pos != oldPos)
		return;
	m_stops.replace(oldPos, newStop);
	m_points.replace(oldPos, newPos);
	this->m_colorChange = true;
	emit sig_MovePoints(oldPos, newPos);
	this->repaint();
}

void AlphaColorMap::changeColorPoint(int Pos, QColor changeColor)
{
	QGradientStop oldStop = m_stops.at(Pos);
	QGradientStop newStop = QGradientStop(oldStop.first, changeColor);

	m_stops.replace(Pos, newStop);
	this->m_colorChange = true;
	emit sig_MovePoints(-1, -1);
	this->repaint();
}

void AlphaColorMap::setColorPoints()
{
	m_points.clear();
	for (int index = 0; index < m_stops.size(); index++)
		m_points.append(m_stops.at(index).first);
}

bool AlphaColorMap::isColorChange()
{
	return m_colorChange;
}

QVector<QColor> AlphaColorMap::getColorTable(SLICE_PRESET preset, int type, int cusIndex, bool rst)
{
	QVector<QColor> colortable;
	QPixmap pixmap(512, 512);
	QLinearGradient gradient;
	QBrush brush;
	int index;
	int imgWidth, imgHeight;
	QImage imgmap;
	QColor pixelColor;

	gradient.setStart(0, 0);
	gradient.setFinalStop(512, 0);
	gradient.setInterpolationMode(gradient.ComponentInterpolation);
	gradient.setStops(getPresetStops(preset, type, rst, cusIndex));

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

	for (int i = 0; i < (512 / 2); i++)
	{
		index = ((float)i / ((512 / 2) - 1)) * (imgWidth - 1);
		pixelColor = imgmap.pixelColor(index, imgHeight / 2);
		colortable.push_back(pixelColor);
	}

	return colortable;
}

QColor AlphaColorMap::getEdgeColor(bool isLeft)
{
	QPixmap pixmap(512, 512);
	QLinearGradient gradient;
	QBrush brush;
	int index;
	int imgWidth, imgHeight;
	QImage imgmap;
	QColor pixelColor;

	gradient.setStart(0, 0);
	gradient.setFinalStop(512, 0);
	gradient.setInterpolationMode(gradient.ComponentInterpolation);
	gradient.setStops(m_stops);

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

	if (isLeft)
		index = 0;
	else
		index = imgWidth - 1;

	pixelColor = imgmap.pixelColor(index, imgHeight / 2);

	return pixelColor;
}

void AlphaColorMap::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		if (!m_colorClick) return;

		//		if (m_preset != SP_DEFAULT) return;

		float index = (float)e->pos().x() / (this->width() - 1);

		QColorDialog dlg(this);
		if (dlg.exec() == QDialog::Accepted)
		{
			if (m_points.contains(index))
			{
				m_stops.remove(m_points.indexOf(index));
				m_points.remove(m_points.indexOf(index));
			}
			int pos = m_points.size();
			for (int i = 0; i < m_points.size(); i++)
			{
				if (m_points.at(i) > index)
				{
					pos = i;
					break;
				}
			}
			m_stops.insert(pos, QGradientStop(index, dlg.selectedColor()));
			this->m_colorChange = true;
			setColorPoints();
			emit sig_AddPoints();
			this->repaint();
		}
	}
}

void AlphaColorMap::paintEvent(QPaintEvent* e)
{
	QBrush brush;
	QLinearGradient gradient;

	gradient.setStart(0, 0);
	gradient.setFinalStop(this->width(), 0);
	gradient.setInterpolationMode(gradient.ComponentInterpolation);
	gradient.setStops(m_stops);

	brush = gradient;

	QImage imgmap(this->rect().width(), this->rect().height(), QImage::Format_RGB32);
	imgmap.fill(Qt::transparent);

	QPainter imgPainter(&imgmap);
	imgPainter.setPen(Qt::NoPen);
	imgPainter.fillRect(imgmap.rect(), brush);
	imgPainter.setRenderHint(QPainter::Antialiasing);
	imgPainter.end();

	QPainter painter;
	painter.begin(this);
	painter.setPen(Qt::NoPen);
	painter.drawImage(this->rect(), imgmap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.end();
}
