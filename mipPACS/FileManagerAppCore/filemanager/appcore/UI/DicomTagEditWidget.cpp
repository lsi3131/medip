#include "stdafx.h"
#include "DicomTagEditWidget.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "filemanager/dicom/DicomDataset.h"
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcelem.h>
#include <dcmtk/dcmdata/dcsequen.h>
#include <dcmtk/dcmdata/dcitem.h>
#include <qclipboard>

namespace fm
{
	DicomTagEditWidget::DicomTagEditWidget(QWidget* parent) :
		m_pDicomSeries(nullptr)
	{
		setupUi(this);
		m_colHeaderInfoList_DicomTag.Add(COL_TAG_ID, "TAG ID", true);
		m_colHeaderInfoList_DicomTag.Add(COL_VR, "VR", true);
		m_colHeaderInfoList_DicomTag.Add(COL_VM, "VM", false);
		m_colHeaderInfoList_DicomTag.Add(COL_LENGTH, "Length", false);
		m_colHeaderInfoList_DicomTag.Add(COL_DESCRIPTION, "Description", true);
		m_colHeaderInfoList_DicomTag.Add(COL_VALUE, "Value", true);

		m_treeDicomTag->setHeaderLabels(m_colHeaderInfoList_DicomTag.GetNameHeaderList());
		m_treeDicomTag->setSortingEnabled(true);
		m_treeDicomTag->sortByColumn(0, Qt::AscendingOrder);
		for (int idx : m_colHeaderInfoList_DicomTag.GetInvisibleHeaderIndexList())
		{
			m_treeDicomTag->hideColumn(idx);
		}
		for (int idx : m_colHeaderInfoList_DicomTag.GetVisibleHeaderIndexList())
		{
			m_treeDicomTag->showColumn(idx);
		}
		connect(m_sliderSeriesRange, &QSlider::valueChanged, this, &DicomTagEditWidget::onSliderSeriesRangeChanged);
		connect(m_editSearchFilter, &QLineEdit::textChanged, this, &DicomTagEditWidget::onEditSearchFilterChanged);
		connect(m_btnCopyPath, &QPushButton::clicked, this, &DicomTagEditWidget::onBtnCopyDicomPath);
		connect(m_btnExpandAll, &QPushButton::clicked, this, &DicomTagEditWidget::onBtnExpandAll);
		connect(m_btnCollpaseAll, &QPushButton::clicked, this, &DicomTagEditWidget::onBtnCollapsedAll);
		connect(m_btnCopyData, &QPushButton::clicked, this, &DicomTagEditWidget::onBtnCopyData);

		m_btnCopyData->setVisible(false);
	}

	void DicomTagEditWidget::SetDicomSeries(DicomInfomationModelSeriesObject* pSeries, bool forceToUpdate)
	{
		m_pDicomSeries = pSeries;

		if (m_pDicomSeries == nullptr)
		{
			return;
		}

		InitSliderBySeries();
		UpdateIndexRangeTextBySeries();

		if (IsEmpty() || forceToUpdate)
		{
			UpdateControl();
		}
	}

	bool DicomTagEditWidget::IsEmpty()
	{
		return (m_treeDicomTag->topLevelItemCount() == 0);
	}

	void DicomTagEditWidget::Clear()
	{
		m_pDicomSeries = nullptr;
		m_treeDicomTag->clear();
	}

	void DicomTagEditWidget::InitSliderBySeries()
	{
		if (m_pDicomSeries == nullptr)
		{
			return;
		}

		m_sliderSeriesRange->blockSignals(true);
		m_sliderSeriesRange->setRange(0, m_pDicomSeries->ImageInstanceCount() - 1);
		m_sliderSeriesRange->blockSignals(false);
	}

	void DicomTagEditWidget::UpdateIndexRangeTextBySeries()
	{
		int index = m_sliderSeriesRange->value() + 1;
		int maxRange = m_sliderSeriesRange->maximum() + 1;
		m_editSeriesIndex->setText(QString("%1 / %2").arg(index).arg(maxRange));
	}

	bool DicomTagEditWidget::GetCurSelDicomDataset(fm::DicomDataset*& pOutDicomDataset)
	{
		if (m_pDicomSeries == nullptr)
		{
			return false;
		}

		int index = m_sliderSeriesRange->value();
		return m_pDicomSeries->Get(&pOutDicomDataset, index);
	}

	void DicomTagEditWidget::UpdateControl()
	{
		UpdateDicomTagTree();
		UpdateFilePath();
	}

	void DicomTagEditWidget::UpdateDicomTagTree()
	{
		fm::DicomDataset* pDcmDataset = nullptr;
		if (GetCurSelDicomDataset(pDcmDataset) == false)
		{
			qWarning() << "can't get dicom dataset from series data";
			return;
		}

		m_treeDicomTag->clear();
		SetTreeWidgetByDcmItem_Recursive(pDcmDataset->Data(), nullptr);

		m_treeDicomTag->expandAll();
		for (int i = 0; i < m_treeDicomTag->header()->count(); ++i)
		{
			m_treeDicomTag->resizeColumnToContents(i);
		}

	}

