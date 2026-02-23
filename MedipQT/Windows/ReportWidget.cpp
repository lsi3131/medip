#include "stdafx.h"
#include "ReportWidget.h"

#include "Windows\windowManager.h"
#include "System\stringManager.h"
#include "System\styleManager.h"
#include "System\resourceManager.h"
#include "UI\ReportTextEdit.h"
#include "MedipQT.h"
#include "Actions/ActionManager.h"
#include "System/ProductManager.h"

#include "Tabwindow.h"
#include "System/LicenseManager.h"
#include "System/FileManager.h"

TempReportData::TempReportData() : m_fileName(), m_html()
{
	m_image_list.reserve(20);
}

TempReportData::~TempReportData()
{

}

bool TempReportData::isEmpty()
{
	return m_fileName.isEmpty() && m_image_list.empty() && m_html.isEmpty();
}

void TempReportData::clear()
{
	m_fileName.clear();
	m_image_list.clear();
	m_html.clear();
	htmlSize = 0;
}

void TempReportData::setFileName(const QString& fileName)
{
	m_fileName = fileName;
}

void TempReportData::setImageList(const std::vector<std::pair<QImage, QString>>& image_list)
{
	for (auto iter : image_list)
	{
		iter.first = iter.first.convertToFormat(QImage::Format_ARGB32);
		m_image_list.push_back(iter);
	}
}

void TempReportData::setHTML(const QString& html)
{
	m_html = html;
	htmlSize = m_html.toLocal8Bit().size() + 1;
}

int TempReportData::getHTMLSize()
{
	return htmlSize;
}

//====================================================================================================

ReportWidget::ReportWidget(QWidget* parent)
	: MainTabWidget(parent), m_textEdit(nullptr), m_btnHeaderList(nullptr), m_btnSequenceList(nullptr)
{
	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setMargin(0);
	layout->setSpacing(0);
	setLayout(layout);

	m_textEdit = new ReportTextEdit(this);

	QToolBar* toolBar = createToolbar();
	layout->addWidget(toolBar, 0);

	QHBoxLayout* hLay = new QHBoxLayout;
#ifndef DEEPCATCH_REPORT_150PPI_INTEGRATE
	QWidget* editingPlace = createEditingPlace();
	hLay->addWidget(editingPlace, 1);

	QScrollArea* scrollArea = createScrollArea();
	hLay->addWidget(scrollArea, 0);
#else
	QWidget* editingPlace = new QWidget(this);
	editingPlace->setStyleSheet(STYLE_MANAGER->reportEditingPlace);
	QVBoxLayout* textEditLayout = new QVBoxLayout;
	textEditLayout->setSizeConstraint(QLayout::SetFixedSize);
	textEditLayout->setAlignment(Qt::AlignCenter);
	textEditLayout->addWidget(m_textEdit, 0, Qt::AlignCenter);
	editingPlace->setLayout(textEditLayout);

	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setStyleSheet(STYLE_MANAGER->reportScrollArea);
	scrollArea->setAlignment(Qt::AlignCenter);
	scrollArea->setWidget(editingPlace);
	hLay->addWidget(scrollArea, 1);
#endif

	layout->addItem(hLay);

	m_temp_dir_list.reserve(10);
}

ReportWidget::~ReportWidget()
{
	for (auto iter : m_temp_dir_list)
	{
		QDir(iter).removeRecursively();
	}
}

void ReportWidget::activate()
{
	WIN_MANAGER->mainWindow->setDockWindowTitle(STRING_MANAGER->getString(STR_REPORT));
	WIN_MANAGER->mainHLayout->addWidget(this);

	if (!WIN_MANAGER->tempReportData.isEmpty())
	{
		loadFromMip(WIN_MANAGER->tempReportData.getFileName(),
			WIN_MANAGER->tempReportData.getImageList(), WIN_MANAGER->tempReportData.getHTML());
		WIN_MANAGER->tempReportData.clear();
	}

	// report tab 이동시 deepcatch format이 선택된 경우는 option tab expand.
	if (!LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH) || !LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH_V2))
	{
		OptionalTab* optionTab = WIN_MANAGER->GetTab()->getOptionalTab();
		optionTab->setOpenWidget(true);
	}
	else
	{
		OptionalTab* optionTab = WIN_MANAGER->GetTab()->getOptionalTab();
		optionTab->setOpenWidget(false);
	}

	show();
}

void ReportWidget::deactivate()
{
#ifdef DEEPCATCH_REPORT_150PPI_INTEGRATE
	m_textEdit->initZoom();
#endif

	hide();
	WIN_MANAGER->mainHLayout->removeWidget(this);
}

void ReportWidget::mousePressEvent(QMouseEvent* ev)
{
	buttonRelease();

	QWidget::mousePressEvent(ev);
}

