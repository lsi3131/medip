#include "stdafx.h"
#include "AISetTab.h"
#include "VolumeData.h"
#include "ProductManager.h"
#include "StyleManager.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "LicenseManager.h"
#include "ResourceManager.h"
#include "FileManager.h"
#include "AISegTab.h"
#include "algorithm/Radiomics.h"

AISetTab::AISetTab(VOLUME_DATA* pVolumeData, ProductManager* pProductManager, QWidget* parent /*= NULL*/) :
	QGroupBox(parent),
	m_pVolumeData(pVolumeData),
	m_pProductManager(pProductManager),
	MIN_OUTPUT(0), MAX_OUTPUT(255)
{
	int nRow = 0;
	QLabel* label = nullptr;

#if 0

	label = new QLabel(this);
	label->setText("Window width/level set");
	label->show();
	addWidget(label, nRow++);

	label = new QLabel(this);
	label->setText("Width");
	label->show();
	addWidget(label, nRow);

	m_editWW = new QLineEdit(this);
	m_editWW->setValidator(new QIntValidator(this));
	m_editWW->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_editWW->setObjectName("EditWidth");
	m_editWW->setMouseTracking(true);
	m_editWW->installEventFilter(this);
	m_editWW->setReadOnly(true);
	m_editWW->show();
	connect(m_editWW, &QLineEdit::returnPressed, this, &AISetTab::slot_OnWWEditFinished);
	connect(m_editWW, &QLineEdit::editingFinished, this, &AISetTab::slot_OnWWEditFinished);

	addWidget(m_editWW, nRow);

	label = new QLabel(this);
	label->setText("Level");
	label->show();
	addWidget(label, nRow);

	m_editWL = new QLineEdit(this);
	m_editWL->setValidator(new QIntValidator(this));
	m_editWL->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_editWL->setObjectName("EditLevel");
	m_editWL->setMouseTracking(true);
	m_editWL->installEventFilter(this);
	m_editWL->setReadOnly(true);
	m_editWL->show();
	connect(m_editWL, &QLineEdit::returnPressed, this, &AISetTab::slot_OnWLEditFinished);
	connect(m_editWL, &QLineEdit::editingFinished, this, &AISetTab::slot_OnWLEditFinished);

	addWidget(m_editWL, nRow++);

	m_syncWL = new QCheckBox(this);
	m_syncWL->setText("Sync with WW/WL");
	m_syncWL->setCheckable(true);
	m_syncWL->setChecked(false);
	m_syncWL->show();
	m_syncWL->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	connect(m_syncWL, &QCheckBox::stateChanged, this, &AISetTab::slot_OnSyncWL);

	addWidget(m_syncWL, nRow++);
#endif

	QVBoxLayout* grpLay = new QVBoxLayout;
	QHBoxLayout* hLay = new QHBoxLayout;

	setLayout(grpLay);
	setTitle("Output setting");

	grpLay->addLayout(hLay);

	Factory* pProductFactory = m_pProductManager->getFactory();

	m_chBoxGMMautoApply = pProductFactory->createWidget<QCheckBox>(MFL_Product_MEDIP_Plugin_AIPack, this);// new QCheckBox(this);
	if (m_chBoxGMMautoApply)
	{
		m_chBoxGMMautoApply->setText("GMM auto apply");
		m_chBoxGMMautoApply->setCheckable(true);
		m_chBoxGMMautoApply->setChecked(true);
		m_chBoxGMMautoApply->setStyleSheet(STYLE_MANAGER->m_Checkbox);

		hLay->addWidget(m_chBoxGMMautoApply);
	}

	m_btnGMM = pProductFactory->createWidget<QPushButton>(MFL_Product_MEDIP_Plugin_AIPack, this);// new QCheckBox(this);
	if (m_btnGMM)
	{
		m_btnGMM->setText("GMM");
		m_btnGMM->setStyleSheet(STYLE_MANAGER->buttonNormal);
		connect(m_btnGMM, &QPushButton::clicked, this, &AISetTab::slot_OnGMM);

		hLay->addWidget(m_btnGMM);
	}

	hLay = new QHBoxLayout;
	grpLay->addLayout(hLay);

	m_chBoxColorMap = pProductFactory->createWidget<QCheckBox>(MFL_Product_MEDIP_Plugin_AIPack, this);// new QCheckBox(this);
	if (m_chBoxColorMap)
	{
		m_chBoxColorMap->setText("Enable mask Heatmap");
		m_chBoxColorMap->setCheckable(true);
		m_chBoxColorMap->setChecked(false);
		m_chBoxColorMap->setStyleSheet(STYLE_MANAGER->m_Checkbox);
		connect(m_chBoxColorMap, &QCheckBox::stateChanged, this, &AISetTab::slot_OnHeatMap);

		hLay->addWidget(m_chBoxColorMap);
	}
	//#endif


	hLay = new QHBoxLayout;

	grpLay->addLayout(hLay);

	m_editOut = pProductFactory->createWidget<QDoubleSpinBox>(MFL_Product_MEDIP_Plugin_AIPack, this);//new QDoubleSpinBox(this);
	if (m_editOut)
	{
		//m_editOut->setButtonSymbols(QDoubleSpinBox::NoButtons);
		m_editOut->setSingleStep(0.01);
		m_editOut->setRange(0, 1);
		m_editOut->setValue(0.5);
		m_editOut->setStyleSheet(STYLE_MANAGER->doubleSpinbox);
		m_editOut->setObjectName("EditOutput");
		m_editOut->setContextMenuPolicy(Qt::NoContextMenu);
		m_editOut->setMouseTracking(true);
		m_editOut->installEventFilter(this);

		connect(m_editOut, &QDoubleSpinBox::editingFinished, this, &AISetTab::slot_OnOutputChanged);
	}

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	hLay->addWidget(m_editOut);
	// 	hLay->addWidget(m_btnOut);
	// 	hLay->addWidget(m_labelOut);
	hLay->addWidget(emptyBox0);

	hLay = new QHBoxLayout;
	grpLay->addLayout(hLay);

	m_sliderOut = pProductFactory->createWidget<QSlider>(MFL_Product_MEDIP_Plugin_AIPack, this); //new QSlider(Qt::Horizontal, this);
	if (m_sliderOut)
	{
		m_sliderOut->setOrientation(Qt::Horizontal);
		m_sliderOut->setObjectName("SliderOutput");
		m_sliderOut->setMinimum(MIN_OUTPUT);
		m_sliderOut->setMaximum(MAX_OUTPUT);
		m_sliderOut->setValue((MAX_OUTPUT + MIN_OUTPUT + 1) / 2);
		m_sliderOut->installEventFilter(this);

		connect(m_sliderOut, &QSlider::sliderPressed, this, &AISetTab::slot_OnSliderPressed);
		connect(m_sliderOut, &QSlider::valueChanged, this, &AISetTab::slot_OnSliderMoved);
		connect(m_sliderOut, &QSlider::sliderReleased, this, &AISetTab::slot_OutsetApply);
		hLay->addWidget(m_sliderOut);
	}

	emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	//addWidget(emptyBox0, nRow++);

	// 	QString title = "Optional Set (Predict ONLY)";
	// 	setTitle(title);
}

