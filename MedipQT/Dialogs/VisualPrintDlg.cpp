#include "stdafx.h"
#include "VisualPrintDlg.h"
#include "Windows/Main/MainVisualPrintMeshWidget.h"
#include "UI/VisualPrint/VisualPrintMeshListWidget.h"
#include "styleManager.h"
#include "LicenseManager.h"

#include "WindowBase.h"
#include "windowManager.h"
#include "Network/Network.h"
#include "Main/MainSegmentWidget.h"

#include "UI/CollapseDock.h"

#include "MeshControl.h"

#include "DataContext.h"

#include <ppltasks.h>

VisualPrintDlg::VisualPrintDlg(DataContext* pDataContext, QWidget* parent)
	: QDialog(parent),
	m_pDataContext(pDataContext),
	m_Splliter(0),
	m_rValue(0),
	m_nAddWidgetRowCount(0)
{	
	int width = WIN_MANAGER->ScreenWidth * 0.85;
	int height = WIN_MANAGER->ScreenHeight * 0.85;

	resize(width, height);

	setMaximumWidth(WIN_MANAGER->ScreenWidth);
	setMaximumHeight(WIN_MANAGER->ScreenHeight);

	m_pVisualPrintTab = WIN_MANAGER->GetTab()->getVisualPrintTab();

	m_bVisualPrintMeshTabCreate = true;

	setWindowTitle(STRING_MANAGER->getString(STR_VISUAL_PRINT_DIALOG));
	setStyleSheet("background-color: rgba(82, 82, 82, 255); color: rgba(255,255,255,255);");

	QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
	spLeft.setHorizontalStretch(3);
	QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
	spRight.setHorizontalStretch(1);

	if (WIN_MANAGER->pVisualPrintMeshWidget == NULL)
	{		
		WIN_MANAGER->pVisualPrintMeshWidget = PRODUCT_FACTORY->createWidget<MainVisualPrintMeshWidget>(MFL_Common_VisualPrinting_BasicMeshEditing, this);
		if (WIN_MANAGER->pVisualPrintMeshWidget)
		{	
			WIN_MANAGER->pVisualPrintMeshWidget->init(m_pDataContext);
		}
	}	

	if (!m_Splliter)
	{
		m_Splliter = new QSplitter(Qt::Horizontal, this);
	}
	
	if (WIN_MANAGER->pVisualPrintMeshWidget)
	{		
		// Visual Print Mesh Preview Widget
		QWidget *pQWidgetMesh = WIN_MANAGER->pVisualPrintMeshWidget->getWidgetMesh();
		if (pQWidgetMesh)
		{			
			pQWidgetMesh->setSizePolicy(spLeft);

			pQWidgetMesh->resize(this->width() * 0.6, this->height());

			m_Splliter->addWidget(pQWidgetMesh);
		}	

		// Visual Print Mesh List Tab Widget
//		int nRow = 0;
		m_nAddWidgetRowCount = 0;
		VisualPrintMeshTab *pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
		if (pVisualPrintMeshTab)
		{	
			pVisualPrintMeshTab->setOpenWidget(true);		

			m_bVisualPrintMeshTabCreate = false;			
			pVisualPrintMeshTab->GetVisualPrintMeshListWidget()->updateSize(false);	
			pVisualPrintMeshTab->show();

			WIN_MANAGER->renderLater_All();		

			pVisualPrintMeshTab->setSizePolicy(spRight);

			pVisualPrintMeshTab->resize(this->width() * 0.4, this->height());

			m_Splliter->addWidget(pVisualPrintMeshTab);
		}
		else
		{
			QMessageBox::warning(NULL, "pVisualPrintMeshTab is Null", nullptr);
		}		

		// add empty-box
		QWidget* emptyBox = new QWidget(this);
		emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		pVisualPrintMeshTab->addWidget(emptyBox, m_nAddWidgetRowCount++, 1);

		m_nAddWidgetRowCount++;

		// Sync slider(Smooth / Reduce)
		{
			m_p_LabelSyncSmooth = NULL;
			m_p_LabelSyncSmooth = new QLabel(this);
			m_p_LabelSyncSmooth->setText("Sync Smooth 0");
			pVisualPrintMeshTab->addWidget(m_p_LabelSyncSmooth, m_nAddWidgetRowCount++, 1);

			// Smooth sync slider
			m_pSliderSyncSmooth = NULL;

			m_pSliderSyncSmooth = new QSlider(Qt::Horizontal, this);
			m_pSliderSyncSmooth->setRange(0, 100);
			m_pSliderSyncSmooth->setValue(0);
			m_pSliderSyncSmooth->setStyleSheet(STYLE_MANAGER->slideBarView); 
			m_pSliderSyncSmooth->setSliderDown(true);
			m_pSliderSyncSmooth->installEventFilter(this);
			m_pSliderSyncSmooth->setMouseTracking(true);

			pVisualPrintMeshTab->addWidget(m_pSliderSyncSmooth, m_nAddWidgetRowCount++, 1);

			connect(m_pSliderSyncSmooth, &QSlider::actionTriggered, this, &VisualPrintDlg::SyncSmoothSliderMoved);
			connect(m_pSliderSyncSmooth, &QSlider::valueChanged, this, &VisualPrintDlg::SyncSmoothSliderChanged);
			connect(m_pSliderSyncSmooth, &QSlider::sliderReleased, this, &VisualPrintDlg::SyncSmoothSliderReleased);

			m_p_LabelSyncReduce = NULL;
			m_p_LabelSyncReduce = new QLabel(this);
			m_p_LabelSyncReduce->setText("Sync Reduce 0");
			pVisualPrintMeshTab->addWidget(m_p_LabelSyncReduce, m_nAddWidgetRowCount++, 1);

			// Reduce sync slider
			m_pSliderSyncReduce = NULL;

			m_pSliderSyncReduce = new QSlider(Qt::Horizontal, this);
			m_pSliderSyncReduce->setRange(0, 100);
			m_pSliderSyncReduce->setValue(0);
			m_pSliderSyncReduce->setStyleSheet(STYLE_MANAGER->slideBarView);
			m_pSliderSyncReduce->setSliderDown(true);
			m_pSliderSyncReduce->installEventFilter(this);
			m_pSliderSyncReduce->setMouseTracking(true);

			pVisualPrintMeshTab->addWidget(m_pSliderSyncReduce, m_nAddWidgetRowCount++, 1);

			connect(m_pSliderSyncReduce, &QSlider::actionTriggered, this, &VisualPrintDlg::SyncReduceSliderMoved);
			connect(m_pSliderSyncReduce, &QSlider::valueChanged, this, &VisualPrintDlg::SyncReduceSliderChanged);
			connect(m_pSliderSyncReduce, &QSlider::sliderReleased, this, &VisualPrintDlg::SyncReduceSliderReleased);
		}

		// add empty-box
		pVisualPrintMeshTab->addWidget(emptyBox, m_nAddWidgetRowCount++, 1);
		
		m_p_BtnPreview = new QPushButton(this);
		m_p_BtnPreview->setText(tr("Update"));
		m_p_BtnPreview->setFixedHeight(25);
		m_p_BtnPreview->setStyleSheet(STYLE_MANAGER->buttonBehind);
		m_p_BtnPreview->setGeometry(10, 10, 200, 200);
		pVisualPrintMeshTab->addWidget(m_p_BtnPreview, m_nAddWidgetRowCount++);
		connect(m_p_BtnPreview, &QPushButton::clicked, this, &VisualPrintDlg::VisualPrint);
		
//		// For CT Plane Preview		
//		QLabel *pSpaceUpper = new QLabel(this);		
//		pSpaceUpper->setText("");
//		pVisualPrintMeshTab->addWidget(pSpaceUpper, nRow++, 1);
//
//		Winsize = m_pDataContext->volume_data.getSizeX()*0.5f;
//		if (Winsize < m_pDataContext->volume_data.getSizeY()*0.5f) Winsize = m_pDataContext->volume_data.getSizeY()*0.5f;
//		if (Winsize < m_pDataContext->volume_data.getSizeZ()*0.5f) Winsize = m_pDataContext->volume_data.getSizeZ()*0.5f;						
//
//		// Axial Plane //////////////////////////////////////////////////////////////////////////////////////////////
//		QLabel *pLabelAxial = new QLabel(this);
////		pLabel->setText(tr("(%1, %2, %3)").arg(currentPos->x()).arg(currentPos->y()).arg(currentPos->z()));
//		pLabelAxial->setText("CT Plane (Axial)");
//		pVisualPrintMeshTab->addWidget(pLabelAxial, nRow++, 1);
//		
//		m_pSliderAxial = NULL;
////		if (m_pSliderAxial == NULL)
//		{
//			m_pSliderAxial = new QSlider(Qt::Horizontal, this);
//			m_pSliderAxial->setRange(0, 250);
////			m_pSliderAxial->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));
//			m_pSliderAxial->setValue(100 / 2.0);
//			m_pSliderAxial->setStyleSheet(STYLE_MANAGER->slideBarView);
//			m_pSliderAxial->setSliderDown(true);
//			m_pSliderAxial->installEventFilter(this);
//
//			m_pSliderAxial->setMouseTracking(true);
////			pVisualPrintMeshTab->addWidget(m_pSliderAxial);
//			
//			connect(m_pSliderAxial, &QSlider::actionTriggered, this, &VisualPrintDlg::OnDepthSlideMoved);
//			connect(m_pSliderAxial, &QSlider::valueChanged, this, &VisualPrintDlg::OnDepthSlideChangedAxial);
//			connect(m_pSliderAxial, &QSlider::sliderReleased, this, &VisualPrintDlg::OnDepthSlideReleased);
//			
//			if (!m_pDataContext->volume_data.isValidate())
//				m_pSliderAxial->hide();
//
//			pVisualPrintMeshTab->addWidget(m_pSliderAxial, nRow++, 1);
//		}		
//		InitCTPlaneAxial(m_pSliderAxial, WT_AXIAL);
//		
//		// Coronal Plane //////////////////////////////////////////////////////////////////////////////////////////////
//		QLabel *pLabelCoronal = new QLabel(this);
//		pLabelCoronal->setText("CT Plane (Coronal)");
//		pVisualPrintMeshTab->addWidget(pLabelCoronal, nRow++, 1);
//		m_pSliderCoronal = NULL;
//		//		if (m_pSliderAxial == NULL)
//		{
//			m_pSliderCoronal = new QSlider(Qt::Horizontal, this);
//			m_pSliderCoronal->setRange(0, 99);
//			//			m_pSliderAxial->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));			
//			m_pSliderCoronal->setValue(100 / 2.0);
////			m_pSliderCoronal->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));
//			m_pSliderCoronal->setStyleSheet(STYLE_MANAGER->slideBarView);
//			m_pSliderCoronal->setSliderDown(true);
//			m_pSliderCoronal->installEventFilter(this);
//
//			m_pSliderCoronal->setMouseTracking(true);
//			//			pVisualPrintMeshTab->addWidget(m_pSliderAxial);
//
//			connect(m_pSliderCoronal, &QSlider::actionTriggered, this, &VisualPrintDlg::OnDepthSlideMoved);
//			connect(m_pSliderCoronal, &QSlider::valueChanged, this, &VisualPrintDlg::OnDepthSlideChangedCoronal);
//			connect(m_pSliderCoronal, &QSlider::sliderReleased, this, &VisualPrintDlg::OnDepthSlideReleased);
//
//			if (!m_pDataContext->volume_data.isValidate())
//				m_pSliderCoronal->hide();
//
//			pVisualPrintMeshTab->addWidget(m_pSliderCoronal, nRow++, 1);
//		}
//		InitCTPlaneCoronal(m_pSliderCoronal, WT_CORONAL);
//
//		// Sagittal Plane //////////////////////////////////////////////////////////////////////////////////////////////		
//		QLabel *pLabelSagittal = new QLabel(this);
//		pLabelSagittal->setText("CT Plane (Sagittal)");
//		pVisualPrintMeshTab->addWidget(pLabelSagittal, nRow++, 1);
//
//		m_pSliderSagital = NULL;
//		//		if (m_pSliderAxial == NULL)
//		{
//			m_pSliderSagital = new QSlider(Qt::Horizontal, this);
//			m_pSliderSagital->setRange(0, 99);
//			//			m_pSliderAxial->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));			
//			m_pSliderSagital->setValue(100 / 2.0);
////			m_pSliderSagital->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));
//			m_pSliderSagital->setStyleSheet(STYLE_MANAGER->slideBarView);
//			m_pSliderSagital->setSliderDown(true);
//			m_pSliderSagital->installEventFilter(this);
//
//			m_pSliderSagital->setMouseTracking(true);
//			//			pVisualPrintMeshTab->addWidget(m_pSliderAxial);
//
//			connect(m_pSliderSagital, &QSlider::actionTriggered, this, &VisualPrintDlg::OnDepthSlideMoved);
//			connect(m_pSliderSagital, &QSlider::valueChanged, this, &VisualPrintDlg::OnDepthSlideChangedSagittal);
//			connect(m_pSliderSagital, &QSlider::sliderReleased, this, &VisualPrintDlg::OnDepthSlideReleased);
//
//			if (!m_pDataContext->volume_data.isValidate())
//				m_pSliderSagital->hide();
//
//			pVisualPrintMeshTab->addWidget(m_pSliderSagital, nRow++, 1);
//		}
//		InitCTPlaneSagittal(m_pSliderSagital, WT_SAGITTAL);
//
//		QPushButton *m_p_BtnWebLink = new QPushButton("3D Web View");		
//		m_p_BtnWebLink->setFixedHeight(25);
//		m_p_BtnWebLink->setStyleSheet(STYLE_MANAGER->buttonBehind);
//		m_p_BtnWebLink->setGeometry(10, 10, 200, 200);
//		pVisualPrintMeshTab->addWidget(m_p_BtnWebLink, nRow++, 1);
//		connect(m_p_BtnWebLink, &QPushButton::clicked, this, &VisualPrintDlg::VisualPrintWebLink);		
	}

	connect(this, SIGNAL(sig_updateFinished()), this, SLOT(slot_updateFinished()));
	connect(this, SIGNAL(sig_updateProgress(int)), this, SLOT(slot_updateProgress(int)));
	connect(this, SIGNAL(sig_updateRender(mip::MeshTopology*)), this, SLOT(slot_updateRender(mip::MeshTopology*)));

	m_progressDlg   = NULL;
	WIN_MANAGER->pVisualPrintMeshWidget->setUpdateFinish(true);

	m_Splliter->resize(this->width(), this->height());

	// 201027 허 건 대리
	InitUndoRedo();
}

