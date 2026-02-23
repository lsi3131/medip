#include "stdafx.h"
#include "LayerHistogram.h"
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>
#include "Windows/windowManager.h"
#include "graphics/BoundingBox.h"
#include "System/stringManager.h"
#include "System/FileManager.h"
#include "DataContext.h"

LayerHistogram::LayerHistogram(int layerUID, QWidget *parent /* = NULL */)
	:QwtPlot(parent)
{
	m_uid = layerUID;
	m_maskIndex = layerUID >= MASK_SECOND_MAX ? (m_uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	
	m_actCopy = new QAction(this);
	m_actCopy->setText("Copy");
	connect(m_actCopy, &QAction::triggered, this, &LayerHistogram::OnCopy);

	m_actSave = new QAction(this);
	m_actSave->setText("Save to *.csv format");
	connect(m_actSave, &QAction::triggered, this, &LayerHistogram::OnSave);
	
	//setStyleSheet("background-color: rgba(255, 255, 255, 255); color: rgba(0, 0, 0, 255);");
	setStyleSheet("background-color: rgba(50, 50, 50, 255); color: rgba(200, 200, 200, 255);");
	// 주의 : 배경색으로 alpha 값 투명하게 하면 배경에 잔상 같은 무늬가 생김 
	setMouseTracking(true);
		
}

LayerHistogram::~LayerHistogram()
{
}

void LayerHistogram::initPlot()
{
	axisWidget(QwtPlot::yLeft)->setContentsMargins(0, 0, 0, 0);
	axisWidget(QwtPlot::xBottom)->setContentsMargins(0, 0, 0, 0);

//	axisWidget(QwtPlot::yLeft)->hide();
//	axisWidget(QwtPlot::xBottom)->hide();

//	enableAxis(QwtPlot::yLeft, false);
//	enableAxis(QwtPlot::xBottom, false);
	
	plotLayout()->setAlignCanvasToScales(true);
	plotLayout()->setSpacing(0);
//	setFixedHeight(120);
}

void LayerHistogram::CreateHistogram(int fileSeq)
{
	this->fileSeq = fileSeq;
	BoundingBoxI box = DATA_CONTEXT->volume_data.getBoundingBox(m_uid);//&(WIN_MANAGER->volume_data.boundingBoxROI[m_uid]);
	MaskInfo * info = DATA_CONTEXT->volume_data.getMaskInfo(m_uid, true);

	float *AllCount = NULL;
	float *AllCount2 = NULL;

	uchar m = m_maskIndex != 0 ? info->mask_id2 : info->mask_id;
	int count = 0;
	float maxCount = 0;
	float maxLogCount = 0;
	int cy, cx;
	mint16 _min, _max;

	if (!box.isFitROI())
		DATA_CONTEXT->volume_data.updateUIDBoundingBox(m_uid, true);

	cy = DATA_CONTEXT->volume_data.getCY();
	cx = DATA_CONTEXT->volume_data.getCX();

	_min = HUMax = DATA_CONTEXT->volume_data.getHuMin();
	_max =HUMin = DATA_CONTEXT->volume_data.getHuMax();
	int Allsize = (_max + 1) - _min;

	if (Allsize > 30000)
	{
		AllCount = new float[(Allsize / 2)];
		AllCount2 = new float[(Allsize / 2)];
		memset(AllCount, 0, sizeof(float)*(Allsize/2));
		memset(AllCount2, 0, sizeof(float)*(Allsize / 2));
	}
	else
	{
		AllCount = new float[Allsize];

		memset(AllCount, 0, sizeof(float)*Allsize);
	}
	

	for (int z = box.getMinZ(); z <= box.getMaxZ(); z++)
	{
		for (int y = box.getMinY(); y <= box.getMaxY(); y++)
		{
			for (int x = box.getMinX(); x <= box.getMaxX(); x++)
			{
				if (count == DATA_CONTEXT->volume_data.getVoxelCount(m_uid)) break;

				if (DATA_CONTEXT->volume_data.getMaskData(z*cx*cy + y*cx + x, m_maskIndex) & m)
				{
					count++;

					if (HUMax < DATA_CONTEXT->volume_data.getData(z*cx*cy + y*cx + x))
						HUMax = DATA_CONTEXT->volume_data.getData(z*cx*cy + y*cx + x);
					if (HUMin > DATA_CONTEXT->volume_data.getData(z*cx*cy + y*cx + x))
						HUMin = DATA_CONTEXT->volume_data.getData(z*cx*cy + y*cx + x);

					int index = DATA_CONTEXT->volume_data.getData(z*cx*cy + y*cx + x);

					index -= _min;

					if (0 <= index && index < Allsize)
					{
						if (Allsize > 30000)
						{
							if(index < (Allsize/2))
								AllCount[index]++;
							else
								AllCount2[index-(Allsize/2)]++;
						}
						else
							AllCount[index]++;
					}

				}
			}
		}
	}
	QString _tmpFile = STRING_MANAGER->cacheFilePath + QString("/%1%2Hu.tmp").arg(m_uid).arg(fileSeq);

	QFile file(_tmpFile);

	bool res = file.open(QIODevice::WriteOnly);
	int maxrange = abs(HUMax - HUMin) > 30000 ? 30000 : abs(HUMax - HUMin);
//	float *_count = new float[maxrange+1];
	float _count[30001];
	float HURange = (float)(HUMax - HUMin) / maxrange;

	memset(_count, 0, sizeof(float)*(30001));
	points.reserve(maxrange + 1);
	logPoints.reserve(maxrange + 1);

	for (mint16 i= HUMin; i<= HUMax; i++)
	{
		int index = i;
		index-=_min;

		if (0 <= index && index < Allsize)
		{
			int dt=0;
			
			if (Allsize > 30000)
			{
				if (index < (Allsize / 2))
					dt = AllCount[index];
				else
					dt = AllCount2[index - (Allsize / 2)];
			}
			else
				dt = AllCount[index];

			if (res)
				file.write((char*)&dt, sizeof(int));

			index = i;
			index-=HUMin;

			int _index = (float)(index *maxrange)/ (HUMax - HUMin);

			if (1.0f == HURange)
				_index = index;

			if ((_index <= maxrange) && (_index >= 0))
				_count[_index] += dt;

		}
	}

	SAFE_DELETES(AllCount);
	SAFE_DELETES(AllCount2);

	if (res)
		file.close();

	for (int i = 0; i <= maxrange; i++)
	{
		if (maxCount < _count[i])
			maxCount = _count[i];

		points.push_back(QPointF((HUMin + (int)(HURange*i)), _count[i]));

		if (_count[i] != 0)
			_count[i] = log10f(_count[i]);

		if (maxLogCount < _count[i])
			maxLogCount = _count[i];
	}

	for (int i = 0; i <= maxrange; i++)
	{
		float val = _count[i] / maxLogCount*maxCount;

		logPoints.push_back(QPointF((HUMin + (int)(HURange*i)), val));
	}
	
//	SAFE_DELETES(_count);

	setAxisScale(QwtPlot::xBottom, HUMin, HUMax);
	
	plotArea.setCurveAttribute(QwtPlotCurve::Inverted);
	plotArea.setSamples(points);
	setAxisScale(QwtPlot::yLeft, 0.0, plotArea.maxYValue());
	//plotArea.setPen(QColor(0, 0, 0, 255), 1);
	plotArea.setPen(QColor(255, 255, 255, 255), 1);
	plotArea.setRenderHint(QwtPlotItem::RenderHint::RenderAntialiased);

	plotArea2.setCurveAttribute(QwtPlotCurve::Inverted);
	plotArea2.setSamples(logPoints);
	setAxisScale(QwtPlot::yLeft, 0.0, plotArea2.maxYValue());
	plotArea2.setPen(QColor(255, 0, 0, 255), 1);
	plotArea2.setRenderHint(QwtPlotItem::RenderHint::RenderAntialiased);

	plotArea.attach(this);
	canvas()->setCursor(Qt::ArrowCursor);

	////////////////////////////////////////////

	double tick = (HUMax - HUMin) / 3.0;
	QList<double> doubleListmin;//min scale// 	
	QList<double> doubleListmed;//medium scale
	QList<double> doubleListmaj;//major scale
	doubleListmin.append((double)HUMin + tick);
	doubleListmed.append((double)HUMin + tick * 2);
	doubleListmaj.append(HUMax);
	QList<double> doubleList[3];
	doubleList[0] = doubleListmin;
	doubleList[1] = doubleListmed;
	doubleList[2] = doubleListmaj;
	QwtScaleDiv *xDiv = new QwtScaleDiv(HUMin, HUMax, doubleList);
	//lowerbound is -0.642455, c is 0.642455, doubleList customizes the scale

	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->setXDiv(*xDiv);
  	grid->attach(this);
	//////////////////////////////////////////////
}

void LayerHistogram::SetHistogram(mint16 _min, mint16 _max, QVector<QPointF> _points, QVector<QPointF> _logPoints, int fileSeq)
{
	this->fileSeq = fileSeq;
	HUMin = _min;
	HUMax = _max;

	this->points.clear();
	this->points = _points;

	this->logPoints.clear();
	this->logPoints = _points;

	setAxisScale(QwtPlot::xBottom, HUMin, HUMax);

	plotArea.setCurveAttribute(QwtPlotCurve::Inverted);
	plotArea.setSamples(points);
	setAxisScale(QwtPlot::yLeft, 0.0, plotArea.maxYValue());
	//plotArea.setPen(QColor(0, 0, 0, 255), 1);
	plotArea.setPen(QColor(255, 255, 255, 255), 1);
	plotArea.setRenderHint(QwtPlotItem::RenderHint::RenderAntialiased);

	plotArea2.setCurveAttribute(QwtPlotCurve::Inverted);
	plotArea2.setSamples(logPoints);
	setAxisScale(QwtPlot::yLeft, 0.0, plotArea2.maxYValue());
	plotArea2.setPen(QColor(255, 0, 0, 255), 1);
	plotArea2.setRenderHint(QwtPlotItem::RenderHint::RenderAntialiased);

	plotArea.attach(this);
	canvas()->setCursor(Qt::ArrowCursor);

	////////////////////////////////////////////

	double tick = (HUMax - HUMin) / 3.0;
	QList<double> doubleListmin;//min scale// 	
	QList<double> doubleListmed;//medium scale
	QList<double> doubleListmaj;//major scale
	doubleListmin.append((double)HUMin + tick);
	doubleListmed.append((double)HUMin + tick * 2);
	doubleListmaj.append(HUMax);
	QList<double> doubleList[3];
	doubleList[0] = doubleListmin;
	doubleList[1] = doubleListmed;
	doubleList[2] = doubleListmaj;
	QwtScaleDiv* xDiv = new QwtScaleDiv(HUMin, HUMax, doubleList);
	//lowerbound is -0.642455, c is 0.642455, doubleList customizes the scale

	QwtPlotGrid* grid = new QwtPlotGrid();
	grid->setXDiv(*xDiv);
	grid->attach(this);
	//////////////////////////////////////////////
}

void LayerHistogram::setCheckZoom(bool value)
{
	if (value)
		plotArea2.attach(this);
	else
		plotArea2.detach();

	replot();
}

QVector<QPointF> LayerHistogram::getPoints(bool _log)
{
	if (_log)
		return logPoints;

	return points;
}

bool LayerHistogram::getRange(mint16 & _min, mint16 & _max)
{
	_min = HUMin;
	_max = HUMax;
	return true;
}

void LayerHistogram::mouseReleaseEvent(QMouseEvent * evt)
{
	if(evt->button() & Qt::RightButton)
	{
		QMenu menu(this);
		menu.addAction(m_actCopy);
		menu.addAction(m_actSave);
		menu.exec(QCursor::pos());
	}
}

void LayerHistogram::OnSave()
{
	MaskInfo * info = DATA_CONTEXT->volume_data.getCurrentMaskInfo();
	QString strCSV;
	bool latest = WIN_MANAGER->lastestPathGet(strCSV, true);
	if (latest)
		strCSV += "/" + DATA_CONTEXT->volume_data.getMaskName(m_uid, true) + "_Histogram";
	else
		strCSV = "";

	QFileInfo fileInfo(strCSV);
	QString fileName = ExportFileDialog(
		this,
		STRING_MANAGER->getString(STR_EXPORT_STL),
		fileInfo.fileName(),
		fileInfo.dir().path(),
		tr("CSV File(*.csv;*.CSV)"),
		QFileDialog::ShowDirsOnly
	);

	if (!fileName.isEmpty())
	{
		QString valText;

		int count = 0;

		QString _tmpFile = STRING_MANAGER->cacheFilePath + QString("/%1%2Hu.tmp").arg(m_uid).arg(fileSeq);

		QFile file(_tmpFile);

		if (!file.open(QIODevice::ReadOnly))
		{			
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DR_0001)).exec();
			return;
		}

		QFile newFile(fileName);

		if (!newFile.open(QIODevice::WriteOnly))
		{
			file.close();
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1002)).exec();
			return;
		}

		valText = QString("%1,Histogram,Accumlated Histogram").arg(WIN_MANAGER->getUnitString());
		newFile.write((const char*)valText.toStdWString().c_str(), sizeof(WCHAR)*valText.size());
		for (mint16 index = HUMin; index <= HUMax; index++)
		{
			int dt = 0;
			file.read((char*)&dt, sizeof(int));

			if (dt > 0)
				count += dt;

			valText = QString("\r\n%1,%2,%3").arg(index).arg(dt).arg(count);
			newFile.write((const char*)valText.toStdWString().c_str(), sizeof(WCHAR)*valText.size());
		}

		newFile.close();
		file.close();
	}
}