void AISetTab::getWWWL(int& ww, int& wl)
{
	ww = m_editWW->text().toInt();
	wl = m_editWL->text().toInt();
}

int AISetTab::getSliderOutVal()
{
	//WIN_MANAGER->setMoveFocus(true);

	double tmpVal = m_sliderOut->value();

	return tmpVal;
}

void AISetTab::setSliderOutVal(int nVal)
{
	//WIN_MANAGER->setMoveFocus(true);
	m_sliderOut->setValue(nVal);
}

void AISetTab::setDoubleSpinValue(double val)
{
	m_editOut->setValue(val);
}

int AISetTab::getPreOutVal()
{
	return m_nPreSliderValue;
}

void AISetTab::setEnableObject(bool enable)
{
	if (m_btnGMM)
		m_btnGMM->setEnabled(enable);

	if (m_chBoxColorMap)
		m_chBoxColorMap->setEnabled(enable);

	if (m_editOut)
		m_editOut->setEnabled(enable);

	if (m_sliderOut)
		m_sliderOut->setEnabled(enable);
}

bool AISetTab::isGMMAutoApplyChecked(void)
{
	return m_chBoxGMMautoApply->isChecked();
}

bool AISetTab::isHeatMapChecked(void)
{
	if (m_chBoxColorMap)
	{
		return m_chBoxColorMap->isChecked();
	}

	return false;
}

