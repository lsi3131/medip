#include "stdafx.h"
#include "DrawcutTab.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "StyleManager.h"
#include "LicenseManager.h"
#include "ProductManager.h"
#include "ResourceManager.h"
#include "ShortcutManager.h"
#include "Main/MainSegmentWidget.h"
#include "Main/MainAnalWidget.h"
#include "ROITab2.h"
#include "Tabwindow.h"

#include "DataContext.h"

DrawcutTab::DrawcutTab(QWidget* parent /*= NULL*/)
	:CollapseWidget(QString(), parent)
{
	int nRow = 0;

	addWidget(NULL, nRow++);

	QBoxLayout* layMain = getLayout(nRow - 1);

	QVBoxLayout* laySeed = new QVBoxLayout;
	QVBoxLayout* layApply = new QVBoxLayout;

	QHBoxLayout* layList = new QHBoxLayout;
	QHBoxLayout* layVoxel = new QHBoxLayout;
	QHBoxLayout* layLambda = new QHBoxLayout;
	QHBoxLayout* laySlider = new QHBoxLayout;
	QBoxLayout* layCursor = new QVBoxLayout;
	QHBoxLayout* layCheck = new QHBoxLayout;

	layMain->addLayout(laySeed, 1);
	layMain->addLayout(layApply, 1);
	laySeed->addLayout(layList);
	laySeed->addLayout(layVoxel);
	layApply->addLayout(layLambda);
	layApply->addLayout(laySlider);
	layApply->addLayout(layCheck);
	layApply->addLayout(layCursor);



	layMain->setContentsMargins(0, 0, 0, 0);
	laySeed->setContentsMargins(0, 0, 0, 0);
	layApply->setContentsMargins(0, 0, 0, 0);
	layList->setContentsMargins(0, 0, 0, 0);
	layVoxel->setContentsMargins(0, 0, 0, 0);
	layLambda->setContentsMargins(0, 0, 0, 0);
	laySlider->setContentsMargins(0, 0, 0, 0);
	layCursor->setContentsMargins(0, 0, 0, 0);
	layCheck->setContentsMargins(0, 0, 0, 0);

	//m_tabSet = PRODUCT_FACTORY->createWidget<AISetTab>(MS_COLLAPS_AI_SETTING_TAB, this); //new AISetTab(this);

	//m_actClear = new QAction(STRING_MANAGER->getString(STR_CLR_ROI), this);
	m_actClear = PRODUCT_FACTORY->createObject<QAction>(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut, this);
	if (m_actClear)
	{
		m_actClear->setText(STRING_MANAGER->getString(STR_CLR_ROI));
		m_actClear->setParent(this);
		connect(m_actClear, &QAction::triggered, this, &DrawcutTab::slot_OnClear);
	}

	QFontMetrics fm(this->font());
	QRect rect = fm.boundingRect("Backseed");

	//m_listSeed = new QTreeWidget(this);
	m_listSeed = PRODUCT_FACTORY->createWidget<QTreeWidget>(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut, this);
	if (m_listSeed)
	{
		m_listSeed->setColumnCount(3);
		m_listSeed->setContentsMargins(0, 5, 0, 5);
		m_listSeed->setHeaderHidden(true);
		m_listSeed->setColumnWidth(0, 20);
		m_listSeed->setColumnWidth(1, 20);
		m_listSeed->setStyleSheet(STYLE_MANAGER->treeBasicList);
		m_listSeed->setRootIsDecorated(false);
		m_listSeed->setContextMenuPolicy(Qt::CustomContextMenu);
		m_listSeed->setSelectionMode(QAbstractItemView::SingleSelection);

		connect(m_listSeed, &QTreeWidget::currentItemChanged, this, &DrawcutTab::slot_OnSelectChange);
		connect(m_listSeed, &QTreeWidget::itemClicked, this, &DrawcutTab::slot_OnColumnClicked);
		connect(m_listSeed, &QTreeWidget::itemDoubleClicked, this, &DrawcutTab::slot_OnColumnDoubleClicked);
		connect(m_listSeed, &QTreeWidget::customContextMenuRequested, this, &DrawcutTab::slot_OnContext);
	}

	//m_labelSeed = new QLabel(this);
	m_labelSeed = PRODUCT_FACTORY->createWidget<QLabel>(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut, this);
	if (m_labelSeed)
		m_labelSeed->setText("Voxel : 0 (0.00%)");

	//m_btnFitbox = new QPushButton(this);
	m_btnFitbox = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut, this);
	if (m_btnFitbox)
	{
		m_btnFitbox->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FIT_COORD));
		m_btnFitbox->setIconSize(QSize(34, 34));
		m_btnFitbox->setFixedSize(QSize(34, 34));
		m_btnFitbox->setObjectName("ButtonFit");
		m_btnFitbox->setMouseTracking(true);
		m_btnFitbox->installEventFilter(this);
		m_btnFitbox->setStyleSheet("color: black;");
		m_btnFitbox->setToolTip("Fit to Seed region proposal(Working region)");
		connect(m_btnFitbox, &QPushButton::clicked, this, &DrawcutTab::slot_OnFitBoundingBox);
	}

	//QWidget* emptyBox0 = new QWidget(this);
	QWidget* emptyBox0 = PRODUCT_FACTORY->createWidget<QWidget>(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut, this);
	if (emptyBox0) emptyBox0->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	if (m_listSeed) layList->addWidget(m_listSeed);
	if (m_btnFitbox) layVoxel->addWidget(m_btnFitbox);
	if (emptyBox0) layVoxel->addWidget(emptyBox0);
	if (m_labelSeed) layVoxel->addWidget(m_labelSeed);

	//QLabel *labelLam = new QLabel(this);
	QLabel* labelLam = PRODUCT_FACTORY->createWidget<QLabel>(MFL_Common_Segmentation_Semi_Autosegmentation_adjustlambda, this);
	if (labelLam) labelLam->setText("Lambda");

	//m_textLambda = new QLineEdit(this);	
	m_textLambda = PRODUCT_FACTORY->createWidget<QLineEdit>(MFL_Common_Segmentation_Semi_Autosegmentation_adjustlambda, this);
	if (m_textLambda)
	{
		m_textLambda->setValidator(new QDoubleValidator(1.0, 1000.0, 0, this));
		m_textLambda->setText(QString::number(50.0));
		m_textLambda->setStyleSheet(STYLE_MANAGER->editBoxTab);
		m_textLambda->setObjectName("EditLambda");
		m_textLambda->setMouseTracking(true);
		m_textLambda->installEventFilter(this);
		connect(m_textLambda, &QLineEdit::textChanged, this, &DrawcutTab::slot_OnTextChanged);
		connect(m_textLambda, &QLineEdit::returnPressed, this, &DrawcutTab::slot_OnTextEditFinished);
		connect(m_textLambda, &QLineEdit::editingFinished, this, &DrawcutTab::slot_OnTextEditFinished);
	}

	//QPushButton *btnReset = new QPushButton(this);
	QPushButton* btnReset = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Semi_Autosegmentation_adjustlambda, this);
	if (btnReset)
	{
		btnReset->setText(STRING_MANAGER->getString(STR_RESET));
		btnReset->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(btnReset, &QPushButton::clicked, this, &DrawcutTab::slot_OnReset);
	}

	if (labelLam) layLambda->addWidget(labelLam);
	if (m_textLambda) layLambda->addWidget(m_textLambda, 1);
	if (btnReset) layLambda->addWidget(btnReset, 1);

	//m_slideLambda = new QSlider(Qt::Horizontal, this);
	m_slideLambda = PRODUCT_FACTORY->createWidget<QSlider>(MFL_Common_Segmentation_Semi_Autosegmentation_adjustlambda, this);
	if (m_slideLambda)
	{
		m_slideLambda->setOrientation(Qt::Horizontal);
		m_slideLambda->setObjectName("SliderLambda");
		m_slideLambda->setMinimum(1.0);
		m_slideLambda->setMaximum(1000.0);
		m_slideLambda->setValue(50);
		m_slideLambda->installEventFilter(this);

		connect(m_slideLambda, &QSlider::valueChanged, this, &DrawcutTab::slot_OnSliderMoved);

		laySlider->addWidget(m_slideLambda);
	}

	//QGroupBox* grpBox = new QGroupBox(this);
	QGroupBox* grpBox = PRODUCT_FACTORY->createWidget<QGroupBox>(MFL_Common_Segmentation_Semi_Autosegmentation_DrawCut, this);
	if (grpBox)
	{
		grpBox->setTitle("Cursor shape");

		QVBoxLayout* layBox = new QVBoxLayout;
		QHBoxLayout* layButtons = new QHBoxLayout;

		layCursor->addLayout(layBox);
		layCursor->addLayout(layButtons);

		layBox->addWidget(grpBox);
		/*layBox = new QVBoxLayout;
		layCursor->addLayout(layBox);*/

		emptyBox0 = new QWidget(this);
		emptyBox0->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		layBox->addWidget(emptyBox0);

		layCursor = new QHBoxLayout;
		grpBox->setLayout(layCursor);

		m_grCursor = new QButtonGroup(this);

		QRadioButton* btnC = new QRadioButton(this);
		btnC->setText("Angular");
		btnC->setCheckable(true);
		btnC->setChecked(true);
		btnC->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_grCursor->addButton(btnC, 0);
		layCursor->addWidget(btnC);

		btnC = new QRadioButton(this);
		btnC->setText("Circular");
		btnC->setCheckable(true);
		btnC->setChecked(false);
		btnC->setStyleSheet(STYLE_MANAGER->m_Radiobtn);

		m_grCursor->addButton(btnC, 1);
		layCursor->addWidget(btnC);

		connect(m_grCursor, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnCursorChanged(int)));


		//
		// 	m_cbTargeted = new QCheckBox(this);
		// 	m_cbTargeted->setText("Targeted Draw-Cut");
		// 	m_cbTargeted->setCheckable(true);
		// 	m_cbTargeted->setChecked(false);
		// 	m_cbTargeted->setVisible(false);
		// 	connect(m_cbTargeted, &QCheckBox::clicked, this, &DrawcutTab::changeTargetedDrawCutMode);
		// 	layCheck->addWidget(m_cbTargeted);
		// 
		// //#ifdef TARGETED_DRAWCUT
		// 	m_cbTargeted->setVisible(true);
		//#endif

