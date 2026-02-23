#include "stdafx.h"
#include "SummaryTab.h"
#include "WindowManager.h"
#include "ResourceManager.h"
#include "styleManager.h"
#include "StringManager.h"
#include "ProductManager.h"
#include "LicenseManager.h"
#include "DataContext.h"

SummaryTab::SummaryTab(QWidget* parent /*= NULL*/)
	:CollapseWidget(QString(), parent),
	m_isFirstSeriesInfoInitialized(true)
{
	int nRow = 0;

	m_infoList = new QTreeWidget(this);
	//m_infoList = PRODUCT_FACTORY->createWidget<QTreeWidget>(MS_COLLAPS_SUMMARY_TAB, this);
	if (m_infoList)
	{
		m_infoList->setColumnCount(2);
		m_infoList->setHeaderHidden(true);
		m_infoList->setStyleSheet(STYLE_MANAGER->treeBasicList);
		m_infoList->setRootIsDecorated(false);
		m_infoList->setSelectionMode(QAbstractItemView::ExtendedSelection);
		addWidget(m_infoList, nRow++);
	}

	setSummary();

	QWidget* widgetButtonList = new QWidget(this);
	QBoxLayout* groupHori = new QHBoxLayout;
	widgetButtonList->setLayout(groupHori);

	m_btnCopy = new QPushButton(this);
	//QPushButton *btnCopy = PRODUCT_FACTORY->createWidget<QPushButton>(MS_COLLAPS_SUMMARY_TAB, this);
	if (m_btnCopy)
	{
		m_btnCopy->setText("Copy Info");
		m_btnCopy->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(m_btnCopy, &QPushButton::clicked, this, &SummaryTab::slot_OnCopy);
		groupHori->addWidget(m_btnCopy);
	}

	m_btnUpdateInfo = new QPushButton(this);
	if (m_btnUpdateInfo)
	{
		m_btnUpdateInfo->setText("Update Info");
		m_btnUpdateInfo->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(m_btnUpdateInfo, &QPushButton::clicked, this, &SummaryTab::slot_OnUpdateInfo);
		groupHori->addWidget(m_btnUpdateInfo);

	}
	addWidget(widgetButtonList, nRow++, 0, QMargins(5, 15, 5, 5));

	QWidget* emptyBox0 = new QWidget(this);
	//QWidget *emptyBox0 = PRODUCT_FACTORY->createWidget<QWidget>(MS_COLLAPS_SUMMARY_TAB, this);
	if (emptyBox0)
	{
		emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		addWidget(emptyBox0, nRow++, 0);
	}
	QString title = "Summary";
	setTitle(title);
	setOpenWidget(false);
}

void SummaryTab::setSummary()
{
	m_infoList->clear();

	DcmtkSeriesInfo* pDcmSeriesInfo = WIN_MANAGER->GetDicomInfo();

	AddItemToList(STRING_MANAGER->getString(STR_STUDY_INSTANCE_UID), QString::fromStdString(pDcmSeriesInfo->strStudyUID));
	AddItemToList(STRING_MANAGER->getString(STR_SERIES_INSTANCE_UID), QString::fromStdString(pDcmSeriesInfo->strSeriesUID));
	AddItemToList(STRING_MANAGER->getString(STR_MODALITY), QString::fromStdString(pDcmSeriesInfo->modality_));
	AddItemToList(STRING_MANAGER->getString(STR_SERIES_DESC), QString::fromStdString(pDcmSeriesInfo->description_));
	AddItemToList(STRING_MANAGER->getString(STR_STUDY_DESC), QString::fromStdString(pDcmSeriesInfo->studyDescription));
	AddItemToList(STRING_MANAGER->getString(STR_ID), QString::fromStdString(pDcmSeriesInfo->patientId_));
	AddItemToList(STRING_MANAGER->getString(STR_EXAM_DATE), QString::fromStdString(pDcmSeriesInfo->seriesDate));
	AddItemToList(STRING_MANAGER->getString(STR_NAME), QString::fromStdString(pDcmSeriesInfo->patientsName_));
	AddItemToList(STRING_MANAGER->getString(STR_SEX), QString::fromStdString(pDcmSeriesInfo->sex_));
	AddItemToList(STRING_MANAGER->getString(STR_AGE), QString::fromStdString(pDcmSeriesInfo->age_));
	AddItemToList(STRING_MANAGER->getString(STR_WEIGHT), QString::fromStdString(pDcmSeriesInfo->patientWeight_));

	AddItemToList(STRING_MANAGER->getString(STR_WXH),
		QString("%1x%2x%3")
		.arg(DATA_CONTEXT->volume_data.getCX())
		.arg(DATA_CONTEXT->volume_data.getCY())
		.arg(DATA_CONTEXT->volume_data.getCZ()));

	AddItemToList(STRING_MANAGER->getString(STR_X_SPACING), QString::number(DATA_CONTEXT->volume_data.getSpaceX(true)));
	AddItemToList(STRING_MANAGER->getString(STR_Y_SPACING), QString::number(DATA_CONTEXT->volume_data.getSpaceY(true)));
	AddItemToList(STRING_MANAGER->getString(STR_Z_SPACING), QString::number(DATA_CONTEXT->volume_data.getSpaceZ(true)));
	AddItemToList(STRING_MANAGER->getString(STR_IMG_POS), QString::fromStdString(pDcmSeriesInfo->imagePosition));
	AddItemToList(STRING_MANAGER->getString(STR_IMG_ORIENTATION), GetOrientationText());
	AddItemToList(STRING_MANAGER->getString(STR_MANUFACTURER), QString::fromStdString(pDcmSeriesInfo->Manufacturer));
	AddItemToList(STRING_MANAGER->getString(STR_MANUFACTURER_MODEL), QString::fromStdString(pDcmSeriesInfo->manufacturerModel));
	AddItemToList(STRING_MANAGER->getString(STR_KERNEL), QString::fromStdString(pDcmSeriesInfo->convolutionkernel));
	AddItemToList(STRING_MANAGER->getString(STR_KVP), QString::fromStdString(pDcmSeriesInfo->kvp_));
	AddItemToList(STRING_MANAGER->getString(STR_MAS), QString::fromStdString(pDcmSeriesInfo->ma_));
	AddItemToList(STRING_MANAGER->getString(STR_EXAM_ID), QString::fromStdString(pDcmSeriesInfo->examID));
	AddItemToList(STRING_MANAGER->getString(STR_ACCESSION_NUMBER), QString::fromStdString(pDcmSeriesInfo->accessionNumber));

	m_infoList->header()->setStretchLastSection(false);
	m_infoList->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_infoList->resizeColumnToContents(0);
}

