#pragma once

#include <string>
#include "graphics/color.h"
#include "Renderer/MeshTopology.h"
#include "graphics/volumedata.h"
#include "Omniverse/Usd/mipUSDMesh.h"
#include "Omniverse/Usd/mipUSDMaterial.h"
#include "Omniverse/OmniverseFileInfo.h"
#include "Omniverse/OmniverseSharedResource.h"
#include "Omniverse/LiveSession/OmniverseLiveChannelMessage.h"
#include "Omniverse/OmniverseStage.h"
#include "Omniverse/OmniverseConnector.h"
#include "Omniverse/Config/OmniverseConfig.h"

namespace mip
{
	class Renderer;
}
class OmniverseContextPrivate;
class MeshData;
class mipUsdStage;
class mipUsdPreset;
class WindowManager;
class DataContext;
class CMeshWorkManager;
class CMeshManipulator;
class ShortcutManager;
class UsdMeshConverter;

class OmniverseContext : public QObject
{
	Q_OBJECT

public:
	static void OmniClientLiveQueuedCallbackImpl();

public:
	OmniverseContext();
	OmniverseContext(mip::Renderer* pRenderer);

	~OmniverseContext();

public:
	bool IsConnect() const;
	bool Connect(const std::string& ipAddress);
	bool Disconnect();
	bool Destroy();

	bool CreateOmniverseStage(const std::string& filePath);
	bool OpenOmniverseStage(const std::string& filePath);

	mipUsdStage* GetUsdStage();
	OmniverseStage* GetStage() const;
	OmniverseConnector* GetConnector() const;

	OmniverseConfig* GetConfig() const;

public:
	std::shared_ptr<OmniverseContextPrivate> m_p;
};

/*
	Helper
*/
std::vector<std::string> FindNotExistStringList(const std::vector<std::string>& oldList, const std::vector<std::string>& newList);
