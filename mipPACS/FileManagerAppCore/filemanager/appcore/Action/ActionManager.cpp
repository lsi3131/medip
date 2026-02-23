#include "stdafx.h"
#include "ActionManager.h"
#include "filemanager/data/Entity/EntityDataManager.h"
#include "filemanager/appcore/AppCoreContext.h"
#include "filemanager/appcore/Event/EventManager.h"
#include "ActionPatientListUploadDicomFileList.h"
#include "ActionPatientListEditDicomFileList.h"
#include <qfileinfo>
#include <qdir>
#include <future>

namespace fm
{
	ActionManager::ActionManager() :
		m_pContext(nullptr),
		m_pImportDcmDAO(nullptr),
		m_pLastEditedFileDAO(nullptr)
	{
	}

	ActionManager::~ActionManager()
	{
	}

	bool ActionManager::Initialize(AppCoreContext* pContext)
	{
		m_pContext = pContext;
		m_pImportDcmDAO = m_pContext->GetDataManager()->GetImportedDicomInfoDAO();
		m_pLastEditedFileDAO = m_pContext->GetDataManager()->GetLastEditedFileDAO();

		return true;
	}

	void ActionManager::DirectorySave(const QString& dirpath)
	{
		/* 폴더 저장 */
		if (AddLastEditFileInfo(dirpath) == false)
		{
			return;
		}

		emit g_EventManager.fileSelected(dirpath);
	}

	void ActionManager::FileSave(const QString& dirpath, const QString& fileName)
	{
		/* 폴더 저장 */
		if (AddLastEditFileInfo(dirpath) == false)
		{
			return;
		}

		QString saveFilePath = dirpath + "/" + fileName;
		emit g_EventManager.fileSelected(saveFilePath);
	}

	/*
		파일을 Open시 진행 Process
	*/
	void ActionManager::FileOpen(const QString& filepath)
	{
		FileListOpen({ filepath });
	}

	void ActionManager::FileListOpen(const QStringList& filepathList)
	{
		if (filepathList.isEmpty())
		{
			return;
		}

		/* 첫번째 경로만 추가 */
		if (AddLastEditFileInfo(filepathList[0]) == false)
		{
			return;
		}

		emit g_EventManager.fileListSelected(filepathList);
	}

	/*
		DICOM 신규 추가 시 진행할 프로세스를 정의
	*/
	void ActionManager::ImportDicomFileList(const QStringList& filepathList)
	{
		std::vector<DicomDataset> dcmDatasetList;

		for (auto& filepath : filepathList)
		{
			DicomDataset dcmDataset;
			if (dcmDataset.LoadFromFile(filepath))
			{
				dcmDatasetList.push_back(dcmDataset);
			}
		}
		return ImportDicomDatasetList(dcmDatasetList);
	}

	void ActionManager::ImportDicomDatasetList(std::vector<DicomDataset>& dcmDatasetList)
	{
		if (dcmDatasetList.empty())
		{
			return;
		}

		std::vector< ImportedDicomInfoDTO> inputDTOList_Input;
		std::vector<ImportedDicomInfoDTO> outDTOList_Modified;
		std::vector<ImportedDicomInfoDTO> outDTOList_Added;

		std::shared_ptr<DicomInfomationModel> model = DicomInfomationModel::CreateDefault();
		for (auto& dset : dcmDatasetList)
		{
			model->Add(dset);
		}

		std::vector<DicomInfomationModelStudyObject*> studyList = model->GetStudyList();
		for (DicomInfomationModelStudyObject* pStudy : studyList)
		{
			std::vector<DicomInfomationModelSeriesObject*> seriesList = pStudy->GetSeriesList();
			for (DicomInfomationModelSeriesObject* pSeries : seriesList)
			{
				ImportedDicomInfoDTO dto;
				if (GetImportedDicomInfo(dto, pSeries))
				{
					inputDTOList_Input.push_back(dto);
				}
			}
		}

		AddOrModifyImportedDicomInfoList(inputDTOList_Input, outDTOList_Modified, outDTOList_Added);

		if (outDTOList_Added.empty() == false)
		{
			emit g_EventManager.patientList_dicomListAdded(outDTOList_Added);
		}
		else if (outDTOList_Modified.empty() == false)
		{
			emit g_EventManager.patientList_dicomListModified(outDTOList_Modified);
		}
	}