#ifdef MULTI_DRAWCUT_MODE
		m_actDelSeed = new QAction("Use ONLY in ROI tab", this);
		connect(m_actDelSeed, &QAction::triggered, this, &DrawcutTab::slot_OnSeedDelete);

		grpBox = new QGroupBox(this);
		grpBox->setTitle("Drawing Seed");

		layBox->addWidget(grpBox);

		layCursor = new QHBoxLayout;
		grpBox->setLayout(layCursor);

		for (int i = 0; i < 2; i++)
		{
			comboDrawing[i] = new QComboBox(this);
			comboDrawing[i]->setObjectName("ComboDrawing");
			comboDrawing[i]->addItem("Foreseed");
			comboDrawing[i]->addItem("Backseed");
			comboDrawing[i]->setCurrentIndex(i);
			comboDrawing[i]->setStyleSheet(STYLE_MANAGER->comboBoxTab);
			comboDrawing[i]->installEventFilter(this);

			connect(comboDrawing[i], SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnDrawingLayerChanged(int)));

			layCursor->addWidget(comboDrawing[i]);

			if (i == 0)
			{
				labelLam = new QLabel(this);
				labelLam->setText(" / ");
				layCursor->addWidget(labelLam);
			}
		}
#else
		m_actDelSeed = nullptr;
		for (int i = 0; i < 2; i++)
			comboDrawing[i] = nullptr;
