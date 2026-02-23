#include "stdafx.h"
#include "ThreSholdTab.h"
#include "WindowManager.h"
#include "ResourceManager.h"
#include "styleManager.h"
#include "StringManager.h"
#include "ProductManager.h"
#include "LicenseManager.h"
#include "Main/MainSegmentWidget.h"
#include "ShortcutManager.h"
#include "CustomHistogram.h"
#include "RangeWidget.h"
#include "MedipQT.h"
#include "Tabwindow.h"

#include "DataContext.h"

ThreSholdTab::ThreSholdTab(ProductManager* pProductManager, QWidget* parent /*= NULL*/)
	: CollapseWidget(QString(), parent),
	m_pProductManager(pProductManager)
{
	m_pProductFactory = m_pProductManager->getFactory();

	int nRow = 0;
	m_mode = RG_MODE_HU_RANGE;
	m_connectivity = RG_CONNECTIVITY_26;

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Threshold_Thresholdbasic))
	{
		m_ThreColor = QColor(Qt::red);
		//TODO Histogram widget add

		QLabel* labelType = new QLabel(this);
		labelType->setText("Type");
		m_ThreType = new QComboBox(this);
		m_ThreType->setObjectName("ComboType");
		m_ThreType->addItem(STRING_MANAGER->getString(STR_DEFAULT));
		m_ThreType->addItem(STRING_MANAGER->getString(STR_SOFT_TISSUE));
		m_ThreType->addItem(STRING_MANAGER->getString(STR_AIR));
		m_ThreType->addItem(STRING_MANAGER->getString(STR_FAT));
		m_ThreType->addItem(STRING_MANAGER->getString(STR_BONE));
		m_ThreType->addItem(STRING_MANAGER->getString(STR_FLUIDS));

		m_ThreType->setStyleSheet(STYLE_MANAGER->comboBoxTab);
		m_ThreType->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
		m_ThreType->installEventFilter(this);
		connect(m_ThreType, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnComboChanged(int)));


		addWidget(labelType, nRow, 1, QMargins(0, 5, 0, 0));
		addWidget(m_ThreType, nRow, 5);
		nRow++;
		//addWidget(emptyBox, nRow++, 1);

		QLabel* labelStart = new QLabel(this);
		labelStart->setText("Min");
		m_startHU = new QLineEdit(this);
		m_startHU->setText("0");
		m_startHU->setObjectName("EditStart");
		m_startHU->setValidator(new QIntValidator(this));
		m_startHU->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_startHU->installEventFilter(this);
		m_startHU->setMouseTracking(true);
		connect(m_startHU, &QLineEdit::editingFinished, this, &ThreSholdTab::slot_OnStartTextChanged);
		connect(m_startHU, &QLineEdit::returnPressed, this, &ThreSholdTab::slot_OnStartTextChanged);
		QLabel* labelEnd = new QLabel(this);
		labelEnd->setText("Max");
		m_endHU = new QLineEdit(this);
		m_endHU->setText("0");
		m_endHU->setObjectName("EditEnd");
		m_endHU->setValidator(new QIntValidator(this));
		m_endHU->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_endHU->installEventFilter(this);
		m_endHU->setMouseTracking(true);
		connect(m_endHU, &QLineEdit::editingFinished, this, &ThreSholdTab::slot_OnEndTextChanged);
		connect(m_endHU, &QLineEdit::returnPressed, this, &ThreSholdTab::slot_OnEndTextChanged);
		addWidget(labelStart, nRow, 1);
		addWidget(m_startHU, nRow, 2);
		addWidget(labelEnd, nRow, 1);
		addWidget(m_endHU, nRow++, 2);

		/////////////// threshold
		createFrameLine(nRow++);

		QLabel* pLabelThreshold = new QLabel(this);
		pLabelThreshold->setText(QString("Threshold"));

		addWidget(pLabelThreshold, nRow++, 1);

		m_bWithin = false;
		//QCheckBox *checkWithin = new QCheckBox(this);
		QCheckBox* checkWithin = m_pProductFactory->createWidget<QCheckBox>(MFL_Common_Segmentation_Threshold_WithinSelectedLayer, this);
		if (checkWithin)
		{
			checkWithin->setText("Within Selected Layer");
			checkWithin->setCheckable(true);
			checkWithin->setChecked(false);
			checkWithin->setStyleSheet(STYLE_MANAGER->m_Checkbox);

			connect(checkWithin, &QCheckBox::clicked, this, &ThreSholdTab::slot_OnWithinCheck);

			addWidget(checkWithin, nRow, 1);
		}


		m_bBrushin = false;
		m_checkBrushin = new QCheckBox(this);
		m_checkBrushin->setText("Within Brush");
		m_checkBrushin->setCheckable(true);
		m_checkBrushin->setChecked(false);
		m_checkBrushin->setStyleSheet(STYLE_MANAGER->m_Checkbox);

		if (SHORTCUT_MANAGER->Action_Threshold_WithinBrush())
		{
			connect(SHORTCUT_MANAGER->Action_Threshold_WithinBrush(), &QAction::triggered, m_checkBrushin, &QCheckBox::click);
		}
		connect(m_checkBrushin, &QCheckBox::clicked, this, &ThreSholdTab::slot_OnBrushinCheck);

		addWidget(m_checkBrushin, nRow, 1);
		++nRow;

		m_bThrePreview = new QCheckBox(this);
		m_bThrePreview->setText("Show Preview");
		m_bThrePreview->setCheckable(true);
		m_bThrePreview->setChecked(false);
		m_bThrePreview->setStyleSheet(STYLE_MANAGER->m_Checkbox);

		if (SHORTCUT_MANAGER->Action_Threshold_Preview())
		{
			connect(SHORTCUT_MANAGER->Action_Threshold_Preview(), &QAction::triggered, m_bThrePreview, &QCheckBox::click);
		}
		connect(m_bThrePreview, &QCheckBox::clicked, this, &ThreSholdTab::slot_OnCheckPreview);

		m_ThrePreImg = new QWidget(this);
		m_ThrePreImg->setStyleSheet(QString("background-color: rgba(%1,%2,%3,255); border-radius:10px;")
			.arg(m_ThreColor.red()).arg(m_ThreColor.green()).arg(m_ThreColor.blue()));

		QPushButton* btnColor = new QPushButton(this);
		btnColor->setText("Preview Color");
		btnColor->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(btnColor, &QPushButton::clicked, this, &ThreSholdTab::slot_OnColorChanged);

		QPushButton* btnReset = new QPushButton(this);
		btnReset->setText(STRING_MANAGER->getString(STR_RESET));
		btnReset->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(btnReset, &QPushButton::clicked, this, &ThreSholdTab::slot_OnResetThreshold);

		QPushButton* btnApply = new QPushButton(this);
		btnApply->setText(STRING_MANAGER->getString(STR_APPLY));
		btnApply->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(btnApply, &QPushButton::clicked, this, &ThreSholdTab::slot_OnApplyThreshold);

		addWidget(m_bThrePreview, nRow, 4, QMargins(0, 5, 0, 5));
		addWidget(m_ThrePreImg, nRow, 1);
		addWidget(btnColor, nRow, 4);
		addWidget(btnReset, nRow, 2);
		addWidget(btnApply, nRow++, 2);
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Threshold_Thresholdbasic)
		&& m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Regiongrowing_RegionGrowing))
	{
		createFrameLine(nRow++);
	}

	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Regiongrowing_RegionGrowing))
	{
		//////////////// region growing	

		QLabel* pLabelRegionGrowing = new QLabel(this);
		pLabelRegionGrowing->setText(QString("Region Growing"));

		m_mode = RG_MODE_HU_RANGE;
		m_connectivity = RG_CONNECTIVITY_26;

		QLabel* pLabelMode = new QLabel(this);
		pLabelMode->setText("Mode");

		m_grMode = new QButtonGroup(this);

		QRadioButton* radioHURangeMode = nullptr;
		int nGrModeSize = 1;
		if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Regiongrowing_ModeHURange))
		{
			radioHURangeMode = new QRadioButton(this);
			radioHURangeMode->setText("HU Range");
			radioHURangeMode->setCheckable(true);
			radioHURangeMode->setChecked(true);
			radioHURangeMode->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

			m_grMode->addButton(radioHURangeMode, 0);
		}
		else
		{
			nGrModeSize = 2;
		}

		QRadioButton* radioLayerMode = new QRadioButton(this);
		radioLayerMode->setText("Layer");
		radioLayerMode->setCheckable(true);
		radioLayerMode->setChecked(false);
		radioLayerMode->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_grMode->addButton(radioLayerMode, 1);

		connect(m_grMode, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnChangeMode(int)));

		QWidget* emptyBox0 = new QWidget(this);
		emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

		if (pLabelRegionGrowing) addWidget(pLabelRegionGrowing, nRow++, 1);
		if (pLabelMode) addWidget(pLabelMode, nRow, 1);
		if (radioHURangeMode) addWidget(radioHURangeMode, nRow, 1);
		if (radioLayerMode) addWidget(radioLayerMode, nRow, nGrModeSize);
		if (emptyBox0) addWidget(emptyBox0, nRow++, 1);

		////////////connectivity
		QLabel* pLabelConnectivity = new QLabel(this);
		pLabelConnectivity->setText("Connectivity");

		m_grConnect = new QButtonGroup(this);

		QRadioButton* rdoConnectivity_26 = new QRadioButton(this);
		rdoConnectivity_26->setText("26");
		rdoConnectivity_26->setCheckable(true);
		rdoConnectivity_26->setChecked(true);
		rdoConnectivity_26->setStyleSheet(STYLE_MANAGER->m_Radiobtn);
		m_grConnect->addButton(rdoConnectivity_26, 0);

		QRadioButton* rdoConnectivity_6 = nullptr;
		rdoConnectivity_6 = m_pProductFactory->createWidget<QRadioButton>(MFL_Common_Segmentation_Regiongrowing_6connectivity, this);
		if (rdoConnectivity_6)
		{
			rdoConnectivity_6->setText("6");
			rdoConnectivity_6->setCheckable(true);
			rdoConnectivity_6->setChecked(false);
			rdoConnectivity_6->setStyleSheet(STYLE_MANAGER->m_Radiobtn);
			m_grConnect->addButton(rdoConnectivity_6, 1);
		}

		connect(m_grConnect, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnChangeConnect(int)));

		emptyBox0 = new QWidget(this);
		emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

		addWidget(pLabelConnectivity, nRow, 1);
		addWidget(rdoConnectivity_26, nRow, 1);
		addWidget(rdoConnectivity_6, nRow, 1);
		addWidget(emptyBox0, nRow++, 1);

		// seed point
		QLabel* pLabelSeedPoints = new QLabel(this);
		pLabelSeedPoints->setText("Seed points");

		m_RGList = new QTreeWidget(this);
		m_RGList->setColumnCount(1);
		m_RGList->setContentsMargins(0, 0, 0, 5);
		m_RGList->setHeaderHidden(true);
		m_RGList->setStyleSheet(STYLE_MANAGER->treeBasicList);
		m_RGList->setRootIsDecorated(false);
		m_RGList->setMinimumWidth(20);
		m_RGList->setSelectionMode(QAbstractItemView::ExtendedSelection);
		m_RGList->installEventFilter(this);

		addWidget(pLabelSeedPoints, nRow++, 1, QMargins(0, 10, 0, 0));
		addWidget(m_RGList, nRow++, 1);

		QPushButton* btnResetRG = new QPushButton(this);
		btnResetRG->setText(STRING_MANAGER->getString(STR_RESET));
		btnResetRG->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(btnResetRG, &QPushButton::clicked, this, &ThreSholdTab::slot_OnResetRG);

		m_btnSelect = new QPushButton(this);
		m_btnSelect->setText(STRING_MANAGER->getString(STR_SELECT_SEED));
		m_btnSelect->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(m_btnSelect, &QPushButton::clicked, this, &ThreSholdTab::slot_OnSelectSeed);


		QPushButton* btnApplyRG = new QPushButton(this);
		btnApplyRG->setText(STRING_MANAGER->getString(STR_APPLY));
		btnApplyRG->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(btnApplyRG, &QPushButton::clicked, this, &ThreSholdTab::slot_OnApplyRG);

		addWidget(btnResetRG, nRow, 1);
		addWidget(m_btnSelect, nRow++, 1);
		addWidget(btnApplyRG, nRow++, 1);
	}

	QString title;
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Threshold_Thresholdbasic)
		&& m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Regiongrowing_RegionGrowing))
	{
		title = "Threshold && Region Growing";
	}
	else if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Threshold_Thresholdbasic))
	{
		title = "Threshold";
	}
	else if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Regiongrowing_RegionGrowing))
	{
		title = "Region Growing";
	}

	setTitle(title);

	setOpenWidget(false);
}

