#pragma once

/*
	작성자 : 이상일
	목적 : 
	파일관리자에서 사용되는 데이터 타입 정의
*/

#include <string>
#include "filemanager/std/export.h"

class QDateTime;

namespace fm
{
	class FM_STD_EXPORT DateTime
	{
	public:
		static DateTime CurrentDateTime();
		static bool GetDateTime_TextYYYYMMDD_hhmmss(DateTime& outDateTime, std::string textYYYYMMDD, std::string text_hhmmss);
	public:
		DateTime();
		DateTime(
			int year,
			int month,
			int day,
			int hour,
			int minute,
			int second
		);

		DateTime(
			int year,
			int month,
			int day
		);

		DateTime(const QDateTime& qdatetime);
			
		bool IsEmpty() const;
		std::string ToFormatText_DateTime() const;
		std::string ToFormatText_Date() const;
		std::string ToYYYYMMDDhhmmss() const;
		std::string ToYYYYMMDD() const;
		std::string Tohhmmss() const;
		bool SetFormattedText(std::string text);

		int Year();
		int Month();
		int Day();
		int Hour();
		int Minute();
		int Second();

		bool operator==(const DateTime &other) const;
		inline bool operator!=(const DateTime &other) const { return !(*this == other); }
		bool operator<(const DateTime &other) const;
		inline bool operator<=(const DateTime &other) const { return !(other < *this); }
		inline bool operator>(const DateTime &other) const { return other < *this; }
		inline bool operator>=(const DateTime &other) const { return !(*this < other); }

	private:
		int m_year;
		int m_month;
		int m_day;
		int m_hour;
		int m_minute;
		int m_second;
	};
}
