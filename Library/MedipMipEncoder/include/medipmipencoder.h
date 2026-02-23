#ifndef MEDIPMIPENCODER_H
#define MEDIPMIPENCODER_H

#include "medipmipencoder_global.h"
#include "defineMipEncoder.h"
#include "MipEncoder\ProjectBase.h"

class MEDIPMIPENCODER_EXPORT MedipMipEncoder
{
public:
	MedipMipEncoder();
	virtual ~MedipMipEncoder();

	static MedipMipEncoder *getInstance() {
		static MedipMipEncoder instance;
		return &instance;
	}
		
	void SetProjectMethod(MIP_ENCODER::PROJ_TYPE eType);
	MIP_ENCODER::ERROR_MESSAGE LoadProjectFile(QString strPath, MIP_ENCODER::ProjectDataInfo* pProData, MIP_ENCODER::ProjectHead* pOutLoadedHeader = nullptr);
	MIP_ENCODER::ERROR_MESSAGE SaveProjectFile(QString strPath, const MIP_ENCODER::ProjectDataInfo* pProData);
	MIP_ENCODER::ERROR_MESSAGE SaveProjectFile(QString strPath, const MIP_ENCODER::ProjectDataInfo* pProData, int nMipVersion);

	QString GetStringFromModalityType(mint8 type);
	mint8 GetModalityTypeFromString(QString strType);

	void GetPojectVersionInfo(mint32 &nMIP, mint32& nMIPD);

	QString GetErrorMsg(MIP_ENCODER::ERROR_MESSAGE eErr);
private:
	MIP_ENCODER::ProjectHead LoadHeader(QIODevice & file);	

	MIP_ENCODER::ProjectHead m_header;	// load시 에만 필요
	ProjectBase* m_pProjectFile = nullptr;
	MIP_ENCODER::PROJ_TYPE	m_eProjectType = MIP_ENCODER::PROJ_TYPE::PT_MIP;

	mint32 m_latestMIP;
	mint32 m_latestMIPD;
	mint32 m_latestMIPA;

};

#define MIP_ENCODER_SINGLTON (MedipMipEncoder::getInstance())

#endif // MEDIPMIPENCODER_H