#endif

		btnDrawCut = new QPushButton(this);
		btnDrawCut->setText(QString("Draw Cut"));
		btnDrawCut->setCheckable(true);
		btnDrawCut->setChecked(false);
		btnDrawCut->setStyleSheet(STYLE_MANAGER->buttonChecked);

		if (SHORTCUT_MANAGER->Action_SemiAutoSeg_DrawCutMode())
		{
			connect(SHORTCUT_MANAGER->Action_SemiAutoSeg_DrawCutMode(), &QAction::triggered, btnDrawCut, &QPushButton::click);
		}
		connect(btnDrawCut, &QPushButton::clicked, this, &DrawcutTab::slot_OnSketchDrawSegmentation);

		btnApply = new QPushButton(this);
		btnApply->setText(QString("Apply"));
		btnApply->setEnabled(false);
		btnApply->setStyleSheet(STYLE_MANAGER->buttonEnable);

		if (SHORTCUT_MANAGER->Action_SemiAutoSeg_DrawCutApply())
		{
			connect(SHORTCUT_MANAGER->Action_SemiAutoSeg_DrawCutApply(), &QAction::triggered, btnApply, &QPushButton::click);
		}
		connect(btnApply, &QPushButton::clicked, this, &DrawcutTab::slot_OnApply);

		layButtons->addWidget(btnDrawCut, 1);
		layButtons->addWidget(btnApply, 1);

		layButtons->setContentsMargins(0, 0, 0, 5);
	}

	QString title = "Draw Cut";
	setTitle(title);

	setOpenWidget(false);
}

