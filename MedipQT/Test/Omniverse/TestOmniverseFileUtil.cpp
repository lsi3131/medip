#include "stdafx.h"
#include "Test/test_pch.h"
#include "Omniverse/OmniverseFileUtil.h"
#include "Test/Omniverse/OmniverseFileTestResource.h"
#include "Test/Omniverse/Usd/UsdFileTestResource.h"
#include <memory>

class TestOmniverseFileUtil : public ::testing::Test
{
public:
	void SetUp() override
	{
	}
	void TearDown() override
	{
	}

protected:
	OmniverseFileTestResource m_OmniFileResource;
	UsdFileTestResource m_UsdFileResource;
};

TEST_F(TestOmniverseFileUtil, TestCreate_And_Delete_Directory)
{
	std::shared_ptr<OmniverseConnector> pConnector = std::make_shared<OmniverseConnector>();
	pConnector->Connect(m_OmniFileResource.LocalIpAddress);

	OmniverseFileUtil::CreateDir(pConnector.get(), "Projects/unittest");

	OmniverseFileInfoPtr pFileInfo_Exist = pConnector->FindFileByUrl("Projects/unittest");
	EXPECT_NE(nullptr, pFileInfo_Exist);
	EXPECT_TRUE(pFileInfo_Exist->CanHaveChildren());

	OmniverseFileUtil::DeleteServerFile(pConnector.get(), "Projects/unittest");

	OmniverseFileInfoPtr pFileInfo_NotExist = pConnector->FindFileByUrl("Projects/unittest");
	EXPECT_EQ(nullptr, pFileInfo_NotExist);

}

TEST_F(TestOmniverseFileUtil, TestUploadFile)
{
	std::shared_ptr<OmniverseConnector> pConnector = std::make_shared<OmniverseConnector>();
	pConnector->Connect(m_OmniFileResource.LocalIpAddress);

	OmniverseFileUtil::UploadFile(pConnector.get(), "Projects", m_UsdFileResource.USD_Empty_FilePath);

	OmniverseFileInfoPtr pFileInfo = pConnector->FindFileByUrl("Projects/empty.usd");
	EXPECT_NE(nullptr, pFileInfo);
	EXPECT_FALSE(pFileInfo->CanHaveChildren());

	OmniverseFileUtil::DeleteServerFile(pConnector.get(), "Projects/empty.usd");
}

