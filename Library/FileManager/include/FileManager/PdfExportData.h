#pragma once

#include <memory>
#include <QTextDocument>
#include "filemanager/export.h"

class QPrinter;

namespace fm
{
	class PdfExportData;
	class PdfExportDataPrivate;

	using PdfExportDataPtr = std::shared_ptr<PdfExportData>;

	class FM_CORE_EXPORT PdfExportData
	{
	public:
		static PdfExportDataPtr Create(QTextDocument* pDoc, std::shared_ptr<QPrinter> pPrinter);

	public:
		PdfExportData(QTextDocument* pDoc, std::shared_ptr<QPrinter> pPrinter);

	public:
		QTextDocument* GetPdfDocument() const;
		QPrinter* GetPrinter() const;

	public:
		std::shared_ptr<PdfExportDataPrivate> m_p;
	};
}
