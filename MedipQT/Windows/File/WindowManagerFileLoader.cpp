#include "stdafx.h"
#include "WindowManagerFileLoader.h"
#include "FunctionLevel.h"
#include "ProductManager.h"
#include "StringManager.h"
#include "LicenseManager.h"
#include "MedipQT.h"
#include "ActionImageIsotropic.h"
#include "Windows/Main/MainSegmentWidget.h"
#include "Renderer/MeshTopology.h"
#include "Renderer/model.h"
#include "3MF/C3MFLoader.h"

#include "Dialogs/DicomSeriesSelectDialog.h"
#include "Dialogs/CroppingDialog.h"

WindowManagerFileLoader::WindowManagerFileLoader(WindowManager* pWinManager) :
	m_pWinManager(pWinManager)
{
	m_pActionManager = m_pWinManager->m_pActionManager;
	m_pProductManager = m_pWinManager->m_pProductManager;
	m_pDataContext = m_pWinManager->m_pDataContext;
	m_pRenderer = m_pWinManager->m_pRenderer;
	m_pMessageBox = m_pWinManager->m_pMessageBox.get();
	m_mainWindow = m_pWinManager->mainWindow;
}

bool WindowManagerFileLoader::fileOpen(const QString& filepath, bool bReverse)
{
	QString openFilePath = QString::fromLocal8Bit(filepath.toLocal8Bit());

	qDebug() << "open file path :" << openFilePath;

	QString ext = openFilePath.section('.', -1);
	ext = ext.toLower();

#ifdef ONLY_FOR_FTP_UPLOAD_VER

#else 
	if (!m_pWinManager->IsLicensePass())
	{
		m_pMessageBox->warning(nullptr, "License", "The program must be licensed to activate.");
		return false;
	}

#endif

	bool bMIP_File = (0 == ext.compare("mip") && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIP));
#ifdef OPEN_MIPD_IN_MEDIP
	bool bMIPD_File = (0 == ext.compare("mipd"));
#else
	bool bMIPD_File = (0 == ext.compare("mipd") && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPD));
#endif
	bool bMIPA_File = (0 == ext.compare("mipa") && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_MIPA));
	bool bNII_File = (0 == ext.compare("nii") && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask));
	bool bRAW_File = (0 == ext.compare("raw") && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask));
	bool bPRD_File = (0 == ext.compare("prd") && (m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising) && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetcustomising)));
	bool bROI_File = (0 == ext.compare("roi") && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask));
	bool bTXT_File = (0 == ext.compare("txt") && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask));
#ifdef MESH_TEST
	bool bSTL_File = (0 == ext.compare("stl") && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport));
	bool bOBJ_File = (0 == ext.compare("obj") && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport));
	bool bVTK_File = (0 == ext.compare("vtk") && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport));
#ifdef USD_IMPORT
	bool bUSD_File = (0 == ext.compare("usd") && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport));
#else
	bool bUSD_File = false;
#endif
	bool b3MF_File = (0 == ext.compare("3mf") && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport));
#endif
	bool bIMG_File = ((0 == ext.compare("png") || 0 == ext.compare("bmp") || 0 == ext.compare("jpg")) && m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Report_ReportImport));

	// ExistFileSave 체크. => mip, mipd, mipa, dicom일 경우만 체크.
	if (!bNII_File && !bRAW_File && !bPRD_File && !bROI_File && !bTXT_File
#ifdef MESH_TEST
		&& !bSTL_File && !bOBJ_File && !bVTK_File && !bUSD_File && !b3MF_File
#endif
		&& !bIMG_File)
	{
		if (m_pWinManager->ShouldCheckSaveStatus())
		{
			int res = m_pMessageBox->warning(m_mainWindow,
				STRING_MANAGER->getString(STR_SAVE_FILE),
				STRING_MANAGER->getString(STR_SAVE_FILE_DESC),
				QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel);
			if (res == QMessageBox::Ok)
			{
				m_mainWindow->ExistFileSave(openFilePath);
				return true;
			}
			else if (res == QMessageBox::Cancel)
			{
				return false;
			}
		}
	}

	m_pWinManager->InitViewControlsState();

#ifdef _M_IX86
	m_pWinManager->setRenderType(RT_SURFACE);
#endif // DEBUG
	m_pWinManager->pRadiomics3DVolume = nullptr;

	if (bMIP_File)
	{
		//20201112_byPHS - Dlg reject
		m_pWinManager->rejectMEViewDlg();

		m_pWinManager->setRenderable(false);

		bool bRet = true;
		if (m_pActionManager->m_IsMacroMode)
		{
			bRet = m_pActionManager->action_FileWork_Import_Mip(openFilePath, MIP_ENCODER::PT_MIP, false);
		}
		else
		{
			m_pActionManager->action_FileWork_Import_Mip(openFilePath, MIP_ENCODER::PT_MIP);
		}
		m_pWinManager->setCoordType(INVALID_TYPE);
		m_pWinManager->setVolumeAlphaSlider(m_pWinManager->getVolumeAlpha(true));
		m_pWinManager->setLayerAlphaSlider(m_pWinManager->getLayerAlpha(true));

		m_pWinManager->setHeatMapMode(false);

		return bRet;
	}
	else if (bMIPD_File)
	{
		m_pWinManager->setRenderable(false);

		bool bRet = true;
		if (m_pActionManager->m_IsMacroMode)
		{
			bRet = m_pActionManager->action_FileWork_Import_Mip(openFilePath, MIP_ENCODER::PT_MIPD, false);
		}
		else
		{
			m_pActionManager->action_FileWork_Import_Mip(openFilePath, MIP_ENCODER::PT_MIPD);
		}

		m_pWinManager->setCoordType(INVALID_TYPE);
		m_pWinManager->setVolumeAlphaSlider(m_pWinManager->getVolumeAlpha(true));
		m_pWinManager->setLayerAlphaSlider(m_pWinManager->getLayerAlpha(true));

		m_pWinManager->setHeatMapMode(false);

		return bRet;
	}
	else if (bMIPA_File)
	{
		m_pWinManager->setRenderable(false);

		bool bRet = m_pActionManager->action_FileWork_Import_Mip(openFilePath, MIP_ENCODER::PT_MIPA);

		m_pWinManager->setCoordType(INVALID_TYPE);
		m_pWinManager->setVolumeAlphaSlider(m_pWinManager->getVolumeAlpha(true));
		m_pWinManager->setLayerAlphaSlider(m_pWinManager->getLayerAlpha(true));

		m_pWinManager->setHeatMapMode(false);

		return bRet;
	}
	else if (bNII_File)
	{
		return loadNII(openFilePath);
	}
	else if (bRAW_File)
	{
		m_pWinManager->resetMaskTexture();
		return loadRAW(openFilePath, bReverse);
	}
	else if (bPRD_File)
	{
		return loadPRD(openFilePath);
	}
	else if (bROI_File)
	{
		m_pWinManager->resetMaskTexture();
		return loadROI(openFilePath);
	}
	else if (bTXT_File)
	{
		m_pWinManager->resetMaskTexture();
		return loadTXT(openFilePath);
	}
