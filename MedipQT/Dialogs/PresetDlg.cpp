#include "stdafx.h"
#include "PresetDlg.h"

#include "Dialogs/ColorMapDialog.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "UI/CustomHistogram.h"
#include "UI/AlphaColorMap.h"
#include "System/stringManager.h"
#include "Actions/ActionManager.h"
#include "System/ProductManager.h"
#include "DataContext.h"

PresetDialog::PresetDialog(int index, bool type, QWidget *parent)
	:QDialog(parent)
{
	setStyleSheet("background-color: rgba(82, 82, 82, 255);");

	m_index = index;
	m_type = type;
	m_name = "";

	setWindowTitle(QString("Custom preset %1 dialog (default setting)").arg(m_type? QString("Edit"):QString("Save")));

	QVBoxLayout * mainLayout = new QVBoxLayout(this);

	QHBoxLayout *hLayout = new QHBoxLayout;
	mainLayout->addLayout(hLayout);

	QLabel *label = new QLabel(this);
	label->setText("Type");

	QFontMetrics fontMet(this->font());
	QRect rect = fontMet.boundingRect("(2D Default)");
	label->setFixedWidth(rect.width());

	m_typeName = new QLineEdit(this);
	if(m_type)
		m_typeName->setText(WIN_MANAGER->getPresetName(index));
	m_typeName->setReadOnly(m_type);

	QWidget *emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	hLayout->addWidget(label);
	hLayout->addWidget(m_typeName);
	hLayout->addWidget(emptyBox);

	hLayout = new QHBoxLayout;
	mainLayout->addLayout(hLayout);

	label = new QLabel(this);
	label->setText(QString("WW/WL<br>(2D)"));
	
	hLayout->addWidget(label);

	m_2DWW = new QLineEdit(this);
	m_2DWW->setText(QString::number(WIN_MANAGER->getWindowWidth()));
	m_2DWW->setValidator(new QIntValidator(1, DATA_CONTEXT->volume_data.getHuMax() - DATA_CONTEXT->volume_data.getHuMin(), this));
	
	label = new QLabel(this);
	label->setText(QString(" / "));

	m_2DWL = new QLineEdit(this);
	m_2DWL->setText(QString::number(WIN_MANAGER->getWindowLevel()));
	m_2DWL->setValidator(new QIntValidator(DATA_CONTEXT->volume_data.getHuMin(), DATA_CONTEXT->volume_data.getHuMax(), this));

	emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	hLayout->addWidget(m_2DWW);
	hLayout->addWidget(label);
	hLayout->addWidget(m_2DWL);
	hLayout->addWidget(emptyBox);
	
	hLayout = new QHBoxLayout;
	mainLayout->addLayout(hLayout);

	label = new QLabel(this);
	label->setText(QString("WW/WL<br>(3D)"));

	hLayout->addWidget(label);

	m_3DWW = new QLineEdit(this);
	m_3DWW->setText(QString::number(WIN_MANAGER->getVolumeWidth()));
	m_3DWW->setValidator(new QIntValidator(1, DATA_CONTEXT->volume_data.getHuMax() - DATA_CONTEXT->volume_data.getHuMin(), this));

	label = new QLabel(this);
	label->setText(QString(" / "));

	m_3DWL = new QLineEdit(this);
	m_3DWL->setText(QString::number(WIN_MANAGER->getVolumeLevel()));
	m_3DWL->setValidator(new QIntValidator(DATA_CONTEXT->volume_data.getHuMin(), DATA_CONTEXT->volume_data.getHuMax(), this));

	emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	hLayout->addWidget(m_3DWW);
	hLayout->addWidget(label);
	hLayout->addWidget(m_3DWL);
	hLayout->addWidget(emptyBox);

	hLayout = new QHBoxLayout;
	mainLayout->addLayout(hLayout);

	label = new QLabel(this);
	label->setText(QString("Color table<br>(2D)"));

	m_2DColor = new AlphaColorMap(this, CL_2D);
	m_2DColor->setMouseTracking(true);
	m_2DColor->setObjectName("ColorTable2D");
	m_2DColor->installEventFilter(this); // mouse press event to colormap dialog
	m_2DColor->setType(CL_2D);
	if(m_type)
		m_2DColor->setPreset((SLICE_PRESET)SP_DEFAULT, false, index);
	else
		m_2DColor->setPreset((SLICE_PRESET)SP_CUSTOM, false);

	emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	hLayout->addWidget(label);
	hLayout->addWidget(m_2DColor);
	hLayout->addWidget(emptyBox);

	int w, h;

	w = this->sizeHint().width() - (label->sizeHint().width()+10);
	h = label->sizeHint().height();

	m_2DColor->setFixedSize(w, h);

	hLayout = new QHBoxLayout;
	mainLayout->addLayout(hLayout);

	label = new QLabel(this);
	label->setText(QString("Color table<br>(3D)"));

	m_3DColor = new AlphaColorMap(this, CL_3D);
	m_3DColor->setMouseTracking(true);
	m_3DColor->setObjectName("ColorTable3D");
	m_3DColor->installEventFilter(this); // mouse press event to colormap dialog
	m_3DColor->setType(CL_3D);

	if (m_type)
		m_3DColor->setPreset((SLICE_PRESET)SP_DEFAULT, false, index);
	else
		m_3DColor->setPreset((SLICE_PRESET)SP_CUSTOM, false);

	emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	hLayout->addWidget(label);
	hLayout->addWidget(m_3DColor);
	hLayout->addWidget(emptyBox);

	w = this->sizeHint().width() - (label->sizeHint().width() + 10);
	h = label->sizeHint().height();

	m_3DColor->setFixedSize(w, h);

	hLayout = new QHBoxLayout;
	mainLayout->addLayout(hLayout);
	
	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetfileexport))
	{
		m_save = new QPushButton(this);
		m_save->setText(m_type ? QString("Edit") : QString("Save"));

		connect(m_save, &QPushButton::clicked, this, &PresetDialog::OnSave);
	}

	QPushButton* btnCancel = new QPushButton(this);
	btnCancel->setText("Cancel");

	connect(btnCancel, &QPushButton::clicked, this, &PresetDialog::OnCancel);

	emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	hLayout->addWidget(m_save);
	hLayout->addWidget(btnCancel);
	hLayout->addWidget(emptyBox);

	w = this->sizeHint().width();
	h = this->sizeHint().height();

	setFixedSize(w, h);
}

