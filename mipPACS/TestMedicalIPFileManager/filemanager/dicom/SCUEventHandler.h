#pragma once

#include <qobject>
#include "filemanager/dicom/Network/DicomNetworkSCU.h"
#include <vector>

class SCUEventHandler : public QObject
{
	Q_OBJECT

public slots:
	void onSCUCommunicationInProgress(fm::DicomNetworkSCUStatus status);

public:
	std::vector<fm::DicomNetworkSCUStatus> StatusList;
};
