#include "stdafx.h"
#include "OptionalTab.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "StyleManager.h"
#include "LicenseManager.h"
#include "ProductManager.h"
#include "ResourceManager.h"
#include "ShortcutManager.h"
#include "ApplicationManager.h"
#include "Config/VisualPrintConfig.h"
#include "Config/ConfigManager.h"
#include "MedipQT.h"
#include "Main/MainSegmentWidget.h"
#include "Main/MainMeshWidget.h"
#include "Main/MainAnalWidget.h"
#include "FileManager.h"
#include "MEVolumeView.h"
#include "DataContext.h"

OptionalTab::OptionalTab(ProductManager* pProductManager, QWidget* parent /*= NULL*/) :
	CollapseWidget(QString(), parent),
	m_pProductManager(pProductManager)
{
	m_pProductFactory = m_pProductManager->getFactory();

	int nRow = 0;

	QLabel* pLabelGamma = new QLabel(this);
	pLabelGamma->setText(QString("Gamma"));
	//QBoxLayout *LayGroup = new QVBoxLayout;
	//pLabelGamma->setLayout(LayGroup);
	//QHBoxLayout *hBox = new QHBoxLayout;
	//LayGroup->addLayout(hBox);

	m_chkGamma = new QCheckBox("Use Gamma", this);
	m_chkGamma->setCheckable(true);
	m_chkGamma->setChecked(WIN_MANAGER->isGammaMode());
	m_chkGamma->setStyleSheet(STYLE_MANAGER->m_Checkbox);

	connect(m_chkGamma, &QCheckBox::clicked, this, &OptionalTab::slot_OnChkGamma);

	addWidget(pLabelGamma, nRow++, 1);
	addWidget(m_chkGamma, nRow++, 1);

	// 	hBox = new QHBoxLayout;
	// 	LayGroup->addLayout(hBox);

	m_labelGamma = new QLabel(STRING_MANAGER->getString(STR_GAMMA), this);

	QWidget* emptyBox0;

	m_textGamma = new QLineEdit(this);
	m_textGamma->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_textGamma->setText(QString::number(WIN_MANAGER->getGamma()));
	m_textGamma->setObjectName("EditGamma");
	m_textGamma->installEventFilter(this);
	m_textGamma->setMouseTracking(true);

	connect(m_textGamma, &QLineEdit::returnPressed, this, &OptionalTab::slot_OnGammaEditFinish);
	connect(m_textGamma, &QLineEdit::editingFinished, this, &OptionalTab::slot_OnGammaEditFinish);

	m_btnGamma = new QPushButton(this);
	m_btnGamma->setText(STRING_MANAGER->getString(STR_RESET));
	m_btnGamma->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(m_btnGamma, &QPushButton::clicked, this, &OptionalTab::slot_OnReset);

	addWidget(m_labelGamma, nRow, 1);
	addWidget(m_textGamma, nRow, 2);
	addWidget(m_btnGamma, nRow, 2);
	nRow++;


	// 	hBox = new QHBoxLayout;
	// 	LayGroup->addLayout(hBox);

	m_slideGamma = new QSlider(this);
	m_slideGamma->setObjectName("SliderGamma");
	m_slideGamma->setOrientation(Qt::Horizontal);
	m_slideGamma->installEventFilter(this);
	m_slideGamma->setRange(1, 100);
	m_slideGamma->setValue((int)(WIN_MANAGER->getGamma() * 10));
	m_slideGamma->setStyleSheet(STYLE_MANAGER->sliderBarTab);
	connect(m_slideGamma, SIGNAL(sliderMoved(int)), this, SLOT(slot_OnGammaChanged(int)));

	addWidget(m_slideGamma, nRow++, 1);

	slot_OnChkGamma();

	createFrameLine(nRow++);
	//////////////////////////


	QLabel* labelDesc = nullptr;

	//addWidget(NULL, nRow++);
	// 	QBoxLayout *LayMain = getLayout(nRow - 1);
	// 	QHBoxLayout* LayWheel = new QHBoxLayout;
	// 	QHBoxLayout* LayRedo = new QHBoxLayout;
	//
	// 	LayMain->addLayout(LayWheel, 1);
	// 	LayMain->addLayout(LayRedo, 1);

	//	QFrame *frLine;

	QLabel* pLabelWheelSlider = new QLabel(this);
	pLabelWheelSlider->setText(QString("%1Wheel / %2Slider").arg(QString(QChar(0x2195))).arg(QString(QChar(0x2194))));

	m_FuncGroup = new QButtonGroup(this);

	QRadioButton* radioDefault = new QRadioButton(this);
	radioDefault->setText(QString("%1Zoom / %2Depth").arg(QString(QChar(0x2195))).arg(QString(QChar(0x2194))));
	radioDefault->setCheckable(true);
	radioDefault->setChecked(WIN_MANAGER->getWheelZoom());
	radioDefault->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	QRadioButton* radioChange = new QRadioButton(this);
	radioChange->setText(QString("%1Depth / %2Zoom").arg(QString(QChar(0x2195))).arg(QString(QChar(0x2194))));
	radioChange->setCheckable(true);
	radioChange->setChecked(!WIN_MANAGER->getWheelZoom());
	radioChange->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

	m_FuncGroup->addButton(radioDefault, 0);
	m_FuncGroup->addButton(radioChange, 1);

	connect(m_FuncGroup, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnChangeMode(int)));

	addWidget(pLabelWheelSlider, nRow++, 1);
	addWidget(radioDefault, nRow, 1);
	addWidget(radioChange, nRow, 1);
	nRow++;

	if (WIN_MANAGER->IsLicensePass())
	{
		QLabel* pLabelReUndo = new QLabel(this);
		pLabelReUndo->setText(QString("Re/Undo limit"));
		pLabelReUndo->setFixedWidth(100);
		// 		LayGroup = new QVBoxLayout;
		// 		boxGroup->setLayout(LayGroup);

		m_textLimit = new QLineEdit(this);
		m_textLimit->setText(QString::number(WIN_MANAGER->getUndoLimit()));
		m_textLimit->setValidator(new QIntValidator(this));
		m_textLimit->setObjectName("EditLimit");
		m_textLimit->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textLimit->installEventFilter(this);
		m_textLimit->setMouseTracking(true);
		//	connect(m_textLimit, &QLineEdit::returnPressed, this, &OptionalTab::slot_OnUndoEditFinish);
		connect(m_textLimit, &QLineEdit::editingFinished, this, &OptionalTab::slot_OnUndoEditFinish);

		addWidget(pLabelReUndo, nRow, 1);
		addWidget(m_textLimit, nRow++, 1);

	}
	else
	{
		emptyBox0 = new QWidget(this);
		emptyBox0->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		addWidget(emptyBox0, nRow++, 1);
	}


	if (WIN_MANAGER->IsLicensePass())
	{
		createFrameLine(nRow++);

		QLabel* pLabelTemDir = new QLabel(this);
		pLabelTemDir->setText("Temporary Directory");

		m_btnTmp = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_FILE_NON_OPEN), "", this);
		m_btnTmp->setIconSize(QSize(30, 30));
		m_btnTmp->setFixedSize(QSize(30, 30));
		m_btnTmp->setMouseTracking(true);
		m_btnTmp->installEventFilter(this);
		m_btnTmp->setObjectName("ButtonTemporary");
		m_btnTmp->setToolTip("Change temporary directory");
		m_btnTmp->setStyleSheet("color: black;");
		connect(m_btnTmp, &QPushButton::clicked, this, &OptionalTab::slot_OnTempDirOpen);

		m_lineTmp = new QLineEdit(STRING_MANAGER->cacheFilePath, this);
		m_lineTmp->setReadOnly(true);
		m_lineTmp->setStyleSheet(STYLE_MANAGER->editBoxTab);

		addWidget(pLabelTemDir, nRow++, 1);
		addWidget(m_btnTmp, nRow, 1);
		addWidget(m_lineTmp, nRow++, 5);

		// expert report temp directory
		if (m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY) == PRODUCT_NAME_DEEPCATCH)
		{
			createFrameLine(nRow++);

			QLabel* pLabelTemDir_report = new QLabel(this);
			pLabelTemDir_report->setText("Expert report file path");

			m_btnTmp_report = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_FILE_NON_OPEN), "", this);
			m_btnTmp_report->setIconSize(QSize(30, 30));
			m_btnTmp_report->setFixedSize(QSize(30, 30));
			m_btnTmp_report->setMouseTracking(true);
			m_btnTmp_report->installEventFilter(this);
			m_btnTmp_report->setObjectName("ButtonExpertReportFilePath");
			m_btnTmp_report->setToolTip("Change expert report file path");
			m_btnTmp_report->setStyleSheet("color: black;");
			connect(m_btnTmp_report, &QPushButton::clicked, this, &OptionalTab::slot_OnTempDirOpen_report);

			m_lineTmp_report = new QLineEdit(STRING_MANAGER->expertReportFilePath, this);
			m_lineTmp_report->setReadOnly(true);
			m_lineTmp_report->setStyleSheet(STYLE_MANAGER->editBoxTab);

			QPushButton* openBtn = new QPushButton(this);
			openBtn->setText("Open");
			openBtn->setStyleSheet(STYLE_MANAGER->buttonBehind);
			openBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
			connect(openBtn, &QPushButton::released, this, &OptionalTab::slot_OnExpertReportFileOpen);

			addWidget(pLabelTemDir_report, nRow++, 1);
			addWidget(m_btnTmp_report, nRow, 1);
			addWidget(m_lineTmp_report, nRow, 5);
			addWidget(openBtn, nRow++, 1);
		}

		// auto save 체크 박스 및 interval 입력 창 추가.
		createFrameLine(nRow++);

		QLabel* pLabelAutoSave = new QLabel(this);
		pLabelAutoSave->setText("Auto Save");

		m_chkAutoSave = new QCheckBox("Auto Save (Supported mip, mipd files)", this);
		m_chkAutoSave->setCheckable(true);
		m_chkAutoSave->setChecked(APP_MNG->getAutoSaveFlag());
		m_chkAutoSave->setStyleSheet(STYLE_MANAGER->m_Checkbox);

		connect(m_chkAutoSave, &QCheckBox::clicked, this, &OptionalTab::slot_OnChkAutoSave);

		addWidget(pLabelAutoSave, nRow++, 1);
		addWidget(m_chkAutoSave, nRow++, 1);

		m_labelInterval = new QLabel("Interval(minute)", this);

		m_textAutoSave = new QLineEdit(this);
		m_textAutoSave->setValidator(new QIntValidator(1, 999999999));
		m_textAutoSave->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textAutoSave->setText(QString::number(APP_MNG->getAutoSaveInterval()));
		m_textAutoSave->setObjectName("EditAutoSaveInterval");
		m_textAutoSave->installEventFilter(this);
		m_textAutoSave->setMouseTracking(true);

		connect(m_textAutoSave, &QLineEdit::returnPressed, this, &OptionalTab::slot_OnAutoSaveEditFinish);
		connect(m_textAutoSave, &QLineEdit::editingFinished, this, &OptionalTab::slot_OnAutoSaveEditFinish);

		addWidget(m_labelInterval, nRow, 1);
		addWidget(m_textAutoSave, nRow, 2);

		slot_OnChkAutoSave();

		/* URL 설정 입력 Widget */
		nRow++;
		createFrameLine(nRow++);

		if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_VisualPrinting_VisualPrintingservice))
		{
			QLabel* pLabelVisualPrint = new QLabel(this);
			pLabelVisualPrint->setText(QString("Visual Print"));

			addWidget(pLabelVisualPrint, nRow++, 1);

			VisualPrintConfig* pVPConfig = CONFIG_MANAGER->getConfig_VisualPrint();

			QLabel* pLabelVisualPrintURL = new QLabel(this);
			pLabelVisualPrintURL->setText(QString("URL"));
			pLabelVisualPrintURL->setFixedWidth(100);

			m_textVisualPrintURL = new QLineEdit(this);
			m_textVisualPrintURL->setText(pVPConfig->getUrl());
			m_textVisualPrintURL->setObjectName("EditVisualPrintURL");
			m_textVisualPrintURL->setStyleSheet(STYLE_MANAGER->editBoxTab);
			m_textVisualPrintURL->installEventFilter(this);
			m_textVisualPrintURL->setMouseTracking(true);

			connect(m_textVisualPrintURL, &QLineEdit::editingFinished, this, &OptionalTab::slot_OnVisualPrintURLEditFinish);

			addWidget(pLabelVisualPrintURL, nRow, 1);
			addWidget(m_textVisualPrintURL, nRow++, 1);

			QLabel* pLabelVisualPrintPort = new QLabel(this);
			pLabelVisualPrintPort->setText(QString("Port"));
			pLabelVisualPrintPort->setFixedWidth(100);

			m_textVisualPrintPort = new QLineEdit(this);
			m_textVisualPrintPort->setText(QString::number(pVPConfig->getPort()));
			m_textVisualPrintPort->setObjectName("EditVisualPrintPort");
			m_textVisualPrintPort->setStyleSheet(STYLE_MANAGER->editBoxTab);
			m_textVisualPrintPort->installEventFilter(this);
			m_textVisualPrintPort->setMouseTracking(true);

			m_textVisualPrintPort->setValidator(new QIntValidator(this));

			connect(m_textVisualPrintPort, &QLineEdit::editingFinished, this, &OptionalTab::slot_OnVisualPrintPortEditFinish);

			addWidget(pLabelVisualPrintPort, nRow, 1);
			addWidget(m_textVisualPrintPort, nRow++, 1);

			createFrameLine(nRow++);
		}
	}

	//Render 
	QLabel* pLabelRender = new QLabel(this);
	pLabelRender->setText(QString("Rendering"));
	//pLabelRender->setFixedWidth(100);
	addWidget(pLabelRender, nRow++, 1);

	m_chkCubeRender = new QCheckBox("Visible Coordinate System", this);
	m_chkCubeRender->setCheckable(true);
	m_chkCubeRender->setChecked(APP_MNG->getAutoSaveFlag());
	m_chkCubeRender->setStyleSheet(STYLE_MANAGER->m_Checkbox);

	connect(m_chkCubeRender, &QCheckBox::clicked, this, &OptionalTab::slot_OnChkCubeRender);

	addWidget(m_chkCubeRender, nRow++, 1);

	createFrameLine(nRow++);

	// Start brush settings
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut))
	{
		QLabel* pLabelBrush = new QLabel(this);
		pLabelBrush->setText(QString("Brush(Segmentation) Settings"));
		addWidget(pLabelBrush, nRow++, 1);

		QLabel* pLabelBrushThickness = new QLabel(this);
		pLabelBrushThickness->setText(QString("Outline Thickness : "));
		addWidget(pLabelBrushThickness, nRow, 1);

		m_spinBrushThickness = new QSpinBox(this);
		m_spinBrushThickness->setRange(1, 10);
		m_spinBrushThickness->setValue(WIN_MANAGER->getBrushThickness());
		m_spinBrushThickness->setStyleSheet(STYLE_MANAGER->spinbox);
		m_spinBrushThickness->setFixedWidth(75);

		connect(m_spinBrushThickness, QOverload<int>::of(&QSpinBox::valueChanged), this, &OptionalTab::slot_OnSpinBoxBrushThicknessChanged);

		addWidget(m_spinBrushThickness, nRow++, 1);

		QLabel* pLabelBrushOpacity = new QLabel(this);
		pLabelBrushOpacity->setText(QString("Opacity : "));
		addWidget(pLabelBrushOpacity, nRow, 1);

		m_spinBrushOpacity = new QSpinBox(this);
		m_spinBrushOpacity->setRange(0, 255);
		m_spinBrushOpacity->setValue(WIN_MANAGER->getBrushOpacity());
		m_spinBrushOpacity->setStyleSheet(STYLE_MANAGER->spinbox);
		m_spinBrushOpacity->setFixedWidth(75);

		connect(m_spinBrushOpacity, QOverload<int>::of(&QSpinBox::valueChanged), this, &OptionalTab::slot_OnSpinBoxBrushOpacityChanged);

		addWidget(m_spinBrushOpacity, nRow++, 1);
		// Finish brush settings

		// 	emptyBox0 = new QWidget(this);
		// 	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		// 
		// 	addWidget(emptyBox0, nRow++, 0);

		createFrameLine(nRow++);
	}
	//////////////////////////

	// Export 3D Mesh Method
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileExport))
	{
		QLabel* pLabelExportMesh = new QLabel(this);
		pLabelExportMesh->setText(QString("Export 3D Mesh Method"));
		addWidget(pLabelExportMesh, nRow++, 1);

		m_export3DMeshFitSize = new QCheckBox(this);
		m_export3DMeshFitSize->setText(QString("Fit Volume Size"));
		m_export3DMeshFitSize->setChecked(true);
		m_export3DMeshFitSize->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		addWidget(m_export3DMeshFitSize, nRow, 1);

		connect(m_export3DMeshFitSize, &QCheckBox::clicked, this, &OptionalTab::slot_OnExport3DMeshFitSizeClicked);

		m_export3DMeshMarchingCube = new QCheckBox(this);
		m_export3DMeshMarchingCube->setText(QString("MarchingCube"));
		m_export3DMeshMarchingCube->setChecked(false);
		m_export3DMeshMarchingCube->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		addWidget(m_export3DMeshMarchingCube, nRow++, 1);

		connect(m_export3DMeshMarchingCube, &QCheckBox::clicked, this, &OptionalTab::slot_OnExport3DMeshMarchingCubeClicked);
	}

	QString title = "Option";
	setTitle(title);

	setOpenWidget(false);
}

