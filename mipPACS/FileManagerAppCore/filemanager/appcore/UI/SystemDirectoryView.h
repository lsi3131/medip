#pragma once

#include <QListView>
#include <QEventLoop>
#include "filemanager/export.h"

class QAbstractItemModel;
class QFileSystemModel;

namespace fm
{
	class FM_CORE_EXPORT SystemDirectoryView : public QListView
	{
		Q_OBJECT
	public:
		SystemDirectoryView(QWidget* parent = nullptr);
		virtual ~SystemDirectoryView();

	public:
		void Initialize();

		QFileSystemModel* GetModel();

	protected:
		virtual void keyPressEvent(QKeyEvent* e) override;

	Q_SIGNALS:
		void returnPressed(const QModelIndex& index);

	private:
		QFileSystemModel* m_dirModel;
	};
}
