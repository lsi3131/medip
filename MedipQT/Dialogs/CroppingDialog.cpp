#include "stdafx.h"
#include "CroppingDialog.h"

#include "DicomReader.h"

#include "System\styleManager.h"
#include "System\stringManager.h"
#include "CroppingViewControlWidget.h"
#include "Actions/ActionManager.h"
#include "Renderer/VolumeComputer.h"
#include "Renderer/Renderer.h"
#include "Windows/windowManager.h"

CroppingDialog::CroppingDialog(VOLUME_DATA* pVolumeData, QWidget* parent)
	:QDialog(parent),
	m_pVolumeData(pVolumeData),
	m_cx(0),
	m_cy(0),
	m_cz(0),
	m_pData_HU(nullptr)
{
	setupUi(this);
	setWindowTitle(STRING_MANAGER->getString(STR_CROP_WINDOW_TILE));

	m_axialViewWidget->InitLayout(WT_AXIAL);
	connect(m_axialViewWidget->View(), &CroppingView::sig_BoxUpdate, this, &CroppingDialog::OnChangeMemoryLabel);
	connect(m_axialViewWidget->View(), &CroppingView::sig_angleUpdate, this, &CroppingDialog::OnChangeMemoryLabel);

	m_coronalViewWidget->InitLayout(WT_CORONAL);
	connect(m_coronalViewWidget->View(), &CroppingView::sig_BoxUpdate, this, &CroppingDialog::OnChangeMemoryLabel);
	connect(m_coronalViewWidget->View(), &CroppingView::sig_angleUpdate, this, &CroppingDialog::OnChangeMemoryLabel);

	m_sagittalViewWidget->InitLayout(WT_SAGITTAL);
	connect(m_sagittalViewWidget->View(), &CroppingView::sig_BoxUpdate, this, &CroppingDialog::OnChangeMemoryLabel);
	connect(m_sagittalViewWidget->View(), &CroppingView::sig_angleUpdate, this, &CroppingDialog::OnChangeMemoryLabel);

	m_widgetControlPanel->setStyleSheet("background-color: rgba(82, 82, 82, 255);");

	InitMemoryInfoUI();
	InitLengthInfoUI();
	InitButtonUI();

	m_croppingRangeBox.reset();
}


CroppingDialog::~CroppingDialog()
{
	if (m_pData_HU == nullptr)
	{
		delete m_pData_HU;
		m_pData_HU = nullptr;
	}
}

void CroppingDialog::InitMemoryInfoUI()
{
	m_groupMemoryInfo->setStyleSheet("QGroupBox{"
		"Color: White;"
		"border - color: rgba(48, 48, 48, 255);"
		"}");
	m_totalMemory->setStyleSheet("Color: rgba(248, 248, 248, 255)");

	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);

	m_totalMemory->setText("Total : " + QString::number(status.ullTotalPhys / 1024 / 1024) + "MB");

	m_availablelMemory->setStyleSheet("Color: rgba(248, 248, 248, 255)");
}

void CroppingDialog::InitLengthInfoUI()
{
	m_groupLengthInfo->setStyleSheet("Color: White");

	m_pQLabelxlen->setStyleSheet("Color: rgba(248, 248, 248, 255)");
	m_pQLabelylen->setStyleSheet("Color: rgba(248, 248, 248, 255)");
	m_pQLabelzlen->setStyleSheet("Color: rgba(248, 248, 248, 255)");
}

void CroppingDialog::InitButtonUI()
{
	m_btnOpen->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(m_btnOpen, &QPushButton::clicked, this, &CroppingDialog::OnCompleteCropping);

	//QPushButton* m_btnReset = new QPushButton("Reset", pWidget);
	//m_btnReset->setFixedSize(76, 30);
	m_btnReset->setStyleSheet(STYLE_MANAGER->buttonBehind);
	connect(m_btnReset, &QPushButton::clicked, this, &CroppingDialog::OnReset);
}