void OptionalTab::slot_OnUndoEditFinish()
{
	int CurVal = WIN_MANAGER->getUndoLimit();

	WIN_MANAGER->setMoveFocus(true);

	QString strVal = m_textLimit->text();
	int val = strVal.toInt();

	/*if (val < WIN_MANAGER->m_undoBasic)
	{
	val = WIN_MANAGER->m_undoBasic;
	}
	else*/ if (val > 100)
	{
		val = 100;
	}
	else if (val < 1)
	{
		val = 1;
	}

	if (QString::number(val) != strVal)
	{
		m_textLimit->blockSignals(true);
		m_textLimit->setText(QString::number(val));
		m_textLimit->blockSignals(false);
	}

	// 201103 허 건 대리
	// Undo Redo 횟수 갱신 위한 스택 재생성
	if (CurVal != val)
	{
		QMessageBox::warning(this, "Undo/Redo", "Undo/Redo Stack memory is initialized.");

		WIN_MANAGER->setUndoLimit(val);

		WIN_MANAGER->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_undoLimit, strVal);

		auto p_win_main = WIN_MANAGER->mainWindow;

		ACTION_MANAGER->UndoStack_clear();
		ACTION_MANAGER->setUndoStack(new QUndoStack(p_win_main));

		p_win_main->CreateUndoRedoAction();
	}

	// 201103 허 건 대리
	// setUndoLimit만 할 경우, limit 적용 안됨..
	{
		//auto undo_stack = ACTION_MANAGER->getUndoStack();

		//if (undo_stack)
		//{
		//	undo_stack->setUndoLimit(val);
		//}

		//auto undo_stack_visualPrint = ACTION_MANAGER->getUndoStack_VisualPrint();

		//if (undo_stack_visualPrint)
		//{
		//	undo_stack_visualPrint->setUndoLimit(val);
		//}
	}

	// 201103 허 건
	// 기존 프로그램 종료 소스코드 주석처리
	//bool bExec = true;
	//if (WIN_MANAGER->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_undoLimit, strVal))
	//{
	//	if (strVal == m_textLimit->text())
	//		bExec = false;
	//}
	//else
	//	strVal = QString::number(WIN_MANAGER->getUndoLimit());

	//if (CurVal != val)
	//	CurVal = val;
	//else
	//	bExec = false;

	//if (bExec)
	//{
	//	QMessageBox dlg(this);
	//	QPushButton *btnYes, *btnNo;
	//	dlg.setWindowTitle("Re/Undo limit");
	//	dlg.setText("The changed value will be applied after the program is rerun. Do you want to replay now?");
	//	btnYes = dlg.addButton(QMessageBox::Yes);
	//	btnNo = dlg.addButton(QMessageBox::No);
	//	dlg.addButton(QMessageBox::Cancel);

	//	if (dlg.exec())
	//	{
	//		if (btnYes == dlg.clickedButton()) //작업 중 내용 저장하고, 재실행
	//		{
	//			WIN_MANAGER->setUndoLimit(m_textLimit->text().toInt());
	//			if (!WIN_MANAGER->getSaveState())
	//				WIN_MANAGER->mainWindow->ExistFileSave(QString(), 6);
	//			else
	//				WIN_MANAGER->mainWindow->RebootProgram();
	//		}
	//		else if (btnNo == dlg.clickedButton()) //값만 적용
	//		{
	//			WIN_MANAGER->setUndoLimit(m_textLimit->text().toInt());
	//		}
	//		else //이전 값과 동일하게 수정
	//		{
	//			m_textLimit->blockSignals(true);
	//			m_textLimit->setText(strVal);
	//			m_textLimit->blockSignals(false);
	//		}
	//	}
	//}
}