void LayerHistogram::OnSaveMacro(QString fileName)
{
	
//	MaskInfo * info = WIN_MANAGER->volume_data.getCurrentMaskInfo();
// 	QString strCSV;
// 	bool latest = WIN_MANAGER->lastestPathGet(strCSV, true);
// 	if (latest)
// 		strCSV += "/" + WIN_MANAGER->volume_data.getMaskName(m_uid, true) + "_Histogram";
// 	else
// 		strCSV = "";
// 
// 	const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
// 	QString selectedFilter;
// 	QString fileName = QFileDialog::getSaveFileName(this,
// 		STRING_MANAGER->getString(STR_EXPORT_STL),
// 		strCSV.isEmpty() ? QDir::homePath() : strCSV, // const QString & dir = QString(),
// 		tr("CSV File(*.csv;*.CSV)"), //const QString & filter = QString()
// 		&selectedFilter, // QString * selectedFilter = 0,
// 		options); // Options options = 0)

	if (!fileName.isEmpty())
	{
		QString valText;

		int count = 0;

		QString _tmpFile = STRING_MANAGER->cacheFilePath + QString("/%1%2Hu.tmp").arg(m_uid).arg(fileSeq);

		QFile file(_tmpFile);

		if (!file.open(QIODevice::ReadOnly))
		{
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DR_0001)).exec();
			return;
		}

		QFile newFile(fileName);

		if (!newFile.open(QIODevice::WriteOnly))
		{
			file.close();			
			QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1002)).exec();
			return;
		}

		valText = QString("%1,Histogram,Accumlated Histogram").arg(WIN_MANAGER->getUnitString());
		newFile.write((const char*)valText.toStdWString().c_str(), sizeof(WCHAR)*valText.size());
		for (mint16 index = HUMin; index <= HUMax; index++)
		{
			int dt = 0;
			file.read((char*)&dt, sizeof(int));

			if (dt > 0)
				count += dt;

			valText = QString("\r\n%1,%2,%3").arg(index).arg(dt).arg(count);
			newFile.write((const char*)valText.toStdWString().c_str(), sizeof(WCHAR)*valText.size());
		}

		newFile.close();
		file.close();
	}
	
}
void LayerHistogram::OnCopy()
{
	QString valText;

	int count = 0;

	QString _tmpFile = STRING_MANAGER->cacheFilePath + QString("/%1%2Hu.tmp").arg(m_uid).arg(fileSeq);

	QFile file(_tmpFile);

	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DR_0001)).exec();
		return;
	}


	valText = QString("%1\tHistogram\tAccumlated Histogram").arg(WIN_MANAGER->getUnitString());

	for (mint16 index = HUMin; index <= HUMax; index++)
	{
		int dt = 0;
		file.read((char*)&dt, sizeof(int));

		if (dt > 0)
			count += dt;

		valText.append(QString("\r\n%1\t%2\t%3").arg(index).arg(dt).arg(count));
	}

	auto clip = QApplication::clipboard();
	clip->clear();
	clip->setText(valText);

	file.close();
}
