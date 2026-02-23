#include "stdafx.h"
#include "CaptureTab.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "StyleManager.h"
#include "LicenseManager.h"
#include "ProductManager.h"
#include "ActionManager.h"
#include "Main/MainAnalWidget.h"
#include "Main/MainSegmentWidget.h"
#include "MedipQT.h"
#include "AnalVolumeView.h"
#include "AnalMPRPlaneView.h"
#include <QApplication>
#include <QDesktopWidget>
#include "DataContext.h"

CaptureTab::CaptureTab(QWidget* parent /*= NULL*/) : 
	CollapseWidget(QString(), parent)
{
	int nRow = 0;

	QLabel* labelMode = new QLabel(this);
	labelMode->setText("Mode");
	labelMode->setStyleSheet(STYLE_MANAGER->labelNormal);

	addWidget(labelMode, nRow);

	m_comboMode = new QComboBox(this);
	m_comboMode->addItem("Rectangle");
	m_comboMode->addItem("Specific Window");
	m_comboMode->addItem("All Windows");
	m_comboMode->addItem("Full Screen");
	m_comboMode->setStyleSheet(STYLE_MANAGER->comboBoxTab);
	m_comboMode->installEventFilter(this);
	connect(m_comboMode, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnComboChanged(int)));

	addWidget(m_comboMode, nRow);

	QWidget* emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	addWidget(emptyBox, nRow++, 1);


	QGroupBox* boxGroup = new QGroupBox(this);
	boxGroup->setTitle("Capture Type");
	QVBoxLayout* LayGroup = new QVBoxLayout;
	boxGroup->setLayout(LayGroup);

	m_groupType = new QButtonGroup(this);

	QRadioButton* radioROI = new QRadioButton(this);
	radioROI->setText(QString("Only Data && ROI"));
	radioROI->setCheckable(true);
	radioROI->setChecked(true);
	radioROI->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	QRadioButton* radioAll = new QRadioButton(this);
	radioAll->setText("Capture All(Coord, Annotation, etc...)");
	radioAll->setCheckable(true);
	radioAll->setChecked(false);
	radioAll->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	m_groupType->addButton(radioROI, 0);
	m_groupType->addButton(radioAll, 1);

	connect(m_groupType, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnTypeChanged(int)));

	LayGroup->addWidget(radioROI);
	LayGroup->addWidget(radioAll);

	addWidget(boxGroup, nRow++);

	m_btnCapture = new QPushButton(this);
	m_btnCapture->setText("Capture");
	m_btnCapture->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnCapture->setCheckable(true);
	m_btnCapture->setChecked(false);

	connect(m_btnCapture, &QPushButton::clicked, this, &CaptureTab::slot_OnCapture);

	addWidget(m_btnCapture, nRow++);

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	addWidget(emptyBox0, nRow++);

	QString title = "Screen Capture";
	setTitle(title);

	setOpenWidget(false);

	m_mode = false;
	m_type = false;

}


void CaptureTab::slot_OnCapture()
{
	int index = m_comboMode->currentIndex();

	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		cancelCapture();
		return;
	}

	if (m_btnCapture->isChecked())
	{
		if (3 == index || 2 == index) //all windows
		{
			if (3 == index)
				CaptureFull();
			else
				Capture4Windows();

			cancelCapture();

			return;
		}

		m_btnCapture->setText("Capture cancel");

		if (WIN_MANAGER->mainSegmentWidget)
			WIN_MANAGER->setWorkMode(WORK_CAPTURE);
		if (WIN_MANAGER->mainAnalWidget)
			WIN_MANAGER->mainAnalWidget->setWorkMode(ANAL_WORK_CAPTURE);
	}
	else
	{
		cancelCapture();
	}
}

void CaptureTab::cancelCapture()
{
	if (m_btnCapture)
	{
		m_btnCapture->setChecked(false);
		m_btnCapture->setText("Capture");
	}

	if (WIN_MANAGER->mainSegmentWidget)
	{
		WORK_MODE md = WIN_MANAGER->getWorkMode();

		if (md == WORK_CAPTURE)
			WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
	}
	if (WIN_MANAGER->mainAnalWidget)
	{
		ANAL_WORK_MODE md = WIN_MANAGER->getAnalWorkMode();

		if (md == ANAL_WORK_CAPTURE)
			WIN_MANAGER->mainAnalWidget->setWorkMode(ANAL_WORK_NONE);
	}
}

void CaptureTab::slot_OnTypeChanged(int index)
{
	WIN_MANAGER->setMoveFocus(true);
	if (0 == index) //default
	{
		m_groupType->button(1)->setChecked(false);
		m_type = false;
	}
	else
	{
		m_groupType->button(0)->setChecked(false);
		m_type = true;
	}
}

void CaptureTab::slot_OnComboChanged(int index)
{
	WIN_MANAGER->setMoveFocus(true);

	if (0 == index) //Rectangle
		m_mode = false;
	else if (1 == index) //Specific window
		m_mode = true;
	else //All window & full screen
	{
		if (m_btnCapture->isChecked())
			cancelCapture();
	}
}