	void ActionManager::ImportDicomInfoDTOList(std::vector<ImportedDicomInfoDTO>& importedDicomInfoList)
	{
		//std::vector

		std::vector<ImportedDicomInfoDTO> outDTOList_Modified;
		std::vector<ImportedDicomInfoDTO> outDTOList_Added;

		AddOrModifyImportedDicomInfoList(importedDicomInfoList, outDTOList_Modified, outDTOList_Added);

		if (outDTOList_Added.empty() == false)
		{
			emit g_EventManager.patientList_dicomListAdded(outDTOList_Added);
		}
		else if (outDTOList_Modified.empty() == false)
		{
			emit g_EventManager.patientList_dicomListModified(outDTOList_Modified);
		}
	}

	void ActionManager::CloseApp()
	{
		emit g_EventManager.appClosed();
	}

	void ActionManager::DicomSCPServerStart()
	{
		m_pContext->GetDicomNetworkManager()->ReleaseStoreSCP();

		if (m_pContext->GetDicomNetworkManager()->IsStoreSCPInitialized() == false)
		{
			EDicomNetworkResult result = m_pContext->GetDicomNetworkManager()->InitializeStoreSCP();
			if (result != EDicomNetworkResult::SUCCESS)
			{
				qWarning() << "StartListener. fail to start store SCP ";
				emit g_EventManager.dcmNet_SCPServerStartFailed();
			}
			else
			{
				qInfo() << "StartListener. success to start store SCP ";
				m_pContext->GetDicomNetworkManager()->StartStoreSCP_Async();
			}
		}
	}

	void ActionManager::PatientList_FileCopy(QString srcDirPath, QString targetDirPath)
	{
		QFuture<void> future = QtConcurrent::run([=]() {
			emit g_EventManager.patientList_fileCopy_Started();

			QDir srcDir(srcDirPath);
			QFileInfoList srcFileInfoList = srcDir.entryInfoList(QDir::Filter::Files);
			int maxCount = srcFileInfoList.count();
			for (int i = 0; i < srcFileInfoList.count(); ++i)
			{
				QString destFilePath = targetDirPath + "/" + srcFileInfoList[i].fileName();
				QFile::copy(srcFileInfoList[i].absoluteFilePath(), destFilePath);
				emit g_EventManager.patientList_fileCopy_InProgressed(i, maxCount, destFilePath);
			}

			emit g_EventManager.patientList_fileCopy_Finished(targetDirPath);
			});
		//return future;
	}

	void ActionManager::PatientList_UploadDicomFileList(QStringList filepathList)
	{
		QFuture<void> future = QtConcurrent::run([=]() {
			ActionPatientListUploadDicomFileList action(m_pContext, &m_dcmDatasetIO, filepathList);
			action.Do();
			});
	}

	void ActionManager::PatientList_EditDicomFileList(QStringList filepathList, const DicomDataset& editDcmDataset)
	{
		QFuture<void> future = QtConcurrent::run([=]() {
			ActionPatientListEditDicomFileList action(m_pContext, &m_dcmDatasetIO, filepathList, editDcmDataset);
			action.Do();
			});
	}