void DrawcutTab::slot_OnCursorChanged(int index)
{
	if (index == 0)
	{
		setDrawCutMode(DRAW_CUT_SHAPE_ANGULAR);
	}
	else if (index == 1)
	{
		setDrawCutMode(DRAW_CUT_SHAPE_CIRCULAR);
	}

	if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION)
	{
		if (WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION)
			WIN_MANAGER->mainSegmentWidget->changeShapeSize();
	}
	else if (WIN_MANAGER->mainTabType == MAINTAB_MEASUREMENT)
	{
		if (WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_SKETCHDRAWSEGMENTATION)
			WIN_MANAGER->mainAnalWidget->changeShapeSize();
	}
}

void DrawcutTab::slot_OnReset()
{
	WIN_MANAGER->setMoveFocus(true);
	m_textLambda->setText(QString::number(50.0));
}

double DrawcutTab::getLambda()
{
	if (m_textLambda)
		return m_textLambda->text().toDouble();

	if (m_slideLambda)
		return m_slideLambda->value();

	return 50.0;
}

void DrawcutTab::changeSeedColor(int index, COLOR _col)
{
	if (0 > index || index > 1) return;

	QTreeWidgetItem* item = m_listSeed->topLevelItem(index);

	if (item)
		item->setBackground(0, QBrush(QColor(_col.r, _col.g, _col.b)));
}

