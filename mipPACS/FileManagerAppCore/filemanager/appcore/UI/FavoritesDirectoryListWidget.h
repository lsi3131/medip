#pragma once

#include <qwidget>
#include <QListWidget>

namespace fm
{
	class FolderViewConfig;

	class FavoritesDirectoryListWidget : public QListWidget
	{
		Q_OBJECT

	public:
		FavoritesDirectoryListWidget(QWidget* parent = nullptr);

	public:
		void Init(FolderViewConfig* pFoloderViewConfig);
		bool Add(QString filepath, QString name);
		bool EditName(QString filepath, QString name);
		bool DeleteCurSel();

	private:
		void UpdateList();

	Q_SIGNALS:
		void directorySelected(QString filepath);

	private slots:
		void onItemClicked(QListWidgetItem* item);

	private:
		FolderViewConfig* m_pFolderViewConfig;
	};
}
