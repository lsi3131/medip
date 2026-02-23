#include "stdafx.h"
#include "WorkingRegionTab.h"
#include "StringManager.h"
#include "StyleManager.h"
#include "WindowManager.h"
#include "ProductManager.h"
#include "Main/MainSegmentWidget.h"
#include "FileManager.h"
#include "DataContext.h"

WorkingRegionTab::WorkingRegionTab(QWidget* parent /*= NULL*/) : 
	CollapseWidget(QString(), parent)
{
	int nRow = 0;

	m_btnPoint = new QPushButton(this);
	m_btnPoint->setText("Select a Patchy Point");
	m_btnPoint->setStyleSheet(STYLE_MANAGER->buttonBehind);

	connect(m_btnPoint, &QPushButton::clicked, this, &WorkingRegionTab::slot_OnSelect);

	m_labelPoint = new QLabel(this);
	m_labelPoint->setText("Not Selected");

	addWidget(m_btnPoint, nRow);
	addWidget(m_labelPoint, nRow++);

	QGroupBox* boxRegion = new QGroupBox(this);
	boxRegion->setTitle("Patchy Region");
	QVBoxLayout* boxMain = new QVBoxLayout;
	QHBoxLayout* boxCombo = new QHBoxLayout;
	QHBoxLayout* boxLabel = new QHBoxLayout;
	boxMain->addLayout(boxCombo);
	boxMain->addLayout(boxLabel);
	boxRegion->setLayout(boxMain);

	addWidget(boxRegion, nRow, 1);

	m_comboRegion = new QComboBox(this);
	m_comboRegion->addItem(QString("1"));
	m_comboRegion->addItem(QString("2"));
	m_comboRegion->addItem(QString("3"));
	m_comboRegion->addItem(QString("4"));
	m_comboRegion->addItem(QString("5"));
	m_comboRegion->addItem(QString("All regions"));
	m_comboRegion->addItem(QString("Custom"));
	m_comboRegion->setStyleSheet(STYLE_MANAGER->comboBoxTab);
	m_comboRegion->installEventFilter(this);
	m_comboRegion->setDisabled(true);
	connect(m_comboRegion, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnComboChanged(int)));

	m_editRegion = new QLineEdit(this);
	m_editRegion->setValidator(new QDoubleValidator(1, 10000, 2, this));
	m_editRegion->setText(QString::number(1.00));
	m_editRegion->setStyleSheet(STYLE_MANAGER->editBoxTab);
	m_editRegion->setObjectName("EditRegion");
	m_editRegion->installEventFilter(this);
	m_editRegion->setMouseTracking(true);
	m_editRegion->hide();

	connect(m_editRegion, &QLineEdit::editingFinished, this, &WorkingRegionTab::slot_OnApply);
	connect(m_editRegion, &QLineEdit::returnPressed, this, &WorkingRegionTab::slot_OnApply);

	QLabel* labelcm3 = new QLabel(this);
	labelcm3->setText(QString("cm%1").arg(QString(QChar(0x00B3))));

	boxCombo->addWidget(m_comboRegion);
	boxCombo->addWidget(m_editRegion);
	boxCombo->addWidget(labelcm3);

	m_labelInfo = new QLabel(this);
	m_labelInfo->setStyleSheet(STYLE_MANAGER->labelNormal);
	m_labelInfo->hide();

	boxLabel->addWidget(m_labelInfo);

	QPushButton* btnApply = new QPushButton(this);
	btnApply->setText(STRING_MANAGER->getString(STR_APPLY));
	btnApply->setStyleSheet(STYLE_MANAGER->buttonBehind);

	connect(btnApply, &QPushButton::clicked, this, &WorkingRegionTab::slot_OnApply);

	//QMenu *clipMenu = new QMenu(this);
	QMenu* clipMenu = PRODUCT_FACTORY->createWidget<QMenu>(MFL_Common_LayerOperation_MaskExport, this);
	QToolButton* btnExport = nullptr;
	if (clipMenu)
	{
		QAction* actNII = new QAction("NIfTI-1 (.nii, Patchy Region)", this);
		QAction* actRAW = new QAction("HU raw (.raw, Patchy Region)", this);

		connect(actNII, &QAction::triggered, this, &WorkingRegionTab::slot_OnSaveToNII);
		connect(actRAW, &QAction::triggered, this, &WorkingRegionTab::slot_OnSaveToHURAW);

		clipMenu->addAction(actNII);
		clipMenu->addAction(actRAW);

		btnExport = new QToolButton(this);
		btnExport->setPopupMode(QToolButton::InstantPopup);
		btnExport->setMenu(clipMenu);
		btnExport->setText("Export as...");
		btnExport->setStyleSheet("color: black;");
		btnExport->setToolTip("Patchy Region to Export as...");
		btnExport->setStyleSheet(STYLE_MANAGER->buttonBehind);


	}

	boxRegion = new QGroupBox(this);
	boxRegion->setStyleSheet("QGroupBox { border: none;} ");
	QVBoxLayout* boxMain2 = new QVBoxLayout;
	boxRegion->setLayout(boxMain2);

	boxMain2->addWidget(btnApply);
	if (clipMenu) boxMain2->addWidget(btnExport);

	addWidget(boxRegion, nRow++, 1);

	////////////////////////////////////////////////////////////////
	/////////////////////move working region////////////////////////
	////////////////////////////////////////////////////////////////

	QFrame* frLine;

	frLine = new QFrame(this);
	frLine->setStyleSheet("background-color:rgba(255,255,255,80);");
	frLine->setFrameShape(QFrame::HLine);
	frLine->setFrameShadow(QFrame::Sunken);
	frLine->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	addWidget(frLine, nRow++);

	m_btnMove = new QPushButton(this);
	m_btnMove->setText("Move Working Region");
	m_btnMove->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnMove->setCheckable(true);
	m_btnMove->setChecked(false);

	connect(m_btnMove, &QPushButton::clicked, this, &WorkingRegionTab::slot_OnMoveRegion);

	addWidget(m_btnMove, nRow++);

	QCheckBox* chkRatio = new QCheckBox(this);
	chkRatio->setText("Maintain Aspect Ratio");
	chkRatio->setCheckable(true);
	chkRatio->setChecked(true);
	chkRatio->setStyleSheet(STYLE_MANAGER->m_Checkbox);

	m_ratio = true;

	connect(chkRatio, &QCheckBox::clicked, this, &WorkingRegionTab::slot_OnRatioChk);

	addWidget(chkRatio, nRow++);

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	addWidget(emptyBox0, nRow++, 0);

	QString title = "Working Region";
	setTitle(title);

	setOpenWidget(false);
}

