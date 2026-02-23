#include "stdafx.h"
#include "ColumnHeaderInfoList.h"

namespace fm
{
	void ColumnHeaderInfoList::Add(int index, QString name, bool isVisible)
	{
		ColumnHeaderInfo info;
		info.Index = index;
		info.Name = name;
		info.IsVisible = isVisible;
		m_columnHeaderInfoList.push_back(info);
	}

	bool ColumnHeaderInfoList::SetVisible(int index, bool value)
	{
		if (index < 0 || index >= m_columnHeaderInfoList.size())
		{
			return false;
		}
		m_columnHeaderInfoList[index].IsVisible = value;
		return true;
	}

	int ColumnHeaderInfoList::Count()
	{
		return m_columnHeaderInfoList.size();
	}

	QStringList ColumnHeaderInfoList::GetNameHeaderList()
	{
		QStringList columnHeaderList;
		for (int i = 0; i < m_columnHeaderInfoList.size(); ++i)
		{
			columnHeaderList << m_columnHeaderInfoList[i].Name;
		}
		return columnHeaderList;
	}

	std::vector<int> ColumnHeaderInfoList::GetInvisibleHeaderIndexList()
	{
		std::vector<int> invisibleIndexList;
		for (int idx = 0; idx < m_columnHeaderInfoList.size(); ++idx)
		{
			if (!m_columnHeaderInfoList[idx].IsVisible)
			{
				invisibleIndexList.push_back(idx);
			}
		}
		return invisibleIndexList;
	}

	std::vector<int> ColumnHeaderInfoList::GetVisibleHeaderIndexList()
	{
		std::vector<int> invisibleIndexList;
		for (int idx = 0; idx < m_columnHeaderInfoList.size(); ++idx)
		{
			if (m_columnHeaderInfoList[idx].IsVisible)
			{
				invisibleIndexList.push_back(idx);
			}
		}
		return invisibleIndexList;
	}
}
