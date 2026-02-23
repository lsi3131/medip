#pragma once

#include <string>
#include <vector>
#include "Math/Math.h"

class TestUtil
{
public:
	static std::string To_Text(const std::vector<mip::VECTOR3>& v_list);
	static std::string To_Text(const mip::VECTOR3& v);
};
