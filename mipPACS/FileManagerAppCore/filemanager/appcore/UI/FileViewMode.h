#pragma once

#include "filemanager/export.h"

namespace fm
{
	class FM_CORE_EXPORT FileViewMode
	{
	public:
		enum EOpenSaveMode
		{
			open,
			save
		};

		enum EFileSelectMode
		{
			file = 0x01,
			directory = 0x02,
			file_and_directory = file | directory,
		};

	public:
		FileViewMode();

		FileViewMode(
			bool canSelectMultiItem,
			bool itemExpandable,
			EOpenSaveMode openSaveMode,
			EFileSelectMode selectMode, 
			QString filters,
			QString defaultSelectFilter = QString("")
		);

	public:
		void SetSelectMultiItem(bool value);
		void SetFileFilterMode(EFileSelectMode mode);

		void SetFilters(const QString& filter);
		void SetDefaultSelectFilter(const QString& defaultSelectFilter);

		bool CanSelectMultiItem() const;
		bool ItemExpandable() const;
		EOpenSaveMode OpenSaveMode() const;
		EFileSelectMode FileSelectMode() const;

		QString Filters() const;
		QString GetDefaultSelectFilter() const;

		bool CanSelectFile() const;
		bool CanSelectDirectory() const;
		bool IsOnlyDirectorySelectMode() const;

	private:
		QString RemoveFilterLastSemicolon(QString text) const;

	private:
		bool m_canSelectMultiItem;
		bool m_itemExpandable;
		EOpenSaveMode m_openSaveMode;
		EFileSelectMode m_selectMode;

		QString m_filters;
		QString m_defaultSelectFilter;
	};
}
