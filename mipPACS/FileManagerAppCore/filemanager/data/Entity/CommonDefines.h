#pragma once

#include <vector>
#include <string>

namespace fm
{
	struct ColumnRecord
	{
		std::wstring ColumnName;
		std::wstring Value;
	};

	class FindCondition
	{
	public:
		struct KeyValue
		{
			std::wstring Key;
			std::wstring Value;

			KeyValue(std::wstring key, std::wstring value)
			{
				Key = key;
				Value = value;
			}
		};
	public:
		FindCondition(){}

	public:
		void Add(std::wstring key, std::wstring value)
		{
			m_findDatas.push_back(KeyValue(key, value));
		}

		const std::vector<KeyValue>& GetFindDatas() const
		{
			return m_findDatas;
		}

	private:		
		std::vector<KeyValue> m_findDatas;
	};
}

