#include "stdafx.h"
#include "Visualize3DTab.h"
#include "WindowManager.h"
#include "StringManager.h"
#include "StyleManager.h"
#include "WindowManager.h"
#include "ProductManager.h"
#include "Renderer/Renderer.h"
#include "Main/MainSegmentWidget.h"
#include "Main/MainAnalWidget.h"
#include "Dialogs/PresetDlg.h"
#include "DataContext.h"

Visualize3DTab::Visualize3DTab(ProductManager* pProductManager, QWidget* parent /*= NULL*/) :
	CollapseWidget(QString(), parent),
	m_pProductManager(pProductManager)
{
	m_pProductFactory = m_pProductManager->getFactory();
	m_nRow = 0;
	m_cusPreset = -1;
	createPresetUI();

	createWidthLevelUI();

	createAlphaUI();

	createShaderQualityUI();

#ifdef DEV_VER
	m_btnHighLight = NULL;
#endif	
	createClipOptionUI();

	createMixModeUI();
	m_nRow++;

	createVRUI();	// VR

	slot_OnShaderComboChanged(2);

	m_bPress = false;
	QString title = QString("3D Visualization");
	setTitle(title);

	setOpenWidget(false);
}

void Visualize3DTab::createPresetUI()
{
	QLabel* labelPreset = new QLabel(STRING_MANAGER->getString(STR_PRESET), this);
	if (labelPreset)
	{
		labelPreset->setText(STRING_MANAGER->getString(STR_PRESET));
	}

	m_presetCombo = new QComboBox(this);
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
		connect(m_presetEdit, &QPushButton::clicked, this, &Visualize3DTab::slot_OnSave);
	}

	//m_presetDel = new QPushButton(this);
	m_presetDel = m_pProductFactory->createWidget<QPushButton>(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising, this);
	if (m_presetDel)
	{
		m_presetDel->setText("Delete");
		m_presetDel->setStyleSheet(STYLE_MANAGER->buttonBehind
			+ QString("QPushButton{min-width:%1px;}").arg(m_presetDel->text().size() * 7));
		m_presetDel->hide();
		connect(m_presetDel, &QPushButton::clicked, this, &Visualize3DTab::slot_OnDel);
	}

	if (labelPreset)
	{
		addWidget(labelPreset, m_nRow++, 0, QMargins(0, 0, 15, 0));
	}

	if (m_presetCombo)
	{
		addWidget(m_presetCombo, m_nRow, 0);
	}

	if (m_presetEdit)
	{
		addWidget(m_presetEdit, m_nRow);
	}

	if (m_presetDel)
	{
		addWidget(m_presetDel, m_nRow);
	}
	m_nRow++;

}

void Visualize3DTab::createWidthLevelUI()
{
	createFrameLine(m_nRow++);

	QWidget* pWidthLevelLine = new QWidget(this);
	QBoxLayout* groupHori = new QHBoxLayout;
	QBoxLayout* groupVer1 = new QVBoxLayout;
	QBoxLayout* groupVer2 = new QVBoxLayout;

	QBoxLayout* groupHori1_1 = new QHBoxLayout;
	QBoxLayout* groupHori1_2 = new QHBoxLayout;

	//QBoxLayout*groupHori2_1 = new QHBoxLayout;

	groupHori->addLayout(groupVer1, 3);
	groupHori->addLayout(groupVer2, 1);

	groupVer1->addLayout(groupHori1_1);
	groupVer1->addLayout(groupHori1_2);

	//groupVer2->addLayout(groupHori2_1);

	pWidthLevelLine->setLayout(groupHori);

	addWidget(pWidthLevelLine, m_nRow++, 0);

	///////////// hori1-1
	QLabel* labelWidth = new QLabel(STRING_MANAGER->getString(STR_WIDTH), this);
	labelWidth->setFixedWidth(40);

	m_textWidth = new QLineEdit(this);
	m_textWidth->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_textWidth->setText(tr("0"));
	m_textWidth->setAlignment(Qt::AlignCenter);
	m_textWidth->setFixedWidth(60);
	m_textWidth->installEventFilter(this);
	m_textWidth->setObjectName("EditWidth");
	m_textWidth->setMouseTracking(true);
	m_textWidth->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	connect(m_textWidth, &QLineEdit::returnPressed, this, &Visualize3DTab::slot_OnWidthLevelOKClick);
	connect(m_textWidth, &QLineEdit::editingFinished, this, &Visualize3DTab::slot_OnWidthLevelOKClick);

	m_slideWidth = new QSlider(this);
	m_slideWidth->setObjectName("SliderWidth");
	m_slideWidth->setOrientation(Qt::Horizontal);
	m_slideWidth->setStyleSheet(STYLE_MANAGER->sliderBarTab);
	m_slideWidth->installEventFilter(this);
	m_slideWidth->setMouseTracking(true);
	connect(m_slideWidth, &QSlider::valueChanged, this, &Visualize3DTab::slot_OnWidthChanged);

	groupHori1_1->addWidget(labelWidth, 1);
	groupHori1_1->addWidget(m_textWidth, 1);
	groupHori1_1->addWidget(m_slideWidth, 3);

	///////////// hori1-2
	QLabel* labelLevel = new QLabel(STRING_MANAGER->getString(STR_LEVEL), this);
	labelLevel->setFixedWidth(40);

	m_textLevel = new QLineEdit(this);
	m_textLevel->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_textLevel->setText(tr("0"));
	m_textLevel->setAlignment(Qt::AlignCenter);
	m_textLevel->setFixedWidth(60);
	m_textLevel->installEventFilter(this);
	m_textLevel->setObjectName("EditLevel");
	m_textLevel->setMouseTracking(true);
	m_textLevel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	connect(m_textLevel, &QLineEdit::returnPressed, this, &Visualize3DTab::slot_OnWidthLevelOKClick);
	connect(m_textLevel, &QLineEdit::editingFinished, this, &Visualize3DTab::slot_OnWidthLevelOKClick);

	m_slideLevel = new QSlider(this);
	m_slideLevel->setObjectName("SliderLevel");
	m_slideLevel->setOrientation(Qt::Horizontal);
	m_slideLevel->setStyleSheet(STYLE_MANAGER->sliderBarTab);
	m_slideLevel->installEventFilter(this);
	m_slideLevel->setMouseTracking(true);
	connect(m_slideLevel, &QSlider::valueChanged, this, &Visualize3DTab::slot_OnLevelChanged);

	groupHori1_2->addWidget(labelLevel, 1);
	groupHori1_2->addWidget(m_textLevel, 1);
	groupHori1_2->addWidget(m_slideLevel, 3);

	/////////// ver2
	QPushButton* btnReset = new QPushButton(this);
	btnReset->setText(STRING_MANAGER->getString(STR_RESET));
	btnReset->setStyleSheet(STYLE_MANAGER->buttonBehind);
	btnReset->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	connect(btnReset, &QPushButton::released, this, &Visualize3DTab::slot_OnWidthLevelResetClick);
	groupVer2->addWidget(btnReset);

}