void ThreSholdTab::Init()
{
	setThreType(THRE_DEFAULT);

	//m_rgTab->Reset();
	slot_OnResetRG();
}

void ThreSholdTab::cancelSelectMode()
{
	// 	if (m_rgTab)
	// 		m_rgTab->cancelSelectMode();

	if (m_btnSelect)
		m_btnSelect->setText(STRING_MANAGER->getString(STR_SELECT_SEED));
}

void ThreSholdTab::slot_OnWithinCheck(bool checked)
{
	WIN_MANAGER->setMoveFocus(true);
	m_bWithin = checked;

	if (m_bWithin)
	{
		WIN_MANAGER->setThresholdWorkModeOn(THRESHOLD_WORK_WITHIN_SELECTED_LAYER);
	}
	else
	{
		WIN_MANAGER->setThresholdWorkModeOff(THRESHOLD_WORK_WITHIN_SELECTED_LAYER);
	}
	WIN_MANAGER->renderLater_All(false);
}

void ThreSholdTab::slot_OnBrushinCheck(bool checked)
{
	WIN_MANAGER->setMoveFocus(true);
	m_bBrushin = checked;

	if (m_bBrushin)
	{
		WIN_MANAGER->setThresholdWorkModeOn(THRESHOLD_WORK_WITHIN_BRUSH);
	}
	else
	{
		WIN_MANAGER->setThresholdWorkModeOff(THRESHOLD_WORK_WITHIN_BRUSH);
	}
	WIN_MANAGER->renderLater_All(false);
}

