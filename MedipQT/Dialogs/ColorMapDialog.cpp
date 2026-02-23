#include "stdafx.h"
#include "ColorMapDialog.h"
#include "UI/CustomHistogram.h"
#include "System/styleManager.h"
#include "System/stringManager.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"

#include "UI/AlphaColorMap.h"
#include "UI/HoverPoints.h"

ColorMapDialog::ColorMapDialog(int type, QWidget *parent /* = NULL */, bool hideWL /*= false*/)
	:QDialog(parent)
	, groupBox(NULL)
	, pointGroup(NULL)
{
	setStyleSheet("background-color: rgba(82, 82, 82, 255);");
	setMouseTracking(true);
	setWindowTitle(tr("Color Table Setting"));
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setFixedSize(530, 150);
	
	bChanged = false;
	bAlphaChanged = false;
	bWLChanged = false;
	bApply = false;
	QVBoxLayout * layout = new QVBoxLayout(this);
	QVBoxLayout * mapLayout = new QVBoxLayout();
	pointLayout = new QHBoxLayout();
	QHBoxLayout * btnLayout = new QHBoxLayout();
	int nWidth, nLevel;
	int nConfigWidth, nConfigLevel;

	layout->addLayout(mapLayout);
	layout->addLayout(pointLayout);
	layout->addLayout(btnLayout);
	layout->setContentsMargins(0, 0, 0, 0);
	pointLayout->setContentsMargins(0, 0, 0, 0);
	btnLayout->setContentsMargins(0, 0, 10, 10);
	mapLayout->setContentsMargins(5, 0, 0, 0);
	//combo (read only)
	QLabel * labelWindow = new QLabel(this);
	isVolume = type;
	m_preset = (isVolume ?
		WIN_MANAGER->getSelectedVolumePreset() : WIN_MANAGER->getSelectedPreset());
	m_custom = (WIN_MANAGER->getSelectedCustomPreset(type));
	switch (type)
	{
	case CL_2D:
		WIN_MANAGER->getConfigWL(QString("SCREEN_WL%1").arg(m_custom<= -1? QString::number(m_preset):""), nConfigWidth, nConfigLevel);
		nWidth = WIN_MANAGER->getWindowWidth();
		nLevel = WIN_MANAGER->getWindowLevel();
		isVolume = false;
		labelWindow->setText(STRING_MANAGER->getString(STR_2D_SCREEN));
		break;
	case CL_3D:
		WIN_MANAGER->getConfigWL(QString("VOLUME_WL%1").arg(m_custom <= -1 ? QString::number(m_preset) : ""), nConfigWidth, nConfigLevel);
		nWidth = WIN_MANAGER->getVolumeWidth();
		nLevel = WIN_MANAGER->getVolumeLevel();
		isVolume = true;
		labelWindow->setText(STRING_MANAGER->getString(STR_3D_VOLUME));
		break;
	default:
		break;
	}
	labelWindow->setFixedSize(90, 15);
	//layout add widget
	mapLayout->addWidget(labelWindow);
	
	//setgeometry
	colormap = new AlphaColorMap(this, type, true);
	connect(colormap, SIGNAL(sig_AddPoints()), this, SLOT(CheckPoints()));
	connect(colormap, SIGNAL(sig_MovePoints(int, qreal)), this, SLOT(MovePoint(int, qreal)));
	colormap->setFixedSize(512, 70);
	colormap->setType(type);
	colormap->setPreset((SLICE_PRESET)m_preset, false, m_custom);
	mapLayout->addWidget(colormap);
	
	if (type == CL_3D)
	{
		alphaPoints = new HoverPoints(type, (SLICE_PRESET)m_preset, colormap, m_custom);
		connect(alphaPoints, SIGNAL(ChangePoint()), this, SLOT(OnChangeAlpha()));
		alphaPoints->setFixedSize(512, 70);
	}
	else
		alphaPoints = NULL;

	if (!hideWL)
	{
		QLabel * labelEmpty = new QLabel(this);
		labelEmpty->setText(QString(" Width/Level (%1,%2)").arg(nWidth).arg(nLevel));
		labelEmpty->setFixedSize(340, 25);

		if ((nConfigWidth != nWidth) || (nConfigLevel != nLevel))
		{
			bApply = true;
			bWLChanged = true;
			labelEmpty->setStyleSheet(QString("color: red;"));
		}
	
		/*현재 width, level을 적용할 것인지 표시*/
		includeWL = new QCheckBox("Include Width/Level");
		includeWL->setFixedSize(130, 25);
		//현재 width, level이 동일하면 해당 위젯 hide
	
		btnLayout->addWidget(labelEmpty);
		btnLayout->addWidget(includeWL);

		if (!bApply)
			includeWL->hide();
	}
	else
	{
		QWidget *emptyBox = new QWidget(this);
		emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		btnLayout->addWidget(emptyBox);
	}
	QPushButton *resetBtn = new QPushButton(this);
	resetBtn->setText(STRING_MANAGER->getString(STR_RESET));
	resetBtn->setFixedWidth(50);
	resetBtn->setFixedHeight(25);
	resetBtn->setStyleSheet(STYLE_MANAGER->buttonNormal);

	btnLayout->addWidget(resetBtn);

	applyBtn = new QPushButton(this);
	applyBtn->setText(STRING_MANAGER->getString(STR_APPLY));
	applyBtn->setFixedWidth(50);
	applyBtn->setFixedHeight(25);

	if(bApply)
		applyBtn->setStyleSheet(STYLE_MANAGER->buttonChange);
	else
		applyBtn->setStyleSheet(STYLE_MANAGER->buttonNormal);
	//layout add widget
	
	btnLayout->addWidget(applyBtn);

	QPushButton *cancelBtn = new QPushButton(this);
	cancelBtn->setText(STRING_MANAGER->getString(STR_CANCEL));
	cancelBtn->setFixedWidth(50);
	cancelBtn->setFixedHeight(25);
	cancelBtn->setStyleSheet(STYLE_MANAGER->buttonNormal);
	btnLayout->addWidget(cancelBtn);
	
	connect(applyBtn, &QPushButton::clicked, this, &ColorMapDialog::OnApply);
	connect(cancelBtn, &QPushButton::clicked, this, &ColorMapDialog::OnCancel);
	connect(resetBtn, &QPushButton::clicked, this, &ColorMapDialog::OnReset);

	setLayout(layout);
	
}

