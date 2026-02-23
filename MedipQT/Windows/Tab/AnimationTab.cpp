#include "stdafx.h"
#include "AnimationTab.h"
#include "ProductManager.h"
#include "StyleManager.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "LicenseManager.h"
#include "ResourceManager.h"
#include "Main/MainSegmentWidget.h"
#include "Main/MainAnalWidget.h"
#include "Dialogs/AnnoPathUsageDlg.h"

#include "DataContext.h"

AnimationTab::AnimationTab(QWidget* parent /*= NULL*/) : 
	CollapseWidget(QString(), parent)
{
	int nRow = 0;

	QGroupBox* boxMode = new QGroupBox(this);
	boxMode->setTitle("Animation Points");
	QHBoxLayout* boxMain = new QHBoxLayout;
	boxMode->setLayout(boxMain);

	m_listDefault = new QTreeWidget(this);
	m_listDefault->setColumnCount(1);
	m_listDefault->setContentsMargins(0, 0, 0, 5);
	m_listDefault->setHeaderHidden(true);
	m_listDefault->setStyleSheet(STYLE_MANAGER->treeAniList);
	m_listDefault->setRootIsDecorated(false);
	m_listDefault->setMinimumWidth(20);
	m_listDefault->setContextMenuPolicy(Qt::CustomContextMenu);
	m_listDefault->setSelectionMode(QAbstractItemView::ExtendedSelection);

	m_vecPath.push_back(m_listDefault);
	int val = 0;
	WIN_MANAGER->aniCount.push_back(val);

	connect(m_listDefault, &QWidget::customContextMenuRequested, this, &AnimationTab::slot_OnContext);

	QWidget* emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	tabList = new QTabWidget(this);
	tabList->setStyleSheet(STYLE_MANAGER->listTabWidget);
	tabList->addTab(m_listDefault, "Group 1");
	tabList->addTab(emptyBox, "+");
	tabList->setTabsClosable(true);
	tabList->setCurrentIndex(0);
	boxMain->addWidget(tabList);

	connect(tabList, &QTabWidget::tabBarClicked, this, &AnimationTab::slot_OnTabClicked);
	connect(tabList, &QTabWidget::currentChanged, this, &AnimationTab::slot_OnTabChanged);
	connect(tabList, &QTabWidget::tabCloseRequested, this, &AnimationTab::slot_OnTabClear);

	addWidget(boxMode, nRow++);

	emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	addWidget(emptyBox, nRow, 1, QMargins(0, 0, 5, 0));

	m_chkSpiral = new QCheckBox(this);
	m_chkSpiral->setText("Spiral Curve");
	m_chkSpiral->setCheckable(true);
	m_chkSpiral->setChecked(false);
	m_chkSpiral->setFixedWidth(100);
	m_chkSpiral->setStyleSheet(STYLE_MANAGER->m_Checkbox);
	connect(m_chkSpiral, &QCheckBox::clicked, this, &AnimationTab::slot_OnSpiral);
	addWidget(m_chkSpiral, nRow, 3);

	m_btnCreate = new QPushButton(this);
	m_btnCreate->setText("Add Point");
	m_btnCreate->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnCreate->setCheckable(true);
	m_btnCreate->setChecked(false);
	m_btnCreate->setFixedWidth(100);
	connect(m_btnCreate, &QPushButton::clicked, this, &AnimationTab::slot_OnCreate);
	addWidget(m_btnCreate, nRow, 2);

	QPushButton* btnDelete = new QPushButton(this);
	btnDelete->setText("Delete");
	btnDelete->setFixedWidth(100);
	btnDelete->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(btnDelete, &QPushButton::clicked, this, &AnimationTab::slot_OnDelete);
	addWidget(btnDelete, nRow++, 2);

	m_actDel = new QAction(STRING_MANAGER->getString(STR_DELETE), this);
	connect(m_actDel, &QAction::triggered, this, &AnimationTab::slot_OnDelete);

	emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	addWidget(emptyBox, nRow, 1);
	QLabel* label = new QLabel(this);
	label->setText("Speed");
	addWidget(label, nRow);

	m_nSec = 20;
	m_comboSpeed = new QComboBox(this);
	m_comboSpeed->setObjectName("Combospeed");
	m_comboSpeed->addItem("Slow");
	m_comboSpeed->addItem("Normal");
	m_comboSpeed->addItem("Fast");
	m_comboSpeed->setStyleSheet(STYLE_MANAGER->comboBoxTab);
	m_comboSpeed->setCurrentIndex(1);
	m_comboSpeed->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	m_comboSpeed->installEventFilter(this);
	connect(m_comboSpeed, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnComboChanged(int)));
	addWidget(m_comboSpeed, nRow);


	m_btnRepeat = new QPushButton(this);
	m_btnRepeat->setIcon(RESOURCE_MANAGER->getIcon(ICON_ANIMATION_SINGLE));
	m_btnRepeat->setIconSize(QSize(34, 34));
	m_btnRepeat->setFixedSize(QSize(34, 34));
	m_btnRepeat->setMouseTracking(true);
	m_btnRepeat->installEventFilter(this);
	m_btnRepeat->setToolTip("Single playback");
	m_btnRepeat->setStyleSheet("color : black;");
	m_btnRepeat->setObjectName("ButtonRepeat");

	connect(m_btnRepeat, &QPushButton::clicked, this, &AnimationTab::slot_OnPlayback);
	addWidget(m_btnRepeat, nRow);

	m_btnPlay = new QPushButton(this);
	m_btnPlay->setText("Play");
	m_btnPlay->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnPlay->setCheckable(true);
	m_btnPlay->setChecked(false);
	connect(m_btnPlay, &QPushButton::clicked, this, &AnimationTab::slot_OnPlay);

	addWidget(m_btnPlay, nRow++, 1);

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	addWidget(emptyBox0, nRow++, 0);

	m_AniTimer = new QTimer(this);
	if (m_AniTimer)
		m_AniTimer->stop();
	connect(m_AniTimer, SIGNAL(timeout()), this, SLOT(slot_OnAnimate()));

	m_playback = NONE;
	m_group = 0;
	QString title = "Path Animation";
	setTitle(title);

	setOpenWidget(false);
}

