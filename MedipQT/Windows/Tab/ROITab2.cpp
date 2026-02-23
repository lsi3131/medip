#include "stdafx.h"
#include "ROITab2.h"
#include "WindowManager.h"
#include "ResourceManager.h"
#include "styleManager.h"
#include "StringManager.h"
#include "ProductManager.h"
#include "LicenseManager.h"
#include "Main/MainSegmentWidget.h"
#include "DataContext.h"

ROITab2::ROITab2(ProductManager* pProductManager, QWidget* parent)
	: CollapseWidget(QString(), parent),
	m_pProductManager(pProductManager),
	m_nRow(0)
{
	QString title = "ROI List";
	setTitle(title);

	initDataFromConfigFile();

	//m_nIconSize = WIN_MANAGER->mainWindow->IconSize;
	m_nIconSize = WIN_MANAGER->ScreenWidth / 80; // 24x24

	addMaskListWidget();
	addButtonWidgets();
	addVoxelInfoWidgets();
	addLegacyWidgets();

	QWidget* emptyBox0 = new QWidget(this);
	emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	addWidget(emptyBox0, m_nRow++, 0);

	updateHeight();
	updateGeometry();

	setOpenWidget(true);
}

void ROITab2::AddROIList()
{
	if (!DATA_CONTEXT->volume_data.isValidate())
		return;

	if (ROIList)
	{
		ROIList->UpdateByVolumeData();
		updateROIHeader();
	}
}

void ROITab2::ClearROIList()
{
	if (ROIList)
		ROIList->clear();
}

void ROITab2::slot_OnHeaderClick(QTreeWidgetItem* item, int column)
{
	if (NULL == item)
		return;

	if (column == L_COL_SHOW)//|| column == L_COL_MESH)
	{
		bool res = item->data(column, Qt::UserRole + 1).toBool();

		res = !res;

		if (L_COL_SHOW == column)
		{
			item->setIcon(L_COL_SHOW, RESOURCE_MANAGER->getIcon
			(res ? ICON_LIST_VISIBLE : ICON_LIST_INVISIBLE));
			item->setBackground(L_COL_SHOW, QBrush(res ? QColor(0, 0, 0, 0) : QColor(48, 48, 48)));
			item->setForeground(L_COL_SHOW, QBrush(res ? QColor(0, 0, 0, 0) : QColor(48, 48, 48)));

			item->setData(L_COL_SHOW, Qt::UserRole + 1, QVariant(res));
		}

		ROIList->UpdateColumn((LAYER_COLUMN_AKA)column, res);
	}
	else if (column == L_COL_COUNT)
	{
		m_bExpand = !m_bExpand;
		ROIList->UpdateSize(m_bExpand);
		item->setText(column, QString(QChar(m_bExpand ? 0x25BC : 0x25B2)));
		updateHeight();

		WIN_MANAGER->setConfigValue(ELEMENT_FILE, STRING_MANAGER->config_expandROI, QString::number(m_bExpand));
	}
}

void ROITab2::slot_OnColumnClick(QTreeWidgetItem* item, int column)
{
	if (NULL == item)
		return;

	if (ROIList->selectedItems().count() != 1)
		return;

	muint32 index = ROIList->indexOfTopLevelItem(item);
	if (item->childCount() == 0)
	{
		item = item->parent();
		index = ROIList->indexOfTopLevelItem(item);
		ROIList->SelectionRefresh(index);
		return;
	}
	else
	{
		ROIList->SelectionRefresh(index);

		if (L_COL_ALPHA == column)
		{
			bool res = false;
			if (item->child(0))
			{
				res = !item->child(0)->isHidden();
			}

			if (res)
			{
				res = item->isExpanded();

				item->setExpanded(!res);

				if (!res)
					item->setText(L_COL_ALPHA, QString("-"));
				else
					item->setText(L_COL_ALPHA, QString("+"));
			}

		}
		else if (L_COL_SHOW == column) //layer show/hide
		{
			MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(index);
			if (info)
			{
				info->show = !info->show;
				item->setIcon(L_COL_SHOW, RESOURCE_MANAGER->getIcon(info->show ? ICON_LIST_VISIBLE : ICON_LIST_INVISIBLE));
				updateROIHeader(L_COL_SHOW);
				WIN_MANAGER->forceUpdate2DViewData(false, true);
				WIN_MANAGER->renderLater_GridView(true);
			}
		}

		VOLUME_DATA volume = DATA_CONTEXT->volume_data;

		DATA_CONTEXT->volume_data.setCurrentMaskIndex(index);
		MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(index);
		std::vector<muint32> vecSelect;
		vecSelect.push_back(info->uid);
		DATA_CONTEXT->volume_data.setMultiSelectUID(vecSelect);

		int per = (int)(((float)DATA_CONTEXT->volume_data.getVoxelCount(info->uid) / DATA_CONTEXT->volume_data.getVolumeDataLength()) * 10000);
		float val = (float)per * 0.01;

		if (!LICENSE_DATA->getProductName().compare(PRODUCT_NAME_MEDIP))
		{
			QString str = DATA_CONTEXT->volume_data.getVoxelCount(info->uid) > 0 ? QString("%, %1cm%2")
				.arg(QString::number(DATA_CONTEXT->volume_data.getSpace3D(true) * DATA_CONTEXT->volume_data.getVoxelCount(info->uid) / 1000.f, 'f', 2))
				.arg(QString(QChar(0x00B3)))
				: "%";

			m_labelVoxel->setText(QString("Voxel : %1 (%2%3)")
				.arg(DATA_CONTEXT->volume_data.getVoxelCount(info->uid))
				.arg(QString::number(val, 'f', 2))
				.arg(str));
		}
		else if (!LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH))
		{
			QString str = DATA_CONTEXT->volume_data.getVoxelCount(info->uid) > 0 ? QString("%1cm%2")
				.arg(QString::number(DATA_CONTEXT->volume_data.getSpace3D(true) * DATA_CONTEXT->volume_data.getVoxelCount(info->uid) / 1000.f, 'f', 2))
				.arg(QString(QChar(0x00B3)))
				: "%";

			m_labelVoxel->setText(QString("Voxel : %1 (%2)")
				.arg(DATA_CONTEXT->volume_data.getVoxelCount(info->uid))
				.arg(str));
		}


#ifdef DEV_VER	
		m_labelBurdenWeight->setText(QString("Burden : %1 g")
			.arg(DATA_CONTEXT->volume_data.getBurdenVal(info->uid)));
#endif

	}
}

