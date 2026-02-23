#include "stdafx.h"
#include "OmniverseStage.h"
#include "OmniverseHeader.h"
#include "LiveSession/OmniverseLiveChannel.h"
#include "OmniverseStageWriterWorker.h"
#include "OmniverseStageReaderWorker.h"
#include "OmniverseStageEventListenerWorker.h"
#include "Omniverse/Converter/UsdMeshConverter.h"

#include "Usd/mipUsdStage.h"

#include "WindowManager.h"
#include "ShortcutManager.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "MeshEdit/CMeshManipulator.h"

/*
	OmniverseStagePrivate
*/
class OmniverseStagePrivate
{
public:
	OmniverseStagePrivate()
	{
		pCurrentEditStage = &RootStage;
	}

public:
	mipUsdStage RootStage;
	std::string CurrentStageUrl;
	OmniverseConnector* pConnection = nullptr;
	OmniverseLiveSessionPtr pLiveSession = nullptr;
	std::shared_ptr<mipUsdStage> pLiveSessionStage = nullptr;
	mipUsdStage* pCurrentEditStage = nullptr;
	OmniverseLiveChannel LiveChannel;
	MeshData* pMeshData = nullptr;
	std::shared_ptr<OmniverseStageWriterWorker> WriteWorker;
	std::shared_ptr<OmniverseStageReaderWorker> ReaderWorker;
	std::shared_ptr<OmniverseStageEventListenerWorker> EventListenerWorker;
};

/*
	OmniverseStage
*/
static void OmniClientConnectionStatusCallbackImpl(void* userData, const char* url, OmniClientConnectionStatus status) noexcept
{
}

bool OmniverseStage::IsSupportFileExtension(const std::string& fileName)
{
	QFileInfo fileInfo(QString::fromStdString(fileName));
	QString suffix = fileInfo.suffix();
	return
		suffix.toLower() == "usd" ||
		suffix.toLower() == "usda" ||
		suffix.toLower() == "usdc" ||
		suffix.toLower() == "live";
}

OmniverseStage::OmniverseStage()
{
	m_p = std::make_shared<OmniverseStagePrivate>();

	qRegisterMetaType<OmniverseLiveChannelMessagePtr>("OmniverseLiveChannelMessagePtr");
	qRegisterMetaType<std::set<OmniversePeerUser>>("std::set<OmniversePeerUser>");

	m_p->LiveChannel.AddReceiveMessageHandler([](OmniverseLiveChannel* channel, OmniverseLiveChannelMessagePtr message, void* pUserData) {
		OmniverseStage* pData = (OmniverseStage*)pUserData;

		std::set<OmniversePeerUser> userList = channel->GetUserList();

		emit pData->sig_recvLiveSessionMessage(message, userList);
		}, this);

	omniClientRegisterConnectionStatusCallback(this, OmniClientConnectionStatusCallbackImpl);
}


bool OmniverseStage::IsOpen() const
{
	return m_p->RootStage.IsOpen();
}

bool OmniverseStage::Create(OmniverseConnector* pConnection, const std::string& filepath)
{
	if (IsSupportFileExtension(filepath) == false)
	{
		return false;
	}

	if (pConnection == nullptr)
	{
		return false;
	}

	if (pConnection->IsConnect() == false)
	{
		return false;
	}

	m_p->CurrentStageUrl = pConnection->GetRootUrl() + "/" + filepath;

	omniClientWait(omniClientDelete(m_p->CurrentStageUrl.c_str(), nullptr, nullptr));

	/* usd 폴더*/
	// Create this file in Omniverse cleanly
	if (m_p->RootStage.CreateNew(m_p->CurrentStageUrl) == false)
	{
		return false;
	}

	m_p->pConnection = pConnection;
	m_p->pLiveSession = OmniverseLiveSession::New(this);

	emit sig_StageDataChanged();

	return true;
}

bool OmniverseStage::Open(OmniverseConnector* pConnection, const std::string& filepath)
{
	if (IsSupportFileExtension(filepath) == false)
	{
		return false;
	}

	if (pConnection == nullptr)
	{
		return false;
	}

	if (pConnection->IsConnect() == false)
	{
		return false;
	}

	m_p->CurrentStageUrl = pConnection->GetRootUrl() + "/" + filepath;

	if (m_p->RootStage.Open(m_p->CurrentStageUrl) == false)
	{
		return false;
	}

	m_p->pConnection = pConnection;
	m_p->pLiveSession = OmniverseLiveSession::New(this);

	emit sig_StageDataChanged();

	return true;
}

