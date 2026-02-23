#pragma once

#include <string>
#include <QTemporaryFile>
#include <memory>
#include <QByteArray>
#include "mipUsdStage.h"
#include "Omniverse/Usd/mipUsdRenderSetting.h"

class mipPresetMeshMaterialPair
{
public:
	std::string GetMeshName() const
	{
		QString meshName = QFileInfo(QString::fromStdString(MeshPath)).fileName();
		return meshName.toStdString();
	}

	std::string GetMaterialName() const
	{
		QString materialName = QFileInfo(QString::fromStdString(MaterialPath)).fileName();
		return materialName.toStdString();
	}

public:
	std::string MeshPath;
	std::string MaterialPath;
	
};

class mipUsdPreset
{
public:
	mipUsdPreset();
	~mipUsdPreset();

public:
	bool LoadPreset(const std::string presetDirPath);
	bool IsPresetFileLoaded() const;

	/*
		.usd, .usda Áö¿ø 
	*/
	bool OpenNew();
	bool OpenUsd(const std::string& usdFilePath);
	bool SaveToPresetFile(const std::string& presetDirectoryPath, const std::string& presetName);

	bool DeletePresetFile();

	bool AddMaterialByMdlFile(const std::string& mdlFilePath, const std::string& materialName);
	bool BindMeshToMaterial(const std::string& meshName, const std::string& materialName);

	std::string GetPresetName() const;
	std::string GetPresetDirectoryPath() const;

	bool IsOpen() const;

	std::vector<mipPresetMeshMaterialPair> GetMeshToMaterialPairList() const;
	std::vector<mipUSDMaterialPtr> CreateMaterialList() const;
	std::vector<mipUsdLightPtr> CreateLightList() const;

	std::string GetResourceUsdaFilePath() const;
	std::string GetMeshMaterialTableJsonFilePath() const;

	void SetRenderSetting(const mipUsdRenderSetting& setting);
	mipUsdRenderSetting GetRenderSetting() const;

private:
	bool isFileExist(const std::string& usdFilePath, const std::string& presetDirectoryPath) const;
	bool initUsdTempFileWithData(const QByteArray& srcUsdData, const QString& extension);
	bool initUsdTempFile();
	bool tryReadUsdSourceData(QByteArray* pOutUsdData, const std::string& usdFilePath) const;

	bool initMeshMaterialPairListByUsdStage();

	bool saveMeshMaterialTableJson(const QString& targetPresetDirPath);
	bool loadMeshMaterialTableJson(const QString& targetPresetDirPath);

private:
	std::shared_ptr<mipUsdStage> m_pUsdStage;
	std::unique_ptr<QTemporaryFile> m_pTempFile;
	QString m_tempFilePath;
	std::vector<mipPresetMeshMaterialPair> m_meshMaterialPairList;
	std::string m_presetDirectoryPath;
};

using mipUsdPresetPtr = std::shared_ptr<mipUsdPreset>;

