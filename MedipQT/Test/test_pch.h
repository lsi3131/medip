#pragma once

#include <gtest/gtest.h>
#include "define.h"
#include "Test/TestUtil.h"

#define EXPECT_EQ_VECTOR(v1, v2) EXPECT_STREQ(TestUtil::To_Text((v1)).c_str(), TestUtil::To_Text((v2)).c_str());

inline std::string GetUnitTestDataDirectory(std::string subDirPath)
{
	return (QDir::currentPath() + "/Test/" + subDirPath.c_str()).toStdString();
}