void Visualize3DTab::createAlphaUI()
{
	createFrameLine(m_nRow++);

	QWidget* pOpacityLine = new QWidget(this);
	QBoxLayout* groupHori = new QHBoxLayout;
	QBoxLayout* groupVer1 = new QVBoxLayout;
	QBoxLayout* groupVer2 = new QVBoxLayout;

	QBoxLayout* groupHori1_1 = new QHBoxLayout;
	QBoxLayout* groupHori1_2 = new QHBoxLayout;

	groupHori->addLayout(groupVer1, 3);
	groupHori->addLayout(groupVer2, 1);

	groupVer1->addLayout(groupHori1_1);
	groupVer1->addLayout(groupHori1_2);

	pOpacityLine->setLayout(groupHori);

	addWidget(pOpacityLine, m_nRow++, 0);

	QLabel* labelVolumeAlpha = new QLabel(QString("Volume Opacity"), this);
	labelVolumeAlpha->setFixedWidth(100);

	m_editVolumeAlpha = new QLineEdit(this);
	m_editVolumeAlpha->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_editVolumeAlpha->setText(tr("0"));
	m_editVolumeAlpha->setAlignment(Qt::AlignCenter);
	m_editVolumeAlpha->setFixedWidth(60);
	m_editVolumeAlpha->setMinimumWidth(40);
	m_editVolumeAlpha->setValidator(new QIntValidator(0, WIN_MANAGER->getVolumeAlphaMax(), this));
	connect(m_editVolumeAlpha, &QLineEdit::editingFinished, this, &Visualize3DTab::slot_OnVolumeAlphaOKClick);

	m_sliderVolumeAlpha = new QSlider(this);
	m_sliderVolumeAlpha->setOrientation(Qt::Horizontal);
	m_sliderVolumeAlpha->setStyleSheet(STYLE_MANAGER->sliderBarTab);
	m_sliderVolumeAlpha->setMinimum(0);
	m_sliderVolumeAlpha->setMaximum(WIN_MANAGER->getVolumeAlphaMax());
	m_sliderVolumeAlpha->setObjectName("SliderVolume");
	m_sliderVolumeAlpha->installEventFilter(this);
	/* 21.03.29 이상일 대리. 의미가 없는 듯하여 주석처리. 테스트 후 삭제할 것 */
	//connect(m_sliderVolumeAlpha, &QSlider::sliderPressed, this, &Visualize3DTab::slot_OnSliderPress);
	//connect(m_sliderVolumeAlpha, &QSlider::sliderReleased, this, &Visualize3DTab::slot_OnSliderRelease);
	connect(m_sliderVolumeAlpha, &QSlider::valueChanged, this, &Visualize3DTab::slot_OnVolumeAlphaChanged);

	groupHori1_1->addWidget(labelVolumeAlpha, 1);
	groupHori1_1->addWidget(m_editVolumeAlpha, 1);
	groupHori1_1->addWidget(m_sliderVolumeAlpha, 2);

	QLabel* labelLayerAlpha = new QLabel(QString("Layer Opacity"), this);
	labelLayerAlpha->setFixedWidth(100);

	m_editLayerAlpha = new QLineEdit(this);
	m_editLayerAlpha->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_editLayerAlpha->setText(tr("0"));
	m_editLayerAlpha->setAlignment(Qt::AlignCenter);
	m_editLayerAlpha->setFixedWidth(60);
	m_editLayerAlpha->setMinimumWidth(40);
	connect(m_editLayerAlpha, &QLineEdit::editingFinished, this, &Visualize3DTab::slot_OnLayerAlphaOKClick);

	m_sliderLayerAlpha = new QSlider(this);
	m_sliderLayerAlpha->setOrientation(Qt::Horizontal);
	m_sliderLayerAlpha->setStyleSheet(STYLE_MANAGER->sliderBarTab);
	m_sliderLayerAlpha->setMinimum(0);
	m_sliderLayerAlpha->setMaximum(200);
	m_sliderLayerAlpha->setObjectName("SliderLayer");
	m_sliderLayerAlpha->installEventFilter(this);
	/* 21.03.29 이상일 대리. 의미가 없는 듯하여 주석처리. 테스트 후 삭제할 것 */
	//connect(m_sliderLayerAlpha, &QSlider::sliderPressed, this, &Visualize3DTab::slot_OnSliderPress);
	//connect(m_sliderLayerAlpha, &QSlider::sliderReleased, this, &Visualize3DTab::slot_OnSliderRelease);
	connect(m_sliderLayerAlpha, &QSlider::valueChanged, this, &Visualize3DTab::slot_OnLayerAlphaChanged);

	groupHori1_2->addWidget(labelLayerAlpha, 1);
	groupHori1_2->addWidget(m_editLayerAlpha, 1);
	groupHori1_2->addWidget(m_sliderLayerAlpha, 2);

	QPushButton* alphaResetBtn = new QPushButton(QString("Reset"), this);
	alphaResetBtn->setStyleSheet(STYLE_MANAGER->buttonBehind);
	alphaResetBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	connect(alphaResetBtn, &QPushButton::released, this, &Visualize3DTab::slot_OnAlphaResetClick);

	groupVer2->addWidget(alphaResetBtn);

}


