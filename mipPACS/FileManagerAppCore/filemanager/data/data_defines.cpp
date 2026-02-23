#include "stdafx.h"
#include "data_defines.h"

namespace fm
{
	struct RegisterData
	{
		RegisterData()
		{
			qRegisterMetaType<ImportedDicomInfoDTO>("ImportedDicomInfoDTO");
			qRegisterMetaType<std::vector<ImportedDicomInfoDTO>>("std::vector<ImportedDicomInfoDTO>");
			qRegisterMetaType<LastEditedFileDTO>("LastEditedFileDTO");
			qRegisterMetaType<std::vector<LastEditedFileDTO>>("std::vector<LastEditedFileDTO>");
		}
	};
	static RegisterData __registerData__;
}