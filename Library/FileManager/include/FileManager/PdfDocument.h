#pragma once

#include <memory>
#include <QTextDocument>
#include <string>
#include "filemanager/export.h"

class QPrinter;

namespace fm
{
	class PdfDocument;
	class PdfDocumentPrivate;

	using PdfDocumentPtr = std::shared_ptr<PdfDocument>;

	class FM_CORE_EXPORT PdfDocument
	{
	public:
		static PdfDocumentPtr Create(QTextDocument* pDoc, std::shared_ptr<QPrinter> pPrinter);
		static PdfDocumentPtr CreateWithDefaultPrinter(QTextDocument* pDoc);

	private:
		PdfDocument(QTextDocument* pDoc, std::shared_ptr<QPrinter> pPrinter);
	public:
		~PdfDocument();

	public:
		QTextDocument* GetDocument() const;
		QPrinter* GetPrinter() const;

		bool SaveTempFile(const std::wstring& saveDirPath, std::wstring* pOutTempPdfFilePath = nullptr);

		std::wstring GetTempFilePath() const;
		std::wstring GetTempFileName() const;

	private:
		std::wstring generatorTempFileName(const std::wstring& saveDirPath) const;

	public:
		std::shared_ptr<PdfDocumentPrivate> m_p;
	};
}