void Visualize3DTab::createShaderQualityUI()
{
	createFrameLine(m_nRow++);

	//QLabel *labelShader = new QLabel("Shader & Quality", this);
	QLabel* labelShader = m_pProductFactory->createWidget<QLabel>(MFL_Common_Rendering_3DVolumeViewer_3DViewShaderQualityLow, this);
	if (labelShader)
	{
		labelShader->setText(QString("Shader & Quality"));
		labelShader->setFixedWidth(100);
		addWidget(labelShader, m_nRow, 0, QMargins(0, 0, 15, 0));
	}

	//m_shaderCombo = new QComboBox(this);
	m_shaderCombo = m_pProductFactory->createWidget<QComboBox>(MFL_Common_Rendering_3DVolumeViewer_3DViewShaderQualityLow, this);
	if (m_shaderCombo)
	{
		m_shaderCombo->setObjectName("ComboShader");
		m_shaderCombo->addItem("INVISIBLE");
		m_shaderCombo->addItem("LOW");
		m_shaderCombo->addItem("MIDDLE");
		m_shaderCombo->setStyleSheet(STYLE_MANAGER->comboBoxTab);
		m_shaderCombo->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
		m_shaderCombo->installEventFilter(this);
		m_shaderCombo->setCurrentIndex(WIN_MANAGER->getShaderQuality());
		connect(m_shaderCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnShaderComboChanged(int)));

		addWidget(m_shaderCombo, m_nRow++, 0);
	}
}

void Visualize3DTab::createClipOptionUI()
{
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_3DVolumeViewer_ClipMode))
	{
		createFrameLine(m_nRow++);
	}

	m_clipModeBtn = m_pProductFactory->createWidget<QPushButton>(MFL_Common_Rendering_3DVolumeViewer_ClipMode, this);
	if (m_clipModeBtn)
	{
		m_clipModeBtn->setText(QString("Clip Mode"));
		m_clipModeBtn->setCheckable(true);
		m_clipModeBtn->setChecked(false);
		m_clipModeBtn->setStyleSheet(STYLE_MANAGER->buttonChecked);
		connect(m_clipModeBtn, &QPushButton::clicked, this, &Visualize3DTab::slot_OnClip);

		addWidget(m_clipModeBtn, m_nRow++, 1);
	}

	//m_boxClip = m_pProductFactory->createWidget<QWidget>(MFL_Common_Rendering_3DVolumeViewer_ClipMode, this);
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_3DVolumeViewer_ClipMode)
		|| m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Segmentation_Manualdrawingtools_3DViewManualsplitfunction))
	{
		m_boxClip = new QWidget(this);
	}

	if (m_boxClip)
	{
		QBoxLayout* groupVer1 = new QVBoxLayout;
		m_boxClip->setLayout(groupVer1);

		QLabel* pLabelClipOption = new QLabel(QString("Clipping option"), this);
		groupVer1->addWidget(pLabelClipOption);

		QCheckBox* chkVal = new QCheckBox(this);
		chkVal->setText("Exclude Volume/Layer");
		chkVal->setCheckable(true);
		chkVal->setChecked(!WIN_MANAGER->getCullingMaskVolume());
		chkVal->setEnabled(false);
		chkVal->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		connect(chkVal, &QCheckBox::clicked, this, &Visualize3DTab::slot_OnCullingVolumeChecked);
		groupVer1->addWidget(chkVal);

		chkVal = new QCheckBox(this);
		chkVal->setText("Exclude Surface");
		chkVal->setCheckable(true);
		chkVal->setChecked(!WIN_MANAGER->getCullingMesh());
		chkVal->setEnabled(false);
		chkVal->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		connect(chkVal, &QCheckBox::clicked, this, &Visualize3DTab::slot_OnCullingSufaceChecked);
		groupVer1->addWidget(chkVal);

		chkVal = new QCheckBox(this);
		chkVal->setText("Show Clipped Image");
		chkVal->setCheckable(true);
		chkVal->setChecked(WIN_MANAGER->getClip2DPlane());
		chkVal->setEnabled(false);
		chkVal->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		connect(chkVal, &QCheckBox::clicked, this, &Visualize3DTab::slot_OnClip2DPlane);
		groupVer1->addWidget(chkVal);

		addWidget(m_boxClip, m_nRow++, 1);
	}
}

