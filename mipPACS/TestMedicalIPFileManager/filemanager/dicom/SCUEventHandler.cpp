#include "stdafx.h"
#include "SCUEventHandler.h"

void SCUEventHandler::onSCUCommunicationInProgress(fm::DicomNetworkSCUStatus status)
{
	StatusList.push_back(status);
}
