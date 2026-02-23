#include "stdafx.h"
#include "AnnotationTab2.h"
#include "ProductManager.h"
#include "StyleManager.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "LicenseManager.h"
#include "ResourceManager.h"

AnnotationTab2::AnnotationTab2(QWidget* parent /*= NULL*/)
	:CollapseWidget(QString(), parent)
{
	int nRow = 0;

	m_SegOpen = false;
	m_MeasureOpen = true;
	m_annoList = new QTreeWidget(this);
	m_annoList->setObjectName("TreeAnno");
	m_annoList->header()->setMinimumSectionSize(20);
	m_annoList->setColumnCount(3);
	m_annoList->setColumnWidth(0, 20);
	m_annoList->setColumnWidth(1, 20);
	m_annoList->setHeaderHidden(true);
	m_annoList->setStyleSheet(STYLE_MANAGER->treeBasicList);
	m_annoList->setRootIsDecorated(false);
	m_annoList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_annoList->installEventFilter(this);
	connect(m_annoList, &QTreeWidget::itemClicked, this, &AnnotationTab2::slot_OnColumnClick);
	connect(m_annoList, &QTreeWidget::itemDoubleClicked, this, &AnnotationTab2::slot_OnColumnDoubleClick);

	addWidget(m_annoList, nRow++);

	m_btnDel = new QPushButton(this);
	m_btnDel->setIcon(RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_DEL));
	m_btnDel->setIconSize(QSize(24, 24));
	m_btnDel->setFixedSize(QSize(24, 24));
	m_btnDel->setObjectName("ButtonDel");
	m_btnDel->setMouseTracking(true);
	m_btnDel->installEventFilter(this);
	m_btnDel->setToolTip("Delete Annotations");
	m_btnDel->setStyleSheet("color: black;");


	//	btnDel->setStyleSheet(STYLE_MANAGER->buttonBehind);

	connect(m_btnDel, &QPushButton::released, this, &AnnotationTab2::slot_OnAnnoDel);

	QWidget* emptyBox1 = new QWidget(this);
	emptyBox1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	addWidget(emptyBox1, nRow, 1, QMargins(5, 0, 0, 0));
	addWidget(m_btnDel, nRow++, 1);

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	addWidget(emptyBox0, nRow++, 0);

	QString title = "Annotation";
	setTitle(title);

	if (WIN_MANAGER->mainTabType == MAINTAB_SEGMENTATION)
		setOpenWidget(m_SegOpen);
	else
		setOpenWidget(m_MeasureOpen);

}

void AnnotationTab2::setOpenState(MAINTAB_TYPE type)
{
	if (type == MAINTAB_MEASUREMENT)
		setOpenWidget(m_MeasureOpen);
	else if (type == MAINTAB_SEGMENTATION)
		setOpenWidget(m_SegOpen);

	AddAnnoList(type);
}

void AnnotationTab2::updateOpenState(MAINTAB_TYPE preType)
{
	if (preType == MAINTAB_MEASUREMENT)
		m_MeasureOpen = m_TbtnCollapse->isChecked();
	else if (preType == MAINTAB_SEGMENTATION)
		m_SegOpen = m_TbtnCollapse->isChecked();
}

bool AnnotationTab2::eventFilter(QObject* watched, QEvent* e)
{
	if (watched == NULL) return QWidget::eventFilter(watched, e);

	if (watched->objectName().contains("Button"))
	{
		static QIcon delHover = RESOURCE_MANAGER->getIcon(ICON_ROILIST_DEL_HOVER);
		static QIcon delLeave = RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_DEL);
		static QIcon delPress = RESOURCE_MANAGER->getIcon(ICON_ROILIST_DEL);

		if (!(e->type() == QEvent::MouseMove ||
			e->type() == QEvent::MouseButtonPress ||
			e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease))
			return QWidget::eventFilter(watched, e);

		if (m_btnDel == watched)
		{
			if (e->type() == QEvent::MouseMove)
				m_btnDel->setIcon(delHover);
			else if (e->type() == QEvent::MouseButtonPress)
				m_btnDel->setIcon(delPress);
			else if (e->type() == QEvent::HoverLeave ||
				e->type() == QEvent::MouseButtonRelease)
				m_btnDel->setIcon(delLeave);
		}

	}

	if (e->type() == QEvent::KeyRelease)
	{
		QKeyEvent* evt = dynamic_cast<QKeyEvent*>(e);

		if (evt)
		{
			if (evt->key() == Qt::Key_Delete)
				slot_OnAnnoDel();
		}
	}


	return QWidget::eventFilter(watched, e);
}

