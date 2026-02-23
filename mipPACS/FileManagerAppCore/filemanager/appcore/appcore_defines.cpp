#include "stdafx.h"
#include "filemanager/appcore/appcore_defines.h"
#include <qmetatype>

namespace fm
{
	struct RegisterAppCore
	{
		RegisterAppCore()
		{
			qRegisterMetaType<DcmNetFindData>("DcmNetFindData");
			qRegisterMetaType<DcmNetDownloadData>("DcmNetDownloadData");
		}
	};
	RegisterAppCore __registerAppcore__;
}
