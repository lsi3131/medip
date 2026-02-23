#pragma once

#include <vector>
#include <QTreeWidget>
#include "graphics/MeshInfo.h"
#include "graphics/MeshLayerData.h"

class VOLUME_DATA;
class OmniverseContext;

class OmniverseMeshListWidget : public QTreeWidget
{
	Q_OBJECT
		
public:
	enum eColumn
	{
		COL_SUB = 0,
		COL_COLOR,
		COL_SHOW,
		COL_NAME,
		COL_ONLINE,
		COL_EDIT_MODE,
		COL_COUNT,
	};

	enum eOmniverseOnlineStatus
	{
		Online,
		Local,
	};

	enum eOmniverseEditMode
	{
		Open,
		Lock,
		Edit,
	};

public:
	OmniverseMeshListWidget(QWidget* parent);

public:
	void Init(OmniverseContext* pOmniverse);
	std::vector<MeshLayerData> GetSelectedMeshList() const;

	void Update();
	//void SetVolumeData(VOLUME_DATA* pVolumeData);

private:
	void updateList();

private:
	OmniverseContext* m_pOmniverse;
	std::vector<MeshLayerData> m_meshDataList;
};
