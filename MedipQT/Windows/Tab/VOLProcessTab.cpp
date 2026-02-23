#include "stdafx.h"
#include "VOLProcessTab.h"
#include "StringManager.h"
#include "StyleManager.h"
#include "WindowManager.h"
#include "ProductManager.h"
#include "Main/MainSegmentWidget.h"
#include "VolumeCalculator.h"
#include "DataContext.h"

VOLProcessTab::VOLProcessTab(QWidget* parent /*= NULL*/) : 
	CollapseWidget(QString(), parent)
{
	int nRow = 0;

	//btnXFlip = new QPushButton(this);
	m_btnXFlip = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_VolumeOperation_VolumeFlipping, this);
	if (m_btnXFlip)
	{
		m_btnXFlip->setText(" X-Flip ");
		m_btnXFlip->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(m_btnXFlip, &QPushButton::clicked, this, &VOLProcessTab::slot_OnXFlip);

		addWidget(m_btnXFlip, nRow, 0, QMargins(5, 0, 5, 0));
	}


	//m_btnYFlip = new QPushButton(this);
	m_btnYFlip = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_VolumeOperation_VolumeFlipping, this);
	if (m_btnYFlip)
	{
		m_btnYFlip->setText(" Y-Flip ");
		m_btnYFlip->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(m_btnYFlip, &QPushButton::clicked, this, &VOLProcessTab::slot_OnYFlip);

		addWidget(m_btnYFlip, nRow, 0);
	}

	//m_btnZFlip = new QPushButton(this);
	m_btnZFlip = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_VolumeOperation_VolumeFlipping, this);
	if (m_btnZFlip)
	{
		m_btnZFlip->setText(" Z-Flip ");
		m_btnZFlip->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(m_btnZFlip, &QPushButton::clicked, this, &VOLProcessTab::slot_OnZFlip);

		addWidget(m_btnZFlip, nRow++, 0);
	}

	//QPushButton *btnIso = new QPushButton(this);
	QPushButton* btnIso = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_VolumeOperation_Z_Isotropification, this);
	if (btnIso)
	{
		btnIso->setText(" Spacing Modifier");
		btnIso->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(btnIso, &QPushButton::clicked, this, &VOLProcessTab::slot_OnIsolation);

		addWidget(btnIso, nRow, 0, QMargins(5, 0, 5, 0));
	}

	//QPushButton *btn = new QPushButton(this);
	QPushButton* btn = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_VolumeOperation_VolumeDownscale, this);
	if (btn)
	{
		btn->setText("Downscale");
		btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(btn, &QPushButton::clicked, this, &VOLProcessTab::slot_OnScale);

		addWidget(btn, nRow, 0);
	}

	//btn = new QPushButton(this);
	btn = nullptr;
	btn = PRODUCT_FACTORY->createWidget<QPushButton>(MFL_Common_VolumeOperation_VolumeCropping, this);
	if (btn)
	{
		btn->setText("Volume Cropping");
		btn->setStyleSheet(STYLE_MANAGER->buttonBehind);
		connect(btn, &QPushButton::clicked, this, &VOLProcessTab::slot_OnVolumeCrop);

		addWidget(btn, nRow++, 0);
	}

	//QWidget *emptyBox0 = new QWidget(this);
	// 	QWidget *emptyBox0 = PRODUCT_FACTORY->createWidget<QPushButton>(MS_COLLAPS_VOLUME_PROCESSING_TAB, this);
	// 	if (emptyBox0)
	// 	{
	// 		emptyBox0->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	// 
	// 		addWidget(emptyBox0, nRow++, 0);
	// 	}

	QString title = "Volume Processing";
	setTitle(title);
	setOpenWidget(false);
}
#include "Dialogs/SpacingDialog.h"

