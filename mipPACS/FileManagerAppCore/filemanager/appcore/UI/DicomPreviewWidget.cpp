#include "stdafx.h"
#include "DicomPreviewWidget.h"
#include "ui_DicomPreviewWidget.h"
#include "filemanager/dicom/DicomDataset.h"
#include "filemanager/dicom/DicomType.h"
#include "filemanager/dicom/Image/DicomDataImage_sint16.h"
#include "filemanager/dicom/Image/DicomImageToQTConverter.h"
#include "filemanager/dicom/Convert/DicomConverter.h"
#include "filemanager/dicom/DicomInfomationModelSeriesObject.h"
#include "filemanager/data/Image/VolumeImageData.h"
#include "filemanager/render/CPUMPRPlane2DRenderingManager.h"
#include "filemanager/appcore/System/FilePathManager.h"
#include <qlabel>
#include <qlayout>
#include <qdir>
#include <QElapsedTimer>
#include <qtreewidget>
#include <qpainter>
#include <QTextBlock>
#include <QTextItem>
#include <QMessageBox>

namespace fm
{
	DicomPreviewWidget::DicomPreviewWidget(QWidget* parent) :
		QWidget(parent),
		m_pDcmInfomationModel(nullptr)
	{
		m_ui = new Ui::DicomPreviewWidget();
		m_ui->setupUi(this);

		m_pDcmInfomationModel = std::make_shared<DicomInfomationModel>(true);
		m_pDcmInfomationModel->SetEnableSaveNonePixelData(true);

		SetStudyListVisible(true);
		SetSeriesListVisibile(true);

		m_colHeaderInfoList_StudyModel.Add(eTreeColumn::StudyDescription, "Study Description", true);
		m_colHeaderInfoList_StudyModel.Add(eTreeColumn::SeriesDescription, "Series Description", true);
		m_colHeaderInfoList_StudyModel.Add(eTreeColumn::Index, "Index", true);
		m_colHeaderInfoList_StudyModel.Add(eTreeColumn::Modality, "Modality", true);
		m_colHeaderInfoList_StudyModel.Add(eTreeColumn::ImageCount, "Images", true);

		m_ui->m_treeDicomStudy->setHeaderLabels(m_colHeaderInfoList_StudyModel.GetNameHeaderList());
		m_ui->m_treeDicomStudy->setSortingEnabled(true);
		m_ui->m_treeDicomStudy->sortByColumn(0, Qt::AscendingOrder);

		m_ui->m_listDicomSeries->setIconSize(QSize(128, 128));

		/*
			해당 옵션을 추가하지 않으면 이미지를 움직일 때 Assert 발생
			이미지를 움직이 못하도록 Fix
		*/
		m_ui->m_listDicomSeries->setMovement(QListView::Static);

		m_pDcmInfoLoader = new DicomInfomationModelLoader();

		connect(m_pDcmInfoLoader, &DicomInfomationModelLoader::dcmFileLoadedStarted, this, &DicomPreviewWidget::onDcmFileLoadedStarted);
		connect(m_pDcmInfoLoader, &DicomInfomationModelLoader::dcmFileLoadedInProgress, this, &DicomPreviewWidget::onDcmFileLoaded_InProgress, Qt::ConnectionType::QueuedConnection);
		connect(m_pDcmInfoLoader, &DicomInfomationModelLoader::dcmFileLoadedFinished, this, &DicomPreviewWidget::onDcmFileLoadedFinished);

		//m_ui->m_listDicomSeries->setIconSize(QSize(96, 96));

		connect(m_ui->m_treeDicomStudy, &QTreeWidget::currentItemChanged, this, &DicomPreviewWidget::onStudyModelCurrentItemChanged);
		connect(m_ui->m_treeDicomStudy, &QTreeWidget::itemDoubleClicked, this, &DicomPreviewWidget::onStudyModelItemDoubleClicked);

		connect(m_ui->m_listDicomSeries, &QListWidget::currentRowChanged, this, &DicomPreviewWidget::onSupportedSeriesListItemChanged);
		connect(m_ui->m_listDicomSeries, &QListWidget::itemDoubleClicked, this, &DicomPreviewWidget::onSupportedSeriesListItemDoubleClicked);
	}