// void ThreSholdTab::OnRangeChanged(int val, bool isStart)
// {
// 	if (isStart)
// 	{
// 		m_startHU->setText(QString::number(val));
// 		WIN_MANAGER->setThreLower(m_startHU->text().toInt());
// 	}
// 	else
// 	{
// 		m_endHU->setText(QString::number(val));
// 		WIN_MANAGER->setThreUpper(m_endHU->text().toInt());
// 	}
// }

void ThreSholdTab::slot_OnStartTextChanged()
{
	int Val = m_startHU->text().toInt();

	if (getEndHU() < Val)
		Val = getEndHU();

	if (!(DATA_CONTEXT->volume_data.getHuMin() <= Val &&
		DATA_CONTEXT->volume_data.getHuMax() >= Val))
		Val = DATA_CONTEXT->volume_data.getHuMin();

	m_startHU->setText(QString::number(Val));

	WIN_MANAGER->setThreLower(Val);
	WIN_MANAGER->setMoveFocus(true);

	if (WIN_MANAGER->mainSegmentWidget)
	{
		if (WIN_MANAGER->mainSegmentWidget->getHistogramView())
			WIN_MANAGER->mainSegmentWidget->getHistogramView()->HuRangeSlider->setFirstValue(Val);
	}

	if (WIN_MANAGER->bThrePreview)
		WIN_MANAGER->renderLater_GridView(false);
}