void WorkingRegionTab::slot_OnSelect(bool val/*=false*/)
{
	if (!DATA_CONTEXT->volume_data.isValidate())
		return;

	WORK_MODE mode = WIN_MANAGER->getWorkMode();

	if (mode == WORK_PATCHY_SELECT)
	{
		WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
	}
	else
	{
		m_btnPoint->setText("Select a Patchy Point mode cancel");
		WIN_MANAGER->selectPatchyMode();
	}
}

void WorkingRegionTab::SelectPoint()
{
	if (-1 == WIN_MANAGER->patchyPoint.x())
	{
		m_labelPoint->setText("Not Selected");
		m_comboRegion->setDisabled(true);
		m_comboRegion->blockSignals(true);
		m_comboRegion->setCurrentIndex(0);
		m_comboRegion->blockSignals(false);
		setRegionInfo(false);
	}
	else
	{
		m_labelPoint->setText(QString("(%1, %2, %3)").arg(WIN_MANAGER->patchyPoint.x()).
			arg(WIN_MANAGER->patchyPoint.y()).arg(DATA_CONTEXT->volume_data.getCZ() - (WIN_MANAGER->patchyPoint.z() + 1)));
		m_comboRegion->setDisabled(false);
		slot_OnApply();
	}


}

void WorkingRegionTab::cancelSelectMode()
{
	if (m_btnPoint)
		m_btnPoint->setText("Select a Patchy Point");
}

void WorkingRegionTab::cancelMoveMode()
{
	if (m_btnMove)
		m_btnMove->setText("Move Working region");
}

