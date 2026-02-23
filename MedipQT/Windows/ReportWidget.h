#pragma once

#ifndef REPORTWIDGET_H
#define REPORTWIDGET_H

#include "Windows/Main/MainTabWidget.h"
#include <qscrollarea.h>

class ReportTextEdit;
class QAbstractButton;
class QToolBar;
class QPushButton;
class QComboBox;

class TempReportData
{
private:
	QString m_fileName;
	std::vector<std::pair<QImage, QString>> m_image_list;
	QString m_html;
	int htmlSize;

private:
	TempReportData(const TempReportData& copy) {};
	const TempReportData& operator=(const TempReportData& rhs) {};

public:
	explicit TempReportData();
	~TempReportData();

	bool isEmpty();
	void clear();
	
	void setFileName(const QString& fileName);
	void setImageList(const std::vector<std::pair<QImage, QString>>& image_list);
	void setHTML(const QString& html);

	int getHTMLSize();
	const QString& getFileName() { return m_fileName; }
	const std::vector<std::pair<QImage, QString>>& getImageList() { return m_image_list; }
	const QString& getHTML() { return m_html; }
};

class ReportWidget : public MainTabWidget
{
	Q_OBJECT
public:
	explicit ReportWidget(QWidget* parent = nullptr);
	~ReportWidget();

	void activate() override;
	void deactivate() override;
	void mousePressEvent(QMouseEvent * ev) override;
	bool eventFilter(QObject * target, QEvent * e) override;

	void importImage(const QImage& img);
	void setPatientData();
	void setTextureFeature();
	void setCOVID19AnalysisData(void);
	void setDeepCatchAnalysisData(void);
	void initFormatComboBox();
	void addFormatAndShow();
	void setTempReportData(const QString& fileName);

	void loadFromMip(const QString& fileName, const std::vector<std::pair<QImage, QString>>& image_list, const QString& html);
	std::vector<int> currentReportImageResourceSize();
	int currentReportHTMLSize();
	void getImageResource(std::vector<std::pair<QImage, QString>>& image_list);

	void pdfExport(QString fileName);

	void Enable_SaveDeleteBtn(bool bEnable = true);
	void change_report_format(bool bAppDataLocal = true);
	void setItemData_ComboReportFormat(QString strData);

	QTextDocument* GetReportDocument();

public slots:
	void slot_OnReportFormatChange(const QString& dirName);
	void slot_OnReportFormatIndexChange(int index);
	void slot_OnNewFile();
	void slot_OnFileSave();
	void slot_OnFileSaveAs();
	void slot_OnDeleteFile();

	void slot_OnFileImport();
	void slot_OnFileExport();
	void slot_OnPrintReport();
	void slot_OnPrintPreview();

	void slot_OnTextBold();
	void slot_OnTextUnderline();
	void slot_OnTextItalic();

	void slot_OnTextFamily(const QString& fontFamily);
	void slot_OnTextSize(const QString &size);
	void slot_OnTextStyle(int styleIndex);
	void slot_OnTextColor();

	void slot_OnTextAlign(QAbstractButton* btn);

	void slot_OnInsertTable();
	void slot_OnMergeCell();
	void slot_OnSplitCell();
	void slot_OnInsertRow();
	void slot_OnInsertCol();
	void slot_OnOpenCellProperty();
		 
	void slot_OnDiscList();
	void slot_OnCircleList();
	void slot_OnSquareList();

	void slot_OnDecimalList();
	void slot_OnLowerAlphaList();
	void slot_OnUpperAlphaList();
	void slot_OnLowerRomanList();
	void slot_OnUpperRomanList();

private:
	ReportWidget(const ReportWidget& copy) {}
	const ReportWidget& operator=(const ReportWidget& rhs) {}

	void buttonRelease();

	QToolBar*	 createToolbar();
	QWidget*	 createEditingPlace();
	QScrollArea* createScrollArea();

private:
	const QSize m_BtnSize = QSize(32, 32);
	const QSize m_BtnColorIconSize = QSize(16, 16);

	std::vector<QString> m_temp_dir_list;

	ReportTextEdit* m_textEdit;

	QComboBox* m_comboReportFormat = nullptr;
	QPushButton* m_btnNewFormat = nullptr;
	QPushButton* m_btnSave = nullptr;
	QPushButton* m_btnSaveAs = nullptr;
	QPushButton* m_btnDel = nullptr;
	
	QPushButton* m_btnFileImport = nullptr;
	QPushButton* m_btnFileExport = nullptr;
	QPushButton* m_btnPrintPreview = nullptr;

	QPushButton* m_btnPACSImport = nullptr;
	QPushButton* m_btnPACSExport = nullptr;

	QPushButton* m_btnHeaderList = nullptr;
	QPushButton* m_btnSequenceList = nullptr;
	QPushButton* m_btnTextColor = nullptr;

	QPushButton* m_btnTextBold = nullptr;
	QPushButton* m_btnTextUnderline = nullptr;
	QPushButton* m_btnTextItalic = nullptr;

	QPushButton* m_btnTextLeftAlign = nullptr;
	QPushButton* m_btnTextCenterAlign = nullptr;
	QPushButton* m_btnTextRightAlign = nullptr;
	QPushButton* m_btnTextJustifyAlign = nullptr;

	QPushButton* m_btnInsertTable = nullptr;
	QPushButton* m_btnMergeCell = nullptr;
	QPushButton* m_btnSplitCell = nullptr;
	QPushButton* m_btnInsertRow = nullptr;
	QPushButton* m_btnInsertColumn = nullptr;
	QPushButton* m_btnCellProperty = nullptr;
};
#endif