void OptionalTab::slot_OnGammaEditFinish()
{
	WIN_MANAGER->setMoveFocus(true);
	float fGamma = WIN_MANAGER->getGamma();

	if (fGamma != m_textGamma->text().toFloat())
	{
		fGamma = m_textGamma->text().toFloat();
		InAreaGamma(fGamma);
		m_textGamma->setText(QString::number(fGamma));
		m_slideGamma->setValue(fGamma * 10);
		WIN_MANAGER->setGamma(fGamma);

		WIN_MANAGER->forceUpdate2DViewData();
		WIN_MANAGER->renderLater_GridView();
	}
}

void OptionalTab::slot_OnAutoSaveEditFinish()
{
	WIN_MANAGER->setMoveFocus(true);
	APP_MNG->updateAutoSaveInteval(m_textAutoSave->text().toInt());
}

void OptionalTab::slot_OnVisualPrintURLEditFinish()
{
	QString url = m_textVisualPrintURL->text();
	int port = m_textVisualPrintPort->text().toInt();
	CONFIG_MANAGER->setConfig_VisualPrint(url, port);
}

void OptionalTab::slot_OnVisualPrintPortEditFinish()
{
	QString url = m_textVisualPrintURL->text();
	int port = m_textVisualPrintPort->text().toInt();
	CONFIG_MANAGER->setConfig_VisualPrint(url, port);
}

