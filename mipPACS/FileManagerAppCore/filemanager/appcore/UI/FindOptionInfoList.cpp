#include "stdafx.h"
#include "FindOptionInfoList.h"

namespace fm
{
	void FindOptionInfoList::Add(std::wstring ID, std::wstring name, bool isVisible)
	{
		FindOptionInfo info;
		info.ID = ID;
		info.Name = name;
		info.IsVisible = isVisible;
		m_findOptionList.push_back(info);
	}

	bool FindOptionInfoList::SetVisible(std::wstring id, bool value)
	{
		for (auto& option : m_findOptionList)
		{
			if (option.ID == id)
			{
				option.IsVisible = value;
				return true;
			}
		}
		return false;
	}

	int FindOptionInfoList::Count()
	{
		return m_findOptionList.size();
	}

	std::vector<FindOptionInfo> FindOptionInfoList::GetList()
	{
		return m_findOptionList;
	}

	std::vector<FindOptionInfo> FindOptionInfoList::GetVisibleList()
	{
		std::vector<FindOptionInfo> list;
		for (auto& option : m_findOptionList)
		{
			if (option.IsVisible)
			{
				list.push_back(option);
			}
		}
		return list;
	}
}