void ROITab2::slot_OnColumnDoubleClick(QTreeWidgetItem* item, int column)
{
	if (NULL == item)
		return;

	if (column != L_COL_NAME)
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));

	if (ROIList->selectedItems().count() != 1)
		return;

	muint32 index = ROIList->indexOfTopLevelItem(item);

	if (item->childCount() == 0)
	{
		item = item->parent();
		index = ROIList->indexOfTopLevelItem(item);
		ROIList->SelectionRefresh(index);
		return;
	}

	if (L_COL_COLOR == column) //color change
	{
		QColorDialog dlg;
		dlg.setStyleSheet("background-color : rgba(48,48,48,255); color : white");
		MaskInfo* pMaskInfo = DATA_CONTEXT->volume_data.getMaskInfo(index);
		dlg.setCurrentColor(QColor(pMaskInfo->color.r, pMaskInfo->color.g, pMaskInfo->color.b, pMaskInfo->color.a));

		if (dlg.exec() == QDialog::Accepted)
		{
			QColor color = dlg.selectedColor();

			ACTION_MANAGER->action_MaskList_color(ROIList, index, COLOR(color.red(), color.green(), color.blue(), 255));
		}
	}
	else if (L_COL_NAME == column) //item text change
	{
		//if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_LayerOperation_GeneralFunction))
		{
			ROIList->editMode = true;
			WIN_MANAGER->setMoveFocus(false);

			item->setFlags(item->flags() | Qt::ItemIsEditable);
			ROIList->editItem(item, L_COL_NAME);
		}
	}
	ROIList->SelectionUpdate(index);
	DATA_CONTEXT->volume_data.setCurrentMaskIndex(index);
	MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(index);
	std::vector<muint32> vecSelect;
	vecSelect.push_back(info->uid);
	DATA_CONTEXT->volume_data.setMultiSelectUID(vecSelect);
	int per = (int)(((float)DATA_CONTEXT->volume_data.getVoxelCount(info->uid) / DATA_CONTEXT->volume_data.getVolumeDataLength()) * 10000);
	float val = (float)per * 0.01;
	QString str = DATA_CONTEXT->volume_data.getVoxelCount(info->uid) > 0 ?
		QString("%, %1cm%2").arg((QString::number(DATA_CONTEXT->volume_data.getSpace3D(true) * DATA_CONTEXT->volume_data.getVoxelCount(info->uid) / 1000.f, 'f', 2))).arg(QString(QChar(0x00B3))) :
		"%";
	m_labelVoxel->setText(QString("Voxel : %1 (%2%3)").arg(DATA_CONTEXT->volume_data.getVoxelCount(info->uid))
		.arg(QString::number(val, 'f', 2)).arg(str));
}

void ROITab2::slot_OnColumnChanged(QTreeWidgetItem* item, int column)
{
	if (NULL == item)
		return;

	if (item->childCount() == 0)
		return;

	if (L_COL_NAME == column)
	{
		if (ROIList->editMode && 0 != DATA_CONTEXT->volume_data.getMaskName(ROIList->indexOfTopLevelItem(item)).compare(item->text(column)))
		{
			if (item->text(column).contains(QRegularExpression(FILE_NAME_RULE)))
			{
				ROIList->blockSignals(true);
				QMessageBox::warning(NULL, QString("Name rule"),
					QString("A layer name cannot contain any of the following characters : \"\\ / : *? \" < > |	\""));
				item->setText(column, DATA_CONTEXT->volume_data.getMaskName(ROIList->indexOfTopLevelItem(item)));
				ROIList->blockSignals(false);
			}
			// jhc[2020.10.29] mask name 중복 할당 방지
			else if (DATA_CONTEXT->volume_data.isDuplicateMaskName(item->text(column)))
			{
				ROIList->blockSignals(true);
				QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_WARN), QString("There are duplicate mask names. retry other name."));
				item->setText(column, DATA_CONTEXT->volume_data.getMaskName(ROIList->indexOfTopLevelItem(item)));
				ROIList->blockSignals(false);
			}
			else
				ACTION_MANAGER->action_MaskList_text_change(ROIList, ROIList->indexOfTopLevelItem(item), item->text(column));
		}
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));
		ROIList->editMode = false;
		WIN_MANAGER->setMoveFocus(true);
	}
}

