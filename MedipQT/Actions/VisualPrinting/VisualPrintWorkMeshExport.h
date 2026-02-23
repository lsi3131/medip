#pragma once

#ifndef WORK_VISUALPRINT_MESH_EXPORT_H
#define WORK_VISUALPRINT_MESH_EXPORT_H

#include <QtWidgets>

#include "MeshData.h"

namespace mip
{
	class MeshTopology;
};

// 201005 허 건 대리
// Mesh-list 통한 visual print 수행 클래스
class VisualPrintWorkMeshExport : public QObject
{
	Q_OBJECT

public:
	VisualPrintWorkMeshExport(MeshData* pVisualPrintMeshData, MeshData* pOriginMeshData);
	virtual ~VisualPrintWorkMeshExport();

public slots:
	void slot_threadRun();

signals:
	void sig_progress(int, QString);
	void sig_finished();

	void sig_buildRenderBufferTopology(mip::MeshTopology* _p_mesh);
	void sig_renderLater();

private:
	MeshData* m_pVisualPrintMeshData;
	MeshData* m_pSourceMeshData;
};
#endif