void VisualPrintDlg::Init()
{
	//	m_pTabBar = NULL;	
	//	m_pHBoxLayout = NULL;	
	//	m_pVisualPrintTab = NULL;	
	//	m_pVisualPrintTab = WIN_MANAGER->GetTab()->getVisualPrintTab();
	//	VisualPrintMeshTab *pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
	//	m_pVisualPrintMeshTab = NULL;	
	//	m_pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();	
	//	m_pVisualPrintMeshTab->setOpenWidget(true);
	//	QMessageBox::warning(NULL, "For Debug", "m_pVisualPrintMeshTab->setOpenWidget(true);");	// For Debug

//	m_bVisualPrintMeshTabCreate = true;

//	setWindowTitle(STRING_MANAGER->getString(STR_VISUAL_PRINT_DIALOG));
//	setStyleSheet("background-color: rgba(82, 82, 82, 255); color: rgba(255,255,255,255);");

	QGridLayout *mainLayout = new QGridLayout(this);
	QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
	spLeft.setHorizontalStretch(3);
	QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
	spRight.setHorizontalStretch(1);


	QVBoxLayout *leftLayout = new QVBoxLayout;
	QVBoxLayout *rightLayout = new QVBoxLayout;


	QGridLayout * pGridLayout = new QGridLayout(this);
	QGridLayout * pGridBox = new QGridLayout;
	pGridLayout->addLayout(pGridBox, 1, 2);

	/*if (WIN_MANAGER->pVisualPrintMeshWidget == NULL)
	{
		WIN_MANAGER->pVisualPrintMeshWidget = PRODUCT_FACTORY->createWidget<MainVisualPrintMeshWidget>(MFL_Common_VisualPrinting_BasicMeshEditing, this);
		if (WIN_MANAGER->pVisualPrintMeshWidget)
		{
			WIN_MANAGER->pVisualPrintMeshWidget->init();
		}
	}*/

	/*if (WIN_MANAGER->pVisualPrintMeshWidget)
	{
		WIN_MANAGER->pVisualPrintMeshWidget->init();
	}*/

	if (WIN_MANAGER->pVisualPrintMeshWidget == NULL)
	{
		WIN_MANAGER->pVisualPrintMeshWidget = PRODUCT_FACTORY->createWidget<MainVisualPrintMeshWidget>(MFL_Common_VisualPrinting_BasicMeshEditing, this);
		if (WIN_MANAGER->pVisualPrintMeshWidget)
		{
			WIN_MANAGER->pVisualPrintMeshWidget->init(m_pDataContext);
		}
	}

	if (WIN_MANAGER->pVisualPrintMeshWidget)
	{
		// Visual Print Mesh Preview Widget
		QWidget *pQWidgetMesh = WIN_MANAGER->pVisualPrintMeshWidget->getWidgetMesh();
		if (pQWidgetMesh)
		{
			pQWidgetMesh->setSizePolicy(spLeft);
			mainLayout->addWidget(pQWidgetMesh, 0, 0);
		}

		//// Visual Print CT Plane Tab Widget
		//VisualPrintCTPlaneTab *pVisualPrintCTPlaneTab = WIN_MANAGER->GetTab()->getVisualPrintCTPlaneTab();
		//QMessageBox::warning(NULL, "For Debug", "VisualPrintCTPlaneTab *pVisualPrintCTPlaneTab = WIN_MANAGER->GetTab()->getVisualPrintCTPlaneTab();");	// For Debug
		//if (pVisualPrintCTPlaneTab)
		//{
		//	//			pVisualPrintMeshTab->Update(false, selIndex);			
		//	pVisualPrintCTPlaneTab->setOpenWidget(true);
		//	pVisualPrintCTPlaneTab->GetVisualPrintMeshListWidget()->update(m_bVisualPrintMeshTabCreate);
		//	m_bVisualPrintMeshTabCreate = false;
		//	pVisualPrintCTPlaneTab->GetVisualPrintMeshListWidget()->updateSize(false);
		//	pVisualPrintCTPlaneTab->show();
		//	WIN_MANAGER->renderLater_All();
		//	//			subVbox->addWidget(pVisualPrintMeshTab);			
		//	pVisualPrintCTPlaneTab->setSizePolicy(spRight);
		//	mainLayout->addWidget(pVisualPrintCTPlaneTab, 0, 1);
		//}
		//else
		//{
		//	QMessageBox::warning(NULL, "VisualPrintCTPlaneTab is Null", nullptr);
		//}

		// Visual Print Mesh List Tab Widget
//		int nRow = 0;
		m_nAddWidgetRowCount = 0;
		VisualPrintMeshTab *pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
		if (pVisualPrintMeshTab)
		{
			//			pVisualPrintMeshTab->Update(false, selIndex);			
			pVisualPrintMeshTab->setOpenWidget(true);
			pVisualPrintMeshTab->GetVisualPrintMeshListWidget()->update(m_bVisualPrintMeshTabCreate);
			m_bVisualPrintMeshTabCreate = false;
			pVisualPrintMeshTab->GetVisualPrintMeshListWidget()->updateSize(false);
			pVisualPrintMeshTab->show();
			WIN_MANAGER->renderLater_All();
			//			subVbox->addWidget(pVisualPrintMeshTab);			
			pVisualPrintMeshTab->setSizePolicy(spRight);
			mainLayout->addWidget(pVisualPrintMeshTab, m_nAddWidgetRowCount++, 2);
		}
		else
		{
			QMessageBox::warning(NULL, "pVisualPrintMeshTab is Null", nullptr);
		}

		// 공백 추가
		QWidget* emptyBox = new QWidget(this);
		emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		pVisualPrintMeshTab->addWidget(emptyBox, m_nAddWidgetRowCount++, 1);

		m_nAddWidgetRowCount++;

		// Sync slider(Smooth / Reduce)
		{
			m_p_LabelSyncSmooth = NULL;

			m_p_LabelSyncSmooth = new QLabel(this);
			m_p_LabelSyncSmooth->setText("Sync Smooth 0");
			pVisualPrintMeshTab->addWidget(m_p_LabelSyncSmooth, m_nAddWidgetRowCount++, 1);

			// Smooth sync slider
			m_pSliderSyncSmooth = NULL;

			m_pSliderSyncSmooth = new QSlider(Qt::Horizontal, this);
			m_pSliderSyncSmooth->setRange(0, 100);
			m_pSliderSyncSmooth->setValue(0);
			m_pSliderSyncSmooth->setStyleSheet(STYLE_MANAGER->slideBarView);
			m_pSliderSyncSmooth->setSliderDown(true);
			m_pSliderSyncSmooth->installEventFilter(this);

			m_pSliderSyncSmooth->setMouseTracking(true);

			connect(m_pSliderSyncSmooth, &QSlider::actionTriggered, this, &VisualPrintDlg::SyncSmoothSliderMoved);
			connect(m_pSliderSyncSmooth, &QSlider::valueChanged, this, &VisualPrintDlg::SyncSmoothSliderChanged);
			connect(m_pSliderSyncSmooth, &QSlider::sliderReleased, this, &VisualPrintDlg::SyncSmoothSliderReleased);

			pVisualPrintMeshTab->addWidget(m_pSliderSyncSmooth, m_nAddWidgetRowCount++, 1);

			m_p_LabelSyncReduce = NULL;
			m_p_LabelSyncReduce = new QLabel(this);
			m_p_LabelSyncReduce->setText("Sync Reduce 0");
			pVisualPrintMeshTab->addWidget(m_p_LabelSyncReduce, m_nAddWidgetRowCount++, 1);

			// Reduce sync slider
			m_pSliderSyncReduce = NULL;

			m_pSliderSyncReduce = new QSlider(Qt::Horizontal, this);
			m_pSliderSyncReduce->setRange(0, 100);
			m_pSliderSyncReduce->setValue(0);
			m_pSliderSyncReduce->setStyleSheet(STYLE_MANAGER->slideBarView);
			m_pSliderSyncReduce->setSliderDown(true);
			m_pSliderSyncReduce->installEventFilter(this);

			m_pSliderSyncReduce->setMouseTracking(true);

			connect(m_pSliderSyncReduce, &QSlider::actionTriggered, this, &VisualPrintDlg::SyncReduceSliderMoved);
			connect(m_pSliderSyncReduce, &QSlider::valueChanged, this, &VisualPrintDlg::SyncReduceSliderChanged);
			connect(m_pSliderSyncReduce, &QSlider::sliderReleased, this, &VisualPrintDlg::SyncReduceSliderReleased);

			pVisualPrintMeshTab->addWidget(m_pSliderSyncReduce, m_nAddWidgetRowCount++, 1);
		}

		pVisualPrintMeshTab->addWidget(emptyBox, m_nAddWidgetRowCount++, 1);

		m_p_BtnPreview = new QPushButton(this);
		m_p_BtnPreview->setText(tr("Update"));
		m_p_BtnPreview->setFixedHeight(25);
		m_p_BtnPreview->setStyleSheet(STYLE_MANAGER->buttonBehind);
		m_p_BtnPreview->setGeometry(10, 10, 200, 200);
		pVisualPrintMeshTab->addWidget(m_p_BtnPreview, m_nAddWidgetRowCount++,1);
		connect(m_p_BtnPreview, &QPushButton::clicked, this, &VisualPrintDlg::VisualPrint);
		
		//// For CT Plane Preview		
		//QLabel *pSpaceUpper = new QLabel(this);
		//pSpaceUpper->setText("");
		//pVisualPrintMeshTab->addWidget(pSpaceUpper, nRow++, 1);

		//Winsize = m_pDataContext->volume_data.getSizeX()*0.5f;
		//if (Winsize < m_pDataContext->volume_data.getSizeY()*0.5f) Winsize = m_pDataContext->volume_data.getSizeY()*0.5f;
		//if (Winsize < m_pDataContext->volume_data.getSizeZ()*0.5f) Winsize = m_pDataContext->volume_data.getSizeZ()*0.5f;

		//// Axial Plane //////////////////////////////////////////////////////////////////////////////////////////////
		//QLabel *pLabelAxial = new QLabel(this);
		////		pLabel->setText(tr("(%1, %2, %3)").arg(currentPos->x()).arg(currentPos->y()).arg(currentPos->z()));
		//pLabelAxial->setText("CT Plane (Axial)");
		//pVisualPrintMeshTab->addWidget(pLabelAxial, nRow++, 1);

		//m_pSliderAxial = NULL;
		////		if (m_pSliderAxial == NULL)
		//{
		//	m_pSliderAxial = new QSlider(Qt::Horizontal, this);
		//	m_pSliderAxial->setRange(0, 250);
		//	//			m_pSliderAxial->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));
		//	m_pSliderAxial->setValue(100 / 2.0);
		//	m_pSliderAxial->setStyleSheet(STYLE_MANAGER->slideBarView);
		//	m_pSliderAxial->setSliderDown(true);
		//	m_pSliderAxial->installEventFilter(this);

		//	m_pSliderAxial->setMouseTracking(true);
		//	//			pVisualPrintMeshTab->addWidget(m_pSliderAxial);

		//	connect(m_pSliderAxial, &QSlider::actionTriggered, this, &VisualPrintDlg::OnDepthSlideMoved);
		//	connect(m_pSliderAxial, &QSlider::valueChanged, this, &VisualPrintDlg::OnDepthSlideChangedAxial);
		//	connect(m_pSliderAxial, &QSlider::sliderReleased, this, &VisualPrintDlg::OnDepthSlideReleased);

		//	/*if (!m_pDataContext->volume_data.isValidate())
		//		m_pSliderAxial->hide();*/			

		//	pVisualPrintMeshTab->addWidget(m_pSliderAxial, nRow++, 1);
		//}
		//InitCTPlaneAxial(m_pSliderAxial, WT_AXIAL);

		//// Coronal Plane //////////////////////////////////////////////////////////////////////////////////////////////
		//QLabel *pLabelCoronal = new QLabel(this);
		//pLabelCoronal->setText("CT Plane (Coronal)");
		//pVisualPrintMeshTab->addWidget(pLabelCoronal, nRow++, 1);
		//m_pSliderCoronal = NULL;
		////		if (m_pSliderAxial == NULL)
		//{
		//	m_pSliderCoronal = new QSlider(Qt::Horizontal, this);
		//	m_pSliderCoronal->setRange(0, 99);
		//	//			m_pSliderAxial->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));			
		//	m_pSliderCoronal->setValue(100 / 2.0);
		//	//			m_pSliderCoronal->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));
		//	m_pSliderCoronal->setStyleSheet(STYLE_MANAGER->slideBarView);
		//	m_pSliderCoronal->setSliderDown(true);
		//	m_pSliderCoronal->installEventFilter(this);

		//	m_pSliderCoronal->setMouseTracking(true);
		//	//			pVisualPrintMeshTab->addWidget(m_pSliderAxial);

		//	connect(m_pSliderCoronal, &QSlider::actionTriggered, this, &VisualPrintDlg::OnDepthSlideMoved);
		//	connect(m_pSliderCoronal, &QSlider::valueChanged, this, &VisualPrintDlg::OnDepthSlideChangedCoronal);
		//	connect(m_pSliderCoronal, &QSlider::sliderReleased, this, &VisualPrintDlg::OnDepthSlideReleased);

		//	/*if (!m_pDataContext->volume_data.isValidate())
		//		m_pSliderCoronal->hide();*/			

		//	pVisualPrintMeshTab->addWidget(m_pSliderCoronal, nRow++, 1);
		//}
		//InitCTPlaneCoronal(m_pSliderCoronal, WT_CORONAL);

		//// Sagittal Plane //////////////////////////////////////////////////////////////////////////////////////////////		
		//QLabel *pLabelSagittal = new QLabel(this);
		//pLabelSagittal->setText("CT Plane (Sagittal)");
		//pVisualPrintMeshTab->addWidget(pLabelSagittal, nRow++, 1);

		//m_pSliderSagital = NULL;
		////		if (m_pSliderAxial == NULL)
		//{
		//	m_pSliderSagital = new QSlider(Qt::Horizontal, this);
		//	m_pSliderSagital->setRange(0, 99);
		//	//			m_pSliderAxial->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));			
		//	m_pSliderSagital->setValue(100 / 2.0);
		//	//			m_pSliderSagital->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));
		//	m_pSliderSagital->setStyleSheet(STYLE_MANAGER->slideBarView);
		//	m_pSliderSagital->setSliderDown(true);
		//	m_pSliderSagital->installEventFilter(this);

		//	m_pSliderSagital->setMouseTracking(true);
		//	//			pVisualPrintMeshTab->addWidget(m_pSliderAxial);

		//	connect(m_pSliderSagital, &QSlider::actionTriggered, this, &VisualPrintDlg::OnDepthSlideMoved);
		//	connect(m_pSliderSagital, &QSlider::valueChanged, this, &VisualPrintDlg::OnDepthSlideChangedSagittal);
		//	connect(m_pSliderSagital, &QSlider::sliderReleased, this, &VisualPrintDlg::OnDepthSlideReleased);

		//	/*if (!m_pDataContext->volume_data.isValidate())
		//		m_pSliderSagital->hide();*/			

		//	pVisualPrintMeshTab->addWidget(m_pSliderSagital, nRow++, 1);
		//}
		//InitCTPlaneSagittal(m_pSliderSagital, WT_SAGITTAL);

		//QPushButton *m_p_BtnWebLink = new QPushButton("3D Web View");
		//m_p_BtnWebLink->setFixedHeight(25);
		//m_p_BtnWebLink->setStyleSheet(STYLE_MANAGER->buttonBehind);
		//m_p_BtnWebLink->setGeometry(10, 10, 200, 200);
		//pVisualPrintMeshTab->addWidget(m_p_BtnWebLink, nRow++, 1);
		//connect(m_p_BtnWebLink, &QPushButton::clicked, this, &VisualPrintDlg::VisualPrintWebLink);
	}

	int width = WIN_MANAGER->ScreenWidth * 0.85;
	int height = WIN_MANAGER->ScreenHeight * 0.85;

	resize(width, height);
	//resize(1024, 768);

	setMaximumWidth(WIN_MANAGER->ScreenWidth);
	setMaximumHeight(WIN_MANAGER->ScreenHeight);
}