void DrawcutTab::UpdateSeed(bool init)
{
	m_listSeed->clear();

	if (init)
		m_seedUID.clear();

	QList<QTreeWidgetItem*> _list;
	_list.reserve(2);

	QTreeWidgetItem* _Item = new QTreeWidgetItem(m_listSeed);
	COLOR _col = WIN_MANAGER->getSeedColor(0);
	_Item->setBackground(0, QBrush(QColor(_col.r, _col.g, _col.b)));
	_Item->setIcon(1, RESOURCE_MANAGER->getIcon(ICON_CLEAR));
	_Item->setText(2, QString("Backseed"));
	_list.append(_Item);
	_Item = new QTreeWidgetItem(m_listSeed);
	_col = WIN_MANAGER->getSeedColor(1);
	_Item->setBackground(0, QBrush(QColor(_col.r, _col.g, _col.b)));
	_Item->setIcon(1, RESOURCE_MANAGER->getIcon(ICON_CLEAR));
	_Item->setText(2, QString("Foreseed"));
	_list.append(_Item);

	if (!m_seedUID.isEmpty())
	{
		//	std::sort(m_seedUID.begin(), m_seedUID.end());

		for (int i = 0; i < m_seedUID.size(); i++)
		{
			int uid = m_seedUID.at(i);
			_Item = new QTreeWidgetItem(m_listSeed);
			QColor col = DATA_CONTEXT->volume_data.getMaskColor(uid, true);
			_Item->setBackground(0, QBrush(col));
			_Item->setIcon(1, RESOURCE_MANAGER->getIcon(ICON_CLEAR));
			_Item->setText(2, DATA_CONTEXT->volume_data.getMaskName(uid, true));
			_list.append(_Item);
		}
	}

	m_listSeed->insertTopLevelItems(0, _list);
	m_listSeed->setCurrentItem(_Item);
#ifdef MULTI_DRAWCUT_MODE
	for (int i = 0; i < 2; i++)
	{
		int cur = comboDrawing[i]->currentIndex();

		comboDrawing[i]->blockSignals(true);
		comboDrawing[i]->clear();
		comboDrawing[i]->addItem("Foreseed");
		comboDrawing[i]->addItem("Backseed");
		for (int j = 0; j < m_seedUID.size(); j++)
		{
			int uid = m_seedUID.at(j);
			comboDrawing[i]->addItem(DATA_CONTEXT->volume_data.getMaskName(uid, true));
		}

		comboDrawing[i]->blockSignals(false);

		comboDrawing[i]->setCurrentIndex(i);

		if (cur < comboDrawing[i]->count())
			comboDrawing[i]->setCurrentIndex(cur);
	}
#endif

	UpdateVoxel();
}

void DrawcutTab::UpdateVoxel()
{
	int index = m_listSeed->indexOfTopLevelItem(m_listSeed->currentItem());

	int voxel = 0;

	if (index <= 1)
		voxel = DATA_CONTEXT->volume_data.getVoxelCount(index, true);
	else
	{
		index -= 2;
		voxel = DATA_CONTEXT->volume_data.getVoxelCount(m_seedUID.at(index));
	}

	m_labelSeed->setText(QString("Voxel : %1(%2%3)").arg(voxel)
		.arg(QString::number((float)voxel / DATA_CONTEXT->volume_data.getVolumeDataLength(), 'f', 2)).arg("%"));
}

void DrawcutTab::drawCutOnOff(bool val)
{
	if (btnDrawCut->isChecked() != val)
		btnDrawCut->setChecked(val);
	if (btnApply->isChecked() != val)
		btnApply->setEnabled(val);
}

void DrawcutTab::ClearSeed()
{
	if (!m_seedUID.empty())
	{
		m_seedUID.clear();
		UpdateSeed();
	}
}

void DrawcutTab::AddSeed(int uid)
{
	m_seedUID.push_back(uid);

	UpdateSeed();
}

void DrawcutTab::AddSeed(std::vector<muint32> uid)
{
	bool chk = false;

	for (int i = 0; i < uid.size(); i++)
	{
		int _uid = uid.at(i);

		if (!m_seedUID.contains(_uid))
		{
			if (!chk) chk = true;
			m_seedUID.push_back(_uid);
		}
	}

	if (chk)
		UpdateSeed();
}

void DrawcutTab::DelSeed(int uid)
{
	if (-1 != m_seedUID.indexOf(uid))
	{
		m_seedUID.removeOne(uid);
		UpdateSeed();
	}
}

void DrawcutTab::DelSeed(std::vector<muint32> uid)
{
	bool chk = false;

	for (int i = 0; i < uid.size(); i++)
	{
		int _uid = uid.at(i);

		if (-1 != m_seedUID.indexOf(_uid))
		{
			if (!chk) chk = true;

			m_seedUID.removeOne(_uid);
		}
	}

	if (chk)
		UpdateSeed();
}

void DrawcutTab::ChangeSeed(int oldUID, int newUID)
{
	//	m_seedUID.remove(oldUID);
	//	m_seedUID.push_back(newUID);
	int index = m_seedUID.indexOf(oldUID);

	m_seedUID.replace(index, newUID);

	//	UpdateSeed();
}

bool DrawcutTab::contains(int seedUID)
{
	return m_seedUID.contains(seedUID);
}

