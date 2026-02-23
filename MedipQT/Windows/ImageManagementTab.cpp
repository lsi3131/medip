#include "stdafx.h"
#include "ImageManagementTab.h"

#include "System\resourceManager.h"
#include "System\styleManager.h"
#include "Windows\windowManager.h"
#include "System/stringManager.h"

#include "UI\MaskList.h"

#include "Actions/ActionManager.h"

#include "MedipQT.h"

#include "UI\ImageListWidget.h"
#include "ReportWidget.h"
#include "System/ProductManager.h"
#include "System/FileManager.h"

//---
#include <memory>

ImageManagementTab::ImageManagementTab(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout;

	m_tabImgList = new ImageListTab(this);

	mainLayout->addWidget(m_tabImgList);

	QWidget *emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	mainLayout->addWidget(emptyBox0, 1);

	setLayout(mainLayout);
}

ImageManagementTab::~ImageManagementTab()
{

}

ImageListTab::ImageListTab(QWidget* parent /*= NULL*/)
	:CollapseWidget(QString(), parent)
{
	int nRow = 0;

	QLabel *label = new QLabel(this);
	label->setText("Style");
	addWidget(label, nRow);

	m_comboStyle = new QComboBox(this);
	m_comboStyle->setObjectName("ComboStyle");
	m_comboStyle->addItem("Tile");
	m_comboStyle->addItem("List");
	m_comboStyle->setStyleSheet(STYLE_MANAGER->comboBoxTab);
	m_comboStyle->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	m_comboStyle->installEventFilter(this);
	connect(m_comboStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnStyleChange(int)));

	addWidget(m_comboStyle, nRow);

	QWidget* emptyBox1 = new QWidget(this);
	emptyBox1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	addWidget(emptyBox1, nRow);

	label = new QLabel(this);
	label->setText("Sort by");
	addWidget(label, nRow);

	m_comboSort = new QComboBox(this);
	m_comboSort->setObjectName("ComboSort");
	m_comboSort->addItem("Oldest");//cap 1,2,.....10
	m_comboSort->addItem("Latest");
	m_comboSort->setStyleSheet(STYLE_MANAGER->comboBoxTab);
	m_comboSort->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	m_comboSort->installEventFilter(this);

	m_sort = false;
	connect(m_comboSort, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnSortChange(int)));
	addWidget(m_comboSort, nRow);

	emptyBox1 = new QWidget(this);
	emptyBox1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	addWidget(emptyBox1, nRow++);

	m_listCapture = new ImageListWidget(this);
	m_listCapture->setViewMode(QListWidget::IconMode);
	m_listCapture->setResizeMode(QListView::Adjust);
	m_listCapture->setDragDropMode(QListWidget::DragOnly);
	m_listCapture->setSelectionMode(QListWidget::SingleSelection);
	m_listCapture->setStyleSheet(STYLE_MANAGER->ListWidget);
	m_listCapture->installEventFilter(this);
	connect(m_listCapture, &QListWidget::itemClicked, this, &ImageListTab::slot_OnCaptureClicked);

	m_listImport = new ImageListWidget(this);
	m_listImport->setViewMode(QListWidget::IconMode);
	m_listImport->setResizeMode(QListView::Adjust);
	m_listImport->setDragDropMode(QListWidget::DragOnly);
	m_listImport->setSelectionMode(QListWidget::SingleSelection);
	m_listImport->setStyleSheet(STYLE_MANAGER->ListWidget);
	m_listImport->installEventFilter(this);
	connect(m_listImport, &QListWidget::itemClicked, this, &ImageListTab::slot_OnImportClicked);


	m_tabImg = new QTabWidget(this);
	m_tabImg->setStyleSheet(STYLE_MANAGER->listTabWidget);
	m_tabImg->addTab(m_listCapture, STRING_MANAGER->getString(STR_CAPTURE_TAB));
	m_tabImg->addTab(m_listImport, STRING_MANAGER->getString(STR_IMAGE_IMPORT_TAB));
	addWidget(m_tabImg, nRow++);

	connect(m_tabImg, &QTabWidget::currentChanged, this, &ImageListTab::slot_OnTabChanged);

	label = new QLabel(this);
	label->setText("-");
	addWidget(label, nRow);
	
	m_sliderIcon = new QSlider(Qt::Horizontal, this);
	m_sliderIcon->setObjectName("SliderIcon");	
	m_sliderIcon->setRange(5, 10);
	m_sliderIcon->setValue(5);
	m_sliderIcon->setStyleSheet(STYLE_MANAGER->sliderBarTab);
	m_sliderIcon->installEventFilter(this);
	connect(m_sliderIcon, &QSlider::valueChanged, this, &ImageListTab::slot_OnSizeChange);

	addWidget(m_sliderIcon, nRow);

	label = new QLabel(this);
	label->setText("+");
	addWidget(label, nRow);

	emptyBox1 = new QWidget(this);
	emptyBox1->setObjectName("btnDelSupport");
	emptyBox1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	addWidget(emptyBox1, nRow);

	QPushButton* btn = new QPushButton(this);
	btn->setObjectName("sharedBtnToReport");
	btn->setText("To Report Tab");
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);

	connect(btn, &QPushButton::clicked, this, &ImageListTab::slot_OnToReportTab);

	addWidget(btn, nRow);

	btn = new QPushButton(this);
	btn->setObjectName("sharedBtnDel");
	btn->setText("Delete");
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);

	connect(btn, &QPushButton::clicked, this, &ImageListTab::slot_OnDelete);

	addWidget(btn, nRow);

	btn = new QPushButton(this);
	btn->setText("Save");
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);

	connect(btn, &QPushButton::clicked, this, &ImageListTab::slot_OnSave);
	addWidget(btn, nRow);

	btn = new QPushButton(this);
	btn->setObjectName("loadImage");
	btn->setText("Load");
	btn->hide();
	btn->setStyleSheet(STYLE_MANAGER->buttonBehind);

	connect(btn, &QPushButton::clicked, this, &ImageListTab::slot_OnLoad);

	addWidget(btn, nRow++);

	m_laybtns = getLayout(nRow - 1);

	m_listCWidth = m_listCHeight = m_gridCWidth = m_gridCHeight = 1;

	//grid icon set
	QWidget*AllScreen =
		QApplication::desktop()->screen(QApplication::desktop()->screenNumber(WIN_MANAGER->mainWindow));

	m_gridIWidth = m_gridCWidth = AllScreen->width() / 100.0f * 5;
	m_gridIHeight = m_gridCHeight = AllScreen->width() / 100.0f * 5;

	m_listCapture->setIconSize(QSize(m_gridCWidth, m_gridCHeight));
	m_listImport->setIconSize(QSize(m_gridIWidth, m_gridIHeight));
	//list icon set
	m_listIWidth = m_listCWidth = AllScreen->width() / 100.0f*((float)5 / 2);
	m_listIHeight = m_listCHeight = AllScreen->width() / 100.0f *((float)5 / 2);

	QString title = "Image List";
	setTitle(title);

}