void VisualPrintDlg::InitCTPlaneAxial(QSlider *pSlideBar, WINDOW_TYPE windowType)
{
//	Winsize = m_pDataContext->volume_data.getSizeX()*0.5f;
//	if (Winsize < m_pDataContext->volume_data.getSizeY()*0.5f) Winsize = m_pDataContext->volume_data.getSizeY()*0.5f;
//	if (Winsize < m_pDataContext->volume_data.getSizeZ()*0.5f) Winsize = m_pDataContext->volume_data.getSizeZ()*0.5f;

	/*m_Scamera.setPos(mip::VECTOR3(0, 0, -Winsize * 2));

	m_Scamera.setZoomMax(Winsize * 3.0f);
	m_Scamera.setZoom(Winsize*2.0f);
	m_Scamera.setNearFar(0.1f, Winsize * 8);
	m_Scamera.setAt(mip::VECTOR3(0, 0, 0));
	m_Scamera.setOffset(initOffset);*/
	if (pSlideBar)
	{
		pSlideBar->blockSignals(true);

		if (WIN_MANAGER->getWheelZoom())
		{
			mip::VECTOR3 *point = &m_pDataContext->volume_data.axialPPlane[0];
			//WIN_MANAGER->analSlideOffset[0] = WIN_MANAGER->analSlideOffset[0].Zero;
			m_pDataContext->volume_data.getAnal3DPlanes(windowType, point, true);
			//		m_slidebar->setRange(0, SLIDER_RANGE);//50 이상 양수, 미만 음수 (plane의 위치)
			//		m_slidebar->setValue(SLIDER_MID);
			pSlideBar->setRange(0, m_pDataContext->volume_data.getCZ() - 1);
			pSlideBar->setValue(m_pDataContext->volume_data.getCZ() / 2);
		}
		else
		{
			pSlideBar->setRange(0, 99);
			pSlideBar->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));
		}
		pSlideBar->blockSignals(false);
		pSlideBar->show();