	bool DicomPreviewWidget::TryGetCurrentSelectedSeries(DicomInfomationModelSeriesObject** ppOutSeries)
	{
		DicomInfomationModelSeriesObject* pSeries = GetCurrentSelectedSeries();
		if (pSeries == nullptr)
		{
			return false;
		}

		**ppOutSeries = *pSeries;
		return true;
	}

	DicomInfomationModelSeriesObject* DicomPreviewWidget::GetCurrentSelectedSeries()
	{
		QListWidgetItem* current = m_ui->m_listDicomSeries->currentItem();
		if (current == nullptr)
		{
			return nullptr;
		}

		QVariant data = current->data(Qt::UserRole);
		if (data.isNull())
		{
			return nullptr;
		}

		DicomInfomationModelSeriesObject* pSeries = (DicomInfomationModelSeriesObject*)data.value<void*>();
		QListWidgetItem* item = FindItemFromSeriesList(pSeries);
		if (item)
		{
			return pSeries;
		}
		else
		{
			return nullptr;
		}
	}

	bool DicomPreviewWidget::SelectCurrentSeries()
	{
		DicomInfomationModelSeriesObject* pSeries = GetCurrentSelectedSeries();
		return selectSeries(pSeries);
	}

	void DicomPreviewWidget::SetDicomInfomationModel(std::shared_ptr<DicomInfomationModel> pDcmInfomationModel)
	{
		m_pDcmInfomationModel = pDcmInfomationModel;

		ForceToUpdate();
	}

	void DicomPreviewWidget::SetDicomInfomationModel_Series(const DicomInfomationModelSeriesObject* pSeries)
	{
		m_pDcmInfomationModel->Clear();
		std::vector<DicomDataset*> dcmDatasetList = pSeries->GetDatasetList();
		for (DicomDataset* pDset : dcmDatasetList)
		{
			m_pDcmInfomationModel->Add(*pDset);
		}

		ForceToUpdate();
	}

	void DicomPreviewWidget::AbortAndWaitLoader()
	{
		if (m_pDcmInfomationModel == nullptr)
		{
			return;
		}

		if (m_pDcmInfoLoader->IsIOInProgress())
		{
			m_pDcmInfoLoader->Abort_IOProgress();
			m_dcmLoaderFuture.waitForFinished();
		}
	}

	void DicomPreviewWidget::Clear()
	{
		//m_pDcmInfomationModel->Clear();
		m_ui->m_listDicomSeries->clear();
		m_ui->m_dicomImageViewer->Clear();
		m_ui->m_dicomTagEditWidget->Clear();
	}

	void DicomPreviewWidget::SetStudyListVisible(bool value)
	{
		m_ui->m_studyListContainer->setVisible(value);
	}

	bool DicomPreviewWidget::IsStudyListVisible()
	{
		return m_ui->m_studyListContainer->isVisible();
	}

	void DicomPreviewWidget::SetSeriesListVisibile(bool value)
	{
		m_ui->m_seriesListContainer->setVisible(value);
	}

	bool DicomPreviewWidget::IsSeriesListVisibile()
	{
		return m_ui->m_seriesListContainer->isVisible();
	}

	void DicomPreviewWidget::RestoreCenterSplitter(const QByteArray& state)
	{
		m_ui->m_splitterCenter->restoreState(state);
	}

	void DicomPreviewWidget::RestoreBottomSplitter(const QByteArray& state)
	{
		m_ui->m_splitterBottom->restoreState(state);
	}

	QByteArray DicomPreviewWidget::SaveCenterSplitter()
	{
		return m_ui->m_splitterCenter->saveState();
	}

	QByteArray DicomPreviewWidget::SaveBottomSplitter()
	{
		return m_ui->m_splitterBottom->saveState();
	}

	void DicomPreviewWidget::Update(bool forceToUpdate)
	{
		UpdateStudyModelTree();
		UpdateSeriesModelList();
		UpdateImageAndDicomTag(forceToUpdate);
	}

	void DicomPreviewWidget::ForceToUpdate()
	{
		Update(true);
	}

	void DicomPreviewWidget::resizeEvent(QResizeEvent* e)
	{
		QWidget::resizeEvent(e);
		bool forceToUpdate = true;
		UpdateImageAndDicomTag(forceToUpdate);
	}