void ThreSholdTab::ApplyRegionGrowing()
{
	//m_rgTab->ApplyRegionGrowing();
	slot_OnApplyRG();
}

void	ThreSholdTab::setPreviewThreshold(bool bCheck)
{
	m_bThrePreview->setChecked(bCheck);

	slot_OnCheckPreview(bCheck);
}

void ThreSholdTab::PreviewFinish()
{
	if (WIN_MANAGER->bThrePreview)
	{
		m_bThrePreview->setChecked(false);
		slot_OnCheckPreview(false);
	}
}

void ThreSholdTab::setRegionGrowing()
{
	WIN_MANAGER->setMoveFocus(false);

	int startHU, endHU;

	startHU = getStartHU();
	endHU = getEndHU();

	if (startHU > WIN_MANAGER->regionGrowingHu)
	{
		setStartHU(startHU - (startHU - WIN_MANAGER->regionGrowingHu));
	}

	else if (endHU < WIN_MANAGER->regionGrowingHu)
	{
		setEndHU(WIN_MANAGER->regionGrowingHu);
	}

	//m_rgTab->UpdateList();
	UpdateRGList();

	m_startHU->setFocus();
}

void ThreSholdTab::setThreType(ThreType index)
{
	//	m_ThreType->setCurrentIndex(index);
	slot_OnComboChanged(index);
}

int ThreSholdTab::getStartHU()
{

	if (m_startHU)
		return m_startHU->text().toInt();
	return 0;
}