void ROITab2::slot_OnInverse()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = ROIList->selectedItems();

	QList<QTreeWidgetItem*> Invlist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = ROIList->indexOfTopLevelItem(item);

		if (!Invlist.contains(item))
			Invlist.push_back(item);
	}

	if (Invlist.size() != 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(STR_ONE_SELECT) +
			STRING_MANAGER->getString(STR_INVERSE));
		return;
	}

	muint32 index = ROIList->indexOfTopLevelItem(Invlist[0]);
	MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(index);
	int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

	ACTION_MANAGER->action_MaskList_Invert(*info, mI);
}

void ROITab2::slot_OnFFS()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false)
		return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = ROIList->selectedItems();
	QList<QTreeWidgetItem*> FFSlist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = ROIList->indexOfTopLevelItem(item);

		if (!FFSlist.contains(item))
			FFSlist.push_back(item);
	}


	if (FFSlist.size() != 2)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_FFS_ROI), STRING_MANAGER->getString(STR_SELECT_2_WARN));
		return;
	}
	int count = DATA_CONTEXT->volume_data.getMaskInfoListCnt();
	if (count < 2)
		return;

	mint32 indeces[2];
	int cnt = 0;
	for (cnt = 0; cnt < FFSlist.size(); cnt++)
	{
		indeces[cnt] = ROIList->indexOfTopLevelItem(FFSlist[cnt]);
	}


	MaskInfo* A = DATA_CONTEXT->volume_data.getMaskInfo(indeces[0]);
	MaskInfo* B = DATA_CONTEXT->volume_data.getMaskInfo(indeces[1]);

	if (A && B)
	{
		ACTION_MANAGER->action_MaskList_DifferenceAB(*A, *B);
	}
}

void ROITab2::updateROIList(bool isSelection, int selection)
{
	if (ROIList)
	{
		if (!isSelection)
		{
			ROIList->UpdateByVolumeData();
		}
		else
		{
			ROIList->SelectionRefresh(selection);
		}

		if (m_ROIheader->isHidden())
		{
			QSize size = ROIList->SizeForRow();

			m_ROIheader->setFixedHeight(size.height() + 5);
			m_ROIheader->setHidden(false);
			ROIList->setStyleSheet(STYLE_MANAGER->treeDivList);
		}

		updateROIHeader();
		WIN_MANAGER->settingOutset();

		MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(DATA_CONTEXT->volume_data.getCurrentMaskInfoID());
		if (info)
		{
			int per = (int)(((float)DATA_CONTEXT->volume_data.getVoxelCount(info->uid) / DATA_CONTEXT->volume_data.getVolumeDataLength()) * 10000);
			float val = (float)per * 0.01;
			QString str = DATA_CONTEXT->volume_data.getVoxelCount(info->uid) > 0 ?
				QString("%, %1cm%2").arg(QString::number(DATA_CONTEXT->volume_data.getSpace3D(true) * DATA_CONTEXT->volume_data.getVoxelCount(info->uid) / 1000.f, 'f', 2)).arg(QString(QChar(0x00B3))) :
				"%";
			m_labelVoxel->setText(QString("Voxel : %1 (%2%3)").arg(DATA_CONTEXT->volume_data.getVoxelCount(info->uid))
				.arg(QString::number(val, 'f', 2)).arg(str));

#ifdef DEV_VER
			m_labelBurdenWeight->setText(QString("Burden : %1 g").arg(DATA_CONTEXT->volume_data.getBurdenVal(info->uid)));
#endif
		}
	}
}

void ROITab2::updateROIListMultiMask(const std::vector<muint32>& vecMultiMaskUID)
{
	if (ROIList)
	{
		ROIList->SelectionMultiUpdate(vecMultiMaskUID);

		if (m_ROIheader->isHidden())
		{
			QSize size = ROIList->SizeForRow();

			m_ROIheader->setFixedHeight(size.height() + 5);
			m_ROIheader->setHidden(false);
			ROIList->setStyleSheet(STYLE_MANAGER->treeDivList);
		}

		updateROIHeader();
	}
}

void ROITab2::updateROIHeader(LAYER_COLUMN_AKA type)
{
	QTreeWidgetItem* item = NULL;

	if (L_COL_COUNT == type)
	{
		m_ROIheader->clear();
		item = new QTreeWidgetItem(m_ROIheader);

		m_ROIheader->insertTopLevelItem(0, item);
	}
	else
	{
		item = m_ROIheader->topLevelItem(0);
	}

	if (item)
	{
		if (L_COL_COUNT == type || L_COL_SHOW == type)
		{
			//bool res = DATA_CONTEXT->volume_data.visibleCheckAll();
			bool res = DATA_CONTEXT->volume_data.visibleCheckAll(true);

			item->setIcon(L_COL_SHOW, RESOURCE_MANAGER->getIcon
			(res ? ICON_LIST_VISIBLE : ICON_LIST_INVISIBLE));
			item->setBackground(L_COL_SHOW, QBrush(res ? QColor(0, 0, 0, 0) : QColor(48, 48, 48)));
			item->setForeground(L_COL_SHOW, QBrush(res ? QColor(0, 0, 0, 0) : QColor(48, 48, 48)));

			item->setData(L_COL_SHOW, Qt::UserRole + 1, QVariant(res));
		}

		if (L_COL_COUNT == type)
		{
			if (m_bExpand)
			{
				ROIList->UpdateSize(m_bExpand);
				updateHeight();
				updateGeometry();
			}

			item->setText(L_COL_COUNT, QString(QChar(m_bExpand ? 0x25BC : 0x25B2)));
		}
	}
}