#ifdef MESH_TEST
	else if (bSTL_File)
	{
		bool b_success = loadSTL(openFilePath);

		if (b_success)
		{
			m_pWinManager->updateMeshUI();
		}

		return b_success;
	}
	else if (bOBJ_File)
	{
		bool b_success = loadOBJ(openFilePath);

		if (b_success)
		{
			m_pWinManager->updateMeshUI();
		}

		return b_success;
	}
	else if (bVTK_File)
	{
		bool b_success = loadVTK(openFilePath);

		if (b_success)
		{
			m_pWinManager->updateMeshUI();
		}

		return b_success;
	}
	else if (bUSD_File)
	{
		bool b_success = loadUSD(openFilePath);

		if (b_success)
		{
			m_pWinManager->updateMeshUI();
		}

		return b_success;
	}
	else if (b3MF_File)
	{
		bool b_success = load3MF(openFilePath);

		if (b_success)
		{
			m_pWinManager->updateMeshUI();
		}

		return b_success;
	}
#endif
	else if (bIMG_File)
	{
		return loadImage(openFilePath);
	}
	else
	{
		if (m_pWinManager->mainSegmentWidget)
		{
			VolumeView* vView = static_cast<VolumeView*>(m_pWinManager->mainSegmentWidget->getViewVolume());

			if (vView)
			{
				vView->resetDisplayMode();
			}
		}

		if (THREAD_NONE != m_pActionManager->GetAfterThread())
		{
			return true;
		}

		if (0 == ext.compare("mip") || 0 == ext.compare("mipd"))
		{
			QString strMsgTitle = QString("'%1' file open.").arg(ext);
			m_pMessageBox->warning(nullptr, strMsgTitle, "It is not supported file format.");
			return false;
		}

		DICOM_RESULT ret = loadEtcFile(openFilePath);

		if (ret == DR_LOW_SPACING)
		{
			// MACRO 문제로 인해 동기화처리
			//m_pActionManager->action_ImageIsotropic_Modification();	
			//m_pActionManager->action_ProgressEnd();
			//QString str = "Loading...";
			//m_pActionManager->action_ProgressBegin(str);

			m_pWinManager->m_pDataContext->volume_data.threadStop = false;
			m_pWinManager->m_pDataContext->volume_data.createTempMaskData();

			WorkImageIsotropicConversionModification work(&m_pWinManager->m_pDataContext->volume_data);
			m_mainWindow->connect(&work, SIGNAL(progress(int)), m_mainWindow, SLOT(OnUpdateProgress(int)));
			work.threadRun();

			ActionImageIsotropicConversionModification actionIso(&m_pWinManager->m_pDataContext->volume_data, m_pWinManager->m_pDataContext->volume_data.threadResult);
			actionIso.redo();
		}
		else if (ret == DR_NORMAL)
		{
			m_pWinManager->setRenderable(true);
		}

		m_pActionManager->action_ProgressEnd();

		return ret;
	}
}

bool WindowManagerFileLoader::fileOpen_PET_DCM(const QString& filename)
{
	DICOM_RESULT ret = loadPETFile(filename);

	if (ret == DR_NORMAL)
	{
	}
	m_pWinManager->setRenderable(true);
	m_pActionManager->action_ProgressEnd();

	return ret == DR_NORMAL;
}

bool WindowManagerFileLoader::fileMultiOpen(const QStringList& filePathList)
{
	if (filePathList.isEmpty())
	{
		return false;
	}
	m_pActionManager->SetAfterThread(THREAD_IMPORT_FILES);
	m_pWinManager->importList.reserve(filePathList.size());

	for (QString fileName : filePathList)
	{
		m_pWinManager->importList.push_back(fileName);
	}

	loadFiles(m_pWinManager->importList.takeFirst());

	return true;
}

bool WindowManagerFileLoader::fileMultiImport(const QStringList& filePathList)
{
	if (filePathList.isEmpty())
	{
		return false;
	}
	m_pActionManager->SetAfterThread(THREAD_IMPORT_FILES);
	m_pWinManager->importList.reserve(filePathList.size());

	for (QString fileName : filePathList)
	{
		m_pWinManager->importList.push_back(fileName);
	}

	if (loadFiles(m_pWinManager->importList.takeFirst()) == false)
	{
		/* 실패 시 THREAD_NONE으로 설정하여 side effect 방지 */
		m_pActionManager->SetAfterThread(THREAD_NONE);
		return false;
	}

	return true;
}

bool WindowManagerFileLoader::loadDicomData(const mint16* HU, DcmtkSeriesInfo& dcmSeriesInfo, DicomVolumeInfo& dcmVolumeInfo)
{
	m_pActionManager->action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_DICOM_FILE));
	m_pActionManager->action_ProgressUpdate(0);
	m_pActionManager->clear();

	/* series info update*/
	m_pWinManager->GetDicomInfo(&dcmSeriesInfo);

	bool init = dcmVolumeInfo.xAxis == mip::VECTOR3().Zero;

	m_pDataContext->volume_data.setImgOrientation(true, dcmVolumeInfo.xAxis, init);

	init = dcmVolumeInfo.yAxis == mip::VECTOR3().Zero;

	m_pDataContext->volume_data.setImgOrientation(false, dcmVolumeInfo.yAxis, init);

	bool bCreateMemory = m_pDataContext->volume_data.createData(
		dcmVolumeInfo.dx,
		dcmVolumeInfo.dy,
		dcmVolumeInfo.dz,
		dcmVolumeInfo.x_spacing * 0.1f,
		dcmVolumeInfo.y_spacing * 0.1f,
		dcmVolumeInfo.z_spacing * 0.1f
	);

	int size = dcmVolumeInfo.dx * dcmVolumeInfo.dy * dcmVolumeInfo.dz * sizeof(mint16);
	memcpy(m_pDataContext->volume_data.getHUDataPoint(), HU, size);

	DcmtkSeriesInfo* pDcmInfo = m_pWinManager->GetDicomInfo();

	int len = pDcmInfo->imagePosition.length();
	if (len > 1)
	{
		QString tmpStr = pDcmInfo->imagePosition.c_str();
		QStringList tmpList = tmpStr.split("/");

		if (tmpList.count() == 3)
		{
			float pos[3];
			for (int i = 0; i < 3; i++)
				pos[i] = tmpList.at(i).toFloat();

			m_pDataContext->volume_data.setImgPos(pos[0], pos[1], pos[2]);
		}
	}

	bool bIsotropic = false;
	float fThresholdSpacing = 0.95f;

	if ((!LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH) || !LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH_V2))
		&& m_pDataContext->volume_data.getSpaceX(true) < fThresholdSpacing
		&& m_pDataContext->volume_data.getSpaceY(true) < fThresholdSpacing)
	{
		bIsotropic = true;
	}

	/* 병목 구간 존재 */
	m_pWinManager->initMedipUIData(dcmVolumeInfo.window_width, dcmVolumeInfo.window_level);

	m_pWinManager->setRenderable(true);
	m_pWinManager->setHeatMapMode(false);

	m_pActionManager->action_ProgressEnd();
	return true;
}

