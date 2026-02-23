#pragma once

#include "dcmtk/dcmnet/dfindscu.h"
#include "dcmtk/dcmnet/diutil.h"

namespace fm
{
	struct NetworkSendData
	{
		T_ASC_Network *Net = NULL;
		T_ASC_Association* Assoc = NULL;
		T_ASC_Parameters* Params = NULL;
		T_ASC_PresentationContextID PresentationContextID;
		T_DIMSE_Message Message;
		T_DIMSE_BlockingMode BlockMode = DIMSE_BLOCKING;
	};

	struct NetworkRecvData
	{
		T_ASC_PresentationContextID PresentationContextID;
		T_DIMSE_Message Message;
		DcmDataset* ReceivedDataset = NULL;
		Uint16 ReturnStatus = 0;
	};

	//struct 
}