void AnimationTab::AddAnipoint(int index, int group, QTreeWidgetItem* item)
{
	if (0 > group || group >= m_vecPath.count()) return;

	int rIndex = index + WIN_MANAGER->getAniStartPoint(group);

	QVector3D pos = WIN_MANAGER->aniLine.at(rIndex);

	if (NULL == item)
	{
		item = new QTreeWidgetItem(m_vecPath.at(group));

		QString posText = WIN_MANAGER->ConvertText_VoxelPosToSliceImagePos(pos.x(), pos.y(), pos.z());
		item->setText(0, QString("(%1)").arg(posText));
	}

	m_vecPath.at(group)->insertTopLevelItem(index, item);
}

void AnimationTab::UpdateLength(int group)
{
	if (0 > group || group >= m_vecPath.count()) return;

	float _len = WIN_MANAGER->GetAniLength(group, m_chkSpiral->isChecked(), (m_group != group));

	QString strLen = "";

	if (_len > 0)
		strLen = QString("(%1 mm)").arg(_len);

	tabList->setTabText(group, QString("Group %1%2").arg(group + 1).arg(strLen));
}

void AnimationTab::UpdateAniList()
{
	int size = m_vecPath.count();

	for (int i = 0; i < size; i++)
	{
		m_vecPath.last()->clear();

		if (m_vecPath.last() != m_listDefault)
		{
			QTreeWidget* pathList = m_vecPath.takeLast();
			tabList->removeTab(tabList->count() - 2);
			pathList->deleteLater();
		}
	}

	m_btnCreate->setChecked(false);
	slot_OnCreate();

	QList<QTreeWidgetItem*> items;
	size = WIN_MANAGER->aniCount.count();
	for (int i = 0; i < size; i++)
	{
		if (0 != i)
			createNewTab();

		bool res = false;
		int index = WIN_MANAGER->getAniStartPoint(i);
		for (int j = 0; j < WIN_MANAGER->aniCount.at(i); j++)
		{
			res = true;

			QVector3D pos = WIN_MANAGER->aniLine.at(index + j);
			QTreeWidgetItem* item = new QTreeWidgetItem(m_vecPath.at(i));

			QString posText = WIN_MANAGER->ConvertText_VoxelPosToSliceImagePos(pos.x(), pos.y(), pos.z());
			item->setText(0, QString("(%1)").arg(posText));

			items.append(item);
		}
		if (res)
			m_vecPath.at(i)->insertTopLevelItems(0, items);

		items.clear();
	}

	updateTabname();

	m_btnCreate->setText("Add Point");

}