void WindowManagerFileLoader::loadNextFiles()
{
	if (m_pWinManager->importList.isEmpty())
	{
		m_pActionManager->SetAfterThread(THREAD_NONE);
		return;
	}

	if (m_pActionManager->GetAfterThread() != THREAD_IMPORT_FILES)
	{
		m_pWinManager->importList.clear();
		return;
	}

	QString filename = m_pWinManager->importList.takeFirst();
	loadFiles(filename);
}

bool WindowManagerFileLoader::loadFiles(const QString& filepath)
{
	QString ext = filepath.section('.', -1);
	if (!ext.compare("prd") || !ext.compare("PRD"))
	{
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Rendering_2D_3DHistogram_Presetfileimport))
		{
			return false;
		}

		return loadPRD(filepath);
	}
	else if (!ext.compare("stl") || !ext.compare("STL"))
	{
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
		{
			return false;
		}

		return loadSTL(filepath);
	}
	else if (!ext.compare("obj") || !ext.compare("OBJ"))
	{
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
		{
			return false;
		}

		return loadOBJ(filepath);
	}
	else if (!ext.compare("vtk") || !ext.compare("VTK"))
	{
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
		{
			return false;
		}

		return loadVTK(filepath);
	}
	else if (!ext.compare("3mf") || !ext.compare("3MF"))
	{
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_MeshEditing_FileImport))
		{
			return false;
		}

		return load3MF(filepath);
	}
	else if (!ext.compare("raw") || !ext.compare("RAW"))
	{
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask))
		{
			return false;
		}

		return loadRAW(filepath);
	}
	else if (!ext.compare("nii") || !ext.compare("NII"))
	{
		if (!(m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_RAW_Mask)
			|| m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_NII_HU)))
		{
			return false;
		}

		return loadNII(filepath);
	}
	else if (!ext.compare("txt") || !ext.compare("TXT"))
	{
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Import_TXTcoordinate))
		{
			return false;
		}

		return loadTXT(filepath);
	}
	else if (!ext.compare("jpg") || !ext.compare("JPG") || !ext.compare("png") || !ext.compare("PNG")
		|| !ext.compare("bmp") || !ext.compare("BMP")
		)
	{
		if (!m_pProductManager->IsAvailableFunctionLevel(MFL_Common_Report_ImageManagement_ImageImport))
		{
			return false;
		}

		return loadImage(filepath);
	}

	return false;
}

bool WindowManagerFileLoader::loadRAW(const QString& mPath, bool reverse)
{
	if (!m_pDataContext->volume_data.isValidate())
	{
		m_pMessageBox->warning(
			nullptr, 
			STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD),
			STRING_MANAGER->getString(STR_LOAD_FIRST));
		return false;
	}

	if (!m_pActionManager->action_MaskList_addRawFile(mPath, reverse))
	{
		return false;
	}

	return true;
}

bool WindowManagerFileLoader::loadROI(const QString& filepath)
{
	if (!m_pDataContext->volume_data.isValidate())
	{
		m_pMessageBox->warning(
			nullptr, 
			STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD),
			STRING_MANAGER->getString(STR_LOAD_FIRST));
		return false;
	}

	if (m_pDataContext->volume_data.getMaskInfoListCnt() >= MASK_MAX)
	{
		m_pMessageBox->warning(
			nullptr, 
			STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD),
			STRING_MANAGER->getString(STR_DELETE_FIRST));
		return false;
	}

	if (!m_pActionManager->action_MaskList_addROIFile(filepath))
	{
		return false;
	}

	return true;
}

bool WindowManagerFileLoader::loadNII(const QString& mPath)
{
	m_pActionManager->action_FileWork_Import_NII(mPath);

	return true;
}

bool WindowManagerFileLoader::loadPRD(const QString& mPath)
{
	if (!m_pWinManager->IsLicensePass())
	{
		m_pMessageBox->warning(nullptr, ("Not Supported"), ("Supported features in current version.\n(Edu version or later is supported.)"));
		if (m_pActionManager->GetAfterThread() == THREAD_IMPORT_FILES)
		{
			loadNextFiles();
		}
		return false;
	}

	QString filepath;
	QString filename;

	QFileInfo file(mPath);

	filepath = file.absolutePath();
	filename = file.fileName();

	if (filepath != STRING_MANAGER->presetFilePath)
	{
		QFile prd(mPath);
		QString copyName = STRING_MANAGER->presetFilePath + "/" + filename;
		if (!prd.copy(copyName))
		{
			int res = m_pMessageBox->warning(m_mainWindow, QString("custom preset name rule"),
				QString("It is the same as the name of the unloaded file. Are you sure you want to overwrite that file?"),
				QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel);
			if (res != QMessageBox::Ok)
			{
				if (m_pActionManager->GetAfterThread() == THREAD_IMPORT_FILES)
				{
					loadNextFiles();
				}
				return false;
			}
			prd.remove(copyName);
			prd.copy(copyName);
		}
		filename = filename.section('.', 0, 0);
	}
	else
	{
		filename = filename.section('.', 0, 0);

		if (m_pWinManager->m_presetNameList.contains(filename))
		{
			m_pMessageBox->warning(
				m_mainWindow, 
				QString("custom preset name rule"),
				QString("The file has already been loaded."));

			if (m_pActionManager->GetAfterThread() == THREAD_IMPORT_FILES)
			{
				loadNextFiles();
			}

			return true;
		}
	}

	m_pWinManager->setPresetName(filename, m_pWinManager->getPresetCount(), true);

	if (m_pActionManager->GetAfterThread() == THREAD_IMPORT_FILES)
	{
		loadNextFiles();
	}

	return true;
}

