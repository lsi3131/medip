#include "stdafx.h"
#include "CListWidget.h"
#include "volumedata.h"
#include "windowManager.h"
#include "AlphaWidget.h"
#include "ProductManager.h"
#include "ResourceManager.h"

//////////////////////////////////////////////////////////////////////////////////
// CListWidget Start
/*
@brief		리스트 위젯
@author		허 건 과장
@date		2021.12.09
*/
//////////////////////////////////////////////////////////////////////////////////
/*
@brief
*/
CListWidget::CListWidget(QWidget* parent, VOLUME_DATA* p_volume_data)
{
	setContextMenuPolicy(Qt::DefaultContextMenu);
	setAnimated(true);

	setRootIsDecorated(false);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	m_p_VolumeData = p_volume_data;
}

/*
@brief
*/
CListWidget::~CListWidget()
{

}

/*
@brief
@return
*/
void CListWidget::mousePressEvent(QMouseEvent* event)
{
	QTreeWidget::mousePressEvent(event);

	QTreeWidgetItem* item = itemAt(event->pos());

	if (item == NULL || (Qt::NoModifier == event->modifiers() && (event->button() != Qt::RightButton)))
	{
		mint32 index = indexOfTopLevelItem(item);

		if (index >= 0 && index < topLevelItemCount())
		{
			selectionRefresh(index);
		}
		//else
		//{
		//	selectionRefresh(data->GetCurrentMeshIndex());
		//}

		return;
	}

	mint32 index = indexOfTopLevelItem(item);

	if (index >= 0 && index < topLevelItemCount())
	{
		selectionUpdate(index);
	}
}

/*
@brief
@return
*/
void		CListWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QTreeWidgetItem* item = itemAt(event->pos());

	if (selectedItems().size() <= 1)
	{
		QTreeWidget::mouseReleaseEvent(event);
	}

	//if (item == NULL)
	//{
	//	if (selectedItems().size() <= 1)
	//	{
	//		selectionRefresh(WIN_MANAGER->volume_data.GetCurrentMeshIndex());
	//		return;
	//	}
	//}
}

/*
@brief
@return
*/
void		CListWidget::keyReleaseEvent(QKeyEvent* event)
{
	//if (Qt::Key_Delete == event->key())
	//{
	//	//OnDeleteMeshes();
	//	return;
	//	
	//}

	QTreeWidget::keyReleaseEvent(event);
}

/*
@brief
@return
*/
void		CListWidget::keyPressEvent(QKeyEvent* event)
{
	QTreeWidget::keyPressEvent(event);
}

/*
@brief
@return
*/
void		CListWidget::resizeEvent(QResizeEvent* event)
{
	QTreeWidget::resizeEvent(event);
}


/*
@brief
@return
*/
void		CListWidget::update(QList<QTreeWidgetItem*>& _vt_list, QList<muint32>& _vt_uid)
{
	clear();

	m_vt_UID.clear();

	if ((_vt_list.size() == 0) || (_vt_uid.size() == 0) || (_vt_uid.size() != _vt_list.size()))
	{
		return;
	}

	muint32 item_cnt = _vt_list.size();

	setItemsExpandable(false);
	for (int i = 0; i < item_cnt; ++i)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(this);

		item->setBackground(L_COL_COLOR, _vt_list[i]->background(L_COL_COLOR));
		item->setText(L_COL_NAME, _vt_list[i]->text(L_COL_NAME));
		item->setTextColor(L_COL_NAME, QColor(255, 255, 255));

		insertTopLevelItem(0, item);
	}

	if (item_cnt > 0)
	{
		QTreeWidgetItem* item = topLevelItem(0);
		//if (item)
		//{
		//	item->setBackground(L_COL_ALPHA, QBrush(QColor(200, 200, 200, 255)));
		//	item->setBackground(L_COL_SHOW, QBrush(QColor(200, 200, 200, 255)));
		//	item->setBackground(L_COL_AI, QBrush(QColor(200, 200, 200, 255)));
		//	item->setBackground(L_COL_NAME, QBrush(QColor(200, 200, 200, 255)));
		//	setItemSelected(item, true);
		//}

		if (item)
		{
			setCurrentItem(item);
		}

		m_vt_UID.resize(_vt_uid.size());
		std::copy(_vt_uid.begin(), _vt_uid.end(), m_vt_UID.begin());
	}
}

/*
@brief
@return
*/
void		CListWidget::selectionRefresh(int index_dest)
{
	static QIcon icons[] = { RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE), RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE) };
	static const QBrush selBrush = QBrush(QColor(200, 200, 200, 255));
	muint32 item_cnt = topLevelItemCount();
	QTreeWidgetItem* item = topLevelItem(index_dest);
	if (NULL == item)
		return;

	if (NULL == item)
	{
		setItemsExpandable(false);
		item = new QTreeWidgetItem(this);

		insertTopLevelItem(index_dest, item);
	}

	//item->setBackground(L_COL_COLOR, item->background(L_COL_COLOR));
	//item->setText(L_COL_NAME, item->text(L_COL_NAME));

	if (item_cnt > 0)
	{
		clearSelection();

		//if (item)
		//{
		//	setCurrentItem(item);
		//	item->setBackground(L_COL_ALPHA, selBrush);
		//	item->setBackground(L_COL_SHOW, selBrush);
		//	item->setBackground(L_COL_AI, selBrush);
		//	item->setBackground(L_COL_NAME, selBrush);
		//	item->setSelected(true);
		//	setItemSelected(item, true);
		//	scrollToItem(item);

		//	selectionUpdate(index_dest);
		//}

		selectionUpdate(index_dest);
	}
}

/*
@brief
@return
*/
void		CListWidget::selectionUpdate(int index_dest)
{
	const QBrush selBrush[] = { QBrush(QColor(48, 48, 48, 255)) , QBrush(QColor(200, 200, 200, 255)) };

	for (int n = 0; n < topLevelItemCount(); n++)
	{
		QTreeWidgetItem* it = topLevelItem(n);
		bool selected = (index_dest == n || it->isSelected());

		//	if (selected)
		//		setCurrentItem(it);

		it->setBackground(L_COL_ALPHA, selBrush[selected]);
		it->setBackground(L_COL_SHOW, selBrush[selected]);
		it->setBackground(L_COL_AI, selBrush[selected]);
		it->setBackground(L_COL_NAME, selBrush[selected]);

		//	if (it->isSelected() != selected)
		{
			it->setSelected(selected);
			setItemSelected(it, selected);
		}
	}

	if (currentItem() == NULL)
	{
		setCurrentItem(topLevelItem(index_dest));
	}
}

/*
@brief
@return
*/
void		CListWidget::getSeletedList(QList<muint32>& _list)
{
	_list.clear();

	QList<QTreeWidgetItem*> list = selectedItems();

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
		{
			item = item->parent();
		}

		muint32 index = indexOfTopLevelItem(item);

		if (!_list.contains(index))
		{
			_list.push_back(index);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
// CListWidget Finish
//////////////////////////////////////////////////////////////////////////////////