bool AISetTab::isHeatMapEnabled(void)
{
	if (m_chBoxColorMap)
	{
		return m_chBoxColorMap->isEnabled();
	}

	return false;
}

void AISetTab::setHeatMapChecked(bool bCheck)
{
	if (m_chBoxGMMautoApply)
	{
		m_chBoxColorMap->setChecked(bCheck);
	}
}

bool AISetTab::eventFilter(QObject* watched, QEvent* event)
{
	static QIcon openIcon[] = { RESOURCE_MANAGER->getIcon(ICON_FILE_NON_OPEN),
		RESOURCE_MANAGER->getIcon(ICON_FILE_OPEN_HOVER),RESOURCE_MANAGER->getIcon(ICON_FILE_OPEN) };

	if (watched == NULL) return QWidget::eventFilter(watched, event);

	if (watched->objectName().contains("Slider"))
	{
		if (event->type() == QEvent::Scroll ||
			event->type() == QEvent::Wheel)
			return true;
	}

	if (watched->objectName().contains("Edit"))
	{
		if (event->type() == QEvent::FocusIn)
			WIN_MANAGER->setMoveFocus(false);
		if (watched->inherits("QDoubleSpinBox"))
		{
			if (event->type() == QEvent::Scroll ||
				event->type() == QEvent::Wheel)
				return true;
		}
	}

	// 	if (m_btnOut == watched)
	// 	{
	// 		if (event->type() == QEvent::MouseMove)
	// 			m_btnOut->setIcon(openIcon[1]);
	// 		else if (event->type() == QEvent::MouseButtonPress)
	// 			m_btnOut->setIcon(openIcon[2]);
	// 		else if (event->type() == QEvent::HoverLeave ||
	// 			event->type() == QEvent::MouseButtonRelease)
	// 			m_btnOut->setIcon(openIcon[0]);
	// 	}

	return QWidget::eventFilter(watched, event);
}

void AISetTab::slot_OnWWEditFinished()
{
	WIN_MANAGER->setMoveFocus(true);

	if (!m_pVolumeData->isValidate())
		return;

	int tmpVal = m_editWW->text().toInt();
	int HURange = (m_pVolumeData->getHuMax() - m_pVolumeData->getHuMin()) + 1;

	if (tmpVal < 1)
	{
		m_editWW->blockSignals(true);
		m_editWW->setText(QString::number(1));
		m_editWW->blockSignals(false);
	}
	else if (tmpVal > HURange)
	{
		m_editWW->blockSignals(true);
		m_editWW->setText(QString::number(HURange));
		m_editWW->blockSignals(false);
	}
}

void AISetTab::slot_OutsetApply()
{
	//if (m_fileOut.isEmpty()) return;

	if (!m_pVolumeData->isValidate())
		return;

	MaskInfo* info = m_pVolumeData->getCurrentMaskInfo();

	if (info)
	{
		int nSliderVal = getSliderOutVal(); // 0~255
		for (auto it = m_pVolumeData->m_vecAIOutset.begin(); it != m_pVolumeData->m_vecAIOutset.end(); ++it)
		{
			if (it->first == info->uid)
			{
				if (it->second != nSliderVal)
				{
					it->second = nSliderVal;
					ACTION_MANAGER->action_Deepdraw_outset(nSliderVal, info->uid, false);
				}
			}
		}
	}
}

void AISetTab::slot_OnSliderPressed(void)
{
	m_nPreSliderValue = m_sliderOut->value();
}

