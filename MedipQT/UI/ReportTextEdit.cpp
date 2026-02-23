#include "stdafx.h"
#include "ReportTextEdit.h"

#include "graphics\color.h"
#include "Dialogs\ImageDialog.h"
#include "Dialogs\TableDialog.h"
#include "Dialogs\ReportTabDialog.h"
#include "System\stringManager.h"
#include "System\styleManager.h"
#include "Windows\windowManager.h"
#include "Actions\ActionManager.h"
#include "Actions\ActionImageFeatureExtractor.h"
#include "qprinterinfo.h"

#include "algorithm/FeatureExtractor.h"

#ifdef DEV_VER
#include <iostream>
#include <qdebug.h>
#endif // !DEV_VER
#include "algorithm\MagicCut.h"
#include "Network/Network.h"
#include "version.h"
#include "System/ProductManager.h"

#include "deepcatchAreaChart.h"
#include "Tabwindow.h"
#include "System\FileManager.h"

#include "DataContext.h"

const QSize SelectedImageFormat::m_resizingControllerSize = QSize(10, 10);

SelectedImageFormat::SelectedImageFormat()
	: m_select(false), m_resizing(false), m_fragmentPos(0), m_fragmentLen(0), m_imageRect(), m_name()
{
	for (int i = 0; i < m_resizingPointer.size(); ++i)
		m_resizingPointer[i] = QRect();
}

SelectedImageFormat::~SelectedImageFormat()
{

}

void SelectedImageFormat::updateResizingPointer()
{
	QPointF imgPos = QPointF(m_imageRect.topLeft());
	QSizeF imgSize = QSizeF(m_imageRect.size());
	QRect rect(QPoint(imgPos.x() - m_resizingControllerSize.width() / 2, imgPos.y() - m_resizingControllerSize.height() / 2), m_resizingControllerSize);

	m_resizingPointer[(int)ImageResizingPointer::TopLeft] = QRect(rect.topLeft(), m_resizingControllerSize);
	m_resizingPointer[(int)ImageResizingPointer::TopMid] = QRect(QPoint(rect.x() + imgSize.width() / 2, rect.y()), m_resizingControllerSize);
	m_resizingPointer[(int)ImageResizingPointer::TopRight] = QRect(QPoint(rect.x() + imgSize.width(), rect.y()), m_resizingControllerSize);

	m_resizingPointer[(int)ImageResizingPointer::MidLeft] = QRect(QPoint(rect.x(), rect.y() + imgSize.height() / 2), m_resizingControllerSize);
	m_resizingPointer[(int)ImageResizingPointer::MidRight] = QRect(QPoint(rect.x() + imgSize.width(), rect.y() + imgSize.height() / 2), m_resizingControllerSize);

	m_resizingPointer[(int)ImageResizingPointer::BotLeft] = QRect(QPoint(rect.x(), rect.y() + imgSize.height()), m_resizingControllerSize);
	m_resizingPointer[(int)ImageResizingPointer::BotMid] = QRect(QPoint(rect.x() + imgSize.width() / 2, rect.y() + imgSize.height()), m_resizingControllerSize);
	m_resizingPointer[(int)ImageResizingPointer::BotRight] = QRect(QPoint(rect.x() + imgSize.width(), rect.y() + imgSize.height()), m_resizingControllerSize);
}

void SelectedImageFormat::updateImage(const QTextCursor& cursor)
{
	QTextImageFormat imgFmt;
	imgFmt.setName(m_name);
	imgFmt.setWidth(m_imageRect.width());
	imgFmt.setHeight(m_imageRect.height());

	QTextCursor editCursor = cursor;

	editCursor.setPosition(m_fragmentPos);
	editCursor.setPosition(m_fragmentPos + m_fragmentLen, QTextCursor::KeepAnchor);
	editCursor.setCharFormat(imgFmt);
}

void SelectedImageFormat::drawResizeController(QPainter& painter)
{
	if (false == m_select) return;

	painter.drawRect(m_imageRect);

	painter.setBrush(Qt::white);
	for (int i = 0; i < m_resizingPointer.size(); ++i)
	{
		painter.drawEllipse(m_resizingPointer[i]);
	}
}

ImageResizingPointer SelectedImageFormat::hitTest(const QPoint& mousePos)
{
	for (int i = 0; i < m_resizingPointer.size(); ++i)
	{
		if (isHitResizingPointer(mousePos, m_resizingPointer[i]))
			return static_cast<ImageResizingPointer>(i);
	}
	return ImageResizingPointer::None;
}

bool SelectedImageFormat::isHitResizingPointer(const QPoint& pos, const QRect& area)
{
	int correction = 2;
	bool xRange = area.x() - correction <= pos.x() && pos.x() <= area.x() + area.width() + correction;
	bool yRange = area.y() - correction <= pos.y() && pos.y() <= area.y() + area.height() + correction;

	if (xRange && yRange)
		return true;

	return false;
}

void SelectedImageFormat::setSelectedFormat(const QRect imgRect, const QString name, int fragmentPos, int fragmentLen)
{
	m_select = true;
	m_fragmentPos = fragmentPos;
	m_fragmentLen = fragmentLen;
	m_imageRect = imgRect;
	m_name = name;

	updateResizingPointer();
}

void SelectedImageFormat::releaseSelection()
{
	m_select = false;
	m_resizing = false;
	m_fragmentPos = 0;
	m_fragmentLen = 0;
	m_imageRect = QRect();
	m_name.clear();
}

//======================================================================================

const QSize ReportTextEdit::IMPORTED_IMAGESIZE = QSize(150, 150);
const QSize ReportTextEdit::IMAGE_MIN_SIZE = QSize(30, 30);

const QSize ReportTextEdit::MIN_FIXED_DOCUMENT_SIZE = QSize(595, 842);
const QSize ReportTextEdit::REAL_FIXED_DOCUMENT_SIZE = QSize(1240, 1754);
const QSize ReportTextEdit::MAX_FIXED_DOCUMENT_SIZE = QSize(2480, 3508);

const float ReportTextEdit::ONE_STEP_ZOOM_IN_SCALE_FACTOR = 1.25f;
const float ReportTextEdit::ONE_STEP_ZOOM_OUT_SCALE_FACTOR = 0.8f;

#ifndef DEEPCATCH_REPORT_150PPI_INTEGRATE
ReportTextEdit::ReportTextEdit(QWidget* parent)
	: QTextEdit(parent), m_fileName(), m_textureFeatureLayerName(), bLeftClicked(false), m_bImgOverwriteFlag(false), clickedPos(),
	m_imageOverwrite(std::make_pair(false, OverwriteImageFormat())), m_selectedCellPos(std::make_pair(false, -1)), m_mouseOverImgRect(), m_selectedImgFmt()
{
	setStyleSheet(STYLE_MANAGER->reportTextEdit);
	
	zoomScaleFactor = 1.0f;

	setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

	QSizeF size = QSizeF(595.0f, 842.0f)/*.scaled(WIN_MANAGER->mainCenterWidget->size(), Qt::AspectRatioMode::KeepAspectRatio)*/; //A4 Pixel ??????;
	m_strCurLoadedReportSubDirName = QString("595x842");
	if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) || !(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2))
	{
		// 해상도에 따른 레포트 해상도 자동 변경.
		QScreen *screen = QGuiApplication::primaryScreen();
		QRect  screenGeometry = screen->geometry();
		int height = screenGeometry.height();
		int width = screenGeometry.width();

		QVector<QPair<QSizeF, QString>> vecA4Size;
		vecA4Size.append(QPair<QSizeF, QString>(QSizeF(595.0f, 842.0f), QString("595x842")));		// 72 ppi
	//	vecA4Size.append(QPair<QSizeF, QString>(QSizeF(794.0f, 1123.0f), QString("794x1123")));		// 96 ppi
		vecA4Size.append(QPair<QSizeF, QString>(QSizeF(1240.0f, 1754.0f), QString("1240x1754")));	// 150 ppi
	//	vecA4Size.append(QPair<QSizeF, QString>(QSizeF(2480.0f, 3508.0f), QString("2480x3508")));	// 300 ppi
		int findIndex = -1;
		for (int i = 0; i < vecA4Size.size(); i++)
		{
			if (vecA4Size.at(i).first.width() <= width && vecA4Size.at(i).first.height() <= height)
				findIndex = i;
		}
		// size 계산.
		if (findIndex != -1)
		{
			size = vecA4Size.at(findIndex).first;
			m_strCurLoadedReportSubDirName = vecA4Size.at(findIndex).second;
		}
	}

	setFixedSize(size.toSize());
	setLineWrapMode(QTextEdit::FixedPixelWidth);
	setLineWrapColumnOrWidth(static_cast<int>(size.width()));
	setFixedWidth(size.toSize().width());
	document()->setPageSize(size);

	viewport()->setMouseTracking(true);
	viewport()->installEventFilter(this);

	setContextMenuPolicy(Qt::CustomContextMenu);

	// default report format 지정.
	m_defaultFormat_list.reserve(10);
	QString strTempleteName;
	if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_MEDIP_COVID19))
		strTempleteName = PRODUCT_NAME_MEDIP_COVID19;
	else if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH))
		strTempleteName = PRODUCT_NAME_DEEPCATCH;
	else if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2))
		strTempleteName = PRODUCT_NAME_DEEPCATCH_V2;
	else
		strTempleteName = "Texture_Feature";
	m_defaultFormat_list.push_back(strTempleteName);

	//connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(test()));
	connect(document()->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged, this, &ReportTextEdit::OnPageWidthLimit);

	m_strHtml_1 = QString("<span style=\"font-wieight:600; font-family:" "'Yu Gothic UI'" "; color:#000000; \">");
	m_strHtml_2 = QString("</span>");
}
#else
ReportTextEdit::ReportTextEdit(QWidget* parent)
	: QTextEdit(parent), m_fileName(), m_textureFeatureLayerName(), bLeftClicked(false), m_bImgOverwriteFlag(false), clickedPos(),
	m_imageOverwrite(std::make_pair(false, OverwriteImageFormat())), m_selectedCellPos(std::make_pair(false, -1)), m_mouseOverImgRect(), m_selectedImgFmt()
{
	setStyleSheet(STYLE_MANAGER->reportTextEdit);

	zoomScaleFactor = 1.0f;

	//	setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	//	setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	verticalScrollBar()->setStyleSheet(STYLE_MANAGER->reportScrollArea);
	horizontalScrollBar()->setStyleSheet(STYLE_MANAGER->reportScrollArea);

	QSizeF size = QSizeF(595.0f, 842.0f)/*.scaled(WIN_MANAGER->mainCenterWidget->size(), Qt::AspectRatioMode::KeepAspectRatio)*/; //A4 Pixel ??????;
	m_strCurLoadedReportSubDirName = QString("595x842");
	if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH)
		|| !(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2))
	{
		// 리포트 해상도 150ppi로 고정 건
		size = QSizeF(1240.0f, 1754.0f);
		m_strCurLoadedReportSubDirName = QString("1240x1754");
	}

	setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	setFixedSize(size.toSize());
	setLineWrapMode(QTextEdit::FixedPixelWidth);
	setLineWrapColumnOrWidth(static_cast<int>(size.width()));
	setFixedWidth(size.toSize().width());
	document()->setPageSize(size);

	viewport()->setMouseTracking(true);
	viewport()->installEventFilter(this);

	setContextMenuPolicy(Qt::CustomContextMenu);

	// default report format 지정.
	m_defaultFormat_list.reserve(10);
	QString strTempleteName;
	if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_MEDIP_COVID19))
		strTempleteName = PRODUCT_NAME_MEDIP_COVID19;
	else if (!(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) || !(PRODUCT_MANAGER->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2))
		strTempleteName = PRODUCT_NAME_DEEPCATCH;
	else
		strTempleteName = "Texture_Feature";
	m_defaultFormat_list.push_back(strTempleteName);

	//connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(test()));

	m_strHtml_1 = QString("<span style=\"font-wieight:600; font-family:" "'Yu Gothic UI'" "; color:#000000; \">");
	m_strHtml_2 = QString("</span>");
}
#endif

ReportTextEdit::~ReportTextEdit()
{

}

bool ReportTextEdit::canInsertFromMimeData(const QMimeData* source) const
{
	return source->hasImage() || source->hasUrls() || source->hasFormat("Texture_Feature") ||
		QTextEdit::canInsertFromMimeData(source);
}

void ReportTextEdit::insertFromMimeData(const QMimeData * source)
{
	if (source->hasImage())
	{
		static int i = getImageStringNumber("dropped_image_%1.png", 0);
		QUrl url(QString("dropped_image_%1.png").arg(i++));

		QImage image = qvariant_cast<QImage>(source->imageData());
		QSize size = image.size();
		dropImage(url, image.scaled(size, Qt::KeepAspectRatio));
	}
	else if (source->hasUrls())
	{
		foreach(QUrl url, source->urls())
		{
			QFileInfo info(url.toLocalFile());

			if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1()))
			{
				QImage image(info.filePath());
				dropImage(url, image.scaled(convertSizeToEditArea(image.size()), Qt::KeepAspectRatio));
			}
			else if ("html" == info.suffix())
			{
				if (!load(info.filePath()))
					QMessageBox::warning(nullptr, "Report Tab", tr("Could not open \"%1\"").arg(QDir::toNativeSeparators(info.fileName())));
			}
			else
				dropTextFile(url);
		}
	}
	else if (source->hasFormat("Texture_Feature"))
	{
		if (!WIN_MANAGER->IsLicensePass())
		{
			QMessageBox::warning(this, tr("Not Supported"), tr("Supported features in current version.\n(Edu version or later is supported.)"));
			m_selectedCellPos.first = false;
			m_selectedCellPos.second = -1;
			return;
		}

		if (false == m_selectedCellPos.first) return;

		QByteArray cellDataArray = source->data("Texture_Feature");
		char* data = cellDataArray.data();

		int nextAddress = 0;
		WCHAR layerName[20];
		std::memcpy(&layerName, &data[nextAddress], sizeof(layerName));
		nextAddress += sizeof(layerName);
		m_textureFeatureLayerName = QString::fromWCharArray(layerName);

		int mI = 0;
		std::memcpy(&mI, &data[nextAddress], sizeof(int));
		nextAddress += sizeof(int);

		MaskInfo info;
		std::memcpy(&info, &data[nextAddress], sizeof(MaskInfo));
		nextAddress += sizeof(MaskInfo);
		
		ACTION_MANAGER->action_ImageFeatureExtractor_Add(mI == 0 ? info.mask_id : info.mask_id2, mI, info.uid,true);
	}
	else
	{
		QTextEdit::insertFromMimeData(source);
	}
}

void ReportTextEdit::paintEvent(QPaintEvent *ev)
{
	QTextEdit::paintEvent(ev);

	QPainter painter(viewport());

	drawPageBoundaryLine(painter);

	m_selectedImgFmt.drawResizeController(painter);

	if (m_bImgOverwriteFlag)
	{
		painter.fillRect(m_mouseOverImgRect, QColor(0, 0, 128, 128));
	}
}

void ReportTextEdit::keyPressEvent(QKeyEvent *ev)
{
	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_EditingFunction))
	{
		if (ev->matches(QKeySequence::Copy) && m_selectedImgFmt.selectionFlag())
		{
			int pos = m_selectedImgFmt.getFragmentPos();
			int len = m_selectedImgFmt.getFragmentLen();
			selectFragment(pos, len);
		}
		else if (ev->matches(QKeySequence::Cut) && m_selectedImgFmt.selectionFlag())
		{
			int pos = m_selectedImgFmt.getFragmentPos();
			int len = m_selectedImgFmt.getFragmentLen();
			selectFragment(pos, len);
			m_selectedImgFmt.releaseSelection();
		}
		else if (ev->key() != Qt::Key_Control)
			releaseSelection();
	}

	QTextEdit::keyPressEvent(ev);
}

void ReportTextEdit::mousePressEvent(QMouseEvent * ev)
{
	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_EditingFunction))
	{
		if (ev->button() == Qt::LeftButton)
		{
			bLeftClicked = true;
			if (Qt::CursorShape::SizeVerCursor <= viewport()->cursor().shape() &&
				viewport()->cursor().shape() <= Qt::CursorShape::SizeFDiagCursor)
			{
				m_selectedImgFmt.setResizeFlag(true);
			}
			clickedPos = ev->pos();
		}
	}

	QTextEdit::mousePressEvent(ev);
}

void ReportTextEdit::mouseMoveEvent(QMouseEvent * ev)
{
	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_EditingFunction))
	{
		int cursorPos = 0;
		static ImageResizingPointer pointer = ImageResizingPointer::None;
		if (isImageFormat(ev->pos(), cursorPos))
		{
			if (false == m_selectedImgFmt.resizingFlag())
			{
				pointer = m_selectedImgFmt.hitTest(ev->pos());

				if (ImageResizingPointer::TopMid == pointer || ImageResizingPointer::BotMid == pointer)
					viewport()->setCursor(Qt::SizeVerCursor);
				else if (ImageResizingPointer::MidLeft == pointer || ImageResizingPointer::MidRight == pointer)
					viewport()->setCursor(Qt::SizeHorCursor);
				else if (ImageResizingPointer::MidLeft == pointer || ImageResizingPointer::MidRight == pointer)
					viewport()->setCursor(Qt::SizeHorCursor);
				else if (ImageResizingPointer::TopRight == pointer || ImageResizingPointer::BotLeft == pointer)
					viewport()->setCursor(Qt::SizeBDiagCursor);
				else if (ImageResizingPointer::TopLeft == pointer || ImageResizingPointer::BotRight == pointer)
					viewport()->setCursor(Qt::SizeFDiagCursor);
				else
					viewport()->setCursor(Qt::SizeAllCursor);
			}
		}
		else
		{
			viewport()->setCursor(Qt::IBeamCursor);
		}

		if (true == m_selectedImgFmt.resizingFlag())
		{
			viewport()->setCursor(Qt::CrossCursor);
			resizingImgFormat(ev->pos(), pointer);
			update();
			return;
		}
	}

	QTextEdit::mouseMoveEvent(ev);
}

void ReportTextEdit::mouseReleaseEvent(QMouseEvent* ev)
{
	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_EditingFunction))
	{
		int cursorPos = 0;

		//리사이징 완료
		if (true == m_selectedImgFmt.resizingFlag())
		{
			updateImage();
			QTextCursor cursor = textCursor();
			cursor.setPosition(m_selectedImgFmt.getFragmentPos());
			QRect rect = cursorRect(cursor);
			QPointF imgPos(viewport()->mapToParent(rect.bottomLeft()));
			imgPos.ry() -= m_selectedImgFmt.rect().size().height() + 1;
			imgPos.ry() = std::max(imgPos.ry(), static_cast<qreal>(rect.topLeft().y()));
			QRectF imgFmtRect(imgPos, m_selectedImgFmt.rect().size());
			m_selectedImgFmt.setImageRect(imgFmtRect);
			m_selectedImgFmt.updateResizingPointer();
			//updateResizingPointer(m_selectedImgFmt.rect().size());
		}

		//이미지 선택, 해제
		if (false == m_selectedImgFmt.resizingFlag())
		{
			if (isImageFormat(ev->pos(), cursorPos) && !textCursor().hasSelection())
			{
				setSeletedIamgeFormat(cursorPos - 1);
				setTextCursorPosition(cursorPos - 1);
				update();
			}
			else
			{
				releaseSelection();
			}
		}

		bLeftClicked = false;
		m_selectedImgFmt.setResizeFlag(false);
		mouseOver(false);
	}

	QTextEdit::mouseReleaseEvent(ev);
}