void OptionalTab::slot_OnGammaChanged(int pos)
{
	WIN_MANAGER->setMoveFocus(true);
	if (m_slideGamma)
	{
		m_textGamma->setText(QString::number(pos * 0.1f));
		WIN_MANAGER->setGamma(m_textGamma->text().toFloat());

		WIN_MANAGER->forceUpdate2DViewData();
		WIN_MANAGER->renderLater_GridView();
	}
}


void OptionalTab::slot_OnChkGamma()
{
	WIN_MANAGER->setMoveFocus(true);
	bool res = m_chkGamma->isChecked();

	if (!res)
	{
		float fGamma = 1.0f;

		m_textGamma->setText(QString::number(fGamma));
		m_slideGamma->setValue(fGamma * 10);
		WIN_MANAGER->setGamma(fGamma);

		if (DATA_CONTEXT->volume_data.isValidate())
		{
			WIN_MANAGER->forceUpdate2DViewData();
			WIN_MANAGER->renderLater_GridView();
		}
	}

	m_textGamma->setEnabled(res);
	m_slideGamma->setEnabled(res);

	m_labelGamma->setHidden(!res);
	m_textGamma->setHidden(!res);
	m_slideGamma->setHidden(!res);
	m_btnGamma->setHidden(!res);

	updateHeight();
	updateGeometry();

	WIN_MANAGER->setGammaMode(res, false, false);

}