//		renderLater();
	}

	/*if (m_comboThickness)
	{
		if (m_comboThickness->isHidden())
		{
			m_comboThickness->show();

			if (!m_btnThickness->isCheckable())
			{
				m_btnThickness->setCheckable(true);
				m_btnThickness->setFixedHeight(m_comboThickness->height());
			}

			if (m_comboThickness->currentIndex() == W2PTK_CUSTOM)
				m_btnThickness->show();
		}
	}

#ifdef _DEBUG
	if (m_comboFilterList)
	{
		if (m_comboFilterList->isHidden())
			m_comboFilterList->show();
	}
#endif*/

}

void VisualPrintDlg::InitCTPlaneCoronal(QSlider *pSlideBar, WINDOW_TYPE windowType)
{
	//	Winsize = m_pDataContext->volume_data.getSizeX()*0.5f;
	//	if (Winsize < m_pDataContext->volume_data.getSizeY()*0.5f) Winsize = m_pDataContext->volume_data.getSizeY()*0.5f;
	//	if (Winsize < m_pDataContext->volume_data.getSizeZ()*0.5f) Winsize = m_pDataContext->volume_data.getSizeZ()*0.5f;

	/*m_Scamera.setPos(mip::VECTOR3(0, 0, -Winsize * 2));

	m_Scamera.setZoomMax(Winsize * 3.0f);
	m_Scamera.setZoom(Winsize*2.0f);
	m_Scamera.setNearFar(0.1f, Winsize * 8);
	m_Scamera.setAt(mip::VECTOR3(0, 0, 0));
	m_Scamera.setOffset(initOffset);*/
	if (pSlideBar)
	{
		pSlideBar->blockSignals(true);

		if (WIN_MANAGER->getWheelZoom())
		{
			mip::VECTOR3 *point = &m_pDataContext->volume_data.coronalPPlane[0];
			//WIN_MANAGER->analSlideOffset[1] = WIN_MANAGER->analSlideOffset[1].Zero;
			m_pDataContext->volume_data.getAnal3DPlanes(windowType, point, true);
			//m_slidebar->setRange(0, SLIDER_RANGE);//50 이상 양수, 미만 음수 (plane의 위치)
			//m_slidebar->setValue(SLIDER_MID);
			pSlideBar->setRange(0, m_pDataContext->volume_data.getCY() - 1);
			pSlideBar->setValue(m_pDataContext->volume_data.getCY() / 2);
		}
		else
		{
			pSlideBar->setRange(0, 99);
			pSlideBar->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));
		}
		pSlideBar->blockSignals(false);
		pSlideBar->show();
		//		renderLater();
	}

	/*if (m_comboThickness)
	{
	if (m_comboThickness->isHidden())
	{
	m_comboThickness->show();

	if (!m_btnThickness->isCheckable())
	{
	m_btnThickness->setCheckable(true);
	m_btnThickness->setFixedHeight(m_comboThickness->height());
	}

	if (m_comboThickness->currentIndex() == W2PTK_CUSTOM)
	m_btnThickness->show();
	}
	}

	#ifdef _DEBUG
	if (m_comboFilterList)
	{
	if (m_comboFilterList->isHidden())
	m_comboFilterList->show();
	}
	#endif*/

}