	void DicomPreviewWidget::UpdateStudyModelTree()
	{
		if (m_pDcmInfomationModel == nullptr)
		{
			return;
		}
		m_ui->m_treeDicomStudy->clear();
		std::vector<DicomInfomationModelStudyObject*> studyList = m_pDcmInfomationModel->GetStudyList();
		DicomDataset* pDcmDataset;
		DICOM_HEADER_INFO info;
		for (DicomInfomationModelStudyObject* pStudy : studyList)
		{
			QTreeWidgetItem* itemStudy = CreateStudyTreeWidgetItem(pStudy);
			if (itemStudy)
			{
				m_ui->m_treeDicomStudy->addTopLevelItem(itemStudy);

				/* Series UID별 SeriesWithImageTypList로 분류 진행 */
				std::map<std::wstring, std::vector<DicomInfomationModelSeriesObject*>> seriesWithImageTypeListMap;
				std::vector<DicomInfomationModelSeriesObject*> seriesList = pStudy->GetSeriesList();
				for (DicomInfomationModelSeriesObject* pSeries : seriesList)
				{
					std::wstring seriesUID = pSeries->SeriesInstanceUID();
					auto it = seriesWithImageTypeListMap.find(seriesUID);
					if (it != seriesWithImageTypeListMap.end())
					{
						(*it).second.push_back(pSeries);
					}
					else
					{
						std::vector<DicomInfomationModelSeriesObject*> newSeriesWithImageTypeList;
						newSeriesWithImageTypeList.push_back(pSeries);
						seriesWithImageTypeListMap.insert(std::make_pair(seriesUID, newSeriesWithImageTypeList));
					}
				}

				/* SeriesList 분류 */
				for (auto itPair : seriesWithImageTypeListMap)
				{
					std::vector<DicomInfomationModelSeriesObject*> seriesWithImageTypeList = itPair.second;
					if (!seriesWithImageTypeList.empty())
					{
						QTreeWidgetItem* itemSeries = CreateSeriesTreeWidgetItem(seriesWithImageTypeList[0]);
						if (itemSeries)
						{
							itemStudy->addChild(itemSeries);
						}
						for (int i = 0; i < seriesWithImageTypeList.size(); ++i)
						{
							QTreeWidgetItem* itemSeriesWithImageType = CreateSeriesWithImageTypeTreeWidgetItem(seriesWithImageTypeList[i], i);
							if (itemSeriesWithImageType)
							{
								itemSeries->addChild(itemSeriesWithImageType);
							}
						}
					}
				}

				//std::vector<DicomInfomationModelSeriesObject*> seriesList = pStudy->GetSeriesList();
				//for (DicomInfomationModelSeriesObject* pSeries : seriesList)
				//{
				//	QTreeWidgetItem* itemSeries = CreateSeriesTreeWidgetItem_DicomStudy(pSeries);
				//	if (itemSeries)
				//	{
				//		itemStudy->addChild(itemSeries);
				//	}
				//}
			}
		}
		m_ui->m_treeDicomStudy->expandAll();
		for (int i = 0; i < m_ui->m_treeDicomStudy->header()->count(); ++i)
		{
			m_ui->m_treeDicomStudy->resizeColumnToContents(i);
		}
	}

	void DicomPreviewWidget::UpdateSeriesModelList()
	{
		if (m_pDcmInfomationModel == nullptr)
		{
			return;
		}
		std::vector<DicomInfomationModelStudyObject*> studyList = m_pDcmInfomationModel->GetStudyList();

		m_ui->m_listDicomSeries->clear();
		bool isListEmpty = (m_ui->m_listDicomSeries->count() == 0);

		for (DicomInfomationModelStudyObject* pStudy : studyList)
		{
			std::vector<DicomInfomationModelSeriesObject*> seriesList = pStudy->GetSeriesList();
			for (DicomInfomationModelSeriesObject* pSeries : seriesList)
			{
				QListWidgetItem* item = AddOrModifySeriesToSeriesPreivewList(pSeries);

				if (isListEmpty && (item != nullptr))
				{
					m_ui->m_listDicomSeries->setCurrentItem(item);
					isListEmpty = false;
				}
			}
		}
	}

	void DicomPreviewWidget::UpdateImageAndDicomTag(bool forceToUpdate)
	{
		DicomInfomationModelSeriesObject* pSeries = GetCurrentSelectedSeries();
		m_ui->m_dicomImageViewer->SetDicomSeries(pSeries, forceToUpdate);
		m_ui->m_dicomTagEditWidget->SetDicomSeries(pSeries, forceToUpdate);
	}

