#pragma once

#include <qicon>
#include <qstring>

namespace fm
{
	class IconManager
	{
	public:
		static QIcon GetFileIcon(QString filepath);
	};
}