void AnimationTab::ClearAniList()
{
	StopAnimation();

	m_listDefault->clear();
}

void AnimationTab::StopAnimation()
{
	if (WORK_ANNOTATION_PATH_PLAY == WIN_MANAGER->getWorkMode())
		WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);

	if (ANAL_WORK_PATH_3D_PLAY == WIN_MANAGER->getAnalWorkMode())
		WIN_MANAGER->mainAnalWidget->setWorkMode(ANAL_WORK_NONE);

	if (WIN_MANAGER->bStartAni)
		WIN_MANAGER->bStartAni = false;

	if (m_btnPlay->isChecked())
		m_btnPlay->setChecked(false);

	if (0 != m_btnPlay->text().compare("Play"))
		m_btnPlay->setText("Play");

	if (!m_comboSpeed->isEnabled())
		m_comboSpeed->setDisabled(false);

	if (!tabList->isEnabled())
		tabList->setDisabled(false);
}

void AnimationTab::CompleteAnimation()
{
	if (m_btnCreate)
	{
		m_btnCreate->setChecked(false);
		m_btnCreate->setText("Add Point");
	}
}

bool AnimationTab::isSpiral()
{
	if (m_chkSpiral)
		return m_chkSpiral->isChecked();

	return false;
}

QTreeWidget* AnimationTab::takeList(int group)
{
	if (group < 0 ||
		group >= m_vecPath.count()) return nullptr;

	if (m_group == group)
	{
		m_group = 0;
		tabList->blockSignals(true);
		tabList->setCurrentIndex(0);
		tabList->blockSignals(false);
	}

	QTreeWidget* pathList = m_vecPath.takeAt(group);
	tabList->removeTab(group);
	updateTabname();

	return pathList;
}

void AnimationTab::InsertList(QTreeWidget* pathList, int group)
{
	if (group < 0) return;

	m_vecPath.insert(group, pathList);
	tabList->insertTab(group, pathList, QString("Group %1").arg(group + 1));
	if (tabList->currentIndex() != group)
		tabList->setCurrentIndex(group);
}

QTreeWidgetItem* AnimationTab::takeItem(int index, int group)
{
	int count = m_vecPath.at(group)->topLevelItemCount();
	if (index < 0 ||
		index >= count) return nullptr;

	StopAnimation();

	return m_vecPath.at(group)->takeTopLevelItem(index);
}