void DrawcutTab::setDrawCutMode(DRAW_CUT_SHAPE shape)
{
	if (shape == DRAW_CUT_SHAPE_ANGULAR)
	{
		WIN_MANAGER->setDCutShape(DRAW_CUT_SHAPE_ANGULAR);

		if (m_grCursor->button(0))
		{
			m_grCursor->button(0)->setChecked(true);
		}
		if (m_grCursor->button(1))
		{
			m_grCursor->button(1)->setChecked(false);
		}

		WIN_MANAGER->setConfigValue(
			ELEMENT_FILE,
			STRING_MANAGER->config_DrawCutShape,
			QString::number(shape));
	}
	else if (shape == DRAW_CUT_SHAPE_CIRCULAR)
	{
		WIN_MANAGER->setDCutShape(DRAW_CUT_SHAPE_CIRCULAR);
		if (m_grCursor->button(0))
		{
			m_grCursor->button(0)->setChecked(false);
		}
		if (m_grCursor->button(1))
		{
			m_grCursor->button(1)->setChecked(true);
		}

		WIN_MANAGER->setConfigValue(
			ELEMENT_FILE,
			STRING_MANAGER->config_DrawCutShape,
			QString::number(shape));
	}
	else
	{
		/* 사각형을 Default로 설정 */
		WIN_MANAGER->setDCutShape(DRAW_CUT_SHAPE_ANGULAR);
		if (m_grCursor->button(0))
		{
			m_grCursor->button(0)->setChecked(true);
		}
		if (m_grCursor->button(1))
		{
			m_grCursor->button(1)->setChecked(false);
		}
	}
}


bool DrawcutTab::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == NULL) return QWidget::eventFilter(watched, event);

	if (watched->objectName().contains("Slider") ||
		watched->objectName().contains("Combo"))
	{
		if (event->type() == QEvent::Scroll ||
			event->type() == QEvent::Wheel)
			return true;
	}

	if (watched->objectName().contains("Edit"))
	{
		if (event->type() == QEvent::FocusIn)
			WIN_MANAGER->setMoveFocus(false);
	}

	return QWidget::eventFilter(watched, event);
}

void DrawcutTab::slot_OnSliderMoved(int val)
{
	WIN_MANAGER->setMoveFocus(true);
	m_textLambda->blockSignals(true);
	m_textLambda->setText(QString::number(val));
	m_textLambda->blockSignals(false);
}

void DrawcutTab::slot_OnTextChanged(const QString& txt)
{
	double tmpVal = txt.toDouble();

	if (m_slideLambda)
	{
		if (tmpVal < 1)
		{
			m_slideLambda->setValue(1);
			m_textLambda->blockSignals(true);
			m_textLambda->setText(QString::number(1.0));
			m_textLambda->blockSignals(false);
		}
		else if (tmpVal > 1000)
		{
			m_slideLambda->setValue(1000);
			m_textLambda->blockSignals(true);
			m_textLambda->setText(QString::number(1000.0));
			m_textLambda->blockSignals(false);
		}
		else
			m_slideLambda->setValue(tmpVal);
	}
}

void DrawcutTab::slot_OnTextEditFinished()
{
	WIN_MANAGER->setMoveFocus(true);

	double tmpVal = m_textLambda->text().toDouble();

	if (tmpVal < 1)
	{
		m_textLambda->blockSignals(true);
		m_textLambda->setText(QString::number(1.0));
		m_textLambda->blockSignals(false);
	}
	else if (tmpVal > 1000)
	{
		m_textLambda->blockSignals(true);
		m_textLambda->setText(QString::number(1000.0));
		m_textLambda->blockSignals(false);
	}
}

void DrawcutTab::slot_OnSeedDelete()
{
	QList<QTreeWidgetItem*> selList = m_listSeed->selectedItems();

	if (selList.size() != 1)return;

	muint32 index = m_listSeed->indexOfTopLevelItem(selList.at(0));

	if (index > 1)
	{
		index -= 2;
		index = m_seedUID.at(index);

		ACTION_MANAGER->action_MaskList_Cancel_Drawseed(index);
	}
}

