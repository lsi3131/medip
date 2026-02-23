#include "stdafx.h"
#include "FileViewMode.h"

namespace fm
{
	FileViewMode::FileViewMode() :
		m_canSelectMultiItem(false),
		m_itemExpandable(false),
		m_openSaveMode(EOpenSaveMode::open),
		m_selectMode(EFileSelectMode::file)
	{
	}
	FileViewMode::FileViewMode(
		bool canSelectMultiItem, 
		bool itemExpandable, 
		EOpenSaveMode openSaveMode, 
		EFileSelectMode selectMode, 
		QString filters,
		QString defaultSelectFilter) :
		m_canSelectMultiItem(canSelectMultiItem),
		m_itemExpandable(itemExpandable),
		m_openSaveMode(openSaveMode),
		m_selectMode(selectMode),
		m_filters(filters),
		m_defaultSelectFilter(defaultSelectFilter)
	{
		m_filters = RemoveFilterLastSemicolon(m_filters);
	}

	void FileViewMode::SetSelectMultiItem(bool value)
	{
		m_canSelectMultiItem = value;
	}

	void FileViewMode::SetFileFilterMode(EFileSelectMode mode)
	{
		m_selectMode = mode;
	}

	void FileViewMode::SetFilters(const QString& filter)
	{
		m_filters = RemoveFilterLastSemicolon(filter);
	}

	void FileViewMode::SetDefaultSelectFilter(const QString& defaultSelectFilter)
	{
		m_defaultSelectFilter = defaultSelectFilter;
	}

	bool fm::FileViewMode::CanSelectMultiItem() const
	{
		return m_canSelectMultiItem;
	}

	bool fm::FileViewMode::ItemExpandable() const
	{
		return m_itemExpandable;
	}

	FileViewMode::EOpenSaveMode fm::FileViewMode::OpenSaveMode() const
	{
		return m_openSaveMode;
	}

	FileViewMode::EFileSelectMode fm::FileViewMode::FileSelectMode() const
	{
		return m_selectMode;
	}

	QString FileViewMode::Filters() const
	{
		return m_filters;
	}

	QString FileViewMode::GetDefaultSelectFilter() const
	{
		return m_defaultSelectFilter;
	}

	bool FileViewMode::CanSelectFile() const
	{
		return (m_selectMode & file);
	}

	bool FileViewMode::CanSelectDirectory() const
	{
		return (m_selectMode & directory);
	}

	bool FileViewMode::IsOnlyDirectorySelectMode() const
	{
		return (m_selectMode == directory);
	}

	QString FileViewMode::RemoveFilterLastSemicolon(QString text) const
	{
		QString textRemoveLastSemicolon = text;
		if (text.endsWith(";;"))
		{
			int lastIndex = text.lastIndexOf(";;");
			textRemoveLastSemicolon = text.left(lastIndex);
		}
		return textRemoveLastSemicolon;
	}
}