void CroppingDialog::adjustBoundingBoxSize(WINDOW_TYPE eWinType)
{
	//createVolumeTextureGL 함수를 위한 인자 조정(홀수 불가능, XLen은 4의 배수)

	BoundingBoxI boxTotal;

	if (eWinType == WT_AXIAL)
	{
		boxTotal = m_axialViewWidget->View()->getBouningBox();
	}
	else if (eWinType == WT_CORONAL)
	{
		boxTotal = m_coronalViewWidget->View()->getBouningBox();
	}
	else if (eWinType == WT_SAGITTAL)
	{
		boxTotal = m_sagittalViewWidget->View()->getBouningBox();
	}

	// 	if ((boxTotal.maxY - boxTotal.minY + 1) == 0 || (boxTotal.maxX - boxTotal.minX + 1) == 0)
	// 	{
	// 		boxTotal.maxX = m_preBox.maxX;
	// 		boxTotal.minX = m_preBox.minX;
	// 		boxTotal.maxY = m_preBox.maxY;
	// 		boxTotal.minY = m_preBox.minY;
	// 	}
	// 	else if ((boxTotal.maxY - boxTotal.minY + 1) < 40 || (boxTotal.maxX - boxTotal.minX + 1) < 40)
	// 	{
	// 		boxTotal.maxX = m_preBox.maxX;
	// 		boxTotal.minX = m_preBox.minX;
	// 		boxTotal.maxY = m_preBox.maxY;
	// 		boxTotal.minY = m_preBox.minY;
	// 	}

	m_axialViewWidget->View()->setBoundingBox(boxTotal);
	m_coronalViewWidget->View()->setBoundingBox(boxTotal);
	m_sagittalViewWidget->View()->setBoundingBox(boxTotal);

	m_croppingRangeBox = boxTotal;
}

void CroppingDialog::initData(const DicomVolumeInfo& dcmVolumeInfo)
{
	this->setWindowModality(Qt::WindowModality::ApplicationModal);

	m_dcmVolumeInfo = dcmVolumeInfo;

	m_cx = dcmVolumeInfo.dx;
	m_cy = dcmVolumeInfo.dy;
	m_cz = dcmVolumeInfo.dz;

	int length = m_cx * m_cy * m_cz;
	m_pData_HU = new mint16[length];
	int byteOfSize = m_cx * m_cy * m_cz * sizeof(mint16);
	memset(m_pData_HU, 0, byteOfSize);

	m_axialViewWidget->View()->createData(m_cx, m_cy, m_cz, m_dcmVolumeInfo.window_width, m_dcmVolumeInfo.window_level, m_pData_HU);
	m_coronalViewWidget->View()->createData(m_cx, m_cy, m_cz, m_dcmVolumeInfo.window_width, m_dcmVolumeInfo.window_level, m_pData_HU);
	m_sagittalViewWidget->View()->createData(m_cx, m_cy, m_cz, m_dcmVolumeInfo.window_width, m_dcmVolumeInfo.window_level, m_pData_HU);

	m_croppingRangeBox.reset(m_cx, m_cy, m_cz);

	m_axialViewWidget->View()->initZoomFactor(m_dcmVolumeInfo.x_spacing, m_dcmVolumeInfo.y_spacing, false);
	m_coronalViewWidget->View()->initZoomFactor(m_dcmVolumeInfo.x_spacing, m_dcmVolumeInfo.z_spacing, false);
	m_sagittalViewWidget->View()->initZoomFactor(m_dcmVolumeInfo.y_spacing, m_dcmVolumeInfo.z_spacing, false);

	updateInfomationPanel();
}

void CroppingDialog::InitDataWithHU(const mint16* pData_HU, const DicomVolumeInfo& dcmVolumeInfo)
{
	initData(dcmVolumeInfo);

	int byteOfSize = m_cx * m_cy * m_cz * sizeof(mint16);
	memcpy(m_pData_HU, pData_HU, byteOfSize);

	initWidget();
}

void CroppingDialog::initWidget()
{
	m_axialViewWidget->Init();
	m_coronalViewWidget->Init();
	m_sagittalViewWidget->Init();

	renderLater();
}

void CroppingDialog::updateInfomationPanel()
{
	BoundingBoxI box = m_axialViewWidget->View()->getBouningBox();
	int xLen = box.maxX - box.minX + 1;
	int yLen = box.maxY - box.minY + 1;
	int zLen = box.maxZ - box.minZ + 1;

	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);

	int nAvailalbleMemory = status.ullAvailPhys / 1024 / 1024;

	m_availablelMemory->setText("Free : " + QString::number(nAvailalbleMemory) + "MB");

	bool isAllocatableMemory = true;
	if (m_volumeMemory)
	{
		int expectedVolumeMemory = xLen * yLen * zLen * 3 / 1024 / 1024;
		isAllocatableMemory = nAvailalbleMemory > expectedVolumeMemory * 2;
		QString labelStyle = isAllocatableMemory ?
			STYLE_MANAGER->availableVolumeMemory : STYLE_MANAGER->unavailableVolumeMemory;
		m_volumeMemory->setStyleSheet(labelStyle);
		m_volumeMemory->setText("Volume : " + QString::number(xLen * yLen * zLen * 3 / 1024 / 1024) + "MB");
	}
	if (m_pQLabelxlen)
		m_pQLabelxlen->setText("x length : " + QString::number(xLen) + "px");

	if (m_pQLabelylen)
		m_pQLabelylen->setText("y length : " + QString::number(yLen) + "px");

	if (m_pQLabelzlen)
		m_pQLabelzlen->setText("z length : " + QString::number(zLen) + "px");

	if (m_btnOpen)
	{
		if (isAllocatableMemory)
		{
			m_btnOpen->setEnabled(true);
			m_btnOpen->setStyleSheet(STYLE_MANAGER->buttonBehind + QString("Color: White"));
		}
		else
		{
			m_btnOpen->setEnabled(false);
			m_btnOpen->setStyleSheet(STYLE_MANAGER->buttonBehind + QString("Color: Red"));
		}
	}
}