bool WindowManagerFileLoader::loadTXT(const QString& filepath)
{
	if (!m_pDataContext->volume_data.isValidate())
	{
		m_pMessageBox->warning(
			nullptr, 
			STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD),
			STRING_MANAGER->getString(STR_LOAD_FIRST));
		return false;
	}

	if (m_pDataContext->volume_data.getMaskInfoListCnt() >= MASK_MAX)
	{
		m_pMessageBox->warning(
			nullptr, 
			STRING_MANAGER->getString(STR_FAILED_TO) + STRING_MANAGER->getString(STR_LOAD),
			STRING_MANAGER->getString(STR_DELETE_FIRST));
		return false;
	}

	if (!m_pActionManager->action_MaskList_addTXTFile(filepath))
	{
		return false;
	}

	return true;
}


bool WindowManagerFileLoader::loadSTL(const QString& filepath)
{
	mip::MeshTopology* mesh = new mip::MeshTopology(m_pRenderer);
	if (!mip::model::LoadSTLFile(filepath.toLocal8Bit().constData(), mesh, 0.1f))
	{
		SAFE_DELETE(mesh);

		//setRenderable(true);
		return false;
	}

	QFileInfo f(filepath);

	QString tempName = f.fileName();
	tempName.chop(4);

	{
		// create mesh info
		m_pDataContext->m_MeshData.CreateMeshInfo();

		muint32 uid = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		m_pDataContext->m_MeshData.SetMeshName(tempName, uid);

		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(uid);

		if (info)
		{
			info->upScale = false;
			info->uid = uid;
		}

		// push pck true 
		//vt_pckID.push_back(true);
		m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(uid, true);

		// insert mesh
		m_pDataContext->m_MeshData.InsertMesh(uid, mesh);

		m_pDataContext->m_MeshData.MeshRenderUpdate(uid);
	}

	if (m_pActionManager->GetAfterThread() == THREAD_IMPORT_FILES)
	{
		loadNextFiles();
	}

	return true;
}

bool WindowManagerFileLoader::load3MF(const QString& filepath)
{
	mip::lib3mf::C3MFLoader _3mf;
	std::vector<mip::MeshTopology*> meshList;
	_3mf.Load3mfFile(filepath.toUtf8().constData(), meshList, 0.1f);

	for (int ii = 0; ii > meshList.size(); ii++)
	{
		mip::MeshTopology* mesh = meshList[ii];
		if (mesh->m_tverts.size() < 1)
		{
			SAFE_DELETE(mesh);
			return false;
		}
	}

	QFileInfo f(filepath);
	QString tempName = f.fileName();
	tempName.chop(4);

	for (int ii = 0; ii < meshList.size(); ii++)
	{
		mip::MeshTopology* mesh = meshList[ii];

		// create mesh info
		m_pDataContext->m_MeshData.CreateMeshInfo();

		muint32 uid = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		m_pDataContext->m_MeshData.SetMeshName(tempName, uid);

		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(uid);

		if (info)
		{
			info->upScale = false;
			info->uid = uid;
		}

		m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(uid, true);

		mesh->buildTopologyHEdge();
		mesh->buildTree();
		mesh->updateVertex();
		//
		MeshInfo* mInfo = m_pDataContext->m_MeshData.GetMeshInfo(uid);
		mip::VECTOR4 color(mip::VECTOR4(mInfo->color.r / 255.0f, mInfo->color.g / 255.0f, mInfo->color.b / 255.0f, mesh->getAlphaVal() / 255.0f));
		mesh->updateColor(color);
		mesh->setTopologyed(true);

		m_pDataContext->m_MeshData.InsertMesh(uid, mesh);
		m_pDataContext->m_MeshData.MeshRenderUpdate(uid);
	}

	if (m_pActionManager->GetAfterThread() == THREAD_IMPORT_FILES)
	{
		loadNextFiles();
	}

	return true;
}

bool WindowManagerFileLoader::loadImage(const QString& path)
{
	if (m_pWinManager->imgTabList)
	{
		ImageListTab* tab = m_pWinManager->imgTabList->getImgTab();

		QImage img;
		if (false == img.load(path))
		{
			if (m_pActionManager->GetAfterThread() == THREAD_IMPORT_FILES)
			{
				loadNextFiles();
			}

			return false;
		}

		img.convertToFormat(QImage::Format::Format_RGBA8888);

		if (tab)
		{
			m_pActionManager->action_ImageList_Import(img);
			m_mainWindow->showImageManagementTab();
			int tabIndex = m_pWinManager->imgTabList->getImgTab()->getTabIndex(STRING_MANAGER->getString(STR_IMAGE_IMPORT_TAB));
			if (-1 == tabIndex)
			{
				if (m_pActionManager->GetAfterThread() == THREAD_IMPORT_FILES)
				{
					loadNextFiles();
				}
				return false;
			}
			m_pWinManager->imgTabList->getImgTab()->changeTab(tabIndex);
		}
	}

	if (m_pActionManager->GetAfterThread() == THREAD_IMPORT_FILES)
	{
		loadNextFiles();
	}

	return true;
}

bool WindowManagerFileLoader::loadOBJ(const QString& filepath)
{
	mip::MeshTopology* mesh = new mip::MeshTopology(m_pRenderer);
	if (!mip::model::LoadObjFile(filepath.toLocal8Bit().constData(), mesh, 0.1f))
	{
		SAFE_DELETE(mesh);
		m_pWinManager->setRenderable(true);
		return false;
	}

	QFileInfo f(filepath);

	QString tempName = f.fileName();
	tempName.chop(4);
	{
		// create mesh info
		m_pDataContext->m_MeshData.CreateMeshInfo();

		muint32 uid = m_pDataContext->m_MeshData.GetCurrentMeshIndex();

		m_pDataContext->m_MeshData.SetMeshName(tempName, uid);

		MeshInfo* info = m_pDataContext->m_MeshData.GetMeshInfo(uid);

		if (info)
		{
			info->upScale = false;

			info->uid = uid;
		}

		// push pck true 
		//vt_pckID.push_back(true);
		m_pDataContext->m_MeshData.SetMeshInfoModeSelectMode(uid, true);

		// insert mesh
		m_pDataContext->m_MeshData.InsertMesh(uid, mesh);

		m_pDataContext->m_MeshData.MeshRenderUpdate(uid);
	}

	if (m_pActionManager->GetAfterThread() == THREAD_IMPORT_FILES)
	{
		loadNextFiles();
	}

	return true;
}