void ReportTextEdit::mouseDoubleClickEvent(QMouseEvent * ev)
{
	QTextEdit::mouseDoubleClickEvent(ev);

	if (PRODUCT_MANAGER->IsAvailableFunctionLevel(MFL_Common_Report_EditingFunction))
	{
		int cursorPos = 0;
		if (isImageFormat(ev->pos(), cursorPos))
		{
			if (m_selectedImgFmt.size().width() <= 0 || m_selectedImgFmt.size().height() <= 0) return;

			int width = std::max(m_selectedImgFmt.size().width(), 1.0);
			int height = std::max(m_selectedImgFmt.size().height(), 1.0);

			ImageDialog dlg(width, height, this, Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
			if (dlg.exec() == QDialog::Accepted)
			{
				QSize size = dlg.getImageSize();
				if (IMAGE_MIN_SIZE.width() > size.width() || IMAGE_MIN_SIZE.height() > size.height()) return;
				QTextCursor cursor = textCursor();
				cursor.setPosition(m_selectedImgFmt.getFragmentPos());
				QRect rect = cursorRect(cursor);
				QPointF imgPos(viewport()->mapToParent(rect.bottomLeft()));
				imgPos.ry() -= m_selectedImgFmt.rect().size().height() + 1;
				imgPos.ry() = std::max(imgPos.ry(), static_cast<qreal>(rect.topLeft().y()));
				QRectF imgRect(imgPos, size);
				//imgRect.setSize(size);
				m_selectedImgFmt.setImageRect(imgRect);
				updateImage();
				updateResizingPointer(size);
			}
			QTextCursor cursor = textCursor();
			cursor.clearSelection();
			setTextCursor(cursor);
		}
	}
}

void ReportTextEdit::dropEvent(QDropEvent * ev)
{
	int cursorPos = 0;

	if (isImageFormat(ev->pos(), cursorPos))
	{
		if (true == m_selectedImgFmt.selectionFlag())
		{
			textCursor().setPosition(cursorPos);
			QTextImageFormat imgFmt;
			OverwriteImageFormat overwriteFmt;
			findImageFormat(textCursor().block(), imgFmt, cursorPos - 1, overwriteFmt.fragmentPos, overwriteFmt.fragmentLen);
			overwriteFmt.cursorPos = cursorPos;
			overwriteFmt.size = QSize(imgFmt.width(), imgFmt.height());
			m_imageOverwrite = std::make_pair(true, overwriteFmt);
		}
	}
	else if (isTableFormat(ev->pos(), cursorPos) && canTextureFeatureMime(cursorPos))
	{
		m_selectedCellPos.first = true;
		m_selectedCellPos.second = cursorPos;
	}
	bLeftClicked = false;
	m_selectedImgFmt.setResizeFlag(false);
	mouseOver(false);
	QTextEdit::dropEvent(ev);
}

void ReportTextEdit::dragLeaveEvent(QDragLeaveEvent * ev)
{
	mouseOver(false);
	QTextEdit::dragLeaveEvent(ev);
}

void ReportTextEdit::dragMoveEvent(QDragMoveEvent * ev)
{
	int cursorPos = 0;
	if (isImageFormat(ev->pos(), cursorPos) && m_selectedImgFmt.selectionFlag())
	{
		m_mouseOverImgRect = m_selectedImgFmt.rect();
		mouseOver(true);
	}
	else
		mouseOver(false);

	QTextEdit::dragMoveEvent(ev);
}

void ReportTextEdit::scrollContentsBy(int dx, int dy)
{
	releaseSelection();
	QTextEdit::scrollContentsBy(dx, dy);
}

void ReportTextEdit::wheelEvent(QWheelEvent *_e)
{
#ifdef DEEPCATCH_REPORT_150PPI_INTEGRATE
//	qDebug() << "void ReportTextEdit::wheelEvent(QWheelEvent *e) enter";
	if (_e->modifiers() & Qt::ControlModifier) 
	{
	//	qDebug() << "wheel deleta : " << delta;
		float delta = _e->angleDelta().y() / 120.f;
		if (delta != 0.f)
		{
			if (delta < 0.f)
			{
				oneStepZoom(false);
			//	qDebug() << "zoom out : curScaleFactor : " << getZoomScaleFactor();
			//	qDebug() << "zoom out : curSize : " << size() << endl;
				return;
			}
			else if (delta > 0.f)
			{
				oneStepZoom(true);
			//	qDebug() << "zoom in : curScaleFactor : " << getZoomScaleFactor();
			//	qDebug() << "zoom in : curSize : " << size() << endl;
				return;
			}
		}
	}
#endif
	QTextEdit::wheelEvent(_e);
//	updateMicroFocus();
}

bool ReportTextEdit::oneStepZoom(bool bZoomIn)
{
	if (bZoomIn)
	{
		QSize changingSize = size()*ONE_STEP_ZOOM_IN_SCALE_FACTOR;
		QSize maxSize = MAX_FIXED_DOCUMENT_SIZE;
		if (maxSize.width() >= changingSize.width() && maxSize.height() >= changingSize.height())
		{
			zoom(ONE_STEP_ZOOM_IN_SCALE_FACTOR);
			setZoomScaleFactor(getZoomScaleFactor()*ONE_STEP_ZOOM_IN_SCALE_FACTOR);

			setFixedSize(changingSize);
		//	setLineWrapMode(QTextEdit::FixedPixelWidth);
			setLineWrapColumnOrWidth(static_cast<int>(changingSize.width()));
			setFixedWidth(changingSize.width());
			document()->setPageSize(changingSize);
			return true;
		}
	}
	else
	{
		QSize changingSize = size()*ONE_STEP_ZOOM_OUT_SCALE_FACTOR;
		QSize minSize = MIN_FIXED_DOCUMENT_SIZE;
		if (minSize.width() <= changingSize.width() && minSize.height() <= changingSize.height())
		{
			zoom(ONE_STEP_ZOOM_OUT_SCALE_FACTOR);
			setZoomScaleFactor(getZoomScaleFactor()*ONE_STEP_ZOOM_OUT_SCALE_FACTOR);

			setFixedSize(changingSize);
		//	setLineWrapMode(QTextEdit::FixedPixelWidth);
			setLineWrapColumnOrWidth(static_cast<int>(changingSize.width()));
			setFixedWidth(changingSize.width());
			document()->setPageSize(changingSize);
			return true;
		}
	}
	return false;
}

bool ReportTextEdit::newFormat(bool wantClear)
{
	QString fileName;
	ReportDirNamingDialog dlg(this, Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

	bool success = false;

	while (false == success)
	{
		int result = dlg.exec();

		if (result == QDialog::Accepted)
			fileName = dlg.getDirName();
		else if (result == QDialog::Rejected)
			return false;
		QString test = QFileInfo(fileName).baseName();
		success = !QFileInfo(fileName).baseName().isEmpty();
		if (false == success)
		{
			QMessageBox::warning(nullptr, "Failed", "It is an empty string.");
			continue;
		}

		success = checkSpecialCharacters(QFileInfo(fileName).baseName());
		if (false == success)
		{
			QMessageBox::warning(nullptr, "Failed", "The name is not in English or contains special characters.");
			continue;
		}
#ifndef REPORT_CREATER
		success = !checkMEDIPBasicFormat(QFileInfo(fileName).baseName());
		if (false == success)
		{
			QMessageBox::warning(nullptr, "Failed", "The name is the default format for MEDIP.");
			continue;
		}
#endif
		QDir dir(STRING_MANAGER->report_format + "/" + QFileInfo(fileName).baseName());
		bool isExists = dir.exists();

		if (true == isExists)
		{
			QMessageBox::warning(nullptr, "Failed", "The name of the file already saved.");
		}
		success = isExists ? false : true;
	}

	setCurrentFileName(STRING_MANAGER->report_format + "/" + QFileInfo(fileName).baseName() + "/" + fileName);
	if (true == wantClear) clear();

	return fileSave();
}

bool ReportTextEdit::fileSave()
{
	if (m_fileName.size() <= 0)
		newFormat();

	setCurrentFileName(STRING_MANAGER->report_format + "/" + QFileInfo(m_fileName).baseName() + "/" + QFileInfo(m_fileName).fileName());

#ifndef REPORT_CREATER
	bool success = !checkMEDIPBasicFormat(QFileInfo(m_fileName).baseName());
	if (false == success)
	{
		const QMessageBox::StandardButton ret = QMessageBox::warning(nullptr, "Warning",
			"The name is MEDIP's default format.\nDo you want save as?.",
			QMessageBox::Ok | QMessageBox::Cancel);

		if (QMessageBox::Ok == ret)
			return fileSaveAs();
		else if (QMessageBox::Cancel == ret)
			return false;
	}
#endif
	QDir dir(STRING_MANAGER->report_format);
#ifndef REPORT_CREATER
	success = dir.mkdir(QFileInfo(m_fileName).baseName());
#else
	bool success = dir.mkdir(QFileInfo(m_fileName).baseName());
#endif
	if (false == success)
	{
		dir.cd(QFileInfo(m_fileName).baseName());
		success = dir.remove(QFileInfo(m_fileName).baseName() + ".html");
	}

	copyImageToReportDirectory();
	resourceCopyToReportDirectory();

	QTextDocumentWriter writer(m_fileName);
	success = writer.write(document());
	if (success) {
		document()->setModified(false);
	}
	else {
		QMessageBox::warning(nullptr, "Save Failed", tr("Could not save \"%1\"").arg(QDir::toNativeSeparators(m_fileName)));
	}
	return success;
}

bool ReportTextEdit::fileSaveAs()
{
	bool result = newFormat(false);
	if (false == result) return false;
	return fileSave();
}

bool ReportTextEdit::fileImport(const QString& path)
{
	releaseSelection();
	return load(path);
}

bool ReportTextEdit::fileExport()
{
	QString strLatestOpenPath = "";
	bool latest = false;
	latest = WIN_MANAGER->lastestPathGet(strLatestOpenPath, true);

	QString filepath = latest ? strLatestOpenPath : QDir::homePath() + "/Desktop";

	QFileInfo fileInfo(filepath);

	QString fileName = ExportPDFDialog(
		this,
		document(),
		tr("Export file"),
		fileInfo.fileName(),
		fileInfo.dir().path(),
		tr("PDF (*.pdf);;HTML (*.html)"),
		QFileDialog::ShowDirsOnly);

	QString type = QFileInfo(fileName).suffix();

	if ("html" == type)
	{
		QTextDocumentWriter writer(fileName);
		writer.write(document());
	}
	else if ("pdf" == type)
	{
		//QPrinterInfo info = QPrinterInfo::defaultPrinter();
		//if (info.state() == QPrinter::Error)
		//{
		//	QMessageBox::warning(nullptr, "Failed", "Can not save to PDF.");
		//	return false;
		//}
		//QPrinter printer(info, QPrinter::HighResolution);

		pdfExport(fileName);
	}

	return true;
}

void ReportTextEdit::pdfExport(QString fileName)
{
	QPrinter printer(QPrinter::HighResolution);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setPaperSize(QPrinter::A4);
	printer.setOutputFileName(fileName);
	printer.setPageMargins(QMarginsF(15, 15, 15, 15));

	document()->print(&printer);
}


bool ReportTextEdit::deleteFormat()
{
	bool result = checkMEDIPBasicFormat(QFileInfo(m_fileName).baseName());
	if (true == result)
	{
		QMessageBox::warning(nullptr, "Failed", "Can not delete the default format.");
		return false;
	}

	const QMessageBox::StandardButton ret = QMessageBox::warning(nullptr, "Warning",
		"Are you sure you want to delete the form?",
		QMessageBox::Ok | QMessageBox::Cancel
	);
	if (ret == QMessageBox::Ok)
	{
		return QDir(STRING_MANAGER->report_format + "/" + QFileInfo(m_fileName).baseName()).removeRecursively();
	}
	else if (ret == QMessageBox::Cancel)
		return false;
	return false;
}

bool ReportTextEdit::openReportFormatDir(const QString& path, QFileInfoList& fileList)
{
	QDir reportDir(path);

	reportDir.setFilter((QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot));

	fileList = reportDir.entryInfoList();
	if (fileList.size() <= 0) return false;

	fileList.erase(std::remove_if(fileList.begin(), fileList.end(), [](const QFileInfo& info) {
		QDir dir(info.filePath());
		return dir.isEmpty();
	}), fileList.end()); //비어있는 디렉토리 삭제

	fileList.erase(std::remove_if(fileList.begin(), fileList.end(), [](const QFileInfo& info) {
		QDir dir(info.filePath());
		return dir.dirName().contains(".");
	}), fileList.end()); // ( . )을 포함하는 디렉토리 삭제

	return loadFromDirName(m_defaultFormat_list[0]);
}

bool ReportTextEdit::openReportFormatDir(const QStringList& paths, QFileInfoList& fileList)
{
	for (int i = 0; i < paths.size(); i++)
	{
		QFileInfoList tempFileList;
		QDir reportDir(paths.at(i));
		reportDir.setFilter((QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot));
		tempFileList = reportDir.entryInfoList();
		fileList += tempFileList;
	}

	if (fileList.size() <= 0) return false;

	fileList.erase(std::remove_if(fileList.begin(), fileList.end(), [](const QFileInfo& info) {
		QDir dir(info.filePath());
		return dir.isEmpty();
	}), fileList.end()); //비어있는 디렉토리 삭제

	fileList.erase(std::remove_if(fileList.begin(), fileList.end(), [](const QFileInfo& info) {
		QDir dir(info.filePath());
		return dir.dirName().contains(".");
	}), fileList.end()); // ( . )을 포함하는 디렉토리 삭제

	return loadFromDirName(m_defaultFormat_list[0]);
}

void ReportTextEdit::filePrint()
{
	/*QPrinterInfo info = QPrinterInfo::defaultPrinter();
	if (info.state() == QPrinter::Error)
	{
		QMessageBox::warning(nullptr, "Failed", "Can not print.");
		return;
	}*/
	//QPrinter printer(info, QPrinter::HighResolution);
	QPrinter printer(QPrinter::HighResolution);
	QPrintDialog dlg(&printer, this);
	if (textCursor().hasSelection())
		dlg.addEnabledOption(QAbstractPrintDialog::PrintSelection);
	dlg.setWindowTitle(tr("Print Document"));
	if (dlg.exec() == QDialog::Accepted)
		print(&printer);
}

void ReportTextEdit::printPreview()
{
	//QList<QPrinterInfo> printer_list = QPrinterInfo::availablePrinters();
	//if (0 == printer_list.size())
	//{
	//	QMessageBox::warning(nullptr, "Failed", tr("Can not print."));
	//	return;
	//}
	//int printIndex = 0;
	//QPrinterInfo info = printer_list[printIndex];
	//while (info.state() == QPrinter::Error)
	//{
	//	printIndex++;
	//	if (printer_list.size() <= printIndex) break;
	//	info = printer_list[printIndex];
	//}
	//if (info.state() == QPrinter::Error)
	//{
	//	QMessageBox::warning(nullptr, "Failed", "Can not print.");
	//	return;
	//}
	//QPrinter printer(info, QPrinter::HighResolution);
	QPrinter printer(QPrinter::HighResolution);
	printer.setFromTo(1, document()->pageCount());
	QPrintPreviewDialog preview(&printer, this);
	connect(&preview, &QPrintPreviewDialog::paintRequested, this, &ReportTextEdit::OnPrintPreview);
	preview.exec();
}

void ReportTextEdit::textBold()
{
	QTextCharFormat fmt;
	QTextCursor cursor = textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	bool isBold = cursor.charFormat().fontWeight() == QFont::Bold;
	fmt.setFontWeight(isBold ? QFont::Normal : QFont::Bold);
	mergeFormatOnWordOrSelection(fmt);
}

void ReportTextEdit::textUnderline()
{
	QTextCharFormat fmt;
	QTextCursor cursor = textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	bool bUnder = cursor.charFormat().fontUnderline();
	fmt.setFontUnderline(!bUnder);
	mergeFormatOnWordOrSelection(fmt);
}

void ReportTextEdit::textItalic()
{
	QTextCharFormat fmt;
	QTextCursor cursor = textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	bool bItalic = cursor.charFormat().fontItalic();
	fmt.setFontItalic(!bItalic);
	mergeFormatOnWordOrSelection(fmt);
}

void ReportTextEdit::textFontFamily(const QString &fontFamily)
{
	QTextCharFormat fmt;
	fmt.setFontFamily(fontFamily);
	mergeFormatOnWordOrSelection(fmt);
}

void ReportTextEdit::textSize(const QString &size)
{
	qreal pointSize = size.toFloat();
	if (size.toFloat() > 0) {
		QTextCharFormat fmt;
		fmt.setFontPointSize(pointSize);
		mergeFormatOnWordOrSelection(fmt);

		QTextCursor cursor = textCursor();

		if (cursor.currentList()) {
			QTextBlockFormat blockFmt = cursor.blockFormat();
			QTextListFormat listFmt;
			int indent = blockFmt.indent();
			listFmt.setIndent(blockFmt.indent() + 1);

			qreal fontSize = 0.0;
			if (textCursor().atBlockEnd())
			{
				QTextBlock block = textCursor().block();
				fontSize = block.charFormat().fontPointSize();
			}
			else
			{
				textCursor().movePosition(QTextCursor::EndOfBlock);
				fontSize = textCursor().charFormat().fontPointSize();
			}
			qreal docIndent = document()->indentWidth();
			int indentation = std::floor(fontSize * 3.0 / docIndent);
			blockFmt.setIndent(indentation);
			cursor.setBlockFormat(blockFmt);
		}
	}
}

void ReportTextEdit::textStyle(int styleIndex)
{
	QTextCursor cursor = textCursor();

	if (styleIndex != 0) {
		QTextListFormat::Style style = QTextListFormat::ListDisc;

		switch (styleIndex) {
		default:
		case 1:
			style = QTextListFormat::ListDisc;
			break;
		case 2:
			style = QTextListFormat::ListCircle;
			break;
		case 3:
			style = QTextListFormat::ListSquare;
			break;
		case 4:
			style = QTextListFormat::ListDecimal;
			break;
		case 5:
			style = QTextListFormat::ListLowerAlpha;
			break;
		case 6:
			style = QTextListFormat::ListUpperAlpha;
			break;
		case 7:
			style = QTextListFormat::ListLowerRoman;
			break;
		case 8:
			style = QTextListFormat::ListUpperRoman;
			break;
		}

		cursor.beginEditBlock();

		QTextBlockFormat blockFmt = cursor.blockFormat();

		QTextListFormat listFmt;

		if (cursor.currentList())
			listFmt = cursor.currentList()->format();

		if(listFmt.style() == style)
		{
			style = QTextListFormat::ListStyleUndefined;
			listFmt.setIndent(0);
			blockFmt.setIndent(0);
		}
		else {
			listFmt.setIndent(blockFmt.indent() + 1);
			
			qreal fontSize = 0.0;
			if (textCursor().atBlockEnd())
			{
				QTextBlock block = textCursor().block();
				fontSize = block.charFormat().fontPointSize();
			}
			else
			{
				textCursor().movePosition(QTextCursor::EndOfBlock);
				fontSize = textCursor().charFormat().fontPointSize();
			}
			qreal docIndent = document()->indentWidth();
			int indentation = std::floor(fontSize * 3.0 / docIndent);
			blockFmt.setIndent(indentation);
			cursor.setBlockFormat(blockFmt);
		}

		listFmt.setStyle(style);

		cursor.createList(listFmt);

		cursor.endEditBlock();
	}
	else {
		QTextBlockFormat bfmt;
		bfmt.setObjectIndex(-1);
		cursor.mergeBlockFormat(bfmt);
	}
}

QColor ReportTextEdit::textColorChange()
{
	QColorDialog dlg(textColor());
	dlg.setStyleSheet("background-color : rgba(48,48,48,255); color : white");

	if (dlg.exec() == QDialog::Accepted)
	{
		QColor color = dlg.selectedColor();

		if (!color.isValid())
			return color;
		QTextCharFormat fmt;
		fmt.setForeground(color);
		mergeFormatOnWordOrSelection(fmt);
		return color;
	}

	return textColor();
}

void ReportTextEdit::textLeftAlign()
{
	setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
}

void ReportTextEdit::textCenterAlign()
{
	setAlignment(Qt::AlignHCenter);
}

void ReportTextEdit::textRightAlign()
{
	setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
}

void ReportTextEdit::textJustifyAlign()
{
	setAlignment(Qt::AlignJustify);
}

void ReportTextEdit::importImage(const QImage& img)
{
	static int cnt = getImageStringNumber("import_image_%1.png", 0);
	QString imgName(QString("import_image_%1.png").arg(++cnt));

	document()->addResource(QTextDocument::ImageResource, QUrl(imgName), img);
	
	QSize size = img.size();
	size.scale(IMPORTED_IMAGESIZE, Qt::AspectRatioMode::KeepAspectRatio);

	QTextImageFormat imgFmt;
	imgFmt.setName(imgName);
	imgFmt.setWidth(size.width());
	imgFmt.setHeight(size.height());

	textCursor().beginEditBlock();
	textCursor().insertImage(imgFmt);
	textCursor().insertText(" ");
	textCursor().endEditBlock();

	releaseSelection();
	this->setFocus();
}

void ReportTextEdit::insertTable()
{
	TableDialog dlg(this, Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

	if (dlg.exec() == QDialog::Accepted)
	{
		QTextTable* table = textCursor().insertTable(dlg.getRow(), dlg.getColumn(), dlg.getTableFormat());

		textCursor().beginEditBlock();
		//table cell color init
		for (int r = 0; r < table->rows(); ++r)
		{
			for (int c = 0; c < table->columns(); ++c)
			{
				QTextCharFormat fmt;
				fmt.setBackground(QBrush(QColor(Qt::white)));
				table->cellAt(r, c).setFormat(fmt);
			}
		}
		textCursor().endEditBlock();
	}
}

void ReportTextEdit::mergeCell()
{
	QTextTable* table = textCursor().currentTable();
	if (table)
	{
		int firstRow = 0, numRow = 0, firstCol = 0, numCol = 0;
		textCursor().selectedTableCells(&firstRow, &numRow, &firstCol, &numCol);
		table->mergeCells(firstRow, firstCol, numRow, numCol);
	}
}

void ReportTextEdit::splitCell()
{
	QTextTable* table = textCursor().currentTable();
	int fr, nr, fc, nc;
	textCursor().selectedTableCells(&fr, &nr, &fc, &nc);

	if (fr >= 0 || fc >= 0)
	{
		QMessageBox::warning(nullptr, "Failed", "Only one cell should be selected.");
		return;
	}
	if (nullptr == table) return;

	QTextTableCell cell = table->cellAt(textCursor());

	int selectedRow = cell.row();
	int selectedCol = cell.column();

	if (selectedRow < 0 || selectedCol < 0)
		return;

	int rowSpan = cell.rowSpan();
	int columnSpan = cell.columnSpan();

	if (rowSpan <= 1 && columnSpan <= 1)
	{
		QMessageBox::warning(nullptr, "Failed", "It is only possible to split cells that span multiple rows or columns,\n"
			"such as rows that have been merged using merge cells.");
		return;
	}

	rowSpan = std::max(1, rowSpan - 1);
	columnSpan = std::max(1, columnSpan - 1);

	table->splitCell(selectedRow, selectedCol, rowSpan, columnSpan);
}

void ReportTextEdit::insertRow()
{
	QTextTable* table = textCursor().currentTable();

	if (nullptr == table) return;

	QTextTableCell cell = table->cellAt(textCursor());

	int selectedRow = cell.row();

	if (selectedRow < 0)
		return;

	table->insertRows(selectedRow + 1, 1);
}

void ReportTextEdit::insertCol()
{
	QTextTable* table = textCursor().currentTable();

	if (nullptr == table) return;

	QTextTableCell cell = table->cellAt(textCursor());

	int selectedCol = cell.column();

	if (selectedCol < 0)
		return;

	table->insertColumns(selectedCol + 1, 1);
}

void ReportTextEdit::openCellProperty()
{
	int firstRow = 0, numRow = 0, firstCol = 0, numCol = 0;
	textCursor().selectedTableCells(&firstRow, &numRow, &firstCol, &numCol);

	if (firstRow < 0 && firstCol < 0)
	{
		QTextTable* table = textCursor().currentTable();
		if (table)
		{
			QTextTableCell cell = table->cellAt(textCursor());
			QColor cellColor = cell.format().background().color();

			TableCellDialog dlg(cellColor, this, Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
			if (dlg.exec() == QDialog::Accepted)
			{
				cell.setFormat(dlg.getTableCellFormat());
			}
		}
	}
	else
	{
		QTextTable* table = textCursor().currentTable();

		if (nullptr == table) return;

		TableCellDialog dlg(Qt::white, this, Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

		if (dlg.exec() == QDialog::Accepted)
		{
			textCursor().beginEditBlock();
			for (int r = firstRow; r < firstRow + numRow; ++r)
			{
				for (int c = firstCol; c < firstCol + numCol; ++c)
				{
					QTextTableCell cell = table->cellAt(r, c);
					cell.setFormat(dlg.getTableCellFormat());
					QTextCharFormat fmt;
					fmt.setBackground(cell.format().background());
					mergeFormatOnWordOrSelection(fmt);
				}
			}
			textCursor().endEditBlock();
		}
	}
}

bool ReportTextEdit::loadFromDirName(const QString& dirName)
{
	QString strDir = tr("");
	if (!dirName.compare(PRODUCT_NAME_DEEPCATCH) || !dirName.compare(PRODUCT_NAME_DEEPCATCH_V2))
		strDir = STRING_MANAGER->report_format + "/" + dirName + "/" + m_strCurLoadedReportSubDirName;
	else
		strDir = STRING_MANAGER->report_format + "/" + dirName;

	QDir dir(strDir);
	dir.setFilter((QDir::Files | QDir::NoSymLinks));

	QStringList nameFilters;
	nameFilters << "*.html";
	dir.setNameFilters(nameFilters);

	if (dir.isEmpty()) return false;

	QFileInfoList htmlList = dir.entryInfoList();
	if (htmlList.size() <= 0) return false;

	releaseSelection();

	QString fileName = dir.path() + "/" + htmlList.at(0).fileName();

#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER
	bool bDOCTORANSWER_CHART_FUNCTION = true;
	if (bDOCTORANSWER_CHART_FUNCTION &&
		(!LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH) || !LICENSE_DATA->getProductType().compare(PRODUCT_NAME_DEEPCATCH_V2)))
	{
		QString strName = getHtmlFileName(htmlList);
		fileName = dir.path() + "/" + strName;
	}
#endif
		

	return load(fileName);
}

void ReportTextEdit::setPatientData(const QString& formatType)
{
	if ("Texture_Feature" == formatType)
	{
		setTextureFeatureUserData();
		document()->setModified(false);
	}
}

void ReportTextEdit::setTextureFeatureUserData()
{
	QTextCursor cursor(document());
#if 0
	const int patientDataTablePos = 4;
	cursor.setPosition(patientDataTablePos);
	QTextTable* patientDataTable = cursor.currentTable();
	if (nullptr == patientDataTable) return;
#else
	cursor.movePosition(QTextCursor::Start);
	QTextTable* preTable = nullptr;
	QTextTable* curTable = nullptr;
	int count = 0;
	while (cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 1))
	{
		curTable = cursor.currentTable();
		if (curTable && curTable != preTable)
		{
			preTable = curTable;
			count++;
			if (count == 3)
				break;
		}
	}
	QTextTable* patientDataTable = curTable;
#endif

	const int row = 2, column = 3;
	QString tableStr[row][column] = { 0 };
	tableStr[0][0] = QString::fromLocal8Bit(WIN_MANAGER->GetDicomInfo()->patientId_.c_str());
	tableStr[0][1] = QString::fromLocal8Bit(WIN_MANAGER->GetDicomInfo()->age_.c_str());
	tableStr[0][2] = QString::fromLocal8Bit(WIN_MANAGER->GetDicomInfo()->sex_.c_str());
	tableStr[1][0] = QString::fromLocal8Bit(WIN_MANAGER->GetDicomInfo()->patientsName_.c_str());
	tableStr[1][2] = QString::fromLocal8Bit(WIN_MANAGER->GetDicomInfo()->seriesDate.c_str());

	textCursor().beginEditBlock();
	for (int r = 0; r < row; ++r)
	{
		for (int c = 0; c < column; ++c)
		{
			QTextTableCell cell = patientDataTable->cellAt(r, c);
			cursor.setPosition(cell.firstPosition());
			cursor.movePosition(QTextCursor::NextBlock);
			insertTextAfterRemove(cursor.position(), cell.lastPosition(), tableStr[r][c]);
		}
	}
	textCursor().endEditBlock();
}

void ReportTextEdit::setTextureFeatureData()
{
	mip::TA::TextureFeatureValues* textureFeatureVals = ACTION_MANAGER->getTextureFeatureVals();

	QTextCursor cursor(document());
	cursor.setPosition(m_selectedCellPos.second);
	QTextTable* table = cursor.currentTable();

	if (nullptr == table) return;

	int rowNum = 0;
	for (int i = 1; i < table->rows(); ++i)
	{
		rowNum = i;
		if (table->cellAt(i, 2).lastCursorPosition().block().text().isEmpty())
			break;
	}

	textCursor().beginEditBlock();
#if 0
	cursor.setPosition(table->cellAt(rowNum, 0).firstPosition());
	cursor.setPosition(table->cellAt(rowNum, 0).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	QTextCharFormat fmt;
	fmt.setFontWeight(QFont::Bold);
	table->cellAt(rowNum, 0).lastCursorPosition().insertText(m_textureFeatureLayerName, fmt);
	table->cellAt(rowNum, 2).lastCursorPosition().insertText(QString::number(textureFeatureVals->count));
	table->cellAt(rowNum, 4).lastCursorPosition().insertText(QString::number(textureFeatureVals->volume));
	table->cellAt(rowNum, 6).lastCursorPosition().insertText(QString::number(textureFeatureVals->avg));
	table->cellAt(rowNum, 8).lastCursorPosition().insertText(QString::number(textureFeatureVals->min_value));
	table->cellAt(rowNum, 10).lastCursorPosition().insertText(QString::number(textureFeatureVals->max_value));
	table->cellAt(rowNum, 12).lastCursorPosition().insertText(QString::number(textureFeatureVals->stdev));
	table->cellAt(rowNum, 14).lastCursorPosition().insertText(QString::number(textureFeatureVals->var));
	table->cellAt(rowNum, 16).lastCursorPosition().insertText(QString::number(textureFeatureVals->entropy));
#else
	// 텍스트 입력시 스타일 적용.
	QString startTag = "<p align=\"center\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-family:" "'Arial'" "; font-size:8pt; font-weight:normal; color:#535353;\">";
	QString endTag = "</span></p>";

	cursor.setPosition(table->cellAt(rowNum, 0).firstPosition());
	cursor.setPosition(table->cellAt(rowNum, 0).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();

	QString strTemp = startTag + m_textureFeatureLayerName + endTag;
	table->cellAt(rowNum, 0).lastCursorPosition().insertHtml(strTemp);

	strTemp = startTag + QString::number(textureFeatureVals->count) + endTag;
	table->cellAt(rowNum, 2).lastCursorPosition().insertHtml(strTemp);

	/* 부동 소수점 포함 10자리까지 표기한다. */
	strTemp = startTag + QString::number(textureFeatureVals->volume, 'g', 10) + endTag;
	table->cellAt(rowNum, 4).lastCursorPosition().insertHtml(strTemp);

	strTemp = startTag + QString::number(textureFeatureVals->avg, 'g', 10) + endTag;
	table->cellAt(rowNum, 6).lastCursorPosition().insertHtml(strTemp);

	strTemp = startTag + QString::number(textureFeatureVals->min_value) + endTag;
	table->cellAt(rowNum, 8).lastCursorPosition().insertHtml(strTemp);

	strTemp = startTag + QString::number(textureFeatureVals->max_value) + endTag;
	table->cellAt(rowNum, 10).lastCursorPosition().insertHtml(strTemp);

	strTemp = startTag + QString::number(textureFeatureVals->stdev, 'g', 10) + endTag;
	table->cellAt(rowNum, 12).lastCursorPosition().insertHtml(strTemp);

	strTemp = startTag + QString::number(textureFeatureVals->var, 'g', 10) + endTag;
	table->cellAt(rowNum, 14).lastCursorPosition().insertHtml(strTemp);

	strTemp = startTag + QString::number(textureFeatureVals->entropy, 'g', 10) + endTag;
	table->cellAt(rowNum, 16).lastCursorPosition().insertHtml(strTemp);
#endif
	textCursor().endEditBlock();

	m_selectedCellPos.first = false;
	m_selectedCellPos.second = -1;
}

void ReportTextEdit::setCOVID19AnalysisData(void)
{
	COVID_REPORT *pReport = ACTION_MANAGER->getCOVID19AnalysisVals();

	// insert texture feature
	if (document()->isEmpty())
	{
		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_WARN),
			QString("Not exist COVID19 report format."));
		return;
	}	

	QTextCursor cursor(document());
	
	cursor.movePosition(QTextCursor::Start);
	int nTableIdx = 0;	
	QTextTable* table = nullptr;
	QTextTable* preTable = nullptr;

	while (1)
	{
		bool ret = cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 1);
		int pos = cursor.position();
		table = cursor.currentTable();
		QString str;

		if (table != nullptr && preTable != table)
		{
			preTable = table;
			nTableIdx++;

			if (nTableIdx == 1)	// 첫번째 테이블
				break;
		}

		if (pos > 700)
			break;
	}

	if (nullptr == table)
		return;
		
	textCursor().beginEditBlock();

	int nStrLength = 10;
	// patient name	
	cursor.setPosition(table->cellAt(0, 1).firstPosition());
	cursor.setPosition(table->cellAt(0, 1).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	std::string strTmp = WIN_MANAGER->GetDicomInfo()->patientsName_.substr(0, nStrLength);
	table->cellAt(0, 1).lastCursorPosition().insertHtml(m_strHtml_1 + QString::fromStdString(strTmp) + m_strHtml_2);

	// Age
	cursor.setPosition(table->cellAt(0, 3).firstPosition());
	cursor.setPosition(table->cellAt(0, 3).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTmp = WIN_MANAGER->GetDicomInfo()->age_.substr(0, nStrLength);
	table->cellAt(0, 3).lastCursorPosition().insertHtml(m_strHtml_1 + QString::fromStdString(strTmp) + m_strHtml_2);

	// Gender
	cursor.setPosition(table->cellAt(0, 5).firstPosition());
	cursor.setPosition(table->cellAt(0, 5).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTmp = WIN_MANAGER->GetDicomInfo()->sex_.substr(0, nStrLength);
	table->cellAt(0, 5).lastCursorPosition().insertHtml(m_strHtml_1 + QString::fromStdString(strTmp) + m_strHtml_2);

	// Patient ID
	cursor.setPosition(table->cellAt(1, 1).firstPosition());
	cursor.setPosition(table->cellAt(1, 1).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTmp = WIN_MANAGER->GetDicomInfo()->patientId_.substr(0, nStrLength);
	table->cellAt(1, 1).lastCursorPosition().insertHtml(m_strHtml_1 + QString::fromStdString(strTmp) + m_strHtml_2);
	
	// Modality
	cursor.setPosition(table->cellAt(1, 3).firstPosition());
	cursor.setPosition(table->cellAt(1, 3).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTmp = WIN_MANAGER->GetDicomInfo()->modality_.substr(0, nStrLength);
	table->cellAt(1, 3).lastCursorPosition().insertHtml(m_strHtml_1+ QString::fromStdString(strTmp) + m_strHtml_2);

	// Study Date
	cursor.setPosition(table->cellAt(1, 5).firstPosition());
	cursor.setPosition(table->cellAt(1, 5).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTmp = WIN_MANAGER->GetDicomInfo()->studyDate_.substr(0, nStrLength);
	table->cellAt(1, 5).lastCursorPosition().insertHtml(m_strHtml_1 + QString::fromStdString(strTmp) + m_strHtml_2);

	// Radiologist
	cursor.setPosition(table->cellAt(2, 1).firstPosition());
	cursor.setPosition(table->cellAt(2, 1).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	table->cellAt(2, 1).lastCursorPosition().insertHtml(m_strHtml_1 + QString("-") + m_strHtml_2);

	// Reviewer
	cursor.setPosition(table->cellAt(2, 3).firstPosition());
	cursor.setPosition(table->cellAt(2, 3).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	table->cellAt(2, 3).lastCursorPosition().insertHtml(m_strHtml_1 + QString("-") + m_strHtml_2);

	// Report Data
	QDateTime date;
	QString strTime;
	if (network::GetServerTime(date))
	{
		//str = date.toString("yyyy-MM-dd HH:mm:ss");
		strTime = date.toString("yyyy-MM-dd");
	}
	//table->cellAt(3, 5).
	cursor.setPosition(table->cellAt(2, 5).firstPosition());
	cursor.setPosition(table->cellAt(2, 5).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();		
	table->cellAt(2, 5).lastCursorPosition().insertHtml(m_strHtml_1+ strTime+ m_strHtml_2);
	
	textCursor().endEditBlock();
	///////////////////////////
	table = nullptr;
	preTable = nullptr;
	nTableIdx = 0;
	cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
	while (1)
	{
		bool ret = cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 1);
		int pos = cursor.position();
		table = cursor.currentTable();
		QString str;

		if (table != nullptr && preTable != table)
		{
			preTable = table;
			nTableIdx++;

			if (nTableIdx == 2)	// 두번째 테이블
				break;
		}
			
		if (pos > 700)
			break;
	}

	if (nullptr == table) 
		return;

	textCursor().beginEditBlock();
// 	cursor.setPosition(table->cellAt(1, 0).firstPosition());
// 	cursor.setPosition(table->cellAt(1, 0).lastPosition(), QTextCursor::KeepAnchor);
// 	cursor.removeSelectedText();
// 	QTextCharFormat fmt;
// 	fmt.setFontWeight(QFont::Bold);
		
	// 목록 이름 
// 	for (int i = 1; i < 6; ++i)
// 	{
// 		QTextTableCell cell = table->cellAt(0, i);
// 		QTextCharFormat cellFormat = cell.format();
// 		cellFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
// 		cell.setFormat(cellFormat);
// 	}
	
	// insert 값
	QTextBlockFormat textBlockFormat;
	int nFeatureCnt = pReport->vecFeature.size();	
	for (int i = 0; i < nFeatureCnt; ++i)
	{
		// roi name
		cursor.setPosition(table->cellAt(i + 1, 0).firstPosition());
		cursor.setPosition(table->cellAt(i + 1, 0).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		table->cellAt(i + 1, 0).lastCursorPosition().insertHtml(m_strHtml_1 + QString::fromStdString(pReport->vecFeature[i].strROIName) + m_strHtml_2);
		
		// volume
		cursor.setPosition(table->cellAt(i + 1, 1).firstPosition());
		cursor.setPosition(table->cellAt(i + 1, 1).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();	
		textBlockFormat = cursor.blockFormat();		
		textBlockFormat.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);		
		cursor.mergeBlockFormat(textBlockFormat);
		table->cellAt(i + 1, 1).lastCursorPosition().insertHtml(m_strHtml_1 + QString::number(pReport->vecFeature[i].fObjectVolume, 'f', 1) + m_strHtml_2);
		
		// mean hu
		cursor.setPosition(table->cellAt(i + 1, 2).firstPosition());
		cursor.setPosition(table->cellAt(i + 1, 2).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		textBlockFormat = cursor.blockFormat();
		textBlockFormat.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		cursor.mergeBlockFormat(textBlockFormat);
		table->cellAt(i + 1, 2).lastCursorPosition().insertHtml(m_strHtml_1 + QString::number(pReport->vecFeature[i].fMeanHU, 'f', 1) + m_strHtml_2);

		// std deviation
		cursor.setPosition(table->cellAt(i + 1, 3).firstPosition());
		cursor.setPosition(table->cellAt(i + 1, 3).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		textBlockFormat = cursor.blockFormat();
		textBlockFormat.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		cursor.mergeBlockFormat(textBlockFormat);
		table->cellAt(i + 1, 3).lastCursorPosition().insertHtml(m_strHtml_1 + QString::number(pReport->vecFeature[i].fStdDeviation, 'f', 1) + m_strHtml_2);
		
		// pneumonia
		cursor.setPosition(table->cellAt(i + 1, 4).firstPosition());
		cursor.setPosition(table->cellAt(i + 1, 4).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();		
		textBlockFormat = cursor.blockFormat();
		textBlockFormat.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		cursor.mergeBlockFormat(textBlockFormat);
				
		if (i == 0)
			table->cellAt(i + 1, 4).lastCursorPosition().insertHtml(QString("-"));
		else
			table->cellAt(i + 1, 4).lastCursorPosition().insertHtml(m_strHtml_1 + QString::number(pReport->vecFeature[i].fPneumoniaBurden, 'f', 1) + m_strHtml_2);

		// extent
		cursor.setPosition(table->cellAt(i + 1, 5).firstPosition());
		cursor.setPosition(table->cellAt(i + 1, 5).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		textBlockFormat = cursor.blockFormat();
		textBlockFormat.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		cursor.mergeBlockFormat(textBlockFormat);
				
		float fRatio = round(pReport->vecFeature[i].fRatio*10)/10;
		float fRatioPercent = pReport->vecFeature[i].fRatio * 100.f;

		if (i == 0)
		{
			table->cellAt(i + 1, 5).lastCursorPosition().insertHtml(m_strHtml_1 + QString("-") + m_strHtml_2);
		}
		else
		{			
			table->cellAt(i + 1, 5).lastCursorPosition().insertHtml(m_strHtml_1 + QString("%1%").arg(QString::number(fRatioPercent, 'f', 1))
				+ QString(" (%1)").arg(QString::number(fRatio, 'f', 1)) + m_strHtml_2);
		}
	}	

	textCursor().endEditBlock();

	////////////////////////////
	table = nullptr;
	preTable = nullptr;
	nTableIdx = 0;
	cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
	while (1)
	{
		bool ret = cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 1);
		int pos = cursor.position();
		table = cursor.currentTable();
		QString str;

		if (table != nullptr && preTable != table)
		{
			preTable = table;
			nTableIdx++;

			if (nTableIdx == 3)	// 3번째 테이블
				break;
		}

		if (pos > 700)
			break;
	}

	// insert image	
	if (nullptr == table)
		return;
	
	// Axial
	QImage rectImg_1 = QImage(200, 200, QImage::Format_RGBA8888);
	QPainter paint_1(&rectImg_1);
	drawSliceCustom(&paint_1, WT_AXIAL, pReport->nAxialDepth);
	drawMaskCustom(&paint_1, pReport->nUID, WT_AXIAL, pReport->nAxialDepth);

	static int i = getImageStringNumber("COVID19_image_%1.png", 0);
	QUrl url(QString("COVID19_image_%1.png").arg(i++));

	cursor.setPosition(table->cellAt(0, 0).firstPosition());
	cursor.setPosition(table->cellAt(0, 0).lastPosition(), QTextCursor::KeepAnchor);
	textBlockFormat = cursor.blockFormat();
	textBlockFormat.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);	
	cursor.mergeBlockFormat(textBlockFormat);
	insertImage(cursor, url, rectImg_1);

	// coronal
	QImage rectImg_2 = QImage(200, 200, QImage::Format_RGBA8888);
	QPainter paint_2(&rectImg_2);
	drawSliceCustom(&paint_2, WT_CORONAL, pReport->nCoronalDepth);
	drawMaskCustom(&paint_2, pReport->nUID, WT_CORONAL, pReport->nCoronalDepth);

	i = getImageStringNumber("COVID19_image_%1.png", 0);
	url = QUrl(QString("COVID19_image_%1.png").arg(i++));

	cursor.setPosition(table->cellAt(0, 1).firstPosition());
	cursor.setPosition(table->cellAt(0, 1).lastPosition(), QTextCursor::KeepAnchor);
	textBlockFormat = cursor.blockFormat();
	textBlockFormat.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);	
	cursor.mergeBlockFormat(textBlockFormat);
	insertImage(cursor, url, rectImg_2);

	// sagittal
	QImage rectImg_3 = QImage(200, 200, QImage::Format_RGBA8888);
	QPainter paint_3(&rectImg_3);
	drawSliceCustom(&paint_3, WT_SAGITTAL, pReport->nSaggitalDepth);
	drawMaskCustom(&paint_3, pReport->nUID, WT_SAGITTAL, pReport->nSaggitalDepth);

	i = getImageStringNumber("COVID19_image_%1.png", 0);
	url = QUrl(QString("COVID19_image_%1.png").arg(i++));

	cursor.setPosition(table->cellAt(0, 2).firstPosition());
	cursor.setPosition(table->cellAt(0, 2).lastPosition(), QTextCursor::KeepAnchor);
	textBlockFormat = cursor.blockFormat();
	textBlockFormat.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);	
	cursor.mergeBlockFormat(textBlockFormat);
	insertImage(cursor, url, rectImg_3);
		
}

int ReportTextEdit::currentHTMLSize()
{
	int reportSize = 0;
	QString html = toHtml();
	int length = html.toLocal8Bit().size();
	reportSize += length * sizeof(WCHAR);
	return reportSize;
}

void ReportTextEdit::getImageResource(std::vector<std::pair<QImage, QString>>& image_list)
{
	QString htmlString = toHtml();
	std::vector<QString> imageString_list;

	imageResourcePath(htmlString, imageString_list);
	for (int i = 0; i < imageString_list.size(); ++i)
	{
		QImage img(imageString_list[i]);
		QString imgName = QFileInfo(imageString_list[i]).baseName();
		image_list.push_back(std::make_pair(img, imgName));
	}

	imageString_list.clear();
	imageResourceName(htmlString, imageString_list);
	for (int i = 0; i < imageString_list.size(); ++i)
	{
		QVariant v = document()->resource(QTextDocument::ImageResource, imageString_list[i]);
		QImage img = qvariant_cast<QImage>(v);
		QString imgName = QFileInfo(imageString_list[i]).baseName();
		image_list.push_back(std::make_pair(img, imgName));
	}
}

void ReportTextEdit::getHTML(QString& html)
{
	html = toHtml();
}

bool ReportTextEdit::loadFromMip(const QString& fileName, const std::vector<std::pair<QImage, QString>>& image_list, const QString& html)
{
	QString baseName = QFileInfo(fileName).baseName();

	QDir dir(STRING_MANAGER->report_format);

	bool success = dir.mkdir(baseName);

	int cnt = 0;
	while (false == success)
	{
		baseName = (baseName + "(%1)").arg(++cnt);
		success = dir.mkdir(baseName);
	}
	
	setCurrentFileName(STRING_MANAGER->report_format + "/" + baseName + "/" + baseName + ".html");

	QString htmlString = WIN_MANAGER->tempReportData.getHTML();
	std::vector<QString> path_list;
	imageResourcePath(htmlString, path_list);

	for (int i = 0; i < path_list.size(); ++i)
	{
		QFileInfo fi(path_list.at(i));
		QString newPath = STRING_MANAGER->report_format + "/" + baseName + "/" + fi.fileName();

		htmlString.replace(path_list.at(i), newPath);
	}
	setHtml(htmlString);

	for (int i = 0; i < WIN_MANAGER->tempReportData.getImageList().size(); ++i)
	{
		QFileInfo fi(WIN_MANAGER->tempReportData.getImageList()[i].second);		
		QString path = STRING_MANAGER->report_format + QString("/") + baseName + QString("/") + fi.baseName() + QString(".png");		
		bool res = WIN_MANAGER->tempReportData.getImageList()[i].first.save(path);
		if (false == res) return false;
	}

	QTextDocumentWriter writer(m_fileName);
	success = writer.write(document());
	if (success) {
		document()->setModified(false);
	}
	else {
		QMessageBox::warning(nullptr, "Save Failed", tr("Could not save \"%1\"").arg(QDir::toNativeSeparators(m_fileName)));
	}
	return success;
}

void ReportTextEdit::OnPrintPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
	Q_UNUSED(printer);
#else
	print(printer);
#endif
}

void ReportTextEdit::OnPageWidthLimit()
{
	if (this->size().width() < document()->documentLayout()->documentSize().width())
	{
		undo();
		releaseSelection();
	}
}

int ReportTextEdit::checkModified()
{
	if (document()->isModified())
	{
		//const QMessageBox::StandardButton ret =
		//	QMessageBox::warning(nullptr, "Report Tab",
		//		tr("The document has been modified.\n"
		//			"Do you want to save your changes?"),
		//		QMessageBox::No | QMessageBox::Save | QMessageBox::Cancel);
		//if (ret == QMessageBox::Save)
		//	return true;
		//else if (ret == QMessageBox::Cancel)
		//	return false;
		QMessageBox msgBox(QMessageBox::Warning, "modified",
			tr("The document has been modified.\n"
				"Do you want to save your changes?"),
			QMessageBox::Yes | QMessageBox::Save | QMessageBox::Cancel);

		msgBox.setButtonText(QMessageBox::Yes, trUtf8("Save As"));
		return msgBox.exec();
	}
	return 0;
}

bool ReportTextEdit::load(const QString &fileName)
{
	if (!QFile::exists(fileName))
		return false;

	QFile file(fileName);
	if (!file.open(QFile::ReadOnly))
		return false;

	QByteArray data = file.readAll();
	QTextCodec *codec = Qt::codecForHtml(data);
	QString str = codec->toUnicode(data);

	if (Qt::mightBeRichText(str)) {
		setHtml(str);
	}
	else {
		str = QString::fromLocal8Bit(data);
		setPlainText(str);
	}

	setCurrentFileName(fileName);

	if ("Texture_Feature" == QFileInfo(fileName).baseName())
	{
		QString htmlString = toHtml();
		std::vector<QString> path_list;
		imageResourcePath(htmlString, path_list);
		for (int i = 0; i < path_list.size(); ++i)
		{
			QFileInfo fi(path_list.at(i));
			if (path_list.at(i) == STRING_MANAGER->report_format + "/Texture_Feature/" + fi.fileName())
				continue;
			QString imgPath = STRING_MANAGER->report_format + "/" + QFileInfo(m_fileName).baseName() + "/" + fi.fileName();
			htmlString.replace(path_list.at(i), imgPath);
		}
		setHtml(htmlString);
	}
	else if (PRODUCT_NAME_DEEPCATCH == QFileInfo(fileName).baseName())
	{
		QString htmlString = toHtml();
		std::vector<QString> path_list;
		imageResourcePath(htmlString, path_list);
		for (int i = 0; i < path_list.size(); ++i)
		{
			QFileInfo fi(path_list.at(i));
			if (path_list.at(i) == STRING_MANAGER->report_format + "/" + PRODUCT_NAME_DEEPCATCH + "/" + m_strCurLoadedReportSubDirName + "/" + fi.fileName())
				continue;
			QString imgPath = STRING_MANAGER->report_format + "/" + QFileInfo(m_fileName).baseName() + "/" + m_strCurLoadedReportSubDirName + "/" + fi.fileName();
			htmlString.replace(path_list.at(i), imgPath);
		}
		setHtml(htmlString);
	}
	else if (DEEPCATCH_HTMLNAME_DOCTORANSWER == QFileInfo(fileName).baseName())
	{
		QString htmlString = toHtml();
		std::vector<QString> path_list;
		imageResourcePath(htmlString, path_list);
		for (int i = 0; i < path_list.size(); ++i)
		{
			QFileInfo fi(path_list.at(i));
			if (path_list.at(i) == STRING_MANAGER->report_format + "/" + PRODUCT_NAME_DEEPCATCH + "/" + m_strCurLoadedReportSubDirName + "/" + fi.fileName())
				continue;
			//QString imgPath = STRING_MANAGER->report_format + "/" + QFileInfo(m_fileName).baseName() + "/" + m_strCurLoadedReportSubDirName + "/" + fi.fileName();
			QString imgPath = STRING_MANAGER->report_format + "/" + PRODUCT_NAME_DEEPCATCH + "/" + m_strCurLoadedReportSubDirName + "/" + fi.fileName();
			htmlString.replace(path_list.at(i), imgPath);
		}
		setHtml(htmlString);
	}
	else if (PRODUCT_NAME_MEDIP_COVID19 == QFileInfo(fileName).baseName())
	{
		QString htmlString = toHtml();
		std::vector<QString> path_list;
		imageResourcePath(htmlString, path_list);
		for (int i = 0; i < path_list.size(); ++i)
		{
			QFileInfo fi(path_list.at(i));
			if (path_list.at(i) == STRING_MANAGER->report_format + "/" + PRODUCT_NAME_MEDIP_COVID19 + "/" + fi.fileName())
				continue;
			QString imgPath = STRING_MANAGER->report_format + "/" + QFileInfo(m_fileName).baseName() + "/" + fi.fileName();
			htmlString.replace(path_list.at(i), imgPath);
		}
		setHtml(htmlString);
		
		///////////////////////
		QTextCursor newCursor(document());
		while (!newCursor.isNull() && !newCursor.atEnd())
		{
			newCursor = document()->find(QString("v0.0.0.0"), newCursor);

			if (!newCursor.isNull())
			{
				newCursor.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);

				newCursor.removeSelectedText();
				newCursor.insertHtml(m_strHtml_1 + QString("&nbsp;v") + QString(VER_FILE_VERSION_STR_NO_BUILD) + m_strHtml_2);
				
			}
		}

		///////////////////////////
		QTextTable* table = nullptr;
		QTextTable* preTable = nullptr;
				
		int nTableIdx = 0;
		QTextCursor cursor(document());
		cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
		while (1)
		{
			bool ret = cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 1);
			int pos = cursor.position();
			table = cursor.currentTable();
			QString str;

			if (table != nullptr && preTable != table)
			{
				preTable = table;
				nTableIdx++;

				if (nTableIdx == 2)	// 두번째 테이블
					break;
			}

			if (pos > 700)
				break;
		}

		if (nullptr != table)
		{
			textCursor().beginEditBlock();

			// 목록 이름 
			for (int i = 1; i < 6; ++i)
			{
				QTextTableCell cell = table->cellAt(0, i);
				QTextCharFormat cellFormat = cell.format();
				cellFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
				cell.setFormat(cellFormat);
			}
			textCursor().endEditBlock();
		}
	}

	setPatientData(QFileInfo(fileName).baseName());

	return true;
}

void ReportTextEdit::setCurrentFileName(const QString &fileName)
{
	m_fileName = fileName;
	document()->setModified(false);

	QString shownName;
	if (fileName.isEmpty())
		shownName = "untitled.txt";
	else
		shownName = QFileInfo(fileName).fileName();

	setWindowModified(false);
}

void ReportTextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
	QTextCursor cursor = textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
	mergeCurrentCharFormat(format);
}

void ReportTextEdit::dropImage(const QUrl& url, const QImage& image)
{
	if (image.isNull()) return;

	document()->addResource(QTextDocument::ImageResource, url, image);

	QSize imgSize = image.size();
	if (m_imageOverwrite.first)
		imgSize.scale(m_imageOverwrite.second.size, Qt::KeepAspectRatio);
	else
		imgSize.scale(IMPORTED_IMAGESIZE, Qt::AspectRatioMode::KeepAspectRatio);

	QTextImageFormat imgFmt;
	imgFmt.setName(url.url());
	imgFmt.setWidth(imgSize.width());
	imgFmt.setHeight(imgSize.height());

	if (m_imageOverwrite.first)
	{
		QTextCursor editCursor(document());
		editCursor.setPosition(m_imageOverwrite.second.cursorPos);
		editCursor.setPosition(m_imageOverwrite.second.fragmentPos);
		editCursor.setPosition(m_imageOverwrite.second.fragmentPos + m_imageOverwrite.second.fragmentLen, QTextCursor::KeepAnchor);
		editCursor.setCharFormat(imgFmt);
		releaseSelection();
	}
	else
	{
		textCursor().beginEditBlock();
		textCursor().insertImage(imgFmt);
		textCursor().insertText(" ");
		textCursor().endEditBlock();
	}

	m_imageOverwrite = std::make_pair(false, OverwriteImageFormat());
	releaseSelection();
	this->setFocus();
}

void ReportTextEdit::insertImage(QTextCursor &cursor, const QUrl& url, const QImage& image)
{
	if (image.isNull()) return;

	document()->addResource(QTextDocument::ImageResource, url, image);

	QSize imgSize = image.size();
	if (m_imageOverwrite.first)
		imgSize.scale(m_imageOverwrite.second.size, Qt::KeepAspectRatio);
	else
		imgSize.scale(IMPORTED_IMAGESIZE, Qt::AspectRatioMode::KeepAspectRatio);

	QTextImageFormat imgFmt;
	imgFmt.setName(url.url());
	imgFmt.setWidth(imgSize.width());
	imgFmt.setHeight(imgSize.height());

	if (m_imageOverwrite.first)
	{		
// 		cursor.setPosition(m_imageOverwrite.second.cursorPos);
// 		cursor.setPosition(m_imageOverwrite.second.fragmentPos);
// 		cursor.setPosition(m_imageOverwrite.second.fragmentPos + m_imageOverwrite.second.fragmentLen, QTextCursor::KeepAnchor);		
//		cursor.setPosition(534);
		cursor.setCharFormat(imgFmt);
		releaseSelection();
	}
	else
	{
		textCursor().beginEditBlock();		
		cursor.insertImage(imgFmt);
		cursor.insertText(" ");
		textCursor().endEditBlock();
	}

	m_imageOverwrite = std::make_pair(false, OverwriteImageFormat());
	releaseSelection();
	this->setFocus();
}

void ReportTextEdit::dropTextFile(const QUrl& url)
{
	QFile file(url.toLocalFile());
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		textCursor().insertText(file.readAll());
}

void ReportTextEdit::drawPageBoundaryLine(QPainter& painter)
{
	const int nMargin = 10;

	int vScrollBarVal = verticalScrollBar()->value() % static_cast<int>(document()->pageSize().height());
	int boundaryLineY = document()->pageSize().height() - nMargin - vScrollBarVal;

	QPoint emptyPos = QPoint(0, boundaryLineY);
	QSize emptySize = document()->pageSize().toSize();
	emptySize.rheight() = nMargin;
	painter.fillRect(QRect(emptyPos, emptySize), QColor(48, 48, 48, 255));

	emptyPos = QPoint(0, boundaryLineY + static_cast<int>(document()->pageSize().height()));
	painter.fillRect(QRect(emptyPos, emptySize), QColor(48, 48, 48, 255));
}

bool ReportTextEdit::isImageFormat(const QPoint& mousePos, int& cursorPos)
{
	QPoint layoutPos = mousePos;
	layoutPos.ry() += verticalScrollBar()->value();
	cursorPos = document()->documentLayout()->hitTest(layoutPos, Qt::ExactHit);

	if (cursorPos < 0) return false;

	QTextCursor cursor(document());
	cursor.setPosition(cursorPos);
	if (cursor.atEnd()) return false;

	cursorPos += 1;
	cursor.setPosition(cursorPos);
	QTextFormat format = cursor.charFormat();
	if (!format.isImageFormat()) return false;

	return true;
}

bool ReportTextEdit::isTableFormat(const QPoint& mousePos, int& cursorPos)
{
	QPoint layoutPos = mousePos;
	layoutPos.ry() += verticalScrollBar()->value();
	cursorPos = document()->documentLayout()->hitTest(layoutPos, Qt::FuzzyHit);

	if (cursorPos < 0) return false;

	QTextCursor cursor(document());
	cursor.setPosition(cursorPos);
	if (cursor.atEnd()) return false;

	cursorPos += 1;
	cursor.setPosition(cursorPos);
	QTextFormat format = cursor.charFormat();

//	int type = format.type();
//	int objectType = format.objectType();

	if (!format.isTableCellFormat()) return false;

	return true;
}


void ReportTextEdit::setSeletedIamgeFormat(int imgCursorPos)
{
	QTextCursor cursor = textCursor();
	cursor.setPosition(imgCursorPos);

	int fragmentPos = 0;
	int fragmentLen = 0;
	QTextImageFormat imgfmt;
	findImageFormat(cursor.block(), imgfmt, imgCursorPos, fragmentPos, fragmentLen);

	QRect rect = cursorRect(cursor);
	QPoint imgPos(viewport()->mapToParent(rect.bottomLeft()));
	QSize imgSize(imgfmt.width(), imgfmt.height());
	imgPos.ry() -= imgSize.height() + 1;
	imgPos.ry() = std::max(imgPos.ry(), rect.topLeft().y());

	m_selectedImgFmt.setSelectedFormat(QRect(imgPos, imgSize), imgfmt.name(), fragmentPos, fragmentLen);
}

bool ReportTextEdit::findImageFormat(QTextBlock& block, QTextImageFormat& imgFmt, int imageCursorPos, int& fragmentPos, int& fragmentLen)
{
	QTextBlock::iterator it;

	for (it = block.begin(); !(it.atEnd()); ++it)
	{
		QTextFragment fragment = it.fragment();
		
		if (fragment.isValid())
		{
			if (fragment.charFormat().isImageFormat() && imageCursorPos == fragment.position())
			{
				fragmentPos = fragment.position();
				fragmentLen = fragment.length();
				imgFmt = fragment.charFormat().toImageFormat();
				return true;
			}
		}
	}

	return false;
}

void ReportTextEdit::updateResizingPointer(const QSizeF& newSize)
{
	QTextCursor cursor = textCursor();
	cursor.setPosition(m_selectedImgFmt.getFragmentPos());
	QRect rect = cursorRect(cursor);
	QPointF imgPos(m_selectedImgFmt.rect().topLeft());
	//QPointF imgPos(viewport()->mapToParent(rect.bottomLeft()));
	//imgPos.ry() -= newSize.height() + 1;
	//imgPos.ry() = std::max(imgPos.ry(), static_cast<qreal>(rect.topLeft().y()));

	//QTextBlock::iterator it;
	//QTextBlock block = textCursor().block();
	//int maxHeight = 0;
	//for (it = block.begin(); !(it.atEnd()); ++it)
	//{
	//	QTextFragment fragment = it.fragment();

	//	if (fragment.isValid())
	//	{
	//		if (fragment.charFormat().isImageFormat() && m_selectedImgFmt.getFragmentPos() != fragment.position())
	//		{
	//			int imgHeight = fragment.charFormat().toImageFormat().height();

	//			if (maxHeight < imgHeight)
	//			{
	//				maxHeight = imgHeight;
	//			}
	//		}
	//	}
	//}

	//if (maxHeight <= newSize.height())
	//{
	//	imgPos.ry() = rect.topLeft().y();
	//}

	QRectF imgFmtRect(imgPos, newSize);

	m_selectedImgFmt.setImageRect(imgFmtRect);
	m_selectedImgFmt.updateResizingPointer();
}

void ReportTextEdit::resizingImgFormat(const QPoint& pos, ImageResizingPointer pointer)
{
	if (false == m_selectedImgFmt.resizingFlag()) return;

	QSizeF newSize(m_selectedImgFmt.size());
	QSizeF prevSize(m_selectedImgFmt.size());

	int dx = pos.x() - clickedPos.x();
	int dy = pos.y() - clickedPos.y();

	int sign = 1;
	if (0 != dx)
	{
		sign = dx / std::abs(dx);
	}
	int resizeRate = (std::abs(dx) > std::abs(dy)) ? (sign * std::abs(dx)) : (sign * std::abs(dy));

	switch (pointer)
	{
	case ImageResizingPointer::TopLeft:
	case ImageResizingPointer::BotLeft:
		newSize = newSize.scaled(QSizeF(prevSize.width() - resizeRate, prevSize.height() - resizeRate), Qt::KeepAspectRatio);
		break;
	case ImageResizingPointer::BotRight:
	case ImageResizingPointer::TopRight:
		newSize = newSize.scaled(QSizeF(prevSize.width() + resizeRate, prevSize.height() + resizeRate), Qt::KeepAspectRatio);
		break;
	case ImageResizingPointer::TopMid:
		newSize.rheight() -= dy;
		break;
	case ImageResizingPointer::BotMid:
		newSize.rheight() += dy;
		break;
	case ImageResizingPointer::MidLeft:
		newSize.rwidth() -= dx;
		break;
	case ImageResizingPointer::MidRight:
		newSize.rwidth() += dx;
		break;
	default:
		break;
	}

	clickedPos = pos;

	if (newSize.width() < IMAGE_MIN_SIZE.width() || newSize.height() < IMAGE_MIN_SIZE.height())
		return;

	QSize pageSize = document()->pageSize().toSize();
	int margin = (int)document()->documentMargin();
	if (newSize.width() > pageSize.width() - margin * 2 || newSize.height() > pageSize.height() - margin * 2)
		return;

	updateResizingPointer(newSize);
}

void ReportTextEdit::mouseOver(bool over)
{
	m_bImgOverwriteFlag = over;
	update();
}

QSize ReportTextEdit::convertSizeToEditArea(const QSize& size)
{
	QSize editArea(document()->pageSize().toSize());
	editArea.rwidth() = std::max((editArea.width() - (int)document()->documentMargin() * 2), 1);
	editArea.rheight() = std::max((editArea.height() - (int)document()->documentMargin() * 2), 1);

	if (editArea.width() < size.width() || editArea.height() < size.height())
	{
		return editArea;
	}
	return size;
}

void ReportTextEdit::imageResourcePath(const QString& htmlString, std::vector<QString>& path_list)
{
	QString imgPathKey = "<img src=\"file:///";

	int index = 0;
	while (index >= 0)
	{
		index = htmlString.indexOf(imgPathKey, index);
		if (index == -1)
		{
			break;
		}
		else
		{
			QChar c = 0;
			QString path;
			index += imgPathKey.size();
			while ((c = htmlString.at(index++)) != '\"')
			{
				path.push_back(c);
			}
			path_list.push_back(path);
		}
	}
}

void ReportTextEdit::imageResourceName(const QString& htmlString, std::vector<QString>& name_list)
{
	QString imgUrl = "<img src=\"";
	std::set<QString> resource_list;

	int index = 0;
	while (index >= 0)
	{
		index = htmlString.indexOf(imgUrl, index);
		if (index == -1)
		{
			break;
		}
		else
		{
			QChar c = 0;
			QString path;
			index += imgUrl.size();
			while ((c = htmlString.at(index++)) != '\"')
			{
				path.push_back(c);
			}
			if (path.contains("file:///")) continue;
			resource_list.insert(path);
		}
	}

	for (auto iter : resource_list)
	{
		name_list.push_back(iter);
	}
}

void ReportTextEdit::releaseSelection()
{
	m_selectedImgFmt.releaseSelection();
	update();
}

bool ReportTextEdit::copyImageToReportDirectory()
{
	QString htmlString = toHtml();
	std::vector<QString> path_list;
	imageResourcePath(htmlString, path_list);

	for (int i = 0; i < path_list.size(); ++i)
	{
		QFileInfo fi(path_list.at(i));
		QString newPath = STRING_MANAGER->report_format + "/" + QFileInfo(m_fileName).baseName() + "/" + fi.fileName();

		QImage img(path_list[i]);
		QFile::remove(newPath);
		bool res = img.save(newPath);
		if (false == res) return false;
		htmlString.replace(path_list.at(i), newPath);
	}
	setHtml(htmlString);
	return true;
}

bool ReportTextEdit::resourceCopyToReportDirectory()
{
	QString htmlString = toHtml();
	std::vector<QString> resource_list;
	imageResourceName(htmlString, resource_list);

	for (auto resource_name : resource_list)
	{
		QVariant v = document()->resource(QTextDocument::ImageResource, resource_name);
		QImage img = qvariant_cast<QImage>(v);
		QString savePath = STRING_MANAGER->report_format + "/" + QFileInfo(m_fileName).baseName() + "/" + resource_name;
		QSize s = img.size();
		QFile::remove(savePath);
		bool res = img.save(savePath);
		if (false == res) return false;
		savePath = "file:///" + savePath;
		htmlString.replace(resource_name, savePath);
	}
	setHtml(htmlString);
	return true;
}

bool ReportTextEdit::checkMEDIPBasicFormat(const QString& fileName)
{
	for (auto iter : STRING_MANAGER->report_basic_formats)
	{
		if (iter == fileName)
			return true;
	}
	return false;
}

bool ReportTextEdit::checkSpecialCharacters(const QString& str)
{
	QRegExp reg1("^[a-zA-Z0-9_ ]*$");
	return str.contains(reg1);
}

bool ReportTextEdit::canTextureFeatureMime(int cursorPos)
{
	QTextCursor cursor(document());
	cursor.setPosition(cursorPos);
	QTextTable* table = cursor.currentTable();
	if (nullptr == table) return false;
	const int textureFeatureCellCnt = 17;
	return textureFeatureCellCnt == table->columns();
}

void ReportTextEdit::insertTextAfterRemove(int firstCursorPos, int secondCursorPos, const QString& str)
{
	if (str.length() < 1) return;

	QTextCursor cursor = textCursor();
	cursor.setPosition(firstCursorPos);
	cursor.setPosition(secondCursorPos, QTextCursor::KeepAnchor);
	setTextCursor(cursor);
	textCursor().removeSelectedText();
#if 0
	textCursor().insertText(str);
#else
	QString startTag = "<span style=\"font-family:" "'Arial'" "; font-size:8pt; font-weight:normal; color:#000000;\">";
	QString EndTag = "</span>";
	textCursor().insertHtml(startTag + str + EndTag);
#endif
}

void ReportTextEdit::setTextCursorPosition(int cursorPos)
{
	if (cursorPos < 0) return;
	QTextCursor cursor = textCursor();
	cursor.setPosition(cursorPos);
	setTextCursor(cursor);
}

void ReportTextEdit::selectFragment(int fragmentPos, int fragmentLen)
{
	QTextCursor cursor = textCursor();
	cursor.setPosition(fragmentPos);
	cursor.setPosition(fragmentPos + fragmentLen, QTextCursor::KeepAnchor);
	setTextCursor(cursor);
}

int ReportTextEdit::getImageStringNumber(const QString& name, int num)
{
	QString html = toHtml();
	QString imgName = name.arg(num);

	while (html.contains(imgName))
	{
		imgName = name.arg(++num);
	}
	return num;
}

void ReportTextEdit::updateImage()
{
	m_selectedImgFmt.updateImage(textCursor());
}


QRect ReportTextEdit::drawSliceCustom(QPainter * p, WINDOW_TYPE eWinType, int nDepth, bool bOptimizeSize)
{
	int window_level = WIN_MANAGER->getWindowLevel();
	int window_width = WIN_MANAGER->getWindowWidth();

	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	DATA_CONTEXT->volume_data.getLengthForScreen(eWinType, c_x, c_y, c_depth);

	COLOR * pData = new COLOR[c_x*c_y];
	short index_d;
	COLOR color;

	memset(pData, 0, sizeof(COLOR) * (c_x * c_y));

	BoundingBoxI box = DATA_CONTEXT->volume_data.getBoundingBoxForScreen(eWinType);
	bool cliped = false;

	bool Colormode = WIN_MANAGER->getColor2DMode();
	bool Invertmode = WIN_MANAGER->getInvertingLUTMode();

	int airXCount = 0;
	int airYCount = 0;

	for (int y = 0; y < c_y; y++)
	{
		for (int x = 0; x < c_x; x++)
		{
			switch (eWinType)
			{
				case WT_CORONAL:
					color = DATA_CONTEXT->volume_data.getGrayColorData(x, nDepth, c_y - y - 1, window_level, window_width, Colormode, Invertmode);
				break;
				case WT_SAGITTAL:
					color = DATA_CONTEXT->volume_data.getGrayColorData(nDepth, x, c_y - y - 1, window_level, window_width, Colormode, Invertmode);
				break;
				case WT_AXIAL:
				{
					color = DATA_CONTEXT->volume_data.getGrayColorData(x, y, nDepth, window_level, window_width, Colormode, Invertmode);

					if (bOptimizeSize)
					{
						if (y == (c_y / 2))
						{
							int idx = nDepth*c_x*c_y + y*c_x + x;
							mint16 hu = DATA_CONTEXT->volume_data.getData(idx);
							if (-1024 <= hu && hu <= -400)	// air
								airXCount++;
						}

						if (x == (c_x / 2))
						{
							int idx = nDepth*c_x*c_y + y*c_x + x;
							mint16 hu = DATA_CONTEXT->volume_data.getData(idx);
							if (-1024 <= hu && hu <= -400)	// air
								airYCount++;
						}
					}
				}
				break;
				default:
				break;
			}

			pData[(c_x * y) + x] = color;
		}
	}

	QImage image((uchar*)pData, c_x, c_y, QImage::Format_RGBA8888);

	p->setRenderHint(QPainter::Antialiasing);
	QRect pos;
	if (!bOptimizeSize)
		pos = QRect(0, 0, p->device()->width(), p->device()->height());
	else
	{
		float sizeXRatio = (float)airXCount / c_x;
		float sizeYRatio = (float)airYCount / c_y;
		int w = p->device()->width();
		int h = p->device()->height();
	//	QPoint scaled(qRound(float(c_x) * 0.5), qRound(float(c_y) * 0.5));
		QPoint scaled(qRound(float(w) * (1 + sizeXRatio)), qRound(float(h) * (1 + sizeYRatio)));
		QPoint imageZero = (QPoint(w, h) / 2) - (scaled / 2);
		pos = QRect(imageZero.x(), imageZero.y(), scaled.x(), scaled.y());
	}
	p->drawImage(pos, image);

	SAFE_DELETES(pData);
	return pos;
}

void ReportTextEdit::drawMaskCustom(QPainter *p, int nUID, WINDOW_TYPE eWinType, int nDepth, bool bForceDraw, QRect *optimizedRect)
{
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	DATA_CONTEXT->volume_data.getLengthForScreen(eWinType, c_x, c_y, c_depth);

	if (c_x <= 0 || c_y <= 0) return;

	COLOR * pMaskData = new COLOR[c_x*c_y];
	memset(pMaskData, 0, sizeof(COLOR) * (c_x * c_y));

	MaskInfo *info = DATA_CONTEXT->volume_data.getMaskInfo(nUID, true);

	if (info == NULL || (info->show == false && !bForceDraw))
		return;

	if (info->uid >= MASK_MAX)
		return;

	if (DATA_CONTEXT->volume_data.getVoxelCount(info->uid) <= 0)
		return;

	int mI = DATA_CONTEXT->volume_data.GetMaskByteIndex(nUID);
	info->color.a = info->layerAlpha;

	if (info->color.a == 0)
		return;

	BoundingBoxI box = DATA_CONTEXT->volume_data.getLayerBoundingBoxForScreen(eWinType, info->uid);
	if (nDepth < box.minZ || nDepth > box.maxZ)
		return;

	COLOR color;
	mask8 maskBit;

	bool Colormode = WIN_MANAGER->getColor2DMode();
	bool Invertmode = WIN_MANAGER->getInvertingLUTMode();

	for (int y = 0; y < c_y; y++)
	{
		for (int x = 0; x < c_x; x++)
		{
			switch (eWinType)
			{
			case WT_CORONAL:
				//DATA_CONTEXT->volume_data.getMaskData(x - 1, m_depth, (m_flipY ? c_y - y - 1 : y), mI);
				maskBit = DATA_CONTEXT->volume_data.getMaskData(x, nDepth, c_y - y - 1, mI);
				break;
			case WT_SAGITTAL:
				//DATA_CONTEXT->volume_data.getMaskData(m_depth, x - 1, (m_flipY ? c_y - y - 1 : y), mI);
				maskBit = DATA_CONTEXT->volume_data.getMaskData(nDepth, x, c_y - y - 1, mI);
				break;
			case WT_AXIAL:
				maskBit = DATA_CONTEXT->volume_data.getMaskData(x, y, nDepth, mI);
				break;
			default:
				break;
			}

			mask8 maskData = DATA_CONTEXT->volume_data.getMask(nUID);

			if (maskBit & maskData)
			{
				pMaskData[(c_x * y) + x] = info->color;
			}
		}
	}
	p->setRenderHint(QPainter::Antialiasing);
	QImage image((uchar*)pMaskData, c_x, c_y, QImage::Format_RGBA8888);
	QRect pos;
	if (optimizedRect)
		pos = *optimizedRect;
	else 
		pos = QRect(0, 0, p->device()->width(), p->device()->height());

	p->drawImage(pos, image);

	SAFE_DELETES(pMaskData);
}

void ReportTextEdit::drawMaskCustom(QPainter *p, QVector<int> uids, WINDOW_TYPE eWinType, int nDepth, bool bForceDraw, QRect *optimizedRect)
{
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	DATA_CONTEXT->volume_data.getLengthForScreen(eWinType, c_x, c_y, c_depth);

	if (c_x <= 0 || c_y <= 0) 
		return;

	COLOR * pMaskData = new COLOR[c_x*c_y];
	memset(pMaskData, 0, sizeof(COLOR) * (c_x * c_y));

	for (int i = 0; i < uids.size(); i++)
	{
		MaskInfo *info = DATA_CONTEXT->volume_data.getMaskInfo(uids.at(i), true);

		if (info == NULL || (info->show == false && !bForceDraw))
			continue;

		if (info->uid >= MASK_MAX)
			continue;

		if (DATA_CONTEXT->volume_data.getVoxelCount(info->uid) <= 0)
			continue;

		int mI = DATA_CONTEXT->volume_data.GetMaskByteIndex(uids.at(i));
		info->color.a = info->layerAlpha;

		if (info->color.a == 0)
			continue;

		BoundingBoxI box = DATA_CONTEXT->volume_data.getLayerBoundingBoxForScreen(eWinType, info->uid);
		if (nDepth < box.minZ || nDepth > box.maxZ)
			continue;

		COLOR color;
		mask8 maskBit;

		bool Colormode = WIN_MANAGER->getColor2DMode();
		bool Invertmode = WIN_MANAGER->getInvertingLUTMode();

		mask8 maskData = DATA_CONTEXT->volume_data.getMask(uids.at(i));

		for (int y = 0; y < c_y; y++)
		{
			for (int x = 0; x < c_x; x++)
			{
				switch (eWinType)
				{
				case WT_CORONAL:
					//DATA_CONTEXT->volume_data.getMaskData(x - 1, m_depth, (m_flipY ? c_y - y - 1 : y), mI);
					maskBit = DATA_CONTEXT->volume_data.getMaskData(x, nDepth, c_y - y - 1, mI);
					break;
				case WT_SAGITTAL:
					//DATA_CONTEXT->volume_data.getMaskData(m_depth, x - 1, (m_flipY ? c_y - y - 1 : y), mI);
					maskBit = DATA_CONTEXT->volume_data.getMaskData(nDepth, x, c_y - y - 1, mI);
					break;
				case WT_AXIAL:
					maskBit = DATA_CONTEXT->volume_data.getMaskData(x, y, nDepth, mI);
					break;
				default:
					break;
				}

				if (maskBit & maskData)
				{
					pMaskData[(c_x * y) + x] = info->color;
				}
			}
		}
	}

	p->setRenderHint(QPainter::Antialiasing);
	QImage image((uchar*)pMaskData, c_x, c_y, QImage::Format_RGBA8888);
	QRect pos;
	if (optimizedRect)
		pos = *optimizedRect;
	else
		pos = QRect(0, 0, p->device()->width(), p->device()->height());
	p->drawImage(pos, image);

	SAFE_DELETES(pMaskData);
}

QString ReportTextEdit::getHtmlFileName(QFileInfoList & htmlList)
{
	QString strFileName = PRODUCT_NAME_DEEPCATCH;
	for (int ii = 0; ii < htmlList.size(); ii++)
	{
		QString FileName = htmlList.at(ii).baseName();
		if (!FileName.compare(DEEPCATCH_HTMLNAME_DOCTORANSWER))
		{
			strFileName = htmlList.at(ii).fileName();
			break;
		}			
	}

	return strFileName;
}

void ReportTextEdit::setDeepCatchAnalysisData()
{
	//  loading된 문서에서 table을 찾아 vector에 삽입.
	QTextCursor cursor(document());
	cursor.movePosition(QTextCursor::Start);
	QVector<QTextTable*> list_table;
	QTextTable* preTable = nullptr;
	QTextTable* curTable = nullptr;
	while (cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 1))	// false는 문서 끝인 경우.
	{
		curTable = cursor.currentTable();
		if (curTable && curTable != preTable)
		{
			list_table.push_back(curTable);
			preTable = curTable;
		}
	}

	DEEPCATCH_REPORT_PREDICT_INFO *pPredictedInfo = ACTION_MANAGER->getDeepCatch_PredictedInfo();
	DEEPCATCH_REPORT *pReport = ACTION_MANAGER->getDeepCatchAnalysisVals();

//	if (pReport && pReport->reportThreadResult == eDRTRCMaskInvalid)
//		QMessageBox::warning(this, "DeepCatch report", "Some of the mask data is not valid.");

	if (!list_table.isEmpty())
	{
		QString strHeight = tr("");
		QString strWeight = tr("");
		int genderType = -1;

		//	키, 몸무게, 성별
		strHeight = pPredictedInfo->stPredictOpt.strHeight;
		strWeight = pPredictedInfo->stPredictOpt.strWeight;
		genderType = pPredictedInfo->stPredictOpt.genderType;

		// patient information insert.
		QTextTable* dicomInfoTable = list_table.at(1);
		patientInfoInsert(dicomInfoTable, pReport, strHeight, strWeight, genderType);

		// volume view capture image insert
		QTextTable* volumeviewCaptureImageInsertedTable = list_table.at(2);
		volumeCaptureImgInsert(volumeviewCaptureImageInsertedTable, pReport);

		// axial view drawing image insert + area chart image insert.
		QTextTable* axialDrawingImageInsertedTable = list_table.at(3);
		axialViewDrawingImgInsert(axialDrawingImageInsertedTable, pReport);

#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER
		QTextTable *patientStatusDisplayTable = nullptr;

		bool bDOCTORANSWER_CHART_FUNCTION = true;
		if (bDOCTORANSWER_CHART_FUNCTION)
		{
			QTextTable* hccPredictResultTable = list_table.at(4);
			hccPredictResultInfoInsert(hccPredictResultTable, pReport);

			QTextTable* qwtChartDrawImgInsertedTable = list_table.at(5);
			qwtChartDrawImgInsert(qwtChartDrawImgInsertedTable, pReport);
			std::vector<float> empty;
			pReport->HCC_Result_Percentage.swap(empty);

			// muscle / fat state information display table data insert.
			patientStatusDisplayTable = list_table.at(6);
		}
		else
		{
			patientStatusDisplayTable = list_table.at(4);
		}
#else
		// muscle / fat state information display table data insert.
		QTextTable *patientStatusDisplayTable = list_table.at(4);
#endif	
		muscleFatStateTableInfoInsert(patientStatusDisplayTable, pReport, strHeight, strWeight, genderType, pPredictedInfo);
	}
	else // html에 table이 포함되어 있지 않으면 비정상
		QMessageBox(QMessageBox::Warning, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DR_0004)).exec();

	return;
}

#ifndef DEEPCATCH_REPORT_150PPI_INTEGRATE
void ReportTextEdit::patientInfoInsert(QTextTable* dicomInfoTable, DEEPCATCH_REPORT *pReport, QString strHeight, QString strWeight, int genderType)
{
	// owner's information insert.
	QTextCursor cursor(document());
	cursor.beginEditBlock();

	QString patientTableHtmlStartTag = tr("");
	QString patientTableHtmlEndTag = "</span>";
	switch (pReport->curA4Res)
	{
		//	eART96PPI:
		//	break;
	case eART150PPI:
		patientTableHtmlStartTag = "<span style=\"font-family:" "'Arial'" "; font-size:14pt; font-weight:normal; color:#000000;\">";
		break;
		//	eART300PPI:
		//	break;
	case eART72PPI:
	default:
		patientTableHtmlStartTag = "<span style=\"font-family:" "'Arial'" "; font-size:9pt; font-weight:normal; color:#000000;\">";
		break;
	}

	QString strTemp = tr("");
	DcmtkSeriesInfo *dicomFileInfo = WIN_MANAGER->GetDicomInfo();

	// patient name
	cursor.setPosition(dicomInfoTable->cellAt(1, 0).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 0).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::fromStdString(dicomFileInfo->patientsName_);
	//	dicomInfoTable->cellAt(1, 0).lastCursorPosition().insertText(strTemp, textFormat);
	dicomInfoTable->cellAt(1, 0).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	// patient id
	cursor.setPosition(dicomInfoTable->cellAt(1, 1).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 1).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::fromStdString(dicomFileInfo->patientId_);
	//	dicomInfoTable->cellAt(1, 1).lastCursorPosition().insertText(strTemp, textFormat);
	dicomInfoTable->cellAt(1, 1).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	// age
	cursor.setPosition(dicomInfoTable->cellAt(1, 2).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 2).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::fromStdString(dicomFileInfo->age_);
	//	dicomInfoTable->cellAt(1, 2).lastCursorPosition().insertText(strTemp, textFormat);
	dicomInfoTable->cellAt(1, 2).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	// sex
	cursor.setPosition(dicomInfoTable->cellAt(1, 3).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 3).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();

	if (genderType != -1)
	{
		switch (genderType)
		{
		case eDGTUnknown:
			strTemp = tr("Unknown");
			break;
		case eDGTFemale:
			strTemp = tr("F");
			break;
		case eDGTMale:
			strTemp = tr("M");
			break;
		}
	}
	else
		strTemp = tr("Unknown");

	if (strTemp.isEmpty())
		strTemp = " ";

	//	dicomInfoTable->cellAt(1, 3).lastCursorPosition().insertText(strTemp, textFormat);
	dicomInfoTable->cellAt(1, 3).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	//	키, 몸무게
	cursor.setPosition(dicomInfoTable->cellAt(1, 4).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 4).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if (strHeight.isEmpty() || !strHeight.compare("0", Qt::CaseInsensitive))
		strTemp = tr("");
	else
		strTemp = strHeight + QString("cm");
	//	dicomInfoTable->cellAt(1, 4).lastCursorPosition().insertText(aiSegTab->getReportHeightStr() + QString("cm"), textFormat);
	dicomInfoTable->cellAt(1, 4).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	cursor.setPosition(dicomInfoTable->cellAt(1, 5).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 5).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if (strWeight.isEmpty() || !strWeight.compare("0", Qt::CaseInsensitive))
		strTemp = tr("");
	else
		strTemp = strWeight + QString("kg");
	//	dicomInfoTable->cellAt(1, 5).lastCursorPosition().insertText(strTemp, textFormat);
	dicomInfoTable->cellAt(1, 5).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	// OnPredict 버튼 누를 때로 이동
	// date
	//QDateTime date;
	//QString strTime;
	//if (network::GetServerTime(date))
	//{
	//	//str = date.toString("yyyy-MM-dd HH:mm:ss");
	//	strTime = date.toString("yyyy-MM-dd");
	//}
	//else
	//	strTime = QDateTime::currentDateTime().toString("yyyy-MM-dd");

	//pReport->expertReportInfo.strReportingDate = strTime;
	DEEPCATCH_REPORT_PREDICT_INFO *pPredictedInfo = ACTION_MANAGER->getDeepCatch_PredictedInfo();

	cursor.setPosition(dicomInfoTable->cellAt(1, 6).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 6).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	//	dicomInfoTable->cellAt(1, 6).lastCursorPosition().insertText(strTime, textFormat);
	dicomInfoTable->cellAt(1, 6).lastCursorPosition().insertHtml(patientTableHtmlStartTag + pPredictedInfo->stPredictOpt.strReportingDate + patientTableHtmlEndTag);

	cursor.endEditBlock();
}
#else
void ReportTextEdit::patientInfoInsert(QTextTable* dicomInfoTable, DEEPCATCH_REPORT *pReport, QString strHeight, QString strWeight, int genderType)
{
	// owner's information insert.
	QTextCharFormat patientInfoTable_txtCharFormat;
	QFont font("Arial", 14, QFont::Normal);
	patientInfoTable_txtCharFormat.setFont(font);

	DEEPCATCH_REPORT_PREDICT_INFO *pPredictedInfo = ACTION_MANAGER->getDeepCatch_PredictedInfo();

	QString patientTableHtmlStartTag = tr("");
	QString patientTableHtmlEndTag = "</span>";
	switch (pReport->curA4Res)
	{
		//	eART96PPI:
		//	break;
	case eART150PPI:
		patientTableHtmlStartTag = "<span style=\"font-family:" "'Arial'" "; font-size:14pt; font-weight:normal; color:#000000;\">";
		break;
		//	eART300PPI:
		//	break;
	case eART72PPI:
	default:
		patientTableHtmlStartTag = "<span style=\"font-family:" "'Arial'" "; font-size:9pt; font-weight:normal; color:#000000;\">";
		break;
	}

	QString strTemp = tr("");
	DcmtkSeriesInfo *dicomFileInfo = WIN_MANAGER->GetDicomInfo();
	
	QTextCursor cursor(document());
	cursor.beginEditBlock();

	// patient name
	cursor.setPosition(dicomInfoTable->cellAt(1, 0).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 0).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::fromStdString(dicomFileInfo->patientsName_);
	if (strTemp.isEmpty())
		strTemp = " ";
	dicomInfoTable->cellAt(1, 0).lastCursorPosition().insertText(strTemp, patientInfoTable_txtCharFormat);
//	dicomInfoTable->cellAt(1, 0).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	// patient id
	cursor.setPosition(dicomInfoTable->cellAt(1, 1).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 1).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::fromStdString(dicomFileInfo->patientId_);
	if (strTemp.isEmpty())
		strTemp = " ";
	dicomInfoTable->cellAt(1, 1).lastCursorPosition().insertText(strTemp, patientInfoTable_txtCharFormat);
//	dicomInfoTable->cellAt(1, 1).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	// age
	cursor.setPosition(dicomInfoTable->cellAt(1, 2).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 2).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::fromStdString(dicomFileInfo->age_);
	
	if (strTemp.isEmpty())
	{
		DEEPCATCH_REPORT_PREDICT_INFO *predictInfo = ACTION_MANAGER->getDeepCatch_PredictedInfo();
		int age = predictInfo->stPredictOpt.age_;
		if(age < 1)
			strTemp = " ";
		else
			strTemp = QString::number(age);
	}		

	dicomInfoTable->cellAt(1, 2).lastCursorPosition().insertText(strTemp, patientInfoTable_txtCharFormat);
//	dicomInfoTable->cellAt(1, 2).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	// sex
	cursor.setPosition(dicomInfoTable->cellAt(1, 3).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 3).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if (genderType != -1)
	{
		switch (genderType)
		{
			case eDGTUnknown:
				strTemp = tr("Unknown");
			break;
			case eDGTFemale:
				strTemp = tr("F");
			break;
			case eDGTMale:
				strTemp = tr("M");
			break;
		}
	}
	else
		strTemp = tr("Unknown");

	if (strTemp.isEmpty())
		strTemp = " ";
	dicomInfoTable->cellAt(1, 3).lastCursorPosition().insertText(strTemp, patientInfoTable_txtCharFormat);
//	dicomInfoTable->cellAt(1, 3).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	//	키
	cursor.setPosition(dicomInfoTable->cellAt(1, 4).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 4).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if (strHeight.isEmpty() || !strHeight.compare("0", Qt::CaseInsensitive))
		strTemp = tr(" ");
	else
		strTemp = strHeight + QString("cm");
	dicomInfoTable->cellAt(1, 4).lastCursorPosition().insertText(strTemp, patientInfoTable_txtCharFormat);
//	dicomInfoTable->cellAt(1, 4).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	// 몸무게
	cursor.setPosition(dicomInfoTable->cellAt(1, 5).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 5).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if (strWeight.isEmpty() || !strWeight.compare("0", Qt::CaseInsensitive))
		strTemp = tr(" ");
	else
		strTemp = strWeight + QString("kg");
	dicomInfoTable->cellAt(1, 5).lastCursorPosition().insertText(strTemp, patientInfoTable_txtCharFormat);
//	dicomInfoTable->cellAt(1, 5).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	// predict time 기록.
	cursor.setPosition(dicomInfoTable->cellAt(1, 6).firstPosition());
	cursor.setPosition(dicomInfoTable->cellAt(1, 6).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = pPredictedInfo->stPredictOpt.strReportingDate;
	if (strTemp.isEmpty())
		strTemp = " ";
	dicomInfoTable->cellAt(1, 6).lastCursorPosition().insertText(strTemp, patientInfoTable_txtCharFormat);
//	dicomInfoTable->cellAt(1, 6).lastCursorPosition().insertHtml(patientTableHtmlStartTag + strTemp + patientTableHtmlEndTag);

	cursor.endEditBlock();
}
#endif

void ReportTextEdit::volumeCaptureImgInsert(QTextTable* volumeviewCaptureImageInsertedTable, DEEPCATCH_REPORT *pReport)
{
	// avf, muscle, of volume view capture image insert.
	int width = 0, height = 0;
	getDrawingImageSize_keepAspectRatio(pReport, WT_VOLUME, width, height);

	textCursor().beginEditBlock();

	volumeviewCaptureImageInsertedTable->cellAt(0, 0).firstCursorPosition().deleteChar();
	//	volumeviewCaptureImageInsertedTable->cellAt(0, 0).lastCursorPosition().insertImage(pReport->scaledVolumeViewImg[0]);
	QUrl subcutaneousFatUrl("subcutaneousFat.png");
	document()->addResource(QTextDocument::ImageResource, subcutaneousFatUrl, pReport->scaledVolumeViewImg[0]);
	QTextImageFormat imgFmt;
	imgFmt.setName(subcutaneousFatUrl.url());
	imgFmt.setWidth(width);
	imgFmt.setHeight(height);
	volumeviewCaptureImageInsertedTable->cellAt(0, 0).lastCursorPosition().insertImage(imgFmt);

	volumeviewCaptureImageInsertedTable->cellAt(0, 1).firstCursorPosition().deleteChar();
	//	volumeviewCaptureImageInsertedTable->cellAt(0, 1).lastCursorPosition().insertImage(pReport->scaledVolumeViewImg[1]);
	QUrl muscleUrl("muscle.png");
	document()->addResource(QTextDocument::ImageResource, muscleUrl, pReport->scaledVolumeViewImg[1]);
	QTextImageFormat imgFmt2;
	imgFmt2.setName(muscleUrl.url());
	imgFmt2.setWidth(width);
	imgFmt2.setHeight(height);
	volumeviewCaptureImageInsertedTable->cellAt(0, 1).lastCursorPosition().insertImage(imgFmt2);

	volumeviewCaptureImageInsertedTable->cellAt(0, 2).firstCursorPosition().deleteChar();
	//	volumeviewCaptureImageInsertedTable->cellAt(0, 2).lastCursorPosition().insertImage(pReport->scaledVolumeViewImg[2]);
	QUrl abdominalVisceralFatUrl("abdominalVisceralFat.png");
	document()->addResource(QTextDocument::ImageResource, abdominalVisceralFatUrl, pReport->scaledVolumeViewImg[2]);
	QTextImageFormat imgFmt3;
	imgFmt3.setName(abdominalVisceralFatUrl.url());
	imgFmt3.setWidth(width);
	imgFmt3.setHeight(height);
	volumeviewCaptureImageInsertedTable->cellAt(0, 2).lastCursorPosition().insertImage(imgFmt3);

	textCursor().endEditBlock();
}

void ReportTextEdit::axialViewDrawingImgInsert(QTextTable* axialDrawingImageInsertedTable, DEEPCATCH_REPORT *pReport)
{
	int axialWidth = 0;
	int axialHeight = 0;
	int saggitalWidth = 0;
	int saggitalHeight = 0;
	DEEPCATCH_REPORT_PREDICT_INFO *pPredictedInfo = ACTION_MANAGER->getDeepCatch_PredictedInfo();

	// axial view drawing image insert.
	getDrawingImageSize_keepAspectRatio(pReport, WT_AXIAL, axialWidth, axialHeight);
	unsigned int c_x;
	unsigned int c_y;
	unsigned int c_depth;
	DATA_CONTEXT->volume_data.getLengthForScreen(WT_AXIAL, c_x, c_y, c_depth);
	QImage axialImage = QImage(c_x, c_y, QImage::Format_RGBA8888);
	axialImage.fill(Qt::transparent);
	QPainter painter(&axialImage);
	if (pReport->nDisplayAxialDepth != -1)
	{
		QRect rect;
		rect = drawSliceCustom(&painter, WT_AXIAL, pReport->nDisplayAxialDepth, true);
		//	for (int i = 0; i < pReport->vecAxialViewMaskUIDs.size(); i++)
		drawMaskCustom(&painter, pReport->vecAxialViewMaskUIDs, WT_AXIAL, pReport->nDisplayAxialDepth, true, &rect);
	}

	textCursor().beginEditBlock();
	axialDrawingImageInsertedTable->cellAt(0, 0).firstCursorPosition().deleteChar();
	//	axialDrawingImageInsertedTable->cellAt(0, 0).lastCursorPosition().insertImage(axialImage);
	QUrl axialImageUrl("axialImage.png");
	document()->addResource(QTextDocument::ImageResource, axialImageUrl, axialImage);
	QTextImageFormat axialImgFmt;
	axialImgFmt.setName(axialImageUrl.url());
	axialImgFmt.setWidth(axialWidth);
	axialImgFmt.setHeight(axialHeight);
	axialDrawingImageInsertedTable->cellAt(0, 0).lastCursorPosition().insertImage(axialImgFmt);
	textCursor().endEditBlock();
	pReport->axialDrawingImg = axialImage;

	// muscle quality map image
	if (pPredictedInfo && pPredictedInfo->stPredictOpt.bMuscleQualityMap)
	{
		MaskInfo* pMaskInfo = nullptr;
		VOLUME_DATA *pVolumeData = &DATA_CONTEXT->volume_data;
		QVector<int> vecAxialViewMaskUIDs;
		for (int i = 0; i < pVolumeData->getMaskInfoListCnt(); ++i)
		{
			pMaskInfo = pVolumeData->getMaskInfo(i);
			QString maskName = QString::fromWCharArray(pMaskInfo->maskName);
			if (!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ADP_TISSUE) ||
				!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ABNORMAL1) ||
				!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_ABNORMAL2) ||
				!maskName.compare(DEEPCATCH_WHOLEBODY_MASKNAME_MUSCLE_NORMAL))
				vecAxialViewMaskUIDs.push_back(pMaskInfo->uid);
		}

		QImage axialImage_muscleQualityMap = QImage(c_x, c_y, QImage::Format_RGBA8888);
		axialImage_muscleQualityMap.fill(Qt::transparent);
		QPainter painter(&axialImage_muscleQualityMap);
		QRect rect;
		rect = drawSliceCustom(&painter, WT_AXIAL, pReport->nDisplayAxialDepth, true);
		drawMaskCustom(&painter, vecAxialViewMaskUIDs, WT_AXIAL, pReport->nDisplayAxialDepth, true, &rect);
		pReport->axialDrawingImg_muscleQualityMap = axialImage_muscleQualityMap;
	}

	// saggital view drawing image insert.
	QImage *volumeView = &(pReport->scaledVolumeViewImg[3]);
	int cz = DATA_CONTEXT->volume_data.getCZ();
	float y = 0.0f;
	QPainter painter2(volumeView);
	if (pPredictedInfo->stPredictOpt.preferenceType == eDPTSingleSlice)
	{
		QPen pen(Qt::DotLine);
		pen.setColor(QColor(115, 255, 64, 255));
		pen.setWidth(5);
		painter2.setPen(pen);

		y = (float)volumeView->height()*((float)pPredictedInfo->nAxialDepth/cz);
		QPointF startPoint(0, y);
		QPointF endPoint(volumeView->width(), y);
		painter2.drawLine(startPoint, endPoint);
	}
	else if (pPredictedInfo->stPredictOpt.preferenceType == eDPTMultiSlice)
	{
		QPen pen(Qt::DotLine);
		pen.setColor(QColor(76, 87, 243, 255));
		pen.setWidth(5);
		painter2.setPen(pen);

		y = (float)volumeView->height()*((float)pPredictedInfo->nStartAxialDepth/cz);
		QPointF startPoint(0, y);
		QPointF endPoint(volumeView->width(), y);
		painter2.drawLine(startPoint, endPoint);

		y = (float)volumeView->height()*((float)pPredictedInfo->nEndAxialDepth/cz);
		QPointF startPoint2(0, y);
		QPointF endPoint2(volumeView->width(), y);
		painter2.drawLine(startPoint2, endPoint2);
	}

	getDrawingImageSize_keepAspectRatio(pReport, WT_SAGITTAL, saggitalWidth, saggitalHeight, axialHeight);
	textCursor().beginEditBlock();
	axialDrawingImageInsertedTable->cellAt(0, 1).firstCursorPosition().deleteChar();
//	axialDrawingImageInsertedTable->cellAt(0, 1).lastCursorPosition().insertImage(saggitalImage);
	QUrl sagittalImage("sagittalImage.png");
	document()->addResource(QTextDocument::ImageResource, sagittalImage, *volumeView);
	QTextImageFormat imgFmt;
	imgFmt.setName(sagittalImage.url());
	imgFmt.setWidth(saggitalWidth);
	imgFmt.setHeight(saggitalHeight);
	axialDrawingImageInsertedTable->cellAt(0, 1).lastCursorPosition().insertImage(imgFmt);
	textCursor().endEditBlock();

	// area chart image insert. <- 220304 허 건 과장 주석처리
	//areaChartDrawingImgInsert(axialDrawingImageInsertedTable, saggitalHeight);
}

void ReportTextEdit::getDrawingImageSize_keepAspectRatio(DEEPCATCH_REPORT *pReport, WINDOW_TYPE type, int &width, int &height, int limitHeight)
{
	// x,y 비율에 따라 사이즈 계산.
	int maxWidth = 0;
	int maxHeight = 0;
	int axialWidth = 0;
	int axialHeight = 0;
	//	unsigned int c_x;
	//	unsigned int c_y;
	//	unsigned int c_depth;
	//	DATA_CONTEXT->volume_data.getLengthForScreen(type, c_x, c_y, c_depth);
	float sizeX = 0.0f;
	float sizeY = 0.0f;
	if (type == WT_AXIAL)
	{
		sizeX = DATA_CONTEXT->volume_data.getSizeX();
		sizeY = DATA_CONTEXT->volume_data.getSizeY();
	}
	else if (type == WT_SAGITTAL)
	{
		sizeX = pReport->scaledVolumeViewImg[3].width();
		sizeY = pReport->scaledVolumeViewImg[3].height();
	}
	else if (type == WT_VOLUME)
	{
		sizeX = pReport->scaledVolumeViewImg[0].width();
		sizeY = pReport->scaledVolumeViewImg[0].height();
	}

	if (sizeX && sizeY)
	{
		switch (pReport->curA4Res)
		{
			//	eART96PPI:
			//	break;
		case eART150PPI:
		{
			maxWidth = 360;
			if (type == WT_VOLUME)
			{
				if (limitHeight)
					maxHeight = limitHeight;
				else
					maxHeight = 354;
			}
			else
			{
				if (limitHeight)
					maxHeight = limitHeight;
				else
					maxHeight = 335;
			}
			axialWidth = ((float)sizeX / sizeY)*maxHeight;
			axialHeight = ((float)sizeY / sizeX)*maxWidth;
			if (axialWidth > maxWidth)
			{
				axialWidth = maxWidth;
				axialHeight = ((float)sizeY / sizeX)*axialWidth;
			}
			else /*if (axialHeight > maxHeight)*/
			{
				axialHeight = maxHeight;
				axialWidth = ((float)sizeX / sizeY)*axialHeight;
			}
		}
		break;
		//	eART300PPI:
		//	break;
		case eART72PPI:
		default:
		{
			maxWidth = 174;
			if (type == WT_VOLUME)
			{
				if (limitHeight)
					maxHeight = limitHeight;
				else
					maxHeight = 145;
			}
			else
			{
				if (limitHeight)
					maxHeight = limitHeight;
				else
					maxHeight = 158;
			}
			axialWidth = ((float)sizeX / sizeY)*maxHeight;
			axialHeight = ((float)sizeY / sizeX)*maxWidth;
			if (axialWidth > maxWidth)
			{
				axialWidth = maxWidth;
				axialHeight = ((float)sizeY / sizeX)*axialWidth;
			}
			else /*if (axialHeight > maxHeight)*/
			{
				axialHeight = maxHeight;
				axialWidth = ((float)sizeX / sizeY)*axialHeight;
			}
		}
		break;
		}
	}
	width = axialWidth;
	height = axialHeight;
}

void ReportTextEdit::areaChartDrawingImgInsert(QTextTable* chartImageInsertedTable, int limitHeight)
{
#ifdef QT_CHARTS_USE
	// chart drawing image insert.
//	rangeDisplayChart *chart = new rangeDisplayChart();
	deepcatchAreaChart *chart = nullptr;
	if (limitHeight)
		chart = new deepcatchAreaChart(nullptr, limitHeight);
	else 
		chart = new deepcatchAreaChart();
	QChartView *chartView = new QChartView(chart);
//	chartView->rotate(90);
	chartView->setRenderHint(QPainter::Antialiasing);
	QPixmap buffer(chart->m_nChartWidth, chart->m_nChartHeight);
//	const auto dpr = chartView->devicePixelRatioF();
//	buffer.setDevicePixelRatio(dpr);
	buffer.fill(Qt::transparent);

	QTransform transform;
	transform.translate(buffer.height(), 0);
	transform.rotate(90);

	QPainter paint(&buffer);
	paint.setTransform(transform);
	paint.setRenderHint(QPainter::Antialiasing);
//	paint->setPen(*(new QColor(255, 34, 255, 255)));
	chartView->setVisible(true);
	chartView->render(&paint);
	chartView->setVisible(false);

	//	QImage img(buffer.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied));
	QImage img = buffer.toImage();
	textCursor().beginEditBlock();
	chartImageInsertedTable->cellAt(0, 2).firstCursorPosition().deleteChar();
//	chartImageInsertedTable->cellAt(0, 2).lastCursorPosition().insertImage(img);
	QUrl chartImageUrl("chartImage.png");
	document()->addResource(QTextDocument::ImageResource, chartImageUrl, img);
	QTextImageFormat chartImgFmt;
	chartImgFmt.setName(chartImageUrl.url());
	chartImgFmt.setWidth(img.width());
	chartImgFmt.setHeight(img.height());
	chartImageInsertedTable->cellAt(0, 2).lastCursorPosition().insertImage(chartImgFmt);
	textCursor().endEditBlock();
#endif	// QT_CHARTS_USE
}


void ReportTextEdit::hccPredictResultInfoInsert(QTextTable * hccPredictResultTable, DEEPCATCH_REPORT * pReport)
{
#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER
	// owner's information insert.
	QTextCharFormat hccPredictResultTable_txtCharFormat;
	QFont font("Arial", 14, QFont::Normal);
	hccPredictResultTable_txtCharFormat.setFont(font);	
	hccPredictResultTable_txtCharFormat.setVerticalAlignment(QTextCharFormat::VerticalAlignment::AlignMiddle);

	QTextCursor cursor(document());
	cursor.beginEditBlock();

	//Insert HCC Predict Value
	cursor.setPosition(hccPredictResultTable->cellAt(1, 0).firstPosition());
	cursor.setPosition(hccPredictResultTable->cellAt(1, 0).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();	
	//QString strTemp = QString::fromStdString("   TEST01");
	//hccPredictResultTable->cellAt(1, 0).lastCursorPosition().insertText(strTemp, hccPredictResultTable_txtCharFormat);

	
	int HCC_PREDICT_OUT_CNT = 8;
	for (int ii = 0; ii < HCC_PREDICT_OUT_CNT; ii++)
	{
		int index = (ii * 2) + 2;
		cursor.setPosition(hccPredictResultTable->cellAt(1, index).firstPosition());
		cursor.setPosition(hccPredictResultTable->cellAt(1, index).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();

		
		QTextBlockFormat centerAlignment;
		centerAlignment.setAlignment(Qt::AlignHCenter);
		cursor.setBlockFormat(centerAlignment);

		QString values;
		if (pReport->HCC_Result_Percentage.size() > 0)
			values = QString::number(pReport->HCC_Result_Percentage[ii]);
		else
			values = "";
		QString _strTemp = values;
		hccPredictResultTable->cellAt(1, index).lastCursorPosition().insertText(_strTemp, hccPredictResultTable_txtCharFormat);
	}

	cursor.endEditBlock();
#endif
}

void ReportTextEdit::qwtChartDrawImgInsert(QTextTable * qwtchartDrawImageInsertedTable, DEEPCATCH_REPORT * pReport)
{
#ifdef SUPPORT_DEEPCATCH_V2_DOCTORANSWER
	int width = 800, height = 600;
	QImage *pImg = &(pReport->chartImg);
	textCursor().beginEditBlock();

	qwtchartDrawImageInsertedTable->cellAt(0, 0).firstCursorPosition().deleteChar();
	qwtchartDrawImageInsertedTable->cellAt(0, 0).firstCursorPosition().deletePreviousChar();

	if (pReport->HCC_Result_Percentage.size() > 0)
	{
		QUrl qwtChartUrl("qwtChartSample.png");
		document()->addResource(QTextDocument::ImageResource, qwtChartUrl, pReport->chartImg);
		QTextImageFormat imgFmt;
		imgFmt.setName(qwtChartUrl.url());
		imgFmt.setWidth(pReport->chartImg.width());
		imgFmt.setHeight(pReport->chartImg.height());
		qwtchartDrawImageInsertedTable->cellAt(0, 0).lastCursorPosition().insertImage(imgFmt);
	}

	textCursor().endEditBlock();
#endif
}


#ifndef DEEPCATCH_REPORT_150PPI_INTEGRATE
void ReportTextEdit::muscleFatStateTableInfoInsert(QTextTable *patientStatusDisplayTable, DEEPCATCH_REPORT *pReport, QString strHeight, QString strWeight, int genderType, DEEPCATCH_REPORT_PREDICT_INFO *pPredictedInfo)
{
	/* 내장지방, 근육량에 따른 결과 표시 */
	QString strTemp = tr("");
	int voxelCountAVF = 0;
	int voxelCountMiscle = 0;
	int voxelCountOf = 0;

	// L3 or Abdominal Waist mask의 각 부위 voxel count가 invalid한 경우 예외 처리.
//	if (pReport->mapDeepcatchClassToVolxelCount.isEmpty())
//		QMessageBox::warning(this, "DeepCatch report", "Some of the report data(mapDeepcatchClassToVolxelCount is empty) is not valid.");

	QMap<eDeepCatchWholebodyClasses, int>::const_iterator iterMap = pReport->mapDeepcatchClassToVolxelCount.find(eDWMAVF);
	if (iterMap != pReport->mapDeepcatchClassToVolxelCount.end())
		voxelCountAVF = iterMap.value();
	iterMap = pReport->mapDeepcatchClassToVolxelCount.find(eDWMMuscle);
	if (iterMap != pReport->mapDeepcatchClassToVolxelCount.end())
		voxelCountMiscle = iterMap.value();
	iterMap = pReport->mapDeepcatchClassToVolxelCount.find(eDWMOF);
	if (iterMap != pReport->mapDeepcatchClassToVolxelCount.end())
		voxelCountOf = iterMap.value();

	// bmi calc
	float BMI = 0.0f;
	bool bInvalidBMI = false;
	float height_cm = strHeight.toFloat();			// cm
	float height_m = 0.0f;
	if (height_cm != 0.0f)
		height_m = height_cm / 100;		// m
	float weight_kg = strWeight.toFloat();			// kg
	if (height_cm == 0.0f || weight_kg == 0.0f)
		bInvalidBMI = true;
	if (!bInvalidBMI)
		BMI = weight_kg / (height_m*height_m);			//

	pReport->expertReportInfo.bInvalidBMI = bInvalidBMI;
	pReport->expertReportInfo.BMI = BMI;
														// SMA calc
	float skeletalMuscleArea = 0.0f;				// spacex * spacey * voxelCount
	float skeletalMuscleIndices = 0.0f;				// SMA/height의제곱
	float skeletalMuscleBMI = 0.0f;					// SMA/BMI
	if (voxelCountMiscle)
	{
		skeletalMuscleArea = DATA_CONTEXT->volume_data.getSpaceX() * DATA_CONTEXT->volume_data.getSpaceY() * voxelCountMiscle;
		if (!bInvalidBMI)
		{
			skeletalMuscleIndices = skeletalMuscleArea / (height_m*height_m);
			skeletalMuscleBMI = skeletalMuscleArea / BMI;
		}
	}

	// VFA calc
	float visceralFatArea = 0.0f;					// spacex * spacey * voxelCount
	float visceralFatIndices = 0.0f;				// SMA/height의제곱
	float visceralFatBMI = 0.0f;					// SMA/BMI
	if (voxelCountAVF)
	{
		visceralFatArea = DATA_CONTEXT->volume_data.getSpaceX() * DATA_CONTEXT->volume_data.getSpaceY() * voxelCountAVF;
		if (!bInvalidBMI)
		{
			visceralFatIndices = visceralFatArea / (height_m*height_m);
			visceralFatBMI = visceralFatArea / BMI;
		}
	}

	// SFA calc
	float subcutaneousFatArea = 0.0f;				// spacex * spacey * voxelCount
	float subcutaneousFatIndices = 0.0f;			// SMA/height의제곱
	float subcutaneousFatBMI = 0.0f;				// SMA/BMI
	if (voxelCountOf)
	{
		subcutaneousFatArea = DATA_CONTEXT->volume_data.getSpaceX() * DATA_CONTEXT->volume_data.getSpaceY() * voxelCountOf;
		if (!bInvalidBMI)
		{
			subcutaneousFatIndices = subcutaneousFatArea / (height_m*height_m);
			subcutaneousFatBMI = subcutaneousFatArea / BMI;
		}
	}

	// fat amount ratio calc
	float fatAmountRatio = 0.0f;
//	fatAmountRatio = ((float)voxelCountAVF / ((float)voxelCountAVF + (float)voxelCountOf))*100.0f;
	fatAmountRatio = ((float)voxelCountAVF/(float)voxelCountOf)*100.0f;

	// 값에 따른 판단
	eDeepcatchMuscleMassType patientMuscleMassType = eDMMTNone;
	eDeepcatchVisceralFatMassType patientVisceralFatMassType = eDVFMTNone;
	QString strDeepcatchMuscleMassType = tr("");
	QString strDeepcatchVisceralFatMassType = tr("");

	// 근육량
	if (!bInvalidBMI)
	{
		switch (genderType)
		{
		case eDGTMale:
		{
			if ((skeletalMuscleArea > 140.0f) && (skeletalMuscleIndices > 47.0f) && (skeletalMuscleBMI > 5.7f))
			{
				patientMuscleMassType = eDMMTMuch;
				strDeepcatchMuscleMassType = "Preserved muscle mass";
			}
			else if ((119.0f < skeletalMuscleArea && skeletalMuscleArea <= 140.0f) &&
				(40.0f < skeletalMuscleIndices && skeletalMuscleIndices <= 47.0f) &&
				(5.0f < skeletalMuscleBMI && skeletalMuscleBMI <= 5.7f))
			{
				patientMuscleMassType = eDMMTLittle;
				strDeepcatchMuscleMassType = "Borderline decreased muscle mass";
			}
			else if (skeletalMuscleArea <= 119.0f && skeletalMuscleIndices <= 40.0f && skeletalMuscleBMI <= 5.0f)
			{
				patientMuscleMassType = eDMMTLeast;
				strDeepcatchMuscleMassType = "Decreased muscle mass";
			}
		}
		break;
		case eDGTFemale:
		{
			if (skeletalMuscleArea > 87.0f && skeletalMuscleIndices > 34.0f && skeletalMuscleBMI > 4.1f)
			{
				patientMuscleMassType = eDMMTMuch;
				strDeepcatchMuscleMassType = "Preserved muscle mass";
			}
			else if ((74.0f < skeletalMuscleArea && skeletalMuscleArea <= 87.0f) &&
				(28.0f < skeletalMuscleIndices && skeletalMuscleIndices <= 34.0f) &&
				(3.5f < skeletalMuscleBMI && skeletalMuscleBMI <= 4.1f))
			{
				patientMuscleMassType = eDMMTLittle;
				strDeepcatchMuscleMassType = "Borderline decreased muscle mass";
			}
			else if (skeletalMuscleArea <= 74.0f && skeletalMuscleIndices <= 28.0f && skeletalMuscleBMI <= 3.5f)
			{
				patientMuscleMassType = eDMMTLeast;
				strDeepcatchMuscleMassType = "Decreased muscle mass";
			}
		}
		break;
		}
	}

	// area && cm2/height(m2) && cm2/BMI 조건으로 필터링 안되는 경우 area로 처리.
	bool bNotFind = false;
	if (patientMuscleMassType == eDMMTNone && strDeepcatchMuscleMassType.isEmpty())
		bNotFind = true;

	if (bNotFind)
	{
		switch (genderType)
		{
		case eDGTMale:
		{
			if (skeletalMuscleArea > 140.0f)
			{
				patientMuscleMassType = eDMMTMuch;
				strDeepcatchMuscleMassType = "Preserved muscle mass";
			}
			else if (119.0f < skeletalMuscleArea && skeletalMuscleArea <= 140.0f)
			{
				patientMuscleMassType = eDMMTLittle;
				strDeepcatchMuscleMassType = "Borderline decreased muscle mass";
			}
			else if (skeletalMuscleArea <= 119.0f)
			{
				patientMuscleMassType = eDMMTLeast;
				strDeepcatchMuscleMassType = "Decreased muscle mass";
			}
		}
		break;
		case eDGTFemale:
		{
			if (skeletalMuscleArea > 87.0f)
			{
				patientMuscleMassType = eDMMTMuch;
				strDeepcatchMuscleMassType = "Preserved muscle mass";
			}
			else if (74.0f < skeletalMuscleArea && skeletalMuscleArea <= 87.0f)
			{
				patientMuscleMassType = eDMMTLittle;
				strDeepcatchMuscleMassType = "Borderline decreased muscle mass";
			}
			else if (skeletalMuscleArea <= 74.0f)
			{
				patientMuscleMassType = eDMMTLeast;
				strDeepcatchMuscleMassType = "Decreased muscle mass";
			}
		}
		break;
		}
	}

	// 내장 지방
	if (visceralFatArea > 100)
	{
		patientVisceralFatMassType = eDVFMTMuch;
		strDeepcatchVisceralFatMassType = "Increased visceral fat amount";
	}
	else
	{
		patientVisceralFatMassType = eDVFMTNormal;
		strDeepcatchVisceralFatMassType = "Normal visceral fat amount";
	}

	// muscle/fat status information insert.
	QString patientStatusDisplayTableHtmlStartTag = tr("");
	QString patientStatusDisplayTableHtmlEndTag = "</span>";
	QString patientStatusStrHtmlStartTag = tr("");
	QString patientStatusStrHtmlEndTag = "</span>";

	QString fatRatioResultHtmlStartTag = tr("");
	QString AbdominalCircumferenceResultHtmlStartTag = tr("");
	QString fatRatioResultHtmlEndTag = "</span></p>";

	switch (pReport->curA4Res)
	{
		//	eART96PPI:
		//	break;
	case eART150PPI:
	{
		patientStatusDisplayTableHtmlStartTag = "<span style=\"font-family:" "'Arial'" "; font-size:15pt; font-weight:normal; color:#000000;\">";
		patientStatusStrHtmlStartTag = "<span style=\"font-family:" "'Arial'" "; font-size:18pt; font-weight:normal; color:#535353;\">";
		fatRatioResultHtmlStartTag = "<p align=\"center\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-family:" "'Arial'" "; font-size:15pt; font-weight:normal; color:#000000;\">";
		AbdominalCircumferenceResultHtmlStartTag = "<p align=\"center\" style=\"margin-top:10px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-family:" "'Arial'" "; font-size:15pt; font-weight:normal; color:#000000;\">";
	}
	break;
	//	eART300PPI:
	//	break;
	case eART72PPI:
	default:
	{
		patientStatusDisplayTableHtmlStartTag = "<span style=\"font-family:" "'Arial'" "; font-size:8pt; font-weight:normal; color:#000000;\">";
		patientStatusStrHtmlStartTag = "<span style=\"font-family:" "'Arial'" "; font-size:9pt; font-weight:normal; color:#535353;\">";
		fatRatioResultHtmlStartTag = "<p align=\"center\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-family:" "'Arial'" "; font-size:8pt; font-weight:normal; color:#000000;\">";
		AbdominalCircumferenceResultHtmlStartTag = "<p align=\"center\" style=\"margin-top:5px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-family:" "'Arial'" "; font-size:8pt; font-weight:normal; color:#000000;\">";
	}
	break;
	}

	QTextCursor cursor(document());
	cursor.beginEditBlock();

	// sma
	cursor.setPosition(patientStatusDisplayTable->cellAt(1, 2).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(1, 2).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::number(ROUNDING(skeletalMuscleArea, 1), 'f', 1);
	patientStatusDisplayTable->cellAt(1, 2).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);

	// smi // sma/bmi
	cursor.setPosition(patientStatusDisplayTable->cellAt(1, 4).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(1, 4).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();

	cursor.setPosition(patientStatusDisplayTable->cellAt(1, 6).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(1, 6).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if (!bInvalidBMI)
	{
		// smi
		strTemp = QString::number(ROUNDING(skeletalMuscleIndices, 1), 'f', 1);
		patientStatusDisplayTable->cellAt(1, 4).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);

		// sma/bmi
		strTemp = QString::number(ROUNDING(skeletalMuscleBMI, 1), 'f', 1);
		patientStatusDisplayTable->cellAt(1, 6).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
	}

	// vfa
	cursor.setPosition(patientStatusDisplayTable->cellAt(2, 2).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(2, 2).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::number(ROUNDING(visceralFatArea, 1), 'f', 1);
	patientStatusDisplayTable->cellAt(2, 2).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);

	// vfi // vfa/bmi
	cursor.setPosition(patientStatusDisplayTable->cellAt(2, 4).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(2, 4).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();

	cursor.setPosition(patientStatusDisplayTable->cellAt(2, 6).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(2, 6).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();

	if (!bInvalidBMI)
	{
		// vfi
		strTemp = QString::number(ROUNDING(visceralFatIndices, 1), 'f', 1);
		patientStatusDisplayTable->cellAt(2, 4).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);

		// vfa/bmi
		strTemp = QString::number(ROUNDING(visceralFatBMI, 1), 'f', 1);
		patientStatusDisplayTable->cellAt(2, 6).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
	}

	// sfa
	cursor.setPosition(patientStatusDisplayTable->cellAt(3, 2).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(3, 2).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::number(ROUNDING(subcutaneousFatArea, 1), 'f', 1);
	patientStatusDisplayTable->cellAt(3, 2).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);

	// sfi // sfa/bmi
	cursor.setPosition(patientStatusDisplayTable->cellAt(3, 4).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(3, 4).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();

	cursor.setPosition(patientStatusDisplayTable->cellAt(3, 6).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(3, 6).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();

	if (!bInvalidBMI)
	{
		// sfi
		strTemp = QString::number(ROUNDING(subcutaneousFatIndices, 1), 'f', 1);
		patientStatusDisplayTable->cellAt(3, 4).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);

		// sfa/bmi
		strTemp = QString::number(ROUNDING(subcutaneousFatBMI, 1), 'f', 1);
		patientStatusDisplayTable->cellAt(3, 6).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
	}

#if 0
	// fat amount ratio
	cursor.setPosition(patientStatusDisplayTable->cellAt(5, 6).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(5, 6).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::number(fatAmountRatio) + QString("%");
	patientStatusDisplayTable->cellAt(5, 6).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
#else
	// fat amount ratio		
	// abdominal circumference (cm)
	cursor.setPosition(patientStatusDisplayTable->cellAt(5, 6).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(5, 6).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if (voxelCountAVF != 0 && voxelCountOf != 0)
		strTemp = fatRatioResultHtmlStartTag + QString::number(ROUNDING(fatAmountRatio, 1), 'f', 1) + QString("%") + fatRatioResultHtmlEndTag;
	else
		strTemp = fatRatioResultHtmlStartTag + QString("N/A") + fatRatioResultHtmlEndTag;
	if (pReport->fAbdomialCircumference != -1.0f && pReport->fAbdomialCircumference != 0.0f)
		strTemp += AbdominalCircumferenceResultHtmlStartTag + QString::number(ROUNDING(pReport->fAbdomialCircumference, 1), 'f', 1) + QString("cm") + fatRatioResultHtmlEndTag;
	else 
		strTemp += AbdominalCircumferenceResultHtmlStartTag + QString("N/A") + fatRatioResultHtmlEndTag;
	patientStatusDisplayTable->cellAt(5, 6).lastCursorPosition().insertHtml(strTemp);
#endif

	// sma 값에 따라 근육량 많고 적음 
	cursor.setPosition(patientStatusDisplayTable->cellAt(7, 8).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(7, 8).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	patientStatusDisplayTable->cellAt(7, 8).lastCursorPosition().insertHtml(patientStatusStrHtmlStartTag + strDeepcatchMuscleMassType + " / " + strDeepcatchVisceralFatMassType + patientStatusStrHtmlEndTag);

	cursor.endEditBlock();

	QString strImgPath = tr("");
	QString strImgFileName = tr("");
	if (patientMuscleMassType == eDMMTMuch && patientVisceralFatMassType == eDVFMTMuch)
		strImgFileName = "normalM_muchF.png";
	else if (patientMuscleMassType == eDMMTMuch && patientVisceralFatMassType == eDVFMTNormal)
		strImgFileName = "normalM_normalF.png";
	else if (patientMuscleMassType == eDMMTLittle && patientVisceralFatMassType == eDVFMTMuch)
		strImgFileName = "littleM_muchF.png";
	else if (patientMuscleMassType == eDMMTLittle && patientVisceralFatMassType == eDVFMTNormal)
		strImgFileName = "littleM_normalF.png";
	else if (patientMuscleMassType == eDMMTLeast && patientVisceralFatMassType == eDVFMTMuch)
		strImgFileName = "leastM_muchF.png";
	else if (patientMuscleMassType == eDMMTLeast && patientVisceralFatMassType == eDVFMTNormal)
		strImgFileName = "leastM_normalF.png";

	strImgPath = STRING_MANAGER->report_format + "/" + PRODUCT_NAME_DEEPCATCH + "/" + m_strCurLoadedReportSubDirName + "/" + strImgFileName;
	QImage statusImg = QImage(strImgPath);

	int wid = statusImg.width();
	int hei = statusImg.height();

	textCursor().beginEditBlock();
	patientStatusDisplayTable->cellAt(0, 8).firstCursorPosition().deleteChar();
	patientStatusDisplayTable->cellAt(0, 8).lastCursorPosition().insertImage(statusImg);
	textCursor().endEditBlock();

}
#else
void ReportTextEdit::muscleFatStateTableInfoInsert(QTextTable *patientStatusDisplayTable, DEEPCATCH_REPORT *pReport, QString strHeight, QString strWeight, int genderType, DEEPCATCH_REPORT_PREDICT_INFO *pPredictedInfo)
{
	/* 내장지방, 근육량에 따른 결과 표시 */
	QString strTemp = tr("");
	int voxelCountAVF = 0;
	int voxelCountMiscle = 0;
	int voxelCountOf = 0;

	// L3 or Abdominal Waist mask의 각 부위 voxel count가 invalid한 경우 예외 처리.
	//	if (pReport->mapDeepcatchClassToVolxelCount.isEmpty())
	//		QMessageBox::warning(this, "DeepCatch report", "Some of the report data(mapDeepcatchClassToVolxelCount is empty) is not valid.");

	QMap<eDeepCatchWholebodyClasses, int>::const_iterator iterMap = pReport->mapDeepcatchClassToVolxelCount.find(eDWMAVF);
	if (iterMap != pReport->mapDeepcatchClassToVolxelCount.end())
		voxelCountAVF = iterMap.value();
	iterMap = pReport->mapDeepcatchClassToVolxelCount.find(eDWMMuscle);
	if (iterMap != pReport->mapDeepcatchClassToVolxelCount.end())
		voxelCountMiscle = iterMap.value();
	iterMap = pReport->mapDeepcatchClassToVolxelCount.find(eDWMOF);
	if (iterMap != pReport->mapDeepcatchClassToVolxelCount.end())
		voxelCountOf = iterMap.value();

	// bmi calc
	float BMI = 0.0f;
	bool bInvalidBMI = false;
	float height_cm = strHeight.toFloat();			// cm
	float height_m = 0.0f;
	if (height_cm != 0.0f)
		height_m = height_cm / 100;		// m
	float weight_kg = strWeight.toFloat();			// kg
	if (height_cm == 0.0f || weight_kg == 0.0f)
		bInvalidBMI = true;
	if (!bInvalidBMI)
		BMI = weight_kg / (height_m*height_m);			//

	pReport->expertReportInfo.bInvalidBMI = bInvalidBMI;
	pReport->expertReportInfo.BMI = BMI;
	// SMA calc
	float skeletalMuscleArea = 0.0f;				// spacex * spacey * voxelCount
	float skeletalMuscleIndices = 0.0f;				// SMA/height의제곱
	float skeletalMuscleBMI = 0.0f;					// SMA/BMI
	float skeletalMuscleMass = 0.0f;
	if (voxelCountMiscle)
	{
		skeletalMuscleArea = DATA_CONTEXT->volume_data.getSpaceX() * DATA_CONTEXT->volume_data.getSpaceY() * voxelCountMiscle;
		if (!bInvalidBMI)
		{
			skeletalMuscleIndices = skeletalMuscleArea / (height_m*height_m);
			skeletalMuscleBMI = skeletalMuscleArea / BMI;
		}
		if (height_m != 0.0f)
			skeletalMuscleMass = (((voxelCountMiscle*DATA_CONTEXT->volume_data.getSpace3D(true))/1000)*1.06)/(height_m*height_m);
	}

	// VFA calc
	float visceralFatArea = 0.0f;					// spacex * spacey * voxelCount
	float visceralFatIndices = 0.0f;				// SMA/height의제곱
	float visceralFatBMI = 0.0f;					// SMA/BMI
	float visceralFatMass = 0.0f;
	if (voxelCountAVF)
	{
		visceralFatArea = DATA_CONTEXT->volume_data.getSpaceX() * DATA_CONTEXT->volume_data.getSpaceY() * voxelCountAVF;
		if (!bInvalidBMI)
		{
			visceralFatIndices = visceralFatArea / (height_m*height_m);
			visceralFatBMI = visceralFatArea / BMI;
		}
		if (height_m != 0.0f)
			visceralFatMass = (((voxelCountAVF*DATA_CONTEXT->volume_data.getSpace3D(true))/1000)*0.92)/(height_m*height_m);
	}

	// SFA calc
	float subcutaneousFatArea = 0.0f;				// spacex * spacey * voxelCount
	float subcutaneousFatIndices = 0.0f;			// SMA/height의제곱
	float subcutaneousFatBMI = 0.0f;				// SMA/BMI
	float subcutaneousFatMass = 0.0f;
	if (voxelCountOf)
	{
		subcutaneousFatArea = DATA_CONTEXT->volume_data.getSpaceX() * DATA_CONTEXT->volume_data.getSpaceY() * voxelCountOf;
		if (!bInvalidBMI)
		{
			subcutaneousFatIndices = subcutaneousFatArea / (height_m*height_m);
			subcutaneousFatBMI = subcutaneousFatArea / BMI;
		}
		if (height_m != 0.0f)
			subcutaneousFatMass = (((voxelCountOf*DATA_CONTEXT->volume_data.getSpace3D(true)) / 1000)*0.92) / (height_m*height_m);
	}

	// aw이면 mass 다시 계산.
	if (pPredictedInfo->stPredictOpt.preferenceType == eDPTMultiSlice)
	{
		QMap<eDeepCatchWholebodyClasses, int> &mapAbdominalWaistVoxelCount = pReport->expertReportInfo.mapAbdominalWaistVoxelCount;
		if (!mapAbdominalWaistVoxelCount.isEmpty())
		{
			int sumVoxelCountMiscle = 0;
			int sumVoxelCountAVF = 0;
			int sumVoxelCountOf = 0;
			QMap<eDeepCatchWholebodyClasses, int>::const_iterator iterMap;
			iterMap = mapAbdominalWaistVoxelCount.find(eDWMMuscle);
			if (iterMap != mapAbdominalWaistVoxelCount.end())
				sumVoxelCountMiscle = iterMap.value();
			iterMap = mapAbdominalWaistVoxelCount.find(eDWMAVF);
			if (iterMap != mapAbdominalWaistVoxelCount.end())
				sumVoxelCountAVF = iterMap.value();
			iterMap = mapAbdominalWaistVoxelCount.find(eDWMOF);
			if (iterMap != mapAbdominalWaistVoxelCount.end())
				sumVoxelCountOf = iterMap.value();
			if (height_m != 0.0f)
			{
				if (sumVoxelCountMiscle != 0.0f)
					skeletalMuscleMass = (((sumVoxelCountMiscle*DATA_CONTEXT->volume_data.getSpace3D(true))/1000)*1.06)/(height_m*height_m);
				if (sumVoxelCountAVF != 0.0f)
					visceralFatMass = (((sumVoxelCountAVF*DATA_CONTEXT->volume_data.getSpace3D(true))/1000)*0.92)/(height_m*height_m);
				if (sumVoxelCountOf != 0.0f)
					subcutaneousFatMass = (((sumVoxelCountOf*DATA_CONTEXT->volume_data.getSpace3D(true))/1000)*0.92)/(height_m*height_m);
			}
		}

	}

	// fat amount ratio calc
	float fatAmountRatio = 0.0f;
	//	fatAmountRatio = ((float)voxelCountAVF / ((float)voxelCountAVF + (float)voxelCountOf))*100.0f;
	fatAmountRatio = ((float)voxelCountAVF / (float)voxelCountOf)*100.0f;

	// 값에 따른 판단
	eDeepcatchMuscleMassType patientMuscleMassType = eDMMTNone;
	eDeepcatchVisceralFatMassType patientVisceralFatMassType = eDVFMTNone;
	QString strDeepcatchMuscleMassType = tr("");
	QString strDeepcatchVisceralFatMassType = tr("");

	// 근육량
	if (!bInvalidBMI)
	{
		switch (genderType)
		{
		case eDGTMale:
		{
			if ((skeletalMuscleArea > 140.0f) && (skeletalMuscleIndices > 47.0f) && (skeletalMuscleBMI > 5.7f))
			{
				patientMuscleMassType = eDMMTMuch;
				strDeepcatchMuscleMassType = "Preserved muscle mass";
			}
			else if ((119.0f < skeletalMuscleArea && skeletalMuscleArea <= 140.0f) &&
				(40.0f < skeletalMuscleIndices && skeletalMuscleIndices <= 47.0f) &&
				(5.0f < skeletalMuscleBMI && skeletalMuscleBMI <= 5.7f))
			{
				patientMuscleMassType = eDMMTLittle;
				strDeepcatchMuscleMassType = "Borderline decreased muscle mass";
			}
			else if (skeletalMuscleArea <= 119.0f && skeletalMuscleIndices <= 40.0f && skeletalMuscleBMI <= 5.0f)
			{
				patientMuscleMassType = eDMMTLeast;
				strDeepcatchMuscleMassType = "Decreased muscle mass";
			}
		}
		break;
		case eDGTFemale:
		{
			if (skeletalMuscleArea > 87.0f && skeletalMuscleIndices > 34.0f && skeletalMuscleBMI > 4.1f)
			{
				patientMuscleMassType = eDMMTMuch;
				strDeepcatchMuscleMassType = "Preserved muscle mass";
			}
			else if ((74.0f < skeletalMuscleArea && skeletalMuscleArea <= 87.0f) &&
				(28.0f < skeletalMuscleIndices && skeletalMuscleIndices <= 34.0f) &&
				(3.5f < skeletalMuscleBMI && skeletalMuscleBMI <= 4.1f))
			{
				patientMuscleMassType = eDMMTLittle;
				strDeepcatchMuscleMassType = "Borderline decreased muscle mass";
			}
			else if (skeletalMuscleArea <= 74.0f && skeletalMuscleIndices <= 28.0f && skeletalMuscleBMI <= 3.5f)
			{
				patientMuscleMassType = eDMMTLeast;
				strDeepcatchMuscleMassType = "Decreased muscle mass";
			}
		}
		break;
		}
	}

	// area && cm2/height(m2) && cm2/BMI 조건으로 필터링 안되는 경우 area로 처리.
	bool bNotFind = false;
	if (patientMuscleMassType == eDMMTNone && strDeepcatchMuscleMassType.isEmpty())
		bNotFind = true;

	if (bNotFind)
	{
		switch (genderType)
		{
		case eDGTMale:
		{
			if (skeletalMuscleArea > 140.0f)
			{
				patientMuscleMassType = eDMMTMuch;
				strDeepcatchMuscleMassType = "Preserved muscle mass";
			}
			else if (119.0f < skeletalMuscleArea && skeletalMuscleArea <= 140.0f)
			{
				patientMuscleMassType = eDMMTLittle;
				strDeepcatchMuscleMassType = "Borderline decreased muscle mass";
			}
			else if (skeletalMuscleArea <= 119.0f)
			{
				patientMuscleMassType = eDMMTLeast;
				strDeepcatchMuscleMassType = "Decreased muscle mass";
			}
		}
		break;
		case eDGTFemale:
		{
			if (skeletalMuscleArea > 87.0f)
			{
				patientMuscleMassType = eDMMTMuch;
				strDeepcatchMuscleMassType = "Preserved muscle mass";
			}
			else if (74.0f < skeletalMuscleArea && skeletalMuscleArea <= 87.0f)
			{
				patientMuscleMassType = eDMMTLittle;
				strDeepcatchMuscleMassType = "Borderline decreased muscle mass";
			}
			else if (skeletalMuscleArea <= 74.0f)
			{
				patientMuscleMassType = eDMMTLeast;
				strDeepcatchMuscleMassType = "Decreased muscle mass";
			}
		}
		break;
		}
	}

	// 내장 지방
	if (visceralFatArea > 100)
	{
		patientVisceralFatMassType = eDVFMTMuch;
		strDeepcatchVisceralFatMassType = "Increased visceral fat amount";
	}
	else
	{
		patientVisceralFatMassType = eDVFMTNormal;
		strDeepcatchVisceralFatMassType = "Normal visceral fat amount";
	}

	// muscle/fat status information insert.
	QString patientStatusDisplayTableHtmlStartTag = tr("");
	QString patientStatusDisplayTableHtmlEndTag = "</span>";
	QString patientStatusStrHtmlStartTag = tr("");
	QString patientStatusStrHtmlEndTag = "</span>";

	QString fatRatioResultHtmlStartTag = tr("");
	QString AbdominalCircumferenceResultHtmlStartTag = tr("");
	QString fatRatioResultHtmlEndTag = "</span></p>";

	switch (pReport->curA4Res)
	{
		//	eART96PPI:
		//	break;
	case eART150PPI:
	{
		patientStatusDisplayTableHtmlStartTag = "<span style=\"font-family:" "'Arial'" "; font-size:15pt; font-weight:normal; color:#000000;\">";
		patientStatusStrHtmlStartTag = "<span style=\"font-family:" "'Arial'" "; font-size:18pt; font-weight:normal; color:#535353;\">";
		fatRatioResultHtmlStartTag = "<p align=\"center\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-family:" "'Arial'" "; font-size:15pt; font-weight:normal; color:#000000;\">";
		AbdominalCircumferenceResultHtmlStartTag = "<p align=\"center\" style=\"margin-top:10px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-family:" "'Arial'" "; font-size:15pt; font-weight:normal; color:#000000;\">";
	}
	break;
	//	eART300PPI:
	//	break;
	case eART72PPI:
	default:
	{
		patientStatusDisplayTableHtmlStartTag = "<span style=\"font-family:" "'Arial'" "; font-size:8pt; font-weight:normal; color:#000000;\">";
		patientStatusStrHtmlStartTag = "<span style=\"font-family:" "'Arial'" "; font-size:9pt; font-weight:normal; color:#535353;\">";
		fatRatioResultHtmlStartTag = "<p align=\"center\" style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-family:" "'Arial'" "; font-size:8pt; font-weight:normal; color:#000000;\">";
		AbdominalCircumferenceResultHtmlStartTag = "<p align=\"center\" style=\"margin-top:5px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\"font-family:" "'Arial'" "; font-size:8pt; font-weight:normal; color:#000000;\">";
	}
	break;
	}

	QTextCharFormat muscleFatStateTable_txtCharFormat;
	QFont font("Arial", 15, QFont::Normal);
	muscleFatStateTable_txtCharFormat.setFont(font);

	QTextCursor cursor(document());
	cursor.beginEditBlock();

	// sma
	cursor.setPosition(patientStatusDisplayTable->cellAt(1, 2).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(1, 2).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::number(ROUNDING(skeletalMuscleArea, 1), 'f', 1);
	if (strTemp.isEmpty())
		strTemp = " ";
//	patientStatusDisplayTable->cellAt(1, 2).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
	patientStatusDisplayTable->cellAt(1, 2).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);

	// sma mass
	cursor.setPosition(patientStatusDisplayTable->cellAt(1, 8).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(1, 8).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if (height_m != 0.0f && skeletalMuscleMass != 0.0f)
	{
		strTemp = QString::number(ROUNDING(skeletalMuscleMass, 1), 'f', 1);
		if (strTemp.isEmpty())
			strTemp = " ";
		patientStatusDisplayTable->cellAt(1, 8).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);
	}
	else
	{
		strTemp = " ";
		patientStatusDisplayTable->cellAt(1, 8).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);
	}

	if (!bInvalidBMI)
	{
		// smi
		cursor.setPosition(patientStatusDisplayTable->cellAt(1, 4).firstPosition());
		cursor.setPosition(patientStatusDisplayTable->cellAt(1, 4).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		strTemp = QString::number(ROUNDING(skeletalMuscleIndices, 1), 'f', 1);
		if (strTemp.isEmpty())
			strTemp = " ";
	//	patientStatusDisplayTable->cellAt(1, 4).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
		patientStatusDisplayTable->cellAt(1, 4).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);

		// sma/bmi
		cursor.setPosition(patientStatusDisplayTable->cellAt(1, 6).firstPosition());
		cursor.setPosition(patientStatusDisplayTable->cellAt(1, 6).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		strTemp = QString::number(ROUNDING(skeletalMuscleBMI, 1), 'f', 1);
		if (strTemp.isEmpty())
			strTemp = " ";
	//	patientStatusDisplayTable->cellAt(1, 6).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
		patientStatusDisplayTable->cellAt(1, 6).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);
	}
	else
	{
		// smi
		cursor.setPosition(patientStatusDisplayTable->cellAt(1, 4).firstPosition());
		cursor.setPosition(patientStatusDisplayTable->cellAt(1, 4).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		strTemp = " ";
	//	patientStatusDisplayTable->cellAt(1, 4).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
		patientStatusDisplayTable->cellAt(1, 4).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);

		// sma/bmi
		cursor.setPosition(patientStatusDisplayTable->cellAt(1, 6).firstPosition());
		cursor.setPosition(patientStatusDisplayTable->cellAt(1, 6).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		strTemp = " ";
	//	patientStatusDisplayTable->cellAt(1, 6).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
		patientStatusDisplayTable->cellAt(1, 6).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);
	}

	// vfa
	cursor.setPosition(patientStatusDisplayTable->cellAt(2, 2).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(2, 2).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::number(ROUNDING(visceralFatArea, 1), 'f', 1);
	if (strTemp.isEmpty())
		strTemp = " ";
//	patientStatusDisplayTable->cellAt(2, 2).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
	patientStatusDisplayTable->cellAt(2, 2).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);

	// vfa mass
	cursor.setPosition(patientStatusDisplayTable->cellAt(2, 8).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(2, 8).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if (height_m != 0.0f && visceralFatMass != 0.0f)
	{
		strTemp = QString::number(ROUNDING(visceralFatMass, 1), 'f', 1);
		if (strTemp.isEmpty())
			strTemp = " ";
		patientStatusDisplayTable->cellAt(2, 8).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);
	}
	else
	{
		strTemp = " ";
		patientStatusDisplayTable->cellAt(2, 8).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);
	}

	if (!bInvalidBMI)
	{
		// vfi
		cursor.setPosition(patientStatusDisplayTable->cellAt(2, 4).firstPosition());
		cursor.setPosition(patientStatusDisplayTable->cellAt(2, 4).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		strTemp = QString::number(ROUNDING(visceralFatIndices, 1), 'f', 1);
		if (strTemp.isEmpty())
			strTemp = " ";
	//	patientStatusDisplayTable->cellAt(2, 4).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
		patientStatusDisplayTable->cellAt(2, 4).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);

		// vfa/bmi
		cursor.setPosition(patientStatusDisplayTable->cellAt(2, 6).firstPosition());
		cursor.setPosition(patientStatusDisplayTable->cellAt(2, 6).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		strTemp = QString::number(ROUNDING(visceralFatBMI, 1), 'f', 1);
		if (strTemp.isEmpty())
			strTemp = " ";
	//	patientStatusDisplayTable->cellAt(2, 6).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
		patientStatusDisplayTable->cellAt(2, 6).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);
	}
	else
	{
		// vfi
		cursor.setPosition(patientStatusDisplayTable->cellAt(2, 4).firstPosition());
		cursor.setPosition(patientStatusDisplayTable->cellAt(2, 4).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		strTemp = " ";
	//	patientStatusDisplayTable->cellAt(2, 4).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
		patientStatusDisplayTable->cellAt(2, 4).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);

		// vfa/bmi
		cursor.setPosition(patientStatusDisplayTable->cellAt(2, 6).firstPosition());
		cursor.setPosition(patientStatusDisplayTable->cellAt(2, 6).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		strTemp = " ";
	//	patientStatusDisplayTable->cellAt(2, 6).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
		patientStatusDisplayTable->cellAt(2, 6).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);
	}

	// sfa
	cursor.setPosition(patientStatusDisplayTable->cellAt(3, 2).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(3, 2).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	strTemp = QString::number(ROUNDING(subcutaneousFatArea, 1), 'f', 1);
	if (strTemp.isEmpty())
		strTemp = " ";
//	patientStatusDisplayTable->cellAt(3, 2).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
	patientStatusDisplayTable->cellAt(3, 2).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);

	// sfa mass
	cursor.setPosition(patientStatusDisplayTable->cellAt(3, 8).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(3, 8).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if (height_m != 0.0f && subcutaneousFatMass != 0.0f)
	{
		strTemp = QString::number(ROUNDING(subcutaneousFatMass, 1), 'f', 1);
		if (strTemp.isEmpty())
			strTemp = " ";
		patientStatusDisplayTable->cellAt(3, 8).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);
	}
	else
	{
		strTemp = " ";
		patientStatusDisplayTable->cellAt(3, 8).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);
	}

	if (!bInvalidBMI)
	{
		// sfi
		cursor.setPosition(patientStatusDisplayTable->cellAt(3, 4).firstPosition());
		cursor.setPosition(patientStatusDisplayTable->cellAt(3, 4).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		strTemp = QString::number(ROUNDING(subcutaneousFatIndices, 1), 'f', 1);
		if (strTemp.isEmpty())
			strTemp = " ";
	//	patientStatusDisplayTable->cellAt(3, 4).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
		patientStatusDisplayTable->cellAt(3, 4).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);

		// sfa/bmi
		cursor.setPosition(patientStatusDisplayTable->cellAt(3, 6).firstPosition());
		cursor.setPosition(patientStatusDisplayTable->cellAt(3, 6).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		strTemp = QString::number(ROUNDING(subcutaneousFatBMI, 1), 'f', 1);
		if (strTemp.isEmpty())
			strTemp = " ";
	//	patientStatusDisplayTable->cellAt(3, 6).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
		patientStatusDisplayTable->cellAt(3, 6).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);
	}
	else
	{
		cursor.setPosition(patientStatusDisplayTable->cellAt(3, 4).firstPosition());
		cursor.setPosition(patientStatusDisplayTable->cellAt(3, 4).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		strTemp = " ";
	//	patientStatusDisplayTable->cellAt(3, 4).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
		patientStatusDisplayTable->cellAt(3, 4).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);

		// sfa/bmi
		cursor.setPosition(patientStatusDisplayTable->cellAt(3, 6).firstPosition());
		cursor.setPosition(patientStatusDisplayTable->cellAt(3, 6).lastPosition(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		strTemp = " ";
	//	patientStatusDisplayTable->cellAt(3, 6).lastCursorPosition().insertHtml(patientStatusDisplayTableHtmlStartTag + strTemp + patientStatusDisplayTableHtmlEndTag);
		patientStatusDisplayTable->cellAt(3, 6).lastCursorPosition().insertText(strTemp, muscleFatStateTable_txtCharFormat);
	}

	// fat amount ratio		
	// abdominal circumference (cm)
	cursor.setPosition(patientStatusDisplayTable->cellAt(5, 8).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(5, 8).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if (voxelCountAVF != 0 && voxelCountOf != 0)
		strTemp = fatRatioResultHtmlStartTag + QString::number(ROUNDING(fatAmountRatio, 1), 'f', 1) + QString("%") + fatRatioResultHtmlEndTag;
	else 
		strTemp = fatRatioResultHtmlStartTag + QString("N/A") + fatRatioResultHtmlEndTag;
	if (pReport->fAbdomialCircumference != -1.0f && pReport->fAbdomialCircumference != 0.0f)
		strTemp += AbdominalCircumferenceResultHtmlStartTag + QString::number(ROUNDING(pReport->fAbdomialCircumference, 1), 'f', 1) + QString("cm") + fatRatioResultHtmlEndTag;
	else
		strTemp += AbdominalCircumferenceResultHtmlStartTag + QString("N/A") + fatRatioResultHtmlEndTag;
	patientStatusDisplayTable->cellAt(5, 8).lastCursorPosition().insertHtml(strTemp);

	// sma 값에 따라 근육량 많고 적음 
	cursor.setPosition(patientStatusDisplayTable->cellAt(7, 10).firstPosition());
	cursor.setPosition(patientStatusDisplayTable->cellAt(7, 10).lastPosition(), QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	patientStatusDisplayTable->cellAt(7, 10).lastCursorPosition().insertHtml(patientStatusStrHtmlStartTag + strDeepcatchMuscleMassType + " / " + strDeepcatchVisceralFatMassType + patientStatusStrHtmlEndTag);

	cursor.endEditBlock();

	QString strImgPath = tr("");
	QString strImgFileName = tr("");
	if (patientMuscleMassType == eDMMTMuch && patientVisceralFatMassType == eDVFMTMuch)
		strImgFileName = "normalM_muchF.png";
	else if (patientMuscleMassType == eDMMTMuch && patientVisceralFatMassType == eDVFMTNormal)
		strImgFileName = "normalM_normalF.png";
	else if (patientMuscleMassType == eDMMTLittle && patientVisceralFatMassType == eDVFMTMuch)
		strImgFileName = "littleM_muchF.png";
	else if (patientMuscleMassType == eDMMTLittle && patientVisceralFatMassType == eDVFMTNormal)
		strImgFileName = "littleM_normalF.png";
	else if (patientMuscleMassType == eDMMTLeast && patientVisceralFatMassType == eDVFMTMuch)
		strImgFileName = "leastM_muchF.png";
	else if (patientMuscleMassType == eDMMTLeast && patientVisceralFatMassType == eDVFMTNormal)
		strImgFileName = "leastM_normalF.png";

	strImgPath = STRING_MANAGER->report_format + "/" + PRODUCT_NAME_DEEPCATCH + "/" + m_strCurLoadedReportSubDirName + "/" + strImgFileName;
	QImage statusImg = QImage(strImgPath);

	textCursor().beginEditBlock();
	patientStatusDisplayTable->cellAt(0, 10).firstCursorPosition().deleteChar();
//	patientStatusDisplayTable->cellAt(0, 10).lastCursorPosition().insertImage(statusImg);
	QUrl muscleFatStatusImageUrl("muscleFatStatusImage.png");
	document()->addResource(QTextDocument::ImageResource, muscleFatStatusImageUrl, statusImg);
	QTextImageFormat muscleFatStatusImgFmt;
	muscleFatStatusImgFmt.setName(muscleFatStatusImageUrl.url());
	muscleFatStatusImgFmt.setWidth(statusImg.width());
	muscleFatStatusImgFmt.setHeight(statusImg.height());
	patientStatusDisplayTable->cellAt(0, 10).lastCursorPosition().insertImage(muscleFatStatusImgFmt);
	textCursor().endEditBlock();

}
#endif

void ReportTextEdit::zoom(float zoomingOneStepFactor)
{
	QTextCursor cursor(document());
	cursor.movePosition(QTextCursor::Start);
	cursor.movePosition(QTextCursor::StartOfBlock);

	// block의 시작이자 end이면 예외 처리.(테이블로 시작하는 경우)
	if (cursor.atBlockStart() && cursor.atBlockEnd())
		cursor.movePosition(QTextCursor::NextBlock);

	qreal curFontSize = 0;
	QTextTable *preTable = nullptr;
	QTextTable *curTable = nullptr;
	// table을 제외한 block에 대한 처리.
	while (cursor.movePosition(QTextCursor::EndOfBlock))
	{
		// 테이블이면
		curTable = cursor.currentTable();
		if (curTable)
		{
			if (curTable != preTable)
			{
				// table format
				QTextTableFormat tableFormat = curTable->format();
				tableFormat.setTopMargin(tableFormat.topMargin()*zoomingOneStepFactor);
				tableFormat.setBottomMargin(tableFormat.bottomMargin()*zoomingOneStepFactor);
				tableFormat.setLeftMargin(tableFormat.leftMargin()*zoomingOneStepFactor);
				tableFormat.setRightMargin(tableFormat.rightMargin()*zoomingOneStepFactor);
			//	tableFormat.setPadding(tableFormat.padding()*zoomingOneStepFactor);
			//	tableFormat.setCellPadding(tableFormat.cellPadding()*zoomingOneStepFactor);
			//	tableFormat.setCellSpacing(tableFormat.cellSpacing()*zoomingOneStepFactor);
				curTable->setFormat(tableFormat);
			//	qDebug() << "table margin : " << tableFormat.topMargin();

				QTextCursor tableCursor = curTable->rowStart(cursor);
				while (tableCursor.movePosition(QTextCursor::NextBlock) && tableCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor))
				{
					// text char format
					QTextCharFormat chrTextCharFormat = tableCursor.charFormat();
					if (chrTextCharFormat.isImageFormat())
					{
						QTextImageFormat imgFmt = chrTextCharFormat.toImageFormat();
						imgFmt.setWidth(imgFmt.width()*zoomingOneStepFactor);
						imgFmt.setHeight(imgFmt.height()*zoomingOneStepFactor);
						tableCursor.mergeCharFormat(imgFmt);
					//	mergeCurrentCharFormat(imgFmt);
					}
					else
					{
						curFontSize = chrTextCharFormat.fontPointSize();
					//	QString temp = tableCursor.selectedText();
					//	qDebug() << tableCursor.selectedText() << " : " << curFontSize;
						chrTextCharFormat.setFontPointSize(curFontSize*zoomingOneStepFactor);
						tableCursor.mergeCharFormat(chrTextCharFormat);
					//	mergeCurrentCharFormat(chrTextCharFormat);
					}

					// block format
					QTextBlockFormat chrTextBlockFormat = tableCursor.blockFormat();
				//	qDebug() << tableCursor.selectedText() << " : " << chrTextBlockFormat.leftMargin();
					chrTextBlockFormat.setTopMargin(chrTextBlockFormat.topMargin()*zoomingOneStepFactor);
					chrTextBlockFormat.setBottomMargin(chrTextBlockFormat.bottomMargin()*zoomingOneStepFactor);
					chrTextBlockFormat.setLeftMargin(chrTextBlockFormat.leftMargin()*zoomingOneStepFactor);
					chrTextBlockFormat.setRightMargin(chrTextBlockFormat.rightMargin()*zoomingOneStepFactor);
					tableCursor.mergeBlockFormat(chrTextBlockFormat);
				//	tableCursor.setBlockFormat(chrTextBlockFormat);

					// table cell format
					QTextTableCell curCell = curTable->cellAt(tableCursor);
					QTextCharFormat format = curCell.format();
					if (format.isTableCellFormat())
					{
						QTextTableCellFormat cellFormat = format.toTableCellFormat();
					//	if (cellFormat.topPadding() != qreal(0))
					//		qDebug() << tableCursor.selectedText() << " : " << cellFormat.topPadding();
						cellFormat.setTopPadding(cellFormat.topPadding()*zoomingOneStepFactor);
						cellFormat.setBottomPadding(cellFormat.bottomPadding()*zoomingOneStepFactor);
						cellFormat.setLeftPadding(cellFormat.leftPadding()*zoomingOneStepFactor);
						cellFormat.setRightPadding(cellFormat.rightPadding()*zoomingOneStepFactor);
						curCell.setFormat(cellFormat);
					}
				}
				preTable = curTable;
				if (!cursor.hasSelection())
					cursor.select(QTextCursor::BlockUnderCursor);
			}
		}
		else
		{
			if (!cursor.hasSelection())
				cursor.select(QTextCursor::BlockUnderCursor);

			// text char format
			QTextCharFormat chrTextCharFormat = cursor.charFormat();
			if (chrTextCharFormat.isImageFormat())
			{
				QTextImageFormat imgFmt = chrTextCharFormat.toImageFormat();
				imgFmt.setWidth(imgFmt.width()*zoomingOneStepFactor);
				imgFmt.setHeight(imgFmt.height()*zoomingOneStepFactor);
				cursor.mergeCharFormat(imgFmt);
			//	mergeCurrentCharFormat(imgFmt);
			}
			else 
			{
				curFontSize = chrTextCharFormat.fontPointSize();
				//	qDebug() << cursor.selectedText() << " : " << curFontSize;
				chrTextCharFormat.setFontPointSize(curFontSize*zoomingOneStepFactor);
				cursor.mergeCharFormat(chrTextCharFormat);
			//	mergeCurrentCharFormat(chrTextCharFormat);
			}

			// block format
			QTextBlockFormat chrTextBlockFormat = cursor.blockFormat();
		//	qDebug() << cursor.selectedText() << " : " << chrTextBlockFormat.leftMargin();
			chrTextBlockFormat.setTopMargin(chrTextBlockFormat.topMargin()*zoomingOneStepFactor);
			chrTextBlockFormat.setBottomMargin(chrTextBlockFormat.bottomMargin()*zoomingOneStepFactor);
			chrTextBlockFormat.setLeftMargin(chrTextBlockFormat.leftMargin()*zoomingOneStepFactor);
			chrTextBlockFormat.setRightMargin(chrTextBlockFormat.rightMargin()*zoomingOneStepFactor);
			cursor.mergeBlockFormat(chrTextBlockFormat);
		//	cursor.setBlockFormat(chrTextBlockFormat);
		}
		cursor.movePosition(QTextCursor::NextBlock);
	}
}

void ReportTextEdit::initZoom()
{
	float compareValue = zoomScaleFactor;
	float restoreOneStepZoomFactor = 1.0f;
	// zoom out된 경우
	if (zoomScaleFactor < 0.9f)
	{
		while (compareValue < 0.9f)
		{
			compareValue *= ONE_STEP_ZOOM_IN_SCALE_FACTOR;
			restoreOneStepZoomFactor *= ONE_STEP_ZOOM_IN_SCALE_FACTOR;
		}
		zoom(restoreOneStepZoomFactor);
		zoomScaleFactor = 1.0f;

		setFixedSize(REAL_FIXED_DOCUMENT_SIZE);
	//	setLineWrapMode(QTextEdit::FixedPixelWidth);
		setLineWrapColumnOrWidth(static_cast<int>(REAL_FIXED_DOCUMENT_SIZE.width()));
		setFixedWidth(REAL_FIXED_DOCUMENT_SIZE.width());
		document()->setPageSize(REAL_FIXED_DOCUMENT_SIZE);
	}
	// zoom in된 경우
	else if (zoomScaleFactor > 1.1f)
	{
		while (compareValue > 1.1f)
		{
			compareValue *= ONE_STEP_ZOOM_OUT_SCALE_FACTOR;
			restoreOneStepZoomFactor *= ONE_STEP_ZOOM_OUT_SCALE_FACTOR;
		}
		zoom(restoreOneStepZoomFactor);
		zoomScaleFactor = 1.0f;

		setFixedSize(REAL_FIXED_DOCUMENT_SIZE);
	//	setLineWrapMode(QTextEdit::FixedPixelWidth);
		setLineWrapColumnOrWidth(static_cast<int>(REAL_FIXED_DOCUMENT_SIZE.width()));
		setFixedWidth(REAL_FIXED_DOCUMENT_SIZE.width());
		document()->setPageSize(REAL_FIXED_DOCUMENT_SIZE);
	}
	else // 초기상태인 경우 리턴.
		return;
}