void OptionalTab::slot_OnChkAutoSave()
{
	WIN_MANAGER->setMoveFocus(true);
	bool res = m_chkAutoSave->isChecked();
	APP_MNG->updateAutoSaveFlag(res);

	m_textAutoSave->setEnabled(res);

	m_labelInterval->setHidden(!res);
	m_textAutoSave->setHidden(!res);

	updateHeight();
	updateGeometry();

	//	WIN_MANAGER->setGammaMode(res, false, false);
}

void OptionalTab::slot_OnChkCubeRender()
{
	bool res = m_chkCubeRender->isChecked();
	WIN_MANAGER->bVisibleCoordinate = res;

	if (WIN_MANAGER->mainSegmentWidget != nullptr)
		WIN_MANAGER->mainSegmentWidget->update();

	if (WIN_MANAGER->mainMeshWidget != nullptr)
	{
		MEVolumeView* view = WIN_MANAGER->mainMeshWidget->getMainView();
		if (view != nullptr)
			view->update();
	}

	if (WIN_MANAGER->mainAnalWidget != nullptr)
		WIN_MANAGER->mainAnalWidget->update();
}

void OptionalTab::slot_OnSpinBoxBrushThicknessChanged(int value)
{
	WIN_MANAGER->setBrushThickness(value);
	WIN_MANAGER->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_BrushThickness, QString::number(value));
}

