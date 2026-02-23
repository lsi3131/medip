#include "pch.h"
#include "gtest/gtest.h"
#include <qapplication>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	testing::InitGoogleTest(&argc, argv);

	int result = RUN_ALL_TESTS();
	return result;
}

