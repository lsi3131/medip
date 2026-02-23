#pragma once

#include <qstring>
#include <qstringlist>
#include <vector>

namespace fm
{
	struct ColumnHeaderInfo
	{
		int Index;
		QString Name;
		bool IsVisible;
	};

	class ColumnHeaderInfoList
	{
	public:
		void Add(int index, QString name, bool isVisible);
		bool SetVisible(int index, bool value);
		int Count();
		QStringList GetNameHeaderList();

		std::vector<int> GetInvisibleHeaderIndexList();
		std::vector<int> GetVisibleHeaderIndexList();

	private:
		std::vector<ColumnHeaderInfo> m_columnHeaderInfoList;
	};
}