void DrawcutTab::slot_OnColumnClicked(QTreeWidgetItem* item, int column)
{
	if (NULL == item)return;

	int index = m_listSeed->indexOfTopLevelItem(item);
	bool isDefault = true;

	if (index > 1)
	{
		index -= 2;
		index = m_seedUID.at(index);
		isDefault = false;
	}

	int voxel = DATA_CONTEXT->volume_data.getVoxelCount(index, isDefault);

	m_labelSeed->setText(QString("Voxel : %1(%2%3)").arg(voxel)
		.arg(QString::number((float)voxel / DATA_CONTEXT->volume_data.getVolumeDataLength(), 'f', 2)).arg("%"));

	if (1 == column)
		slot_OnClear();
}

void DrawcutTab::slot_OnColumnDoubleClicked(QTreeWidgetItem* item, int column)
{
	if (NULL == item)return;

	int index = m_listSeed->indexOfTopLevelItem(item);
	bool isDefault = true;

	if (index > 1)
	{
		index -= 2;
		index = m_seedUID.at(index);
		isDefault = false;
	}

	int voxel = DATA_CONTEXT->volume_data.getVoxelCount(index, isDefault);


	m_labelSeed->setText(QString("Voxel : %1(%2%3)").arg(voxel)
		.arg(QString::number((float)voxel / DATA_CONTEXT->volume_data.getVolumeDataLength(), 'f', 2)).arg("%"));

	if (0 == column)
	{
		QColor col;

		if (isDefault)
		{
			COLOR _col = WIN_MANAGER->getSeedColor(index);
			col = QColor(_col.r, _col.g, _col.b);
		}
		else
			col = DATA_CONTEXT->volume_data.getMaskColor(index, true);
		QColorDialog dlg;
		dlg.setStyleSheet("background-color : rgba(48,48,48,255); color : white");
		dlg.setCurrentColor(col);
		if (dlg.exec() == QDialog::Accepted)
		{
			QColor color = dlg.selectedColor();
			if (isDefault)
				ACTION_MANAGER->action_Seed_color(this, index, color);
			else
			{
				index = DATA_CONTEXT->volume_data.getMaskIndex(index);

				if (-1 != index)
				{
					ROITab2* tab = WIN_MANAGER->GetTab()->getROITab();

					if (tab)
						ACTION_MANAGER->action_MaskList_color(tab->ROIList, index, COLOR(color.red(), color.green(), color.blue(), 255));
				}
			}
		}
	}
}

void DrawcutTab::slot_OnContext(const QPoint& pos)
{
	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	QTreeWidgetItem* item = m_listSeed->itemAt(pos);

	if (NULL == item)	return;

	int index = m_listSeed->indexOfTopLevelItem(item);

	QMenu menu(m_listSeed);
	menu.addAction(m_actClear);
#ifdef MULTI_DRAWCUT_MODE
	if (index > 1)
		menu.addAction(m_actDelSeed);//todo
#endif
	menu.exec(QCursor::pos());
}

void DrawcutTab::slot_OnFitBoundingBox()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;
	DATA_CONTEXT->volume_data.setPreBoundingBox();

	QTreeWidgetItem* item = m_listSeed->currentItem();
	BoundingBoxI resetBox = DATA_CONTEXT->volume_data.getBoundingBox();
	resetBox.reset(DATA_CONTEXT->volume_data.getCX(), DATA_CONTEXT->volume_data.getCY(), DATA_CONTEXT->volume_data.getCZ());
	muint32 index = m_listSeed->indexOfTopLevelItem(item);
	bool isDefault = true;

	if (index > 1)
	{
		index -= 2;
		index = DATA_CONTEXT->volume_data.getMaskIndex(m_seedUID.at(index));
		isDefault = false;
	}

	bool isEmpty = DATA_CONTEXT->volume_data.isEmptyMaskVoxel(index, isDefault);

	if (isEmpty)
		ACTION_MANAGER->action_BoundingBox_Modify(resetBox);
	else
	{
		bool isFit = DATA_CONTEXT->volume_data.compareBoundingBox(index, isDefault);

		if (isFit)
			ACTION_MANAGER->action_BoundingBox_Modify(resetBox);
		else
		{
			if (!isDefault)
				index = DATA_CONTEXT->volume_data.getMaskInfo(index)->uid;

			ACTION_MANAGER->action_BoundingBox_Modify(DATA_CONTEXT->volume_data.getBoundingBox(index, isDefault));
		}
	}
}