void CroppingDialog::renderLater()
{
	m_axialViewWidget->View()->renderLater();
	m_sagittalViewWidget->View()->renderLater();
	m_coronalViewWidget->View()->renderLater();
}

mint16* CroppingDialog::getHUDataPoint() const
{
	return m_pData_HU;
}

mip::VECTOR3 CroppingDialog::GetAngle() const
{
	return m_angle;
}

BoundingBoxI CroppingDialog::GetCroppingRangeBox() const
{
	return m_croppingRangeBox;
}

DicomVolumeInfo CroppingDialog::GetDicomVolumeInfo() const
{
	return m_dcmVolumeInfo;
}

int CroppingDialog::GetWindowingWidth() const
{
	return m_dcmVolumeInfo.window_width;
}

int CroppingDialog::GetWindowingLevel() const
{
	return m_dcmVolumeInfo.window_level;
}

void CroppingDialog::OnCompleteCropping()
{
	this->setVisible(false);
	this->setWindowModality(Qt::WindowModality::NonModal);

	float fSagittalRot = m_sagittalViewWidget->View()->getRotationAngle();
	float fCoronalRot = m_coronalViewWidget->View()->getRotationAngle();
	float fAxialRot = m_axialViewWidget->View()->getRotationAngle();

	float DtoR = M_PI / 180.0;
	m_angle = mip::VECTOR3(fSagittalRot * DtoR, -fCoronalRot * DtoR, -fAxialRot * DtoR);
	//mip::VECTOR3 angle(2.f, 0.f, 0.f);
	// m_dcm_volume_info 셋팅

	// 오일러 rotation
	mip::MATRIX44 matRotX;
	matRotX.identity();
	matRotX._22 = cos(m_angle.x) * cos(m_angle.x);
	matRotX._32 = sin(m_angle.x) * sin(m_angle.x);
	matRotX._23 = sin(m_angle.x) * sin(m_angle.x);
	matRotX._33 = cos(m_angle.x) * cos(m_angle.x);

	mip::MATRIX44 matRotY;
	matRotY.identity();
	matRotY._11 = cos(m_angle.y) * cos(m_angle.y);
	matRotY._31 = sin(m_angle.y) * sin(m_angle.y);
	matRotY._13 = sin(m_angle.y) * sin(m_angle.y);
	matRotY._33 = cos(m_angle.y) * cos(m_angle.y);

	mip::MATRIX44 matRotZ;
	matRotZ.identity();
	matRotZ._11 = cos(m_angle.z) * cos(m_angle.z);
	matRotZ._21 = sin(m_angle.z) * sin(m_angle.z);
	matRotZ._12 = sin(m_angle.z) * sin(m_angle.z);
	matRotZ._22 = cos(m_angle.z) * cos(m_angle.z);

	mip::VECTOR3 vecRotSpacing = matRotX * matRotY * matRotZ * mip::VECTOR3(m_dcmVolumeInfo.x_spacing, m_dcmVolumeInfo.y_spacing, m_dcmVolumeInfo.z_spacing);

	m_dcmVolumeInfo.x_spacing = vecRotSpacing.x;
	m_dcmVolumeInfo.y_spacing = vecRotSpacing.y;
	m_dcmVolumeInfo.z_spacing = vecRotSpacing.z;

	/* 구버전. 테스트할 때까지 보존 */
	//if (WIN_MANAGER->createCroppingVolumeData(m_pData_HU, m_fileSlice, m_dir_path, m_dcmVolumeInfo, m_preBox, angle) == false)
	//{
	//	this->setWindowModality(Qt::WindowModality::ApplicationModal);
	//	this->setVisible(true);
	//	return;
	//}

	if (createCroppingVolumeDataWithHU(m_pData_HU, m_cz, m_dcmVolumeInfo, m_croppingRangeBox, m_angle) == false)
	{
		this->setWindowModality(Qt::WindowModality::ApplicationModal);
		this->setVisible(true);
		return;
	}

	this->accept();
}

