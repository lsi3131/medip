#include "stdafx.h"
#include "Visualize2DTab.h"
#include "StringManager.h"
#include "StyleManager.h"
#include "WindowManager.h"
#include "ProductManager.h"
#include "Main/MainSegmentWidget.h"
#include "System/FileManager.h"
#include "Dialogs/PresetDlg.h"
#include "DataContext.h"

Visualize2DTab::Visualize2DTab(ProductManager* pProductManager, QWidget* parent /*= NULL*/) : 
	CollapseWidget(QString(), parent), 
	m_pProductManager(pProductManager),
	m_fAutoScaleSlope(1.5f),
	m_rootPETWidget(nullptr)
{
	m_pProductFactory = m_pProductManager->getFactory();

	//-> 2d preset	 
	int nRow = 0;
	QLabel* labelPreset = new QLabel(STRING_MANAGER->getString(STR_PRESET), this);
	//QLabel *labelPreset = m_pProductFactory->createWidget<QLabel>(MS_COLLAPS_2DVIS_PRESET, this);
	if (labelPreset) labelPreset->setText(STRING_MANAGER->getString(STR_PRESET));


	//Factory factory
	m_presetCombo = new QComboBox(this);
	//m_presetCombo = m_pProductFactory->createWidget<QComboBox>(MS_COLLAPS_2DVIS_PRESET, this);	
	if (m_presetCombo)
	{
		m_presetCombo->setObjectName("ComboPreset");
		m_presetCombo->addItem(STRING_MANAGER->getString(STR_DEFAULT));
		setItemData_presetCombo(eCFLApplicationPah);
		m_presetCombo->addItem(STRING_MANAGER->getString(STR_RECOMMEND_1));
		setItemData_presetCombo(eCFLApplicationPah);
		m_presetCombo->addItem(STRING_MANAGER->getString(STR_RECOMMEND_2));
		setItemData_presetCombo(eCFLApplicationPah);
		m_presetCombo->addItem(STRING_MANAGER->getString(STR_BONE));
		setItemData_presetCombo(eCFLApplicationPah);
		m_presetCombo->addItem(STRING_MANAGER->getString(STR_LUNG));
		setItemData_presetCombo(eCFLApplicationPah);
		m_presetCombo->addItem(STRING_MANAGER->getString(STR_MEDIASTINAL));
		setItemData_presetCombo(eCFLApplicationPah);
		m_presetCombo->addItem(STRING_MANAGER->getString(STR_BRAIN));
		setItemData_presetCombo(eCFLApplicationPah);
		m_presetCombo->addItem(STRING_MANAGER->getString(STR_ABD));
		setItemData_presetCombo(eCFLApplicationPah);
		m_presetCombo->addItem(STRING_MANAGER->getString(STR_LIVER));
		setItemData_presetCombo(eCFLApplicationPah);
		m_presetCombo->addItem(STRING_MANAGER->getString(STR_KIDNEY));
		setItemData_presetCombo(eCFLApplicationPah);
		m_presetCombo->addItem(STRING_MANAGER->getString(STR_URINARY_TRACK_BLADDER));
		setItemData_presetCombo(eCFLApplicationPah);
		m_presetCombo->addItem(STRING_MANAGER->getString(STR_CUSTOM));
		setItemData_presetCombo(eCFLAppDataLocalPath);

		for (int i = 0; i < WIN_MANAGER->getPresetCount(); i++)
		{
			m_presetCombo->addItem(WIN_MANAGER->getPresetName(i));
			setItemData_presetCombo(WIN_MANAGER->getPresetPathType(i));
		}

		m_presetCombo->setStyleSheet(STYLE_MANAGER->comboBoxTab);
		m_presetCombo->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
		m_presetCombo->installEventFilter(this);

		connect(m_presetCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnComboChanged(int)));
	}

	//m_presetEdit = new QPushButton(this);
	m_presetEdit = m_pProductFactory->createWidget<QPushButton>(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising, this);
	if (m_presetEdit)
	{
		m_presetEdit->setText("Save");
		m_presetEdit->setStyleSheet(STYLE_MANAGER->buttonBehind
			+ QString("QPushButton{min-width:%1px;}").arg(m_presetEdit->text().size() * 7));
		m_presetEdit->setMinimumWidth(40);
		m_presetEdit->hide();

		connect(m_presetEdit, &QPushButton::clicked, this, &Visualize2DTab::slot_OnSave);
		// end 2d preset
	}

	//m_presetDel = new QPushButton(this);
	m_presetDel = m_pProductFactory->createWidget<QPushButton>(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising, this);
	if (m_presetDel)
	{
		m_presetDel->setText("Delete");
		m_presetDel->setStyleSheet(STYLE_MANAGER->buttonBehind
			+ QString("QPushButton{min-width:%1px;}").arg(m_presetDel->text().size() * 7));
		m_presetDel->setMinimumWidth(40);
		m_presetDel->hide();

		connect(m_presetDel, &QPushButton::clicked, this, &Visualize2DTab::slot_OnDel);
	}

	if (labelPreset) addWidget(labelPreset, nRow++, 0, QMargins(0, 0, 10, 0));
	if (m_presetCombo) addWidget(m_presetCombo, nRow, 1);
	if (m_presetEdit) addWidget(m_presetEdit, nRow, 1);
	if (m_presetDel) addWidget(m_presetDel, nRow, 1);
	nRow++;

	QFrame* frLine = new QFrame(this);
	frLine->setStyleSheet("background-color:#414141;");
	frLine->setFrameShape(QFrame::HLine);
	frLine->setFrameShadow(QFrame::Sunken);
	frLine->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	addWidget(frLine, nRow++, 0, QMargins(0, 5, 0, 5));


	QWidget* p2ndLine = new QWidget(this);
	QBoxLayout* groupHori = new QHBoxLayout;
	QBoxLayout* groupVer1 = new QVBoxLayout;
	QBoxLayout* groupVer2 = new QVBoxLayout;

	QBoxLayout* groupHori1_1 = new QHBoxLayout;
	QBoxLayout* groupHori1_2 = new QHBoxLayout;

	QBoxLayout* groupHori2_1 = new QHBoxLayout;



	groupHori->addLayout(groupVer1);
	groupHori->addLayout(groupVer2);

	groupVer1->addLayout(groupHori1_1);
	groupVer1->addLayout(groupHori1_2);

	groupVer2->addLayout(groupHori2_1);

	p2ndLine->setLayout(groupHori);

	addWidget(p2ndLine, nRow++, 0);

	QLabel* label1 = new QLabel(STRING_MANAGER->getString(STR_WIDTH), this);
	//QLabel *label1 = m_pProductFactory->createWidget<QLabel>(MS_COLLAPS_2DVIS_COMMON, this);
	if (label1) label1->setText(STRING_MANAGER->getString(STR_WIDTH));
	label1->setFixedWidth(40);

	m_textWidth = new QLineEdit(this);
	//m_textWidth = m_pProductFactory->createWidget<QLineEdit>(MS_COLLAPS_2DVIS_COMMON, this);
	if (m_textWidth)
	{
		m_textWidth->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textWidth->setText(tr("0"));
		m_textWidth->setAlignment(Qt::AlignCenter);
		m_textWidth->setFixedWidth(60);
		m_textWidth->setObjectName("EditWidth");
		m_textWidth->setMouseTracking(true);
		m_textWidth->installEventFilter(this);
		connect(m_textWidth, &QLineEdit::returnPressed, this, &Visualize2DTab::slot_OnVisualizationOKClick);
		connect(m_textWidth, &QLineEdit::editingFinished, this, &Visualize2DTab::slot_OnVisualizationOKClick);
	}

	m_slideWidth = new QSlider(this);
	//m_slideWidth = m_pProductFactory->createWidget<QSlider>(MS_COLLAPS_2DVIS_COMMON, this);
	if (m_slideWidth)
	{
		m_slideWidth->setObjectName("SliderWidth");
		m_slideWidth->setOrientation(Qt::Horizontal);
		m_slideWidth->setStyleSheet(STYLE_MANAGER->sliderBarTab);
		m_slideWidth->installEventFilter(this);
		connect(m_slideWidth, &QSlider::valueChanged, this, &Visualize2DTab::slot_OnWidthChanged);
	}

	if (label1) groupHori1_1->addWidget(label1, 1);
	if (m_textWidth) groupHori1_1->addWidget(m_textWidth, 1);
	if (m_slideWidth) groupHori1_1->addWidget(m_slideWidth, 2);

	label1 = new QLabel(STRING_MANAGER->getString(STR_LEVEL), this);
	//label1 = nullptr;
	//label1 = m_pProductFactory->createWidget<QLabel>(MS_COLLAPS_2DVIS_COMMON, this);
	if (label1) label1->setText(STRING_MANAGER->getString(STR_LEVEL));
	label1->setFixedWidth(40);

	m_textLevel = new QLineEdit(this);
	//m_textLevel = m_pProductFactory->createWidget<QLineEdit>(MS_COLLAPS_2DVIS_COMMON, this);
	if (m_textLevel)
	{
		m_textLevel->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textLevel->setText(tr("0"));
		m_textLevel->setAlignment(Qt::AlignCenter);
		m_textLevel->setFixedWidth(60);
		m_textLevel->setObjectName("EditLevel");
		m_textLevel->setMouseTracking(true);
		m_textLevel->installEventFilter(this);
		connect(m_textLevel, &QLineEdit::returnPressed, this, &Visualize2DTab::slot_OnVisualizationOKClick);
		connect(m_textLevel, &QLineEdit::editingFinished, this, &Visualize2DTab::slot_OnVisualizationOKClick);
	}

	m_slideLevel = new QSlider(this);
	//m_slideLevel = m_pProductFactory->createWidget<QSlider>(MS_COLLAPS_2DVIS_COMMON, this);
	if (m_slideLevel)
	{
		m_slideLevel->setObjectName("SliderLevel");
		m_slideLevel->setOrientation(Qt::Horizontal);
		m_slideLevel->setStyleSheet(STYLE_MANAGER->sliderBarTab);
		m_slideLevel->installEventFilter(this);
		connect(m_slideLevel, &QSlider::valueChanged, this, &Visualize2DTab::slot_OnLevelChanged);
	}


	if (label1) groupHori1_2->addWidget(label1, 1);
	if (m_textLevel) groupHori1_2->addWidget(m_textLevel, 1);
	if (m_slideLevel) groupHori1_2->addWidget(m_slideLevel, 2);

	QPushButton* btnAuto = new QPushButton(this);
	//QPushButton* btnAuto = m_pProductFactory->createWidget<QPushButton>(MS_COLLAPS_2DVIS_COMMON, this);
	if (btnAuto)
	{
		btnAuto->setText(STRING_MANAGER->getString(STR_AUTO));
		btnAuto->setStyleSheet(STYLE_MANAGER->buttonBehind);
		btnAuto->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		connect(btnAuto, &QPushButton::released, this, &Visualize2DTab::slot_OnAutoClick);
		connect(this, SIGNAL(sig_autoClickSgl()), this, SLOT(slot_OnAutoClick()));
	}


	QPushButton* btnReset = new QPushButton(this);
	//QPushButton* btnReset = m_pProductFactory->createWidget<QPushButton>(MS_COLLAPS_2DVIS_COMMON, this);
	if (btnReset)
	{
		btnReset->setText(STRING_MANAGER->getString(STR_RESET));
		btnReset->setStyleSheet(STYLE_MANAGER->buttonBehind);
		btnReset->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		connect(btnReset, &QPushButton::released, this, &Visualize2DTab::slot_OnResetClick);
	}

	if (btnAuto) groupHori2_1->addWidget(btnAuto);
	if (btnReset) groupHori2_1->addWidget(btnReset);

	//addWidget(NULL, nRow++);

	createFrameLine(nRow++);

	label1 = new QLabel("Layer Opacity", this);
	label1->setFixedWidth(100);
	//label1 = nullptr;
	//label1 = m_pProductFactory->createWidget<QLabel>(MS_COLLAPS_2DVIS_COMMON, this);
	if (label1) label1->setText(QString("Layer Opacity"));

	m_textLAlpha = new QLineEdit(this);
	//m_textLAlpha = m_pProductFactory->createWidget<QLineEdit>(MS_COLLAPS_2DVIS_COMMON, this);
	if (m_textLAlpha)
	{
		m_textLAlpha->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textLAlpha->setAlignment(Qt::AlignCenter);
		m_textLAlpha->setFixedWidth(60);
		m_textLAlpha->setValidator(new QIntValidator(this));
		m_textLAlpha->setText(QString::number((int)(200 * WIN_MANAGER->getLayer2DAlphaValue())));
		m_textLAlpha->setObjectName("EditLayer2D");
		m_textLAlpha->installEventFilter(this);
		m_textLAlpha->setMouseTracking(true);

		connect(m_textLAlpha, &QLineEdit::returnPressed, this, &Visualize2DTab::slot_OnAlphaEditFinish);
		connect(m_textLAlpha, &QLineEdit::editingFinished, this, &Visualize2DTab::slot_OnAlphaEditFinish);
	}


	m_slideLayer2D = new QSlider(this);
	//m_slideLayer2D = m_pProductFactory->createWidget<QSlider>(MS_COLLAPS_2DVIS_COMMON, this);
	if (m_slideLayer2D)
	{
		m_slideLayer2D->setObjectName("SliderLayer2D");
		m_slideLayer2D->setOrientation(Qt::Horizontal);
		m_slideLayer2D->installEventFilter(this);
		m_slideLayer2D->setRange(0, 200);
		m_slideLayer2D->setValue((int)(200 * WIN_MANAGER->getLayer2DAlphaValue()));
		m_slideLayer2D->setStyleSheet(STYLE_MANAGER->sliderBarTab);
		connect(m_slideLayer2D, &QSlider::sliderMoved, this, &Visualize2DTab::slot_OnAlphaChanged);
		connect(m_slideLayer2D, &QSlider::sliderReleased, this, &Visualize2DTab::slot_OnAlphaRelease);
	}

	if (label1) addWidget(label1, nRow, 1);
	if (m_textLAlpha) addWidget(m_textLAlpha, nRow, 1);
	if (m_slideLayer2D) addWidget(m_slideLayer2D, nRow++, 2);

	createFrameLine(nRow++);

	QCheckBox* chkbox = new QCheckBox(this);

	// 	QCheckBox *chkbox = nullptr;
	// 	chkbox = m_pProductFactory->createWidget<QCheckBox>(MS_COLLAPS_2DVIS_CHECK_BOX, this);
	if (chkbox)
	{
		chkbox->setText("Apply Preset Color");
		chkbox->setCheckable(true);
		chkbox->setChecked(WIN_MANAGER->getColor2DMode());
		chkbox->setStyleSheet(STYLE_MANAGER->m_Checkbox);

		connect(chkbox, &QCheckBox::clicked, this, &Visualize2DTab::slot_OnColorCheck);
		addWidget(chkbox, nRow++);
	}

	chkbox = new QCheckBox(this);
	//chkbox = m_pProductFactory->createWidget<QCheckBox>(MS_COLLAPS_2DVIS_CHECK_BOX, this);
	if (chkbox)
	{
		chkbox->setText("Direction Indicator");
		chkbox->setCheckable(true);
		chkbox->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		connect(chkbox, &QCheckBox::clicked, this, &Visualize2DTab::slot_OnDirectionCheck);
		addWidget(chkbox, nRow++);
	}
	chkbox = new QCheckBox(this);
	//chkbox = m_pProductFactory->createWidget<QCheckBox>(MS_COLLAPS_2DVIS_CHECK_BOX, this);
	if (chkbox)
	{
		chkbox->setText("Slice Repositioning");
		chkbox->setCheckable(true);
		chkbox->setChecked(WIN_MANAGER->getRepositioningMode());
		chkbox->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		connect(chkbox, &QCheckBox::clicked, this, &Visualize2DTab::slot_OnRepositioningCheck);
		addWidget(chkbox, nRow++);
	}
	chkbox = new QCheckBox(this);
	//chkbox = m_pProductFactory->createWidget<QCheckBox>(MS_COLLAPS_2DVIS_CHECK_BOX, this);
	if (chkbox)
	{
		chkbox->setText("Inverting LUT");
		chkbox->setCheckable(true);
		chkbox->setChecked(WIN_MANAGER->getInvertingLUTMode());
		chkbox->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		connect(chkbox, &QCheckBox::clicked, this, &Visualize2DTab::slot_OnInvertingLUTCheck);
		addWidget(chkbox, nRow++);
	}

	// Mesh 2D Outline
	m_chkMesh2DOutline = m_pProductFactory->createWidget<QCheckBox>(MFL_Common_Rendering_MeshTabList, this);
	if (m_chkMesh2DOutline)
	{
		m_chkMesh2DOutline->setText("Mesh 2D Outline");
		m_chkMesh2DOutline->setCheckable(true);
		m_chkMesh2DOutline->setChecked(false);
		m_chkMesh2DOutline->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		connect(m_chkMesh2DOutline, &QCheckBox::clicked, this, &Visualize2DTab::slot_OnMeshOutlineCheck);
		addWidget(m_chkMesh2DOutline, nRow++);
	}