ColorMapDialog::~ColorMapDialog()
{
}

QGradientStops ColorMapDialog::getColorMapStops()
{
	if (colormap)
		return colormap->getPresetStops((SLICE_PRESET)m_preset, isVolume, false, m_custom);
	
	return QGradientStops();
}

void ColorMapDialog::setColorMapStops(QGradientStops & stops)
{
	if (colormap)
		colormap->setPresetStops(stops, isVolume);
}

bool ColorMapDialog::isWLApply()
{
	if (bWLChanged && includeWL->isChecked())
		return true;

	return false;
}

bool ColorMapDialog::eventFilter(QObject * obj, QEvent * e)
{
	QMouseEvent *mEv = (QMouseEvent *)e;
	int type = e->type();
	static bool press = false;
	static bool move = false;

	if ((QEvent::MouseButtonPress == type))
	{
		if (mEv->button() & Qt::RightButton)
			RemovePoint(obj->objectName().toInt());
		else if (mEv->button() & Qt::LeftButton)
			press = true;
	}
	else if ((QEvent::MouseButtonRelease == type))
	{
		if (press)
		{
			press = false;
			if (!move)
			{
				//To-DO
				QColorDialog dlg(this);
				QColor curColor = colormap->getColorPoint(obj->objectName().toInt());
				dlg.setCurrentColor(curColor);
				if (dlg.exec() == QDialog::Accepted)
				{
					if (colormap)
						colormap->changeColorPoint(obj->objectName().toInt(), dlg.selectedColor());
				}
			}
			move = false;
		}
	}
	else if ((QEvent::MouseMove == type))
	{
		if (press)
		{
			move = true;
			if (colormap)
			{
				QPoint newPos = colormap->mapFromParent(mEv->windowPos().toPoint());
				
				colormap->moveColorPoint(obj->objectName().toInt(), ((float)newPos.x() / (colormap->width() - 1)));
			}
		}
	}
	
	return false;
}