	void DicomPreviewWidget::SelectTreeItemBySeries(DicomInfomationModelSeriesObject* pSeries)
	{
		QTreeWidgetItemIterator it(m_ui->m_treeDicomStudy);
		while (*it)
		{
			QVariant data = (*it)->data(eTreeColumn::StudyDescription, Qt::UserRole);
			DicomInfomationModelSeriesObject* pCurSeries = (DicomInfomationModelSeriesObject*)data.value<void*>();

			if (pCurSeries == pSeries)
			{
				m_ui->m_treeDicomStudy->setCurrentItem(*it);
				break;
			}
			++it;
		}
	}

	QTreeWidgetItem* DicomPreviewWidget::CreateStudyTreeWidgetItem(DicomInfomationModelStudyObject* pStudy)
	{
		DicomDataset* pDcmDataset;
		DICOM_HEADER_INFO info;
		if (pStudy->GetStudyDataset(&pDcmDataset) == false)
		{
			return nullptr;
		}

		info = pDcmDataset->ToDcmHeaderInfo();

		QTreeWidgetItem* itemStudy = new QTreeWidgetItem();
		itemStudy->setText(eTreeColumn::StudyDescription, QString::fromStdWString(info.StudyDescription.empty() ? L"N/A" : info.StudyDescription));
		itemStudy->setText(eTreeColumn::SeriesDescription, "");
		itemStudy->setText(eTreeColumn::Index, "");
		itemStudy->setText(eTreeColumn::Modality, "");
		itemStudy->setText(eTreeColumn::ImageCount, "");

		return itemStudy;
	}

	QTreeWidgetItem* DicomPreviewWidget::CreateSeriesTreeWidgetItem(DicomInfomationModelSeriesObject* pSeries)
	{
		DicomDataset* pDcmDataset;
		DICOM_HEADER_INFO info;

		if (pSeries->GetFirst(&pDcmDataset) == false)
		{
			return nullptr;
		}
		info = pDcmDataset->ToDcmHeaderInfo();

		QTreeWidgetItem* itemSeries = new QTreeWidgetItem();
		itemSeries->setText(eTreeColumn::StudyDescription, "");
		itemSeries->setText(eTreeColumn::SeriesDescription, QString::fromStdWString(info.SeriesDescription.empty() ? L"N/A" : info.SeriesDescription));

		//QVariant data = qVariantFromValue<void*>(pSeries);
		//itemSeries->setData(eTreeColumn::StudyDescription, Qt::UserRole, data);

		return itemSeries;
	}

	QTreeWidgetItem* DicomPreviewWidget::CreateSeriesWithImageTypeTreeWidgetItem(DicomInfomationModelSeriesObject* pSeries, int index)
	{
		DicomDataset* pDcmDataset;
		DICOM_HEADER_INFO info;

		if (pSeries->GetFirst(&pDcmDataset) == false)
		{
			return nullptr;
		}
		info = pDcmDataset->ToDcmHeaderInfo();

		QTreeWidgetItem* itemSeries = new QTreeWidgetItem();
		itemSeries->setText(eTreeColumn::StudyDescription, "");
		itemSeries->setText(eTreeColumn::SeriesDescription, "");
		itemSeries->setText(eTreeColumn::Index, QString::number(index));
		itemSeries->setText(eTreeColumn::Modality, QString::fromStdWString(info.Modality));
		itemSeries->setText(eTreeColumn::ImageCount, QString::number(pSeries->ImageInstanceCount()));

		QVariant data = qVariantFromValue<void*>(pSeries);
		itemSeries->setData(eTreeColumn::StudyDescription, Qt::UserRole, data);

		return itemSeries;
	}

	void DicomPreviewWidget::SelectListBySeries(DicomInfomationModelSeriesObject* pSeries)
	{
		for (int row = 0; row < m_ui->m_listDicomSeries->count(); ++row)
		{
			QListWidgetItem* item = m_ui->m_listDicomSeries->item(row);
			if (item)
			{
				QVariant data = item->data(Qt::UserRole);
				if (!data.isNull())
				{
					DicomInfomationModelSeriesObject* pCurSeries = (DicomInfomationModelSeriesObject*)data.value<void*>();
					if (pCurSeries == pSeries)
					{
						m_ui->m_listDicomSeries->setCurrentRow(row);
						break;
					}
				}
			}
		}
	}