#ifdef PET_CT
	createPETOverlayWidget(nRow);
#endif

	QString title = QString("2D Visualization");

	setTitle(title);
	m_cusPreset = -1;
	setOpenWidget(false);
}

void Visualize2DTab::ResetUI()
{
	updatePETOverlayControl();
}

void Visualize2DTab::resetSlide()
{
	setPreset(SP_DEFAULT, true);
	WIN_MANAGER->mainSegmentWidget->SetWindowWidthLine(true);
}

void Visualize2DTab::setWidth(int val)
{
	if (m_slideWidth)
	{
		m_slideWidth->blockSignals(true);
		m_textWidth->blockSignals(true);
		m_slideWidth->setValue(val);
		m_textWidth->setText(QString::number(val));
		m_slideWidth->blockSignals(false);
		m_textWidth->blockSignals(false);
	}
}

void Visualize2DTab::setLevel(int val)
{
	if (m_slideLevel)
	{
		m_slideLevel->blockSignals(true);
		m_textLevel->blockSignals(true);
		m_slideLevel->setValue(val);
		m_textLevel->setText(QString::number(val));
		m_slideLevel->blockSignals(false);
		m_textLevel->blockSignals(false);
	}
}

void Visualize2DTab::addCustomPreset(int index, bool isImport, bool bAppDataPath)
{
	m_presetCombo->addItem(WIN_MANAGER->getPresetName(index));

	bAppDataPath == true ? setItemData_presetCombo(eCFLAppDataLocalPath) : setItemData_presetCombo(eCFLApplicationPah);

	if (!isImport)
	{
		WIN_MANAGER->setConfigWL(QString("SCREEN_WL"), m_textWidth->text().toInt(),
			m_textLevel->text().toInt(), index);
	}

}

