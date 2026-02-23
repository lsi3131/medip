#pragma once

#ifndef REPORTTEXTEDIT_H
#define REPORTTEXTEDIT_H

#include <qtextedit.h>
#include <memory>
#include <qdir.h>
#include <vector>
#include <array>

#include "define.h"
#include "ActionDeepCatchReport.h"

class QFile;
class COLOR;
class QPrinter;


enum class ImageResizingPointer
{
	TopLeft, TopMid, TopRight, MidLeft, MidRight, BotLeft, BotMid, BotRight, None
};

struct OverwriteImageFormat
{
	int fragmentPos;
	int fragmentLen;
	int cursorPos;
	QSize size;
};

class SelectedImageFormat
{
public:
	explicit SelectedImageFormat();
	~SelectedImageFormat();

private:
	static const QSize m_resizingControllerSize;

	bool m_select;
	bool m_resizing;
	int m_fragmentPos;
	int m_fragmentLen;
	QRectF m_imageRect;
	QString m_name;
	std::array<QRect, 8> m_resizingPointer;


public:
	void updateImage(const QTextCursor& cursor);
	void updateResizingPointer();
	void drawResizeController(QPainter& painter);

	ImageResizingPointer hitTest(const QPoint& mousePos);
	bool isHitResizingPointer(const QPoint& pos, const QRect& area);

	void setSelectedFormat(const QRect imgRect, const QString name, int fragmentPos, int fragmentLen);
	void releaseSelection();
	
	inline bool selectionFlag() { return m_select; }

	inline void setResizeFlag(bool val) { m_resizing = val; }
	inline bool resizingFlag() { return m_resizing; }

	inline QSizeF size() { return m_imageRect.size(); }
	inline const QRectF& rect() { return m_imageRect; }
	inline int getFragmentPos() { return m_fragmentPos; }
	inline int getFragmentLen() { return m_fragmentLen; }
	inline void setImageRect(const QRectF& rect) { m_imageRect = rect; };
};

class ReportTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	explicit ReportTextEdit(QWidget* parent = nullptr);
	~ReportTextEdit();

protected:
	bool canInsertFromMimeData(const QMimeData* source) const override;
	void insertFromMimeData(const QMimeData *source) override;

	void paintEvent(QPaintEvent *ev) override;
	void keyPressEvent(QKeyEvent *ev) override;
	void mousePressEvent(QMouseEvent * ev) override;
	void mouseMoveEvent(QMouseEvent * ev) override;
	void mouseReleaseEvent(QMouseEvent* ev) override;
	void mouseDoubleClickEvent(QMouseEvent * ev) override;
	void dropEvent(QDropEvent * ev) override;
	void dragLeaveEvent(QDragLeaveEvent * ev) override;
	void dragMoveEvent(QDragMoveEvent * ev) override;
	void scrollContentsBy(int dx, int dy) override;


	virtual void wheelEvent(QWheelEvent *_e) override;

public:
	bool newFormat(bool wantClear = true);
	bool fileSave();
	bool fileSaveAs();
	bool fileImport(const QString& path);
	bool fileExport();
	void pdfExport(QString fileName);
	bool deleteFormat();
	bool openReportFormatDir(const QString& path, QFileInfoList& fileList);
	bool openReportFormatDir(const QStringList& paths, QFileInfoList& fileList);

	void filePrint();
	void printPreview();

	void textBold();
	void textUnderline();
	void textItalic();
	void textFontFamily(const QString &fontFamily);
	void textSize(const QString &size);
	void textStyle(int styleIndex);
	QColor textColorChange();

	void textLeftAlign();
	void textCenterAlign();
	void textRightAlign();
	void textJustifyAlign();

	void importImage(const QImage& img);
	void insertTable();
	void mergeCell();
	void splitCell();
	void insertRow();
	void insertCol();
	void openCellProperty();
	bool loadFromDirName(const QString& dirName);

	void setPatientData(const QString& formatType);
	void setTextureFeatureUserData();
	void setTextureFeatureData();
	void setCOVID19AnalysisData(void);

	int checkModified();
	int currentHTMLSize();
	void getImageResource(std::vector<std::pair<QImage, QString>>& image_list);
	void getHTML(QString& html);
	bool loadFromMip(const QString& fileName, const std::vector<std::pair<QImage, QString>>& image_list, const QString& html);

	//MEDIP 폴더에 저장되어 있는 이미지들의 경로를 찾는 함수
	void imageResourcePath(const QString& htmlString, std::vector<QString>& path_list);
	//QTextDocument에 저장된 리소스들의 이름을 찾는 함수
	void imageResourceName(const QString& htmlString, std::vector<QString>& name_list);

	void releaseSelection();

	QString getFileName() { return m_fileName; }
	const std::vector<QString>& getDefaultFormats() { return m_defaultFormat_list; }

public slots:
	void OnPrintPreview(QPrinter *printer);
	void OnPageWidthLimit();