void Visualize3DTab::createMixModeUI()
{
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_3DVolumeViewer_MixMode))
	{
		createFrameLine(m_nRow++);
	}

	//QGroupBox *boxGroup = new QGroupBox(this);
	QWidget* pMixModeWidget = m_pProductFactory->createWidget<QWidget>(MFL_Common_Rendering_3DVolumeViewer_MixMode, this);
	if (pMixModeWidget)
	{
		QBoxLayout* LayGroup = new QHBoxLayout;
		pMixModeWidget->setLayout(LayGroup);

		QLabel* pLabelMix = new QLabel(this);
		pLabelMix->setText(QString("Mix Mode"));
		pLabelMix->setFixedWidth(70);

		m_maskGroup = new QButtonGroup(this);

		QRadioButton* radioAll = new QRadioButton(this);
		radioAll->setText("NONE");
		radioAll->setCheckable(true);
		radioAll->setChecked(true);
		radioAll->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		QRadioButton* radioLayer = new QRadioButton(this);
		radioLayer->setText(QString("Volume %1 Layers").arg(QString(QChar(0x2229))));
		radioLayer->setCheckable(true);
		radioLayer->setChecked(false);
		radioLayer->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_maskGroup->addButton(radioAll, 0);
		m_maskGroup->addButton(radioLayer, 1);

		connect(m_maskGroup, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnChangeMode(int)));

		LayGroup->addWidget(pLabelMix);
		LayGroup->addWidget(radioAll);
		LayGroup->addWidget(radioLayer);

		addWidget(pMixModeWidget, m_nRow, 1);
	}
}

void Visualize3DTab::createBottomCheckBoxUI()
{
	QCheckBox* chkVal = new QCheckBox(this);
	chkVal->setText("Visible Annotations");
	chkVal->setCheckable(true);
	chkVal->setChecked(WIN_MANAGER->get3DAnnoState());
	chkVal->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	connect(chkVal, &QCheckBox::clicked, this, &Visualize3DTab::slot_OnAnnoStateChanged);
	addWidget(chkVal, m_nRow++, 1);

	chkVal = new QCheckBox(this);
	chkVal->setText("Visible Surface");
	chkVal->setCheckable(true);
	chkVal->setChecked(WIN_MANAGER->getVisibleMesh());
	chkVal->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	connect(chkVal, &QCheckBox::clicked, this, &Visualize3DTab::slot_OnVisiableSufaceChecked);
	addWidget(chkVal, m_nRow++, 1);

	chkVal = new QCheckBox(this);
	chkVal->setText("Backface Culling");
	chkVal->setCheckable(true);
	chkVal->setChecked(WIN_MANAGER->getBackfaceCullingMode());
	chkVal->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	connect(chkVal, &QCheckBox::clicked, this, &Visualize3DTab::slot_OnBackfaceCulling);
	addWidget(chkVal, m_nRow++, 1);
}

void Visualize3DTab::createVRUI()
{
	if (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_VR_MEDIPVR))
	{
		createFrameLine(m_nRow++);
	}

	//QGroupBox *grpBox = new QGroupBox(this);
	QGroupBox* grpBox = m_pProductFactory->createWidget<QGroupBox>(MFL_Common_VR_MEDIPVR, this);
	if (grpBox)
	{
		grpBox->setTitle("VR ");
		QVBoxLayout* grpMain = new QVBoxLayout;
		grpBox->setLayout(grpMain);
		QBoxLayout* grpSub = new QHBoxLayout;
		grpMain->addLayout(grpSub);

		QLabel* label = new QLabel(this);
		label->setText("Control DPI");
		grpSub->addWidget(label);

		m_editDPI = new QLineEdit(this);
		m_editDPI->setValidator(new QDoubleValidator(1.0, 10.0, 2, this));
		m_editDPI->setText(QString("5.0"));
		m_editDPI->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_editDPI->setObjectName(QString("EditDPI"));

		connect(m_editDPI, &QLineEdit::editingFinished, this, &Visualize3DTab::slot_OnDPIChanged);
		connect(m_editDPI, &QLineEdit::returnPressed, this, &Visualize3DTab::slot_OnDPIChanged);

		grpSub->addWidget(m_editDPI);

		QWidget* emtyBox = new QWidget(this);
		emtyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		grpSub->addWidget(emtyBox);

		grpSub = new QVBoxLayout;
		grpMain->addLayout(grpSub);

		m_sliderDPI = new QSlider(Qt::Horizontal, this);
		m_sliderDPI->setRange(10, 100);
		m_sliderDPI->setValue(50);
		m_sliderDPI->setStyleSheet(STYLE_MANAGER->sliderBarTab);
		m_sliderDPI->setObjectName("SliderDPI");

		connect(m_sliderDPI, &QSlider::valueChanged, this, &Visualize3DTab::slot_OnDPIMoved);
		connect(m_sliderDPI, &QSlider::sliderMoved, this, &Visualize3DTab::slot_OnDPIMoved);

		grpSub->addWidget(m_sliderDPI);

		m_btnVR = new QPushButton(this);
		m_btnVR->setCheckable(true);
		m_btnVR->setChecked(false);
		m_btnVR->setText(QString("PLAY"));
		m_btnVR->setStyleSheet(STYLE_MANAGER->buttonChecked);
		if (g_Renderer->isAvailableVolumeRender() == false)
			m_btnVR->setEnabled(false);
		connect(m_btnVR, &QPushButton::clicked, this, &Visualize3DTab::slot_OnVR);

		grpSub->addWidget(m_btnVR, 0, Qt::AlignRight);

		addWidget(grpBox, m_nRow++);
	}
}

void Visualize3DTab::slot_OnVolumeAlphaChanged(int pos)
{
	if (DATA_CONTEXT->volume_data.isValidate() == false)
	{
		return;
	}

	m_editVolumeAlpha->setText(QString::number(pos));
	WIN_MANAGER->setVolumeAlpha(pos);
	WIN_MANAGER->renderLater_GridView();
}

void Visualize3DTab::slot_OnVolumeAlphaOKClick()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (m_editVolumeAlpha)
	{
		int val = max(0, min(m_editVolumeAlpha->text().toInt(), WIN_MANAGER->getVolumeAlphaMax()));
		m_editVolumeAlpha->setText(QString::number(val));
		m_sliderVolumeAlpha->setValue(val);
		WIN_MANAGER->setVolumeAlpha(val);
		WIN_MANAGER->renderLater_GridView();
	}
}