void Visualize2DTab::delCustomPreset(int index)
{
	m_presetCombo->setCurrentIndex(0); //sp_default set

	m_presetCombo->removeItem(index + SP_COUNT);
}

bool Visualize2DTab::getPresetSetting(int& wl, int& ww, SLICE_PRESET type, int cusPre)
{
	bool res = false;
	if (-1 < cusPre)
	{
		if (WIN_MANAGER->IsLicensePass())
		{
			res = WIN_MANAGER->getConfigWL(QString("SCREEN_WL"), ww, wl, SP_COUNT + cusPre);
		}
	}
	else
		res = WIN_MANAGER->getConfigWL(QString("SCREEN_WL%1").arg(type), ww, wl);

	if (!res)
	{
		switch (m_preset)
		{
		case SP_RECOMMAND_1:
			ww = 960;
			wl = 229;
			break;
		case SP_RECOMMAND_2:
			ww = 960;
			wl = 229;
			break;
		case SP_LUNG:
			ww = 1500;
			wl = -700;
			break;
		case SP_BONE:
			ww = 2000;
			wl = 250;
			break;
		case SP_MEDIASTINAL:
			ww = 400;
			wl = 20;
			break;
		case SP_BRAIN:
			ww = 70;
			wl = 30;
			break;
		case SP_ABD:
			ww = 40;
			wl = 300;
			break;
		case SP_LIVER:
			ww = 30;
			wl = 400;
			break;
		case SP_KIDNEY:
			ww = 500;
			wl = 400;
			break;
		case SP_URINARY_TRACK_BLADDER:
			ww = 50;
			wl = 30;
			break;
		case SP_CUSTOM:
			ww = 500;
			wl = 400;
			break;
		default:
		case SP_DEFAULT:
			ww = WIN_MANAGER->getDefaultWidth();
			wl = WIN_MANAGER->getDefaultLevel();
			break;
		}
	}

	return true;
}