int ThreSholdTab::getEndHU()
{
	if (m_endHU)
		return m_endHU->text().toInt();
	return 0;
}

void ThreSholdTab::setStartHU(int val)
{
	if (!(DATA_CONTEXT->volume_data.getHuMin() <= val &&
		DATA_CONTEXT->volume_data.getHuMax() >= val))
		val = DATA_CONTEXT->volume_data.getHuMin();

	if (m_startHU)
		m_startHU->setText(QString::number(val));

	if (getEndHU() < val)
		setEndHU(val);

	WIN_MANAGER->setThreLower(val);

	if (WIN_MANAGER->bThrePreview)
		WIN_MANAGER->renderLater_GridView(false);
}

void ThreSholdTab::setEndHU(int val)
{
	if (!(DATA_CONTEXT->volume_data.getHuMin() <= val &&
		DATA_CONTEXT->volume_data.getHuMax() >= val))
		val = DATA_CONTEXT->volume_data.getHuMax();

	if (m_endHU)
		m_endHU->setText(QString::number(val));

	if (getStartHU() > val)
		setStartHU(val);

	WIN_MANAGER->setThreUpper(val);

	if (WIN_MANAGER->bThrePreview)
		WIN_MANAGER->renderLater_GridView(false);
}

void ThreSholdTab::setRegionGrowingMode(REGION_GROWING_MODE mode)
{
	/* 버튼이 1개일 경우 Region Growing Mode=Layer로 적용 */
	if (m_grMode->buttons().size() == 1)
	{
		mode = RG_MODE_LAYER;
	}

	m_mode = mode;
	if (mode == RG_MODE_HU_RANGE)
	{
		if (m_grMode->button(0))
		{
			m_grMode->button(0)->setChecked(true);
		}
		if (m_grMode->button(1))
		{
			m_grMode->button(1)->setChecked(false);
		}
	}
	else if (mode == RG_MODE_LAYER)
	{
		if (m_grMode->button(0))
		{
			m_grMode->button(0)->setChecked(false);
		}
		if (m_grMode->button(1))
		{
			m_grMode->button(1)->setChecked(true);
		}
	}
}

void ThreSholdTab::setRegionGrowingConnectivity(REGION_GROWING_CONNECTIVITY connectivity)
{
	/* 버튼이 1개일 경우 Region Growing Connectivity=26 으로 적용 */
	if (m_grConnect->buttons().size() == 1)
	{
		connectivity = RG_CONNECTIVITY_26;
	}

	m_connectivity = connectivity;

	if (connectivity == RG_CONNECTIVITY_26)
	{
		if (m_grConnect->button(0))
		{
			m_grConnect->button(0)->setChecked(true);
		}
		if (m_grConnect->button(1))
		{
			m_grConnect->button(1)->setChecked(false);
		}

		WIN_MANAGER->setConfigValue(
			ELEMENT_FILE,
			STRING_MANAGER->config_RegionGrowingConnectivity,
			QString::number(connectivity));
	}
	else if (connectivity == RG_CONNECTIVITY_6)
	{
		if (m_grConnect->button(0))
		{
			m_grConnect->button(0)->setChecked(false);
		}
		if (m_grConnect->button(1))
		{
			m_grConnect->button(1)->setChecked(true);
		}

		WIN_MANAGER->setConfigValue(
			ELEMENT_FILE,
			STRING_MANAGER->config_RegionGrowingConnectivity,
			QString::number(connectivity));
	}
}

void ThreSholdTab::UpdateRGList()
{
	m_RGList->clear();
	QList<QTreeWidgetItem*> items;

	for (int i = 0; i < WIN_MANAGER->seedLocation.size(); i++)
	{
		QVector3D pos = WIN_MANAGER->seedLocation.at(i);
		QTreeWidgetItem* item = new QTreeWidgetItem(m_RGList);

		QString posText = WIN_MANAGER->ConvertText_VoxelPosToSliceImagePos(pos.x(), pos.y(), pos.z());
		int data = DATA_CONTEXT->volume_data.getData(pos.x(), pos.y(), pos.z());

		item->setText(0, QString("Seed point : (%1), value=%2").arg(posText).arg(data));

		items.append(item);
	}
	if (!items.isEmpty())
		m_RGList->insertTopLevelItems(0, items);
}