bool OmniverseStage::Close()
{
	if (IsLiveSessionMode())
	{
		LeaveLiveSession();
	}
	m_p->pLiveSession = nullptr;
	m_p->pLiveSessionStage = nullptr;
	m_p->pConnection = nullptr;
	m_p->CurrentStageUrl = "";

	m_p->RootStage.Close();

	emit sig_StageDataChanged();
		
	return true;
}

std::string OmniverseStage::GetStageUrl() const
{
	return m_p->CurrentStageUrl;
}

std::string OmniverseStage::GetFileName() const
{
	QFileInfo fileInfo(QString::fromStdString(m_p->CurrentStageUrl));
	return fileInfo.fileName().toStdString();
}

std::string OmniverseStage::GetFileBaseName() const
{
	QFileInfo fileInfo(QString::fromStdString(m_p->CurrentStageUrl));
	return fileInfo.baseName().toStdString();
}

mipUsdStage* OmniverseStage::GetUsdStage() const
{
	return &m_p->RootStage;
}

OmniverseConnector* OmniverseStage::GetConnector() const
{
	return m_p->pConnection;
}

OmniverseLiveSession* OmniverseStage::GetLiveSession() const
{
	return m_p->pLiveSession.get();
}

OmniverseLiveChannel* OmniverseStage::GetLiveChannel() const
{
	return &m_p->LiveChannel;
}

std::string OmniverseStage::GetLiveSessionName() const
{
	return m_p->LiveChannel.GetSessionName();
}

void OmniverseStage::SetMeshData(MeshData* pMeshData)
{
	m_p->pMeshData = pMeshData;
}

bool OmniverseStage::AddMeshList(const MeshData& meshContainer)
{
	std::vector<MeshLayerData> meshDataList = meshContainer.GetList();

	for (auto& m : meshDataList)
	{
		if (m.Info->GetOmniverseStatus() == eOmniverseStatus::Lock)
		{
			AddMesh(m);
		}
	}

	return true;
}

bool OmniverseStage::AddMesh(const MeshLayerData& m)
{
	QElapsedTimer timer;
	timer.restart();

	if (AddMeshByData(*m.Data, *m.Info) == false)
	{
		return false;
	}

	qInfo() << "send mesh data time : " << timer.elapsed() << ", mesh name : " << m.Info->GetName();

	return true;
}

void OmniverseStage::SetLiveModeOn()
{
	if (m_p->WriteWorker == nullptr)
	{
		qInfo() << "start to live mode on";

		m_p->RootStage.InsertSessionSubLayer(*m_p->pLiveSessionStage);
		m_p->RootStage.SetEditTarget(*m_p->pLiveSessionStage);
		m_p->pCurrentEditStage = m_p->pLiveSessionStage.get();

		/* Worker Thread 사용 */
		m_p->WriteWorker = std::make_shared<OmniverseStageWriterWorker>(this, m_p->pMeshData);
		connect(m_p->WriteWorker.get(), &OmniverseStageWriterWorker::sig_liveDataChanged, this, &OmniverseStage::slot_LiveChanged_Writer);
		connect(m_p->WriteWorker.get(), &OmniverseStageWriterWorker::sig_liveDataUpdate, this, &OmniverseStage::slot_LiveUpdate_Writer);
		m_p->WriteWorker->Start();

		m_p->ReaderWorker = std::make_shared<OmniverseStageReaderWorker>(this, m_p->pMeshData);
		connect(m_p->ReaderWorker.get(), &OmniverseStageReaderWorker::sig_liveDataChanged, this, &OmniverseStage::slot_LiveChanged_Reader);
		m_p->ReaderWorker->Start();
	}
}

void OmniverseStage::SetLiveModeOff()
{
	/* Join Worker Thread */
	if (m_p->WriteWorker)
	{
		qInfo() << "start to live mode off";
		std::thread* pThread;
		pThread = m_p->WriteWorker->Stop();
		pThread->join();

		pThread = m_p->ReaderWorker->Stop();
		pThread->join();

		m_p->WriteWorker = nullptr;
		m_p->ReaderWorker = nullptr;
		m_p->EventListenerWorker = nullptr;

		m_p->RootStage.ClearSessionSubLayer();
		m_p->pLiveSessionStage = nullptr;
		m_p->pCurrentEditStage = &m_p->RootStage;

		qInfo() << "finished to live mode off";
	}
}

bool OmniverseStage::OpenLiveChannel(const std::string& sessionName)
{
	if (m_p->LiveChannel.IsChannelOpen())
	{
		m_p->LiveChannel.CloseChannel_And_Wait();
	}

	if (m_p->LiveChannel.OpenChannel(*m_p->pLiveSession, sessionName, "MEDIP") == false)
	{
		return false;
	}

	m_p->LiveChannel.SendChannelMessage(eOmniverseLiveChannelMessageType::GetUsers);
	return true;
}