void WorkingRegionTab::setRegionInfo(bool isCombo)
{
	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		m_labelInfo->hide();
		return;
	}

	if (isCombo)
	{
		m_labelInfo->setStyleSheet(STYLE_MANAGER->labelNormal);
		int index = m_comboRegion->currentIndex();

		m_regionBox.reset(DATA_CONTEXT->volume_data.getCX(),
			DATA_CONTEXT->volume_data.getCY(), DATA_CONTEXT->volume_data.getCZ());

		if (index != 5) // 5- all regions
		{
			float fRegion;

			if (index < 5) // 1~5cm
			{
				fRegion = index + 1; //cm

				//cm to mm
				fRegion *= 10.0f;

				//half mm
				fRegion /= 2.0f;

			}
			else if (index == 6)
			{
				bool res = true;
				fRegion = m_editRegion->text().toFloat(&res);

				if (!res)
				{
					m_editRegion->setText(QString::number(1.00));

					fRegion = 1; //cm

					//cm to mm
					fRegion *= 10.0f;

					//half mm
					fRegion /= 2.0f;
				}
				else
				{
					//cm to mm
					fRegion *= 10.0f;

					//half mm
					fRegion /= 2.0f;
				}
			}

			int res = fRegion / DATA_CONTEXT->volume_data.getSpaceX(true);
			float val = fmodf(fRegion, DATA_CONTEXT->volume_data.getSpaceX(true));

			if (val != 0)
				res++;

			m_xRegion = res;

			res = fRegion / DATA_CONTEXT->volume_data.getSpaceY(true);
			val = fmodf(fRegion, DATA_CONTEXT->volume_data.getSpaceY(true));

			if (val != 0)
				res++;

			m_yRegion = res;

			res = fRegion / DATA_CONTEXT->volume_data.getSpaceZ(true);
			val = fmodf(fRegion, DATA_CONTEXT->volume_data.getSpaceZ(true));

			if (val != 0)
				res++;

			m_zRegion = res;

			m_regionBox.minX = WIN_MANAGER->patchyPoint.x() - m_xRegion;

			if (m_regionBox.minX < 0)
				m_regionBox.minX = 0;

			m_regionBox.maxX = WIN_MANAGER->patchyPoint.x() + m_xRegion;

			if (m_regionBox.maxX >= DATA_CONTEXT->volume_data.getCX())
				m_regionBox.maxX = DATA_CONTEXT->volume_data.getCX() - 1;


			m_regionBox.minY = WIN_MANAGER->patchyPoint.y() - m_yRegion;

			if (m_regionBox.minY < 0)
				m_regionBox.minY = 0;

			m_regionBox.maxY = WIN_MANAGER->patchyPoint.y() + m_yRegion;

			if (m_regionBox.maxY >= DATA_CONTEXT->volume_data.getCY())
				m_regionBox.maxY = DATA_CONTEXT->volume_data.getCY() - 1;

			m_regionBox.minZ = WIN_MANAGER->patchyPoint.z() - m_zRegion;

			if (m_regionBox.minZ < 0)
				m_regionBox.minZ = 0;

			m_regionBox.maxZ = WIN_MANAGER->patchyPoint.z() + m_zRegion;

			if (m_regionBox.maxZ >= DATA_CONTEXT->volume_data.getCZ())
				m_regionBox.maxZ = DATA_CONTEXT->volume_data.getCZ() - 1;
		}
		else
		{
			m_xRegion = (float)(DATA_CONTEXT->volume_data.getCX() - 1) / 2;
			m_yRegion = (float)(DATA_CONTEXT->volume_data.getCY() - 1) / 2;
			m_zRegion = (float)(DATA_CONTEXT->volume_data.getCZ() - 1) / 2;
		}
	}
	else
	{
		m_labelInfo->setStyleSheet(STYLE_MANAGER->labelAddr);
		m_regionBox = DATA_CONTEXT->volume_data.getBoundingBox();

		m_xRegion = abs(m_regionBox.maxX - m_regionBox.minX) / 2;
		m_yRegion = abs(m_regionBox.maxY - m_regionBox.minY) / 2;
		m_zRegion = abs(m_regionBox.maxZ - m_regionBox.minZ) / 2;
	}

	QString xStr, yStr, zStr;

	xStr = QString::number((m_xRegion * DATA_CONTEXT->volume_data.getSpaceX(true)) * 2.0f / 10.0f, 'f', 2);
	yStr = QString::number((m_yRegion * DATA_CONTEXT->volume_data.getSpaceY(true)) * 2.0f / 10.0f, 'f', 2);
	zStr = QString::number((m_zRegion * DATA_CONTEXT->volume_data.getSpaceZ(true)) * 2.0f / 10.0f, 'f', 2);

	m_labelInfo->setText(QString("(%1 * %2 * %3)").arg(xStr).arg(yStr).arg(zStr));

	if (m_labelInfo->isHidden())
		m_labelInfo->show();
}

