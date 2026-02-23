#include "stdafx.h"
#include "mipUsdPreset.h"
#include <QDir>

#define FILE_NAME_RESOURCE_USDA ("resource.usda")
#define FILE_NAME_MESH_MATERIAL_TABLE_JSON ("mesh_material_table.json")

#define JSON_ROOT_NAME ("root")
#define JSON_MESH_MATERIAL_LIST_NAME ("mesh_material_list")

mipUsdPreset::mipUsdPreset() :
	m_pUsdStage(nullptr),
	m_pTempFile(nullptr)
{
}

mipUsdPreset::~mipUsdPreset()
{
	if (m_pUsdStage)
	{
		m_pUsdStage->Close();
	}
	m_pUsdStage = nullptr;
	m_pTempFile = nullptr;
	if (QFile::exists(m_tempFilePath))
	{
		QFile::remove(m_tempFilePath);
	}
}

bool mipUsdPreset::LoadPreset(const std::string presetDirPath)
{
	if (QFile::exists(QString::fromStdString(presetDirPath)) == false)
	{
		return false;
	}

	//load resourece.usda
	std::string resourceUsdaFilePath = presetDirPath + "/" + FILE_NAME_RESOURCE_USDA;
	if (OpenUsd(resourceUsdaFilePath) == false)
	{
		return false;
	}

	//load json
	if (loadMeshMaterialTableJson(QString::fromStdString(presetDirPath)) == false)
	{
		return false;
	}

	m_presetDirectoryPath = presetDirPath;

	return true;
}

bool mipUsdPreset::IsPresetFileLoaded() const
{
	if (IsOpen() == false)
	{
		return false;
	}

	if (m_presetDirectoryPath.empty())
	{
		return false;
	}

	return true;
}

bool mipUsdPreset::OpenNew()
{
	if (initUsdTempFile() == false)
	{
		return false;
	}

	/* 임시 stage 파일 open */
	m_pUsdStage = std::make_shared<mipUsdStage>();
	if (m_pUsdStage->CreateNew(m_tempFilePath.toStdString()) == false)
	{
		qWarning() << "fail to create temp file path : " << m_tempFilePath;
		return false;
	}

	return true;
}


bool mipUsdPreset::OpenUsd(const std::string& usdFilePath)
{
	if (QFile::exists(QString::fromStdString(usdFilePath)) == false)
	{
		qWarning() << "usd file path not exist : " << QString::fromStdString(usdFilePath);
		return false;
	}

	QByteArray srcUsdData;
	if (tryReadUsdSourceData(&srcUsdData, usdFilePath) == false)
	{
		return false;
	}

	QFileInfo fileInfo(QString::fromStdString(usdFilePath));
	QString extension = fileInfo.suffix();

	if (initUsdTempFileWithData(srcUsdData, extension) == false)
	{
		return false;
	}

	/* 임시 stage 파일 open */
	m_pUsdStage = std::make_shared<mipUsdStage>();
	if (m_pUsdStage->Open(m_tempFilePath.toStdString()) == false)
	{
		qWarning() << "fail to open temp file path : " << m_tempFilePath;
		return false;
	}

	initMeshMaterialPairListByUsdStage();

	if (m_pUsdStage->DeleteAllMesh() == false)
	{
		return false;
	}

	return true;
}

bool mipUsdPreset::SaveToPresetFile(const std::string& presetDirectoryPath, const std::string& presetName)
{
	if (IsOpen() == false)
	{
		qWarning() << "usd preset is not open status";
		return false;
	}

	if (QFile::exists(QString::fromStdString(presetDirectoryPath)) == false)
	{
		qWarning() << "preset directory path not exist : " << QString::fromStdString(presetDirectoryPath);
		return false;
	}

	/* preset directory 생성 */
	QString presetDirPath = QString::fromStdString(presetDirectoryPath + "/" + presetName);
	QDir().mkpath(presetDirPath);

	/* mesh_material_table.json 파일 저장 */
	if (saveMeshMaterialTableJson(presetDirPath) == false)
	{
		return false;
	}

	/* material, light 등 resource 저장 */
	QString resourceFilePath = presetDirPath + "/" + FILE_NAME_RESOURCE_USDA;
	if (m_pUsdStage->Export(resourceFilePath.toStdString()) == false)
	{
		return false;
	}

	m_presetDirectoryPath = presetDirPath.toStdString();

	return true;
}