bool ThreSholdTab::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == NULL) return false;

	if (watched->objectName().contains("Slider") || watched->objectName().contains("Combo"))
	{
		if (event->type() == QEvent::Wheel)
			return true;
	}

	if (watched->objectName().contains("Edit"))
	{
		if (event->type() == QEvent::FocusIn)
			WIN_MANAGER->setMoveFocus(false);
	}

	if (event->type() == QEvent::KeyRelease)
	{
		QKeyEvent* e = (QKeyEvent*)event;
		if (e->key() == Qt::Key_Delete)
		{
			slot_OnDelRG();
		}

	}

	return QWidget::eventFilter(watched, event);
}

void ThreSholdTab::slot_OnEndTextChanged()
{
	int Val = m_endHU->text().toInt();

	if (getStartHU() > Val)
		Val = getStartHU();

	if (!(DATA_CONTEXT->volume_data.getHuMin() <= Val &&
		DATA_CONTEXT->volume_data.getHuMax() >= Val))
		Val = DATA_CONTEXT->volume_data.getHuMax();

	m_endHU->setText(QString::number(Val));

	WIN_MANAGER->setThreUpper(Val);
	WIN_MANAGER->setMoveFocus(true);

	if (WIN_MANAGER->mainSegmentWidget)
	{
		if (WIN_MANAGER->mainSegmentWidget->getHistogramView())
			WIN_MANAGER->mainSegmentWidget->getHistogramView()->HuRangeSlider->setSecondValue(Val);
	}


	if (WIN_MANAGER->bThrePreview)
		WIN_MANAGER->renderLater_GridView(false);
}

void ThreSholdTab::slot_OnCheckPreview(bool check)
{
	WIN_MANAGER->setMoveFocus(true);
	WIN_MANAGER->bThrePreview = check;

	qDebug() << check << "(check)";

	if (!(WIN_MANAGER->mainSegmentWidget->isVisibleHistogram() ^ !check))
		WIN_MANAGER->mainWindow->OnHistoClick();

	if (WIN_MANAGER->bThrePreview)
	{
		WIN_MANAGER->setThresholdWorkModeOn(THRESHOLD_WORK_SHOW_PREVIEW);
	}
	else
	{
		WIN_MANAGER->setThresholdWorkModeOff(THRESHOLD_WORK_SHOW_PREVIEW);
	}

	WIN_MANAGER->renderLater_GridView(false);
}

void ThreSholdTab::slot_OnColorChanged()
{
	WIN_MANAGER->setMoveFocus(true);
	QColorDialog dlg;

	dlg.setCurrentColor(m_ThreColor);

	if (dlg.exec() == QDialog::Accepted)
	{
		if (m_ThreColor != dlg.selectedColor())
		{
			m_ThreColor = dlg.selectedColor();
			m_ThrePreImg->setStyleSheet(QString("background-color: rgba(%1,%2,%3,255);")
				.arg(m_ThreColor.red()).arg(m_ThreColor.green()).arg(m_ThreColor.blue()));
			emit sig_currentColorChanged(m_ThreColor);
		}
	}

}

void ThreSholdTab::slot_OnApplyThreshold()
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	int nUpper, nLower;

	WIN_MANAGER->setThreLower(getStartHU());
	WIN_MANAGER->setThreUpper(getEndHU());

	nUpper = WIN_MANAGER->getThreUpper();
	nLower = WIN_MANAGER->getThreLower();

	//	if (nUpper == nLower)
	//	{
	//		QMessageBox::warning(NULL, "Threshold value", "Threshold start value and end value are the same.");
	//		return;
	//	}

	PreviewFinish();
#ifdef SELECT_HIDE
	WIN_MANAGER->setSeedShow(WIN_MANAGER->getSeedShow() | RT_SELECT);
#endif
	ACTION_MANAGER->action_ThresholdSelect(nUpper, nLower, m_bWithin,
		WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());


}

