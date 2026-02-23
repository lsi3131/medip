#include "stdafx.h"
#include "TestUtil.h"
#include <QString>

std::string TestUtil::To_Text(const std::vector<mip::VECTOR3>& v_list)
{
	std::vector<mip::VECTOR3> v_list_sorted = v_list;
	std::sort(v_list_sorted.begin(), v_list_sorted.end(), [](const mip::VECTOR3& lhs, const mip::VECTOR3& rhs) {
		return lhs < rhs;
		});

	QString text;
	for (auto& v : v_list_sorted)
	{
		text += QString("(%1,%2,%3),").arg(v.x).arg(v.y).arg(v.z);
	}
	return text.toStdString();
}

std::string TestUtil::To_Text(const mip::VECTOR3& v)
{
	QString text;
	text = QString("(%1,%2,%3),").arg(v.x).arg(v.y).arg(v.z);
	return text.toStdString();
}

