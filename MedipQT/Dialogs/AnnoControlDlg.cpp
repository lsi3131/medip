#include "stdafx.h"
#include "AnnoControlDlg.h"

#include "System/styleManager.h"
#include "System/resourceManager.h"
#include "System/stringManager.h"
#include "Actions/ActionManager.h"
#include "Windows/windowManager.h"
#include "Windows/Main/MainSegmentWidget.h"

AnnoControlDlg::AnnoControlDlg(QVector3D *currentPos, QWidget* parent, ANNOTATION_TYPE type)
	: QDialog(parent)
{
	ctrlType = type;
	addType = AT_TEXT;

	setWindowTitle(tr("Annotation Control Dialog"));
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	resize(500, 300);

	QVBoxLayout * layout = new QVBoxLayout(this);
	QVBoxLayout * ListLayout = new QVBoxLayout();
	QHBoxLayout * BtnLayout = new QHBoxLayout();
	layout->addLayout(ListLayout);
	layout->addLayout(BtnLayout);

	QLabel *labelAnno = new QLabel(this);
	labelAnno->setText(STRING_MANAGER->getString(STR_ANNO_LIST));
	ListLayout->addWidget(labelAnno);

	m_annoList = new QTreeWidget(this);
	m_annoList->setColumnCount(3);
	m_annoList->setColumnWidth(0, 20);
	m_annoList->setColumnWidth(1, 20);
	m_annoList->setHeaderHidden(true);
	m_annoList->setStyleSheet(STYLE_MANAGER->treeBasicList);
	m_annoList->setRootIsDecorated(false);

	connect(m_annoList, &QTreeWidget::itemClicked, this, &AnnoControlDlg::slot_OnColumnClick);
	connect(m_annoList, &QTreeWidget::itemDoubleClicked, this, &AnnoControlDlg::slot_OnColumnDoubleClick);

	/*annolist add item*/
	ListLayout->addWidget(m_annoList);
	AddAnnoList();

	QLabel *labelHU = new QLabel(this);
	labelHU->setText(tr("(%1, %2, %3)").arg(currentPos->x()).arg(currentPos->y()).arg(currentPos->z()));
	BtnLayout->addWidget(labelHU);

	if (ctrlType != AT_TEXT)
	{
		QComboBox * m_WindowCombo;
		m_WindowCombo = new QComboBox(this);
		m_WindowCombo->addItem(STRING_MANAGER->getString(STR_ANNO_TEXT));
		m_WindowCombo->addItem(STRING_MANAGER->getString(STR_ANNO_LENGTH));
		m_WindowCombo->addItem(STRING_MANAGER->getString(STR_ANNO_ANGLE));
		m_WindowCombo->addItem(STRING_MANAGER->getString(STR_ANNO_OVAL));
		m_WindowCombo->addItem(STRING_MANAGER->getString(STR_ANNO_ARROW));
		m_WindowCombo->addItem(STRING_MANAGER->getString(STR_ANNO_PROF_DRAW));
		m_WindowCombo->setStyleSheet(STYLE_MANAGER->comboBoxNormal);
		connect(m_WindowCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboChanged(int)));
		BtnLayout->addWidget(m_WindowCombo);
	}

	QPushButton *addBtn = new QPushButton(this);
	addBtn->setText(tr("Add"));
	addBtn->setFixedWidth(50);
	addBtn->setFixedHeight(25);
	addBtn->setStyleSheet(STYLE_MANAGER->buttonNormal);
	BtnLayout->addWidget(addBtn);

	QPushButton *seekBtn = new QPushButton(this);
	seekBtn->setText(tr("Check"));
	seekBtn->setFixedWidth(50);
	seekBtn->setFixedHeight(25);
	seekBtn->setStyleSheet(STYLE_MANAGER->buttonNormal);
	BtnLayout->addWidget(seekBtn);

	QPushButton *DelBtn = new QPushButton(this);
	DelBtn->setText(tr("Delete"));
	DelBtn->setFixedWidth(50);
	DelBtn->setFixedHeight(25);
	DelBtn->setStyleSheet(STYLE_MANAGER->buttonNormal);
	BtnLayout->addWidget(DelBtn);

	bAdd = false;

	connect(addBtn, &QPushButton::released, this, &AnnoControlDlg::OnAdd);
	connect(seekBtn, &QPushButton::released, this, &AnnoControlDlg::OnSeek);
	connect(DelBtn, &QPushButton::released, this, &AnnoControlDlg::OnDel);
	//	connect(m_annoList, &QTreeWidget::doubleClicked, this, &AnnoControlDlg::OnSeek);
	setLayout(layout);
}

AnnoControlDlg::~AnnoControlDlg()
{

}

void AnnoControlDlg::OnAdd()
{
	bAdd = true;
	close();
}