bool WindowManagerFileLoader::loadVTK(const QString& path)
{
	QStringList list;
	list.push_back(path);
	m_pActionManager->action_FileWork_Import_VTK(m_pDataContext, list);

	return true;
}

bool WindowManagerFileLoader::loadUSD(const QString& path)
{
	m_pActionManager->action_FileWork_Import_USD(m_pDataContext, path);

	return true;
}

bool WindowManagerFileLoader::loadDCM(
	const QString& filepath,
	std::vector<SliceInfo>& fileslice,
	std::wstring& dir_path,
	DicomVolumeInfo& dcm_volume_info,
	DCM_MODAL_TYPE modality,
	bool multiOpen)
{
	QString openFilePath = filepath;
	DcmtkSeriesInfo info;
	openFilePath.replace('/', "\\");

	bool bSeriesInfo = false;
	bool bFileList = false;

	std::vector<DcmtkSeriesInfo> seriesInfoList;
	sReportOtherInfo* pReportOtherInfo = m_pActionManager->ReportOtherInfoSafeCreate();
	if (multiOpen)
	{
		bSeriesInfo = mip::DcmtkVolumeReader::LoadSeriesInfoList(openFilePath.toStdWString(), seriesInfoList);
		muint32 selIndex = 0;
		if (seriesInfoList.size() > 1)
		{
			DicomSeriesSelectDialog dlg(seriesInfoList);
			if ((!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) ||
				!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2)) 
				&& m_pActionManager->m_IsMacroMode)
			{
#if 0			// jhc [2021.03.29] - DeepCatch이고 macro모드일때 dialog 생략하고 image수가 많은 series 자동 선택.
				int series_num = seriesInfoList.size();
				int temp_slice_num = 0;
				for (int i = 0; i < series_num; i++)
				{

					if (temp_slice_num < std::stoi(seriesInfoList[i].numImages_))
					{
						temp_slice_num = std::stoi(seriesInfoList[i].numImages_);
						selIndex = i;
					}
				}
#else			// jhc [2022.03.14] - DeepCatch이고 macro모드일때 시리즈 선택 알고리즘 적용.
				// [필수단어]가 contain하고 [필수제외단어]가 none contain하면 해당 시리즈로 선택된다.
				// 둘다 포함되면 선택되지 않는다.
				// 선택할 항목이 2개 이상인 경우 Slice가 많은걸로 선택된다.
				// 선택할 항목이 없으면 기존대로 Slice가 많은걸로 선택된다.
				QStringList essentialWordList{ "Pre", "Ax", "Spin", "non", "Br40", "Abd", "Supin", "Prone", "HVP", "remove", "CR", "L-sp", "Stone" };
				QStringList noEssentialWordList{ "cor", "sag", "Scout", "BMD", "delay", "min", "vol" };
				QString curSeriesDescription = "";
				QString curWord = "";
				bool bIncludeEssentialWord = false;
				bool bIncludeNoEssentialWord = false;
				QVector<sSelectedSeriesInfo> vecSelectedSeriesIndex;
				int series_num = seriesInfoList.size();
				for (int i = 0; i < series_num; i++)
				{
					bIncludeEssentialWord = false;
					bIncludeNoEssentialWord = false;
					curSeriesDescription = QString::fromStdString(seriesInfoList[i].description_);
					foreach(curWord, essentialWordList)
					{
						if (curSeriesDescription.contains(curWord, Qt::CaseInsensitive))
						{
							bIncludeEssentialWord = true;
							break;
						}
					}
					if (bIncludeEssentialWord)
					{
						curWord = "";
						foreach(curWord, noEssentialWordList)
						{
							if (curSeriesDescription.contains(curWord, Qt::CaseInsensitive))
								bIncludeNoEssentialWord = true;
						}
						if (!bIncludeNoEssentialWord)
						{
							// essention word에 포함되고 no essential word에는 포함되지 않은 경우 해당 시리즈는 선택된다.
							sSelectedSeriesInfo info;
							info.strSeriesDescription = curSeriesDescription;
							info.nSelectedSeriesIndex = i;
							info.nNumberOfImages = std::stoi(seriesInfoList[i].numImages_);
							vecSelectedSeriesIndex.push_back(info);
						}
					}
				}
				// 선택된 시리즈가 없는 경우 기존대로 image수가 젤 많은 series를 선택
				if (vecSelectedSeriesIndex.isEmpty())
				{
					int series_num = seriesInfoList.size();
					int temp_slice_num = 0;
					for (int i = 0; i < series_num; i++)
					{

						if (temp_slice_num < std::stoi(seriesInfoList[i].numImages_))
						{
							temp_slice_num = std::stoi(seriesInfoList[i].numImages_);
							selIndex = i;
							//	pReportOtherInfo->strSelectedSeriesDescription = QString::fromStdString(seriesInfoList[i].description_);
						}
					}
				}
				// 시리즈 선택 알고리즘에 의해 선택된 시리즈가 하나라도 존재하면
				// 선택된 시리즈 중에서 image수가 젤 많은 series를 선택
				else
				{
					int temp_slice_num = 0;
					sSelectedSeriesInfo curInfo;
					foreach(curInfo, vecSelectedSeriesIndex)
					{
						if (temp_slice_num < curInfo.nNumberOfImages)
						{
							temp_slice_num = curInfo.nNumberOfImages;
							selIndex = curInfo.nSelectedSeriesIndex;
							//	pReportOtherInfo->strSelectedSeriesDescription = curInfo.strSeriesDescription;
						}
					}
				}
#endif
			}
			else
			{
				if (QDialog::Rejected == dlg.exec())
				{
					return false;
				}
				if (dlg.selected_Index < 0)
				{
					m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_0004));
					return false;
				}
				selIndex = dlg.selected_Index;
				//	pReportOtherInfo->strSelectedSeriesDescription = QString::fromStdString(seriesInfoList[selIndex].description_);
			}
		}
		else if (seriesInfoList.size() <= 0)
		{
			// jhc [2022.01.05] - DeepCatch이고 macro모드인데 DICOM Load fail인 경우 다이얼로그 팝업 생략하고 해당 오류 기록 처리.
			if ((!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) ||
				!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2)) && m_pActionManager->m_IsMacroMode)
			{
				m_pActionManager->MacroErrorRecord(m_pActionManager->m_ListMacroCommandLine.first(), STRING_MANAGER->getString(ERR_DU_0004));
			}
			else
			{
				m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_0004));
			}
			return false;
		}

		info = seriesInfoList[selIndex];
		if (bSeriesInfo)
		{
			pReportOtherInfo->strSelectedSeriesDescription = QString::fromStdString(info.description_);
		}
	}
	else
	{
		bSeriesInfo = mip::DcmtkVolumeReader::LoadSeriesInfo(openFilePath.toStdWString(), info);
		if (bSeriesInfo)
		{
			pReportOtherInfo->strSelectedSeriesDescription = QString::fromStdString(info.description_);
		}
	}

	///////////////////////////////

	std::vector<DcmtkSeriesInfo> seriesInfoAcquList;
	bool bAcuiInfo = false;
	if (seriesInfoList.size() > 1)
	{
		bAcuiInfo = mip::DcmtkVolumeReader::LoadAcquisionInfoList(openFilePath.toStdWString(), info, seriesInfoAcquList);
	}

	if (bAcuiInfo)
	{
		DicomSeriesSelectDialog dlg(seriesInfoAcquList, DISPLAY_MODE::DM_ACQUISITIONNUM);
		int nSelectIdx = -1;

		if (seriesInfoAcquList.size() > 1)
		{
			// jhc [2021.03.29] - DeepCatch이고 macro모드일때 dialog 생략하고 image수가 많은 series 자동 선택.
			if ((!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) ||
				!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2)) && m_pActionManager->m_IsMacroMode)
			{
				int temp_slice_num = 0;
				for (int i = 0; i < seriesInfoAcquList.size(); i++)
				{

					if (temp_slice_num < std::stoi(seriesInfoAcquList[i].numImages_))
					{
						temp_slice_num = std::stoi(seriesInfoAcquList[i].numImages_);
						nSelectIdx = i;
					}
				}
			}
			else
			{
				if (QDialog::Rejected == dlg.exec())
				{
					return false;
				}

				if (dlg.selected_Index < 0)
				{
					m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_0004));
					return false;
				}

				nSelectIdx = dlg.selected_Index;
			}
			info = seriesInfoAcquList[nSelectIdx];
		}
	}

	////////////////////////////////
	if (bSeriesInfo)
	{
		int nAcquNum = info.strAcquisitionNum.size() > 0 && seriesInfoList.size() > 1 ? stoi(info.strAcquisitionNum) : -1;
		bFileList = mip::DcmtkVolumeReader::LoadSortedDicomFilesList(openFilePath.toStdWString(), info, nAcquNum, fileslice, dir_path, dcm_volume_info);

		bool init = dcm_volume_info.xAxis == mip::VECTOR3().Zero;

		m_pDataContext->volume_data.setImgOrientation(true, dcm_volume_info.xAxis, init);

		init = dcm_volume_info.yAxis == mip::VECTOR3().Zero;

		m_pDataContext->volume_data.setImgOrientation(false, dcm_volume_info.yAxis, init);
	}

	//false 처리
	if (bSeriesInfo == false)
	{
		m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_0002));
		return false;
	}
	if (bFileList == false)
	{
		// jhc [2022.01.05] - DeepCatch이고 macro모드인데 DICOM Load fail인 경우 다이얼로그 팝업 생략하고 해당 오류 기록 처리.
		if ((!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) ||
			!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2)) && m_pActionManager->m_IsMacroMode)
		{
			m_pActionManager->MacroErrorRecord(m_pActionManager->m_ListMacroCommandLine.first(), "Failed to get file list");
		}
		else
		{
			m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), "Failed to get file list");
		}
		return false;
	}

	if (m_pWinManager->IsSecondaryImage(&info)) //DICOM 파일이 Secondary Capture Image인지 확인
	{
		if ((!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH) ||
			!(m_pProductManager->GetProductInfo_StdString(PRODUCT_NAME_KEY)).compare(PRODUCT_NAME_DEEPCATCH_V2)) && m_pActionManager->m_IsMacroMode)
		{
			m_pActionManager->MacroErrorRecord(m_pActionManager->m_ListMacroCommandLine.first(), "This is an unsupported DICOM file");
		}
		else
		{
			m_pMessageBox->warning(nullptr, "Warning", "This is an unsupported DICOM file");
		}
		return false;
	}

	if (modality == DCM_MODAL_TYPE::DCM_PT)
	{
		m_pWinManager->GetDicomInfo_PET(&info);
	}
	else
	{
		m_pWinManager->GetDicomInfo(&info);
	}

	return true;
}

