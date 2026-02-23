#pragma once

#include <Windows.h>
#include <vector>
#include "defineMEDIP.h"
#include "color.h"

#define MESHINFO_TEXT_LENGTH_MAX	256

struct MeshInfo
{
public:
	MeshInfo();
	MeshInfo(const MeshInfo& rhs);

public:
	void SetName(const QString& name);
	QString GetName() const;

	void SetOmniverseStatus(eOmniverseStatus status);
	eOmniverseStatus GetOmniverseStatus() const;
	QString GetOmniverseStatus_Text() const;

public:
	mint32 uid; //0~29		----> layer uid (mesh uid ---> map key) //NOT CONNECTED : -1
	COLOR color;
	WCHAR MeshName[MESHINFO_TEXT_LENGTH_MAX];
	bool show;
	bool selected;
	muint8 NameLength; //only use projectfile read
	bool upScale;
	muint32 TrisCount; //only use projectfile read
	muint32 NorCount; //only use projectfile read	

	std::vector<std::vector<mip::VECTOR3>> vecOutline2DAxial;
	std::vector<std::vector<mip::VECTOR3>> vecOutline2DCoronal;
	std::vector<std::vector<mip::VECTOR3>> vecOutline2DSagittal;

	// For Export Mesh
	int m_nSmoothLevel;
	int m_nReduceLevel;
	int	m_nRemeshLevel;

	// 211123 허 건 과장
	bool m_bCollisionState; // Check collision between mesh adn volume

private:
	eOmniverseStatus m_omniverseStatus;
	std::string m_metrialFilePath;
};

