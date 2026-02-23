#include "stdafx.h"
#include "DicomEditDialog.h"
#include "filemanager/dicom/DicomInfomationModel.h"
#include "filemanager/dicom/DicomDataset.h"
#include <qtablewidget.h>
#include <QMessageBox>

#define ROW_SEPRATOR_ID -1

namespace fm
{
	DicomEditDialog::DicomEditDialog(QWidget* parent, std::vector<DicomInfomationModelStudyObject*> dicomStudyModelList) :
		QDialog(parent),
		m_originDicomStudyModelList(dicomStudyModelList)
	{
		setupUi(this);

		/* Table Record List ÃÊ±âÈ­ */
		m_tableRecordList.push_back(TableRecord{ (int)ROW_SEPRATOR_ID, QString("Patient Tag Name"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::PatientName, QString("Patient Name"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::PatientID, QString("Patient ID"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::PatientSex, QString("Patient Sex"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::PatientAge, QString("Patient Age"), QString("") });
		//m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::PatientBirthDate, QString("Patient BirthDate"), QString("") });

		m_tableRecordList.push_back(TableRecord{ (int)ROW_SEPRATOR_ID, QString("Study Tag Name"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::StudyDescription, QString("Study Description"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::StudyDate, QString("Study Date"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::StudyTime, QString("Study Time"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::StudyID, QString("Study ID(Exam ID)"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::AccessionNumber, QString("Accession Number"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::ReferringPhysicianName, QString("Referring Physician"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::PerformingPhysicianName, QString("Performing Physician"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::RequestingPhysician, QString("Requesting Physician"), QString("") });
		m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::InstitutionName, QString("Institution"), QString("") });

		//m_tableRecordList.push_back(TableRecord{ (int)ROW_SEPRATOR_ID, QString("Series Tag Name"), QString("") });
		//m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::SeriesDescription, QString("Series Description"), QString("") });
		//m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::FrameOfReferenceUID, QString("FrameOfReferenceUID"), QString("") });
		//m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::Manufacturer, QString("Manufacturer"), QString("") });
		//m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::ManufacturerModelName, QString("Manufacturer Model"), QString("") });
		//m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::Modality , QString("Modality"), QString("") });
		//m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::SeriesDate , QString("Series Date"), QString("") });
		//m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::SeriesTime , QString("Series Time"), QString("") });
		//m_tableRecordList.push_back(TableRecord{ (int)DicomTagID::SeriesNumber , QString("Series Number"), QString("") });

		m_btnSave->setStyleSheet(g_ResourceManager.GetButtonSave());
		m_btnCancel->setStyleSheet(g_ResourceManager.GetButtonCancel());

		m_colHeaderInfoList_DicomTag.Add(0, "Name", true);
		m_colHeaderInfoList_DicomTag.Add(1, "Value", true);

		m_tableStudyTAG->clear();
		m_tableStudyTAG->setColumnCount(m_colHeaderInfoList_DicomTag.GetNameHeaderList().size());
		m_tableStudyTAG->setHorizontalHeaderLabels(m_colHeaderInfoList_DicomTag.GetNameHeaderList());

		connect(m_btnSave, &QPushButton::clicked, this, &DicomEditDialog::onSave);
		connect(m_btnCancel, &QPushButton::clicked, this, &DicomEditDialog::onCancel);

		connect(m_listDicomStudy, &QListWidget::currentItemChanged, this, &DicomEditDialog::onListDicomStudyCurrentItemChanged);
		connect(m_listDicomSeries, &QListWidget::currentItemChanged, this, &DicomEditDialog::onListDicomSeriesCurrentItemChanged);

		UpdateDicomStudyList();
	}

	void DicomEditDialog::UpdateDicomStudyList()
	{
		m_listDicomStudy->clear();
		for (DicomInfomationModelStudyObject* pStudy : m_originDicomStudyModelList)
		{
			if (pStudy)
			{
				DicomDataset* pDatasetStudy;
				if (pStudy->GetStudyDataset(&pDatasetStudy))
				{
					QString studyDesc = QString::fromStdWString(pDatasetStudy->GetValueWString(fm::DicomTagID::StudyDescription));
					QString name = QString("%1").arg(studyDesc);
					QVariant data = qVariantFromValue<void*>(pStudy);
					QListWidgetItem* item = new QListWidgetItem();
					item->setText(name);
					item->setData(Qt::UserRole, data);
					m_listDicomStudy->addItem(item);
				}
			}
		}
	}

	void DicomEditDialog::UpdateDicomSeriesList_By_CurSelStudy()
	{
		DicomInfomationModelStudyObject* pStudy = GetCurselStudy();
		if (pStudy == nullptr)
		{
			return;
		}

		m_listDicomSeries->clear();
		for (DicomInfomationModelSeriesObject* pSeries : pStudy->GetSeriesList())
		{
			if (pSeries)
			{
				DicomDataset* pDatasetSeries;
				if (pSeries->GetFirst(&pDatasetSeries))
				{
					QString seriesDesc = QString::fromStdWString(pDatasetSeries->GetValueWString(fm::DicomTagID::SeriesDescription));
					QString name = QString("%1").arg(seriesDesc);
					QVariant data = qVariantFromValue<void*>(pSeries);
					QListWidgetItem* item = new QListWidgetItem();
					item->setText(name);
					item->setData(Qt::UserRole, data);
					m_listDicomSeries->addItem(item);
				}
			}
		}
	}

	void DicomEditDialog::UpdateDicomTagList()
	{
		DicomInfomationModelStudyObject* pStudy = GetCurselStudy();
		if (pStudy == nullptr)
		{
			return;
		}

		DicomDataset* pDatasetStudy;
		if (pStudy->GetStudyDataset(&pDatasetStudy) == false)
		{
			return;
		}

		UpdateRecordList_By_DicomDataset(pDatasetStudy);
		UpdateTableWidget_By_RecordList();
	}

	DicomInfomationModelStudyObject* DicomEditDialog::GetCurselStudy()
	{
		QListWidgetItem* currentItem = m_listDicomStudy->currentItem();
		if (currentItem == nullptr)
		{
			return nullptr;
		}

		QVariant data = currentItem->data(Qt::UserRole);
		DicomInfomationModelStudyObject* pStudy = (DicomInfomationModelStudyObject*)data.value<void*>();
		return pStudy;
	}

	void DicomEditDialog::UpdateRecordList_By_DicomDataset(DicomDataset* pDataset)
	{
		for (auto& record : m_tableRecordList)
		{
			if (record.ID == ROW_SEPRATOR_ID)
			{
				//SKIP
			}
			else
			{
				QString value = QString::fromStdWString(pDataset->GetValueWString((DicomTagID)record.ID));
				record.Value = value;
			}
		}
	}

	void DicomEditDialog::UpdateTableWidget_By_RecordList()
	{
		m_tableStudyTAG->setRowCount(m_tableRecordList.size());
		int row = 0;
		for (auto& record : m_tableRecordList)
		{
			QTableWidgetItem* itemName = new QTableWidgetItem(record.Name);
			itemName->setFlags(itemName->flags() & ~Qt::ItemIsEditable);

			QTableWidgetItem* itemValue = new QTableWidgetItem(record.Value);
			m_tableStudyTAG->setItem(row, 0, itemName);
			m_tableStudyTAG->setItem(row, 1, itemValue);

			if (record.ID == ROW_SEPRATOR_ID)
			{
				m_tableStudyTAG->setSpan(row, 0, 1, 2);
				m_tableStudyTAG->item(row, 0)->setBackgroundColor(QColor(0x53, 0x53, 0x53));
				m_tableStudyTAG->item(row, 0)->setFlags(itemName->flags() & ~Qt::ItemIsEditable);
			}
			row++;
		}

		for (int col = 0; col < m_tableStudyTAG->columnCount(); ++col)
		{
			m_tableStudyTAG->resizeColumnToContents(col);
		}
	}

	void DicomEditDialog::UpdateRecordList_By_TableWidget()
	{
		for (int row = 0; row < m_tableStudyTAG->rowCount(); ++row)
		{
			int id = m_tableStudyTAG->item(row, 0)->data(Qt::UserRole).toInt();
			if (id == ROW_SEPRATOR_ID)
			{
				//SKIP
			}
			else
			{
				QString value = m_tableStudyTAG->item(row, 1)->text();
				m_tableRecordList[row].Value = value;
			}
		}
	}

	DicomDataset DicomEditDialog::GetEdittedDicomDataset_Study()
	{
		UpdateRecordList_By_TableWidget();

		DicomDataset dcmDataset;
		for (auto& record : m_tableRecordList)
		{
			if (record.ID == ROW_SEPRATOR_ID)
			{
				//SKIP
			}
			else
			{
				if (!record.Value.isEmpty())
				{
					dcmDataset.SetTagValue((DicomTagID)record.ID, record.Value.toStdWString());
				}
			}
		}

		DICOM_HEADER_INFO info = dcmDataset.ToDcmHeaderInfo();

		return dcmDataset;
	}

	void DicomEditDialog::onSave()
	{
		QMessageBox::StandardButton result = QMessageBox::question(
			this,
			StringManager::GetString(STR_TITLE_QUESTION),
			StringManager::GetString(STR_QUESTION_SAVE),
			QMessageBox::Ok |
			QMessageBox::Cancel);

		if (result == QMessageBox::Cancel)
		{
			return;
		}

		accept();
	}

	void DicomEditDialog::onCancel()
	{
		reject();
	}

	void DicomEditDialog::onListDicomStudyCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
	{
		if (current)
		{
			UpdateDicomSeriesList_By_CurSelStudy();
			UpdateDicomTagList();
		}
	}

	void DicomEditDialog::onListDicomSeriesCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
	{
		if (current)
		{
			UpdateDicomTagList();
		}
	}
}
