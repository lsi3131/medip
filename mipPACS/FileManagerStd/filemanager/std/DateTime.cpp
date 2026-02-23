#include "stdafx.h"
#include "DateTime.h"
#include <qdatetime>

using namespace fm;

DateTime fm::DateTime::CurrentDateTime()
{
	QDateTime qdateTime = QDateTime::currentDateTime();
	QDate date = qdateTime.date();
	QTime time = qdateTime.time();

	return DateTime(
		date.year(), date.month(), date.day(),
		time.hour(), time.minute(), time.second()
	);
}

bool DateTime::GetDateTime_TextYYYYMMDD_hhmmss(DateTime& outDateTime, std::string textYYYYMMDD, std::string text_hhmmss)
{
	int length= textYYYYMMDD.length();
	if (textYYYYMMDD.size() != 8)
	{
		return false;
	}

	/* 시간의 기본값이 없을 경우 00:00:00 으로 설정 */
	if (text_hhmmss.size() < 6)
	{
		text_hhmmss = "000000";
	}

	int year = atoi(textYYYYMMDD.substr(0, 4).c_str());
	int month = atoi(textYYYYMMDD.substr(4, 2).c_str());
	int day = atoi(textYYYYMMDD.substr(6, 2).c_str());
	int hour = atoi(text_hhmmss.substr(0, 2).c_str());
	int minute = atoi(text_hhmmss.substr(2, 2).c_str());
	int second = atoi(text_hhmmss.substr(4, 2).c_str());

	outDateTime = DateTime(year, month, day, hour, minute, second);
	return true;
}


DateTime::DateTime() :
	m_year(-1),
	m_month(-1),
	m_day(-1),
	m_hour(-1),
	m_minute(-1),
	m_second(-1)
{
}

DateTime::DateTime(
	int year,
	int month,
	int day,
	int hour,
	int minute,
	int second) :
	m_year(year),
	m_month(month),
	m_day(day),
	m_hour(hour),
	m_minute(minute),
	m_second(second)
{
}

DateTime::DateTime(
	int year,
	int month,
	int day) :
	m_year(year),
	m_month(month),
	m_day(day),
	m_hour(-1),
	m_minute(-1),
	m_second(-1)
{
}

DateTime::DateTime(const QDateTime& qdatetime) :
	DateTime(
		qdatetime.date().year(),
		qdatetime.date().month(),
		qdatetime.date().day(),
		qdatetime.time().hour(),
		qdatetime.time().minute(),
		qdatetime.time().second()
	)
{
}

bool fm::DateTime::IsEmpty() const
{
	if (
		m_year == -1 &&
		m_month == -1 &&
		m_day == -1 &&
		m_hour == -1 &&
		m_minute == -1 &&
		m_second == -1
		)
	{
		return true;
	}

	return false;
}

std::string fm::DateTime::ToFormatText_DateTime() const
{
	if (IsEmpty())
	{
		return "";
	}
	QString editDateTimeText;
	editDateTimeText.sprintf("%04d-%02d-%02d %02d:%02d:%02d",
		m_year, m_month, m_day,
		m_hour, m_minute, m_second
	);

	return editDateTimeText.toStdString();
}

std::string fm::DateTime::ToFormatText_Date() const
{
	if (IsEmpty())
	{
		return "";
	}

	QString editDateTimeText;
	editDateTimeText.sprintf("%04d-%02d-%02d",
		m_year, m_month, m_day
	);

	return editDateTimeText.toStdString();
}

std::string fm::DateTime::ToYYYYMMDDhhmmss() const
{
	if (IsEmpty())
	{
		return "";
	}

	QString editDateTimeText;
	editDateTimeText.sprintf("%04d%02d%02d%02d%02d%02d",
		m_year, m_month, m_day, m_hour, m_minute, m_second
	);

	return editDateTimeText.toStdString();
}

std::string fm::DateTime::ToYYYYMMDD() const
{
	if (IsEmpty())
	{
		return "";
	}

	QString editDateTimeText;
	editDateTimeText.sprintf("%04d%02d%02d",
		m_year, m_month, m_day
	);

	return editDateTimeText.toStdString();
}

std::string fm::DateTime::Tohhmmss() const
{
	if (IsEmpty())
	{
		return "";
	}

	QString editDateTimeText;
	editDateTimeText.sprintf("%02d%02d%02d",
		m_hour, m_minute, m_second
	);

	return editDateTimeText.toStdString();
}

bool fm::DateTime::SetFormattedText(std::string text)
{
	sscanf_s(text.c_str(), "%04d-%02d-%02d %02d:%02d:%02d",
		&m_year, &m_month, &m_day,
		&m_hour, &m_minute, &m_second);

	return true;
}

int fm::DateTime::Year()
{
	return m_year;
}

int fm::DateTime::Month()
{
	return m_month;
}

int fm::DateTime::Day()
{
	return m_day;
}

int fm::DateTime::Hour()
{
	return m_hour;
}

int fm::DateTime::Minute()
{
	return m_minute;
}

int fm::DateTime::Second()
{
	return m_second;
}

bool fm::DateTime::operator==(const DateTime & other) const
{
	QDateTime lhsTime(
		QDate(m_year, m_month, m_day),
		QTime(m_hour, m_minute, m_second)
	);
	QDateTime rhsTime(
		QDate(other.m_year, other.m_month, other.m_day),
		QTime(other.m_hour, other.m_minute, other.m_second)
	);
	return lhsTime == rhsTime;
}

bool fm::DateTime::operator<(const DateTime & other) const
{
	QDateTime lhsTime(
		QDate(m_year, m_month, m_day),
		QTime(m_hour, m_minute, m_second)
	);
	QDateTime rhsTime(
		QDate(other.m_year, other.m_month, other.m_day),
		QTime(other.m_hour, other.m_minute, other.m_second)
	);

	return lhsTime < rhsTime;
}
