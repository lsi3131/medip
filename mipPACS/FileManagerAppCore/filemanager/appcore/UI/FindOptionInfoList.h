#pragma once

#include <string>

namespace fm
{
	struct FindOptionInfo
	{
		std::wstring ID;
		std::wstring Name;
		bool IsVisible;
	};

	class FindOptionInfoList
	{
	public:
		void Add(std::wstring ID, std::wstring name, bool isVisible);
		bool SetVisible(std::wstring id, bool value);
		int Count();

		std::vector<FindOptionInfo> GetList();
		std::vector<FindOptionInfo> GetVisibleList();

	private:
		std::vector<FindOptionInfo> m_findOptionList;
	};
}
