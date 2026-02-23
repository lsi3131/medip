#pragma once

#include "define.h"

#include "graphics/volumedata.h"
#include "graphics/MeshData.h"

#include <QtWidgets>

class WorkMeshToUSD : public QObject
{
	Q_OBJECT
public:
	WorkMeshToUSD(MeshData* pMeshData, const std::vector<int>& indexList, const QString& filepath);

public:
	void setProgressValue(int value, bool init = false);

public slots:
	void threadRun();

signals:
	void progress(int);
	void finished();

private:
	QString m_filePath;
	MeshData* m_pMeshData;
	std::vector<int> m_indexList;
};