bool OmniverseStage::CloseLiveChannel()
{
	return m_p->LiveChannel.CloseChannel_And_Wait();
}

bool OmniverseStage::JoinLiveSession(const std::string& sessionName)
{
	if (m_p->pLiveSession == nullptr)
	{
		qWarning() << "live session is null";
		return false;
	}

	if (IsLiveSessionMode())
	{
		qWarning() << "live session is already exist. should leave session";
		return false;
	}

	m_p->pLiveSessionStage = m_p->pLiveSession->OpenSessionStage(sessionName);
	if (m_p->pLiveSessionStage == nullptr)
	{
		qWarning() << "fail to open session stage";
		return false;
	}


	SetLiveModeOn();
	OpenLiveChannel(sessionName);

	return true;
}

bool OmniverseStage::CreateLiveSession(const std::string& sessionName)
{
	if (m_p->pLiveSession == nullptr)
	{
		qWarning() << "live session is null";
		return false;
	}

	if (IsLiveSessionMode())
	{
		qWarning() << "live session is already exist. should leave session";
		return false;
	}

	m_p->pLiveSessionStage = m_p->pLiveSession->CreateSessionStage(sessionName);
	if (m_p->pLiveSessionStage == nullptr)
	{
		qWarning() << "fail to open session stage";
		return false;
	}


	SetLiveModeOn();
	OpenLiveChannel(sessionName);

	return true;
}

bool OmniverseStage::LeaveLiveSession()
{
	if (IsLiveSessionMode() == false)
	{
		return false;
	}

	m_p->LiveChannel.CloseChannel_And_Wait();

	SetLiveModeOff();
	CloseLiveChannel();

	return true;
}

bool OmniverseStage::IsLiveSessionMode() const
{
	if (m_p->pLiveSession == nullptr)
	{
		return false;
	}

	if (m_p->pLiveSessionStage == nullptr)
	{
		return false;
	}

	pxr::UsdStageRefPtr pRootStage = m_p->RootStage.GetData();
	int subLayerCount = pRootStage->GetSessionLayer()->GetNumSubLayerPaths();

	return subLayerCount > 0;
}

bool OmniverseStage::AddMeshByStlFile(const std::string& inputFilePath, const std::string& meshLayerName, mip::Renderer* pRenderer, COLOR color)
{
	if (m_p->pCurrentEditStage->AddMeshByStlFile(inputFilePath, meshLayerName, pRenderer, color) == false)
	{
		return false;
	}

	m_p->pCurrentEditStage->Save();

	omniClientLiveProcess();
	emit sig_StageDataChanged();
	
	return true;
}

bool OmniverseStage::AddMeshByData(const mip::MeshTopology& topology, const MeshInfo& meshInfo, mipUSDMeshPtr* ppOutUsdMeshLayer)
{
	//if (m_p->pCurrentEditStage->IsMeshExist(meshInfo.GetName().toStdString()))
	//{
	//	/* Mesh가 이미 존재하면 Pass*/
	//	return false;
	//}

	if (m_p->pCurrentEditStage->AddMesh(topology, meshInfo, ppOutUsdMeshLayer) == false)
	{
		return false;
	}

	m_p->pCurrentEditStage->Save();

	omniClientLiveProcess();
	emit sig_StageDataChanged();

	return true;
}

bool OmniverseStage::TrasformMesh(const std::string& meshName, mip::VECTOR3 v_translate_cm, mip::VECTOR3 v_rotate, mip::VECTOR3 v_scale)
{
	if (m_p->pCurrentEditStage->TrasformMesh(meshName, v_translate_cm, v_rotate, v_scale) == false)
	{
		return false;
	}

	emit sig_StageDataChanged();
	return true;
}

bool OmniverseStage::TranslateMesh_cm(const std::string& meshName, mip::VECTOR3 v_translate_cm)
{
	if (m_p->pCurrentEditStage->TranslateMesh_cm(meshName, v_translate_cm) == false)
	{
		return false;
	}

	emit sig_StageDataChanged();
	return true;
}

bool OmniverseStage::AddMaterialByMDLFile(const std::string& filepath, const std::string& name, const std::string& subIdentifier)
{
	if (m_p->pCurrentEditStage->AddMaterialByMDLFile(filepath, name, subIdentifier) == false)
	{
		return false;
	}

	m_p->pCurrentEditStage->Save();
	emit sig_StageDataChanged();

	return true;
}

bool OmniverseStage::BindMeshMaterial(const std::string& meshName, const std::string& materialName)
{
	if (m_p->pCurrentEditStage->BindMeshMaterial(meshName, materialName) == false)
	{
		return false;
	}

	m_p->pCurrentEditStage->Save();
	emit sig_StageDataChanged();

	return true;
}

