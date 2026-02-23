#include "stdafx.h"
#include "AITranslationTab.h"
#include "StringManager.h"
#include "WindowManager.h"
#include "StyleManager.h"
#include "LicenseManager.h"
#include "ActionManager.h"
#include "Renderer/Renderer.h"
#include "DataContext.h"

AITranslationTab::AITranslationTab(QWidget* parent /*= NULL*/)
	:CollapseWidget(QString(), parent)
	, m_tabSet(nullptr)
{
	int nRow = 0;

	QGroupBox* grpBox = new QGroupBox(this);
	grpBox->setTitle("Translation");

	addWidget(grpBox, nRow++);

	QVBoxLayout* grpLay = new QVBoxLayout;
	QHBoxLayout* hLay = new QHBoxLayout;
	grpBox->setLayout(grpLay);
	grpLay->addLayout(hLay);

	QLabel* label = new QLabel(this);
	label->setText("Project");
	hLay->addWidget(label);
	m_ComboProj = new QComboBox(this);
	/*TODO:: load project*/
	LoadProject();
	m_ComboProj->addItem("");
	m_ComboProj->setStyleSheet(STYLE_MANAGER->comboBoxTab);
	m_ComboProj->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	m_ComboProj->installEventFilter(this);
	hLay->addWidget(m_ComboProj, 2);

	//connect(m_ComboProj, SIGNAL(currentIndexChanged(int)), this, SLOT(OnProjChanged(int)));

	QPushButton* btn;// = new QPushButton(this);

	btn = new QPushButton(this);
	btn->setText("Predict");
	btn->setStyleSheet(STYLE_MANAGER->buttonChecked);
	btn->setCheckable(true);

	hLay->addWidget(btn, 1);
	connect(btn, &QPushButton::clicked, this, &AITranslationTab::slot_OnPredict);
	connect(this, &AITranslationTab::sig_disablePredictBtn, btn, &QPushButton::setDisabled);
	connect(this, &AITranslationTab::sig_checkedPredictBtn, btn, &QPushButton::setChecked);



	//GPU CPU button
	{
		hLay = new QHBoxLayout;
		grpLay->addLayout(hLay);


		grpBox = new QGroupBox(this);
		grpBox->setTitle("Processor");
		hLay->addWidget(grpBox, 1);

		// 		emptyBox0 = new QWidget(this);
		// 		emptyBox0->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		// 
		// 		hLay->addWidget(emptyBox0, 1);
		hLay = new QHBoxLayout;
		grpBox->setLayout(hLay);



		m_GrProc = new QButtonGroup(this);

		const QString vendor = QString::fromLocal8Bit(g_Renderer->getVendorName());

		m_GPU = true;
		if (!vendor.contains(QString("NVIDIA"), Qt::CaseInsensitive))
			m_GPU = false;

		QRadioButton* btnR = new QRadioButton(this);
		btnR->setText("GPU");
		btnR->setCheckable(m_GPU);
		btnR->setChecked(m_GPU);
		hLay->addWidget(btnR);
		m_GrProc->addButton(btnR, 0);

		btnR = new QRadioButton(this);
		btnR->setText("CPU");
		btnR->setCheckable(true);
		btnR->setChecked(!m_GPU);
		hLay->addWidget(btnR);
		m_GrProc->addButton(btnR, 1);

		connect(m_GrProc, SIGNAL(buttonClicked(int)), this, SLOT(slot_OnProcChanged(int)));
	}

	hLay = new QHBoxLayout;
	grpLay->addLayout(hLay);


	QString title = "AI Segmentation";
	setTitle(title);

	setOpenWidget(false);
}

int AITranslationTab::getAIType()
{
	if (m_ComboProj)
		return m_ComboProj->currentIndex();
	return 0;
}

void AITranslationTab::LoadProject(bool reload /*= false*/)
{
	m_ComboProj->blockSignals(true);

	if (reload)
		m_ComboProj->clear();

	QDir dir(STRING_MANAGER->AITranslationPath);

	if (dir.exists())
	{
		QString Path = STRING_MANAGER->AITranslationPath + "/weight";
		dir.setPath(Path);

		if (dir.exists())
		{
			QStringList filters;
			filters << "*.mipx" << "*.MIPX";
			dir.setNameFilters(filters);

			QStringList files = dir.entryList();

			for each (auto var in files)
			{
				var.replace(QString(".mipx"), QString(""), Qt::CaseInsensitive);
				if (-1 == m_ComboProj->findText(var))
					m_ComboProj->addItem(var);
			}
		}
	}

	if (reload)
		m_ComboProj->addItem("");

	m_ComboProj->blockSignals(false);
}

bool AITranslationTab::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == NULL);

	if (watched->objectName().contains("Slider") || watched->objectName().contains("Combo"))
	{
		if (event->type() == QEvent::Wheel)
			return true;
	}

	if (watched->objectName().contains("Edit"))
	{
		if (event->type() == QEvent::FocusIn)
			WIN_MANAGER->setMoveFocus(false);
	}

	return QWidget::eventFilter(watched, event);

}

void AITranslationTab::slot_OnProcChanged(int index)
{
	if (!m_GrProc->button(index)->isCheckable())
		return;

	if (index == 0)
		m_GrProc->button(1)->setChecked(false);
	else
		m_GrProc->button(0)->setChecked(false);

	m_GPU = m_GrProc->button(index)->isChecked();
}

void AITranslationTab::slot_OnPredict()
{
	const QString projPath = STRING_MANAGER->AITranslationPath + "/weight";
	QString copyPath = STRING_MANAGER->AITranslationPath + "/predict_result";

	QString projName = m_ComboProj->currentText();

	QDir dir(copyPath);

	if (!dir.exists())
	{
		dir.mkdir(copyPath);
	}

	if (!DATA_CONTEXT->volume_data.isValidate()) return;

	int index = m_ComboProj->currentIndex();
	index++;

	if (index == m_ComboProj->count()) //new
	{
		QMessageBox::warning(this, "AI Predict", "Before you can run Predict, you need to run Train or choose other project.");
		return;
	}

	//int start, end;

	//m_tabSet->getDepth(start, end);

	QString filePath = projPath + QString("/%1.mipx").arg(projName);

	QFile file(filePath);

	bool res = file.exists();
	bool predict_res = false;
	if (res)
	{
		emit sig_disablePredictBtn(true);
		emit sig_checkedPredictBtn(true);

		copyPath.append(QString("/%1").arg(projName));

		if (dir.exists(copyPath))
			dir.removeRecursively();

		predict_res = ACTION_MANAGER->action_Translation_predict(projName, m_GPU);


	}
	else
	{
		QMessageBox::warning(this, "AI Predict", "Before you can run Predict, you need to run Train or choose other trained project.");
		return;
	}

	if (predict_res)
	{


		//m_tabSet->FreezeDepth(true);

	}

}