void ImageListTab::UpdateList(IMG_INDEX index)
{
	if (CAPTURE_IMG == index)
	{
		m_listCapture->clear();
		m_listCapture->clearImageList();

		int _w, _h;

		_w = _h = 0;

		if (m_comboStyle->currentIndex() == 0) //grid
		{
			_w = m_gridCWidth;
			_h = m_gridCHeight;
		}
		else
		{
			_w = m_listCWidth;
			_h = m_listCHeight;
		}

		m_listCapture->setIconSize(QSize(_w, _h));

		QImage drawImg = QImage(_w, _h, QImage::Format_RGBA8888);
		int _x, _y;

		int start, end, gap;

		start = m_sort ? (WIN_MANAGER->captureList.size() - 1) : 0;
		end = m_sort ? 0 : (WIN_MANAGER->captureList.size() - 1);
		gap = m_sort ? -1 : 1;

		for (int i = start;; i += gap)
		{
			if (i < 0 || i >= WIN_MANAGER->captureList.size())
				break;

			QListWidgetItem* item = new QListWidgetItem(m_listCapture);
			QImage *img = WIN_MANAGER->captureList.at(i);
			QString str = QString("Capture %1").arg(i + 1);

			QPixmap pixmap = QPixmap::fromImage(*img);

			if (pixmap.width() > _w || pixmap.height() > _h)
			{
				pixmap = pixmap.scaled(QSize(_w, _h), Qt::KeepAspectRatio, Qt::SmoothTransformation);
			}

			_x = _y = 0;

			if (pixmap.width() < _w)
				_x = (_w - pixmap.width()) / 2;

			if (pixmap.height() < _h)
				_y = (_h - pixmap.height()) / 2;

			QPainter p(&drawImg);
			p.fillRect(0, 0, _w, _h, Qt::white);
			p.drawPixmap(_x, _y, pixmap);
			p.end();

			QIcon _icon =
				QIcon(QPixmap::fromImage(drawImg));

			item->setIcon(_icon);
			item->setText(str);
			item->setData(Qt::UserRole + 1, QVariant(i));

			m_listCapture->addItem(item);
			m_listCapture->appendImage(str, *img);
		}

		/*if (m_sort)
			m_listCapture->sortItems(Qt::DescendingOrder);
		else
			m_listCapture->sortItems();*/
	}
	else if (IMPORT_IMG == index)
	{
		m_listImport->clear();
		m_listImport->clearImageList();

		int _w, _h;

		_w = _h = 0;

		if (m_comboStyle->currentIndex() == 0) //grid
		{
			_w = m_gridCWidth;
			_h = m_gridCHeight;
		}
		else
		{
			_w = m_listCWidth;
			_h = m_listCHeight;
		}

		m_listImport->setIconSize(QSize(_w, _h));

		QImage drawImg = QImage(_w, _h, QImage::Format_RGBA8888);
		int _x, _y;

		int start, end, gap;

		start = m_sort ? (WIN_MANAGER->imgList.size() - 1) : 0;
		end = m_sort ? 0 : (WIN_MANAGER->imgList.size() - 1);
		gap = m_sort ? -1 : 1;

		for (int i = start;; i += gap)
		{
			if (i < 0 || i >= WIN_MANAGER->imgList.size())
				break;

			QListWidgetItem* item = new QListWidgetItem(m_listImport);
			QImage *img = WIN_MANAGER->imgList.at(i);
			QString str = QString("Import %1").arg(i + 1);

			QPixmap pixmap = QPixmap::fromImage(*img);

			if (pixmap.width() > _w || pixmap.height() > _h)
			{
				pixmap = pixmap.scaled(QSize(_w, _h), Qt::KeepAspectRatio, Qt::SmoothTransformation);
			}

			_x = _y = 0;

			if (pixmap.width() < _w)
				_x = (_w - pixmap.width()) / 2;

			if (pixmap.height() < _h)
				_y = (_h - pixmap.height()) / 2;

			QPainter p(&drawImg);
			p.fillRect(0, 0, _w, _h, Qt::white);
			p.drawPixmap(_x, _y, pixmap);
			p.end();

			QIcon _icon =
				QIcon(QPixmap::fromImage(drawImg));

			item->setIcon(_icon);
			item->setText(str);
			item->setData(Qt::UserRole + 1, QVariant(i));

			m_listImport->addItem(item);
			m_listImport->appendImage(str, *img);
		}
	}
}