DICOM_RESULT WindowManagerFileLoader::loadEtcFile(const QString& filename)
{
	if (m_pDataContext->volume_data_PET.isValidate())
	{
		m_pDataContext->volume_data_PET.clear();
	}
	DicomVolumeInfo dcmVolumeInfo;
	return loadFileToVolumeData_CT(&m_pDataContext->volume_data, &dcmVolumeInfo, filename, true);
}

DICOM_RESULT WindowManagerFileLoader::loadPETFile(const QString& filename)
{
	DicomVolumeInfo dcmVolumeInfo;
	VOLUME_DATA tempVolumeData;
	DICOM_RESULT ret = loadFileToVolumeData_PET(&tempVolumeData, &dcmVolumeInfo, filename);

	DcmtkSeriesInfo* pDcmInfo_PET = m_pWinManager->GetDicomInfo_PET();
	DcmtkSeriesInfo* pDcmInfo_CT = m_pWinManager->GetDicomInfo();

	if (dcmVolumeInfo.modality_mode != DCM_MODAL_TYPE::DCM_PT)
	{
		m_pMessageBox->warning(nullptr, "ERROR", "invalid modality data('NOT' PET)");
		tempVolumeData.clearOnly3DHU();
		return DR_ERROR;
	}

	if (pDcmInfo_PET->strStudyUID != pDcmInfo_CT->strStudyUID)
	{
		QString text = QString("PET study UID(=%1) is not equal to CT study UID(=%2)").
			arg(pDcmInfo_PET->strStudyUID.c_str()).
			arg(pDcmInfo_CT->strStudyUID.c_str());
		m_pMessageBox->warning(nullptr, "ERROR", text);
		tempVolumeData.clearOnly3DHU();
		return DR_ERROR;
	}

	double philpseFactor = 0.0;
	std::string radiopharmaceutical = "";
	float bodyWeight = atof(pDcmInfo_PET->patientWeight_.c_str());
	float radionuclideHalfLife = atof(pDcmInfo_PET->radionuclideHalfLife.c_str());
	float radionuclideTotalDose = atof(pDcmInfo_PET->radionuclideTotalDose_.c_str());

	PETVolumeInfo petVolumeInfo;
	std::string errMsg;
	if (petVolumeInfo.Initialize(
		pDcmInfo_PET->units_,
		philpseFactor,
		bodyWeight,
		radiopharmaceutical,
		radionuclideHalfLife,
		radionuclideTotalDose,
		pDcmInfo_PET->acquisitionTime_,
		pDcmInfo_PET->radiopharmaceuticalStartTime_,
		dcmVolumeInfo.rescaleSlose,
		dcmVolumeInfo.rescaleIntercept,
		errMsg) == false)
	{
		m_pMessageBox->warning(nullptr, "ERROR", ("PET DCM initialize ERROR. " + errMsg).c_str());
		tempVolumeData.clearOnly3DHU();
		return DR_ERROR;
	}

	mip::VECTOR3 imgPos = tempVolumeData.getImgPos();
	mip::VECTOR3 pixelSpacing(
		tempVolumeData.getSpaceX(true),
		tempVolumeData.getSpaceY(true),
		tempVolumeData.getSpaceZ(true)
	);
	if (m_pDataContext->volume_data_PET.createData(&tempVolumeData,
		&petVolumeInfo,
		imgPos,
		pixelSpacing) == false)
	{
		tempVolumeData.clearOnly3DHU();
		return DR_ERROR;
	}

	/* PET Volume XY 평면 크기(mm)와 CT Volume XY 평면 크기(mm)가 동일해야한다.
	   따라서 PET Volume XY 평면 크기를 Cropping 한다.*/
	if (
		m_pDataContext->volume_data.isValidate() &&
		m_pDataContext->volume_data_PET.isValidate()
		)
	{
		//TODO : XY평면 크기가 CT > PET일 경우 예외처리 한다.
		float PETWidth_mm = m_pDataContext->volume_data_PET.getCX_mm();
		float PETHeight_mm = m_pDataContext->volume_data_PET.getCY_mm();
		float PETDepth_mm = m_pDataContext->volume_data_PET.getCZ_mm();
		float CTWidth_mm = m_pDataContext->volume_data.getCX() * m_pDataContext->volume_data.getSpaceX(true);
		float CTHeight_mm = m_pDataContext->volume_data.getCY() * m_pDataContext->volume_data.getSpaceY(true);

		/*
		<PET cropping 영역 계산>
		- startX = (PET width - CT width) / 2 (mm)
		- startY = (PET width - CT width) / 2 (mm)
		- startZ = 0;
		- width = CT width(mm)
		- height = CT height(mm)
		- depth = PET height(mm)
		*/
		float startX_mm = (PETWidth_mm - CTWidth_mm) / 2.;
		float startY_mm = (PETHeight_mm - CTHeight_mm) / 2.;
		float startZ_mm = 0;
		float width_mm = CTWidth_mm;
		float height_mm = CTHeight_mm;
		float depth_mm = PETDepth_mm;

		m_pDataContext->volume_data_PET.cropRegion_mm(
			startX_mm,
			startY_mm,
			startZ_mm,
			width_mm,
			height_mm,
			depth_mm
		);
	}

	tempVolumeData.clearOnly3DHU();
	return DR_NORMAL;
}