void VOLProcessTab::slot_OnIsolation()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	{
		bool res = false;
		SpacingDialog dlg(DATA_CONTEXT->volume_data.getSpaceX(true), DATA_CONTEXT->volume_data.getSpaceY(true), DATA_CONTEXT->volume_data.getSpaceZ(true), this);

		dlg.exec();
		double m_spacing[3];// = nullptr;// = new double[3];

		if (dlg.isAccept())
		{
			if (dlg.isIsotro())
			{
				ACTION_MANAGER->action_ImageIsotropic();

			}
			else
			{
				dlg.getSpacing(m_spacing);

				DATA_CONTEXT->volume_data.setSpace(m_spacing[0] / 10, m_spacing[1] / 10, m_spacing[2] / 10);
				WIN_MANAGER->updateSummary();

				WIN_MANAGER->resetTextures();

				DATA_CONTEXT->volume_data.forceUpdateVolume();
				DATA_CONTEXT->volume_data.forceUpdateMaskVolume();
				WIN_MANAGER->setRenderable(true);
				WIN_MANAGER->mainSegmentWidget->resetUI(true);
				WIN_MANAGER->updateUI();
				WIN_MANAGER->renderLater_All();
				WIN_MANAGER->mainSegmentWidget->createHUHisto();

				WIN_MANAGER->patchyPoint = QVector3D(-1, -1, -1);
				WIN_MANAGER->setPatchyPoint();
				WIN_MANAGER->setSaveState(false);
			}
		}

		//		SAFE_DELETES(m_spacing);



	}

	//	bool enableIsotro = true;
	// 	if (DATA_CONTEXT->volume_data.getSpaceX() >= DATA_CONTEXT->volume_data.getSpaceZ())
	// 	{
	// 		enableIsotro = false;
	// 		QMessageBox::warning(NULL, STRING_MANAGER->getString(STR_INFO), "Z-Interpolation is not required.");
	// 
	// 		return;
	// 	}


}

void VOLProcessTab::setFlipmode()
{
	if (WIN_MANAGER->isLayerFlip)
	{
		m_btnXFlip->setStyleSheet(STYLE_MANAGER->buttonBehindch);
		m_btnZFlip->setStyleSheet(STYLE_MANAGER->buttonBehindch);
		m_btnYFlip->setStyleSheet(STYLE_MANAGER->buttonBehindch);
	}
	else
	{
		m_btnXFlip->setStyleSheet(STYLE_MANAGER->buttonBehind);
		m_btnZFlip->setStyleSheet(STYLE_MANAGER->buttonBehind);
		m_btnYFlip->setStyleSheet(STYLE_MANAGER->buttonBehind);
	}
}

void VOLProcessTab::slot_OnXFlip()
{
	static bool YFliped = false;
	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	if (!WIN_MANAGER->isLayerFlip)
	{
		YFliped = !YFliped;
		WIN_MANAGER->setRenderable(false);
		muint32 dx = DATA_CONTEXT->volume_data.getCX();
		muint32 dy = DATA_CONTEXT->volume_data.getCY();
		muint32 dz = DATA_CONTEXT->volume_data.getCZ();
		memcpy(DATA_CONTEXT->volume_data.getHUFlipDataPoint(), DATA_CONTEXT->volume_data.getHUDataPoint(),
			sizeof(mint16) * DATA_CONTEXT->volume_data.getVolumeDataLength());
		int XVal;
		for (int z = 0; z < dz; z++)
		{
			for (int y = 0; y < dy; y++)
			{

				for (int x = 0; x < dx; x++)
				{
					XVal = dx - x - 1;
					DATA_CONTEXT->volume_data.setData(x, y, z,
						WIN_MANAGER->Get16((z * dx * dy) + (y * dx) + XVal));
				}
			}
		}
		SAFE_DELETES(DATA_CONTEXT->volume_data.pData3D_HU_Flip);
		DATA_CONTEXT->volume_data.forceUpdateVolume();
		WIN_MANAGER->setRenderable(true);
		WIN_MANAGER->renderLater_All();

		/* Volume 원본 데이터 Update */
		VOLUME_CALCULATOR->Initialize(&DATA_CONTEXT->volume_data);
	}
	else
	{
		for (int index = 0; index < DATA_CONTEXT->volume_data.getMaskInfoListCnt(); index++)
		{
			MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(index);
			if (info == NULL) continue;
			int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

			ACTION_MANAGER->action_MaskList_YFlip(*info, mI);
		}
	}
}