void ImageListTab::changeTab(int index)
{
	if (nullptr == m_tabImg) return;
	m_tabImg->setCurrentIndex(index);
}

int ImageListTab::getTabIndex(const QString& tabName)
{
	QWidget* widget = nullptr;

	if (STRING_MANAGER->getString(STR_CAPTURE_TAB) == tabName)
		widget = m_listCapture;
	else if (STRING_MANAGER->getString(STR_IMAGE_IMPORT_TAB) == tabName)
		widget = m_listImport;

	if (nullptr == widget) return -1;

	return m_tabImg->indexOf(widget);
}

std::map<QString, QImage> ImageListTab::GetCaptureImageList()
{
	return m_listCapture->GetImageList();
}

std::map<QString, QImage> ImageListTab::GetImportImageList()
{
	return m_listImport->GetImageList();
}

void ImageListTab::focusOutEvent(QFocusEvent * evt)
{
	WIN_MANAGER->bCaptureDel = false;
	WIN_MANAGER->bImportDel = false;
}

void ImageListTab::slot_OnTabChanged(int index)
{
	if (index == 0)
	{
		UpdateList(CAPTURE_IMG);
		WIN_MANAGER->bImportDel = false;
		if (m_laybtns)
		{
			for (int i = 0; i < m_laybtns->count(); i++)
			{
				QWidget* item = m_laybtns->itemAt(i)->widget();

				if (item)
				{
					item->show();
				}
				if (item->objectName().contains("loadImage"))
					item->hide();
			}
		}
	}
	else if (index == 1)
	{
		UpdateList(IMPORT_IMG);
		WIN_MANAGER->bCaptureDel = false;
		if (m_laybtns)
		{
			for (int i = 0; i < m_laybtns->count(); i++)
			{
				QWidget* item = m_laybtns->itemAt(i)->widget();

				if (item)
				{
					if (!item->objectName().contains("shared"))
						item->hide();
					if (item->objectName().contains("loadImage"))
						item->show();
				}
			}
		}
	}
}