bool mipUsdPreset::DeletePresetFile()
{
	if (IsPresetFileLoaded() == false)
	{
		return false;
	}

	QDir(QString::fromStdString(m_presetDirectoryPath)).removeRecursively();

	return true;
}

bool mipUsdPreset::AddMaterialByMdlFile(const std::string& mdlFilePath, const std::string& materialName)
{
	if (IsOpen() == false)
	{
		return false;
	}

	QFileInfo fileInfo(QString::fromStdString(mdlFilePath));
	std::string subId = fileInfo.baseName().toStdString();
	if (m_pUsdStage->AddMaterialByMDLFile(mdlFilePath, materialName, subId) == false)
	{
		return false;
	}

	return true;
}

bool mipUsdPreset::BindMeshToMaterial(const std::string& meshName, const std::string& materialName)
{
	return false;
}

std::string mipUsdPreset::GetPresetName() const
{
	QFileInfo fileInfo(QString::fromStdString(m_presetDirectoryPath));

	QString presetDirName = fileInfo.fileName();

	return presetDirName.toStdString();
}

std::string mipUsdPreset::GetPresetDirectoryPath() const
{
	return m_presetDirectoryPath;
}

bool mipUsdPreset::IsOpen() const
{
	if (m_pUsdStage == nullptr)
	{
		return false;
	}

	if (m_pUsdStage->IsOpen() == false)
	{
		return false;
	}

	return true;
}

std::vector<mipPresetMeshMaterialPair> mipUsdPreset::GetMeshToMaterialPairList() const
{
	return m_meshMaterialPairList;
}

std::vector<mipUSDMaterialPtr> mipUsdPreset::CreateMaterialList() const
{
	std::vector<mipUSDMaterialPtr> materialList;
	if (IsOpen() == false)
	{
		return materialList;
	}

	materialList = m_pUsdStage->CreateMaterialList();
	return materialList;
}

std::vector<mipUsdLightPtr> mipUsdPreset::CreateLightList() const
{
	std::vector<mipUsdLightPtr> lightList;
	if (IsOpen() == false)
	{
		return lightList;
	}

	lightList = m_pUsdStage->CreateLightList();
	return lightList;
}

std::string mipUsdPreset::GetResourceUsdaFilePath() const
{
	if (IsPresetFileLoaded() == false)
	{
		return "";
	}

	return m_presetDirectoryPath + "/" + FILE_NAME_RESOURCE_USDA;
}

std::string mipUsdPreset::GetMeshMaterialTableJsonFilePath() const
{
	if (IsPresetFileLoaded() == false)
	{
		return "";
	}

	return m_presetDirectoryPath + "/" + FILE_NAME_MESH_MATERIAL_TABLE_JSON;
}

void mipUsdPreset::SetRenderSetting(const mipUsdRenderSetting& setting)
{
	mipUsdCustomLayerData customLayerData = m_pUsdStage->GetCustomLayerData();
	customLayerData.SetRenderSetting(setting);
	m_pUsdStage->SetCustomLayerData(customLayerData);
}

mipUsdRenderSetting mipUsdPreset::GetRenderSetting() const
{
	mipUsdRenderSetting setting;
	mipUsdCustomLayerData customLayerData = m_pUsdStage->GetCustomLayerData();
	customLayerData.TryGetRenderSetting(&setting);

	return setting;
}

bool mipUsdPreset::isFileExist(const std::string& usdFilePath, const std::string& presetDirectoryPath) const
{
	if (QFile::exists(QString::fromStdString(usdFilePath)) == false)
	{
		qWarning() << "usd file path not exist : " << QString::fromStdString(usdFilePath);
		return false;
	}

	if (QFile::exists(QString::fromStdString(presetDirectoryPath)) == false)
	{
		qWarning() << "preset directory path not exist : " << QString::fromStdString(presetDirectoryPath);
		return false;
	}

	return true;
}

bool mipUsdPreset::initUsdTempFileWithData(const QByteArray& srcUsdData, const QString& extension)
{
	m_pTempFile = std::make_unique<QTemporaryFile>();

	if (m_pTempFile->open() == false)
	{
		qWarning() << "fail to open temporay file";
		return false;
	}

	/*
		파일 확장자 .usd 추가 작업 진행.
		이후 rename, open을 해야 정상 동작됨
	*/
	m_tempFilePath = m_pTempFile->fileName();
	if (!extension.isEmpty())
	{
		m_tempFilePath += "." + extension;
	}
	if (m_pTempFile->rename(m_tempFilePath) == false)
	{
		qWarning() << "fail to rename temporay file : " << m_tempFilePath;
		return false;
	}
	m_pTempFile->open();
	m_pTempFile->setAutoRemove(true);

	m_pTempFile->write(srcUsdData);
	m_pTempFile->close();

	return true;
}