DICOM_RESULT WindowManagerFileLoader::loadFileToVolumeData_CT(VOLUME_DATA* pVolueData, DicomVolumeInfo* pDCMVolumeInfo, const QString& filename, bool clearScreen)
{
	std::vector<SliceInfo> fileSlices;
	std::wstring dir_path;

	bool bLoadDCM = false;
	bool bCreateMemory = false;
	bool bLoadVolume = false;
	bLoadDCM = loadDCM(filename, fileSlices, dir_path, *pDCMVolumeInfo, DCM_MODAL_TYPE::DCM_CT, true);
	if (bLoadDCM == false)
	{
		mip::DcmtkVolumeReader::UnloadDicomFiles();
		return DR_ERROR;
	}

	if (clearScreen)
	{
		m_pWinManager->setRenderable(false);
	}

	m_pActionManager->action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_DICOM_FILE));
	m_pActionManager->action_ProgressUpdate(0);
	m_pActionManager->clear();
	if (bLoadDCM)
	{
		bCreateMemory = pVolueData->createData(
			pDCMVolumeInfo->dx, pDCMVolumeInfo->dy, pDCMVolumeInfo->dz,
			pDCMVolumeInfo->x_spacing * 0.1f, pDCMVolumeInfo->y_spacing * 0.1f, pDCMVolumeInfo->z_spacing * 0.1f);

		DcmtkSeriesInfo* pDcmInfo = m_pWinManager->GetDicomInfo();

		int len = pDcmInfo->imagePosition.length();
		if (len > 1)
		{
			QString tmpStr = pDcmInfo->imagePosition.c_str();
			QStringList tmpList = tmpStr.split("/");

			if (tmpList.count() == 3)
			{
				float pos[3];
				for (int i = 0; i < 3; i++)
				{
					pos[i] = tmpList.at(i).toFloat();
				}

				pVolueData->setImgPos(pos[0], pos[1], pos[2]);
			}
		}
	}

	// action_ImageIsotropic_Modification
	int nMaxProgress = 100;
	bool bIsotropic = false;
	float fThresholdSpacing = 0.95f;

	if ((!LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH) || !LICENSE_DATA->getProductName().compare(PRODUCT_NAME_DEEPCATCH_V2))
		&& pVolueData->getSpaceX(true) < fThresholdSpacing
		&& pVolueData->getSpaceY(true) < fThresholdSpacing)
	{
		bIsotropic = true;
	}

	if (bIsotropic)
	{
		nMaxProgress = 50;
	}

	//pDCMVolumeInfo->bytesPerVoxel = 2;
	if (bCreateMemory)
	{
		if (fileSlices.size() > 0)//&& pDCMVolumeInfo->bytesPerVoxel == 2)
		{
			float scale = 0.1f;
			unsigned int sampleRate = 1;
			bLoadVolume = mip::DcmtkVolumeReader::LoadVolumeData(
				fileSlices,
				dir_path,
				*pDCMVolumeInfo,
				DCM_VOLUME_DATA_TYPE_NORMAL,
				scale,
				sampleRate,
				pVolueData->getHUDataPoint(),	//OUT 
				nullptr,
				m_pActionManager->action_ProgressDlgGet(),
				nMaxProgress);
		}
		else
		{
			bLoadVolume = false;
		}
	}

	if (bLoadVolume)
	{
		m_pWinManager->initMedipUIData(pDCMVolumeInfo->window_width, pDCMVolumeInfo->window_level);
	}

	qDebug() << __FUNCTION__ << "L." << __LINE__ << " LoadVolume:" << bLoadVolume << endl;