void SummaryTab::AddItemToList(QString name, QString value)
{
	QTreeWidgetItem* item = new QTreeWidgetItem(m_infoList);

	if (value.length() < 1)
	{
		value = QString("No data");
	}

	item->setText(0, name);
	item->setText(1, value);

	m_infoList->addTopLevelItem(item);
}

QString SummaryTab::GetOrientationText()
{
	float xdir[3];
	float ydir[3];

	DATA_CONTEXT->volume_data.getImgOrientation(true, xdir);
	DATA_CONTEXT->volume_data.getImgOrientation(false, ydir);

	QString imageOrientationText = QString("%1/%2/%3/%4/%5/%6")
		.arg(QString::number(xdir[0], 'f', xdir[0] == 1 || xdir[0] == 0 ? 0 : 7))
		.arg(QString::number(xdir[1], 'f', xdir[1] == 1 || xdir[1] == 0 ? 0 : 7))
		.arg(QString::number(xdir[2], 'f', xdir[2] == 1 || xdir[2] == 0 ? 0 : 7))
		.arg(QString::number(ydir[0], 'f', ydir[0] == 1 || ydir[0] == 0 ? 0 : 7))
		.arg(QString::number(ydir[1], 'f', ydir[1] == 1 || ydir[1] == 0 ? 0 : 7))
		.arg(QString::number(ydir[2], 'f', ydir[2] == 1 || ydir[2] == 0 ? 0 : 7));

	return imageOrientationText;
}

bool SummaryTab::IsUploadDicomDataNotExist()
{
	DcmtkSeriesInfo* pDicomSeriesInfo = WIN_MANAGER->GetDicomInfo();
	return
		pDicomSeriesInfo->strStudyUID.empty() ||
		pDicomSeriesInfo->accessionNumber.empty() ||
		pDicomSeriesInfo->examID.empty();
}

void SummaryTab::slot_OnCopy()
{
	QList<QTreeWidgetItem*>& list = m_infoList->selectedItems();
	QString valText = "";
	auto clip = QApplication::clipboard();
	clip->clear();
	if (list.size() <= 0)
	{
		for (int i = 0; i < m_infoList->topLevelItemCount(); i++)
		{
			valText.append(m_infoList->topLevelItem(i)->text(0));
			valText.append(" : ");
			valText.append(m_infoList->topLevelItem(i)->text(1));
			valText.append("\r\n");
		}

		clip->setText(valText);

		return;
	}

	for (int i = 0; i < list.size(); i++)
	{
		valText.append(list[i]->text(0));
		valText.append(" : ");
		valText.append(list[i]->text(1));
		valText.append("\r\n");
	}
	clip->setText(valText);

}

void SummaryTab::slot_OnUpdateInfo()
{
	WIN_MANAGER->importDicomFile_And_UpdateSummary();
}

