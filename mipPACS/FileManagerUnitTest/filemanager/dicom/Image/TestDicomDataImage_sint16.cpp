#include "pch.h"
#include "filemanager/dicom/Image/DicomDataImage_sint16.h"
#include "filemanager/dicom/DicomDataset.h"
#include <qdir>
#include <qurl>

using namespace fm;
class TestDicomDataImage_sint16 : public ::testing::Test
{
public:
	TestDicomDataImage_sint16() {}

	void SetUp() override
	{
		ConnectToNAS();
	}

	void TearDown() override
	{
	}
};


TEST_F(TestDicomDataImage_sint16, TestLoginToNetworkDrive)
{
	NETRESOURCE nr = {};
	memset(&nr, 0, sizeof(nr));
	nr.dwType = RESOURCETYPE_DISK;
	wchar_t remoteName[] = L"\\\\it_team";
	nr.lpRemoteName = remoteName;
	wchar_t username[] = NAS_USER_NAME;
	wchar_t password[] = NAS_USER_PASSWORD;
	
	int ret = WNetUseConnectionW(NULL, &nr, password, username, 0, NULL, NULL, NULL);

	if (ret == ERROR_SESSION_CREDENTIAL_CONFLICT)
	{
	}
	else if (ret == NO_ERROR)
	{
	}
	else
	{
		EXPECT_FALSE(true) << "invalid connect status : " << (int)ret;
	}
}

TEST_F(TestDicomDataImage_sint16, TestDicomExist)
{
	std::wstring testDcmDirectoryPath = GetNASDirPath(L"TestDicomDataImage_sint16");
	QString testFilePath = QString::fromStdWString(testDcmDirectoryPath + L"0001.dcm");
	EXPECT_TRUE(QFile::exists(testFilePath));
}

TEST_F(TestDicomDataImage_sint16, LoadDicomAndCompare)
{
	std::wstring testDcmDirectoryPath = GetNASDirPath(L"TestDicomDataImage_sint16");
	QString testFilePath = QString::fromStdWString(testDcmDirectoryPath + L"0001.dcm");

	DicomDataset dcmDataset;
	EXPECT_TRUE(dcmDataset.LoadFromFile(testFilePath));
	DicomDataImage_sint16 dcmImage;
	EXPECT_TRUE(dcmImage.Init(&dcmDataset));
}