bool AnimationTab::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == NULL) return false;

	QEvent::Type _type = event->type();

	if (watched->objectName().contains("Slider"))
	{
		if (_type == QEvent::Scroll ||
			_type == QEvent::Wheel)
			return false;
	}
	else if (watched->objectName().contains("Combo"))
	{
		if (_type == QEvent::Wheel)
			return true;
	}

	else if (watched->objectName().contains("Edit"))
	{
		if (_type == QEvent::FocusIn)
			WIN_MANAGER->setMoveFocus(false);
	}
	else if (watched->objectName().contains("Repeat"))
	{
		static QIcon singleHover = RESOURCE_MANAGER->getIcon(ICON_ANIMATION_SINGLE_HOVER);
		static QIcon singleLeave = RESOURCE_MANAGER->getIcon(ICON_ANIMATION_SINGLE);
		static QIcon singlePress = RESOURCE_MANAGER->getIcon(ICON_ANIMATION_SINGLE_PRESS);

		static QIcon r1Hover = RESOURCE_MANAGER->getIcon(ICON_ANIMATION_PLAY_AB_HOVER);
		static QIcon r1Leave = RESOURCE_MANAGER->getIcon(ICON_ANIMATION_PLAY_AB);
		static QIcon r1Press = RESOURCE_MANAGER->getIcon(ICON_ANIMATION_PLAY_AB_PRESS);

		static QIcon r2Hover = RESOURCE_MANAGER->getIcon(ICON_ANIMATION_PLAY_BA_HOVER);
		static QIcon r2Leave = RESOURCE_MANAGER->getIcon(ICON_ANIMATION_PLAY_BA);
		static QIcon r2Press = RESOURCE_MANAGER->getIcon(ICON_ANIMATION_PLAY_BA_PRESS);
		const static char* pressSt = "PressState";

		if (!(_type == QEvent::MouseButtonPress ||
			_type == QEvent::HoverEnter ||
			_type == QEvent::HoverLeave ||
			_type == QEvent::MouseButtonRelease))
			return QWidget::eventFilter(watched, event);

		PLAYBACK_MODE mode = WIN_MANAGER->getPlaymode();

		switch (mode)
		{
		case START_RECURSIVE_MODE:
			if (_type == QEvent::MouseButtonPress)
			{
				m_btnRepeat->setIcon(r1Press);
				m_btnRepeat->setProperty(pressSt, QVariant(true));
			}
			else if (_type == QEvent::HoverEnter)
			{
				if (!m_btnRepeat->property(pressSt).toBool())
					m_btnRepeat->setIcon(r1Hover);
			}
			else if (_type == QEvent::HoverLeave)
			{
				if (!m_btnRepeat->property(pressSt).toBool())
					m_btnRepeat->setIcon(r1Leave);
			}
			else if (_type == QEvent::MouseButtonRelease)
			{
				m_btnRepeat->setIcon(r1Leave);
				m_btnRepeat->setProperty(pressSt, QVariant(false));
			}
			break;
		case END_RECURSIVE_MODE:
			if (_type == QEvent::MouseButtonPress)
			{
				m_btnRepeat->setIcon(r2Press);
				m_btnRepeat->setProperty(pressSt, QVariant(true));
			}
			else if (_type == QEvent::HoverEnter)
			{
				if (!m_btnRepeat->property(pressSt).toBool())
					m_btnRepeat->setIcon(r2Hover);
			}
			else if (_type == QEvent::HoverLeave)
			{
				if (!m_btnRepeat->property(pressSt).toBool())
					m_btnRepeat->setIcon(r2Leave);
			}
			else if (_type == QEvent::MouseButtonRelease)
			{
				m_btnRepeat->setIcon(r2Leave);
				m_btnRepeat->setProperty(pressSt, QVariant(false));
			}
			break;
		default:
		case SINGLE_MODE:
			if (_type == QEvent::MouseButtonPress)
			{
				m_btnRepeat->setIcon(singlePress);
				m_btnRepeat->setProperty(pressSt, QVariant(true));
			}
			else if (_type == QEvent::HoverEnter)
			{
				if (!m_btnRepeat->property(pressSt).toBool())
					m_btnRepeat->setIcon(singleHover);
			}
			else if (_type == QEvent::HoverLeave)
			{
				if (!m_btnRepeat->property(pressSt).toBool())
					m_btnRepeat->setIcon(singleLeave);
			}
			else if (_type == QEvent::MouseButtonRelease)
			{
				m_btnRepeat->setIcon(singleLeave);
				m_btnRepeat->setProperty(pressSt, QVariant(false));
			}
			break;
		}


	}
	return QWidget::eventFilter(watched, event);
}

