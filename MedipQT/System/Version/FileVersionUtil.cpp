#include "stdafx.h"
#include "FileVersionUtil.h"
#include "System/stringManager.h"
#include "mipEngine/mipEngine.h"
#include "Renderer/mipRenderer.h"
#include "DeepInsthink.h"

#ifdef _DEBUG
static const std::string SHARED_MODULE_LIST_INTERNAL[] = {
	//"mipEngine.dll",
	//"mipRenderer.dll",
	"mipDICOM.dll",
	"MedipMipEncoderd.dll",
	"MeshControld.dll",
	"MedipLicensed.dll",
	"FileManagerAppCored.dll",
	"MedicalIPUpdater.exe",
	"Radiomics.dll",
	"deepinsthink.dll",
};
#else
static const std::string SHARED_MODULE_LIST_INTERNAL[] = {
	//"mipEngine.dll",
	//"mipRenderer.dll",
	"mipDICOM.dll",
	"MedipMipEncoder.dll",
	"MeshControl.dll",
	"MedipLicense.dll",
	"FileManagerAppCore.dll",
	"MedicalIPUpdater.exe",
	"Radiomics.dll",
	"deepinsthink.dll",
};
#endif

std::wstring FileVersionUtil::GetFileVersion(const std::wstring& filePath)
{
	if (QFile::exists(QString::fromStdWString(filePath)) == false)
	{
		return L"";
	}

	// 버전정보를 담을 버퍼
	char* buffer = nullptr;
	std::wstring versionInfo;

	DWORD infoSize = 0;
	DWORD handle = 0;

	// 파일로부터 버전정보데이터의 크기가 얼마인지를 구한다.
	infoSize = GetFileVersionInfoSizeW(filePath.c_str(), &handle);
	if (infoSize == 0)
	{
		return L"";
	}

	// 버퍼할당
	buffer = new char[infoSize];
	if (buffer)
	{
		// 버전정보데이터를 가져옵니다.
		if (GetFileVersionInfoW(filePath.c_str(), 0, infoSize, buffer) != 0)
		{
			VS_FIXEDFILEINFO* pFineInfo = nullptr;
			UINT bufLen = 0;
			// buffer로 부터 VS_FIXEDFILEINFO 정보를 가져온다.
			if (VerQueryValueW(buffer, (LPCWSTR)L"\\", (LPVOID*)&pFineInfo, &bufLen) != 0)
			{
				WORD majorVer, minorVer, buildNum, revisionNum;
				majorVer = HIWORD(pFineInfo->dwFileVersionMS);
				minorVer = LOWORD(pFineInfo->dwFileVersionMS);
				buildNum = HIWORD(pFineInfo->dwFileVersionLS);
				revisionNum = LOWORD(pFineInfo->dwFileVersionLS);

				// 파일버전 출력
				versionInfo =
					std::to_wstring(majorVer) + L"." +
					std::to_wstring(minorVer) + L"." +
					std::to_wstring(buildNum) + L"." +
					std::to_wstring(revisionNum);
			}
		}
		delete[] buffer;
	}

	return versionInfo;
}

QString FileVersionUtil::GetStaticLibraryVersion(eStaticLibraryType type)
{
	switch (type)
	{
	case eStaticLibraryType::mipEngine:
		return mipEngine().getLibVersion().c_str();
	case eStaticLibraryType::mipRenderer:
		return mipRenderer().getLibVersion().c_str();
	default:
		qWarning() << "invalid static library type : " << (int)type;
		return "";
	}
}

std::vector<std::tuple<QString, QString>> FileVersionUtil::GetModuleVersionList_Internal(StringManager* pStrManager)
{
	std::vector<std::tuple<QString, QString>> result;

	result.push_back(std::make_tuple("mipEngine.lib", GetStaticLibraryVersion(eStaticLibraryType::mipEngine)));
	result.push_back(std::make_tuple("mipRenderer.lib", GetStaticLibraryVersion(eStaticLibraryType::mipRenderer)));

	for (int i = 0; i < _countof(SHARED_MODULE_LIST_INTERNAL); ++i)
	{
		QString name = SHARED_MODULE_LIST_INTERNAL[i].c_str();
		QString filepath = pStrManager->programPath + "/" + name;
		QString version = QString::fromStdWString(GetFileVersion(filepath.toStdWString()));
		if (version.isEmpty() == false)
		{
			result.push_back(std::make_tuple(name, version));
		}
	}

	return result;
}

std::vector<AIWeightFileInfo> FileVersionUtil::GetAIWeightVersionList(StringManager* pStrManager)
{
	std::vector<AIWeightFileInfo> result;

	SegmentationMeta segmentMetaData;
	QString medipAIDeepDrawDirPath = pStrManager->AISegmentationPath + "/weight";

	QDir dir(medipAIDeepDrawDirPath);
	for (auto& fileInfo : dir.entryInfoList({"*.mipx"}, QDir::Filter::Files))
	{
		QString weightFileName = fileInfo.fileName();
		std::vector< std::vector<int>> notUse;
		bool decode = true;
		bool success = Segmentation::readMeta(
			segmentMetaData, 
			medipAIDeepDrawDirPath.toStdString(),
			weightFileName.toStdString(),
			decode, 
			notUse);

		if (success)
		{
			QString name = weightFileName;
			QString version = QString::number(segmentMetaData.weight_version);
			ClientType clientType = segmentMetaData.type;

			AIWeightFileInfo weightFileInfo;
			weightFileInfo.Name = weightFileName;
			weightFileInfo.Version = QString::number(segmentMetaData.weight_version);
			weightFileInfo.Description = segmentMetaData.description.c_str();

			result.push_back(weightFileInfo);
		}
	}

	return result;
}