bool OmniverseStage::UnbindMeshMaterial(const std::string& meshName, const std::string& materialName)
{
	mipUSDMeshPtr pMesh = m_p->pCurrentEditStage->CreateMeshByName(meshName);
	if (pMesh)
	{
		pMesh->UnbindMaterial();
	}

	m_p->pCurrentEditStage->Save();
	emit sig_StageDataChanged();

	return true;
}

bool OmniverseStage::SetVisible(const std::string& meshName, bool value)
{
	return m_p->pCurrentEditStage->SetVisible(meshName, value);
}

bool OmniverseStage::DeleteMeshByName(const std::string& name)
{
	return m_p->pCurrentEditStage->DeleteMeshByName(name);
}

bool OmniverseStage::IsMeshExist(const std::string& meshName) const
{
	return m_p->pCurrentEditStage->IsMeshExist(meshName);
}

std::vector<std::string> OmniverseStage::GetMeshNameList() const
{
	return m_p->pCurrentEditStage->GetMeshNameList();
}

std::vector<mipUSDMeshPtr> OmniverseStage::CreateMeshList() const
{
	return m_p->pCurrentEditStage->CreateMeshList();
}

mipUSDMeshPtr OmniverseStage::CreateMeshByName(const std::string& name) const
{
	return m_p->pCurrentEditStage->CreateMeshByName(name);
}

std::vector<mipUSDMaterialPtr> OmniverseStage::CreateMaterialList() const
{
	return m_p->pCurrentEditStage->CreateMaterialList();
}

mipUSDMaterialPtr OmniverseStage::CreateMeterialByName(const std::string& name) const
{
	return m_p->pCurrentEditStage->CreateMeterialByName(name);
}

bool OmniverseStage::UpdatePreset(const mipUsdPreset& preset)
{
	if (m_p->pCurrentEditStage->IsOpen() == false)
	{
		return false;
	}

	if (preset.IsPresetFileLoaded() == false)
	{
		return false;
	}

	std::vector<mipUSDMeshPtr> meshList = m_p->pCurrentEditStage->CreateMeshList();
	for (auto& pMesh : meshList)
	{
		pMesh->UnbindMaterial();
	}

	m_p->pCurrentEditStage->DeleteAllLights();
	m_p->pCurrentEditStage->DeleteAllMaterials();

	std::string usdaFilePath = preset.GetResourceUsdaFilePath();
	if (m_p->pCurrentEditStage->CopyUsdFile_OnlyMaterialAndLight(usdaFilePath) == false)
	{
		return false;
	}

	if (m_p->pCurrentEditStage->CopyUsdFile_CustomLayerRenderSetting(usdaFilePath) == false)
	{
		return false;
	}

	std::vector<mipPresetMeshMaterialPair> meshMaterialList = preset.GetMeshToMaterialPairList();
	for (auto& pair : meshMaterialList)
	{
		std::string meshName = pair.GetMeshName();
		std::string materialName = pair.GetMaterialName();

		mipUSDMeshPtr pMesh = m_p->pCurrentEditStage->CreateMeshByName(meshName);
		mipUSDMaterialPtr pMaterial = m_p->pCurrentEditStage->CreateMeterialByName(materialName);

		if ((pMesh != nullptr) && (pMaterial != nullptr))
		{
			pMesh->BindMaterial(pMaterial);
		}
	}

	m_p->pCurrentEditStage->Save();

	emit sig_StagePresetChanged();

	return true;
}

bool OmniverseStage::ClearPreset()
{
	if (m_p->pCurrentEditStage->IsOpen() == false)
	{
		return false;
	}

	std::vector<mipUSDMeshPtr> meshList = m_p->pCurrentEditStage->CreateMeshList();
	for (auto& pMesh : meshList)
	{
		pMesh->UnbindMaterial();
	}

	m_p->pCurrentEditStage->DeleteAllLights();
	m_p->pCurrentEditStage->DeleteAllMaterials();

	emit sig_StageDataChanged();

	return true;
}

void OmniverseStage::PushMeshWriteEvent()
{
	if (m_p->WriteWorker)
	{
		m_p->WriteWorker->PushQueueEvent(eWriterWorkerID::Changed);
	}
}

void OmniverseStage::UpdateStage()
{
	emit sig_StageDataChanged();
}

void OmniverseStage::slot_LiveChanged_Writer()
{
	qDebug() << "slot_LiveChanged_Writer";

	this->AddMeshList(*m_p->pMeshData);
}

void OmniverseStage::slot_LiveUpdate_Writer()
{
	omniClientLiveProcess();
}

void OmniverseStage::slot_LiveChanged_Reader()
{
	qDebug() << "slot_LiveChanged_Reader";

	emit sig_liveChanged();
}