void DrawcutTab::slot_OnSelectChange(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	if (current == NULL) return;

	int index = m_listSeed->indexOfTopLevelItem(current);

	int voxel = 0;
	bool isDefault = true;
	if (index > 1)
	{
		isDefault = false;
		index -= 2;
		index = m_seedUID.at(index);
	}

	voxel = DATA_CONTEXT->volume_data.getVoxelCount(index, isDefault);

	m_labelSeed->setText(QString("Voxel : %1(%2%3)").arg(voxel)
		.arg(QString::number((float)voxel / DATA_CONTEXT->volume_data.getVolumeDataLength(), 'f', 2)).arg("%"));

}

void DrawcutTab::slot_OnSketchDrawSegmentation()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false)
	{
		btnDrawCut->setChecked(false);
		return;
	}

	if (btnDrawCut->isChecked())
	{
		WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_SKETCHDRAWSEGMENTATION);

		if (WIN_MANAGER->mainAnalWidget) {
			WIN_MANAGER->mainAnalWidget->setWorkMode(ANAL_WORK_SKETCHDRAWSEGMENTATION);
		}

		btnApply->setEnabled(true);
	}
	else
	{
		if (WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION)
			WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
		if (WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_SKETCHDRAWSEGMENTATION)
			WIN_MANAGER->mainAnalWidget->setWorkMode(ANAL_WORK_NONE);

		btnApply->setEnabled(false);
	}
}

void DrawcutTab::slot_OnApply()
{
	double lam = 50.0;

	if (WIN_MANAGER->GetTab())
	{
		DrawcutTab* tab = WIN_MANAGER->GetTab()->getDrawTab();
		lam = tab->getLambda();
	}

	if (WIN_MANAGER->getWorkMode() == WORK_SKETCHDRAWSEGMENTATION)
		WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
	if (WIN_MANAGER->getAnalWorkMode() == ANAL_WORK_SKETCHDRAWSEGMENTATION)
		WIN_MANAGER->mainAnalWidget->setWorkMode(ANAL_WORK_NONE);

#ifndef MULTI_DRAWCUT_MODE

	// (WindowBase* view, bool merge, mask _mask, int maskIndex, double GPC_lambda)
	ACTION_MANAGER->action_SketchDrawSegmentation(0, false, WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex(), lam);

#else
	//TODO

#endif
}

void DrawcutTab::slot_OnClear()
{
	muint32 index = m_listSeed->indexOfTopLevelItem(m_listSeed->selectedItems().at(0));
	bool isDefault = true;
	if (index > 1)
	{
		index -= 2;
		index = DATA_CONTEXT->volume_data.getMaskIndex(m_seedUID.at(index));
		isDefault = false;
	}

	if (isDefault)
		ACTION_MANAGER->action_Seed_clear(index);
	else
	{
		MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(index);
		int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

		ACTION_MANAGER->action_MaskList_clear_one(index, mI);
	}
}

void DrawcutTab::slot_OnDrawingLayerChanged(int _index)
{
	for (int i = 0; i < 2; i++)
	{
		int index = comboDrawing[i]->currentIndex();

		if (index != _index) continue;

		bool isLeft = (i == 0);
		uchar m = (VM_MASK1 >> index);
		int _mI = 0;
		if (index > 1)
		{
			index -= 2;
			index = m_seedUID.at(index);

			MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(index, true);

			if (info)
			{
				_mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
				m = _mI == 0 ? info->mask_id : info->mask_id2;
			}
		}

		WIN_MANAGER->setDrawingSeed(isLeft, m, _mI);
	}
}