// void ThreSholdTab::OnRGApply()
// {
// 	int nFirst, nSecond;
// 
// 	if (DATA_CONTEXT->volume_data.isValidate() == false) return;
// 
// 	if (WIN_MANAGER->regionGrowingX < 0)
// 	{
// 		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN),
// 			STRING_MANAGER->getString(STR_SELECT_SEED_FIRST));
// 		return;
// 	}
// 
// 	int nUpper, nLower;
// 
// 	nUpper = WIN_MANAGER->getThreUpper();
// 	nLower = WIN_MANAGER->getThreLower();
// 
// 	if (nLower > WIN_MANAGER->regionGrowingHu)
// 	{
// 		setStartHU(nLower - (nLower - WIN_MANAGER->regionGrowingHu));
// 	}
// 
// 	else if (nUpper < WIN_MANAGER->regionGrowingHu)
// 	{
// 		setEndHU(WIN_MANAGER->regionGrowingHu);
// 	}
// 
// 	CustomHistogram * view = WIN_MANAGER->mainSegmentWidget->getHistogramView();
// 	if (view)
// 	{
// 		if (view->HuRangeSlider)
// 		{
// 			nUpper = WIN_MANAGER->getThreUpper();
// 			nLower = WIN_MANAGER->getThreLower();
// 
// 			view->HuRangeSlider->setFirstValue(nLower);
// 			view->HuRangeSlider->setSecondValue(nUpper);
// 		}
// 	}
// 
// 	PreviewFinish();
// 
// #ifdef SELECT_HIDE
// 	WIN_MANAGER->setSeedShow(WIN_MANAGER->getSeedShow() | RT_SELECT);
// #endif
// 	ACTION_MANAGER->action_RegionGrowing(WIN_MANAGER->regionGrowingX, WIN_MANAGER->regionGrowingY, WIN_MANAGER->regionGrowingZ,
// 		nUpper, nLower, m_bWithin, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
// 
// 
// }

void ThreSholdTab::slot_OnResetThreshold() //exclude color change
{
	WIN_MANAGER->setMoveFocus(true);

	if (m_bThrePreview)
	{
		slot_OnComboChanged(m_ThreType->currentIndex());

		//	WIN_MANAGER->setThreLower(getStartHU());
		//	WIN_MANAGER->setThreUpper(getEndHU());

		m_bThrePreview->setChecked(false);
		slot_OnCheckPreview(false);

	}
}

void ThreSholdTab::slot_OnApplyRG()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if ((WIN_MANAGER->seedLocation.size() < 1)
		&& (WIN_MANAGER->layerRGLocation.size() < 1)) return;

	int nUpper, nLower;

	if (m_mode == RG_MODE_HU_RANGE)
	{
		ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();
		nUpper = WIN_MANAGER->getThreUpper();
		nLower = WIN_MANAGER->getThreLower();

		if (tab)
		{
			if (nLower > WIN_MANAGER->regionGrowingHu)
			{
				tab->setStartHU(nLower - (nLower - WIN_MANAGER->regionGrowingHu));
			}

			else if (nUpper < WIN_MANAGER->regionGrowingHu)
			{
				tab->setEndHU(WIN_MANAGER->regionGrowingHu);
			}
		}

		CustomHistogram* view = WIN_MANAGER->mainSegmentWidget->getHistogramView();
		if (view)
		{
			if (view->HuRangeSlider)
			{
				nUpper = WIN_MANAGER->getThreUpper();
				nLower = WIN_MANAGER->getThreLower();

				view->HuRangeSlider->setFirstValue(nLower);
				view->HuRangeSlider->setSecondValue(nUpper);
			}
		}

	}

	nUpper = WIN_MANAGER->getThreUpper();
	nLower = WIN_MANAGER->getThreLower();

#ifdef SELECT_HIDE
	WIN_MANAGER->setSeedShow(WIN_MANAGER->getSeedShow() | RT_SELECT);
#endif
	if (WIN_MANAGER->GetTab())
	{
		ThreSholdTab* tab = WIN_MANAGER->GetTab()->getThreSholdTab();
		if (tab)
		{
			tab->PreviewFinish();
		}
	}

	bool underROI;
	if (m_mode == RG_MODE_HU_RANGE)
	{
		underROI = false;
	}
	else
	{
		underROI = true;
	}

	bool isConnectivity_6 = false;
	if (m_connectivity == RG_CONNECTIVITY_26)
	{
		isConnectivity_6 = false;
	}
	else
	{
		isConnectivity_6 = true;
	}

	ACTION_MANAGER->action_RegionGrowing(
		WIN_MANAGER->regionGrowingX,
		WIN_MANAGER->regionGrowingY,
		WIN_MANAGER->regionGrowingZ,
		nUpper, nLower,
		(!WIN_MANAGER->layerRGLocation.isEmpty()) ? true : underROI,
		WIN_MANAGER->getSelectedMask(),
		WIN_MANAGER->getSelectedMaskByteIndex(),
		isConnectivity_6,
		WIN_MANAGER->getRGType());
}