void Visualize3DTab::slot_OnLayerAlphaChanged(int pos)
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	m_editLayerAlpha->setText(QString::number(pos));
	WIN_MANAGER->setLayerAlpha(pos);
	WIN_MANAGER->renderLater_GridView();
}

void Visualize3DTab::slot_OnLayerAlphaOKClick()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (m_editLayerAlpha)
	{
		int val = max(0, min(m_editLayerAlpha->text().toInt(), 200));
		m_editLayerAlpha->setText(QString::number(val));
		m_sliderLayerAlpha->setValue(val);
		WIN_MANAGER->setLayerAlpha(val);
		WIN_MANAGER->renderLater_GridView();
	}
}

void Visualize3DTab::slot_OnDPIChanged()
{
	double tmpVal = m_editDPI->text().toDouble();

	if (tmpVal < 1.0f)
		tmpVal = 1.0f;
	else if (tmpVal > 10.0f)
		tmpVal = 10.0f;

	WIN_MANAGER->setDPIValue(tmpVal);

	tmpVal /= 10.0f;
	tmpVal *= 100;


	m_sliderDPI->blockSignals(true);
	m_sliderDPI->setValue(tmpVal);
	m_sliderDPI->blockSignals(false);

}

void Visualize3DTab::slot_OnDPIMoved(int pos)
{
	double tmpVal = pos;

	tmpVal /= 10.0f;

	if (m_editDPI->text() != QString::number(tmpVal))
	{
		m_editDPI->blockSignals(true);
		m_editDPI->setText(QString::number(tmpVal));
		m_editDPI->blockSignals(false);
	}

	WIN_MANAGER->setDPIValue(tmpVal);
}

void Visualize3DTab::slot_OnClip()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false)
	{
		m_clipModeBtn->setChecked(false);
		return;
	}
	bool chk = m_clipModeBtn->isChecked();
	if (chk)
	{
		WIN_MANAGER->clip2DOff();

		if (WIN_MANAGER->getWorkMode() == WORK_3D_PLANE_SPLIT)
			WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
		else if (WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_3D_PLANE_SPLIT)
		{
			if (WIN_MANAGER->mainAnalWidget)
				WIN_MANAGER->mainAnalWidget->setWorkMode(ANAL_WORK_NONE);
		}
	}

	setClipOpt();
}

void Visualize3DTab::slot_OnSave()
{
	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	int index = m_presetCombo->currentIndex();
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

void Visualize3DTab::slot_OnDel()
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

void Visualize3DTab::slot_OnCullingVolumeChecked(bool check)
{
	WIN_MANAGER->setMoveFocus(true);
	WIN_MANAGER->setCullingMaskVolume(!check);
}

void Visualize3DTab::slot_OnCullingSufaceChecked(bool check)
{
	WIN_MANAGER->setMoveFocus(true);

	WIN_MANAGER->setCullingMesh(!check);
}

void Visualize3DTab::slot_OnClip2DPlane(bool check)
{
	WIN_MANAGER->setMoveFocus(true);
	WIN_MANAGER->setClip2DPlane(check);
}

void Visualize3DTab::slot_OnVisiableSufaceChecked(bool check)
{
	WIN_MANAGER->setMoveFocus(true);
	WIN_MANAGER->setVisibleMesh(check);
}

void Visualize3DTab::slot_OnAnnoStateChanged(bool chk)
{
	WIN_MANAGER->set3DAnnoState(chk);
}

void Visualize3DTab::slot_OnBackfaceCulling(bool check)
{
	WIN_MANAGER->setBackfaceCullingMode(check);
	WIN_MANAGER->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_backfaceCulling, QString::number(-1 + check));
}

void Visualize3DTab::InAreaWidth(int& nWidth)
{
	nWidth = mip::math::Max(nWidth, m_slideWidth->maximum()) == nWidth ? m_slideWidth->maximum() :
		mip::math::Min(nWidth, m_slideWidth->minimum()) == nWidth ? m_slideWidth->minimum() : nWidth;
}

void Visualize3DTab::slot_OnChangeMode(int type)
{
	WIN_MANAGER->setMoveFocus(true);

	if (0 == type)//All
	{
		if (m_maskGroup)
			m_maskGroup->button(1)->setChecked(false);
		WIN_MANAGER->setMaskingmode(false);
	}
	else
	{
		if (m_maskGroup)
			m_maskGroup->button(0)->setChecked(false);
		WIN_MANAGER->setMaskingmode(true);
	}

}

bool Visualize3DTab::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == NULL)
	{
		return false;
	}

	if (watched->objectName().contains("Slider"))
	{
		/*
		21.03.29 이상일 대리
		- 의도가 불분명하고 의미가 없어서 주석처리함.
		- 테스트 완료되면 삭제할 것
		*/
		//const QEvent::Type type = event->type();
		//if (type == QEvent::Scroll ||
		//	type == QEvent::Wheel)
		//{
		//	return true;
		//}
		//else if (type == QEvent::MouseButtonPress)
		//{
		//	WIN_MANAGER->useSkipRender(0);
		//	return true;
		//}
		//else if (type == QEvent::MouseButtonRelease && !m_bPress)
		//{
		//	slot_OnSliderRelease();
		//	return true;
		//}
	}
	else if (watched->objectName().contains("Combo"))
	{
		if (event->type() == QEvent::Wheel)
			return true;
	}
	else if (watched->objectName().contains("Edit"))
	{
		if (event->type() == QEvent::FocusIn)
			WIN_MANAGER->setMoveFocus(false);
	}

	return QWidget::eventFilter(watched, event);
}