void VisualPrintDlg::InitCTPlaneSagittal(QSlider *pSlideBar, WINDOW_TYPE windowType)
{
	//	Winsize = m_pDataContext->volume_data.getSizeX()*0.5f;
	//	if (Winsize < m_pDataContext->volume_data.getSizeY()*0.5f) Winsize = m_pDataContext->volume_data.getSizeY()*0.5f;
	//	if (Winsize < m_pDataContext->volume_data.getSizeZ()*0.5f) Winsize = m_pDataContext->volume_data.getSizeZ()*0.5f;

	/*m_Scamera.setPos(mip::VECTOR3(0, 0, -Winsize * 2));

	m_Scamera.setZoomMax(Winsize * 3.0f);
	m_Scamera.setZoom(Winsize*2.0f);
	m_Scamera.setNearFar(0.1f, Winsize * 8);
	m_Scamera.setAt(mip::VECTOR3(0, 0, 0));
	m_Scamera.setOffset(initOffset);*/
	if (pSlideBar)
	{
		pSlideBar->blockSignals(true);

		if (WIN_MANAGER->getWheelZoom())
		{
			mip::VECTOR3 *point = &m_pDataContext->volume_data.saggitalPPlane[0];
			//WIN_MANAGER->analSlideOffset[2] = WIN_MANAGER->analSlideOffset[2].Zero;
			m_pDataContext->volume_data.getAnal3DPlanes(windowType, point, true);
			//m_slidebar->setRange(0, SLIDER_RANGE);//50 이상 양수, 미만 음수 (plane의 위치)
			//m_slidebar->setValue(SLIDER_MID);
			pSlideBar->setRange(0, m_pDataContext->volume_data.getCX() - 1);
			pSlideBar->setValue(m_pDataContext->volume_data.getCX() / 2);
		}
		else
		{
			pSlideBar->setRange(0, 99);
			pSlideBar->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));
		}
		pSlideBar->blockSignals(false);
		pSlideBar->show();
		//		renderLater();
	}

	/*if (m_comboThickness)
	{
	if (m_comboThickness->isHidden())
	{
	m_comboThickness->show();

	if (!m_btnThickness->isCheckable())
	{
	m_btnThickness->setCheckable(true);
	m_btnThickness->setFixedHeight(m_comboThickness->height());
	}

	if (m_comboThickness->currentIndex() == W2PTK_CUSTOM)
	m_btnThickness->show();
	}
	}

	#ifdef _DEBUG
	if (m_comboFilterList)
	{
	if (m_comboFilterList->isHidden())
	m_comboFilterList->show();
	}
	#endif*/

}

// 201102 허 건 대리
void VisualPrintDlg::InitUndoRedo()
{
	if (ACTION_MANAGER->setUndoStack_VisualPrint(new QUndoStack(this)))
	{
		m_pActUndo = ACTION_MANAGER->getUndoStack_VisualPrint()->createUndoAction(this, tr("&Undo"));
		m_pActUndo->setShortcuts(QKeySequence::Undo);

		m_pActRedo = ACTION_MANAGER->getUndoStack_VisualPrint()->createRedoAction(this, tr("&Redo"));
		m_pActRedo->setShortcuts(QKeySequence::Redo);
		//m_pActRedo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));		//내용이 중복되는 듯하여 주석 처리 - 이상일 대리

		addAction(m_pActUndo);
		addAction(m_pActRedo);
	}
}

void VisualPrintDlg::OnDepthSlideMoved(int act)
{		
	if (act == QSlider::SliderNoAction) return;	
	
	if (WIN_MANAGER->getWheelZoom())
	{		
		bool res = false;

		if (!m_pDataContext->volume_data.checkRotatePlanes(WT_AXIAL))
		{			
			//float fRan = max(m_pDataContext->volume_data.getSizeX(),
			//	max(m_pDataContext->volume_data.getSizeY(), m_pDataContext->volume_data.getSizeZ()));
			////	fRan /= 2.0f;
			////	fRan *= 10;

			////	float fSlice = fRan / SLIDER_RANGE;
			//float fSlice = fRan == m_pDataContext->volume_data.getSizeX() ? m_pDataContext->volume_data.getSpaceX() :
			//	fRan == m_pDataContext->volume_data.getSizeY() ? m_pDataContext->volume_data.getSpaceY() : m_pDataContext->volume_data.getSpaceZ();

			//float dt = m_slidebar->sliderPosition();
			//dt /= SLIDER_RANGE;
			//dt *= fRan;
			//dt -= (fRan / 2);
			//dt /= fSlice;
			//dt -= SLIDER_MID; //-500~500
			//dt *= fSlice; //value * (한칸당 길이)

			//	res = m_pDataContext->volume_data.getAllThickness(m_thickness, dt, m_windowType, true, m_slidebar->value() > m_slidebar->sliderPosition());

			float dt = (m_pSliderAxial->sliderPosition() - m_pSliderAxial->value());
			float m_thickness = 0;
			res = m_pDataContext->volume_data.getAllThickness(m_thickness, dt, WT_AXIAL, true, m_pSliderAxial->value() > m_pSliderAxial->sliderPosition());			
		}

		if (res)
		{				
			m_pSliderAxial->blockSignals(true);
			m_pSliderAxial->setValue(m_pSliderAxial->value());
			m_pSliderAxial->blockSignals(false);
		}
	}	
}

void VisualPrintDlg::OnDepthSlideChangedAxial(int value)
{	
	if (WIN_MANAGER->getWheelZoom())
	{		
		float dt = value;
		bool isRotate = m_pDataContext->volume_data.checkRotatePlanes(WT_AXIAL);
		if (isRotate)
		{
			float fRan = max(m_pDataContext->volume_data.getSizeX(),
				max(m_pDataContext->volume_data.getSizeY(), m_pDataContext->volume_data.getSizeZ()));
			//	fRan /= 2.0f;
			//	fRan *= 10;

			//	float fSlice = fRan / SLIDER_RANGE;
			float fSlice = fRan == m_pDataContext->volume_data.getSizeX() ? m_pDataContext->volume_data.getSpaceX() :
				fRan == m_pDataContext->volume_data.getSizeY() ? m_pDataContext->volume_data.getSpaceY() : m_pDataContext->volume_data.getSpaceZ();


			dt /= SLIDER_RANGE;
			dt *= fRan;
			dt -= (fRan / 2);
			dt /= fSlice;
		}

		processSlideZ(dt, true, isRotate, WT_AXIAL);

		WIN_MANAGER->pVisualPrintMeshWidget->getWidgetMesh()->update();
	}	
}

void VisualPrintDlg::OnDepthSlideChangedCoronal(int value)
{
	if (WIN_MANAGER->getWheelZoom())
	{
		muint32 cx, cy, cz;
		m_pDataContext->volume_data.getLengthForScreen(WT_CORONAL, cx, cy, cz);

		float dt = float(cz - value - 1);
		bool isRotate = m_pDataContext->volume_data.checkRotatePlanes(WT_CORONAL);
		if (isRotate)
		{
			float fRan = max(m_pDataContext->volume_data.getSizeX(),
				max(m_pDataContext->volume_data.getSizeY(), m_pDataContext->volume_data.getSizeZ()));
			//	fRan /= 2.0f;
			//	fRan *= 10;

			//	float fSlice = fRan / SLIDER_RANGE;
			float fSlice = fRan == m_pDataContext->volume_data.getSizeX() ? m_pDataContext->volume_data.getSpaceX() :
				fRan == m_pDataContext->volume_data.getSizeY() ? m_pDataContext->volume_data.getSpaceY() : m_pDataContext->volume_data.getSpaceZ();


			dt /= SLIDER_RANGE;
			dt *= fRan;
			dt -= (fRan / 2);
			dt /= fSlice;
		}

		processSlideZ(dt, true, isRotate, WT_CORONAL);

		WIN_MANAGER->pVisualPrintMeshWidget->getWidgetMesh()->update();
	}
}

void VisualPrintDlg::OnDepthSlideChangedSagittal(int value)
{
	if (WIN_MANAGER->getWheelZoom())
	{
		float dt = value;
		bool isRotate = m_pDataContext->volume_data.checkRotatePlanes(WT_SAGITTAL);
		if (isRotate)
		{
			float fRan = max(m_pDataContext->volume_data.getSizeX(),
				max(m_pDataContext->volume_data.getSizeY(), m_pDataContext->volume_data.getSizeZ()));
			//	fRan /= 2.0f;
			//	fRan *= 10;

			//	float fSlice = fRan / SLIDER_RANGE;
			float fSlice = fRan == m_pDataContext->volume_data.getSizeX() ? m_pDataContext->volume_data.getSpaceX() :
				fRan == m_pDataContext->volume_data.getSizeY() ? m_pDataContext->volume_data.getSpaceY() : m_pDataContext->volume_data.getSpaceZ();


			dt /= SLIDER_RANGE;
			dt *= fRan;
			dt -= (fRan / 2);
			dt /= fSlice;
		}

		processSlideZ(dt, true, isRotate, WT_SAGITTAL);

		WIN_MANAGER->pVisualPrintMeshWidget->getWidgetMesh()->update();
	}
}