void AnimationTab::updateTabname()
{
	for (int i = 0; i < tabList->count() - 1; i++)
	{
		float _len = WIN_MANAGER->GetAniLength(i, m_chkSpiral->isChecked(), (m_group != i));

		QString strLen = "";

		if (_len > 0)
			strLen = QString("(%1 mm)").arg(_len);

		tabList->setTabText(i, QString("Group %1%2").arg(i + 1).arg(strLen));
	}
}

void AnimationTab::createNewTab()
{
	QTreeWidget* listPath = new QTreeWidget(this);
	listPath->setColumnCount(1);
	listPath->setContentsMargins(0, 0, 0, 5);
	listPath->setHeaderHidden(true);
	listPath->setStyleSheet(STYLE_MANAGER->treeAniList);
	listPath->setRootIsDecorated(false);
	listPath->setMinimumWidth(20);
	listPath->setContextMenuPolicy(Qt::CustomContextMenu);
	listPath->setSelectionMode(QAbstractItemView::ExtendedSelection);
	listPath->installEventFilter(this);

	m_vecPath.push_back(listPath);
	tabList->blockSignals(true);
	tabList->insertTab(m_vecPath.count() - 1, listPath, QString("Group %1").arg(m_vecPath.count()));
	tabList->blockSignals(false);

	if (WIN_MANAGER->aniCount.size() < m_vecPath.size())
	{
		int val = 0;
		WIN_MANAGER->aniCount.push_back(val);
	}
}

bool AnimationTab::isShowUsage()
{
	QString value;

	bool res = WIN_MANAGER->getConfigValue(ELEMENT_FILE, "ANNO_PATH_USAGE", value);

	if ((!res) || (1 == value.toInt())) return true;

	return false;
}

QVector3D AnimationTab::getAnimationDistance(int index, float nDistance, float ttMSec, float dtMSec)
{
	int gap = m_playback > START_RECURSIVE_MODE ? -1 : 1;
	const int _AniCount = WIN_MANAGER->aniCount.at(m_group);
	QVector<QVector3D>* Lines = m_chkSpiral->isChecked() ? &(WIN_MANAGER->spiralLine) : &(WIN_MANAGER->aniLine);

	QVector3D vecDiff;
	QVector3D vecCurr;
	if ((index + gap) < 0 || _AniCount <= (index + gap))
	{
		vecDiff.setX(-1);
		vecDiff.setY(-1);
		vecDiff.setZ(-1);

		return vecDiff;
	}

	int rIndex = WIN_MANAGER->getAniStartPoint(m_group) + index;

	int nRange = 1;

	if (m_chkSpiral->isChecked())
	{
		int nStart, nEnd;

		nStart = WIN_MANAGER->spiralPos.find(rIndex).value();
		nEnd = WIN_MANAGER->spiralPos.find(rIndex + gap).value();

		rIndex = nStart;
		nRange = abs(nEnd - nStart);
		if (nRange < 1)
			nRange = 0;
	}

	float currPos = (float)nRange * (dtMSec / ttMSec); //0.1~1.0

	if (m_chkSpiral->isChecked() && (nRange > 1))
	{
		if (gap > 0)
			rIndex += currPos;
		else
			rIndex -= currPos;

		vecCurr = Lines->at((int)rIndex);
	}
	else
	{
		vecDiff = Lines->at(rIndex) - Lines->at(rIndex + gap);
		vecCurr = QVector3D(Lines->at(rIndex).x() - (vecDiff.x() * currPos), Lines->at(rIndex).y() - (vecDiff.y() * currPos),
			Lines->at(rIndex).z() - (vecDiff.z() * currPos));
	}

	return vecCurr;
}

float AnimationTab::getAnimationTTDistance(int aniIndex)
{
	int gap = m_playback > START_RECURSIVE_MODE ? -1 : 1;

	const int _AniCount = WIN_MANAGER->aniCount.at(m_group);
	QVector<QVector3D>* Lines = &(WIN_MANAGER->aniLine);
	if ((aniIndex + gap) < 0 || _AniCount <= (aniIndex + gap)) return -1;

	int rIndex = WIN_MANAGER->getAniStartPoint(m_group) + aniIndex;

	return sqrt(pow(Lines->at(rIndex).x() - Lines->at(rIndex + gap).x(), 2) + pow(Lines->at(rIndex).y() - Lines->at(rIndex + gap).y(), 2)
		+ pow(Lines->at(rIndex).z() - Lines->at(rIndex + gap).z(), 2));
}