bool Visualize2DTab::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == NULL);

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

	return QWidget::eventFilter(watched, event);
}

void Visualize2DTab::InAreaWidth(int& nWidth)
{
	nWidth = mip::math::Max(nWidth, m_slideWidth->maximum()) == nWidth ? m_slideWidth->maximum() :
		mip::math::Min(nWidth, m_slideWidth->minimum()) == nWidth ? m_slideWidth->minimum() : nWidth;
}

void Visualize2DTab::InAreaLevel(int& nLevel)
{
	nLevel = mip::math::Max(nLevel, m_slideLevel->maximum()) == nLevel ? m_slideLevel->maximum() :
		mip::math::Min(nLevel, m_slideLevel->minimum()) == nLevel ? m_slideLevel->minimum() : nLevel;
}

void Visualize2DTab::createPETOverlayWidget(int& nRow)
{
	m_rootPETWidget = new QWidget(this);
	m_rootPETWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	QBoxLayout* groupPET_Root = new QVBoxLayout;

	QBoxLayout* groupPET_FrameLine = new QVBoxLayout;
	QBoxLayout* groupPET_Title = new QVBoxLayout;
	QBoxLayout* groupPET_Vert1 = new QVBoxLayout;
	QBoxLayout* groupPET_Vert2 = new QVBoxLayout;

	QBoxLayout* groupPET_Horz_1_1 = new QHBoxLayout;
	QBoxLayout* groupPET_Horz_2_1 = new QHBoxLayout;

	groupPET_Root->setContentsMargins(0, 0, 0, 0);
	groupPET_FrameLine->setContentsMargins(0, 0, 0, 0);
	groupPET_Title->setContentsMargins(0, 0, 0, 0);
	groupPET_Vert1->setContentsMargins(6, 6, 6, 6);
	groupPET_Vert2->setContentsMargins(6, 6, 6, 6);
	groupPET_Horz_1_1->setContentsMargins(0, 0, 0, 0);
	groupPET_Horz_2_1->setContentsMargins(0, 0, 0, 0);

	groupPET_Root->addLayout(groupPET_FrameLine, 1);
	groupPET_Root->addLayout(groupPET_Title, 1);
	groupPET_Root->addLayout(groupPET_Vert1, 1);
	groupPET_Root->addLayout(groupPET_Vert2, 1);

	groupPET_Vert1->addLayout(groupPET_Horz_1_1);
	groupPET_Vert2->addLayout(groupPET_Horz_2_1);

	m_rootPETWidget->setLayout(groupPET_Root);

	addWidget(m_rootPETWidget, nRow++, 0);

	/* Frame Line*/
	QFrame* frLine = new QFrame(this);
	if (frLine)
	{
		frLine->setStyleSheet("background-color:#414141;");
		frLine->setFrameShape(QFrame::HLine);
		frLine->setFrameShadow(QFrame::Sunken);
		frLine->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
		groupPET_FrameLine->addWidget(frLine);
	}

	/* Title*/
	QLabel* labelTitle = new QLabel(this);
	if (labelTitle)
	{
		labelTitle->setText(tr("PET-CT Overlay"));
		groupPET_Title->addWidget(labelTitle);
	}

	/* Line 1 - Load Button, Edit Status*/
	QPushButton* loadButton = new QPushButton(this);
	if (loadButton)
	{
		loadButton->setText(tr("Load PET Image"));
		loadButton->setStyleSheet(STYLE_MANAGER->buttonBehind);
		groupPET_Horz_1_1->addWidget(loadButton, 1);
		connect(loadButton, &QPushButton::clicked, this, &Visualize2DTab::slot_onPETLoadBtnClicked);
	}

	m_editPETLoadStatus = new QLineEdit(this);
	if (m_editPETLoadStatus)
	{
		m_editPETLoadStatus->setStyleSheet(STYLE_MANAGER->editBoxNormal);
		m_editPETLoadStatus->setReadOnly(true);
		groupPET_Horz_1_1->addWidget(m_editPETLoadStatus, 2);
	}

	/* Line 2 - Check PET-ON, Opacity Slider */
	m_chkPETOn = new QCheckBox(this);
	if (m_chkPETOn)
	{
		m_chkPETOn->setText(tr("Show PET"));
		m_chkPETOn->setStyleSheet(STYLE_MANAGER->buttonChecked);
		groupPET_Horz_2_1->addWidget(m_chkPETOn);
		connect(m_chkPETOn, &QPushButton::clicked, this, &Visualize2DTab::slot_onPETONChkClicked);
	}

	m_slideOpacity = new QSlider(this);
	if (m_slideOpacity)
	{
		groupPET_Horz_2_1->addWidget(m_slideOpacity);
		m_slideOpacity->setStyleSheet(STYLE_MANAGER->sliderBarTab);
		m_slideOpacity->setMaximum(100);
		m_slideOpacity->setMinimum(0);
		m_slideOpacity->setOrientation(Qt::Horizontal);
		m_slideOpacity->setValue(50);

		connect(m_slideOpacity, &QSlider::valueChanged, this, &Visualize2DTab::slot_onPETOpacityValueChanged);
	}

	QWidget* emptyBox0 = new QWidget(this);
	if (emptyBox0)
	{
		emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		addWidget(emptyBox0, nRow++, 0);
	}

	updatePETOverlayControl();
	/* 처음 시작 시 Hide상태로 초기화 */
	m_rootPETWidget->setVisible(false);
}

