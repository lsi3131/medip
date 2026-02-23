/*****************************************************************//**
 * \file   DataContext.h
 * \brief  Data Context 클래스 헤더파일
 * 
 * \author heog
 * \date   October 2022
 *********************************************************************/

#pragma once

#ifndef DATACONTEXT_H
#define DATACONTEXT_H

#include "volumedata.h"
#include "volumedatapet.h"
#include "MeshData.h"

#include <memory>
#include <QObject>

struct MeshInfo;
class mipUsdPresetManager;
class OmniverseContext;
class WindowManager;
class CMeshWorkManager;
class CMeshManipulator;
class ShortcutManager;

class DataContext : public QObject
{
	Q_OBJECT

public:
	static DataContext* getSingleton();

private:
	static std::shared_ptr<DataContext> m_pInstance;

public: 
	DataContext();
	~DataContext();

public:
	void SetWindowManager(WindowManager* pWinManager);
	void SetMesh(CMeshWorkManager* pMeshWorkManager, CMeshManipulator* pMeshManipulator);
	void SetShorcutManager(ShortcutManager* pShortcutManager);

	void Clear();

	mipUsdPresetManager* GetOmniversePresetManager();
	OmniverseContext* GetOmniverseContext();

	/* TODO: 아래 signal을 하위 context를 생성하여 분배할 것 */
Q_SIGNALS:
	void sig_meshSelectChanged();
	void sig_meshInfoChanged(MeshInfo* pInfo);

private slots:
	void slot_OmniverseStageUpdate();
	void slot_OmniverseLiveChanged();

public:
	VOLUME_DATA volume_data;
	VOLUME_DATA_PET volume_data_PET;

	MeshData m_MeshData;
	MeshData m_VisualPrinting_MeshData;

private:
	std::shared_ptr<mipUsdPresetManager> m_pOmniversePresetManager;
	std::shared_ptr<OmniverseContext> m_pOmniverseContext;

	WindowManager* m_pWinManager;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshManipulator* m_pMeshManipulator;
	ShortcutManager* m_pShortcutManager;
};
#define DATA_CONTEXT (DataContext::getSingleton())

#endif