void AnimationTab::slot_OnPlay()
{
	if (!DATA_CONTEXT->volume_data.isValidate() || WIN_MANAGER->aniCount.at(m_group) <= 1)
	{
		StopAnimation();
		return;
	}

	if (m_btnPlay->isChecked())
	{
		WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_ANNOTATION_PATH_PLAY);
		if (WIN_MANAGER->mainAnalWidget)
			WIN_MANAGER->mainAnalWidget->setWorkMode(ANAL_WORK_PATH_3D_PLAY);

		WIN_MANAGER->bStartAni = true;
		WIN_MANAGER->AniType = WT_AXIAL;
		m_playback = NONE;
		m_Repeat = false;
		if (m_AniTimer)
			m_AniTimer->start();
		m_btnPlay->setText("Stop");
		tabList->setDisabled(true);
		m_comboSpeed->setDisabled(true);
	}
	else
	{
		StopAnimation();
	}
}

void AnimationTab::slot_OnDelete()
{
	QList<QTreeWidgetItem*>& list = m_vecPath.at(m_group)->selectedItems();

	if (list.size() <= 0)
	{
		QMessageBox::warning(this, tr("Path deletion"), tr("Please select items to delete."));
		return;
	}

	for (int i = 0; i < list.size(); i++)
	{
		int index = m_vecPath.at(m_group)->indexOfTopLevelItem(list[i]);

		ACTION_MANAGER->action_Annotation_Path_Del(index, m_group);
	}
}