void Visualize2DTab::updatePETOverlayControl()
{
	m_chkPETOn->setChecked(DATA_CONTEXT->volume_data_PET.isVisible());
	if (DATA_CONTEXT->volume_data_PET.isValidate())
	{
		m_editPETLoadStatus->setText("PET Data is Loaded");
		m_chkPETOn->setEnabled(true);
		m_slideOpacity->setEnabled(true);
	}
	else
	{
		m_editPETLoadStatus->setText("PET Data is Not Loaded");
		m_chkPETOn->setEnabled(false);
		m_slideOpacity->setEnabled(false);
	}
}

void Visualize2DTab::setPreset(SLICE_PRESET type, bool reset, int cusPre)
{
	int nWidth, nLevel;
	bool res;
	QString resStr;
	if (m_slideWidth == NULL) return;
	if (m_presetCombo) m_presetCombo->blockSignals(true);
	if (-1 >= cusPre)
	{
		m_prePreset = m_preset;
		m_preset = type;
		m_cusPreset = -1;
		if (m_presetDel)
			m_presetDel->hide();

		if (m_presetEdit)
		{
			if (m_preset == SP_CUSTOM)
			{
				if (0 != m_presetEdit->text().compare("Save"))
					m_presetEdit->setText("Save");

				if (m_presetEdit->isHidden() && WIN_MANAGER->IsLicensePass())
					m_presetEdit->show();
			}
			else
			{
				if (!m_presetEdit->isHidden())
					m_presetEdit->hide();
			}
		}

#ifdef DEV_USE_APPDATA_PATH
		eConfigFileLocation configFileLoc = (eConfigFileLocation)m_presetCombo->itemData(m_presetCombo->currentIndex(), Qt::UserRole).toUInt();
		STRING_MANAGER->presetFilePath = STRING_MANAGER->presetFilePathVec.at(configFileLoc);
#endif

		if (m_presetCombo)
			m_presetCombo->setCurrentIndex((int)type);
		res = WIN_MANAGER->getConfigWL(QString("SCREEN_WL%1").arg(type), nWidth, nLevel);
	}
	else
	{
		if (!WIN_MANAGER->IsLicensePass())
		{
			m_cusPreset = cusPre = -1;
			if (m_presetCombo)
				m_presetCombo->setCurrentIndex((int)m_preset);
			res = WIN_MANAGER->getConfigWL(QString("SCREEN_WL%1").arg(m_preset), nWidth, nLevel);
		}
		else
		{
			if (m_presetDel)
				m_presetDel->show();

			if (m_presetEdit)
			{
				if (0 != m_presetEdit->text().compare("Edit"))
					m_presetEdit->setText("Edit");

				if (m_presetEdit->isHidden())
					m_presetEdit->show();
			}

#ifdef DEV_USE_APPDATA_PATH
			eConfigFileLocation configFileLoc = (eConfigFileLocation)m_presetCombo->itemData(m_presetCombo->currentIndex(), Qt::UserRole).toUInt();
			STRING_MANAGER->presetFilePath = STRING_MANAGER->presetFilePathVec.at(configFileLoc);
			Show_PresetEditDeleteBtn(configFileLoc == eCFLAppDataLocalPath ? true : false);
#endif

			m_cusPreset = cusPre;
			if (m_presetCombo)
				m_presetCombo->setCurrentIndex(m_cusPreset + SP_COUNT);
			res = WIN_MANAGER->getConfigWL(QString("SCREEN_WL"), nWidth, nLevel, m_cusPreset);
		}
	}

	if (m_presetCombo)
		m_presetCombo->blockSignals(false);

	m_slideLevel->setMinimum(DATA_CONTEXT->volume_data.getHuMin());
	m_slideLevel->setMaximum(DATA_CONTEXT->volume_data.getHuMax());
	m_slideWidth->setMinimum(1);
	m_slideWidth->setMaximum(abs(DATA_CONTEXT->volume_data.getHuMax() -
		DATA_CONTEXT->volume_data.getHuMin()));

	if (!res)
	{
		if (-1 >= m_cusPreset)
		{
			switch (m_preset)
			{
			case SP_RECOMMAND_1:
				nWidth = 960;
				nLevel = 229;
				break;
			case SP_RECOMMAND_2:
				nWidth = 960;
				nLevel = 229;
				break;
			case SP_LUNG:
				nWidth = 1500;
				nLevel = -700;
				break;
			case SP_BONE:
				nWidth = 2000;
				nLevel = 250;
				break;
			case SP_MEDIASTINAL:
				nWidth = 400;
				nLevel = 20;
				break;
			case SP_BRAIN:
				nWidth = 70;
				nLevel = 30;
				break;
			case SP_ABD:
				nWidth = 40;
				nLevel = 300;
				break;
			case SP_LIVER:
				nWidth = 30;
				nLevel = 400;
				break;
			case SP_KIDNEY:
				nWidth = 500;
				nLevel = 400;
				break;
			case SP_URINARY_TRACK_BLADDER:
				nWidth = 50;
				nLevel = 30;
				break;
			case SP_CUSTOM:
				nWidth = 500;
				nLevel = 400;
				break;
			default:
			case SP_DEFAULT:
				m_preset = SP_DEFAULT;
				nWidth = WIN_MANAGER->getDefaultWidth();
				nLevel = WIN_MANAGER->getDefaultLevel();
				break;
			}
		}
		else
		{
			nWidth = WIN_MANAGER->getDefaultWidth();
			nLevel = WIN_MANAGER->getDefaultLevel();
		}
	}

	if ((SP_DEFAULT == m_preset) && reset)
	{
		nWidth = WIN_MANAGER->getDefaultWidth();
		nLevel = WIN_MANAGER->getDefaultLevel();
	}

	InAreaWidth(nWidth);
	InAreaLevel(nLevel);

	WIN_MANAGER->setWindowWidthLevel(nWidth, nLevel);
	DATA_CONTEXT->volume_data.updateColorTable();
}