void VOLProcessTab::slot_OnYFlip()
{
	static bool YFliped = false;
	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	if (!WIN_MANAGER->isLayerFlip)
	{
		YFliped = !YFliped;
		WIN_MANAGER->setRenderable(false);
		muint32 dx = DATA_CONTEXT->volume_data.getCX();
		muint32 dy = DATA_CONTEXT->volume_data.getCY();
		muint32 dz = DATA_CONTEXT->volume_data.getCZ();
		memcpy(DATA_CONTEXT->volume_data.getHUFlipDataPoint(), DATA_CONTEXT->volume_data.getHUDataPoint(),
			sizeof(mint16) * DATA_CONTEXT->volume_data.getVolumeDataLength());
		int YVal;
		for (int z = 0; z < dz; z++)
		{
			for (int y = 0; y < dy; y++)
			{
				YVal = dy - y - 1;
				for (int x = 0; x < dx; x++)
				{
					DATA_CONTEXT->volume_data.setData(x, y, z,
						WIN_MANAGER->Get16((z * dx * dy) + (YVal * dx) + x));
				}
			}
		}
		SAFE_DELETES(DATA_CONTEXT->volume_data.pData3D_HU_Flip);
		DATA_CONTEXT->volume_data.forceUpdateVolume();
		WIN_MANAGER->setRenderable(true);
		WIN_MANAGER->renderLater_All();

		/* Volume 원본 데이터 Update */
		VOLUME_CALCULATOR->Initialize(&DATA_CONTEXT->volume_data);
	}
	else
	{
		for (int index = 0; index < DATA_CONTEXT->volume_data.getMaskInfoListCnt(); index++)
		{
			MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(index);
			if (info == NULL) continue;
			int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

			ACTION_MANAGER->action_MaskList_YFlip(*info, mI);
		}
	}
}

void VOLProcessTab::slot_OnZFlip()
{
	static bool ZFliped = false;
	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	if (!WIN_MANAGER->isLayerFlip)
	{
		ZFliped = !ZFliped;
		WIN_MANAGER->setRenderable(false);
		muint32 dx = DATA_CONTEXT->volume_data.getCX();
		muint32 dy = DATA_CONTEXT->volume_data.getCY();
		muint32 dz = DATA_CONTEXT->volume_data.getCZ();
		memcpy(DATA_CONTEXT->volume_data.getHUFlipDataPoint(), DATA_CONTEXT->volume_data.getHUDataPoint(),
			sizeof(mint16) * DATA_CONTEXT->volume_data.getVolumeDataLength());
		int zVal;
		for (int z = 0; z < dz; z++)
		{
			zVal = dz - z - 1;
			//	if (!ZFliped) zVal = z;
			//	else zVal = dz - z - 1;
			for (int y = 0; y < dy; y++)
			{
				for (int x = 0; x < dx; x++)
				{
					DATA_CONTEXT->volume_data.setData(x, y, z,
						WIN_MANAGER->Get16((zVal * dx * dy) + (y * dx) + x));
				}
			}
		}
		SAFE_DELETES(DATA_CONTEXT->volume_data.pData3D_HU_Flip);
		DATA_CONTEXT->volume_data.forceUpdateVolume();
		WIN_MANAGER->setRenderable(true);
		WIN_MANAGER->renderLater_All();

		/* Volume 원본 데이터 Update */
		VOLUME_CALCULATOR->Initialize(&DATA_CONTEXT->volume_data);
	}
	else
	{
		for (int index = 0; index < DATA_CONTEXT->volume_data.getMaskInfoListCnt(); index++)
		{
			MaskInfo* info = DATA_CONTEXT->volume_data.getMaskInfo(index);
			if (info == NULL) continue;
			int mI = info->uid >= MASK_SECOND_MAX ? (info->uid - MASK_SECOND_MAX) / 8 + 1 : 0;

			ACTION_MANAGER->action_MaskList_ZFlip(*info, mI);
		}
	}
}

void VOLProcessTab::slot_OnScale()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	ACTION_MANAGER->action_ImageScaling();
}

void VOLProcessTab::slot_OnVolumeCrop()
{
	if (DATA_CONTEXT->volume_data.isValidate() == false) return;

	ACTION_MANAGER->action_VolumeCropping();
}