	QListWidgetItem* DicomPreviewWidget::AddOrModifySeriesToSeriesPreivewList(DicomInfomationModelSeriesObject* pSeries)
	{
		/* 실시간으로 Icon 업데이트 시 Overhead 발생. */
		QListWidgetItem* findItem = FindItemFromSeriesList(pSeries);

		if (findItem)
		{
			return findItem;
		}
		else
		{
			QListWidgetItem* newItem = new QListWidgetItem();
			QVariant data = qVariantFromValue<void*>(pSeries);
			QIcon icon = GetSeriesItemIcon(pSeries);
			newItem->setIcon(icon);
			newItem->setData(Qt::UserRole, data);
			m_ui->m_listDicomSeries->addItem(newItem);

			return newItem;
		}
	}

	QListWidgetItem* DicomPreviewWidget::FindItemFromSeriesList(DicomInfomationModelSeriesObject* pSeries)
	{
		for (int i = 0; i < m_ui->m_listDicomSeries->count(); ++i)
		{
			QVariant data = m_ui->m_listDicomSeries->item(i)->data(Qt::UserRole);
			void* ptr = data.value<void*>();
			if (ptr == pSeries)
			{
				return m_ui->m_listDicomSeries->item(i);
			}
		}
		return nullptr;
	}

	void DicomPreviewWidget::UpdatePreviewByCurSeries()
	{
		DicomInfomationModelSeriesObject* pSeries = GetCurrentSelectedSeries();
		if (pSeries == nullptr)
		{
			return;
		}

		fm::DicomDataset* pDcmDataset = nullptr;
		if (pSeries->GetFirst(&pDcmDataset) == false)
		{
			qWarning() << "can't get dicom dataset from series data";
			return;
		}

		DicomDataset::EType type = pDcmDataset->GetDicomType();
		if (type == DicomDataset::EType::EncapsulatedPDFStorage)
		{
			m_ui->m_tabPreviewList->setCurrentIndex(PdfTabIndex);
			std::wstring tempPdfFilePath = FilePathManager::Instance()->TempDirPath() + L"temp.pdf";
			DicomConverter::ConvertDcmDataset_To_PDFFile(*pDcmDataset, tempPdfFilePath);
			m_ui->m_dicomPDF->loadFile(QString::fromStdWString(tempPdfFilePath));
		}
		else
		{
			m_ui->m_tabPreviewList->setCurrentIndex(ImageTabIndex);
			bool forceToUpdate = true;
			UpdateImageAndDicomTag(forceToUpdate);
		}
	}

	QIcon DicomPreviewWidget::GetSeriesItemIcon(DicomInfomationModelSeriesObject* pSeries)
	{
		DicomDataset* pDicomDataset = nullptr;
		QString textSeriesDescription = "N/A";
		QString textModalityAndInstanceCount = "";

		int index = (pSeries->ImageInstanceCount() - 1) / 2;

		if (pSeries->Get(&pDicomDataset, index) == false)
		{
			qWarning() << "can't get dicom dataset. index(=" << index << ")";
			return QIcon();
		}

		DICOM_HEADER_INFO info = pDicomDataset->ToDcmHeaderInfo();
		QString seriesDescription = QString::fromStdWString(info.SeriesDescription);
		QString modality = QString::fromStdWString(info.Modality);
		int instanceCount = pSeries->ImageInstanceCount();

		if (!seriesDescription.isEmpty())
		{
			textSeriesDescription = seriesDescription;
		}

		textModalityAndInstanceCount = QString("%1, %2").arg(modality).arg(instanceCount);

		QSize sizeIcon = m_ui->m_listDicomSeries->iconSize();

		QPixmap newPixmap(sizeIcon);
		newPixmap.fill(Qt::black);
		QPainter p(&newPixmap);

		QRect rectDraw;

		int y = 0;
		Draw_ImagePixmap(p, y, pDicomDataset, rectDraw);

		/* 현재 Series Description Block */
		//y += rectDraw.height();
		//Draw_Text(p, y, textSeriesDescription, rectDraw);

		y += rectDraw.height();
		Draw_Text(p, y, textModalityAndInstanceCount, rectDraw);

		p.end();

		QIcon icon = QIcon(newPixmap);
		return icon;
	}