bool CroppingDialog::createCroppingVolumeDataWithHU(const mint16* pHUdata, int cz, const DicomVolumeInfo& dcmVolumeInfo, BoundingBoxI croppingRangeBox, mip::VECTOR3 angle)
{
	ACTION_MANAGER->action_ProgressBegin(STRING_MANAGER->getString(STR_CROPPING_DICOM_DATA));
	ACTION_MANAGER->action_ProgressUpdate(0);
	ACTION_MANAGER->clear();

	muint32 cropedX = croppingRangeBox.maxX - croppingRangeBox.minX + 1;
	muint32 cropedY = croppingRangeBox.maxY - croppingRangeBox.minY + 1;
	muint32 cropedZ = croppingRangeBox.maxZ - croppingRangeBox.minZ + 1;

	mint32 minX = croppingRangeBox.minX;
	mint32 maxX = croppingRangeBox.maxX;

	mint32 minY = croppingRangeBox.minY;
	mint32 maxY = croppingRangeBox.maxY;

	/* min, max Z값 반전 */
	mint32 minZ = cz - 1 - croppingRangeBox.maxZ;
	mint32 maxZ = cz - 1 - croppingRangeBox.minZ;

	if (m_pVolumeData->createData(
		cropedX,
		cropedY,
		cropedZ,
		dcmVolumeInfo.x_spacing * 0.1f,
		dcmVolumeInfo.y_spacing * 0.1f,
		dcmVolumeInfo.z_spacing * 0.1f
	) == false)
	{
		int ret = QMessageBox::warning(NULL, "Not enough memory",
			"Please have to do more cropping.",
			QMessageBox::Ok);
		return false;
	}
	ACTION_MANAGER->action_ProgressUpdate(5);		//create data - 5%

	// rotation
	std::vector<mint16> vecTempVolumeData(dcmVolumeInfo.dx * dcmVolumeInfo.dy * dcmVolumeInfo.dz, -1024);

	g_Renderer->makeCurrent();

	mip::VolumeComputer computeRotation;
	computeRotation.runComputer(pHUdata, dcmVolumeInfo.dx, dcmVolumeInfo.dy, dcmVolumeInfo.dz, angle, vecTempVolumeData);

	g_Renderer->doneCurrent();

	ACTION_MANAGER->action_ProgressUpdate(40);		//rotation - 40%

	muint32 croppingVolumeLength = m_pVolumeData->getVolumeDataLength();
	int croppingUpdatePercentUnit = croppingVolumeLength / 100;
	int croppingUpdatePercent = 0;
	// crop
	mint16* pVolumeData = m_pVolumeData->getHUDataPoint();
	for (int z = minZ, cropZ = 0; z <= maxZ; ++z, ++cropZ)
	{
		for (int y = minY, cropY = 0; y <= maxY; ++y, ++cropY)
		{
			for (int x = minX, cropX = 0; x <= maxX; ++x, ++cropX)
			{
				int indexOrigin = z * dcmVolumeInfo.dy * dcmVolumeInfo.dx + y * dcmVolumeInfo.dx + x;
				int indexCrop = cropZ * cropedY * cropedX + cropY * cropedX + cropX;
				pVolumeData[indexCrop] = vecTempVolumeData[indexOrigin];

				if ((indexCrop % croppingUpdatePercentUnit) == 0)
				{
					croppingUpdatePercent++;
					ACTION_MANAGER->action_ProgressUpdateWithRange(croppingUpdatePercent, 40, 100);		//crop - 40 ~ 100%
				}

			}
		}
	}
	ACTION_MANAGER->action_ProgressUpdate(100);		//crop - 100%

	ACTION_MANAGER->action_ProgressEnd();

	return true;
}


void CroppingDialog::OnReset()
{
	m_axialViewWidget->Reset();
	m_coronalViewWidget->Reset();
	m_sagittalViewWidget->Reset();

	updateInfomationPanel();
	renderLater();
}

void CroppingDialog::OnChangeMemoryLabel(WINDOW_TYPE eWinType)
{
	adjustBoundingBoxSize(eWinType);
	updateInfomationPanel();
	renderLater();
}