void ImageListTab::slot_OnSizeChange(int _size)
{
	const IMG_INDEX Tabindex = (IMG_INDEX)m_tabImg->currentIndex();

	QImage *img = RESOURCE_MANAGER->getWaterMark();
	QWidget*AllScreen =
		QApplication::desktop()->screen(QApplication::desktop()->screenNumber(WIN_MANAGER->mainWindow));

	m_gridCWidth = AllScreen->width() / 100.0f *_size;
	m_gridCHeight = AllScreen->width() / 100.0f *_size;

	m_listCWidth = AllScreen->width() / 100.0f*((float)_size / 2);
	m_listCHeight = AllScreen->width() / 100.0f *((float)_size / 2);

	if ((Tabindex == CAPTURE_IMG && WIN_MANAGER->captureList.size() != 0) ||
		(Tabindex == IMPORT_IMG && WIN_MANAGER->imgList.size() != 0))
		UpdateList(Tabindex);
}

void ImageListTab::slot_OnStyleChange(int index)
{
	const IMG_INDEX Tabindex = (IMG_INDEX)m_tabImg->currentIndex();

	if (0 == index) //grid
	{
		m_listCapture->setViewMode(QListWidget::IconMode);
		m_listCapture->setIconSize(QSize(m_gridCWidth, m_gridCHeight));
		m_listCapture->setDragDropMode(QListWidget::DragOnly);

		m_listImport->setViewMode(QListWidget::IconMode);
		m_listImport->setIconSize(QSize(m_gridIWidth, m_gridIHeight));
		m_listImport->setDragDropMode(QListWidget::DragOnly);

	}
	else
	{
		m_listCapture->setViewMode(QListWidget::ListMode);
		m_listCapture->setIconSize(QSize(m_listCWidth, m_listCHeight));
		m_listCapture->setDragDropMode(QListWidget::DragOnly);

		m_listImport->setViewMode(QListWidget::ListMode);
		m_listImport->setIconSize(QSize(m_listIWidth, m_listIHeight));
		m_listImport->setDragDropMode(QListWidget::DragOnly);
	}

	if ((Tabindex == CAPTURE_IMG && WIN_MANAGER->captureList.size() != 0) ||
		(Tabindex == IMPORT_IMG && WIN_MANAGER->imgList.size() != 0))
		UpdateList(Tabindex);
}

void ImageListTab::slot_OnSortChange(int index)
{
	const IMG_INDEX Tabindex = (IMG_INDEX)m_tabImg->currentIndex();

	if (0 == index)
		m_sort = false;
	else
		m_sort = true;

	if ((Tabindex == CAPTURE_IMG && WIN_MANAGER->captureList.size() != 0) ||
		(Tabindex == IMPORT_IMG && WIN_MANAGER->imgList.size() != 0))
		UpdateList(Tabindex);
}

void ImageListTab::slot_OnCaptureClicked(QListWidgetItem *item)
{
	if (NULL == item)
		WIN_MANAGER->bCaptureDel = false;
	else
		WIN_MANAGER->bCaptureDel = true;
}

void ImageListTab::slot_OnImportClicked(QListWidgetItem * item)
{
	if (NULL == item)
		WIN_MANAGER->bImportDel = false;
	else
		WIN_MANAGER->bImportDel = true;
}