void Visualize3DTab::InAreaLevel(int& nLevel)
{
	nLevel = mip::math::Max(nLevel, m_slideLevel->maximum()) == nLevel ? m_slideLevel->maximum() :
		mip::math::Min(nLevel, m_slideLevel->minimum()) == nLevel ? m_slideLevel->minimum() : nLevel;
}


void Visualize3DTab::setPreset(SLICE_PRESET type, bool reset, int cusType)
{
	int nWidth, nLevel;
	bool res;
	QString resStr;

	if (m_slideWidth == NULL) return;

	if (!(SP_DEFAULT <= type && type <= (SP_COUNT - 1))) type = SP_DEFAULT;

	m_presetCombo->blockSignals(true);
	if (-1 >= cusType)
	{
		m_presetCombo->setCurrentIndex((int)type);
		m_cusPreset = -1;
		m_prePreset = m_preset;
		m_preset = type;
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

	}
	else
	{
		if (!WIN_MANAGER->IsLicensePass())
		{
			m_cusPreset = cusType = -1;
			m_presetCombo->setCurrentIndex((int)m_preset);
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

			m_cusPreset = cusType;
			m_presetCombo->setCurrentIndex(m_cusPreset + SP_COUNT);
		}
	}

	m_presetCombo->blockSignals(false);

	m_slideLevel->setMinimum(DATA_CONTEXT->volume_data.getHuMin());
	m_slideLevel->setMaximum(DATA_CONTEXT->volume_data.getHuMax());
	m_slideWidth->setMinimum(1);
	m_slideWidth->setMaximum(abs(DATA_CONTEXT->volume_data.getHuMax() -
		DATA_CONTEXT->volume_data.getHuMin()));

	res = WIN_MANAGER->getConfigWL(QString("VOLUME_WL%1").arg(cusType <= -1 ? QString::number(m_preset) : ""),
		nWidth, nLevel, cusType);

	if (!res)
	{
		if (-1 >= cusType)
		{
			switch (m_preset)
			{
			case SP_RECOMMAND_1:
				nWidth = 960;
				nLevel = 229;
				break;
			case SP_RECOMMAND_2:
				nWidth = 230;
				nLevel = 105;
				break;
			case SP_LUNG:
				nWidth = 1500;
				nLevel = -600;
				break;
			case SP_BONE:
				nWidth = 200;
				nLevel = 665;
				break;
			case SP_MEDIASTINAL:
				nWidth = 180;
				nLevel = 100;
				break;
			case SP_BRAIN:
				nWidth = 30;
				nLevel = 230;
				break;
			case SP_ABD:
				nWidth = 200;
				nLevel = 345;
				break;
			case SP_LIVER:
				nWidth = 40;
				nLevel = 60;
				break;
			case SP_KIDNEY:
				nWidth = 80;
				nLevel = 245;
				break;
			case SP_URINARY_TRACK_BLADDER:
				nWidth = 300;
				nLevel = 320;
				break;
			case SP_CUSTOM:
				nWidth = 300;
				nLevel = 245;
				break;
			case SP_DEFAULT:
			default:
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

	InAreaWidth(nWidth);
	InAreaLevel(nLevel);

	WIN_MANAGER->setVolumeWidthLevel(nWidth, nLevel);
}

void Visualize3DTab::resetSlide()
{
	int nWidth, nLevel;

	SLICE_PRESET prePreset = m_preset;
	nWidth = WIN_MANAGER->getVolumeWidth();
	nLevel = WIN_MANAGER->getVolumeLevel();

	setPreset(SP_DEFAULT);

	if ((prePreset != m_preset) || (nWidth != WIN_MANAGER->getVolumeWidth()) || (nLevel != WIN_MANAGER->getVolumeLevel()))
		ACTION_MANAGER->action_VolumePreset(nLevel, nWidth, prePreset, (SLICE_PRESET)SP_DEFAULT);
	WIN_MANAGER->renderLater_GridView(true);
}

void Visualize3DTab::setWidth(int val)
{
	InAreaWidth(val);

	if (m_slideWidth)
	{
		m_slideWidth->blockSignals(true);
		m_textWidth->blockSignals(true);
		m_textWidth->setText(QString::number(val));
		m_slideWidth->setValue(val);
		m_slideWidth->blockSignals(false);
		m_textWidth->blockSignals(false);
	}
}

void Visualize3DTab::setLevel(int val)
{
	InAreaLevel(val);

	if (m_slideLevel)
	{
		m_slideLevel->blockSignals(true);
		m_textLevel->blockSignals(true);
		m_textLevel->setText(QString::number(val));
		m_slideLevel->setValue(val);
		m_slideLevel->blockSignals(false);
		m_textLevel->blockSignals(false);
	}
}

void Visualize3DTab::setVolumeAlpha(int val)
{
	if (m_sliderVolumeAlpha)
	{
		m_editVolumeAlpha->setText(QString::number(val));
		m_sliderVolumeAlpha->setValue(val);
	}
}

void Visualize3DTab::setLayerAlpha(int val)
{
	if (m_sliderLayerAlpha)
	{
		m_editLayerAlpha->setText(QString::number(val));
		m_sliderLayerAlpha->setValue(val);
	}
}

bool Visualize3DTab::isClipMode()
{
	if (m_clipModeBtn)
	{
		return m_clipModeBtn->isChecked();
	}
	return false;
}

void Visualize3DTab::clipOnOff(bool val)
{
	if (m_clipModeBtn != nullptr && m_clipModeBtn->isChecked() != val)
	{
		m_clipModeBtn->setChecked(val);
		slot_OnClip();
	}
}

void Visualize3DTab::setDisableShader(bool set)
{
	if (!m_shaderCombo)
		return;
	if (set)
	{
		m_shaderCombo->setCurrentIndex(0);
		m_shaderCombo->setDisabled(true);
	}
	else
	{
		m_shaderCombo->setDisabled(false);
	}
}

void Visualize3DTab::OnVRMode()
{
	if (m_btnVR)
	{
		m_btnVR->setChecked(!m_btnVR->isChecked());
		slot_OnVR();
	}
}

void Visualize3DTab::addCustomPreset(int index, bool isImport, bool bAppDataPath)
{
	m_presetCombo->addItem(WIN_MANAGER->getPresetName(index));

	bAppDataPath == true ? setItemData_presetCombo(eCFLAppDataLocalPath) : setItemData_presetCombo(eCFLApplicationPah);

	if (!isImport)
	{
		WIN_MANAGER->setConfigWL(QString("VOLUME_WL"), m_textWidth->text().toInt(),
			m_textLevel->text().toInt(), index);
	}

}

void Visualize3DTab::delCustomPreset(int index)
{
	m_presetCombo->setCurrentIndex(0); //sp_default set

	m_presetCombo->removeItem(index + SP_COUNT);
}

void Visualize3DTab::setClipOpt()
{
	bool chk = false;
	if (m_clipModeBtn)
		chk = m_clipModeBtn->isChecked();

	if (!chk)
	{
		MAINTAB_TYPE _type = WIN_MANAGER->mainTabType;

		if (MAINTAB_SEGMENTATION == _type)
			chk = WIN_MANAGER->getWorkMode() == WORK_3D_PLANE_SPLIT;
		else if (MAINTAB_MEASUREMENT == _type)
			chk = WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_3D_PLANE_SPLIT;
	}

	if (m_boxClip)
	{
		// 		QObjectList ListChild = m_boxClip->children();
		// 		for (int i = 0; i < ListChild.size(); ++i)
		// 		{
		// 			QWidget *child = (QWidget*)ListChild.at(i);
		// 			child->setEnabled(chk);
		// 		}
		// 
		// 		int a = 0;
		QLayout* boxLayout = m_boxClip->layout();

		if (boxLayout)
		{
			for (int i = 0; i < boxLayout->count(); i++)
			{
				QWidget* child = boxLayout->itemAt(i)->widget();
				child->setEnabled(chk);
			}
		}
	}

	WIN_MANAGER->renderLater_3DView();
}

void Visualize3DTab::slot_OnLevelChanged(int pos)
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (m_slideLevel->isSliderDown())
	{
		ACTION_MANAGER->action_Volume_Level(pos);
	}
	else
	{
		m_slideLevel->blockSignals(true);
		m_slideLevel->setValue(m_textLevel->text().toInt());
		m_slideLevel->blockSignals(false);
	}
}

void Visualize3DTab::slot_OnWidthChanged(int pos)
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (m_slideWidth->isSliderDown())
	{
		ACTION_MANAGER->action_Volume_Width(pos);
	}
	else
	{
		m_slideWidth->blockSignals(true);
		m_slideWidth->setValue(m_textWidth->text().toInt());
		m_slideWidth->blockSignals(false);
	}
}

void Visualize3DTab::slot_OnSliderPress()
{
	WIN_MANAGER->setMoveFocus(true);

	if (!m_bPress)	m_bPress = true;
}

void Visualize3DTab::slot_OnSliderRelease()
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	WIN_MANAGER->useSkipRender(0, false);

	WIN_MANAGER->renderLater_3DView();


	if (m_bPress)	m_bPress = false;
}