void ROITab2::applyVoxel(int index)
{
	if (DATA_CONTEXT->volume_data.isValidate())
	{
		ROIList->VoxelRefresh(index);
		updateVoxelLabel();
	}
}

void ROITab2::setMaintabType(MAINTAB_TYPE type)
{
	if (MAINTAB_SEGMENTATION == type)
	{
		if (m_btnFitBox)
		{
			if (m_btnFitBox->isHidden())
				m_btnFitBox->show();
			if (m_btnAnalyze && !m_btnAnalyze->isHidden())
				m_btnAnalyze->hide();
		}
	}
	else
	{
		if (m_btnFitBox)
		{
			if (!m_btnFitBox->isHidden())
				m_btnFitBox->hide();
			if (m_btnAnalyze)
			{
				if (MAINTAB_TA == type)
					m_btnAnalyze->show();
				else
					m_btnAnalyze->hide();
			}
		}
	}

	if (ROIList)
	{
		ROIList->SetDisableAlpha(type != MAINTAB_SEGMENTATION);
		ROIList->SetDisplayTA(type == MAINTAB_TA);
	}
}

void ROITab2::updateVoxelLabel()
{
	if (!DATA_CONTEXT->volume_data.isValidate())
		return;

	ROIList->SelectionRefresh(DATA_CONTEXT->volume_data.getCurrentMaskInfoID());

	MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(DATA_CONTEXT->volume_data.getCurrentMaskInfoID());
	int per = (int)(((float)DATA_CONTEXT->volume_data.getVoxelCount(info->uid) / DATA_CONTEXT->volume_data.getVolumeDataLength()) * 10000);
	float val = (float)per * 0.01;
	float volume = DATA_CONTEXT->volume_data.getSpace3D(true) * DATA_CONTEXT->volume_data.getVoxelCount(info->uid);
	QString str = DATA_CONTEXT->volume_data.getVoxelCount(info->uid) > 0 ?
		QString("%, %1cm%2").arg(QString::number(DATA_CONTEXT->volume_data.getSpace3D(true) * DATA_CONTEXT->volume_data.getVoxelCount(info->uid) / 1000.f, 'f', 2)).arg(QString(QChar(0x00B3))) :
		"%";
	m_labelVoxel->setText(QString("Voxel : %1 (%2%3)").arg(DATA_CONTEXT->volume_data.getVoxelCount(info->uid))
		.arg(QString::number(val, 'f', 2)).arg(str));

#ifdef DEV_VER
	/// burden
	int nUID = WIN_MANAGER->getSelectedMaskUID();
	int nMaskByteIdx = DATA_CONTEXT->volume_data.GetMaskByteIndex(nUID);

	double burden = getPneumoniaBurdenValue(DATA_CONTEXT->volume_data.getMaskDataPoint(nMaskByteIdx)
		, DATA_CONTEXT->volume_data.getHUDataPoint(), DATA_CONTEXT->volume_data.getMask(nUID)
		, DATA_CONTEXT->volume_data.getCX(), DATA_CONTEXT->volume_data.getCY(), DATA_CONTEXT->volume_data.getCZ()
		, DATA_CONTEXT->volume_data.getSpaceX(true), DATA_CONTEXT->volume_data.getSpaceY(true), DATA_CONTEXT->volume_data.getSpaceZ(true));

	DATA_CONTEXT->volume_data.setBurdenVal(nUID, burden);

	m_labelBurdenWeight->setText(QString("Burden : %1 g")
		.arg(DATA_CONTEXT->volume_data.getBurdenVal(info->uid)));
#endif
}

void ROITab2::updateTAState(bool needUpdate, int uid)
{
	if (!DATA_CONTEXT->volume_data.isValidate())
		return;

	ROIList->UpdateTAState(needUpdate, uid);
}

void ROITab2::invertShowState()
{
	QList<QTreeWidgetItem*>& list = ROIList->selectedItems();
	QList<QTreeWidgetItem*> invlist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = ROIList->indexOfTopLevelItem(item);

		if (!invlist.contains(item))
			invlist.push_back(item);
	}

	bool res = false;

	for (int i = 0; i < invlist.size(); i++)
	{
		QTreeWidgetItem* item = invlist.at(i);
		muint32 index = ROIList->indexOfTopLevelItem(item);
		MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(index);
		if (info)
		{
			if (!res)res = true;

			info->show = !info->show;
			item->setIcon(L_COL_SHOW, RESOURCE_MANAGER->getIcon(info->show ? ICON_LIST_VISIBLE : ICON_LIST_INVISIBLE));

		}
	}

	if (res)
	{
		updateROIHeader(L_COL_SHOW);
		WIN_MANAGER->forceUpdate2DViewData(false, true);
		WIN_MANAGER->renderLater_GridView(true);
	}
}

