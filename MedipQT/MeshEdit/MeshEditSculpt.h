#pragma once

#include <QPoint>
#include <QSize>

namespace mip
{
	class Renderer;
	class MeshTopology;
}

class ActionManager;
class WindowManager;
class MeshData;
class VOLUME_DATA;
class CMeshWorkManager;
class CMeshModelViewManager;
class CMeshDlgManager;

enum class eMouseMode
{
	Press = 0,
	Move = 1,
	Release = 2,
};

enum class eMeshSelectMode
{
	Select = 0,
	Unselect = 1,
};

class MeshEditSculpt
{
public:
	MeshEditSculpt(
		mip::Renderer* pRenderer,
		ActionManager* pActionManager,
		WindowManager* pWinManager,
		MeshData* pMeshData,
		VOLUME_DATA* pVolumeData,
		CMeshWorkManager* pMeshWorkManager,
		CMeshModelViewManager* pMeshModelViewManager,
		CMeshDlgManager* pMeshDlgManager
	);

public:
	mip::VECTOR3 GetPickPoint() const;
	mip::VECTOR3 GetPickVertPoint() const;

	void SetSphereRadius(float value);
	float GetSphereRadius() const;

	bool ProcessMove(eMouseMode mouseMode, const QPoint& prevMousePoint, const QPoint& newMousePoint, const QSize& screenSize);
	bool ProcessRelease(const QPoint& prevMousePoint, const QPoint& newMousePoint, const QSize& screenSize);

	bool ReadyBrush(MESH_WORK_MODE _MeshWorkMode, int initial = 0);

	bool ProcessSelect(eMeshSelectMode selectMode, const QPoint& mousePoint, const QSize& screenSize);

	bool ProcessBrushSculpt(const QPoint& mousePoint, const QSize& screenSize);

	bool MoveSphere(const QPoint& mousePoint, const QSize& screenSize);

	bool PointTracing(
		mip::VECTOR3& result, 
		const QPoint& point, 
		const QSize& screenSize,
		int& vertIdx,
		mip::MeshTopology* in_mesh, 
		const mip::MATRIX44& mat, 
		const mip::MATRIX44& matworld,
		bool saveFlag = true);

private:
	mip::Renderer* m_pRenderer;
	ActionManager* m_pActionManager;
	WindowManager* m_pWinManager;
	MeshData* m_pMeshData;
	VOLUME_DATA* m_pVolumeData;
	CMeshWorkManager* m_pMeshWorkManager;
	CMeshModelViewManager* m_pMeshModelViewManager;
	CMeshDlgManager* m_pMeshDlgManager;

private:
	mip::VECTOR3 m_realPickPT;
	mip::VECTOR3 m_1stDirPickFromScreen = mip::VECTOR3(0.0f, 0.0f, 0.0f);
	mip::VECTOR3 m_pickPtOld = mip::VECTOR3(0.0f, 0.0f, 0.0f);

	mip::VECTOR3 m_pickPoint;
	mip::VECTOR3 m_pickVertPoint;

	float m_sphereRadius;
};