void AnnotationTab2::AddAnnoList(MAINTAB_TYPE Maintype)
{
	m_annoList->clear();

	QList<QTreeWidgetItem*> items;
	int listIndex = 0;
	for (int i = 0; i < WIN_MANAGER->anotationList.size(); i++)
	{
		ANNOTATION_TYPE type = WIN_MANAGER->anotationList.at(i)->getType();

		// 		if ((Maintype == MAINTAB_SEGMENTATION) && (type != AT_TEXT))
		// 			continue;

		QTreeWidgetItem* item = new QTreeWidgetItem(m_annoList);
		if (type == AT_TEXT)
		{
			AnnoString* str = (AnnoString*)WIN_MANAGER->anotationList.at(i);
			mip::VECTOR3 v_volume = str->getVolumePos();
			QString posText = WIN_MANAGER->ConvertText_VoxelPosToSliceImagePos(v_volume.x, v_volume.y, v_volume.z);

			COLOR col = str->getColor();
			item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
			item->setIcon(1, RESOURCE_MANAGER->getIcon(str->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
			item->setText(2, tr("%3. %4 type \"%1\" (%2)").arg(str->getText())
				.arg(posText).arg(++listIndex).arg(STRING_MANAGER->getString(STR_ANNO_TEXT)));
		}
		else if (type == AT_LEN)
		{
			AnnoLength* len = (AnnoLength*)WIN_MANAGER->anotationList.at(i);
			COLOR col = len->getColor();
			item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
			item->setIcon(1, RESOURCE_MANAGER->getIcon(len->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
			item->setText(2, tr("%1. %3 type (%2 mm)").arg(++listIndex).arg(len->getLength() * 10).arg(STRING_MANAGER->getString(STR_ANNO_LENGTH)));
		}
		else if (type == AT_ANGLE)
		{
			AnnoAngle* angl = (AnnoAngle*)WIN_MANAGER->anotationList.at(i);
			COLOR col = angl->getColor();
			item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
			item->setIcon(1, RESOURCE_MANAGER->getIcon(angl->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
			item->setText(2, tr("%1. %3 type (%2%4)").arg(++listIndex).arg(angl->getAngle())
				.arg(STRING_MANAGER->getString(STR_ANNO_ANGLE)).arg(QString(QChar(0x00B0))));
		}
		else if (type == AT_OVAL)
		{
			/*
			이상일 대리
			현재 사용되지 않아 주석처리
			*/
			//AnnoOval *oval = (AnnoOval *)WIN_MANAGER->anotationList.at(i);
			//COLOR col = oval->getColor();
			//item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
			//item->setIcon(1, RESOURCE_MANAGER->getIcon(oval->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
			//item->setText(2, tr("%1. %2 type").arg(++listIndex).arg(STRING_MANAGER->getString(STR_ANNO_OVAL)));
		}
		else if (type == AT_ARROW)
		{
			AnnoArrow* arrow = (AnnoArrow*)WIN_MANAGER->anotationList.at(i);
			COLOR col = arrow->getColor();
			item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
			item->setIcon(1, RESOURCE_MANAGER->getIcon(arrow->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
			item->setText(2, tr("%1. %2 type").arg(++listIndex).arg(STRING_MANAGER->getString(STR_ANNO_ARROW)));
		}
		else if (type == AT_RECTANGLE)
		{
			AnnoRectangle* rect = (AnnoRectangle*)WIN_MANAGER->anotationList.at(i);
			COLOR col = rect->getColor();
			item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
			item->setIcon(1, RESOURCE_MANAGER->getIcon(rect->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
			item->setText(2, tr("%1. %2 type").arg(++listIndex).arg(STRING_MANAGER->getString(STR_ANNO_RECTANGLE)));
		}


		items.append(item);
	}

	m_annoList->insertTopLevelItems(0, items);
}

void AnnotationTab2::UpdateAnnoItem(QTreeWidgetItem* item, int listIndex)
{
	if (item == NULL) return;

	Annotation* anno;

	anno = WIN_MANAGER->anotationList.at(listIndex);

	COLOR col;
	switch (anno->getType())
	{
	case AT_TEXT:
	{
		AnnoString* str = static_cast<AnnoString*>(anno);
		mip::VECTOR3 v_volume = str->getVolumePos();
		QString posText = WIN_MANAGER->ConvertText_VoxelPosToSliceImagePos(v_volume.x, v_volume.y, v_volume.z);

		col = str->getColor();
		item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
		item->setIcon(1, RESOURCE_MANAGER->getIcon(str->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
		item->setText(2, tr("%3. %4 type \"%1\" (%2)")
			.arg(str->getText())
			.arg(posText).arg(++listIndex).arg(STRING_MANAGER->getString(STR_ANNO_TEXT)));
	}
	break;
	case AT_OVAL:
	{
		/*
		이상일 대리
		- 사용되지 않아 주석 처리
		*/
		//AnnoOval *oval = static_cast<AnnoOval*>(anno);
		//col = oval->getColor();
		//item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
		//item->setIcon(1, RESOURCE_MANAGER->getIcon(oval->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
		//item->setText(2, tr("%1. %2 type").arg(++listIndex).arg(STRING_MANAGER->getString(STR_ANNO_OVAL)));
	}
	break;
	case AT_LEN:
	{
		AnnoLength* len = static_cast<AnnoLength*>(anno);
		col = len->getColor();
		item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
		item->setIcon(1, RESOURCE_MANAGER->getIcon(len->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
		item->setText(2, tr("%1. %3 type (%2 mm)").arg(++listIndex).arg(len->getLength() * 10).arg(STRING_MANAGER->getString(STR_ANNO_LENGTH)));
	}
	break;
	case AT_ARROW:
	{
		AnnoArrow* arrow = static_cast<AnnoArrow*>(anno);
		col = arrow->getColor();
		item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
		item->setIcon(1, RESOURCE_MANAGER->getIcon(arrow->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
		item->setText(2, tr("%1. %2 type").arg(++listIndex).arg(STRING_MANAGER->getString(STR_ANNO_ARROW)));
	}
	break;
	case AT_ANGLE:
	{
		AnnoAngle* angl = static_cast<AnnoAngle*>(anno);
		col = angl->getColor();
		item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
		item->setIcon(1, RESOURCE_MANAGER->getIcon(angl->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
		item->setText(2, tr("%1. %3 type (%2%4)").arg(++listIndex).arg(angl->getAngle()).arg(STRING_MANAGER->getString(STR_ANNO_ANGLE)).arg(QString(QChar(0x00B0))));
	}
	break;
	case AT_RECTANGLE:
	{
		AnnoRectangle* rect = static_cast<AnnoRectangle*>(anno);
		col = rect->getColor();
		item->setBackground(0, QBrush(QColor(col.r, col.g, col.b)));
		item->setIcon(1, RESOURCE_MANAGER->getIcon(rect->isAnnoHidden() ? ICON_LIST_INVISIBLE : ICON_LIST_VISIBLE));
		item->setText(2, tr("%1. %2 type").arg(++listIndex).arg(STRING_MANAGER->getString(STR_ANNO_RECTANGLE)));
	}
	break;
	default:
		break;
	}
}

void AnnotationTab2::slot_OnColumnClick(QTreeWidgetItem* item, int column)
{
	if (item == NULL) return;

	if (column == 1) //visible - invisible
	{
		int listIndex = m_annoList->indexOfTopLevelItem(item);

		Annotation* anno = NULL;

		anno = WIN_MANAGER->anotationList.at(listIndex);

		if (anno)
		{
			anno->setAnnoHidden(!anno->isAnnoHidden());

			UpdateAnnoItem(item, listIndex);

			WIN_MANAGER->renderLater_GridView();
		}
	}
}

void AnnotationTab2::slot_OnColumnDoubleClick(QTreeWidgetItem* item, int column)
{
	if (item == NULL) return;

	if (column == 0) // color change
	{
		muint32 listIndex = m_annoList->indexOfTopLevelItem(item);
		int actionIndex = listIndex;
		Annotation* anno = NULL;

		anno = WIN_MANAGER->anotationList.at(actionIndex);

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

				WIN_MANAGER->renderLater_All();
			}
		}
	}
}

void AnnotationTab2::slot_OnAnnoDel()
{
	QList<QTreeWidgetItem*>& list = m_annoList->selectedItems();

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

	for (int i = 0; i < indeces.size(); i++)
	{
		int index = indeces[i];

		ACTION_MANAGER->action_Annotation_Del(NULL, index);
	}
}