private:
	ReportTextEdit(const ReportTextEdit& copy) {}
	const ReportTextEdit& operator=(const ReportTextEdit& rhs) {}

	bool load(const QString &fileName);
	void setCurrentFileName(const QString &fileName);

	void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
	void dropImage(const QUrl& url, const QImage& image);
	void insertImage(QTextCursor &cursor, const QUrl& url, const QImage& image);
	void dropTextFile(const QUrl& url);

	void drawPageBoundaryLine(QPainter& painter);

	bool isImageFormat(const QPoint& mousePos, int& cursorPos);
	bool isTableFormat(const QPoint& mousePos, int& cursorPos);

	void setSeletedIamgeFormat(int imgCursorPos);

	bool findImageFormat(QTextBlock& block, QTextImageFormat& imgFmt, int imageCursorPos, int& fragmentPos, int& fragmentLen);

	void updateResizingPointer(const QSizeF& newSize);

	void resizingImgFormat(const QPoint& pos, ImageResizingPointer pointer);

	void mouseOver(bool over);
	QSize convertSizeToEditArea(const QSize& size);

	bool copyImageToReportDirectory();
	bool resourceCopyToReportDirectory();

	bool checkMEDIPBasicFormat(const QString& fileName);
	bool checkSpecialCharacters(const QString& str);

	bool canTextureFeatureMime(int cursorPos);
	
	void insertTextAfterRemove(int firstCursorPos, int secondCursorPos, const QString& str);
	void setTextCursorPosition(int cursorPos);
	void selectFragment(int fragmentPos, int fragmentLen);
	int getImageStringNumber(const QString& name, int num = 0);
	void updateImage();

	QRect drawSliceCustom(QPainter * p, WINDOW_TYPE eWinType, int nDepth, bool bOptimizeSize = false);
	void drawMaskCustom(QPainter *p, int nUID, WINDOW_TYPE eWinType, int nDepth, bool bForceDraw = false, QRect *optimizedRect = nullptr);
	void drawMaskCustom(QPainter *p, QVector<int> uids, WINDOW_TYPE eWinType, int nDepth, bool bForceDraw = false, QRect *optimizedRect = nullptr);

	QString getHtmlFileName(QFileInfoList & htmlList);

private:
	static const QSize IMPORTED_IMAGESIZE;
	static const QSize IMAGE_MIN_SIZE;

	QString m_fileName;
	QString m_textureFeatureLayerName;

	bool bLeftClicked;
	bool m_bImgOverwriteFlag;

	QPoint clickedPos;

	std::pair<bool, OverwriteImageFormat> m_imageOverwrite;
	std::pair<bool, int> m_selectedCellPos;

	QRectF m_mouseOverImgRect;
	SelectedImageFormat m_selectedImgFmt;
	std::vector<QString> m_defaultFormat_list;

	QString m_strHtml_1;
	QString m_strHtml_2;

	// deepcatch report
public:
	void setDeepCatchAnalysisData();

private:
	void patientInfoInsert(QTextTable* dicomInfoTable, DEEPCATCH_REPORT *pReport, QString strHeight, QString strWeight, int genderType);
	void volumeCaptureImgInsert(QTextTable* volumeviewCaptureImageInsertedTable, DEEPCATCH_REPORT *pReport);
	void axialViewDrawingImgInsert(QTextTable* axialDrawingImageInsertedTable, DEEPCATCH_REPORT *pReport);
	void areaChartDrawingImgInsert(QTextTable* chartImageInsertedTable, int limitHeight = 0);
//#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER
	void hccPredictResultInfoInsert(QTextTable* hccPredictResultTable, DEEPCATCH_REPORT *pReport);
	void qwtChartDrawImgInsert(QTextTable* qwtchartDrawImageInsertedTable, DEEPCATCH_REPORT *pReport);


	void muscleFatStateTableInfoInsert(QTextTable *patientStatusDisplayTable, DEEPCATCH_REPORT *pReport, QString strHeight, QString strWeight, int genderType, DEEPCATCH_REPORT_PREDICT_INFO *pPredictedInfo);
	void getDrawingImageSize_keepAspectRatio(DEEPCATCH_REPORT *pReport, WINDOW_TYPE type, int &width, int &height, int limitHeight = 0);

	QString m_strCurLoadedReportSubDirName;

	// zoom 관련.(차후 공통, 현재는 deepcatch report만 지원)
public:
	void zoom(float zoomingOneStepFactor);
	void initZoom();
	bool oneStepZoom(bool bZoomIn = true);

	float getZoomScaleFactor() const { return zoomScaleFactor; }
	void setZoomScaleFactor(float val) { zoomScaleFactor = val; }

	static const QSize				MIN_FIXED_DOCUMENT_SIZE;
	static const QSize				REAL_FIXED_DOCUMENT_SIZE;
	static const QSize				MAX_FIXED_DOCUMENT_SIZE;

	static const float				ONE_STEP_ZOOM_IN_SCALE_FACTOR;
	static const float				ONE_STEP_ZOOM_OUT_SCALE_FACTOR;
private:
	float zoomScaleFactor;

};
#endif