void ColorMapDialog::OnCancel()
{
	close();
}

void ColorMapDialog::OnReset()
{
	if(colormap)
		colormap->setPreset((SLICE_PRESET)m_preset, true, m_custom);
	
	if(alphaPoints)
		alphaPoints->ResetAlphaPoint();
}

void ColorMapDialog::CheckPoints()
{
	if ((!bApply) && colormap->isColorChange())
	{
		bApply = true;
		applyBtn->setStyleSheet(STYLE_MANAGER->buttonChange);
	}

	if (NULL == pointGroup)
	{
		groupBox = new QGroupBox();
		groupBox->setFixedSize(this->width(), 15);
		groupBox->setStyleSheet(STYLE_MANAGER->groupBoxPoints);
		pointGroup = new QButtonGroup();
	
		pointLayout->addWidget(groupBox);
	}
	
	QList<QAbstractButton *> buttons = pointGroup->buttons();
	
	if (buttons.size() != 0)
	{
		pointLayout->removeWidget(groupBox);
		groupBox->deleteLater();
		groupBox = new QGroupBox();
		groupBox->setFixedSize(this->width(), 15);
		groupBox->setStyleSheet(STYLE_MANAGER->groupBoxPoints);
		pointLayout->addWidget(groupBox);
	}

	QVector<qreal> points;
	if (colormap)
	{
		points = colormap->getColorPoints();
		for (int i = 0; i < points.size(); i++)
		{
			QPushButton * addBtn = new QPushButton(tr("^"),groupBox);
			int left = (points.at(i) * (525-13))+2;
			addBtn->setGeometry(QRect(left, 0, 8, 10));
			addBtn->setStyleSheet(STYLE_MANAGER->buttonPoints);
			addBtn->setObjectName(QString::number(i));
			addBtn->installEventFilter(this);
			addBtn->setMouseTracking(true);
			pointGroup->addButton(addBtn, i);
		}
	}
}

void ColorMapDialog::MovePoint(int oldPos, qreal newPos)
{
	if ((!bApply) && colormap->isColorChange())
	{
		bApply = true;
		applyBtn->setStyleSheet(STYLE_MANAGER->buttonChange);
	}
	if ((oldPos == -1) && (oldPos == newPos))
		return ;

	QPushButton * moveBtn = qobject_cast<QPushButton*>(pointGroup->button(oldPos));
	int left = (newPos * (525 - 13)) + 2;
	moveBtn->move(left, 0);
}

void ColorMapDialog::RemovePoint(int id)
{
	if (colormap)
	{
		if (colormap->getColorPoints().size() <= 2)
			return;
		if ((id == 0)
			|| (id == (colormap->getColorPoints().size() - 1))) //양쪽 끝 삭제 불가
			return;
		
		colormap->removeColorPoint(id);
	}
}

void ColorMapDialog::OnChangeAlpha()
{
	if(alphaPoints)
	{
		if ((!bApply) && alphaPoints->isChange())
		{
			bApply = true;
			applyBtn->setStyleSheet(STYLE_MANAGER->buttonChange);
		}
	}
}

void ColorMapDialog::OnApply()
{
	bChanged = colormap->isColorChange();
	if (bChanged)
		colormap->applyCustomStops();
	if (alphaPoints)
	{
		bAlphaChanged = alphaPoints->ApplyAlpha();
	}

	close();
}
