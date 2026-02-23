#include "stdafx.h"
#include "Condition.h"

makeConditionConst(fm::Normal, 0, 0, fm::FM_OK, "Normal");
makeConditionConst(fm::MemoryExhausted, 0, 0, fm::FM_FAILURE, "Virtual Memory exhausted");
makeConditionConst(fm::DcmNetInfoNotInitialized, 0, 0, fm::FM_FAILURE, "dicom net info is not initialized");
makeConditionConst(fm::DcmNetAssociationNegociation, 0, 0, fm::FM_FAILURE, "dicom net association negociation failed");
makeConditionConst(fm::DcmNetSCUInitialize, 0, 0, fm::FM_FAILURE, "dicom net SCU initialize failed");
makeConditionConst(fm::DcmNetSCPListenerInitialize, 0, 0, fm::FM_FAILURE, "dicom net SCP listener initialize failed");
makeConditionConst(fm::DcmNetSCPDownloading, 0, 0, fm::FM_FAILURE, "dicom net SCP downloading failed");
makeConditionConst(fm::DcmNetSCPReceiveAssociation, 0, 0, fm::FM_FAILURE, "dicom net SCP receive association failed");
makeConditionConst(fm::DcmNetInvalidDownloadProtocol, 0, 0, fm::FM_FAILURE, "dicom net invalid download protocol");
makeConditionConst(fm::DcmNetDIMSEEcho, 0, 0, fm::FM_FAILURE, "dicom net DIMSE echo failed");
makeConditionConst(fm::DcmNetDIMSEFind, 0, 0, fm::FM_FAILURE, "dicom net DIMSE find failed");
makeConditionConst(fm::DcmNetDIMSEMove, 0, 0, fm::FM_FAILURE, "dicom net DIMSE move failed");
makeConditionConst(fm::DcmNetDIMSEGet, 0, 0, fm::FM_FAILURE, "dicom net DIMSE get failed");
makeConditionConst(fm::DcmNetDIMSEStore, 0, 0, fm::FM_FAILURE, "dicom net DIMSE store failed");
makeConditionConst(fm::DcmNetSCURelease, 0, 0, fm::FM_FAILURE, "dicom net SCU release failed");
makeConditionConst(fm::DcmNetSCPRelease, 0, 0, fm::FM_FAILURE, "dicom net SCP release failed");