bool WorkingRegionTab::eventFilter(QObject* watched, QEvent* e)
{
	if (watched == NULL) return false;

	if (watched->objectName().contains("Slider") || watched->objectName().contains("Combo"))
	{
		if (e->type() == QEvent::Wheel)
			return true;
	}

	if (watched->objectName().contains("Edit"))
	{
		if (e->type() == QEvent::FocusIn)
			WIN_MANAGER->setMoveFocus(false);
	}

	return QWidget::eventFilter(watched, e);
}

void WorkingRegionTab::slot_OnComboChanged(int index /*= 0*/)
{
	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		m_comboRegion->blockSignals(true);
		m_editRegion->hide();
		m_comboRegion->setCurrentIndex(0);
		m_comboRegion->blockSignals(false);
		return;
	}

	WIN_MANAGER->setMoveFocus(true);

	slot_OnApply();

	if (index != 6)
		m_editRegion->hide();
	else
	{
		m_editRegion->show();
		WIN_MANAGER->setMoveFocus(false);
		m_editRegion->setFocus();
	}
}

void WorkingRegionTab::slot_OnApply()
{
	WIN_MANAGER->setMoveFocus(true);

	if (!DATA_CONTEXT->volume_data.isValidate() || WIN_MANAGER->patchyPoint.x() == -1) return;

	setRegionInfo();

	if (DATA_CONTEXT->volume_data.getBoundingBox() != m_regionBox)
	{
		DATA_CONTEXT->volume_data.setPreBoundingBox();
		ACTION_MANAGER->action_BoundingBox_Modify(m_regionBox);
	}
}

void WorkingRegionTab::slot_OnSaveToNII()
{
	WIN_MANAGER->setMoveFocus(true);

	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		return;
	}

	if (!WIN_MANAGER->IsLicensePass() || !PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Export_NII_wholeHU))
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_NII),
			STRING_MANAGER->getString(STR_LICENSE_WARN));

		return;
	}

	setRegionInfo(false);

	QString strLatest;
	bool latest = WIN_MANAGER->lastestPathGet(strLatest);

	QFileInfo fileInfo(strLatest);
	QString fileName = ExportFileDialog(
		this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_SAVE),
		fileInfo.fileName(),
		fileInfo.dir().path(),
		tr("NII File(*.nii;*.NII)"),
		QFileDialog::ShowDirsOnly
	);

	if (!fileName.isEmpty())
	{
		WIN_MANAGER->saveNII(fileName, true, true, m_regionBox);
	}
}

void WorkingRegionTab::slot_OnSaveToHURAW()
{
	WIN_MANAGER->setMoveFocus(true);

	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		return;
	}

	if (!WIN_MANAGER->IsLicensePass())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_EXPORT_HURAW),
			STRING_MANAGER->getString(STR_LICENSE_WARN));

		return;
	}

	setRegionInfo(false);

	QString strLatest;
	bool latest = WIN_MANAGER->lastestPathGet(strLatest);

	QFileInfo fileInfo(strLatest);
	QString fileName = ExportFileDialog(
		this,
		STRING_MANAGER->getString(STR_MEDIP_FILE_SAVE),
		fileInfo.fileName(),
		fileInfo.dir().path(),
		tr("HU Raw File(*.raw;*.RAW)"),
		QFileDialog::ShowDirsOnly
	);

	if (!fileName.isEmpty())
	{
		WIN_MANAGER->saveHURAW(fileName, 0, 0, true, m_regionBox);
	}
}

void WorkingRegionTab::slot_OnMoveRegion(bool checked)
{
	WIN_MANAGER->setMoveFocus(true);

	if (!DATA_CONTEXT->volume_data.isValidate())
		return;

	WORK_MODE mode = WIN_MANAGER->getWorkMode();

	if (WORK_WORKING_REGION_MOVE == mode)
	{
		WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
		m_btnMove->setText("Move Working region");
	}
	else //complete
	{
		m_btnMove->setText("Fix on Working region");
		WIN_MANAGER->moveRegionMode();
	}

}

void WorkingRegionTab::slot_OnRatioChk(bool val)
{
	WIN_MANAGER->setMoveFocus(true);

	m_ratio = val;
}
