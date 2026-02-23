#pragma once

#include <qstring>

namespace fm
{
	enum EDateRangeMode
	{
		All_Date,
		Today,
		YesterDay,
		LastWeek,
		LastMonth,
		LastYear,
		CustomDate,
	};

	struct DateRangeInfo
	{
		EDateRangeMode Mode;
		QString Text;
	};

}

