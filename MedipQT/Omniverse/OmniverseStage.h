#pragma once

#include <QObject>
#include <memory>
#include "Omniverse/Usd/mipUsdStage.h"
#include "Omniverse/Usd/mipUSDMaterial.h"
#include "Omniverse/Usd/mipUSDMesh.h"
#include "Omniverse/Usd/mipUSDPreset.h"
#include "Omniverse/OmniverseFileInfo.h"
#include "Omniverse/OmniverseSharedResource.h"
#include "Omniverse/OmniverseConnector.h"
#include "Omniverse/LiveSession/OmniverseLiveSession.h"
#include "Omniverse/LiveSession/OmniverseLiveChannelMessage.h"
#include "Omniverse/LiveSession/OmniversePeerUser.h"

#include "graphics/color.h"
#include "graphics/MeshInfo.h"
#include "OmniverseUtils.h"

namespace mip
{
	class Renderer;
}

class OmniverseStage;
class OmniverseStagePrivate;
class MeshData;

using OmniverseStagePtr = std::shared_ptr<OmniverseStage>;

class OmniverseStage : public QObject
{
	Q_OBJECT
public:
	static bool IsSupportFileExtension(const std::string& fileName);

public:
	OmniverseStage();

public:
	bool IsOpen() const;
	bool Create(OmniverseConnector* pConnection, const std::string& filepath);
	bool Open(OmniverseConnector* pConnection, const std::string& filepath);
	bool Close();

	std::string GetStageUrl() const;
	std::string GetFileName() const;
	std::string GetFileBaseName() const;

	mipUsdStage* GetUsdStage() const;
	OmniverseConnector* GetConnector() const;
	OmniverseLiveSession* GetLiveSession() const;
	OmniverseLiveChannel* GetLiveChannel() const;
	std::string GetLiveSessionName() const;

	void SetMeshData(MeshData* pMeshData);

	bool AddMeshList(const MeshData& meshContainer);
	bool AddMesh(const MeshLayerData& m);

	void PushMeshWriteEvent();

	bool UpdatePreset(const mipUsdPreset& preset);
	bool ClearPreset();

	void SetLiveModeOn();
	void SetLiveModeOff();

	bool OpenLiveChannel(const std::string& sessionName);
	bool CloseLiveChannel();

	bool JoinLiveSession(const std::string& sessionName);
	bool CreateLiveSession(const std::string& sessionName);
	bool LeaveLiveSession();
	bool IsLiveSessionMode() const;

	bool AddMeshByStlFile(const std::string& inputFilePath, const std::string& meshLayerName, mip::Renderer* pRenderer, COLOR color = COLOR(255, 0, 0));
	bool AddMeshByData(const mip::MeshTopology& topology, const MeshInfo& meshInfo, mipUSDMeshPtr* ppOutUsdMeshLayer = nullptr);

	bool TrasformMesh(const std::string& meshName, mip::VECTOR3 v_translate_cm, mip::VECTOR3 v_rotate, mip::VECTOR3 v_scale);
	bool TranslateMesh_cm(const std::string& meshName, mip::VECTOR3 translate_cm);

	bool AddMaterialByMDLFile(const std::string& filepath, const std::string& name, const std::string& subIdentifier);
	bool BindMeshMaterial(const std::string& meshName, const std::string& materialName);
	bool UnbindMeshMaterial(const std::string& meshName, const std::string& materialName);

	std::vector<mipUSDMeshPtr> CreateMeshList() const;
	mipUSDMeshPtr CreateMeshByName(const std::string& name) const;

	std::vector<mipUSDMaterialPtr> CreateMaterialList() const;
	mipUSDMaterialPtr CreateMeterialByName(const std::string& name) const;

	bool SetVisible(const std::string& meshName, bool value);

	bool DeleteMeshByName(const std::string& name);

	bool IsMeshExist(const std::string& meshName) const;
	std::vector<std::string> GetMeshNameList() const;

	void UpdateStage();
private:

Q_SIGNALS:
	void sig_liveChanged();
	void sig_StageDataChanged();
	void sig_StagePresetChanged();
	void sig_recvLiveSessionMessage(OmniverseLiveChannelMessagePtr pMessage, std::set<OmniversePeerUser> peerUserList);

private slots:
	void slot_LiveChanged_Writer();
	void slot_LiveUpdate_Writer();

	void slot_LiveChanged_Reader();


private:
	std::shared_ptr<OmniverseStagePrivate> m_p;
};