void ImageListTab::slot_OnDelete()
{
	int index = m_tabImg->currentIndex();

	if (0 == index)
	{
		QList<QListWidgetItem*> list = m_listCapture->selectedItems();

		if (list.size() != 1)
		{
			return;
		}

		int index = list.at(0)->data(Qt::UserRole + 1).toInt();

		ACTION_MANAGER->action_Capture_image_Del(index);
	}
	else if (1 == index)
	{
		QList<QListWidgetItem*> list = m_listImport->selectedItems();

		if (list.size() != 1)
		{
			return;
		}

		int index = list.at(0)->data(Qt::UserRole + 1).toInt();

		ACTION_MANAGER->action_ImageList_Del(index);
	}
}

void ImageListTab::slot_OnSave()
{
	int index = m_tabImg->currentIndex();

	if (0 != index) 
		return;

	QList<QListWidgetItem*> list = m_listCapture->selectedItems();

	if (list.size() != 1)
	{
		return;
	}

	index = list.at(0)->data(Qt::UserRole + 1).toInt();

	QString strImg;
	bool latest = WIN_MANAGER->lastestPathGet(strImg, true);
	if (latest)
		strImg += "/" + QString("Capture %1").arg(index + 1);
	else
		strImg = "";

	const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
	QString selectedFilter;

	QString strFilter = "";

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_ImageManagement_ImageExport))
	{
		strFilter += QString("PNG (*.png);;JPG (*.jpg);;BMP (*.bmp)");
	}

	QFileInfo fileInfo(strImg);
	QString filePath = ExportFileDialog(
		this,
		STRING_MANAGER->getString(STR_SAVE_CAPTURE),
		fileInfo.fileName(),
		fileInfo.dir().path(),
		strFilter,
		QFileDialog::ShowDirsOnly
	);

	if (!filePath.isEmpty())
	{
		QImage *orgImg = WIN_MANAGER->captureList.at(index);

		QImage saveImg = QImage(orgImg->width(), orgImg->height(), QImage::Format_RGBA8888);
		QPainter p(&saveImg);

		p.drawImage(0, 0, *orgImg);

		if (!WIN_MANAGER->IsLicensePass())
		{
			QImage * WaterImage = RESOURCE_MANAGER->getWaterMark();
			if (WaterImage != NULL)
			{
				QImage drawImg = WaterImage->scaled(QSize(orgImg->width(), orgImg->height() / 3), Qt::KeepAspectRatio, Qt::SmoothTransformation);
				p.drawImage(QPoint(0, orgImg->height() / 2 - drawImg.height() / 2), drawImg);
			}
		}

		p.end();

		saveImg.save(filePath);
	}

}

void ImageListTab::slot_OnLoad()
{
	const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);

	QString strFilter = QString("All Files (*.*)");

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_ReportImport))
	{
		strFilter += QString(";;HTML (*.html)");
	}

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_ImageManagement_ImageImport))
	{
		strFilter += QString(";;PNG (*.png);;JPG (*.jpg);;BMP (*.bmp)");
	}

	QString dirpath = QDir::homePath() + "/Desktop";
	QString path = ImportFileDialog(
		this,
		"OpenImage",
		"",
		dirpath,
		strFilter,
		options
		);

	QString selectedFilter;
	if (path.isEmpty())
	{
		return;
	}
	bool res = WIN_MANAGER->loadImage(path);
	if (false == res)
	{
		QMessageBox::warning(nullptr, "Faild", "Failed to load image.");
	}
}

void ImageListTab::slot_OnToReportTab()
{
	if (nullptr == WIN_MANAGER->mainReportWidget)
		WIN_MANAGER->mainWindow->createReportTab();

	int index = m_tabImg->currentIndex();

	if (0 == index)
	{
		QList<QListWidgetItem*> list = m_listCapture->selectedItems();
		if (list.size() <= 0) return;
		
		WIN_MANAGER->mainReportWidget->importImage(m_listCapture->getImage(list.at(0)->text()));
	}
	else if (1 == index)
	{
		QList<QListWidgetItem*> list = m_listImport->selectedItems();
		if (list.size() <= 0) return;
		
		WIN_MANAGER->mainReportWidget->importImage(m_listImport->getImage(list.at(0)->text()));
	}
}