bool ROITab2::eventFilter(QObject* watched, QEvent* e)
{
	if (watched == NULL)
		return false;

	if (watched->objectName().contains("Button"))
	{
		static QIcon addHover = RESOURCE_MANAGER->getIcon(ICON_ROILIST_NEW_HOVER, m_nIconSize, m_nIconSize);
		static QIcon addLeave = RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_NEW, m_nIconSize, m_nIconSize);
		static QIcon addPress = RESOURCE_MANAGER->getIcon(ICON_ROILIST_NEW, m_nIconSize, m_nIconSize);

		static QIcon dupHover = RESOURCE_MANAGER->getIcon(ICON_ROILIST_COPY_HOVER, m_nIconSize, m_nIconSize);
		static QIcon dupLeave = RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_COPY, m_nIconSize, m_nIconSize);
		static QIcon dupPress = RESOURCE_MANAGER->getIcon(ICON_ROILIST_COPY, m_nIconSize, m_nIconSize);

		static QIcon delHover = RESOURCE_MANAGER->getIcon(ICON_ROILIST_DEL_HOVER, m_nIconSize, m_nIconSize);
		static QIcon delLeave = RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_DEL, m_nIconSize, m_nIconSize);
		static QIcon delPress = RESOURCE_MANAGER->getIcon(ICON_ROILIST_DEL, m_nIconSize, m_nIconSize);

		static QIcon fitcoordHover = RESOURCE_MANAGER->getIcon(ICON_VIEW_FIT_COORD_HOVER, m_nIconSize, m_nIconSize);
		static QIcon fitcoordLeave = RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FIT_COORD, m_nIconSize, m_nIconSize);
		static QIcon fitcoordClick = RESOURCE_MANAGER->getIcon(ICON_VIEW_FIT_COORD, m_nIconSize, m_nIconSize);

		static QIcon sketchHover = RESOURCE_MANAGER->getIcon(ICON_VIEW_SKETCHDRAWSEGMENTATION_HOVER, m_nIconSize, m_nIconSize);
		static QIcon sketchPress = RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_SKETCHDRAWSEGMENTATION_OK, m_nIconSize, m_nIconSize);
		static QIcon sketchOKHover = RESOURCE_MANAGER->getIcon(ICON_VIEW_SKETCHDRAWSEGMENTATION_OK_HOVER, m_nIconSize, m_nIconSize);
		static QIcon sketchRelease = RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_SKETCHDRAWSEGMENTATION, m_nIconSize, m_nIconSize);

		if (!(e->type() == QEvent::MouseMove ||
			e->type() == QEvent::MouseButtonPress ||
			e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease))
			return QWidget::eventFilter(watched, e);

		if (m_btnAdd == watched)
		{
			if (e->type() == QEvent::MouseMove)
				m_btnAdd->setIcon(addHover);
			else if (e->type() == QEvent::MouseButtonPress)
				m_btnAdd->setIcon(addPress);
			else if (e->type() == QEvent::HoverLeave ||
				e->type() == QEvent::MouseButtonRelease)
				m_btnAdd->setIcon(addLeave);
		}
		else if (m_btnDel == watched)
		{
			if (e->type() == QEvent::MouseMove)
				m_btnDel->setIcon(delHover);
			else if (e->type() == QEvent::MouseButtonPress)
				m_btnDel->setIcon(delPress);
			else if (e->type() == QEvent::HoverLeave ||
				e->type() == QEvent::MouseButtonRelease)
				m_btnDel->setIcon(delLeave);
		}
		else if (m_btnDup == watched)
		{
			if (e->type() == QEvent::MouseMove)
				m_btnDup->setIcon(dupHover);
			else if (e->type() == QEvent::MouseButtonPress)
				m_btnDup->setIcon(dupPress);
			else if (e->type() == QEvent::HoverLeave ||
				e->type() == QEvent::MouseButtonRelease)
				m_btnDup->setIcon(dupLeave);
		}
		else if (watched == m_btnFitBox)
		{
			if (e->type() == QEvent::MouseMove)
				m_btnFitBox->setIcon(fitcoordHover);
			else if (e->type() == QEvent::MouseButtonPress)
				m_btnFitBox->setIcon(fitcoordClick);
			else if (e->type() == QEvent::HoverLeave ||
				e->type() == QEvent::Show ||
				e->type() == QEvent::MouseButtonRelease)
				m_btnFitBox->setIcon(fitcoordLeave);
		}
	}

	return QWidget::eventFilter(watched, e);
}

void ROITab2::initDataFromConfigFile()
{
	QString strVal;

	if (!WIN_MANAGER->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_expandROI, strVal))
		m_bExpand = false;
	else if (0 >= strVal.toInt())
		m_bExpand = false;
	else
		m_bExpand = true;
}