void Visualize3DTab::slot_OnWidthLevelResetClick()
{
	WIN_MANAGER->setMoveFocus(true);

	setPreset(m_preset, true, m_cusPreset);
	WIN_MANAGER->renderLater_GridView(true);
}

void Visualize3DTab::slot_OnAlphaResetClick()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	const int nVolumeAlpha = 100;
	const int nLayerAlpha = 200;

	WIN_MANAGER->setVolumeAlpha(nVolumeAlpha);
	setVolumeAlpha(nVolumeAlpha);

	WIN_MANAGER->setLayerAlpha(nLayerAlpha);
	setLayerAlpha(nLayerAlpha);

	WIN_MANAGER->renderLater_GridView(true);
}

void Visualize3DTab::slot_OnComboChanged(int index)
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	int preLevel = WIN_MANAGER->getVolumeLevel();
	int preWidth = WIN_MANAGER->getVolumeWidth();
	m_prePreset = m_preset;
	ACTION_MANAGER->action_VolumePreset(preLevel, preWidth, m_prePreset, (SLICE_PRESET)(index % SP_COUNT), index - SP_COUNT);
}

void Visualize3DTab::slot_OnShaderComboChanged(int index)
{
	WIN_MANAGER->setMoveFocus(true);

	if (m_btnVR != nullptr && m_btnVR->isChecked() && index == 0)
	{
		m_shaderCombo->setCurrentIndex(1);
		return;
	}

	//INVISIBLE 초기 Volume Alpha값 MAX로 설정
	if (index == 0)
	{
		WIN_MANAGER->resetVolumeAlpha();
	}

	int preIndex = WIN_MANAGER->getShaderQuality();
	WIN_MANAGER->setShaderQuality((SHADER_QUALITY)index);

	if (index == 0 || preIndex == 0)
		WIN_MANAGER->setLowSpecOptions();

	WIN_MANAGER->updatePlaneData_all();
	//INVISIBLE
	WIN_MANAGER->invisibleModeUpdate(WT_AXIAL);
	WIN_MANAGER->invisibleModeUpdate(WT_CORONAL);
	WIN_MANAGER->invisibleModeUpdate(WT_SAGITTAL);

	WIN_MANAGER->renderLater_GridView();
}

