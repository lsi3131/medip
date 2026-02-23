#pragma once

#include "Windows/windowManager.h"

class ActionManager;
class ProductManager;
class DataContext;

namespace mip
{
	class Renderer;
}

class WindowManagerFileLoader
{
public:
	WindowManagerFileLoader(WindowManager* pWinManager);

public:
	bool fileOpen(const QString& filepath, bool bReverse = true);
	bool fileOpen_PET_DCM(const QString& filename);
	bool fileMultiOpen(const QStringList& filePathList);
	bool fileMultiImport(const QStringList& filePathList);

	bool loadDicomData(const mint16* HU, DcmtkSeriesInfo& dcmSeriesInfo, DicomVolumeInfo& dcmVolumeInfo);

	void loadNextFiles();
	bool loadFiles(const QString& filepath);

	bool loadRAW(const QString& mPath, bool reverse = true);
	bool loadROI(const QString& filepath);
	bool loadNII(const QString& filepath);
	bool loadPRD(const QString& filepath);
	bool loadTXT(const QString& filepath);

	bool loadSTL(const QString& filepath);
	bool loadOBJ(const QString& filepath);
	bool loadVTK(const QString& filepath);
	bool loadUSD(const QString& filepath);
	bool load3MF(const QString& filepath);

	bool loadImage(const QString& path);

	bool loadDCM(
		const QString& filepath,
		std::vector<SliceInfo>& fileslice,
		std::wstring& dir_path,
		DicomVolumeInfo& dcm_volume_info,
		DCM_MODAL_TYPE modality,
		bool multiOpen = false);
	DICOM_RESULT loadEtcFile(const QString& filepath);
	DICOM_RESULT loadPETFile(const QString& filepath);
	DICOM_RESULT loadFileToVolumeData_CT(VOLUME_DATA* pVolueData, DicomVolumeInfo* pDCMVolumeInfo, const QString& filepath, bool clearScreen);
	DICOM_RESULT loadFileToVolumeData_PET(VOLUME_DATA* pVolumeDataPET, DicomVolumeInfo* pDCMVolumeInfo, const QString& filepath);

	bool loadCroppingDicomFile(QString& filename);
	bool loadCroppingDicomData(const mint16* pHUdata, const DcmtkSeriesInfo& dcmSeriesInfo, const DicomVolumeInfo& dcmVolumeInfo);

	void SetMessageBox(MessageBoxBase* pMessageBox);

private:
	WindowManager* m_pWinManager;
	ActionManager* m_pActionManager;
	ProductManager* m_pProductManager;
	DataContext* m_pDataContext;
	mip::Renderer* m_pRenderer;
	MessageBoxBase* m_pMessageBox;
	MedipQT* m_mainWindow;
};
