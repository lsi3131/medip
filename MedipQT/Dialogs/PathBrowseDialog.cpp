#include "stdafx.h"
#include "PathBrowseDialog.h"

#include "Windows/windowManager.h"


PathBrowseDialog::PathBrowseDialog(QString title, QString content, QWidget *parent /*= NULL*/)
	:QDialog(parent)
{
	setWindowFlags(this->windowFlags() & ~(Qt::WindowContextHelpButtonHint | Qt::WindowMinMaxButtonsHint));

	setWindowTitle(title);

	
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QHBoxLayout* hLayout = NULL;
	
	
	if(content.length() > 1)
	{
		hLayout = new QHBoxLayout;
		mainLayout->addLayout(hLayout);
	
		QLabel* label = new QLabel(this);
		label->setText(content);
		hLayout->addWidget(label);
	}
	

	hLayout = new QHBoxLayout;
	mainLayout->addLayout(hLayout);

	editPath = new QLineEdit(this);
	editPath->setReadOnly(true);
	if (!WIN_MANAGER->lastestPathGet(strPath, true))
		strPath = QDir::homePath();

	editPath->setText(strPath.toLocal8Bit().constData());

	QPushButton* btn = new QPushButton("...", this);
	connect(btn, &QPushButton::clicked, this, &PathBrowseDialog::OnPathChanged);

	QWidget* emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	hLayout->addWidget(editPath);
	hLayout->addWidget(btn);
	hLayout->addWidget(emptyBox);

	hLayout = new QHBoxLayout;
	mainLayout->addLayout(hLayout);


	btn = new QPushButton("OK", this);
	connect(btn, &QPushButton::clicked, this, &QDialog::accept);

	hLayout->addWidget(btn);

	btn = new QPushButton("Cancel", this);
	connect(btn, &QPushButton::clicked, this, &QDialog::reject);

	hLayout->addWidget(btn);

	emptyBox = new QWidget(this);
	emptyBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	hLayout->addWidget(emptyBox);

}

void PathBrowseDialog::OnPathChanged()
{
	const QFileDialog::Options options = QFlag(QFileDialog::ShowDirsOnly);
	QString selectedFilter;


	QString filePath = QFileDialog::getExistingDirectory(this,
		"Path Browser",
		strPath);

	if (!filePath.isEmpty())
	{
		strPath = filePath;

		editPath->setText(strPath);
	}
}