bool mipUsdPreset::initUsdTempFile()
{
	return initUsdTempFileWithData(QByteArray(), "usd");
}

bool mipUsdPreset::tryReadUsdSourceData(QByteArray* pOutUsdData, const std::string& usdFilePath) const
{
	QFile fileSrcUsd(QString::fromStdString(usdFilePath));
	if (fileSrcUsd.open(QIODevice::ReadOnly) == false)
	{
		qWarning() << "fail to open source usd file : " << usdFilePath.c_str();
		return false;
	}
	*pOutUsdData = fileSrcUsd.readAll();
	fileSrcUsd.close();

	return true;
}

bool mipUsdPreset::initMeshMaterialPairListByUsdStage()
{
	m_meshMaterialPairList.clear();
	std::vector<mipUSDMeshPtr> meshList = m_pUsdStage->CreateMeshList();
	for (auto pMesh : meshList)
	{
		mipUSDMaterialPtr pMaterial = pMesh->GetMaterial();
		if (pMaterial != nullptr)
		{
			std::string meshPath = pMesh->GetPath();
			std::string materialPath = pMaterial->GetPath();
			mipPresetMeshMaterialPair data = { meshPath, materialPath };
			m_meshMaterialPairList.push_back(data);
		}
	}

	return true;
}

bool mipUsdPreset::saveMeshMaterialTableJson(const QString& targetPresetDirPath)
{
	QString meshMaterialTableJsonFilePath = targetPresetDirPath + "/" + "mesh_material_table.json";
	QFile file(meshMaterialTableJsonFilePath);
	if (file.open(QIODevice::WriteOnly) == false)
	{
		qWarning() << "fail to open mesh matarial table json file : " << meshMaterialTableJsonFilePath;
		return false;
	}

	QJsonObject rootJsonObject{ {JSON_ROOT_NAME ,""} };
	QJsonObject meshMaterialJsonObject;

	for (auto& pair : m_meshMaterialPairList)
	{
		QString key = QString::fromStdString(pair.MeshPath);
		QString value = QString::fromStdString(pair.MaterialPath);
		meshMaterialJsonObject.insert(key, value);
	}

	QJsonObject meshMaterialJsonListObject{ {JSON_MESH_MATERIAL_LIST_NAME, ""} };
	meshMaterialJsonListObject.insert(JSON_MESH_MATERIAL_LIST_NAME, meshMaterialJsonObject);

	rootJsonObject.insert(JSON_ROOT_NAME, meshMaterialJsonListObject);

	QJsonDocument document;
	document.setObject(rootJsonObject);

	file.write(document.toJson());
	file.close();

	return true;
}

bool mipUsdPreset::loadMeshMaterialTableJson(const QString& targetPresetDirPath)
{
	QString meshMaterialTableJsonFilePath = targetPresetDirPath + "/" + "mesh_material_table.json";
	QFile file(meshMaterialTableJsonFilePath);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		qWarning() << "fail to open mesh matarial table json file : " << meshMaterialTableJsonFilePath;
		return false;
	}

	QByteArray jsonData = file.readAll();
	file.close();

	QJsonDocument document = QJsonDocument::fromJson(jsonData);
	QJsonObject jsonObject = document.object();

	if (jsonObject.isEmpty())
	{
		return false;
	}

	QJsonObject rootJsonObject = jsonObject.value(JSON_ROOT_NAME).toObject();
	if (rootJsonObject.isEmpty())
	{
		return false;
	}

	QJsonObject meshMaterialListJsonObject = rootJsonObject.value(JSON_MESH_MATERIAL_LIST_NAME).toObject();

	m_meshMaterialPairList.clear();
	for (const QString& key : meshMaterialListJsonObject.keys())
	{
		QString value = meshMaterialListJsonObject.value(key).toString();
		mipPresetMeshMaterialPair pair{ key.toStdString(), value.toStdString() };
		m_meshMaterialPairList.push_back(pair);
	}

	return true;
}