void CaptureTab::CaptureFull(bool save)
{
	QPixmap allPix;
	int scIndex = QApplication::desktop()->screenNumber(WIN_MANAGER->mainWindow);
	QWidget* AllScreen = QApplication::desktop()->screen(scIndex);
	QRect scRect = AllScreen->frameGeometry();//QApplication::screens().at(scIndex)->availableVirtualGeometry();
	QRect rect = WIN_MANAGER->mainWindow->frameGeometry();
	QRect orgRect = WIN_MANAGER->mainWindow->geometry();

	rect.setLeft(orgRect.left() >= scRect.left() ? orgRect.left() : scRect.left());
	rect.setRight(orgRect.right() <= scRect.right() ? orgRect.right() : scRect.right());

	rect.setTop(rect.top() >= scRect.top() ? rect.top() : scRect.top());
	rect.setBottom(orgRect.bottom() <= scRect.bottom() ? orgRect.bottom() : scRect.bottom());


	allPix = QApplication::primaryScreen()->grabWindow(AllScreen->winId(), rect.x(), rect.y(), rect.width(), rect.height());
	QImage img = QImage(rect.width(), rect.height(), QImage::Format_RGBA8888);
	QPainter p(&img);
	p.drawPixmap(0, 0, allPix);
	p.end();
	if (save)
		ACTION_MANAGER->action_Capture_image_Add(img);
	else
		WIN_MANAGER->setThumbnail(img);
}

void CaptureTab::Capture4Windows()
{
	QImage* img;

	int w = 0, h = 0;

	img = new QImage[4];

	for (int i = 0; i < 4; i++)
		img[i] = QImage();


	if (MAINTAB_SEGMENTATION == WIN_MANAGER->mainTabType)
	{
		VolumeView* vView = dynamic_cast<VolumeView*>(WIN_MANAGER->mainSegmentWidget->getViewVolume());

		if (vView)
		{
			img[0] = vView->getCaptureImage(true, m_type);
			w = img[0].width();
			h = img[0].height();
		}


		for (int i = 0; i < 3; i++)
		{
			WindowBase* win = dynamic_cast<WindowBase*>(WIN_MANAGER->mainSegmentWidget->getWindow((WINDOW_TYPE)(WT_AXIAL + i)));

			if (win)
			{
				img[i + 1] = win->getCaptureImage(true, m_type);
				if (w > img[i + 1].width())
					w = img[i + 1].width();
				if (h > img[i + 1].height())
					h = img[i + 1].height();
			}
		}


	}
	else if (MAINTAB_MEASUREMENT == WIN_MANAGER->mainTabType)
	{
		AnalVolumeView* vView = dynamic_cast<AnalVolumeView*>(WIN_MANAGER->mainAnalWidget->getViewVolume());

		if (vView)
		{
			img[0] = vView->getCaptureImage(true, m_type);
			w = img[0].width();
			h = img[0].height();
		}

		for (int i = 0; i < 3; i++)
		{
			AnalMPRPlaneView* win = dynamic_cast<AnalMPRPlaneView*>(WIN_MANAGER->mainAnalWidget->getWindow((WINDOW_TYPE)(WT_AXIAL + i)));

			if (win)
			{
				img[i + 1] = win->getCaptureImage(true, m_type);
				if (w > img[i + 1].width())
					w = img[i + 1].width();
				if (h > img[i + 1].height())
					h = img[i + 1].height();
			}
		}
	}

	QImage allImg = QImage((2 * w), (2 * h), QImage::Format_RGBA8888);
	QPainter p(&allImg);

	QImage drawImg = (img[0]);
	if (img[0].width() > w)
		drawImg = img[0].scaled(QSize(w, h), Qt::AspectRatioMode::KeepAspectRatioByExpanding, Qt::TransformationMode::SmoothTransformation);
	p.drawImage(0, 0, drawImg);


	drawImg = (img[1]);
	if (img[1].width() > w)
		drawImg = img[1].scaled(QSize(w, h), Qt::AspectRatioMode::KeepAspectRatioByExpanding, Qt::TransformationMode::SmoothTransformation);
	p.drawImage(w, 0, drawImg);

	drawImg = (img[2]);
	if (img[2].width() > w)
		drawImg = img[2].scaled(QSize(w, h), Qt::AspectRatioMode::KeepAspectRatioByExpanding, Qt::TransformationMode::SmoothTransformation);
	p.drawImage(0, h, drawImg);

	drawImg = (img[3]);
	if (img[3].width() > w)
		drawImg = img[3].scaled(QSize(w, h), Qt::AspectRatioMode::KeepAspectRatioByExpanding, Qt::TransformationMode::SmoothTransformation);
	p.drawImage(w, h, drawImg);
	p.end();

	SAFE_DELETES(img);

	ACTION_MANAGER->action_Capture_image_Add(allImg);
}