void ThreSholdTab::slot_OnSelectSeed()
{
	// threshold
	// 	if (DATA_CONTEXT->volume_data.isValidate())
	// 		WIN_MANAGER->selectSeedMode();

	////////////////////////////
	// region growing
	if (!DATA_CONTEXT->volume_data.isValidate())
		return;

	WORK_MODE mode = WIN_MANAGER->getWorkMode();

	if (mode == WORK_SEED_SELECT)
		WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
	else
	{
		m_btnSelect->setText(STRING_MANAGER->getString(STR_SELECT_SEED) + " mode cancel");
		WIN_MANAGER->selectSeedMode();
	}
}

void ThreSholdTab::slot_OnComboChanged(int index)
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	m_ThreType->blockSignals(true);
	m_ThreType->setCurrentIndex(index);
	m_ThreType->blockSignals(false);

	ThreType type = (ThreType)index;
	int nUpper, nLower;

	switch (type)
	{
	case THRE_SOFT_TISSUE:
		setStartHU(100);
		setEndHU(300);
		break;
	case THRE_AIR:
		setStartHU(-1024);
		setEndHU(-800);
		break;
	case THRE_FAT:
		setStartHU(-120);
		setEndHU(-90);
		break;
	case THRE_BONE:
		setStartHU(200);
		setEndHU(DATA_CONTEXT->volume_data.getHuMax());
		break;
	case THRE_FLUIDS:
		setStartHU(-30);
		setEndHU(75);
		break;
	case THRE_DEFAULT:
		setStartHU(-940);
		setEndHU(-830);
		break;

	default:
	{
		int HURange = (DATA_CONTEXT->volume_data.getHuMax() - DATA_CONTEXT->volume_data.getHuMin()) / 2;

		HURange += (DATA_CONTEXT->volume_data.getHuMin());

		if ((HURange / 2) < 30)
		{
			setStartHU(HURange - 30);
			setEndHU(HURange + 30);
		}
		else
		{
			setStartHU(HURange - (HURange / 2));
			setEndHU(HURange + (HURange / 2));
		}
	}
	break;
	}

	CustomHistogram* view = WIN_MANAGER->mainSegmentWidget->getHistogramView();

	if (view)
	{
		if (view->HuRangeSlider)
		{
			nLower = getStartHU();
			nUpper = getEndHU();

			view->HuRangeSlider->setFirstValue(nLower);
			view->HuRangeSlider->setSecondValue(nUpper);
		}
	}
}

void ThreSholdTab::slot_OnChangeMode(int mode)
{
	if (mode == m_mode)
	{
		return;
	}

	if (mode == 0)
	{
		setRegionGrowingMode(RG_MODE_HU_RANGE);
	}
	else if (mode == 1)
	{
		setRegionGrowingMode(RG_MODE_LAYER);
	}
}

void ThreSholdTab::slot_OnChangeConnect(int mode)
{
	if (mode == m_connectivity)
	{
		return;
	}

	if (mode == RG_CONNECTIVITY_26)
	{
		setRegionGrowingConnectivity(RG_CONNECTIVITY_26);
	}
	else
	{
		setRegionGrowingConnectivity(RG_CONNECTIVITY_6);
	}
}

void ThreSholdTab::slot_OnResetRG()
{
	WIN_MANAGER->seedLocation.clear();
	WIN_MANAGER->layerRGLocation.clear();

	WIN_MANAGER->regionGrowingHu = WIN_MANAGER->regionGrowingX = WIN_MANAGER->regionGrowingY = WIN_MANAGER->regionGrowingZ = -1;
	UpdateRGList();
}

void ThreSholdTab::slot_OnDelRG()
{
	QList<QTreeWidgetItem*>& list = m_RGList->selectedItems();

	if (list.size() <= 0)
	{
		QMessageBox::warning(this, tr("Region Growing deletion"), tr("Please select items to delete."));
		return;
	}

	for (int i = list.size() - 1; i >= 0; i--)
	{
		int index = m_RGList->indexOfTopLevelItem(list.at(i));

		WIN_MANAGER->seedLocation.remove(index);
	}

	UpdateRGList();

	WIN_MANAGER->renderLater_GridView(false);
}