void AISetTab::slot_OnWLEditFinished()
{
	WIN_MANAGER->setMoveFocus(true);

	if (!m_pVolumeData->isValidate()) return;

	int tmpVal = m_editWL->text().toInt();
	int HUMin = m_pVolumeData->getHuMin();
	int HUMax = m_pVolumeData->getHuMax();

	if (tmpVal < HUMin)
	{
		m_editWW->blockSignals(true);
		m_editWW->setText(QString::number(HUMin));
		m_editWW->blockSignals(false);
	}
	else if (tmpVal > HUMax)
	{
		m_editWW->blockSignals(true);
		m_editWW->setText(QString::number(HUMax));
		m_editWW->blockSignals(false);
	}
}

void AISetTab::slot_OnStartEditFinished()
{
	WIN_MANAGER->setMoveFocus(true);

	if (!m_pVolumeData->isValidate()) return;

	// 	int tmpVal = m_editStart->text().toInt();
	// 	int MaxVal = m_pVolumeData->getCZ() - 1;
	// 
	// 	if (tmpVal < 0)
	// 	{
	// 		m_editStart->blockSignals(true);
	// 		m_editStart->setText(QString::number(0));
	// 		m_editStart->blockSignals(false);
	// 	}
	// 	else if (tmpVal > MaxVal)
	// 	{
	// 		m_editStart->blockSignals(true);
	// 		m_editStart->setText(QString::number(MaxVal));
	// 		m_editStart->blockSignals(false);
	// 	}
}

void AISetTab::slot_OnToEditFinished()
{
	WIN_MANAGER->setMoveFocus(true);

	if (!m_pVolumeData->isValidate()) return;

	// 	int tmpVal = m_editTo->text().toInt();
	// 	int MaxVal = m_pVolumeData->getCZ() - 1;
	// 
	// 	if (tmpVal < 0)
	// 	{
	// 		m_editTo->blockSignals(true);
	// 		m_editTo->setText(QString::number(0));
	// 		m_editTo->blockSignals(false);
	// 	}
	// 	else if (tmpVal > MaxVal)
	// 	{
	// 		m_editTo->blockSignals(true);
	// 		m_editTo->setText(QString::number(MaxVal));
	// 		m_editTo->blockSignals(false);
	// 	}
}

void AISetTab::slot_OnLoad()
{
	QString copyPath = STRING_MANAGER->LocalAISegPath + "predict_result";

	QDir dir(copyPath);

	if (!dir.exists())
	{
		copyPath = QDir::homePath();
	}

	QString strFilter = tr("Raw File(*.raw;*.RAW)");
	QFileInfo fileInfo(copyPath);
	QString fileName = ImportFileDialog(
		this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN),
		fileInfo.fileName(),
		fileInfo.dir().absolutePath(),
		strFilter
	);

	//QString fileName = QFileDialog::getOpenFileName(this,
	//	STRING_MANAGER->getString(STR_MEDIP_FILE_OPEN),
	//	copyPath, // const QString & dir = QString(),
	//	strFilter); // Options options = 0)

	if (m_pVolumeData->getVolumeDataLength() != WIN_MANAGER->getRawSize(fileName))
	{
		//messagebox && return false;
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_SCALE_NOT_MATCHED));
		return;
	}

	//setOutName(fileName);

	slot_OutsetApply();
}

void AISetTab::slot_OnSliderMoved(int val)
{
	WIN_MANAGER->setMoveFocus(true);

	double tmpVal = val;//0-255
	tmpVal /= double(MAX_OUTPUT);//0-1

	if (tmpVal != m_editOut->value())
	{
		m_editOut->blockSignals(true);
		m_editOut->setValue(tmpVal);
		m_editOut->blockSignals(false);
	}
}

void AISetTab::slot_OnOutputChanged()
{
	WIN_MANAGER->setMoveFocus(true);

	double tmpVal = m_editOut->value();

	if (tmpVal < 0)
		tmpVal = 0;
	else if (tmpVal > 1)
		tmpVal = 1;

	if (tmpVal != m_editOut->value())
	{
		m_editOut->blockSignals(true);
		m_editOut->setValue(tmpVal);
		m_editOut->blockSignals(false);
	}

	tmpVal *= (MAX_OUTPUT);
	//tmpVal += 1.f; //1~255


	if (int(tmpVal) != m_sliderOut->value())
	{
		m_sliderOut->blockSignals(true);
		m_sliderOut->setValue(int(tmpVal));
		m_sliderOut->blockSignals(false);
	}

	slot_OutsetApply();
}