void ROITab2::addMaskListWidget()
{
	ROIList = new MaskListWidget(&DATA_CONTEXT->volume_data, m_pProductManager, this);
	//ROIList = PRODUCT_FACTORY->createWidget<MaskListWidget>(MS_COLLAPS_ROILIST_COMMON, this);
	if (ROIList)
	{
		ROIList->header()->setMinimumSectionSize(20);
		ROIList->setColumnCount(L_COL_COUNT);
		ROIList->setColumnWidth(L_COL_ALPHA, 20);
		ROIList->setColumnWidth(L_COL_COLOR, 20);
		ROIList->setColumnWidth(L_COL_SHOW, 20);
		ROIList->setColumnWidth(L_COL_AI, 20);
		ROIList->setHeaderHidden(true);
		ROIList->setStyleSheet(STYLE_MANAGER->treeBasicList);
		ROIList->setAutoFillBackground(true);
		ROIList->setContentsMargins(0, 0, 0, 0);
		connect(ROIList, &QTreeWidget::itemClicked, this, &ROITab2::slot_OnColumnClick);
		connect(ROIList, &QTreeWidget::itemDoubleClicked, this, &ROITab2::slot_OnColumnDoubleClick);
		connect(ROIList, &QTreeWidget::itemChanged, this, &ROITab2::slot_OnColumnChanged);
	}

	m_ROIheader = new QTreeWidget(this);
	m_ROIheader->header()->setMinimumSectionSize(20);
	m_ROIheader->setColumnCount(L_COL_COUNT + 1);
	m_ROIheader->setColumnWidth(L_COL_ALPHA, 20);
	m_ROIheader->setColumnWidth(L_COL_COLOR, 20);
	m_ROIheader->setColumnWidth(L_COL_SHOW, 20);
	//m_ROIheader->setColumnWidth(L_COL_AI, 20);
	m_ROIheader->setColumnWidth(L_COL_COUNT, 20);
	m_ROIheader->setHeaderHidden(true);
	m_ROIheader->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
	m_ROIheader->setStyleSheet(STYLE_MANAGER->treeHeader);
	m_ROIheader->setAutoFillBackground(true);
	m_ROIheader->setHidden(true);
	m_ROIheader->setContentsMargins(0, 0, 0, 0);
	m_ROIheader->setFocusPolicy(Qt::NoFocus);

	connect(m_ROIheader, &QTreeWidget::itemClicked, this, &ROITab2::slot_OnHeaderClick);


	addWidget(m_ROIheader, m_nRow, 0, QMargins(0, 0, 0, 0), Qt::Alignment(Qt::AlignTop), true);

	getLayout(m_nRow)->setSpacing(0);
	getLayout(m_nRow)->setMargin(0);

	if (ROIList)
	{
		addWidget(ROIList, m_nRow++, 0, QMargins(0, 0, 0, 0), Qt::Alignment(Qt::AlignTop));
	}
}

void ROITab2::addButtonWidgets()
{
	QWidget* emptyBox1 = new QWidget(this);
	emptyBox1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	//m_btnFitBox = new QPushButton(this);
	m_btnFitBox = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Segmentation_Workingregion, this);
	if (m_btnFitBox)
	{
		m_btnFitBox->setIcon(RESOURCE_MANAGER->getIcon(ICON_VIEW_NON_FIT_COORD, m_nIconSize, m_nIconSize));
		m_btnFitBox->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnFitBox->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnFitBox->setObjectName("ButtonFit");
		m_btnFitBox->setMouseTracking(true);
		m_btnFitBox->installEventFilter(this);
		m_btnFitBox->setStyleSheet("color: black;");
		m_btnFitBox->setToolTip("Fit to Layer region proposal(Working region)");
		connect(m_btnFitBox, &QPushButton::released, this, &ROITab2::slot_OnFitBoundingBox);
	}

	//m_btnAnalyze = new QPushButton(this);
	m_btnAnalyze = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_Radiomics, this);
	if (m_btnAnalyze)
	{
		m_btnAnalyze->setStyleSheet(STYLE_MANAGER->buttonBehind);
		m_btnAnalyze->setText("Analyze");
		m_btnAnalyze->setToolTip("Analyze current layer");
		m_btnAnalyze->hide();
		if (m_btnAnalyze && ROIList)
			connect(m_btnAnalyze, &QPushButton::clicked, ROIList, &MaskListWidget::OnFeatureExtractor);
	}

	//m_btnAdd = new QPushButton(this);
	m_btnAdd = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_LayerOperation_GeneralFunction, this);

	if (m_btnAdd)
	{
		m_btnAdd->setIcon(RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_NEW, m_nIconSize, m_nIconSize));
		m_btnAdd->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnAdd->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnAdd->setMouseTracking(true);
		m_btnAdd->installEventFilter(this);
		m_btnAdd->setToolTip("Add new layer");
		m_btnAdd->setStyleSheet("color: black;");
		m_btnAdd->setObjectName("ButtonAdd");

		connect(m_btnAdd, &QPushButton::clicked, this, &ROITab2::slot_OnAdd);
	}

	//m_btnDup = new QPushButton(this);
	m_btnDup = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_LayerOperation_GeneralFunction, this);
	if (m_btnDup)
	{
		m_btnDup->setIcon(RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_COPY, m_nIconSize, m_nIconSize));
		m_btnDup->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnDup->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnDup->setMouseTracking(true);
		m_btnDup->installEventFilter(this);
		m_btnDup->setToolTip("Duplicate selected layer");
		m_btnDup->setStyleSheet("color: black;");
		m_btnDup->setObjectName("ButtonDup");

		connect(m_btnDup, &QPushButton::clicked, this, &ROITab2::slot_OnDup);
	}

	//m_btnDel = new QPushButton(this);
	m_btnDel = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_LayerOperation_GeneralFunction, this);
	if (m_btnDel)
	{
		m_btnDel->setIcon(RESOURCE_MANAGER->getIcon(ICON_ROILIST_NON_DEL, m_nIconSize, m_nIconSize));
		m_btnDel->setIconSize(QSize(m_nIconSize, m_nIconSize));
		m_btnDel->setFixedSize(QSize(m_nIconSize, m_nIconSize));
		m_btnDel->setMouseTracking(true);
		m_btnDel->installEventFilter(this);
		m_btnDel->setToolTip("Delete selected layers");
		m_btnDel->setStyleSheet("color: black;");
		m_btnDel->setObjectName("ButtonDel");

		connect(m_btnDel, &QPushButton::clicked, this, &ROITab2::slot_OnDel);
	}

	if (m_btnFitBox)
		addWidget(m_btnFitBox, m_nRow);

	addWidget(m_btnAnalyze, m_nRow);

	addWidget(emptyBox1, m_nRow, 0, QMargins(0, 0, 0, 0));

	if (m_btnAdd)
		addWidget(m_btnAdd, m_nRow);

	if (m_btnDup)
		addWidget(m_btnDup, m_nRow);

	if (m_btnDel)
		addWidget(m_btnDel, m_nRow++);

}