void OptionalTab::slot_OnSpinBoxBrushOpacityChanged(int value)
{
	WIN_MANAGER->setBrushOpacity(value);
	WIN_MANAGER->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_BrushOpacity, QString::number(value));
}

void OptionalTab::slot_OnExport3DMeshFitSizeClicked()
{
	if (m_export3DMeshFitSize)	m_export3DMeshFitSize->setChecked(true);
	if (m_export3DMeshMarchingCube) m_export3DMeshMarchingCube->setChecked(false);

	WIN_MANAGER->setExport3DMeshMethod(EXPORT_3D_SURFACE_MESH_METHOD::FIT_VOXEL_SIZE);
}

void OptionalTab::slot_OnExport3DMeshMarchingCubeClicked()
{
	if (m_export3DMeshMarchingCube) m_export3DMeshMarchingCube->setChecked(true);
	if (m_export3DMeshFitSize) m_export3DMeshFitSize->setChecked(false);

	WIN_MANAGER->setExport3DMeshMethod(EXPORT_3D_SURFACE_MESH_METHOD::CONTOURE_FILTER);
}

void OptionalTab::slot_OnTempDirOpen()
{
	WIN_MANAGER->setMoveFocus(true);
	QFileDialog dlg(this);

	dlg.setFileMode(QFileDialog::DirectoryOnly);

	QString fileName = dlg.getExistingDirectory(this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN),
		STRING_MANAGER->cacheFilePath); // Options options = 0)

	if (!fileName.isEmpty() && (STRING_MANAGER->cacheFilePath != fileName))
	{
		QString strReportFilePath = STRING_MANAGER->expertReportFilePath;
		QString strReportFolderPath = strReportFilePath.left(strReportFilePath.lastIndexOf("/"));
		if (!fileName.compare(strReportFolderPath, Qt::CaseInsensitive))
		{
			QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), "It is the same path as the report path.");
			return;
		}

		QFileInfo info(fileName);

		if (info.isDir())
		{
			WIN_MANAGER->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_cachePath, fileName);

			QDir dir(STRING_MANAGER->cacheFilePath);
			QStringList list = dir.entryList(QDir::Files, QDir::Name);
			bool res = true;
			for (int i = 0; i < list.size(); i++)
			{
				QString fStr = list.at(i);
				QString oldName = STRING_MANAGER->cacheFilePath + "/" + fStr;
				QString newName = fileName + "/" + fStr;

				res = QFile::copy(oldName, newName);

				if (!res)
					break;
			}

			if (res)
			{
				dir.removeRecursively();

				dir.mkdir(STRING_MANAGER->cacheFilePath);

				STRING_MANAGER->cacheFilePath = fileName;
				STRING_MANAGER->cacheFileOriginVolume = fileName + "/core.data";

				m_lineTmp->setText(STRING_MANAGER->cacheFilePath);
			}
			else
			{
				dir.setPath(fileName);
				dir.setCurrent(fileName);
				dir.removeRecursively();
			}
		}

	}
}

