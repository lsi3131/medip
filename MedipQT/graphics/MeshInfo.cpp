#include "stdafx.h"
#include "MeshInfo.h"

MeshInfo::MeshInfo()
{
	memset(MeshName, 0, sizeof(MeshName));
	uid = -1;
	NameLength = 0;

	TrisCount = 0;
	NorCount = 0;

	m_nSmoothLevel = 0;
	m_nReduceLevel = 0;
	m_nRemeshLevel = 0;

	show = false;
	selected = false;
	upScale = false;
	m_bCollisionState = false;

	m_omniverseStatus = eOmniverseStatus::Unlock;
}

MeshInfo::MeshInfo(const MeshInfo& rhs)
{
	color = rhs.color;
	memcpy(MeshName, rhs.MeshName, sizeof(rhs.MeshName));

	show = rhs.show;
	selected = rhs.selected;
	NameLength = rhs.NameLength;
	upScale = rhs.upScale;
	TrisCount = rhs.TrisCount;
	NorCount = rhs.NorCount;

	vecOutline2DAxial = rhs.vecOutline2DAxial;
	vecOutline2DCoronal = rhs.vecOutline2DCoronal;
	vecOutline2DSagittal = rhs.vecOutline2DSagittal;

	m_nSmoothLevel = rhs.m_nSmoothLevel;
	m_nReduceLevel = rhs.m_nReduceLevel;
	m_nRemeshLevel = rhs.m_nRemeshLevel;

	m_bCollisionState = rhs.m_bCollisionState;

	m_omniverseStatus = rhs.m_omniverseStatus;
	m_metrialFilePath = rhs.m_metrialFilePath;
}

void MeshInfo::SetName(const QString& name)
{
	memset(MeshName, 0, sizeof(MeshName));
	name.toWCharArray(MeshName);
	NameLength = name.length();
}

QString MeshInfo::GetName() const
{
	return QString::fromWCharArray(MeshName);
}

void MeshInfo::SetOmniverseStatus(eOmniverseStatus status)
{
	if (m_omniverseStatus != status)
	{
		qInfo() << QString("omniverse value changed <%1> -> <%2>").arg((int)m_omniverseStatus).arg((int)status);
		m_omniverseStatus = status;
	}
}

eOmniverseStatus MeshInfo::GetOmniverseStatus() const
{
	return m_omniverseStatus;
}

QString MeshInfo::GetOmniverseStatus_Text() const
{
	if (m_omniverseStatus == eOmniverseStatus::Lock)
	{
		return "Lock";
	}
	else
	{
		return "Unlock";
	}
}