void ROITab2::addVoxelInfoWidgets()
{
	QWidget* emptyBox2 = new QWidget(this);
	emptyBox2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_labelVoxel = new QLabel(this);
	//m_labelVoxel = PRODUCT_FACTORY->createWidget<QLabel>(MS_COLLAPS_ROILIST_COMMON, this);
	if (m_labelVoxel)
	{
		m_labelVoxel->setTextInteractionFlags(Qt::TextSelectableByMouse);
		m_labelVoxel->setText("Voxel : 0 (0.00%)");
	}

	// 211209 허 건 과장
	m_btnVoxelCountCalculator = new QPushButton(QString("Calculator"), this);
	m_btnVoxelCountCalculator->setStyleSheet(STYLE_MANAGER->buttonBehind);
	m_btnVoxelCountCalculator->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	connect(m_btnVoxelCountCalculator, &QPushButton::released, ROIList, &MaskListWidget::OnCalculateVoxelRatio);

	if (m_labelVoxel)
		addWidget(m_labelVoxel, m_nRow);

	if (m_btnVoxelCountCalculator)
		addWidget(m_btnVoxelCountCalculator, m_nRow++);

	addWidget(emptyBox2, m_nRow);
}

void ROITab2::addLegacyWidgets()
{

#ifdef DEV_VER
	m_labelBurdenWeight = new QLabel(this);
	//m_labelBurdenWeight = PRODUCT_FACTORY->createWidget<QLabel>(MS_COLLAPS_ROILIST_ADDTIONAL_DATA, this);
	if (m_labelBurdenWeight)
	{
		m_labelBurdenWeight->setTextInteractionFlags(Qt::TextSelectableByMouse);
		m_labelBurdenWeight->setText("Burden : 0 g");
		m_labelBurdenWeight->setAlignment(Qt::AlignRight);

		addWidget(m_labelBurdenWeight, nRow++);
	}

#endif

	if (0)
	{
		QWidget* emptyBox3 = new QWidget(this);
		emptyBox3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

		m_labelAxialArea = new QLabel(this);
		m_labelAxialArea->setText("");

		m_btnAxialArea = new QPushButton(this);
		m_btnAxialArea->setText("Axial Surface Area");
		m_btnAxialArea->setStyleSheet(STYLE_MANAGER->buttonBehind);

		connect(m_btnAxialArea, &QPushButton::released, this, &ROITab2::slot_OnAxialArea);
		addWidget(emptyBox3, m_nRow);
		addWidget(m_labelAxialArea, m_nRow);
		addWidget(m_btnAxialArea, m_nRow++);
	}
}

void ROITab2::slot_OnMerge()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = ROIList->selectedItems();
	QList<QTreeWidgetItem*> Merlist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = ROIList->indexOfTopLevelItem(item);

		if (!Merlist.contains(item))
			Merlist.push_back(item);
	}

	if (Merlist.size() <= 1)
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_MERGE), STRING_MANAGER->getString(STR_SELECT_MORE));
		return;
	}
	std::vector<muint32> indeces;
	for (int cnt = 0; cnt < Merlist.size(); cnt++)
	{
		indeces.push_back(ROIList->indexOfTopLevelItem(Merlist[cnt]));
	}

	ACTION_MANAGER->action_MaskList_merge(DATA_CONTEXT, indeces);
}

void ROITab2::slot_OnErosion()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageErosion(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
}

void ROITab2::slot_OnDilation()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_ImageDilation(WIN_MANAGER->getSelectedMask(), WIN_MANAGER->getSelectedMaskByteIndex());
}


void ROITab2::slot_OnAdd()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	ACTION_MANAGER->action_MaskList_add();
}

void ROITab2::slot_OnDup()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = ROIList->selectedItems();
	QList<QTreeWidgetItem*> Duplist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = ROIList->indexOfTopLevelItem(item);

		if (!Duplist.contains(item))
			Duplist.push_back(item);
	}

	if (Duplist.size() > 1)
	{
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1006)).exec();
		return;
	}

	muint32 index = ROIList->indexOfTopLevelItem(Duplist.at(0));

	ACTION_MANAGER->action_MaskList_copy(index);
}