////////////////slots
void Visualize2DTab::slot_OnVisualizationOKClick()
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	int nWidth, nLevel;

	nWidth = WIN_MANAGER->getWindowWidth();
	nLevel = WIN_MANAGER->getWindowLevel();

	if (nLevel != m_textLevel->text().toInt())
	{
		nLevel = m_textLevel->text().toInt();
		InAreaLevel(nLevel);
		ACTION_MANAGER->action_Window_Level(nLevel);
	}

	if (nWidth != m_textWidth->text().toInt())
	{
		nWidth = m_textWidth->text().toInt();
		InAreaWidth(nWidth);
		ACTION_MANAGER->action_Window_Width(nWidth);
	}

	WIN_MANAGER->renderLater_GridView(false);
}

void Visualize2DTab::slot_OnLevelChanged(int pos)
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (m_slideLevel)
	{
		if (WIN_MANAGER->getWindowLevel() == pos) return;
		ACTION_MANAGER->action_Window_Level(pos);
	}
}


void Visualize2DTab::slot_OnWidthChanged(int pos)
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (m_slideWidth)
	{
		if (WIN_MANAGER->getWindowWidth() == pos) return;
		ACTION_MANAGER->action_Window_Width(pos);
	}
}

void Visualize2DTab::slot_OnAutoClick()
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	int prelevel = WIN_MANAGER->getWindowLevel();
	int prewidth = WIN_MANAGER->getWindowWidth();
	float preAutoScaleSlope = m_fAutoScaleSlope;

	int iNumBins = 256;
	double dWindow = 0, dLevel = 0;

	DATA_CONTEXT->volume_data.AutoWindowing(iNumBins, m_fAutoScaleSlope, -1000, dWindow, dLevel);
	m_fAutoScaleSlope = m_fAutoScaleSlope - 0.2;
	if (m_fAutoScaleSlope < 0) m_fAutoScaleSlope = 1.5;

	ACTION_MANAGER->action_LevelWidth_Auto(prelevel, dLevel, prewidth, dWindow, preAutoScaleSlope, m_fAutoScaleSlope);
}

