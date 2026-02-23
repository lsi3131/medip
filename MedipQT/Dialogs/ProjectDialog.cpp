#include "stdafx.h"

#include "Dialogs/ProjectDialog.h"

#include "Windows/windowManager.h"
#include "Windows/Tabwindow.h"
#include "Windows/Tab/AISegTabDeepCatch.h"


ProjectDialog::ProjectDialog(QWidget* parent /*= NULL*/) :QDialog(parent)
	
{
	setWindowFlags(this->windowFlags() & ~(Qt::WindowContextHelpButtonHint | Qt::WindowMinMaxButtonsHint));

	setWindowTitle("New AI Project");

	QHBoxLayout* mainLay = new QHBoxLayout;
	setLayout(mainLay);

	QHBoxLayout *hLay = new QHBoxLayout;
	mainLay->addLayout(hLay);


	QLabel* label = new QLabel(this);
	label->setText("Name");

	hLay->addWidget(label, 0, Qt::AlignLeft);

	m_Name = new QLineEdit(this);
//	connect(m_Name, &QLineEdit::returnPressed, this, &ProjectDialog::OnOK);

	hLay->addWidget(m_Name, 0, Qt::AlignHCenter);

	hLay = new QHBoxLayout;
	mainLay->addLayout(hLay);

	QPushButton* btn = new QPushButton(this);
	btn->setText("OK");
	connect(btn, &QPushButton::clicked, this, &ProjectDialog::OnOK);

	hLay->addWidget(btn);
}

QString ProjectDialog::getProjName()
{
	return m_Name->text();
}

void ProjectDialog::accept()
{
	AISegTab* tab = WIN_MANAGER->GetTab()->getAITab();
	QStringList list;
	QString existProj;
	list.clear();
	if (tab)
	{
		list = tab->GetAIProjectNameList();
	}

	for (int i = 0; i < list.count(); i++)
	{
		if (list.at(i) == m_Name->text())
		{
			existProj = m_Name->text();
			m_Name->clear();
			break;
		}
	}

	if (m_Name->text().isEmpty())
	{
		QMessageBox::warning(this, QString("Project name"), QString("The name can not be added. '%1'").arg(existProj));
		return;
	}
	else if (m_Name->text().contains(QRegularExpression(FILE_NAME_RULE)))
	{
		QMessageBox::warning(this, QString("Project name"),
			QString("A file name cannot contain any of the following characters : \\ / : *? \" < > |	"));

		return;
	}

	QDialog::accept();
}

void ProjectDialog::OnOK()
{
	accept();
}