void AnimationTab::slot_OnAnimate()
{
	static bool init = false;
	static int aniIndex = 0;
	static int gap = m_playback > START_RECURSIVE_MODE ? -1 : 1;
	const int _AniCount = WIN_MANAGER->aniCount.at(m_group);

	if ((!DATA_CONTEXT->volume_data.isValidate()) || (WIN_MANAGER->AniType == WT_NONE))
	{
		WIN_MANAGER->aniMove = false;
		WIN_MANAGER->AniType = WT_NONE;
		init = true;
		aniIndex = 0;
		m_AniTimer->stop();
		StopAnimation();
		return;
	}

	if (!WIN_MANAGER->bStartAni)
	{
		WIN_MANAGER->aniMove = false;
		WIN_MANAGER->AniType = WT_NONE;
		init = true;
		aniIndex = 0;
		m_AniTimer->stop();
		StopAnimation();
		return;
	}

	if (_AniCount < 2)
	{
		m_AniTimer->stop();
		StopAnimation();
		return;
	}

	static QTime startTime = startTime.currentTime();
	static bool dotDraw = true;
	static float nDistance = getAnimationTTDistance(aniIndex);
	static float ttMSec = nDistance * m_nSec;
	static float dtMSec = startTime.msecsTo(startTime.currentTime());
	bool state;
	if (init)
	{
		gap = m_playback > START_RECURSIVE_MODE ? -1 : 1;

		startTime = startTime.currentTime();
		dotDraw = true;
		init = false;
		nDistance = getAnimationTTDistance(aniIndex);
		ttMSec = nDistance * m_nSec;
		dtMSec = startTime.msecsTo(startTime.currentTime());
	}

	if (-1 == nDistance)
	{
		WIN_MANAGER->aniMove = false;
		WIN_MANAGER->AniType = WT_NONE;
		init = true;
		aniIndex = 0;
		m_AniTimer->stop();
		StopAnimation();
		return;
	}

	if ((dtMSec < ttMSec) && ((dtMSec = startTime.msecsTo(startTime.currentTime())) >= ttMSec))
		dtMSec = ttMSec;
	else
		dtMSec = startTime.msecsTo(startTime.currentTime());

	if (dtMSec > ttMSec)
		dotDraw = false;

	if (dotDraw)
	{
		WIN_MANAGER->LinePos = getAnimationDistance(aniIndex, nDistance, ttMSec, dtMSec);

		if (nDistance < 1.f)
			dotDraw = false;

		if (WIN_MANAGER->LinePos.x() <= 0)
		{
			WIN_MANAGER->aniMove = false;
			WIN_MANAGER->AniType = WT_NONE;
			init = true;
			aniIndex = 0;
			m_AniTimer->stop();
			StopAnimation();
			return;
		}

		WIN_MANAGER->aniMove = true;

		OpenGLWidget* volumeView = WIN_MANAGER->mainSegmentWidget->getViewVolume();
		state = volumeView->getUpdateFrameState();

		if (state == true) // VR On
		{
			VOLUME_DATA& vd = DATA_CONTEXT->volume_data;

			float cx = 0, cy = 0, cz = 0;

			cx = vd.getSpaceX() * WIN_MANAGER->LinePos.x() - vd.getSizeX() * 0.5f;
			cy = vd.getSpaceY() * WIN_MANAGER->LinePos.y() - vd.getSizeY() * 0.5f;
			cz = vd.getSpaceZ() * WIN_MANAGER->LinePos.z() - vd.getSizeZ() * 0.5f;

			WIN_MANAGER->matMoveCameraPos.set(cx, cy, cz);
		}
		else
		{
			WindowBase* winA, * winC, * winS;

			winA = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);
			winC = WIN_MANAGER->mainSegmentWidget->getWindow(WT_CORONAL);
			winS = WIN_MANAGER->mainSegmentWidget->getWindow(WT_SAGITTAL);

			winA->setDepth(WIN_MANAGER->LinePos.z());
			winC->setDepth(WIN_MANAGER->LinePos.y());
			winS->setDepth(WIN_MANAGER->LinePos.x());
		}
	}
	else
	{
		init = true;
		aniIndex += gap;

		if ((((_AniCount - 1) <= aniIndex))
			|| ((aniIndex <= 0)))
		{
			m_playback = WIN_MANAGER->getPlaymode();
			switch (m_playback)
			{
			case START_RECURSIVE_MODE:
			{
				aniIndex = 0;
				init = true;
			}
			break;
			case END_RECURSIVE_MODE:
			{
				if (m_Repeat)
				{
					m_playback = START_RECURSIVE_MODE;
					aniIndex = 0;
				}
				else
				{
					aniIndex = _AniCount - 1;
				}

				init = true;

				m_Repeat = !m_Repeat;
			}
			break;
			case SINGLE_MODE:
			default:
			{
				WIN_MANAGER->aniMove = false;
				aniIndex = 0;
				init = true;

				StopAnimation();
			}
			break;
			}
		}
	}

	if (!state || (!WIN_MANAGER->bStartAni))
		WIN_MANAGER->renderLater_GridView();
}

void AnimationTab::slot_OnSpiral()
{
	if (!DATA_CONTEXT->volume_data.isValidate())
		return;

	StopAnimation();
	updateTabname();
}

void AnimationTab::slot_OnTabClicked(int index)
{
	if (index == m_vecPath.count())
	{
		int preIndex = 0;

		if (0 >= WIN_MANAGER->aniCount.count())
			preIndex = 0;
		else
			preIndex = WIN_MANAGER->aniCount.indexOf(0);

		if (0 > preIndex)
		{
			ACTION_MANAGER->action_Annotation_Path_new();
		}
		else
		{
			tabList->blockSignals(true);
			tabList->setCurrentIndex(preIndex);
			tabList->blockSignals(false);

			QMessageBox::warning(this, "New path group", "A group with no path exists.");
			m_group = preIndex;
			return;
		}
	}
	m_group = index;

	if (m_chkSpiral->isChecked())
		UpdateLength(m_group);
}

