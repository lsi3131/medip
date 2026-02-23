#pragma once

#ifndef STAMP3D_DLG_H
#define STAMP3D_DLG_H

#include "define.h"
#include "defineMEDIP.h"
#include "MeshBaseDlg.h"
#include "Renderer/MeshTopology.h"

#define STAMP3D_TEST_FONTMESH_VISIBLE 0
#define STAMP3D_OFFSET_ADJUSTMENT (0.03f)
#define STAMP3D_DIMENSION (36) // 크기가 아닌 정점의 밀도라고 이해하면 될듯

//Offset
//EdgeLength
class Stamp3dDlg : public MeshBaseDlg
{
	Q_OBJECT

private:
	double			m_dOffsetDistance;
	double			m_nEdgeLength;
	mip::VECTOR3	m_PickingPos;
	int				m_nPickFaceIdx;
	mip::VECTOR3	m_OldPickingPos;
	mip::QUATERNION m_accumulatedQuater;

	float			m_fOffsetMin;
	float			m_fOffsetMax;
	int				m_FontSize;

	QFont			m_Font;
	QLineEdit* m_pTextLineEdit;

	//std::shared_ptr<mip::MeshTopology> m_fontMesh;
	mip::MeshTopology* m_fontMesh;
	mip::MeshTopology* m_targetMesh;
	std::vector<mip::TVert> m_orgFontTverts;
	mip::AABB				m_orgFontBoundingBox;
	bool					m_bEnable;

public:
	Stamp3dDlg(DataContext* pDataContext, QWidget* parent = NULL);
	Stamp3dDlg(DataContext* pDataContext, QString _title, QWidget* parent = NULL);
	virtual ~Stamp3dDlg();
	virtual void reject(bool bForce = false);

	double				GetOffsetDist() { return m_dOffsetDistance; }
	int					GetFontSize() { return m_FontSize; }
	mip::VECTOR3		GetPickPos() { return m_PickingPos; }
	int					GetPickFaceIdx() { return m_nPickFaceIdx; }
	mip::VECTOR3		GetPickOldPos() { return m_OldPickingPos; }
	mip::QUATERNION		GetAccQuater() { return m_accumulatedQuater; }
	std::vector<mip::TVert>& GetOrgFontTVerts() { return m_orgFontTverts; }
	mip::AABB			GetOrgFontBox() { return m_orgFontBoundingBox; }
	mip::MeshTopology* GetTargetMesh() { return m_targetMesh; }
	mip::MeshTopology* GetFontMesh() { return m_fontMesh; }
	bool				GetStampEnable() { return m_bEnable; }
	//std::shared_ptr<mip::MeshTopology>	GetFontMesh() { return m_fontMesh; }

	void	SetOffsetDist(double val) { m_dOffsetDistance = val; }
	void	SetFontSize(double val) { m_FontSize = val; }
	void	SetPickPos(mip::VECTOR3 val) { m_PickingPos = val; }
	void	SetPickFaceIdx(int val) { m_nPickFaceIdx = val; }
	void	SetPickOldPos(mip::VECTOR3 val) { m_OldPickingPos = val; }
	void	SetAccQuater(mip::QUATERNION quat) { m_accumulatedQuater = quat; }
	void	SetTargetMesh(mip::MeshTopology* pMT) { m_targetMesh = pMT; }
	void	SetFontMesh(mip::MeshTopology* pMT) { m_fontMesh = pMT; }
	void	SetOrgFontBox(mip::AABB box) { m_orgFontBoundingBox = box; }
	void	SetStampEnable(bool Enable) { m_bEnable = Enable; }
	//void	SetFontMesh(std::shared_ptr<mip::MeshTopology> pMT) { m_fontMesh = pMT; }

protected:
	void closeEvent(QCloseEvent*) override;

private:
	void	initQtUI();
	void	initStampData();
	double	calcEdgeLength(mip::MeshTopology* pMT);

	void exitDialog();

private slots:
	void OnOK();
	void OnCancel();
	void OnTextChanged();
	void OnDepthSlideReleased(int);
	void OnComboChanged(int);
	void OnChangeFontStyle();
	void OnMakefont3D();
	void OnUpdateStamp3D();

};
#endif