bool ReportWidget::eventFilter(QObject* target, QEvent* e)
{
	static QIcon newFormatPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_NEW_FORMAT);
	static QIcon newFormatLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_NEW_FORMAT);
	static QIcon newFormatHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_NEW_FORMAT_HOVER);

	static QIcon savePress = RESOURCE_MANAGER->getIcon(ICON_REPORT_SAVE_FORMAT);
	static QIcon saveLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_SAVE_FORMAT);
	static QIcon saveHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_SAVE_FORMAT_HOVER);

	static QIcon saveAsPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_SAVE_AS_FORMAT);
	static QIcon saveAsLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_SAVE_AS_FORMAT);
	static QIcon saveAsHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_SAVE_AS_FORMAT_HOVER);

	static QIcon delPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_DELETE_FORMAT);
	static QIcon delLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_DELETE_FORMAT);
	static QIcon delHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_DELETE_FORMAT_HOVER);

	static QIcon textColorPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_COLOR);
	static QIcon textColorLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_COLOR);
	static QIcon textColorHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_COLOR_HOVER);

	static QIcon textBoldPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_BOLD);
	static QIcon textBoldLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_BOLD);
	static QIcon textBoldHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_BOLD_HOVER);

	static QIcon textUnderLinePress = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_UNDER_LINE);
	static QIcon textUnderLineLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_UNDER_LINE);
	static QIcon textUnderLineHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_UNDER_LINE_HOVER);

	static QIcon textItalicPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_ITALIC);
	static QIcon textItalicLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_ITALIC);
	static QIcon textItalicHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_ITALIC_HOVER);

	static QIcon textLeftAlignPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_LEFT_ALIGN);
	static QIcon textLeftAlignLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_LEFT_ALIGN);
	static QIcon textLeftAlignHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_LEFT_ALIGN_HOVER);

	static QIcon textCenterAlignPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_CENTER_ALIGN);
	static QIcon textCenterAlignLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_CENTER_ALIGN);
	static QIcon textCenterAlignHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_CENTER_ALIGN_HOVER);

	static QIcon textRightAlignPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_RIGHT_ALIGN);
	static QIcon textRightAlignLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_RIGHT_ALIGN);
	static QIcon textRightAlignHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_RIGHT_ALIGN_HOVER);

	static QIcon textJustifyAlignPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_JUSTIFY_ALIGN);
	static QIcon textJustifyAlignLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_JUSTIFY_ALIGN);
	static QIcon textJustifyAlignHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_JUSTIFY_ALIGN_HOVER);

	static QIcon headerListPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_HEADER_LIST);
	static QIcon headerListLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_HEADER_LIST);
	static QIcon headerListHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_HEADER_LIST_HOVER);

	static QIcon sequenceListPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_SEQUENCE_LIST);
	static QIcon sequenceListLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_SEQUENCE_LIST);
	static QIcon sequenceListHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_SEQUENCE_LIST_HOVER);

	static QIcon insertTablePress = RESOURCE_MANAGER->getIcon(ICON_REPORT_CREATE_TABLE);
	static QIcon insertTableLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_CREATE_TABLE);
	static QIcon insertTableHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_CREATE_TABLE_HOVER);

	static QIcon mergeCellPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_MERGE_CELL);
	static QIcon mergeCellLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_MERGE_CELL);
	static QIcon mergeCellHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_MERGE_CELL_HOVER);

	static QIcon splitCellPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_SPLIT_CELL);
	static QIcon splitCellLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_SPLIT_CELL);
	static QIcon splitCellHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_SPLIT_CELL_HOVER);

	static QIcon insertRowPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_INSERT_ROW);
	static QIcon insertRowLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_INSERT_ROW);
	static QIcon insertRowHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_INSERT_ROW_HOVER);

	static QIcon insertColumnPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_INSERT_COLUMN);
	static QIcon insertColumnLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_INSERT_COLUMN);
	static QIcon insertColumnHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_INSERT_COLUMN_HOVER);

	static QIcon cellPropertyPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_CELL_PROPERTY);
	static QIcon cellPropertyLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_CELL_PROPERTY);
	static QIcon cellPropertyHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_CELL_PROPERTY_HOVER);

	static QIcon importPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_IMPORT);
	static QIcon importLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_IMPORT);
	static QIcon importHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_IMPORT_HOVER);

	static QIcon exportPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_EXPORT);
	static QIcon exportLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_EXPORT);
	static QIcon exportHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_EXPORT_HOVER);

	static QIcon printPreviewPress = RESOURCE_MANAGER->getIcon(ICON_REPORT_PRINT);
	static QIcon printPreviewLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_PRINT);
	static QIcon printPreviewHover = RESOURCE_MANAGER->getIcon(ICON_REPORT_PRINT_HOVER);

	if (target == NULL)
		return QWidget::eventFilter(target, e);

	if (!target->inherits("QPushButton"))
		return QWidget::eventFilter(target, e);

	if (target == m_btnNewFormat)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnNewFormat->setIcon(newFormatHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnNewFormat->setIcon(newFormatPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnNewFormat->setIcon(newFormatLeave);
	}
	else if (target == m_btnSave)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnSave->setIcon(saveHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnSave->setIcon(savePress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnSave->setIcon(saveLeave);
		buttonRelease();
	}
	else if (target == m_btnSaveAs)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnSaveAs->setIcon(saveAsHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnSaveAs->setIcon(saveAsPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnSaveAs->setIcon(saveAsLeave);
	}
	else if (target == m_btnDel)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnDel->setIcon(delHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnDel->setIcon(delPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnDel->setIcon(delLeave);
	}
	else if (target == m_btnTextColor)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnTextColor->setIcon(textColorHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnTextColor->setIcon(textColorPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnTextColor->setIcon(textColorLeave);
	}
	else if (target == m_btnTextBold)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnTextBold->setIcon(textBoldHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnTextBold->setIcon(textBoldPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnTextBold->setIcon(textBoldLeave);
	}
	else if (target == m_btnTextUnderline)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnTextUnderline->setIcon(textUnderLineHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnTextUnderline->setIcon(textUnderLinePress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnTextUnderline->setIcon(textUnderLineLeave);
	}
	else if (target == m_btnTextItalic)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnTextItalic->setIcon(textItalicHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnTextItalic->setIcon(textItalicPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnTextItalic->setIcon(textItalicLeave);
	}
	else if (target == m_btnTextLeftAlign)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnTextLeftAlign->setIcon(textLeftAlignHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnTextLeftAlign->setIcon(textLeftAlignPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnTextLeftAlign->setIcon(textLeftAlignLeave);
	}
	else if (target == m_btnTextCenterAlign)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnTextCenterAlign->setIcon(textCenterAlignHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnTextCenterAlign->setIcon(textCenterAlignPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnTextCenterAlign->setIcon(textCenterAlignLeave);
	}
	else if (target == m_btnTextRightAlign)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnTextRightAlign->setIcon(textRightAlignHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnTextRightAlign->setIcon(textRightAlignPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnTextRightAlign->setIcon(textRightAlignLeave);
	}
	else if (target == m_btnTextJustifyAlign)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnTextJustifyAlign->setIcon(textJustifyAlignHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnTextJustifyAlign->setIcon(textJustifyAlignPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnTextJustifyAlign->setIcon(textJustifyAlignLeave);
	}
	else if (target == m_btnHeaderList)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnHeaderList->setIcon(headerListHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnHeaderList->setIcon(headerListPress);
			m_btnSequenceList->setIcon(sequenceListLeave);
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnHeaderList->setIcon(headerListLeave);
	}
	else if (target == m_btnSequenceList)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnSequenceList->setIcon(sequenceListHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnSequenceList->setIcon(sequenceListPress);
			m_btnHeaderList->setIcon(headerListLeave);
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnSequenceList->setIcon(sequenceListLeave);
	}
	else if (target == m_btnInsertTable)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnInsertTable->setIcon(insertTableHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnInsertTable->setIcon(insertTablePress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnInsertTable->setIcon(insertTableLeave);
	}
	else if (target == m_btnMergeCell)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnMergeCell->setIcon(mergeCellHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnMergeCell->setIcon(mergeCellPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnMergeCell->setIcon(mergeCellLeave);
	}
	else if (target == m_btnSplitCell)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnSplitCell->setIcon(splitCellHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnSplitCell->setIcon(splitCellPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnSplitCell->setIcon(splitCellLeave);
	}
	else if (target == m_btnInsertRow)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnInsertRow->setIcon(insertRowHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnInsertRow->setIcon(insertRowPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnInsertRow->setIcon(insertRowLeave);
	}
	else if (target == m_btnInsertColumn)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnInsertColumn->setIcon(insertColumnHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnInsertColumn->setIcon(insertColumnPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnInsertColumn->setIcon(insertColumnLeave);
	}
	else if (target == m_btnCellProperty)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnCellProperty->setIcon(cellPropertyHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnCellProperty->setIcon(cellPropertyPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnCellProperty->setIcon(cellPropertyLeave);
	}
	else if (target == m_btnFileImport)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnFileImport->setIcon(importHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnFileImport->setIcon(importPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnFileImport->setIcon(importLeave);
	}
	else if (target == m_btnFileExport)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnFileExport->setIcon(exportHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnFileExport->setIcon(exportPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnFileExport->setIcon(exportLeave);
	}
	else if (target == m_btnPrintPreview)
	{
		if (e->type() == QEvent::MouseMove)
			m_btnPrintPreview->setIcon(printPreviewHover);
		else if (e->type() == QEvent::MouseButtonPress)
		{
			m_btnPrintPreview->setIcon(printPreviewPress);
			buttonRelease();
		}
		else if (e->type() == QEvent::HoverLeave ||
			e->type() == QEvent::MouseButtonRelease)
			m_btnPrintPreview->setIcon(printPreviewLeave);
	}

	return QWidget::eventFilter(target, e);
}

void ReportWidget::importImage(const QImage& img)
{
	if (nullptr == m_textEdit) return;
	m_textEdit->importImage(img);
}

void ReportWidget::setPatientData()
{
	m_textEdit->setPatientData(QFileInfo(m_textEdit->getFileName()).baseName());
}

void ReportWidget::setTextureFeature()
{
	m_textEdit->setTextureFeatureData();
}

void ReportWidget::setCOVID19AnalysisData(void)
{
	m_textEdit->setCOVID19AnalysisData();
}

void ReportWidget::setDeepCatchAnalysisData(void)
{
	m_textEdit->setDeepCatchAnalysisData();
}

void ReportWidget::initFormatComboBox()
{
	if (nullptr == m_comboReportFormat) return;

	m_comboReportFormat->clear();

	QFileInfoList fileList;

#ifndef DEV_USE_APPDATA_PATH
	m_textEdit->openReportFormatDir(STRING_MANAGER->report_format, fileList);
#else
	QStringList paths;
	paths << STRING_MANAGER->report_format << STRING_MANAGER->localReport_format;
	m_textEdit->openReportFormatDir(paths, fileList);
#endif

	std::vector<QString> defaultFormat_list = m_textEdit->getDefaultFormats();
	for (auto iter : defaultFormat_list)
	{
		m_comboReportFormat->addItem(iter);
		setItemData_ComboReportFormat(STRING_MANAGER->report_format);
	}

	for (auto iter = fileList.begin(); iter != fileList.end(); ++iter)
	{
		int result = m_comboReportFormat->findText(iter->baseName());
		if (result >= 0) continue;
		m_comboReportFormat->addItem(iter->baseName());
		setItemData_ComboReportFormat(iter->path());
	}
}

void ReportWidget::addFormatAndShow()
{
	if (nullptr == m_comboReportFormat)
		return;
	QFileInfo fileInfo(m_textEdit->getFileName());
	QString strBaseName = fileInfo.baseName();
	int result = m_comboReportFormat->findText(strBaseName);
	if (result >= 0)
		return;
	m_comboReportFormat->addItem(strBaseName);
	setItemData_ComboReportFormat(STRING_MANAGER->report_format);
	m_comboReportFormat->setCurrentIndex(m_comboReportFormat->count() - 1);
	//	emit m_comboReportFormat->currentIndexChanged(m_comboReportFormat->count() - 1);
}

void ReportWidget::setTempReportData(const QString& fileName)
{
	if (nullptr == m_textEdit) return;
	WIN_MANAGER->tempReportData.clear();
	WIN_MANAGER->tempReportData.setFileName(QFileInfo(fileName).baseName());

	std::vector<std::pair<QImage, QString>> image_list;
	m_textEdit->getImageResource(image_list);
	WIN_MANAGER->tempReportData.setImageList(image_list);

	QString html;
	std::vector<QString> resource_list;
	m_textEdit->getHTML(html);
	m_textEdit->imageResourceName(html, resource_list);

	for (auto resource_name : resource_list)
	{
		QString newPath = STRING_MANAGER->report_format + "/" + QFileInfo(m_textEdit->getFileName()).baseName() + "/" + resource_name;

		newPath = "file:///" + newPath;
		html.replace(resource_name, newPath);
	}
	WIN_MANAGER->tempReportData.setHTML(html);
}

void ReportWidget::loadFromMip(const QString& fileName, const std::vector<std::pair<QImage, QString>>& image_list, const QString& html)
{
	change_report_format();
	m_textEdit->loadFromMip(fileName, image_list, html);
	addFormatAndShow();
	m_temp_dir_list.push_back(STRING_MANAGER->report_format + "/" + QFileInfo(m_textEdit->getFileName()).baseName());
}

std::vector<int> ReportWidget::currentReportImageResourceSize()
{
	std::vector<int> imgSize_list;

	return imgSize_list;
}

int ReportWidget::currentReportHTMLSize()
{
	return m_textEdit->currentHTMLSize();
}

void ReportWidget::getImageResource(std::vector<std::pair<QImage, QString>>& image_list)
{
	m_textEdit->getImageResource(image_list);
}

void ReportWidget::pdfExport(QString fileName)
{
	m_textEdit->pdfExport(fileName);
}

void ReportWidget::Enable_SaveDeleteBtn(bool bEnable)
{
	if (m_btnSave)
		m_btnSave->setEnabled(bEnable);
	if (m_btnDel)
		m_btnDel->setEnabled(bEnable);
}

void ReportWidget::change_report_format(bool bAppDataLocal)
{
#ifdef DEV_USE_APPDATA_PATH
	if (bAppDataLocal)	// local report path로 변경.(편집인경우, new, save, save as, import 등)
		STRING_MANAGER->report_format = STRING_MANAGER->localReport_format;
	else	// combobox에 setitemdata로 등록된 경로를 report_format으로 변경 처리.
		STRING_MANAGER->report_format = m_comboReportFormat->itemData(m_comboReportFormat->currentIndex(), Qt::UserRole).toString();
#endif
}

void ReportWidget::setItemData_ComboReportFormat(QString strData)
{
#ifdef DEV_USE_APPDATA_PATH
	m_comboReportFormat->setItemData(m_comboReportFormat->count() - 1, strData, Qt::UserRole);
#endif
}

QTextDocument* ReportWidget::GetReportDocument()
{
	if (m_textEdit)
	{
		return m_textEdit->document();
	}
	return nullptr;
}

void ReportWidget::slot_OnReportFormatIndexChange(int index)
{
#ifdef DEV_USE_APPDATA_PATH
#if 0
	// 0번 인덱스가 선택된 경우 read only이 때문에 save, delete 버튼 비활성화 처리.
	if (!index)
		Enable_SaveDeleteBtn(false);
	else
		Enable_SaveDeleteBtn(true);
#else
	// programfile에서 로딩한 경우는 read only이 때문에 save, delete 버튼 비활성화 처리.
	QString strCurItemData = "";
	strCurItemData = m_comboReportFormat->itemData(index, Qt::UserRole).toString();
	if (!strCurItemData.isEmpty() && !strCurItemData.compare(STRING_MANAGER->localReport_format, Qt::CaseInsensitive))
		Enable_SaveDeleteBtn(true);
	else
		Enable_SaveDeleteBtn(false);
#endif
#endif
}

void ReportWidget::slot_OnReportFormatChange(const QString& dirName)
{
	if (nullptr == m_textEdit) return;
	int res = m_textEdit->checkModified();
	if (res == QMessageBox::Yes) {
		slot_OnFileSaveAs();
	}
	else if (res == QMessageBox::Save) {
		slot_OnFileSave();
	}

	change_report_format(false);

	bool result = m_textEdit->loadFromDirName(dirName);
	if (false == result)
	{
		QMessageBox::warning(nullptr, "Report Tab", tr("Could not open \"%1\"").arg(QDir::toNativeSeparators(dirName)));
		std::vector<QString> defaultFormat_list = m_textEdit->getDefaultFormats();
		for (auto iter : defaultFormat_list)
		{
			if (iter == dirName) return;
		}
		QDir(STRING_MANAGER->report_format + "/" + dirName).removeRecursively();
		int index = m_comboReportFormat->findText(dirName);
		m_comboReportFormat->removeItem(index);
		m_comboReportFormat->setCurrentIndex(0);
		//	emit m_comboReportFormat->currentIndexChanged(0);
	}
	//	int index = m_comboReportFormat->findText(dirName);
	//	m_comboReportFormat->setCurrentIndex(index);
	//	emit m_comboReportFormat->currentIndexChanged(index);

}

void ReportWidget::slot_OnNewFile()
{
	int res = m_textEdit->checkModified();
	if (res == QMessageBox::Yes) {
		slot_OnFileSaveAs();
	}
	else if (res == QMessageBox::Save) {
		slot_OnFileSave();
	}

	change_report_format();

	bool success = m_textEdit->newFormat();
	if (false == success) return;
	if (nullptr == m_comboReportFormat) return;

	QFileInfo fileInfo(m_textEdit->getFileName());
	m_comboReportFormat->addItem(fileInfo.baseName());
	setItemData_ComboReportFormat(STRING_MANAGER->report_format);
	m_comboReportFormat->setCurrentIndex(m_comboReportFormat->count() - 1);
	//	emit m_comboReportFormat->currentIndexChanged(m_comboReportFormat->count() - 1);
}

void ReportWidget::slot_OnFileSave()
{
	bool success = m_textEdit->fileSave();
	m_textEdit->releaseSelection();
	if (false == success)
	{
		return;
	}
	else
	{
		QMessageBox::information(nullptr, "Success", "Successfully save format");
		addFormatAndShow();
	}
}

void ReportWidget::slot_OnFileSaveAs()
{
	change_report_format();

	bool success = m_textEdit->fileSaveAs();
	m_textEdit->releaseSelection();
	if (false == success)
	{
		return;
	}
	else
	{
		QMessageBox::information(nullptr, "Success", "Successfully save format");
		addFormatAndShow();
	}
}

void ReportWidget::slot_OnDeleteFile()
{
	if (nullptr == m_comboReportFormat) return;
	bool success = m_textEdit->deleteFormat();
	m_textEdit->releaseSelection();
	if (false == success)
	{
		QMessageBox::warning(nullptr, "Failed", "Failed to delete format.");
		return;
	}
	else
	{
		QMessageBox::information(nullptr, "Success", "Successfully deleted format.");
		m_comboReportFormat->removeItem(m_comboReportFormat->currentIndex());
		m_comboReportFormat->setCurrentIndex(0);
		//	emit m_comboReportFormat->currentIndexChanged(0);
		slot_OnReportFormatChange(m_comboReportFormat->currentText());
	}
}

void ReportWidget::slot_OnFileImport()
{
	const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
	QString selectedFilter;
	QString strFilter = QString("All Files (*)");

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_ReportImport))
	{
		strFilter += QString(";;HTML (*.html)");
	}

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_ImageManagement_ImageImport))
	{
		strFilter += QString(";;PNG (*.png);;JPG (*.jpg);;BMP (*.bmp)");
	}

	QFileInfo fileInfo(QDir::homePath() + "/Desktop");
	QString path = ImportFileDialog(
		this,
		"Import File",
		fileInfo.fileName(),
		fileInfo.dir().absolutePath(),
		strFilter
	);

	//QString path = QFileDialog::getOpenFileName(this,
	//	"Import File", QDir::homePath() + "/Desktop", strFilter,
	//	&selectedFilter, options);

	if (path.isEmpty())
		return;

	bool res = false;
	if ("html" == QFileInfo(path).suffix())
	{
		res = m_textEdit->fileImport(path);
		if (true == res)
		{
			change_report_format();

			bool res = m_textEdit->fileSave();
			QFileInfo fileInfo(m_textEdit->getFileName());
			QString name = fileInfo.baseName();
			int index = m_comboReportFormat->findText(name);
			if (index >= 0)
			{
				m_comboReportFormat->setCurrentIndex(index);
				//	emit m_comboReportFormat->currentIndexChanged(index);
				m_textEdit->fileImport(path);
				return;
			}
			m_comboReportFormat->addItem(name);
			setItemData_ComboReportFormat(STRING_MANAGER->report_format);
			m_comboReportFormat->setCurrentIndex(m_comboReportFormat->count() - 1);
			//	emit m_comboReportFormat->currentIndexChanged(m_comboReportFormat->count() - 1);
		}
	}
	else
	{
		res = WIN_MANAGER->loadImage(path);
	}

	if (false == res)
	{
		QMessageBox::warning(nullptr, "Faild", "Failed to import file.");
	}

}

void ReportWidget::slot_OnFileExport()
{
	m_textEdit->fileExport();
}

void ReportWidget::slot_OnPrintReport()
{
	m_textEdit->filePrint();
}

void ReportWidget::slot_OnPrintPreview()
{
	m_textEdit->printPreview();
}

void ReportWidget::slot_OnTextBold()
{
	m_textEdit->textBold();
}

void ReportWidget::slot_OnTextUnderline()
{
	m_textEdit->textUnderline();
}

void ReportWidget::slot_OnTextItalic()
{
	m_textEdit->textItalic();
}

void ReportWidget::slot_OnTextFamily(const QString& fontFamily)
{
	m_textEdit->textFontFamily(fontFamily);
}

void ReportWidget::slot_OnTextSize(const QString& size)
{
	m_textEdit->textSize(size);
}

void ReportWidget::slot_OnTextStyle(int styleIndex)
{
	m_textEdit->textStyle(styleIndex);
}

void ReportWidget::slot_OnTextColor()
{
	QColor c = m_textEdit->textColorChange();
	QPixmap pix(m_BtnColorIconSize);
	pix.fill(c);

	if (nullptr == m_btnTextColor) return;
	m_btnTextColor->setIcon(pix);
}

void ReportWidget::slot_OnTextAlign(QAbstractButton* btn)
{
	m_textEdit->releaseSelection();

	if (btn->objectName().contains("Left"))
		m_textEdit->textLeftAlign();
	else if (btn->objectName().contains("Center"))
		m_textEdit->textCenterAlign();
	else if (btn->objectName().contains("Right"))
		m_textEdit->textRightAlign();
	else if (btn->objectName().contains("Justify"))
		m_textEdit->textJustifyAlign();
}

void ReportWidget::slot_OnInsertTable()
{
	m_textEdit->releaseSelection();
	m_textEdit->insertTable();
}

void ReportWidget::slot_OnMergeCell()
{
	m_textEdit->mergeCell();
}

void ReportWidget::slot_OnSplitCell()
{
	m_textEdit->splitCell();
}

void ReportWidget::slot_OnInsertRow()
{
	m_textEdit->insertRow();
}

void ReportWidget::slot_OnInsertCol()
{
	m_textEdit->insertCol();
}

void ReportWidget::slot_OnOpenCellProperty()
{
	m_textEdit->openCellProperty();
}

void ReportWidget::slot_OnDiscList()
{
	int index = QTextListFormat::ListDisc;
	m_textEdit->textStyle(std::abs(index));
	buttonRelease();
}

void ReportWidget::slot_OnCircleList()
{
	int index = QTextListFormat::ListCircle;
	m_textEdit->textStyle(std::abs(index));
	buttonRelease();
}

void ReportWidget::slot_OnSquareList()
{
	int index = QTextListFormat::ListSquare;
	m_textEdit->textStyle(std::abs(index));
	buttonRelease();
}

void ReportWidget::slot_OnDecimalList()
{
	int index = QTextListFormat::ListDecimal;
	m_textEdit->textStyle(std::abs(index));
	buttonRelease();
}

void ReportWidget::slot_OnLowerAlphaList()
{
	int index = QTextListFormat::ListLowerAlpha;
	m_textEdit->textStyle(std::abs(index));
	buttonRelease();
}

void ReportWidget::slot_OnUpperAlphaList()
{
	int index = QTextListFormat::ListUpperAlpha;
	m_textEdit->textStyle(std::abs(index));
	buttonRelease();
}

void ReportWidget::slot_OnLowerRomanList()
{
	int index = QTextListFormat::ListLowerRoman;
	m_textEdit->textStyle(std::abs(index));
	buttonRelease();
}

void ReportWidget::slot_OnUpperRomanList()
{
	int index = QTextListFormat::ListUpperRoman;
	m_textEdit->textStyle(std::abs(index));
	buttonRelease();
}

void ReportWidget::buttonRelease()
{
	static QIcon headerListLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_HEADER_LIST);
	static QIcon sequenceListLeave = RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_SEQUENCE_LIST);

	if (nullptr == m_btnHeaderList) return;
	if (nullptr == m_btnSequenceList) return;

	m_btnHeaderList->setIcon(headerListLeave);
	m_btnSequenceList->setIcon(sequenceListLeave);
}

QToolBar* ReportWidget::createToolbar()
{
	QToolBar* toolBar = new QToolBar(this);
	toolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	toolBar->setOrientation(Qt::Horizontal);
	toolBar->setStyleSheet(STYLE_MANAGER->reportToolbar);

	m_comboReportFormat = new QComboBox(toolBar);
	m_comboReportFormat->setMinimumWidth(100);
	m_comboReportFormat->setStyleSheet(STYLE_MANAGER->comboBoxTab_report);
	m_comboReportFormat->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	m_comboReportFormat->setEditable(false);
	initFormatComboBox();
	m_comboReportFormat->hide();

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_EditingFunction))
	{
		m_comboReportFormat->show();
		toolBar->addWidget(m_comboReportFormat);
		connect(m_comboReportFormat, QOverload<const QString&>::of(&QComboBox::activated), this, &ReportWidget::slot_OnReportFormatChange);
		connect(m_comboReportFormat, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ReportWidget::slot_OnReportFormatIndexChange);

		toolBar->addSeparator();

		m_btnNewFormat = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_NEW_FORMAT), "", toolBar);
		m_btnNewFormat->setFixedSize(m_BtnSize);
		m_btnNewFormat->setIconSize(m_BtnSize);
		m_btnNewFormat->setToolTip("create new format");
		m_btnNewFormat->setStyleSheet("color: black");
		m_btnNewFormat->installEventFilter(this);
		m_btnNewFormat->setMouseTracking(true);
		toolBar->addWidget(m_btnNewFormat);
		connect(m_btnNewFormat, &QPushButton::clicked, this, &ReportWidget::slot_OnNewFile);

		m_btnSave = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_SAVE_FORMAT), "", toolBar);
		m_btnSave->setFixedSize(m_BtnSize);
		m_btnSave->setIconSize(m_BtnSize);
		m_btnSave->setToolTip("save format(Ctrl + Shift + A)");
		m_btnSave->setStyleSheet("color: black;");
		m_btnSave->installEventFilter(this);
		m_btnSave->setMouseTracking(true);
		m_btnSave->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A));
		toolBar->addWidget(m_btnSave);
		connect(m_btnSave, &QPushButton::clicked, this, &ReportWidget::slot_OnFileSave);

		m_btnSaveAs = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_SAVE_AS_FORMAT), "", toolBar);
		m_btnSaveAs->setFixedSize(m_BtnSize);
		m_btnSaveAs->setIconSize(m_BtnSize);
		m_btnSaveAs->setToolTip("save as format(Ctrl + Shift + Z)");
		m_btnSaveAs->setStyleSheet("color: black;");
		m_btnSaveAs->installEventFilter(this);
		m_btnSaveAs->setMouseTracking(true);
		m_btnSaveAs->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));
		toolBar->addWidget(m_btnSaveAs);
		connect(m_btnSaveAs, &QPushButton::clicked, this, &ReportWidget::slot_OnFileSaveAs);

		m_btnDel = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_DELETE_FORMAT), "", toolBar);
		m_btnDel->setFixedSize(m_BtnSize);
		m_btnDel->setIconSize(m_BtnSize);
		m_btnDel->setToolTip("delete format");
		m_btnDel->setStyleSheet("color: black;");
		m_btnDel->installEventFilter(this);
		m_btnDel->setMouseTracking(true);
		toolBar->addWidget(m_btnDel);
		connect(m_btnDel, &QPushButton::clicked, this, &ReportWidget::slot_OnDeleteFile);

		toolBar->addSeparator();

		//QPixmap pix(BtnColorIconSize);
		//pix.fill(Qt::black);
		m_btnTextColor = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_COLOR), "", toolBar);
		//m_btnTextColor = new QPushButton(toolBar);
		//m_btnTextColor->setIcon(QIcon(pix));
		m_btnTextColor->setFixedSize(m_BtnSize);
		m_btnTextColor->setIconSize(m_BtnSize);
		m_btnTextColor->setToolTip("text color");
		m_btnTextColor->setStyleSheet("color: black;");
		m_btnTextColor->installEventFilter(this);
		m_btnTextColor->setMouseTracking(true);
		//m_btnTextColor->setStyleSheet("QToolTip{color: black;}");
		toolBar->addWidget(m_btnTextColor);
		connect(m_btnTextColor, &QPushButton::clicked, this, &ReportWidget::slot_OnTextColor);

		toolBar->addSeparator();

		QFontDatabase base;
		QStringList list = base.families();
		QFontComboBox* comboFontStyle = new QFontComboBox(toolBar);
		comboFontStyle->setMaximumWidth(100);
		comboFontStyle->setStyleSheet(STYLE_MANAGER->comboBoxTab_report);
		comboFontStyle->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
		comboFontStyle->setFontFilters(QFontComboBox::FontFilter::ScalableFonts);
		toolBar->addWidget(comboFontStyle);
		connect(comboFontStyle, QOverload<const QString&>::of(&QComboBox::activated), this, &ReportWidget::slot_OnTextFamily);

		toolBar->addSeparator();

		QComboBox* comboFontSize = new QComboBox(toolBar);
		comboFontSize->setMinimumWidth(50);
		comboFontSize->setStyleSheet(STYLE_MANAGER->comboBoxTab_report);
		comboFontSize->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
		toolBar->addWidget(comboFontSize);
		comboFontSize->setEditable(true);
		const QList<int> standardSizes = QFontDatabase::standardSizes();
		foreach(int size, standardSizes)
			comboFontSize->addItem(QString::number(size));
		comboFontSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));
		connect(comboFontSize, QOverload<const QString&>::of(&QComboBox::activated), this, &ReportWidget::slot_OnTextSize);

		toolBar->addSeparator();

		m_btnTextBold = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_BOLD), "", toolBar);
		m_btnTextBold->setFixedSize(m_BtnSize);
		m_btnTextBold->setIconSize(m_BtnSize);
		m_btnTextBold->setToolTip("text bold(Ctrl + B)");
		m_btnTextBold->setStyleSheet("color: black;");
		m_btnTextBold->installEventFilter(this);
		m_btnTextBold->setMouseTracking(true);
		m_btnTextBold->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
		toolBar->addWidget(m_btnTextBold);
		connect(m_btnTextBold, &QPushButton::clicked, this, &ReportWidget::slot_OnTextBold);

		m_btnTextUnderline = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_UNDER_LINE), "", toolBar);
		m_btnTextUnderline->setFixedSize(m_BtnSize);
		m_btnTextUnderline->setIconSize(m_BtnSize);
		m_btnTextUnderline->setToolTip("text underline(Ctrl + U)");
		m_btnTextUnderline->setStyleSheet("color: black;");
		m_btnTextUnderline->installEventFilter(this);
		m_btnTextUnderline->setMouseTracking(true);
		m_btnTextUnderline->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
		toolBar->addWidget(m_btnTextUnderline);
		connect(m_btnTextUnderline, &QPushButton::clicked, this, &ReportWidget::slot_OnTextUnderline);

		m_btnTextItalic = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_ITALIC), "", toolBar);
		m_btnTextItalic->setFixedSize(m_BtnSize);
		m_btnTextItalic->setIconSize(m_BtnSize);
		m_btnTextItalic->setToolTip("text italic(Ctrl + i)");
		m_btnTextItalic->setStyleSheet("color: black;");
		m_btnTextItalic->setObjectName("textItalic");
		m_btnTextItalic->installEventFilter(this);
		m_btnTextItalic->setMouseTracking(true);
		m_btnTextItalic->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
		toolBar->addWidget(m_btnTextItalic);
		connect(m_btnTextItalic, &QPushButton::clicked, this, &ReportWidget::slot_OnTextItalic);

		toolBar->addSeparator();

		QButtonGroup* alignBtns = new QButtonGroup(toolBar);
		connect(alignBtns, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonReleased), this, &ReportWidget::slot_OnTextAlign);

		m_btnTextLeftAlign = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_LEFT_ALIGN), "", toolBar);
		m_btnTextLeftAlign->setFixedSize(m_BtnSize);
		m_btnTextLeftAlign->setIconSize(m_BtnSize);
		m_btnTextLeftAlign->setToolTip("text left alignment(Ctrl + F)");
		m_btnTextLeftAlign->setStyleSheet("color: black;");
		m_btnTextLeftAlign->setObjectName("Left");
		m_btnTextLeftAlign->installEventFilter(this);
		m_btnTextLeftAlign->setMouseTracking(true);
		m_btnTextLeftAlign->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
		alignBtns->addButton(m_btnTextLeftAlign);
		toolBar->addWidget(m_btnTextLeftAlign);

		m_btnTextCenterAlign = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_CENTER_ALIGN), "", toolBar);
		m_btnTextCenterAlign->setFixedSize(m_BtnSize);
		m_btnTextCenterAlign->setIconSize(m_BtnSize);
		m_btnTextCenterAlign->setToolTip("text center alignment(Ctrl + T)");
		m_btnTextCenterAlign->setStyleSheet("color: black;");
		m_btnTextCenterAlign->setObjectName("Center");
		m_btnTextCenterAlign->installEventFilter(this);
		m_btnTextCenterAlign->setMouseTracking(true);
		m_btnTextCenterAlign->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
		alignBtns->addButton(m_btnTextCenterAlign);
		toolBar->addWidget(m_btnTextCenterAlign);

		m_btnTextRightAlign = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_RIGHT_ALIGN), "", toolBar);
		m_btnTextRightAlign->setFixedSize(m_BtnSize);
		m_btnTextRightAlign->setIconSize(m_BtnSize);
		m_btnTextRightAlign->setToolTip("text right alignment(Ctrl + R)");
		m_btnTextRightAlign->setStyleSheet("color: black;");
		m_btnTextRightAlign->setObjectName("Right");
		m_btnTextRightAlign->installEventFilter(this);
		m_btnTextRightAlign->setMouseTracking(true);
		m_btnTextRightAlign->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
		alignBtns->addButton(m_btnTextRightAlign);
		toolBar->addWidget(m_btnTextRightAlign);

		m_btnTextJustifyAlign = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_JUSTIFY_ALIGN), "", toolBar);
		m_btnTextJustifyAlign->setFixedSize(m_BtnSize);
		m_btnTextJustifyAlign->setIconSize(m_BtnSize);
		m_btnTextJustifyAlign->setToolTip("text justify alignment(Ctrl + P)");
		m_btnTextJustifyAlign->setStyleSheet("color: black;");
		m_btnTextJustifyAlign->setObjectName("Justify");
		m_btnTextJustifyAlign->installEventFilter(this);
		m_btnTextJustifyAlign->setMouseTracking(true);
		m_btnTextJustifyAlign->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
		alignBtns->addButton(m_btnTextJustifyAlign);
		toolBar->addWidget(m_btnTextJustifyAlign);

		toolBar->addSeparator();

		QMenu* headerListMenu = new QMenu(toolBar);

		QAction* actDiscList = new QAction(RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_HEADER_DISC), "Disc", this);
		QAction* actCircleList = new QAction(RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_HEADER_CIRCLE), "Circle", this);
		QAction* actSquareList = new QAction(RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_HEADER_SQUARE), "Square", this);

		connect(actDiscList, &QAction::triggered, this, &ReportWidget::slot_OnDiscList);
		connect(actCircleList, &QAction::triggered, this, &ReportWidget::slot_OnCircleList);
		connect(actSquareList, &QAction::triggered, this, &ReportWidget::slot_OnSquareList);

		headerListMenu->addAction(actDiscList);
		headerListMenu->addAction(actCircleList);
		headerListMenu->addAction(actSquareList);

		m_btnHeaderList = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_HEADER_LIST), "", toolBar);
		m_btnHeaderList->setFixedSize(m_BtnSize);
		m_btnHeaderList->setIconSize(m_BtnSize);
		m_btnHeaderList->setToolTip("headers list");
		m_btnHeaderList->setStyleSheet("color: black;");
		m_btnHeaderList->installEventFilter(this);
		m_btnHeaderList->setMouseTracking(true);
		m_btnHeaderList->setMenu(headerListMenu);
		toolBar->addWidget(m_btnHeaderList);

		QMenu* sequenceListMenu = new QMenu(toolBar);

		QAction* actDecimalList = new QAction(RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_SEQUENCE_DECIMAL), "Decimal", this);
		QAction* actLowerAlphaList = new QAction(RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_SEQUENCE_LOWER_ALPHA), "Lower Alpha", this);
		QAction* actUpperAlphaList = new QAction(RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_SEQUENCE_UPPER_ALPHA), "Upper Alpha", this);
		QAction* actLowerRomanList = new QAction(RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_SEQUENCE_LOWER_ROMAN), "Lower Roman", this);
		QAction* actUpperRomanList = new QAction(RESOURCE_MANAGER->getIcon(ICON_REPORT_TEXT_SEQUENCE_UPPER_ROMAN), "Upper Roman", this);

		connect(actDecimalList, &QAction::triggered, this, &ReportWidget::slot_OnDecimalList);
		connect(actLowerAlphaList, &QAction::triggered, this, &ReportWidget::slot_OnLowerAlphaList);
		connect(actUpperAlphaList, &QAction::triggered, this, &ReportWidget::slot_OnUpperAlphaList);
		connect(actLowerRomanList, &QAction::triggered, this, &ReportWidget::slot_OnLowerRomanList);
		connect(actUpperRomanList, &QAction::triggered, this, &ReportWidget::slot_OnUpperRomanList);

		sequenceListMenu->addAction(actDecimalList);
		sequenceListMenu->addAction(actLowerAlphaList);
		sequenceListMenu->addAction(actUpperAlphaList);
		sequenceListMenu->addAction(actLowerRomanList);
		sequenceListMenu->addAction(actUpperRomanList);

		m_btnSequenceList = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_TEXT_SEQUENCE_LIST), "", toolBar);
		m_btnSequenceList->setFixedSize(m_BtnSize);
		m_btnSequenceList->setIconSize(m_BtnSize);
		m_btnSequenceList->setToolTip("sequence list");
		m_btnSequenceList->setStyleSheet("color: black;");
		m_btnSequenceList->installEventFilter(this);
		m_btnSequenceList->setMouseTracking(true);
		m_btnSequenceList->setMenu(sequenceListMenu);
		toolBar->addWidget(m_btnSequenceList);

		toolBar->addSeparator();

		m_btnInsertTable = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_CREATE_TABLE), "", toolBar);
		m_btnInsertTable->setFixedSize(m_BtnSize);
		m_btnInsertTable->setIconSize(m_BtnSize);
		m_btnInsertTable->setToolTip("create table");
		m_btnInsertTable->setStyleSheet("color: black;");
		m_btnInsertTable->installEventFilter(this);
		m_btnInsertTable->setMouseTracking(true);
		toolBar->addWidget(m_btnInsertTable);
		connect(m_btnInsertTable, &QPushButton::clicked, this, &ReportWidget::slot_OnInsertTable);

		m_btnMergeCell = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_MERGE_CELL), "", toolBar);
		m_btnMergeCell->setFixedSize(m_BtnSize);
		m_btnMergeCell->setIconSize(m_BtnSize);
		m_btnMergeCell->setToolTip("merge cell");
		m_btnMergeCell->setStyleSheet("color: black;");
		m_btnMergeCell->installEventFilter(this);
		m_btnMergeCell->setMouseTracking(true);
		toolBar->addWidget(m_btnMergeCell);
		connect(m_btnMergeCell, &QPushButton::clicked, this, &ReportWidget::slot_OnMergeCell);

		m_btnSplitCell = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_SPLIT_CELL), "", toolBar);
		m_btnSplitCell->setFixedSize(m_BtnSize);
		m_btnSplitCell->setIconSize(m_BtnSize);
		m_btnSplitCell->setToolTip("split cell");
		m_btnSplitCell->setStyleSheet("color: black;");
		m_btnSplitCell->installEventFilter(this);
		m_btnSplitCell->setMouseTracking(true);
		toolBar->addWidget(m_btnSplitCell);
		connect(m_btnSplitCell, &QPushButton::clicked, this, &ReportWidget::slot_OnSplitCell);

		m_btnInsertRow = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_INSERT_ROW), "", toolBar);
		m_btnInsertRow->setFixedSize(m_BtnSize);
		m_btnInsertRow->setIconSize(m_BtnSize);
		m_btnInsertRow->setToolTip("insert row to table(Ctrl + N)");
		m_btnInsertRow->setStyleSheet("color: black;");
		m_btnInsertRow->installEventFilter(this);
		m_btnInsertRow->setMouseTracking(true);
		m_btnInsertRow->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
		toolBar->addWidget(m_btnInsertRow);
		connect(m_btnInsertRow, &QPushButton::clicked, this, &ReportWidget::slot_OnInsertRow);

		m_btnInsertColumn = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_INSERT_COLUMN), "", toolBar);
		m_btnInsertColumn->setFixedSize(m_BtnSize);
		m_btnInsertColumn->setIconSize(m_BtnSize);
		m_btnInsertColumn->setToolTip("insert column to table(Ctrl + K)");
		m_btnInsertColumn->setStyleSheet("color: black;");
		m_btnInsertColumn->installEventFilter(this);
		m_btnInsertColumn->setMouseTracking(true);
		m_btnInsertColumn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_K));
		toolBar->addWidget(m_btnInsertColumn);
		connect(m_btnInsertColumn, &QPushButton::clicked, this, &ReportWidget::slot_OnInsertCol);

		m_btnCellProperty = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_CELL_PROPERTY), "", toolBar);
		m_btnCellProperty->setFixedSize(m_BtnSize);
		m_btnCellProperty->setIconSize(m_BtnSize);
		m_btnCellProperty->setToolTip("Cell Property");
		m_btnCellProperty->setStyleSheet("color: black;");
		m_btnCellProperty->installEventFilter(this);
		m_btnCellProperty->setMouseTracking(true);
		toolBar->addWidget(m_btnCellProperty);
		connect(m_btnCellProperty, &QPushButton::clicked, this, &ReportWidget::slot_OnOpenCellProperty);

	}
	else // editing 권한이 없는 경우 readonly
		m_textEdit->setReadOnly(true);

	toolBar->addSeparator();

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_ReportImport))
	{
		m_btnFileImport = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_IMPORT), "", toolBar);
		m_btnFileImport->setFixedSize(m_BtnSize);
		m_btnFileImport->setIconSize(m_BtnSize);
		m_btnFileImport->setToolTip("file import(Ctrl + Shift + U)");
		m_btnFileImport->setStyleSheet("color: black;");
		m_btnFileImport->installEventFilter(this);
		m_btnFileImport->setMouseTracking(true);
		m_btnFileImport->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_U));
		toolBar->addWidget(m_btnFileImport);
		connect(m_btnFileImport, &QPushButton::clicked, this, &ReportWidget::slot_OnFileImport);
	}

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_ReportExport))
	{
		m_btnFileExport = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_EXPORT), "", toolBar);
		m_btnFileExport->setFixedSize(m_BtnSize);
		m_btnFileExport->setIconSize(m_BtnSize);
		m_btnFileExport->setToolTip("file export(Ctrl + Shift + E)");
		m_btnFileExport->setStyleSheet("color: black;");
		m_btnFileExport->installEventFilter(this);
		m_btnFileExport->setMouseTracking(true);
		m_btnFileExport->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_E));
		toolBar->addWidget(m_btnFileExport);
		connect(m_btnFileExport, &QPushButton::clicked, this, &ReportWidget::slot_OnFileExport);
	}

	//QPushButton* print = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_FILE_UPLOAD), "");
	//print->setToolTip("print file");
	//print->setStyleSheet("color: black;");
	//toolBar->addWidget(print);
	//connect(print, &QPushButton::clicked, this, &ReportWidget::OnPrintReport);

	m_btnPrintPreview = new QPushButton(RESOURCE_MANAGER->getIcon(ICON_NON_REPORT_PRINT), "", toolBar);
	m_btnPrintPreview->setFixedSize(m_BtnSize);
	m_btnPrintPreview->setIconSize(m_BtnSize);
	m_btnPrintPreview->setToolTip("show print preview(Ctrl + Shift + L)");
	m_btnPrintPreview->setStyleSheet("color: black;");
	m_btnPrintPreview->installEventFilter(this);
	m_btnPrintPreview->setMouseTracking(true);
	m_btnPrintPreview->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_L));
	toolBar->addWidget(m_btnPrintPreview);
	connect(m_btnPrintPreview, &QPushButton::clicked, this, &ReportWidget::slot_OnPrintPreview);

	slot_OnReportFormatIndexChange(0);

	return toolBar;
}

QWidget* ReportWidget::createEditingPlace()
{
	QWidget* editingPlace = new QWidget(this);
	editingPlace->setStyleSheet(STYLE_MANAGER->reportEditingPlace);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setAlignment(Qt::AlignCenter);
	layout->addWidget(m_textEdit);
	editingPlace->setLayout(layout);

	return editingPlace;
}

QScrollArea* ReportWidget::createScrollArea()
{
	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setFixedWidth(m_textEdit->verticalScrollBar()->sizeHint().width());
	scrollArea->setVerticalScrollBar(m_textEdit->verticalScrollBar());
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	scrollArea->setStyleSheet(STYLE_MANAGER->reportScrollArea);

	return scrollArea;
}