void Visualize3DTab::slot_OnHighLight()
{
#ifdef DEV_VER

	if (m_btnHighLight != 0)
	{
		WIN_MANAGER->bHighlight = !WIN_MANAGER->bHighlight;

		if (!WIN_MANAGER->bHighlight)
			m_btnHighLight->setText("High light Off");
		else
			m_btnHighLight->setText("High light On");

		if (DATA_CONTEXT->volume_data.isValidate())
			WIN_MANAGER->renderLater_GridView(true);
	}
#endif

}
void Visualize3DTab::slot_OnVR()
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false)
	{
		m_btnVR->setChecked(false);
		return;
	}

	if (WIN_MANAGER->getShaderQuality() == SQ_INVISIBLE)
	{
		m_shaderCombo->setCurrentIndex(1);
		WIN_MANAGER->setShaderQuality(SQ_LOW);
	}

	WIN_MANAGER->viveOn();
}

void Visualize3DTab::slot_OnMeshTest()
{
}

void Visualize3DTab::slot_OnWidthLevelOKClick()
{
	WIN_MANAGER->setMoveFocus(true);

	if (DATA_CONTEXT->volume_data.isValidate() == false)
	{
		return;
	}

	int nWidth, nLevel;

	nWidth = WIN_MANAGER->getVolumeWidth();
	nLevel = WIN_MANAGER->getVolumeLevel();

	if (nLevel != m_textLevel->text().toInt())
	{
		nLevel = m_textLevel->text().toInt();
		ACTION_MANAGER->action_Volume_Level(nLevel);
	}

	if (nWidth != m_textWidth->text().toInt())
	{
		nWidth = m_textWidth->text().toInt();
		ACTION_MANAGER->action_Volume_Width(nWidth);
	}

	WIN_MANAGER->renderLater_GridView();
}

void Visualize3DTab::deepcatch3DTabSetting(DEEPCATCH_REPORT* pReport, bool bRestore)
{
	if (bRestore)	// 복원
	{
		if (pReport->tempInfo.presetComboIndex != -1)
			m_presetCombo->setCurrentIndex(pReport->tempInfo.presetComboIndex);
		if (pReport->tempInfo.shaderComboIndex != -1)
			m_shaderCombo->setCurrentIndex(pReport->tempInfo.shaderComboIndex);

		if (pReport->tempInfo.slideLevelValue != -1)
		{
			setLevel(pReport->tempInfo.slideLevelValue);
			ACTION_MANAGER->action_Volume_Level(pReport->tempInfo.slideLevelValue);
		}

		if (pReport->tempInfo.slideWidthValue != -1)
		{
			setWidth(pReport->tempInfo.slideWidthValue);
			ACTION_MANAGER->action_Volume_Width(pReport->tempInfo.slideWidthValue);
		}

		if (pReport->tempInfo.sliderVolumeAlpha != -1)
		{
			setVolumeAlpha(pReport->tempInfo.sliderVolumeAlpha);
			WIN_MANAGER->setVolumeAlpha(pReport->tempInfo.sliderVolumeAlpha);
		}

		if (pReport->tempInfo.sliderLayerAlpha != -1)
		{
			setLayerAlpha(pReport->tempInfo.sliderLayerAlpha);
			WIN_MANAGER->setLayerAlpha(pReport->tempInfo.sliderLayerAlpha);
		}
	}
	else // 캡쳐 전 3d tab setting
	{

		pReport->tempInfo.presetComboIndex = m_presetCombo->currentIndex();
		m_presetCombo->setCurrentIndex(SP_DEFAULT);
		pReport->tempInfo.shaderComboIndex = m_shaderCombo->currentIndex();
		m_shaderCombo->setCurrentIndex(2);

		pReport->tempInfo.slideLevelValue = m_slideLevel->value();
		setLevel(m_slideLevel->maximum());
		ACTION_MANAGER->action_Volume_Level(m_slideLevel->maximum());

		pReport->tempInfo.slideWidthValue = m_slideWidth->value();
		setWidth(m_slideWidth->maximum());
		ACTION_MANAGER->action_Volume_Width(m_slideWidth->maximum());

		//	setLevel(DATA_CONTEXT->volume_data.getHuMax());
		//	ACTION_MANAGER->action_Volume_Level(DATA_CONTEXT->volume_data.getHuMax());
		//	setWidth(abs(DATA_CONTEXT->volume_data.getHuMax() - DATA_CONTEXT->volume_data.getHuMin()));
		//	ACTION_MANAGER->action_Volume_Width(abs(DATA_CONTEXT->volume_data.getHuMax() - DATA_CONTEXT->volume_data.getHuMin()));

		pReport->tempInfo.sliderVolumeAlpha = m_sliderVolumeAlpha->value();
		//		setVolumeAlpha(140);
		//		WIN_MANAGER->setVolumeAlpha(140);
		setVolumeAlpha(0);
		WIN_MANAGER->setVolumeAlpha(0);

		pReport->tempInfo.sliderLayerAlpha = m_sliderLayerAlpha->value();
		setLayerAlpha(120);
		WIN_MANAGER->setLayerAlpha(120);
	}
	// 업데이트
	WIN_MANAGER->renderLater_GridView();
}

void Visualize3DTab::setItemData_presetCombo(eConfigFileLocation eConfigFileLoc)
{
#ifdef DEV_USE_APPDATA_PATH
	m_presetCombo->setItemData(m_presetCombo->count() - 1, eConfigFileLoc, Qt::UserRole);
#endif
}

void Visualize3DTab::Show_PresetEditDeleteBtn(bool bShow)
{
	if (m_presetDel)
		bShow ? m_presetDel->show() : m_presetDel->hide();
	if (m_presetEdit)
		bShow ? m_presetEdit->show() : m_presetEdit->hide();
}