void Visualize2DTab::slot_OnResetClick()
{
	WIN_MANAGER->setMoveFocus(true);
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;
	int nWidth, nLevel;

	nWidth = WIN_MANAGER->getWindowWidth();
	nLevel = WIN_MANAGER->getWindowLevel();

	setPreset(m_preset, true, m_cusPreset);

	if (nWidth != WIN_MANAGER->getWindowWidth())
		ACTION_MANAGER->action_Window_Width(m_slideWidth->value());
	if (nLevel != WIN_MANAGER->getWindowLevel())
		ACTION_MANAGER->action_Window_Level(m_slideLevel->value());
}

void Visualize2DTab::slot_OnDirectionCheck(bool val)
{
	WIN_MANAGER->setMoveFocus(true);

	WIN_MANAGER->setDirectionMode(val);

	WIN_MANAGER->renderLater_GridView(false);
}

void Visualize2DTab::slot_OnRepositioningCheck(bool val)
{
	WIN_MANAGER->setMoveFocus(true);
	WIN_MANAGER->setRepositioningMode(val);
	WIN_MANAGER->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_rePos, QString::number(-1 + val));
}

void Visualize2DTab::slot_OnInvertingLUTCheck(bool val)
{
	WIN_MANAGER->setMoveFocus(true);
	WIN_MANAGER->setInvertingLUTMode(val);
}

void Visualize2DTab::slot_OnMeshOutlineCheck(bool bset)
{
	WIN_MANAGER->setMoveFocus(true);

	// 2D View 업데이트
	WIN_MANAGER->renderLater_GridView(false);
}

void Visualize2DTab::slot_OnColorCheck(bool val)
{
	WIN_MANAGER->setMoveFocus(true);
	WIN_MANAGER->setColor2DMode(val);
}

void Visualize2DTab::slot_OnComboChanged(int index)
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	int preLevel = WIN_MANAGER->getWindowLevel();
	int preWidth = WIN_MANAGER->getWindowWidth();
	m_prePreset = m_preset;
	ACTION_MANAGER->action_Preset(preLevel, preWidth, m_prePreset, (SLICE_PRESET)(index % SP_COUNT), index - SP_COUNT);
}

void Visualize2DTab::slot_OnSave()
{
	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	int index = 0;
	if (m_presetCombo)
		index = m_presetCombo->currentIndex();

	bool res = false;
	if (index >= SP_COUNT) //custom preset edit
		res = true;

	PresetDialog dlg(res ? (index - SP_COUNT) : WIN_MANAGER->getPresetCount(), res, this);

	if (QDialog::Accepted == dlg.exec())
	{
		if (res)
			return;

#ifndef DEV_USE_APPDATA_PATH
		WIN_MANAGER->setPresetName(dlg.getTypeName(), WIN_MANAGER->getPresetCount());
#else
		eConfigFileLocation configFileLoc = (eConfigFileLocation)m_presetCombo->itemData(m_presetCombo->currentIndex(), Qt::UserRole).toUInt();
		if (configFileLoc == eCFLApplicationPah)
			WIN_MANAGER->setPresetName(dlg.getTypeName(), WIN_MANAGER->getPresetCount());
		else
			WIN_MANAGER->setPresetName(dlg.getTypeName(), WIN_MANAGER->getPresetCount(), false, true);
#endif
	}

}