	void DicomPreviewWidget::Draw_ImagePixmap(QPainter& p, int y, DicomDataset* pDicomDataset, QRect& outDrawRect)
	{
		QPixmap pixmapImage = DicomImageToQTConverter::GetPixmap(pDicomDataset);
		QSize iconSize = m_ui->m_listDicomSeries->iconSize();
		float imageRate = 0.75f;
		QSize imageSize = iconSize * imageRate;
		int xImage = (iconSize.width() - imageSize.width()) / 2.0f;
		int yImage = y;
		QRect rectImage = QRect(QPoint(xImage, yImage), imageSize);
		p.drawPixmap(rectImage, pixmapImage);

		outDrawRect = rectImage;
	}

	void DicomPreviewWidget::Draw_Text(QPainter& p, int y, QString text, QRect& outDrawRect)
	{
		QPen pen;
		pen.setWidth(2);
		pen.setColor(Qt::white);
		p.setPen(pen);

		QFont font = p.font();
		QFontMetrics fm(font);

		outDrawRect = fm.boundingRect(text);

		QSize iconSize = m_ui->m_listDicomSeries->iconSize();
		QSize imageSize = outDrawRect.size();
		int xText = (iconSize.width() - imageSize.width()) / 2.0f;
		int yText = y + fm.height();
		QPoint ptText(xText, yText);

		p.drawText(ptText, text);
	}

	bool DicomPreviewWidget::selectSeries(DicomInfomationModelSeriesObject* pSeries)
	{
		if (pSeries == false)
		{
			return false;
		}

		DicomDataset* pDcmDataset = nullptr;
		if (pSeries->GetFirst(&pDcmDataset) == false)
		{
			return false;
		}

		if (isDcmDatasetSupportedForMEDIP(pDcmDataset))
		{
			emit currentSeriesSelected(pSeries);
			return true;
		}
		else
		{
			QMessageBox::warning(this, StringManager::GetString(STR_TITLE_WARNING), StringManager::GetString(STR_WARN_NOT_SUPPORTED_FOR_MEDIP));
			return false;
		}
	}

	bool DicomPreviewWidget::isDcmDatasetSupportedForMEDIP(DicomDataset* pDcmDataset) const
	{
		DicomType dcmType(pDcmDataset);
		return dcmType.IsCompatibleImageToMEDIP();
	}

	void DicomPreviewWidget::onStudyModelCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
	{
		if (current == previous)
		{
			return;
		}

		if (current)
		{
			QVariant data = current->data(eTreeColumn::StudyDescription, Qt::UserRole);
			if (data.isNull())
			{
				return;
			}

			DicomInfomationModelSeriesObject* pSeries = (DicomInfomationModelSeriesObject*)data.value<void*>();
			SelectListBySeries(pSeries);
		}
	}

	void DicomPreviewWidget::onStudyModelItemDoubleClicked(QTreeWidgetItem* item)
	{
		QVariant data = item->data(0, Qt::UserRole);
		if (data.isNull())
		{
			return;
		}

		DicomInfomationModelSeriesObject* pSeries = (DicomInfomationModelSeriesObject*)data.value<void*>();
		selectSeries(pSeries);
	}

	void DicomPreviewWidget::onSupportedSeriesListItemChanged(int row)
	{
		UpdatePreviewByCurSeries();

		DicomInfomationModelSeriesObject* pSeries = GetCurrentSelectedSeries();
		SelectTreeItemBySeries(pSeries);

		emit currentSeriesChanged(pSeries);
	}

	void DicomPreviewWidget::onSupportedSeriesListItemDoubleClicked(QListWidgetItem* item)
	{
		DicomInfomationModelSeriesObject* pSeries = GetCurrentSelectedSeries();
		selectSeries(pSeries);
	}

	void DicomPreviewWidget::onDcmFileLoadedStarted()
	{
		Clear();
	}

	void DicomPreviewWidget::onDcmFileLoaded_InProgress(int progressCount, int maxCount)
	{
		UpdateSeriesModelList();
		UpdateImageAndDicomTag(false);
	}

	void DicomPreviewWidget::onDcmFileLoadedFinished()
	{
		UpdateSeriesModelList();
		UpdateImageAndDicomTag(true);
	}
}
