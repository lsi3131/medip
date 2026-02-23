#include "stdafx.h"
#include "Dialogs/LayerHistogramDlg.h"
#include "Windows/windowManager.h"
#include "System/styleManager.h"
#include "UI/LayerHistogram.h"

#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>

#include "System/stringManager.h"
#include "System/FileManager.h"


LayerHistogramDlg::LayerHistogramDlg(VOLUME_DATA* pVolumeData, int layerUID, QString _str, QWidget *parent /*= NULL*/)
{
	m_pVolumeData = pVolumeData;

	uid = layerUID;
	QString LayerName = m_pVolumeData->getMaskName(layerUID, true);

	setMouseTracking(true);
	setWindowTitle(QString("`%1` Histogram").arg(LayerName));
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	layout = new QVBoxLayout(this);
	QVBoxLayout * histoLayout = new QVBoxLayout();
	layout->addLayout(histoLayout);

	histogram = new LayerHistogram(layerUID, this);
	histogram->initPlot();
	histogram->CreateHistogram();

	histoLayout->addWidget(histogram);
	setStyleSheet("background-color: rgba(255, 255, 255, 255);");
	btnLog = NULL;
	QHBoxLayout * logLayout = new QHBoxLayout;
	layout->addLayout(logLayout);
	btnLog = new QPushButton(this);
	//	btnLog->setFixedSize(80, 25);
	btnLog->setText(QString("Log"));
	btnLog->setCheckable(true);
	btnLog->setChecked(false);
	btnLog->setStyleSheet(STYLE_MANAGER->buttonBehind);
	logLayout->addWidget(btnLog,1);
	connect(btnLog, &QPushButton::released, this, &LayerHistogramDlg::slot_OnLogHistogram);
	QWidget *emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	logLayout->addWidget(emptyBox, 1);

	if(_str.size() != 0)
	{
		QHBoxLayout * labelLayout = new QHBoxLayout();
		layout->addLayout(labelLayout);
		str = _str;
		QLabel *labelmsg = new QLabel(str, this);
		labelLayout->addWidget(labelmsg);
	}
	
	logLayout = new QHBoxLayout;
	layout->addLayout(logLayout);

	QPushButton* btnCopy = new QPushButton(this);
	btnCopy->setText(QString("Histogram Copy"));
	btnCopy->setStyleSheet(STYLE_MANAGER->buttonBehind);
	btnCopy->setToolTip("Copy histogram to text.");
	logLayout->addWidget(btnCopy);
	connect(btnCopy, &QPushButton::released, histogram, &LayerHistogram::OnCopy);

	QPushButton* btnSave = new QPushButton(this);
	btnSave->setText(QString("Histogram Save"));
	btnSave->setStyleSheet(STYLE_MANAGER->buttonBehind);
	btnSave->setToolTip("Save histogram to *.csv file.");
	logLayout->addWidget(btnSave);
	connect(btnSave, &QPushButton::released, histogram, &LayerHistogram::OnSave);

	btnCopy = new QPushButton(this);
	btnCopy->setText(QString("Feature Copy"));
	btnCopy->setStyleSheet(STYLE_MANAGER->buttonBehind);
	btnCopy->setToolTip("Copy feature to text.");
	logLayout->addWidget(btnCopy);
	connect(btnCopy, &QPushButton::released, this, &LayerHistogramDlg::OnCopy);

	btnSave = new QPushButton(this);
	btnSave->setText(QString("Feature Save"));
	btnSave->setStyleSheet(STYLE_MANAGER->buttonBehind);
	btnSave->setToolTip("Save feature to *.csv file.");
	logLayout->addWidget(btnSave);
	connect(btnSave, &QPushButton::released, this, &LayerHistogramDlg::OnSave);

}

LayerHistogramDlg::~LayerHistogramDlg()
{
}

void LayerHistogramDlg::OnCopy()
{
	auto clip = QApplication::clipboard();
	clip->clear();
	if (str.size() != 0)
	{
		QString copyStr = str;

		copyStr.replace("\t", "");
		copyStr.replace("\n", "\r\n");
		copyStr.replace(" : ", "\t");

		clip->setText(copyStr);
	}
}

//== 호출되지 않음 
void LayerHistogramDlg::OnSave()
{
	MaskInfo * info = m_pVolumeData->getCurrentMaskInfo();
	QString strCSV;
	bool latest = WIN_MANAGER->lastestPathGet(strCSV, true);
	if (latest)
		strCSV += "/" + m_pVolumeData->getMaskName(uid, true) +"_Feature";
	else
		strCSV = "";

	const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
	QString selectedFilter;
	QString fileName = QFileDialog::getSaveFileName(this,
		STRING_MANAGER->getString(STR_EXPORT_STL),
		strCSV.isEmpty() ? QDir::homePath() : strCSV, // const QString & dir = QString(),
		tr("CSV File(*.csv;*.CSV)"), //const QString & filter = QString()
		&selectedFilter, // QString * selectedFilter = 0,
		options); // Options options = 0)

	if (!fileName.isEmpty())
	{
		QString copyStr = str;

		copyStr.replace("\t", "");
		copyStr.replace("\n", "\r\n");
		copyStr.replace(" : ", ",");
		
		QFile newFile(fileName);

		if (!newFile.open(QIODevice::WriteOnly))
		{
			QMessageBox::warning(NULL, "Feature file save fail.", "csv file create fail.");
			return;
		}

		newFile.write((const char*)copyStr.toStdWString().c_str(), sizeof(WCHAR)*copyStr.size());

		newFile.close();
	}
}



void LayerHistogramDlg::slot_OnLogHistogram()
{
	if (btnLog->isChecked())
		btnLog->setText(QString("Non-Log"));
	else
		btnLog->setText(QString("Log"));

	histogram->setCheckZoom(btnLog->isChecked());
}