void AISetTab::slot_OnSyncBox(bool checked)
{
	// 	if (checked)
	// 	{
	// 		if (m_editTo) m_editTo->setStyleSheet(STYLE_MANAGER->editReadOnly);
	// 		if (m_editStart) m_editStart->setStyleSheet(STYLE_MANAGER->editReadOnly);
	// 	}
	// 	else
	// 	{
	// 		if (m_editTo) m_editTo->setStyleSheet(STYLE_MANAGER->editBoxTab);
	// 		if (m_editStart) m_editStart->setStyleSheet(STYLE_MANAGER->editBoxTab);
	// 	}
	// 	if (m_editTo) m_editTo->setReadOnly(checked);
	// 	if (m_editStart) m_editStart->setReadOnly(checked);
	//	setDepth(checked);	
}


void AISegTab::slot_OnComponentBox(bool checked)//??? set? seg?
{
	m_editComponent->setReadOnly(!checked);

}

void AISetTab::slot_OnSyncWL(bool checked)
{
	m_editWL->setReadOnly(checked);
	m_editWW->setReadOnly(checked);
	//setWWWL();
}


void AISetTab::slot_OnHeatMap(bool checked)
{
	if (checked)
	{
		m_editOut->hide();
		m_sliderOut->hide();
	}
	else
	{
		m_editOut->show();
		m_sliderOut->show();
	}

	WIN_MANAGER->forceUpdateSegmentationWidget();
}

void AISetTab::slot_OnGMM(void)
{
	if (m_pVolumeData->isValidate() == false)
		return;

	MaskInfo* pMaskInfo = m_pVolumeData->getCurrentMaskInfo();
	mask8 maskBit = m_pVolumeData->getMask(pMaskInfo->uid);

	//int byteIdx = m_pVolumeData->GetMaskByteIndex(pMaskInfo->uid);


	int nAIResultIdx = -1;
	for (auto i = 0; i < m_pVolumeData->m_vecAIResultData.size(); ++i)
	{
		if (m_pVolumeData->m_vecAIResultData[i].first == pMaskInfo->uid)
		{
			nAIResultIdx = i;
			break;
		}
	}

	if (nAIResultIdx == -1)
		return;

	std::vector<mask8> vecAIMaskData(m_pVolumeData->getVolumeDataLength(), 0);
	std::vector<mint16> vecAIProbabilityData(m_pVolumeData->getVolumeDataLength(), 0);

	for (int j = 0; j < m_pVolumeData->m_vecAIResultData[nAIResultIdx].second.size(); ++j)
	{
		if (m_pVolumeData->m_vecAIResultData[nAIResultIdx].second[j] > 0)
		{
			vecAIMaskData[j] |= maskBit;
		}

		vecAIProbabilityData[j] = m_pVolumeData->m_vecAIResultData[nAIResultIdx].second[j];
	}

	muint32 cx = m_pVolumeData->getCX();
	muint32 cy = m_pVolumeData->getCY();
	muint32 cz = m_pVolumeData->getCZ();

	int num = 2;

	std::vector<GMMOverlapedData> GMMDatas;
	getGMMOverlapValue(&vecAIMaskData[0], &vecAIProbabilityData[0], cx, cy, cz, maskBit, num, GMMDatas);

	double gmmMiddle = 256.0 / 2.0;
	if (GMMDatas.size() >= num)
	{
		double sum = GMMDatas[num - 1].MeanCluster + GMMDatas[num - 2].MeanCluster;
		gmmMiddle = sum / 2.0;
	}

	for (int iter = 0; iter < GMMDatas.size(); iter++)
	{
		qDebug() << GMMDatas[iter].MeanCluster;
	}

	double editVal = gmmMiddle / (double)(MAX_OUTPUT + 1);

	setDoubleSpinValue(editVal);
	slot_OnOutputChanged();

}

Qt::CheckState AISetTab::getColorMapState(void)
{
	return m_chBoxColorMap->checkState();
}