void AnimationTab::slot_OnTabClear(int index)
{
	if (index == m_vecPath.count())
	{
		QMessageBox::warning(this, "Path group deletion", "The Add button can not be deleted.");
		return;
	}

	if ((0 == index && WIN_MANAGER->aniCount.at(index) > 0)
		|| index != 0)
		ACTION_MANAGER->action_Annotation_Path_Clear(index);
}

void AnimationTab::slot_OnTabChanged(int index)
{
	if (index >= m_vecPath.count())
	{
		int preIndex = WIN_MANAGER->aniCount.indexOf(0);

		if (-1 == preIndex)
			preIndex = index - 1;

		tabList->blockSignals(true);
		tabList->setCurrentIndex(preIndex);
		tabList->blockSignals(false);

		m_group = preIndex;

		return;
	}
	m_group = index;

	if (m_chkSpiral->isChecked())
		UpdateLength(m_group);
}

void AnimationTab::slot_OnComboChanged(int index)
{
	m_nSec = (3 - index) * 10;
}

void AnimationTab::slot_OnContext(const QPoint& pos)
{
	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	if (NULL == m_listDefault->itemAt(pos))	return;

	QMenu menu(m_listDefault);
	menu.addAction(m_actDel);
	menu.exec(QCursor::pos());
}

void AnimationTab::slot_OnCreate()
{
	if (!DATA_CONTEXT->volume_data.isValidate())
	{
		if (m_btnCreate->isChecked())
		{
			m_btnCreate->setChecked(false);
			m_btnCreate->setText("Add Point");
		}
		return;
	}

	static bool showUsage = isShowUsage();
	if (m_btnCreate->isChecked())
	{
		WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_ANNOTATION_PATH);
		if (WIN_MANAGER->mainAnalWidget)
			WIN_MANAGER->mainAnalWidget->setWorkMode(ANAL_WORK_PATH);

		WIN_MANAGER->bCreateAni = true;
		WIN_MANAGER->renderLater_GridView(false);
		if (showUsage)
		{
			AnnoPathUsageDlg dlg(this);

			dlg.exec();

			if (dlg.isShowAgain())
			{
				showUsage = false;

				WIN_MANAGER->setConfigValue(ELEMENT_FILE, "ANNO_PATH_USAGE", QString("0"));
			}
		}
		m_btnCreate->setText("Complete");
	}
	else
	{
		WIN_MANAGER->mainSegmentWidget->setWorkMode(WORK_NONE);
		if (WIN_MANAGER->mainAnalWidget)
			WIN_MANAGER->mainAnalWidget->setWorkMode(ANAL_WORK_NONE);

		WIN_MANAGER->bCreateAni = false;

		m_btnCreate->setText("Add Point");

		WIN_MANAGER->renderLater_GridView(false);
	}
}

void AnimationTab::slot_OnPlayback()
{
	StopAnimation();

	PLAYBACK_MODE mode = WIN_MANAGER->getPlaymode();

	switch (mode)
	{
	case START_RECURSIVE_MODE: //TO END
	{
		m_btnRepeat->setIcon(RESOURCE_MANAGER->getIcon(ICON_ANIMATION_PLAY_BA_HOVER));
		m_btnRepeat->setToolTip("Repeat once(A-B-B-A)");
		WIN_MANAGER->setPlaymode(END_RECURSIVE_MODE);
	}
	break;
	case END_RECURSIVE_MODE: //TO SINGLE
	{
		m_btnRepeat->setIcon(RESOURCE_MANAGER->getIcon(ICON_ANIMATION_SINGLE_HOVER));
		m_btnRepeat->setToolTip("Single playback");
		WIN_MANAGER->setPlaymode(SINGLE_MODE);
	}
	break;
	case SINGLE_MODE: // TO START
	default:
	{
		m_btnRepeat->setIcon(RESOURCE_MANAGER->getIcon(ICON_ANIMATION_PLAY_AB_HOVER));
		m_btnRepeat->setToolTip("Repeat once(A-B-A-B)");
		WIN_MANAGER->setPlaymode(START_RECURSIVE_MODE);
	}
	break;
	}
}

