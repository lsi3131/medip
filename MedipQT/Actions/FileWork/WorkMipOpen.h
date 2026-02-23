#pragma once

#ifndef WORK_MIP_OPEN_H
#define WORK_MIP_OPEN_H

#include <QtWidgets>

#include "graphics/volumedata.h"
#include "graphics/MeshData.h"
#include "defineMipEncoder.h"
#include "medipmipencoder.h"

class ProductManager;
class WindowManager;

class WorkMipOpen : public QObject
{
	Q_OBJECT

public:
	static void updateprogress(int value, void* data);
	static void updateprogress2(int value, void* data);
public:
	WorkMipOpen(const QString& filepath, WindowManager* pWinManager, VOLUME_DATA* pVolume, MeshData* pMeshData, ProductManager* pProduct);
	virtual ~WorkMipOpen();

private:
	void setProgressValue(int value, bool init = false);

	void initMeshData(MIP_ENCODER::ProjectDataInfo& proj);

public slots:
	void threadRun();

signals:
	void clearVolume();
	void setDepth(int type, float depth);
	void updateAnno();
	void createHistogram();
	void resetRC();
	void resetRG();
	void resetWork();
	void setPreset(bool, int, int, int, int);
	void wheelZoom(bool);
	void progress(int);
	void setGamma(bool, bool);
	void resetUI();
	void UpdateUI();
	void UpdateSummary();
	void UpdateReport();
	void finished();

public:
	WindowManager* m_pWinManager;
	VOLUME_DATA* m_pVolumData;
	MeshData* m_pMeshData;
	QString m_filePath;
	ProductManager* m_pProduct;
};
#endif