void ROITab2::slot_OnDel()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	if (!ACTION_MANAGER->isActionFinished())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), STRING_MANAGER->getString(ERR_DU_1016_STR_ALREADY_EXIST_THREAD));
		return;
	}

	QList<QTreeWidgetItem*>& list = ROIList->selectedItems();
	QList<QTreeWidgetItem*> Dellist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = ROIList->indexOfTopLevelItem(item);

		if (!Dellist.contains(item))
			Dellist.push_back(item);
	}



	if (Dellist.size() >= DATA_CONTEXT->volume_data.getMaskInfoListCnt())
	{
		QMessageBox::warning(this, STRING_MANAGER->getString(STR_WARN), QString("Only %1 or fewer layers can be deleted.").
			arg(DATA_CONTEXT->volume_data.getMaskInfoListCnt() - 1));
		return;
	}

	if (Dellist.size() == 1)
	{
		muint32 index = ROIList->indexOfTopLevelItem(Dellist.at(0));
		MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(index);
		int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

		//ACTION_MANAGER->action_MaskList_del(index, mI);
		ACTION_MANAGER->action_MaskList_del_ex(&DATA_CONTEXT->volume_data, index, mI);
	}
	else if (Dellist.size() > 1)
	{
		std::vector<int> indeces;
		for (int cnt = 0; cnt < Dellist.size(); cnt++)
		{
			indeces.push_back(ROIList->indexOfTopLevelItem(Dellist[cnt]));
		}

		ACTION_MANAGER->action_MaskList_del_list_ex(&DATA_CONTEXT->volume_data, indeces);
	}

}

void ROITab2::slot_OnAxialArea()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	WindowBase* axialWindow = WIN_MANAGER->mainSegmentWidget->getWindow(WT_AXIAL);
	int axialDepth = axialWindow->getDepth();
	//qDebug() << axialWindow->getDepth();

	int z = axialDepth;
	int length = DATA_CONTEXT->volume_data.getVolumeDataLength();
	muint32 cx, cy, cz;

	DATA_CONTEXT->volume_data.getLengthForScreen(WT_AXIAL, cx, cy, cz);

	MaskInfo* info = DATA_CONTEXT->volume_data.getCurrentMaskInfo();

	if (info == NULL)
		return;

	int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;
	uchar _m = mI == 0 ? info->mask_id : info->mask_id2;

	uchar* dt = DATA_CONTEXT->volume_data.getMaskDataPoint(mI);

	if (dt == NULL)
		return;

	int countAxialVoxel = 0;
	for (int y = 0; y < cy; y++)
	{
		for (int x = 0; x < cx; x++)
		{
			int index = z * cx * cy + y * cx + x;

			if (index < 0 || index >= length)
				continue;

			if (dt[index] & _m)
				countAxialVoxel++;
		}
	}

	QString str = countAxialVoxel > 0 ? QString("%1mm%2").arg(QString::number(DATA_CONTEXT->volume_data.getSpace2D(true) * countAxialVoxel, 'f', 2)).arg(QString(QChar(0x00B2))) : "-";
	m_labelAxialArea->setText(QString("Axial Area : %1 (%2)")
		.arg(countAxialVoxel)
		.arg(str)
	);


	// copy to clipboard
	QString valText = str;
	auto clip = QApplication::clipboard();
	clip->clear();
	clip->setText(valText);
}


void ROITab2::slot_OnFitBoundingBox()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;
	DATA_CONTEXT->volume_data.setPreBoundingBox();

	QList<QTreeWidgetItem*>& list = ROIList->selectedItems();

	QList<QTreeWidgetItem*> Fitlist;

	for (int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem* item = list.at(i);

		if (item->childCount() == 0)
			item = item->parent();

		muint32 index = ROIList->indexOfTopLevelItem(item);

		if (!Fitlist.contains(item))
			Fitlist.push_back(item);
	}

	BoundingBoxI resetBox = DATA_CONTEXT->volume_data.getBoundingBox();
	resetBox.reset(DATA_CONTEXT->volume_data.getCX(), DATA_CONTEXT->volume_data.getCY(), DATA_CONTEXT->volume_data.getCZ());
	if (Fitlist.size() > 1)
	{
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_1007)).exec();
		return;
	}
	muint32 index = DATA_CONTEXT->volume_data.getCurrentMaskInfoID();
	bool isEmpty = DATA_CONTEXT->volume_data.isEmptyMaskVoxel(index);

	if (isEmpty)
		ACTION_MANAGER->action_BoundingBox_Modify(resetBox);
	else
	{
		bool isFit = DATA_CONTEXT->volume_data.compareBoundingBox(index);

		if (isFit)
			ACTION_MANAGER->action_BoundingBox_Modify(resetBox);
		else
			ACTION_MANAGER->action_BoundingBox_Modify(DATA_CONTEXT->volume_data.getBoundingBox(DATA_CONTEXT->volume_data.getCurrentMaskInfo()->uid));
		//	ACTION_MANAGER->action_BoundingBox_Modify(DATA_CONTEXT->volume_data.boundingBoxROI[DATA_CONTEXT->volume_data.getCurrentMaskInfo()->uid]);
	}
}