void Visualize2DTab::slot_OnDel()
{
	if (m_presetCombo)
	{
#ifndef DEV_USE_APPDATA_PATH
		WIN_MANAGER->delPresetName(m_presetCombo->currentIndex() - SP_COUNT);
#else
		eConfigFileLocation configFileLoc = (eConfigFileLocation)m_presetCombo->itemData(m_presetCombo->currentIndex(), Qt::UserRole).toUInt();
		if (configFileLoc == eCFLApplicationPah)
			WIN_MANAGER->delPresetName(m_presetCombo->currentIndex() - SP_COUNT);
		else
			WIN_MANAGER->delPresetName(m_presetCombo->currentIndex() - SP_COUNT, true);
#endif
	}
}

void Visualize2DTab::slot_OnAlphaEditFinish()
{
	WIN_MANAGER->setMoveFocus(true);
	m_textLAlpha->blockSignals(true);
	m_slideLayer2D->blockSignals(true);
	int val = m_textLAlpha->text().toInt();

	if (val < 0)
	{
		val = 0;
		m_textLAlpha->setText(QString::number(val));
	}
	else if (val > 255)
	{
		val = 255;
		m_textLAlpha->setText(QString::number(val));
	}
	m_slideLayer2D->setValue(val);
	m_slideLayer2D->blockSignals(false);
	m_textLAlpha->blockSignals(false);

	WIN_MANAGER->setLayer2DAlphaValue(val);
}

void Visualize2DTab::slot_OnAlphaChanged(int val)
{
	WIN_MANAGER->setMoveFocus(true);
	m_textLAlpha->blockSignals(true);
	m_textLAlpha->setText(QString::number(val));
	m_textLAlpha->blockSignals(false);
	WIN_MANAGER->setLayer2DAlphaValue(val, false);
}

void Visualize2DTab::slot_OnAlphaRelease()
{
	WIN_MANAGER->setMoveFocus(true);
	if (m_slideLayer2D)
		WIN_MANAGER->setLayer2DAlphaValue(m_slideLayer2D->value(), true, false);
}

void Visualize2DTab::slot_onPETLoadBtnClicked()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false)
	{
		/* Volume이 Load되지 않을 경우 Skip한다. */
		return;
	}

	QString strLatest;
	bool latest = WIN_MANAGER->lastestPathGet(strLatest);
	QString strFilter = QString("DICOM File (*.dcm;*.DCM);;");

	//QString fileName = QFileDialog::getOpenFileName(this,
	//	STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN),
	//	latest ? strLatest : QDir::homePath(),
	//	strFilter,
	//	&selectedFilter,
	//	options);

	QFileInfo fileInfo(latest ? strLatest : QDir::homePath());
	QString fileName = ImportFileDialog(
		this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN),
		fileInfo.fileName(),
		fileInfo.dir().absolutePath(),
		strFilter
	);

	if (!fileName.isEmpty())
	{
		WIN_MANAGER->fileOpen_PET_DCM(fileName);
		WIN_MANAGER->lastestPathSave(fileName);
		updatePETOverlayControl();
	}
}


void Visualize2DTab::slot_onPETONChkClicked(bool checked)
{
	DATA_CONTEXT->volume_data_PET.setVisible(checked);
	WIN_MANAGER->forceUpdateRadiomicsView();
}

void Visualize2DTab::slot_onPETOpacityValueChanged(int value)
{
	float opacity = m_slideOpacity->value() / 100.0;
	DATA_CONTEXT->volume_data_PET.setOpacity(opacity);
	WIN_MANAGER->forceUpdateRadiomicsView();
}


void Visualize2DTab::setItemData_presetCombo(eConfigFileLocation eConfigFileLoc)
{
#ifdef DEV_USE_APPDATA_PATH
	m_presetCombo->setItemData(m_presetCombo->count() - 1, eConfigFileLoc, Qt::UserRole);
#endif
}

void Visualize2DTab::Show_PresetEditDeleteBtn(bool bShow)
{
	if (m_presetDel)
		bShow ? m_presetDel->show() : m_presetDel->hide();
	if (m_presetEdit)
		bShow ? m_presetEdit->show() : m_presetEdit->hide();
}

void Visualize2DTab::showWidgetByTabType(MAINTAB_TYPE type)
{
	if (m_rootPETWidget)
	{
		switch (type)
		{
		case MAINTAB_SEGMENTATION:
		case MAINTAB_MEASUREMENT:
			if (m_chkMesh2DOutline) m_chkMesh2DOutline->setVisible(true);
			break;
		case MAINTAB_TA:
			if (m_rootPETWidget) m_rootPETWidget->setVisible(true);
			if (m_chkMesh2DOutline)	m_chkMesh2DOutline->setVisible(false);
			break;
		default:
			if (m_rootPETWidget) m_rootPETWidget->setVisible(false);
			break;
		}
		updateUI();
	}
}

bool Visualize2DTab::isVisibleMesh2DOutlineCheckBox()
{
	bool bVisible = false;
	if (m_chkMesh2DOutline)
	{
		bVisible = m_chkMesh2DOutline->isVisible();
	}

	return bVisible;
}

bool Visualize2DTab::isOutlineMode()
{
	bool bChecked = false;
	if (m_chkMesh2DOutline)
	{
		bChecked = m_chkMesh2DOutline->isChecked();
	}

	return bChecked;
}