void OptionalTab::slot_OnTempDirOpen_report()
{
	WIN_MANAGER->setMoveFocus(true);
#if 0
	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::DirectoryOnly);
	QString fileName = dlg.getExistingDirectory(this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN),
		STRING_MANAGER->expertReportFilePath); // Options options = 0)
#else
	QFileInfo fileinfo(STRING_MANAGER->expertReportFilePath);
	QString fileName = ExportFileDialog(
		this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_SAVE),
		fileinfo.fileName(),
		fileinfo.dir().path(),
		tr("Files (*.csv)")
	);
#endif

	if (!fileName.isEmpty() && (STRING_MANAGER->expertReportFilePath != fileName))
	{
		QString strReportFolderPath = fileName.left(fileName.lastIndexOf("/"));
		if (!strReportFolderPath.compare(STRING_MANAGER->cacheFilePath, Qt::CaseInsensitive))
		{
			QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), "It is the same path as the cache path.");
			return;
		}

		WIN_MANAGER->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_expertReportPath, fileName);
		STRING_MANAGER->expertReportFilePath = fileName;
		m_lineTmp_report->setText(fileName);
	}
}

void OptionalTab::slot_OnExpertReportFileOpen()
{
	QDesktopServices::openUrl(QUrl(STRING_MANAGER->expertReportFilePath));

	QFileInfo info(STRING_MANAGER->expertReportFilePath);
	QDesktopServices::openUrl(QUrl("file:///" + info.path(), QUrl::TolerantMode));
}