void VisualPrintDlg::processSlideZ(float dt, bool isSlider, bool isRotate, WINDOW_TYPE windowType)
{
	if (!isSlider)
		dt = dt > 0 ? 1.0f : -1.0f;

	mip::VECTOR3 * planes = NULL;
	//mip::VECTOR3 slideOffset;
//	WINDOW_TYPE windowType = WT_AXIAL;
	switch (windowType)
	{
	case WT_AXIAL:
		if (isSlider && (!isRotate))
		{
			++dt;
			dt -= (float)m_pDataContext->volume_data.getCZ() / 2.f;
		}
		dt *= (float)m_pDataContext->volume_data.getSpaceZ();
		planes = &m_pDataContext->volume_data.axialPPlane[0];
		//slideOffset = WIN_MANAGER->analSlideOffset[0];
		break;
	case WT_CORONAL:
		if (isSlider && (!isRotate))
		{
			++dt;
			dt -= (float)m_pDataContext->volume_data.getCY() / 2.f;
		}
		dt *= (float)m_pDataContext->volume_data.getSpaceY();
		planes = &m_pDataContext->volume_data.coronalPPlane[0];
		//slideOffset = WIN_MANAGER->analSlideOffset[1];
		break;
	case WT_SAGITTAL:
		if (isSlider && (!isRotate))
			dt -= (float)m_pDataContext->volume_data.getCX() / 2.f;
		dt *= (float)m_pDataContext->volume_data.getSpaceX();
		planes = &m_pDataContext->volume_data.saggitalPPlane[0];
		//slideOffset = WIN_MANAGER->analSlideOffset[2];
		break;
	default:
		break;
	}

	if (planes == NULL) return;

	if (isSlider)
	{
		m_pDataContext->volume_data.getAnal3DPlanes(windowType, planes, !isRotate);
	}

	mip::VECTOR3 v = ((planes[0] - planes[1]) ^ (planes[2] - planes[1])).normalize();

	v *= dt;

	for (int n = 0; n < 4; ++n)
	{
		planes[n] += v;
	}

	//if (isSlider)
	//{
	//	if (slideOffset != slideOffset.Zero)
	//	{
	//		for (int i = 0; i < 4; i++)
	//			planes[i] += slideOffset;
	//	}
	//}


	WIN_MANAGER->renderLater_All(false);
}

void VisualPrintDlg::OnDepthSlideReleased()
{
	m_pSliderAxial->setSliderDown(true);
}

VisualPrintDlg::~VisualPrintDlg()
{	
	/*if (m_pTabBar)
	{
		SAFE_DELETE(m_pTabBar)		
	}
	if (m_pHBoxLayout)
	{
		SAFE_DELETE(m_pHBoxLayout)		
	}*/
	/*if (m_pVisualPrintTab)
	{
		SAFE_DELETE(m_pVisualPrintTab)		
	}*/
	/*if (m_pVisualPrintMeshTab)
	{
		SAFE_DELETE(m_pVisualPrintMeshTab)		
	}	*/
	/*if (WIN_MANAGER->pVisualPrintMeshWidget)
	{
		SAFE_DELETE(WIN_MANAGER->pVisualPrintMeshWidget)
	}*/
}

//VisualPrintMeshTab * VisualPrintDlg::GetVisualPrintMeshTab()
//{
//	return m_pVisualPrintMeshTab;
//}

void VisualPrintDlg::Update()
{
	WIN_MANAGER->preRenderProcess();	// For Preview CT Plane

	if (WIN_MANAGER->pVisualPrintMeshWidget)
	{		
		// Visual Print Mesh List Tab Widget
		VisualPrintMeshTab *pVisualPrintMeshTab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();		
		if (pVisualPrintMeshTab)
		{
			m_bVisualPrintMeshTabCreate = false;

			UpdateRender();

			pVisualPrintMeshTab->Update(); 

			pVisualPrintMeshTab->show();

			m_prevType = WIN_MANAGER->mainTabType;

			WIN_MANAGER->mainTabType = MAINTAB_TYPE::MAINTAB_VISUAL_PRINT_MESH_EDITING;

			// 201102 허 건 대리
			// Dlg Close 시, Clear 하기 때문에 다시 set함
			InitUndoRedo();

			// set pck idx
			int		n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();
			auto	p_mesh_view = WIN_MANAGER->pVisualPrintMeshWidget->getMainView();

			p_mesh_view->resetUI();

			p_mesh_view->updatePckModelIndex(n_mesh - 1);

			// For CT Plane Preview	- Start ////////////////////////////////////////////////////////////////////////////////	
			QLabel *pSpaceUpper = new QLabel(this);
			//			int nRow = 0;
			m_nAddWidgetRowCount = 8;
			pSpaceUpper->setText("");
			pVisualPrintMeshTab->delete_addWidget(pSpaceUpper, m_nAddWidgetRowCount++, 1);

			Winsize = m_pDataContext->volume_data.getSizeX()*0.5f;
			if (Winsize < m_pDataContext->volume_data.getSizeY()*0.5f) Winsize = m_pDataContext->volume_data.getSizeY()*0.5f;
			if (Winsize < m_pDataContext->volume_data.getSizeZ()*0.5f) Winsize = m_pDataContext->volume_data.getSizeZ()*0.5f;

			// Axial Plane //////////////////////////////////////////////////////////////////////////////////////////////
			QLabel *pLabelAxial = new QLabel(this);
			//		pLabel->setText(tr("(%1, %2, %3)").arg(currentPos->x()).arg(currentPos->y()).arg(currentPos->z()));
			pLabelAxial->setText("CT Plane (Axial)");
			pVisualPrintMeshTab->delete_addWidget(pLabelAxial, m_nAddWidgetRowCount++, 1);

			m_pSliderAxial = NULL;
			//		if (m_pSliderAxial == NULL)
			{
				m_pSliderAxial = new QSlider(Qt::Horizontal, this);
				m_pSliderAxial->setRange(0, 250);
				//			m_pSliderAxial->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));
				m_pSliderAxial->setValue(100 / 2.0);
				m_pSliderAxial->setStyleSheet(STYLE_MANAGER->slideBarView);
				m_pSliderAxial->setSliderDown(true);
				m_pSliderAxial->installEventFilter(this);

				m_pSliderAxial->setMouseTracking(true);
				//			pVisualPrintMeshTab->addWidget(m_pSliderAxial);

				connect(m_pSliderAxial, &QSlider::actionTriggered, this, &VisualPrintDlg::OnDepthSlideMoved);
				connect(m_pSliderAxial, &QSlider::valueChanged, this, &VisualPrintDlg::OnDepthSlideChangedAxial);
				connect(m_pSliderAxial, &QSlider::sliderReleased, this, &VisualPrintDlg::OnDepthSlideReleased);

				/*if (!m_pDataContext->volume_data.isValidate())
				m_pSliderAxial->hide();*/

				pVisualPrintMeshTab->delete_addWidget(m_pSliderAxial, m_nAddWidgetRowCount++, 1);
			}
			InitCTPlaneAxial(m_pSliderAxial, WT_AXIAL);

			// Coronal Plane //////////////////////////////////////////////////////////////////////////////////////////////
			QLabel *pLabelCoronal = new QLabel(this);
			pLabelCoronal->setText("CT Plane (Coronal)");
			pVisualPrintMeshTab->delete_addWidget(pLabelCoronal, m_nAddWidgetRowCount++, 1);
			m_pSliderCoronal = NULL;
			//		if (m_pSliderAxial == NULL)
			{
				m_pSliderCoronal = new QSlider(Qt::Horizontal, this);
				m_pSliderCoronal->setRange(0, 99);
				//			m_pSliderAxial->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));			
				m_pSliderCoronal->setValue(100 / 2.0);
				//			m_pSliderCoronal->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));
				m_pSliderCoronal->setStyleSheet(STYLE_MANAGER->slideBarView);
				m_pSliderCoronal->setSliderDown(true);
				m_pSliderCoronal->installEventFilter(this);

				m_pSliderCoronal->setMouseTracking(true);
				//			pVisualPrintMeshTab->addWidget(m_pSliderAxial);

				connect(m_pSliderCoronal, &QSlider::actionTriggered, this, &VisualPrintDlg::OnDepthSlideMoved);
				connect(m_pSliderCoronal, &QSlider::valueChanged, this, &VisualPrintDlg::OnDepthSlideChangedCoronal);
				connect(m_pSliderCoronal, &QSlider::sliderReleased, this, &VisualPrintDlg::OnDepthSlideReleased);

				/*if (!m_pDataContext->volume_data.isValidate())
				m_pSliderCoronal->hide();*/

				pVisualPrintMeshTab->delete_addWidget(m_pSliderCoronal, m_nAddWidgetRowCount++, 1);
			}
			InitCTPlaneCoronal(m_pSliderCoronal, WT_CORONAL);

			// Sagittal Plane //////////////////////////////////////////////////////////////////////////////////////////////		
			QLabel *pLabelSagittal = new QLabel(this);
			pLabelSagittal->setText("CT Plane (Sagittal)");
			pVisualPrintMeshTab->delete_addWidget(pLabelSagittal, m_nAddWidgetRowCount++, 1);

			m_pSliderSagital = NULL;
			//		if (m_pSliderAxial == NULL)
			{
				m_pSliderSagital = new QSlider(Qt::Horizontal, this);
				m_pSliderSagital->setRange(0, 99);
				//			m_pSliderAxial->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));			
				m_pSliderSagital->setValue(100 / 2.0);
				//			m_pSliderSagital->setValue(99 - (Winsize*2.0f * 99) / (Winsize*3.0));
				m_pSliderSagital->setStyleSheet(STYLE_MANAGER->slideBarView);
				m_pSliderSagital->setSliderDown(true);
				m_pSliderSagital->installEventFilter(this);

				m_pSliderSagital->setMouseTracking(true);
				//			pVisualPrintMeshTab->addWidget(m_pSliderAxial);

				connect(m_pSliderSagital, &QSlider::actionTriggered, this, &VisualPrintDlg::OnDepthSlideMoved);
				connect(m_pSliderSagital, &QSlider::valueChanged, this, &VisualPrintDlg::OnDepthSlideChangedSagittal);
				connect(m_pSliderSagital, &QSlider::sliderReleased, this, &VisualPrintDlg::OnDepthSlideReleased);

				/*if (!m_pDataContext->volume_data.isValidate())
				m_pSliderSagital->hide();*/

				pVisualPrintMeshTab->delete_addWidget(m_pSliderSagital, m_nAddWidgetRowCount++, 1);
			}
			InitCTPlaneSagittal(m_pSliderSagital, WT_SAGITTAL);
			// For CT Plane Preview	- End ////////////////////////////////////////////////////////////////////////////////

			m_p_BtnWebLink = new QPushButton("3D Web View");
			m_p_BtnWebLink->setFixedHeight(25);
			m_p_BtnWebLink->setStyleSheet(STYLE_MANAGER->buttonBehind);
			m_p_BtnWebLink->setGeometry(10, 10, 200, 200);
			pVisualPrintMeshTab->delete_addWidget(m_p_BtnWebLink, m_nAddWidgetRowCount++, 1);
			connect(m_p_BtnWebLink, &QPushButton::clicked, this, &VisualPrintDlg::VisualPrintWebLink);
		}
		else
		{
//			QMessageBox::warning(NULL, "pVisualPrintMeshTab is Null", nullptr);
		}	
		if(WIN_MANAGER->pVisualPrintMeshWidget->getMainView() != nullptr)
			WIN_MANAGER->pVisualPrintMeshWidget->getMainView()->updateGeometry(true);
	}
}