void AnnoControlDlg::AddAnnoList()
{
	m_annoList->clear();

	QList<QTreeWidgetItem *> items;
	int count = 0;
	for (int i = 0; i < WIN_MANAGER->anotationList.size(); i++)
	{
		ANNOTATION_TYPE type = WIN_MANAGER->anotationList.at(i)->getType();
		if (ctrlType == AT_TEXT && type != AT_TEXT)
			continue;

		QTreeWidgetItem * item = new QTreeWidgetItem(m_annoList);

		if (type == AT_TEXT)
		{
			AnnoString * str = (AnnoString *)WIN_MANAGER->anotationList.at(i);
			COLOR col = str->getColor();
			mip::VECTOR3 volumePos = str->getVolumePos();
			item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
			item->setIcon(1, RESOURCE_MANAGER->getIcon(str->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
			item->setText(2, tr("%5. Annotation Text \"%1\" (%2,%3,%4)").arg(str->getText())
				.arg(volumePos.x).arg(volumePos.y).arg(volumePos.z).arg(++count));
		}
		else if (type == AT_LEN)
		{
			AnnoLength *len = (AnnoLength *)WIN_MANAGER->anotationList.at(i);
			COLOR col = len->getColor();
			item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
			item->setIcon(1, RESOURCE_MANAGER->getIcon(len->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
			item->setText(2, tr("%1. Annotation Length (%2 mm)").arg(++count).arg(len->getLength() * 10));
		}

		else if (type == AT_ANGLE)
		{
			AnnoAngle *angl = (AnnoAngle *)WIN_MANAGER->anotationList.at(i);
			COLOR col = angl->getColor();
			item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
			item->setIcon(1, RESOURCE_MANAGER->getIcon(angl->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
			item->setText(2, tr("%1. Annotation Angle (%2%3)").arg(++count).arg(angl->getAngle()).arg(QString(QChar(0x00B0))));
		}
		else if (type == AT_ARROW)
		{
			AnnoArrow *arrow = (AnnoArrow *)WIN_MANAGER->anotationList.at(i);
			COLOR col = arrow->getColor();
			item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
			item->setIcon(1, RESOURCE_MANAGER->getIcon(arrow->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
			item->setText(2, tr("%1. Annotation Arrow").arg(++count));
		}
		else if (type == AT_RECTANGLE)
		{
			AnnoRectangle *rect = (AnnoRectangle *)WIN_MANAGER->anotationList.at(i);
			COLOR col = rect->getColor();
			item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
			item->setIcon(1, RESOURCE_MANAGER->getIcon(rect->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
			item->setText(2, tr("%1. Annotation Rectangle").arg(++count));
		}

		items.append(item);
	}

	m_annoList->insertTopLevelItems(0, items);
}

void AnnoControlDlg::OnDel()
{
	// 선택된 anno list item 삭제(action manager 사용) & close 후 render

	QList<QTreeWidgetItem *> & list = m_annoList->selectedItems();

	if (list.size() <= 0)
	{
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1005)).exec();
		return;
	}

	std::vector<muint32> indeces;
	for (int cnt = 0; cnt < list.size(); cnt++)
	{
		indeces.push_back(m_annoList->indexOfTopLevelItem(list[cnt]));
	}

	std::sort(indeces.begin(), indeces.end(), std::greater<muint32>());

	if (ctrlType == AT_ALL)
	{
		for (int i = 0; i < indeces.size(); i++)
		{
			int index = indeces[i];

			ACTION_MANAGER->action_Annotation_Del(NULL, index);

			m_annoList->takeTopLevelItem(indeces[i]);
		}
	}
	else if (ctrlType == AT_TEXT)
	{
		int count = 0;
		for (int i = 0; i < WIN_MANAGER->anotationList.size(); i++)
		{
			if (count >= indeces.size())	break;

			if (WIN_MANAGER->anotationList.at(i)->getType() != AT_TEXT) continue;

			int index = indeces[count];
			m_annoList->takeTopLevelItem(index);
			ACTION_MANAGER->action_Annotation_Del(NULL, index);
			count++;
		}
	}

}

void AnnoControlDlg::OnSeek()
{
	if (ctrlType != AT_TEXT)
	{
		QMessageBox::warning(NULL, "Not Supported", "Not Supported this function on Measurement tab.");
		return;
	}

	// 선택된 anno list 1개인지 확인 & 해당 위치로 view 위치 이동
	QList<QTreeWidgetItem *> & list = m_annoList->selectedItems();

	if (list.size() != 1)
	{
		QMessageBox::warning(this, tr("Annotation location"), tr("Please select an item to check location."));
		return;
	}
	int index = m_annoList->indexOfTopLevelItem(list[0]);
	auto * anno = WIN_MANAGER->anotationList.at(index);

	if (anno->getType() == AT_TEXT)
	{
		AnnoString* annoStr = (AnnoString*)anno;
		WindowBase * winA, *winC, *winS;

		winA = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);
		winC = WIN_MANAGER->mainSegmentWidget->getWindow(WT_CORONAL);
		winS = WIN_MANAGER->mainSegmentWidget->getWindow(WT_SAGITTAL);

		mip::VECTOR3 v = annoStr->getVolumePos();
		if (winA)
			winA->setDepth(v.z);

		if (winC)
			winC->setDepth(v.y);

		if (winS)
			winS->setDepth(v.x);
	}
	else if (anno->getType() == AT_LEN)
	{
		//TODO 1. Set depth? or,,,,
	}

	close();
}

void AnnoControlDlg::OnComboChanged(int index)
{
	addType = (ANNOTATION_TYPE)(index + 1);
}

void AnnoControlDlg::slot_OnColumnClick(QTreeWidgetItem *item, int column)
{
	if (item == NULL) return;

	if (column == 1) //visible - invisible
	{
		muint32 index = m_annoList->indexOfTopLevelItem(item);

		Annotation *anno = NULL;

		if (ctrlType == AT_ALL)
		{
			anno = WIN_MANAGER->anotationList.at(index);
		}
		else
		{
			int count = 0;
			for (int i = 0; i < WIN_MANAGER->anotationList.size(); i++)
			{
				if (WIN_MANAGER->anotationList.at(i)->getType() != AT_TEXT) continue;

				if (count == index)
				{
					anno = WIN_MANAGER->anotationList.at(i);
					break;
				}
				count++;
			}
		}

		if (anno)
		{
			anno->setAnnoHidden(!anno->isAnnoHidden());
			AddAnnoList();

			WIN_MANAGER->renderLater_GridView();
		}
	}

}

void AnnoControlDlg::slot_OnColumnDoubleClick(QTreeWidgetItem *item, int column)
{
	if (item == NULL)
		return;

	if (column == 0) // color change
	{
		muint32 listIndex = m_annoList->indexOfTopLevelItem(item);
		int actionIndex = listIndex;
		Annotation *anno = NULL;

		if (ctrlType == AT_ALL)
		{
			anno = WIN_MANAGER->anotationList.at(actionIndex);
		}
		else
		{
			actionIndex = 0;
			for (int i = 0; i < WIN_MANAGER->anotationList.size(); i++)
			{
				if (WIN_MANAGER->anotationList.at(i)->getType() != AT_TEXT) continue;

				if (actionIndex == listIndex)
				{
					anno = WIN_MANAGER->anotationList.at(i);
					actionIndex = i;
					break;
				}
				actionIndex++;
			}
		}

		if (anno)
		{
			QColorDialog dlg;
			QColor colOrigin = toQColor(anno->getColor());
			dlg.setStyleSheet("background-color : rgba(48,48,48,255); color : white");
			dlg.setCurrentColor(colOrigin);

			if (dlg.exec() == QDialog::Accepted)
			{
				QColor colNew = dlg.selectedColor();

				switch (anno->getType())
				{
				case AT_TEXT:
				{
					AnnoString* annoStr = (AnnoString*)anno;
					AnnoString originAnno = *annoStr;
					AnnoString newAnno = *annoStr;
					newAnno.setColor(toCOLOR(colNew));

					ACTION_MANAGER->action_Annotation_Text_Edit(NULL, actionIndex, &originAnno, &newAnno);
					break;
				}
				case AT_LEN:
				{
					AnnoLength* annoLength = (AnnoLength*)anno;
					AnnoLength originAnno = *annoLength;
					AnnoLength newAnno = *annoLength;
					newAnno.setColor(toCOLOR(colNew));

					ACTION_MANAGER->action_Annotation_Len_Edit(actionIndex, &originAnno, &newAnno);
					break;
				}
				case AT_ARROW:
				{
					AnnoArrow* annoArrow = (AnnoArrow*)anno;
					AnnoArrow originAnno = *annoArrow;
					AnnoArrow newAnno = *annoArrow;
					newAnno.setColor(toCOLOR(colNew));

					ACTION_MANAGER->action_Annotation_Arrow_Edit(actionIndex, &originAnno, &newAnno);
					break;
				}
				case AT_ANGLE:
				{
					AnnoAngle* annoAngle = (AnnoAngle*)anno;
					AnnoAngle originAnno = *annoAngle;
					AnnoAngle newAnno = *annoAngle;
					newAnno.setColor(toCOLOR(colNew));

					ACTION_MANAGER->action_Annotation_Angle_Edit(actionIndex, &originAnno, &newAnno);
					break;
				}
				case AT_RECTANGLE:
				{
					AnnoRectangle* annoRect = (AnnoRectangle*)anno;
					AnnoRectangle originAnno = *annoRect;
					AnnoRectangle newAnno = *annoRect;
					newAnno.setColor(toCOLOR(colNew));

					ACTION_MANAGER->action_Annotation_Rectangle_Edit(actionIndex, &originAnno, &newAnno);
					break;
				}
				default:
					break;
				}
				AddAnnoList();
			}
		}
	}
	else if (column == 2)
	{
		OnSeek();
	}
}