	void DicomTagEditWidget::UpdateFilePath()
	{
		fm::DicomDataset* pDicomDataset = nullptr;
		if (GetCurSelDicomDataset(pDicomDataset))
		{
			std::wstring filepath = pDicomDataset->GetFilePath_Extension();
			m_editFilePath->setText(QString::fromStdWString(filepath));
		}
	}

	void DicomTagEditWidget::SetTreeWidgetByDcmItem_Recursive(DcmItem* dcmItem, QTreeWidgetItem* parentTreeItem)
	{
		if (dcmItem)
		{
			int card = dcmItem->card();
			for (int i = 0; i < card; ++i)
			{
				DcmElement* element = dcmItem->getElement(i);
				SetTreeWidgetByDcmElement(element, parentTreeItem);
			}
		}
	}

	void DicomTagEditWidget::SetTreeWidgetByDcmElement(DcmElement* element, QTreeWidgetItem* parentTreeItem)
	{
		QTreeWidgetItem* newTreeItem = CreateNewDicomTagTreeItem(element);

		QString searchFilter = m_editSearchFilter->text().toLower();
		QString tagID = newTreeItem->text(COL_TAG_ID).toLower();
		QString vr = newTreeItem->text(COL_VR).toLower();
		QString vm = newTreeItem->text(COL_VM).toLower();
		QString length = newTreeItem->text(COL_LENGTH).toLower();
		QString tagName = newTreeItem->text(COL_DESCRIPTION).toLower();
		QString value = newTreeItem->text(COL_VALUE).toLower();

		if (tagID.contains(searchFilter) || tagName.contains(searchFilter) || value.contains(searchFilter))
		{
			if (parentTreeItem)
			{
				parentTreeItem->addChild(newTreeItem);
			}
			else
			{
				m_treeDicomTag->addTopLevelItem(newTreeItem);
			}
		}

		if (element->isLeaf())
		{
		}
		else if (element->isNested())
		{
		}
		else
		{
			/* dcm sequence item -> 신규 item에 추가 */
			DcmSequenceOfItems* dcmSeqItem = (DcmSequenceOfItems*)element;
			int card = dcmSeqItem->card();
			for (int i = 0; i < card; ++i)
			{
				DcmItem* dcmItem = dcmSeqItem->getItem(i);
				SetTreeWidgetByDcmItem_Recursive(dcmItem, newTreeItem);
			}
		}
	}

	QTreeWidgetItem* DicomTagEditWidget::CreateNewDicomTagTreeItem(DcmElement* element)
	{
		DcmTag tag = element->getTag();

		OFString value;
		int vm = element->getVM();
		DcmEVR vr = element->getVR();
		int length = element->getLength();

		int pos = 0;
		OFString tempData;
		for (pos = 0; pos < vm - 1; ++pos)
		{
			if (element->getOFString(tempData, pos).good())
			{
				value += tempData + "\\";
			}
		}
		if (element->getOFString(tempData, pos).good())
		{
			value += tempData;
		}

		QString tagID = tag.toString().c_str();
		QString tagName = tag.getTagName();
		QString dataValue = QString::fromStdWString(StringUtil::OFStringToWideString(value));

		QTreeWidgetItem* newTreeItem = new QTreeWidgetItem();
		newTreeItem->setText(COL_TAG_ID, tagID);
		newTreeItem->setText(COL_VR, DcmVR(vr).getVRName());
		newTreeItem->setText(COL_VM, QString::number(vm));
		newTreeItem->setText(COL_LENGTH, QString::number(length));
		newTreeItem->setText(COL_DESCRIPTION, tagName);
		newTreeItem->setText(COL_VALUE, dataValue);

		QVariant elementData = qVariantFromValue<void*>(element);
		newTreeItem->setData(0, Qt::UserRole, elementData);
		newTreeItem->setFlags(newTreeItem->flags() | Qt::ItemIsEditable);

		return newTreeItem;
	}

	void DicomTagEditWidget::onSliderSeriesRangeChanged(int value)
	{
		UpdateControl();
		UpdateIndexRangeTextBySeries();
	}

	void DicomTagEditWidget::onEditSearchFilterChanged(const QString& text)
	{
		UpdateDicomTagTree();
	}

	void DicomTagEditWidget::onBtnCopyDicomPath()
	{
		QClipboard* clipboard = QGuiApplication::clipboard();
		QString filepath = m_editFilePath->text();
		clipboard->setText(filepath);
	}

	void DicomTagEditWidget::onBtnCopyData()
	{

	}

	void DicomTagEditWidget::onBtnExpandAll()
	{
		m_treeDicomTag->expandAll();
	}

	void DicomTagEditWidget::onBtnCollapsedAll()
	{
		m_treeDicomTag->collapseAll();
	}
}