bool VisualPrintDlg::GetVisualPrintMeshTabCreate()
{
	return m_bVisualPrintMeshTabCreate;
}

void VisualPrintDlg::VisualPrint()
{
	// 200928 허 건 대리
	// Mesh Update 
	if (!WIN_MANAGER->pVisualPrintMeshWidget->getUpdateFinish())
	{
		return;
	}

	// 201030 허 건 대리
	// check parameter
	bool b_check = false;
	int n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();

	for (int i = 0; i < n_mesh; ++i)
	{
		auto info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(i);

		if (info && ((info->m_nSmoothLevel != 0) || (info->m_nReduceLevel != 0)))
		{
			b_check = true;
			break;
		}
	}

	if (b_check)
	{

		WIN_MANAGER->pVisualPrintMeshWidget->setUpdateFinish(false);

		BeginProgress();

		UpdateParallel();
	}
}

void	VisualPrintDlg::UpdateSingle()
{
	concurrency::create_task([this]
	{
		int			n_mesh;

		mip::MeshTopology*		p_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(0);

		MeshInfo*				mesh_info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(0);

		m_ProgressInterval.local() = 40;
		m_ProgressTotal.local() = 0;
		m_countMesh.local() = 1;
		m_pckMesh.local() = 1;

		if (p_mesh)
		{
			// undo redo 
			ACTION_MANAGER->action_UndoRedo_update_VisualPrint(m_pDataContext);

			// Reduce
			m_ProgressTotal.local() += 10;
			emit sig_updateProgress(m_ProgressTotal.local());

			if ((float)mesh_info->m_nReduceLevel > 0)
			{
				m_pDataContext->volume_data.threadResult = mip::mesh_control::collapseTopology(p_mesh, 0, (float)mesh_info->m_nReduceLevel, true, UpdateProgress, this);
			}

			// Smooth
			if ((float)mesh_info->m_nSmoothLevel > 0)
			{
				mip::mesh_control::smoothTopology(p_mesh, 0, (float)mesh_info->m_nSmoothLevel * 0.01f, UpdateProgress, this);
			}

			p_mesh->updateVertex();

			m_ProgressTotal.local() = 80;
			emit sig_updateProgress(m_ProgressTotal.local());

			WIN_MANAGER->buildRenderBufferTopology(p_mesh);

			// save stl file
			{
				QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
				//QString		current_path = QDir::currentPath();
				QString		folder_name = "Temp";
				QString		save_path = current_path + "\\" + folder_name;

				auto		mesh_name = m_pDataContext->m_VisualPrinting_MeshData.GetMeshName(0);

				QString save_file = save_path + "\\" + mesh_name + ".stl";
				WIN_MANAGER->saveMeshFilesVisualPrint(save_file, 0, mesh_info->uid, EX_FILES_STL, false, false);
			}
		}

		emit sig_updateProgress(100);

	}).then([this]
	{
		WIN_MANAGER->renderLater_3DView();

		emit sig_updateFinished();
	});
}

void	VisualPrintDlg::UpdateParallel()
{
	EnableControl(false);

	QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
	QString folder_name = "Temp";
	QString save_path = current_path + "\\" + folder_name;

	if (!QDir(save_path).exists())
	{
		QDir().mkdir(save_path);
	}

	QUndoStack* p_stack = ACTION_MANAGER->getUndoStack_VisualPrint();

	p_stack->beginMacro("block");


	m_ProgressInterval.clear();
	m_ProgressTotal.clear();
	m_countMesh.clear();
	m_pckMesh.clear();

	concurrency::create_task([this]
	{
		int									n_mesh;

		float								sum;

		n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();

		m_ProgressInterval.local() += 100.f / (float)n_mesh * 0.4f;
		m_countMesh.local() += n_mesh;
		m_pckMesh.local() += n_mesh;

		if (n_mesh > 0)
		{
			// undo redo 
			ACTION_MANAGER->action_UndoRedo_update_VisualPrint(m_pDataContext);

			m_ProgressTotal.local() += 10.f;
			emit sig_updateProgress((int)m_ProgressTotal.combine(plus<float>()));

			concurrency::parallel_for(0, n_mesh, [&](int i)
			{
				mip::MeshTopology* p_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(i);

				MeshInfo* mesh_info = m_pDataContext->m_VisualPrinting_MeshData.GetMeshInfo(i);

				if (p_mesh)
				{
					if ((float)mesh_info->m_nReduceLevel > 0)
					{
						m_pDataContext->volume_data.threadResult = mip::mesh_control::collapseTopology(p_mesh, 0, (float)mesh_info->m_nReduceLevel, true, UpdateProgress, this);
					}
					else
					{
						m_ProgressTotal.local() += m_ProgressInterval.local();
					}

					float tmp_progress = m_ProgressTotal.combine(plus<float>());
					emit sig_updateProgress((int)tmp_progress);

					// Smooth
					if ((float)mesh_info->m_nSmoothLevel > 0)
					{
						mip::mesh_control::smoothTopology(p_mesh, 0, (float)mesh_info->m_nSmoothLevel * 0.01f, UpdateProgress, this);
					}
					else
					{
						m_ProgressTotal.local() += m_ProgressInterval.local();
					}

					tmp_progress = m_ProgressTotal.combine(plus<float>());
					emit sig_updateProgress((int)tmp_progress);

					p_mesh->updateVertex();

					emit sig_updateRender(p_mesh);

					m_ProgressTotal.local() += m_ProgressInterval.local() * 0.1f;
					tmp_progress = m_ProgressTotal.combine(plus<float>());

					emit sig_updateProgress((int)tmp_progress);
				}
			});

			emit sig_updateProgress(100);
		}
	}).then([this]
	{
		QUndoStack* p_stack = ACTION_MANAGER->getUndoStack_VisualPrint();

		p_stack->endMacro();

		WIN_MANAGER->renderLater_3DView();

		emit sig_updateFinished();
	});
}