void PresetDialog::OnSave()
{
	QString name = m_typeName->text();

	if (!m_type)
	{
		bool res = name.length() == 0;

		for (int i = 0; i < WIN_MANAGER->getPresetCount(); i++)
		{
			if (0 == name.compare(WIN_MANAGER->getPresetName(i), Qt::CaseInsensitive))
			{
				res = true;
				break;
			}
		}

		if (res)
		{
			//warning msg
			QMessageBox::warning(this, QString("custom preset name rule"),
				QString("The preset name must be <b>at least one character</b> and<br><b>can not be duplicated</b> with other names."));
			return;
		}
		
		if (name.contains(QRegularExpression(FILE_NAME_RULE)))
		{
			QMessageBox::warning(this, QString("custom preset name rule"),
				QString("The file name can not contain any of the following characters:<br><b>\\ \" / : * ? &lt; &gt; | </b> "));
			return;
		}


		QFile file(STRING_MANAGER->presetFilePath + "/" + name + ".prd");

 		if(file.exists())
 		{
			if ((QMessageBox::warning(this, "custom preset name rule",
				"It is the same as the name of the unloaded file. Are you sure you want to overwrite that file?",
				QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel)) != QMessageBox::Ok)
			{
				return;
			}
			else
				file.remove();
 		}
	}

	m_name = name;

	int ww, wl;

	ww = m_2DWW->text().toInt();
	wl = m_2DWL->text().toInt();
	
	if (ww != WIN_MANAGER->getWindowWidth())
		ACTION_MANAGER->action_Window_Width(ww);

	if (wl != WIN_MANAGER->getWindowLevel())
		ACTION_MANAGER->action_Window_Level(wl);

	ww = m_3DWW->text().toInt();
	wl = m_3DWL->text().toInt();

	if (ww != WIN_MANAGER->getVolumeWidth())
		ACTION_MANAGER->action_Volume_Width(ww);

	if (wl != WIN_MANAGER->getVolumeLevel())
		ACTION_MANAGER->action_Volume_Level(wl);

	QDialog::accept();
}

bool PresetDialog::eventFilter(QObject *obj, QEvent * evt)
{
	if (NULL == obj)
		return QWidget::eventFilter(obj, evt);

	if(!obj->objectName().contains("ColorTable"))
		return QWidget::eventFilter(obj, evt);

	QEvent::Type evtType = evt->type();

	if (!(evtType == QEvent::Enter ||
		evtType == QEvent::Leave ||
		evtType == QEvent::MouseButtonPress))
		return QWidget::eventFilter(obj, evt);
	
	switch (evtType)
	{
	case QEvent::MouseButtonPress:
	{
		AlphaColorMap *colorMap = dynamic_cast<AlphaColorMap*>(obj);

		if (colorMap)
		{
			ColorMapDialog dlg(colorMap->getType(), this, true);

			dlg.exec();

			if (dlg.isChanged())
			{
				colorMap->setPresetStops(dlg.getColorMapStops(), colorMap->getType());
				switch (colorMap->getType())
				{
				case CL_3D:
					if (WIN_MANAGER->mainSegmentWidget->isVisibleHistogram())
						WIN_MANAGER->mainSegmentWidget->SetVolumeWidthLine(true);

					WIN_MANAGER->updatePresetTexture(WIN_MANAGER->getSelectedVolumePreset(), WIN_MANAGER->getSelectedCustomPreset(CL_3D));
					WIN_MANAGER->renderLater_3DView();
					break;
				case CL_2D:
				default:
					if (WIN_MANAGER->mainSegmentWidget->isVisibleHistogram())
						WIN_MANAGER->mainSegmentWidget->SetWindowWidthLine(true);
					DATA_CONTEXT->volume_data.updateColorTable();
					WIN_MANAGER->update2DPresetTexture(WIN_MANAGER->getSelectedPreset(), WIN_MANAGER->getSelectedCustomPreset(CL_2D));
					DATA_CONTEXT->volume_data.forceUpdateMPR();//2dviewdata
					WIN_MANAGER->forceUpdate2DViewData();
					WIN_MANAGER->renderLater_GridView(false);
					break;
				}
			}
			if (dlg.isAlphaChanged())
			{
				if (colorMap->getType() == CL_3D)
				{
					WIN_MANAGER->updatePresetTexture(WIN_MANAGER->getSelectedVolumePreset(), WIN_MANAGER->getSelectedCustomPreset(CL_3D));
					WIN_MANAGER->renderLater_GridView(true);
				}
			}
		}

	}
	break;
	case QEvent::Enter:
	{
		QCursor cur = QCursor(Qt::PointingHandCursor);
		this->setCursor(cur);
	}
		break;
	case QEvent::Leave:
	{
		QCursor cur = QCursor(Qt::ArrowCursor);
		this->setCursor(cur);
	}
		break;
	default:
		break;
	}

	return QWidget::eventFilter(obj, evt);
}

void PresetDialog::OnCancel()
{
	QDialog::reject();
}