void OptionalTab::slot_OnChangeMode(int type)
{
	WIN_MANAGER->setMoveFocus(true);
	if (0 == type) //default
	{
		m_FuncGroup->button(1)->setChecked(false);
		WIN_MANAGER->setWheelZoom(true);
	}
	else
	{
		m_FuncGroup->button(0)->setChecked(false);
		WIN_MANAGER->setWheelZoom(false);
	}
}

void OptionalTab::setGammaMode(bool val)
{
	float fGamma = 1.0f;
	if (val) //enable
		fGamma = WIN_MANAGER->getGamma();

	m_chkGamma->setChecked(val);

	m_textGamma->setText(QString::number(fGamma));
	m_slideGamma->setValue(fGamma * 10);

	m_textGamma->setEnabled(val);
	m_slideGamma->setEnabled(val);

	WIN_MANAGER->renderLater_GridView();
}

bool OptionalTab::isGammaMode()
{
	if (m_chkGamma)
		return m_chkGamma->isChecked();

	return false;
}

void OptionalTab::slot_OnReset()
{
	WIN_MANAGER->setMoveFocus(true);
	float fGamma;

	if (m_chkGamma->isChecked())
		fGamma = 2.5f;
	else
		fGamma = 1.0f;

	m_textGamma->setText(QString::number(fGamma));
	m_slideGamma->setValue(fGamma * 10);
	WIN_MANAGER->setGamma(fGamma);

	if (DATA_CONTEXT->volume_data.isValidate())
	{
		WIN_MANAGER->forceUpdate2DViewData();
		WIN_MANAGER->renderLater_GridView();
	}
}

bool OptionalTab::eventFilter(QObject* watched, QEvent* e)
{
	if (watched == NULL) return false;

	if (watched->objectName().contains("Slider") || watched->objectName().contains("Combo"))
	{
		if (e->type() == QEvent::Wheel)
			return true;
	}

	if (watched->objectName().contains("ButtonTemporary"))
	{
		static QIcon openHover = RESOURCE_MANAGER->getIcon(ICON_FILE_OPEN_HOVER);
		static QIcon openLeave = RESOURCE_MANAGER->getIcon(ICON_FILE_NON_OPEN);
		static QIcon openPress = RESOURCE_MANAGER->getIcon(ICON_FILE_OPEN);

		if (!(e->type() == QEvent::MouseMove ||
			e->type() == QEvent::MouseButtonPress ||
			e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease))
			return QWidget::eventFilter(watched, e);

		if (m_btnTmp == watched)
		{
			if (e->type() == QEvent::MouseMove)
				m_btnTmp->setIcon(openHover);
			else if (e->type() == QEvent::MouseButtonPress)
				m_btnTmp->setIcon(openPress);
			else if (e->type() == QEvent::HoverLeave ||
				e->type() == QEvent::MouseButtonRelease)
				m_btnTmp->setIcon(openLeave);
		}
		if (m_btnTmp_report == watched)
		{
			if (e->type() == QEvent::MouseMove)
				m_btnTmp_report->setIcon(openHover);
			else if (e->type() == QEvent::MouseButtonPress)
				m_btnTmp_report->setIcon(openPress);
			else if (e->type() == QEvent::HoverLeave ||
				e->type() == QEvent::MouseButtonRelease)
				m_btnTmp_report->setIcon(openLeave);
		}
	}

	if (watched->objectName().contains("Edit"))
	{
		if (e->type() == QEvent::FocusIn)
			WIN_MANAGER->setMoveFocus(false);
	}

	return QWidget::eventFilter(watched, e);
}

void OptionalTab::InAreaGamma(float& fGamma)
{
	fGamma = mip::math::Max(fGamma, 10.0f) == fGamma ? 10.0f :
		mip::math::Min(fGamma, 0.1f) == fGamma ? 0.1f : fGamma;
}