void VisualPrintDlg::VisualPrintWebLink()
{	
	if (!WIN_MANAGER->pVisualPrintMeshWidget->getUpdateFinish())
	{
		return;
	}	

	WIN_MANAGER->GetTab()->getVisualPrintTab()->OnVisualPrintWebLink();

	this->hide();
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////       프로그레스바 구현함수부     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void VisualPrintDlg::BeginProgress()
{
	initProgress();

	m_progressDlg->reset();

	m_progressDlg->setValue(0); 
	
	QApplication::processEvents();

	m_progressDlg->show();

	m_progressDlg->setLabelText("Visual Print");
}

void VisualPrintDlg::initProgress()
{
	if (m_progressDlg != NULL)
	{
		m_progressDlg->deleteLater();
		m_progressDlg = NULL;
	}

	m_progressDlg = new QProgressDialog(this);
	m_progressDlg->setFixedWidth(300);

	QPushButton* btn_cancel = new QPushButton("Cancel");
	m_progressDlg->setCancelButton(btn_cancel);
	btn_cancel->hide();
	btn_cancel->setEnabled(false);

	m_progressDlg->setWindowFlags( Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

	m_progressDlg->setCancelButtonText(STRING_MANAGER->getString(STR_CANCEL));
	m_progressDlg->setRange(0, 100);
	m_progressDlg->setWindowTitle(STRING_MANAGER->getString(STR_WORK));
	//progressDlg->setWindowModality(Qt::WindowModal);
	//	progressDlg->setMinimumDuration(500);
	m_progressDlg->setMinimumDuration(0);
	m_progressDlg->setAutoReset(false);
}


void	VisualPrintDlg::EndProgress()
{
	if (m_progressDlg)
	{
		m_progressDlg->hide();
		m_progressDlg->deleteLater();
		m_progressDlg = NULL;
	}
}

void	VisualPrintDlg::CancelProgress()
{

}

void	VisualPrintDlg::UpdateProgress(int _val)
{
	if (m_progressDlg != NULL && 0 < _val && _val <= 100)
	{
		//if ((progress_value + 5) <= value) // 200924 허 건 대리 주석처리
		{
			m_progressDlg->setValue(_val);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////

// Dialog Close Event
void VisualPrintDlg::closeEvent(QCloseEvent * event)
{
	if (!WIN_MANAGER->pVisualPrintMeshWidget->getUpdateFinish())
	{
		event->ignore();
		return;
	}

	m_bVisualPrintMeshTabCreate = true;

	//WIN_MANAGER->vt_pckIDVisualPrint.clear();

	WIN_MANAGER->enableMeshList(true);
	WIN_MANAGER->enableROIList(true);

	WIN_MANAGER->mainTabType = m_prevType;

	ACTION_MANAGER->VisualPrint_UndoStack_clear();

	m_pDataContext->m_VisualPrinting_MeshData.ClearMeshInfo();

	// 201014 허 건 대리
	// 임시폴더 삭제 
	QString current_path = STRING_MANAGER->m_strAppDataLocalPath; //20210319_byPHS_관리자권한으로 인한 수정
	//QString current_path = QDir::currentPath();
	QString folder_name = "Temp";
	QString save_path = current_path + "\\" + folder_name;
	if (QDir(save_path).exists())
	{
		QDir dir(save_path);
		dir.removeRecursively();
	}
}

// 20201021 허 건 대리
void VisualPrintDlg::resizeEvent(QResizeEvent *event)
{
	if (m_Splliter)
	{
		m_Splliter->resize(this->width(), this->height());
	}
}


void VisualPrintDlg::Onfinish()
{
	QMessageBox::warning(NULL, "VisualPrintDlg", "VisualPrintDlg is Close.");
}

void VisualPrintDlg::OnLinkClick()
{
	QMessageBox::warning(NULL, "VisualPrintDlg", "void VisualPrintDlg::OnLinkClick()");
	m_rValue = LINK_CLICKED;
	close();
}

void	VisualPrintDlg::slot_updateFinished()
{
	EndProgress();

	WIN_MANAGER->pVisualPrintMeshWidget->setUpdateFinish(true);

	WIN_MANAGER->setVisualPrintParams(0, 0);

	m_pSliderSyncSmooth->setValue(0);
	m_pSliderSyncReduce->setValue(0);
	m_p_LabelSyncSmooth->setText("Sync Smooth 0");
	m_p_LabelSyncReduce->setText("Sync Reduce 0");

	EnableControl(true);

	WIN_MANAGER->pVisualPrintMeshWidget->getMainView()->updateGeometry(true);
}

void	VisualPrintDlg::slot_updateProgress(int _val)
{
	UpdateProgress(_val);
}

void	VisualPrintDlg::slot_updateRender(mip::MeshTopology* _p_mesh)
{
	WIN_MANAGER->buildRenderBufferTopology(_p_mesh);
}

bool VisualPrintDlg::eventFilter(QObject * watched, QEvent * evt)
{
	if (NULL == watched) return false;

	const QEvent::Type type = evt->type();

	int res = 0;

	if (type == QEvent::HoverEnter)
	{
		this->unsetCursor();
	}
	else if (type == QEvent::HoverLeave || type == QEvent::Leave || type == QEvent::Show)
	{		
		this->unsetCursor();
	}	

	return QWidget::eventFilter(watched, evt);
}

//201019 허 건 대리
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////         Sync Slider Fuction       ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void	VisualPrintDlg::SyncSmoothSliderMoved(int _act)
{
	if (_act == QSlider::SliderNoAction)
	{
		return;
	}

	int value = m_pSliderSyncSmooth->value();

	WIN_MANAGER->setVisualPrintSmoothVal(value);

	m_p_LabelSyncSmooth->setText(QString().sprintf("Sync Smooth %d", value));
}

void	VisualPrintDlg::SyncSmoothSliderChanged(int _value)
{
	WIN_MANAGER->setVisualPrintSmoothVal(_value);

	m_p_LabelSyncSmooth->setText(QString().sprintf("Sync Smooth %d", _value));
}

void	VisualPrintDlg::SyncSmoothSliderReleased()
{
	int value = m_pSliderSyncSmooth->value();

	WIN_MANAGER->setVisualPrintSmoothVal(value);

	m_p_LabelSyncSmooth->setText(QString().sprintf("Sync Smooth %d", value));
}

void	VisualPrintDlg::SyncReduceSliderMoved(int _act)
{
	if (_act == QSlider::SliderNoAction)
	{
		return;
	}

	int value = m_pSliderSyncReduce->value();

	WIN_MANAGER->setVisualPrintReduceVal(value);

	m_p_LabelSyncReduce->setText(QString().sprintf("Sync Reduce %d", value));
}

void	VisualPrintDlg::SyncReduceSliderChanged(int _value)
{
	WIN_MANAGER->setVisualPrintReduceVal(_value);

	m_p_LabelSyncReduce->setText(QString().sprintf("Sync Reduce %d", _value));
}

void	VisualPrintDlg::SyncReduceSliderReleased()
{
	int value = m_pSliderSyncReduce->value();

	WIN_MANAGER->setVisualPrintReduceVal(value);

	m_p_LabelSyncReduce->setText(QString().sprintf("Sync Reduce %d", value));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////


void	VisualPrintDlg::UpdateRender()
{
	int			n_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMeshCount();

	for (int i = 0; i < n_mesh; ++i)
	{
		auto p_mesh = m_pDataContext->m_VisualPrinting_MeshData.GetMesh(i);

		if (p_mesh)
		{
			p_mesh->updateVertex();

			m_pDataContext->m_VisualPrinting_MeshData.MeshRenderUpdate(i);
		}
	}
}

void	VisualPrintDlg::EnableControl(bool _b_enable)
{
	m_pSliderAxial->setEnabled(_b_enable);
	m_pSliderCoronal->setEnabled(_b_enable);
	m_pSliderSagital->setEnabled(_b_enable);

	m_pSliderSyncSmooth->setEnabled(_b_enable);
	m_pSliderSyncReduce->setEnabled(_b_enable);
	m_p_LabelSyncSmooth->setEnabled(_b_enable);
	m_p_LabelSyncReduce->setEnabled(_b_enable);

	m_p_BtnWebLink->setEnabled(_b_enable);
	m_p_BtnPreview->setEnabled(_b_enable);

	auto tab = WIN_MANAGER->GetTab()->getVisualPrintMeshTab();
	if (tab)
	{
		auto widget = tab->GetVisualPrintMeshListWidget();
		if (widget)
		{
			widget->setEnabled(_b_enable);
		}
	}
}

void VisualPrintDlg::UpdateProgress(float val, void* dt)
{
	VisualPrintDlg* worker = (VisualPrintDlg*)dt;

	worker->m_ProgressTotal.local() += (val * worker->m_ProgressInterval.combine(plus<float>()) * 0.01f);

	int result = (int)(worker->m_ProgressTotal.combine(plus<float>()));
	emit worker->sig_updateProgress(result);
}