#ifdef _M_IX86
	setRenderType(RT_SURFACE);
	//m_pRenderer->setAvailableVolumeRender(false);
#endif // DEBUG

	//false 처리
	if (bCreateMemory == false)
	{
		m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0003));
		bool result = m_pWinManager->setVolumeDataToCropWindow(dir_path, fileSlices, *pDCMVolumeInfo);
		if (result == false)
		{
			m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0004));
		}
		return DR_ERROR;
	}

	mip::DcmtkVolumeReader::UnloadDicomFiles();
	if (bLoadVolume == false)
	{
		pVolueData->clear();
		m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_0003));
		return DR_ERROR;
	}

#ifdef _M_IX86
	if (bLoadVolume)
	{
		renderVolumeVTKData();
	}
#endif

	if (bIsotropic)
	{
		return DR_LOW_SPACING;
	}

	return DR_NORMAL;
}

DICOM_RESULT WindowManagerFileLoader::loadFileToVolumeData_PET(VOLUME_DATA* pVolumeData, DicomVolumeInfo* pDCMVolumeInfo, const QString& filename)
{
	std::vector<SliceInfo> fileSlices;
	std::wstring dir_path;

	bool bLoadVolume = false;
	bool bLoadDCM = loadDCM(filename, fileSlices, dir_path, *pDCMVolumeInfo, DCM_MODAL_TYPE::DCM_PT, true);
	if (bLoadDCM == false)
	{
		mip::DcmtkVolumeReader::UnloadDicomFiles();
		return DR_ERROR;
	}

	m_pActionManager->action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_DICOM_FILE));
	m_pActionManager->action_ProgressUpdate(0);
	m_pActionManager->clear();

	//HU 관련 Volume 데이터만 생성
	bool bCreateMemory = pVolumeData->create3DHUData(
		pDCMVolumeInfo->dx, pDCMVolumeInfo->dy, pDCMVolumeInfo->dz,
		pDCMVolumeInfo->x_spacing * 0.1f,
		pDCMVolumeInfo->y_spacing * 0.1f,
		pDCMVolumeInfo->z_spacing * 0.1f);

	if (bCreateMemory == false)
	{
		m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0003));
		bool result = m_pWinManager->setVolumeDataToCropWindow(dir_path, fileSlices, *pDCMVolumeInfo);
		if (result == false)
		{
			m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0004));
		}
		return DR_ERROR;
	}

	DcmtkSeriesInfo* pDcmInfo = m_pWinManager->GetDicomInfo_PET();

	int len = pDcmInfo->imagePosition.length();
	if (len > 1)
	{
		QString tmpStr = pDcmInfo->imagePosition.c_str();
		QStringList tmpList = tmpStr.split("/");

		if (tmpList.count() == 3)
		{
			float pos[3];
			for (int i = 0; i < 3; i++)
			{
				pos[i] = tmpList.at(i).toFloat();
			}

			pVolumeData->setImgPos(pos[0], pos[1], pos[2]);
		}
	}

	int nMaxProgress = 100;
	if (bCreateMemory)
	{
		if (fileSlices.size() > 0)
		{
			float scale = 0.1f;
			unsigned int sampleRate = 1;
			bLoadVolume = mip::DcmtkVolumeReader::LoadVolumeData(
				fileSlices,
				dir_path,
				*pDCMVolumeInfo,
				DCM_VOLUME_DATA_TYPE_PET_SUV,
				scale,
				sampleRate,
				pVolumeData->getHUDataPoint(),
				nullptr,
				m_pActionManager->action_ProgressDlgGet(),
				nMaxProgress);
		}
		else
		{
			bLoadVolume = false;
		}
	}

	mip::DcmtkVolumeReader::UnloadDicomFiles();
	if (bLoadVolume == false)
	{
		pVolumeData->clear();
		m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_0003));
		return DR_ERROR;
	}

	return DR_NORMAL;
}

bool WindowManagerFileLoader::loadCroppingDicomFile(QString& filename)
{
	std::vector<SliceInfo> fileslice;
	std::wstring dir_path;
	DicomVolumeInfo dcm_volume_info;

	if (loadDCM(filename, fileslice, dir_path, dcm_volume_info, DCM_MODAL_TYPE::DCM_CT, true))
	{
		bool result = m_pWinManager->setVolumeDataToCropWindow(dir_path, fileslice, dcm_volume_info);
		if (result == false)
		{
			m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DH_0004));
			return false;
		}
		return true;
	}
	else
	{
		m_pMessageBox->warning(m_mainWindow, STRING_MANAGER->getString(STR_WARN), STRING_MANAGER->getString(ERR_DU_0002));
		return false;
	}
}

bool WindowManagerFileLoader::loadCroppingDicomData(const mint16* pHUdata, const DcmtkSeriesInfo& dcmSeriesInfo, const DicomVolumeInfo& dcmVolumeInfo)
{
	m_pWinManager->setRenderable(false);
	m_pWinManager->setCropOnOff(true);

	bool success = false;
	try
	{
		CroppingDialog dlgCropping(&m_pDataContext->volume_data, m_mainWindow);

		dlgCropping.InitDataWithHU(pHUdata, dcmVolumeInfo);
		int result = dlgCropping.exec();
		if (result == QDialog::Accepted)
		{
			m_pActionManager->action_ProgressBegin(STRING_MANAGER->getString(STR_LOAD_DICOM_FILE));
			m_pActionManager->action_ProgressUpdate(0);
			m_pActionManager->clear();
			if (m_pWinManager->mainSegmentWidget)
			{
				m_pWinManager->mainSegmentWidget->resetResource();
			}
			m_pWinManager->initMedipUIData(dlgCropping.GetWindowingWidth(), dlgCropping.GetWindowingLevel());
			m_pActionManager->action_ProgressEnd();
			success = true;
		}
	}
	catch (...)
	{
		qCritical() << "exception to load cropping dicom data";
	}

	m_pWinManager->setRenderable(true);
	m_pWinManager->setCropOnOff(false);

	return success;
}


void WindowManagerFileLoader::SetMessageBox(MessageBoxBase* pMessageBox)
{
	m_pMessageBox = pMessageBox;
}