	bool ActionManager::AddLastEditFileInfo(const QString& filepath)
	{
		QFileInfo fileInfo(filepath);
		LastEditedFileDTO recentEditFile;
		recentEditFile.FilePath = filepath.toStdWString();
		if (fileInfo.isFile())
		{
			QString suffix = fileInfo.suffix();

			/*
				TODO : mip, jpg, 기타 등등 파일일 경우 해당하는 파일 처리 기능 추가
			*/
			if (suffix.toLower() == "dcm")
			{
				/* DCM File에서 TagData Load */
				DicomDataset dcmDataset;
				if (dcmDataset.LoadFromFile(filepath) == false)
				{
					qWarning() << "fail to read tag(path=" << filepath << ")";
				}
				else
				{
					DICOM_HEADER_INFO dcmInfo = dcmDataset.ToDcmHeaderInfo();
					recentEditFile.SeriesInstanceUID = dcmInfo.SeriesInstanceUID;
					recentEditFile.StudyInstanceUID = dcmInfo.StudyInstanceUID;
					recentEditFile.PatientName = dcmInfo.PatientName;
					recentEditFile.DateOfBirth = StringUtil::MultiByteToWide(dcmInfo.DateOfBirth.ToFormatText_Date());
					recentEditFile.PatientID = dcmInfo.PatientID;
					recentEditFile.Modality = dcmInfo.Modality;
					recentEditFile.StudyDescription = dcmInfo.StudyDescription;
					recentEditFile.SeriesDescription = dcmInfo.SeriesDescription;
					recentEditFile.AccessionNumber = dcmInfo.AccessionNumber;
					recentEditFile.ExamID = dcmInfo.ExamID;
					recentEditFile.ReferringPhysician = dcmInfo.ReferringPhysician;
					recentEditFile.PerformingPhysician = dcmInfo.PerformingPhysician;
					recentEditFile.ReadingPhysician = dcmInfo.ReadingPhysician;
				}
			}
			//		/*
			//			<21.04.29 김영덕 차장님 Requirement >
			//			DICOM 정보를 환자 List 항목에 추가한다. 현재 사용되지 않음
			//		*/
			//		//AddOrModifyImportDcmInfo(filepath, dcmInfo);
			//	}
			//}
		}
		else if (fileInfo.isDir())
		{
			//Skip
		}
		else
		{
			qWarning() << "invalid file info type(not file or directory)";
			return false;
		}


		/* DICOM 정보를 Database의 Last Edited File Entity에 추가한다. */
		LastEditedFileDAO* pLastEditedFileDAO = m_pContext->GetDataManager()->GetLastEditedFileDAO();
		pLastEditedFileDAO->AddOrModify(recentEditFile);

		return true;
	}

	bool ActionManager::GetImportedDicomInfo(ImportedDicomInfoDTO& dto, DicomInfomationModelSeriesObject* pSeries)
	{
		DicomDataset* pDataset;
		if (pSeries->GetFirst(&pDataset) == false)
		{
			return false;
		}

		dto.InitFromDcmDataset(*pDataset);

		for (DicomDataset* pDSet : pSeries->GetDatasetList())
		{
			QFileInfo fileInfo(QString::fromStdWString(pDSet->GetValue_Extension(DCM_EXT_FILE_PATH)));
			dto.DicomFileList.push_back(fileInfo.fileName().toStdWString());
		}

		return true;
	}

	void ActionManager::AddOrModifyImportedDicomInfoList(
		const std::vector< ImportedDicomInfoDTO>& inputDTOList_Input,
		std::vector< ImportedDicomInfoDTO>& outDTOList_Modified,
		std::vector< ImportedDicomInfoDTO>& outDTOList_Added)
	{
		for (auto& dto : inputDTOList_Input)
		{
			ImportedDicomInfoDTO dtoFind;

			if (m_pImportDcmDAO->FindByStudySeriesInstanceUID(dtoFind, dto.StudyInstanceUID, dto.SeriesInstanceUID))
			{
				/* Patient, Study, Series 정보 업데이트 진행할 것 */
				if (m_pImportDcmDAO->ModifyPatientStudyInfoByStudyUID(dto.StudyInstanceUID, dto) == false)
				{
					qWarning() << "fail to modify imported patient study info dto data";
				}
				else
				{
					if (m_pImportDcmDAO->ModifySeriesInfoByStudySeriesUID(dto.StudyInstanceUID, dto.SeriesInstanceUID, dto) == false)
					{
						qWarning() << "fail to modify imported series info dto data";
					}
					else
					{
						outDTOList_Modified.push_back(dto);
					}
				}
			}
			else
			{
				if (m_pImportDcmDAO->Add(dto) == false)
				{
					qWarning() << "fail to add imported dicom info dto data";
				}
				else
				{
					outDTOList_Added.push_back(dto);
				}
			}
		}
	}
}
