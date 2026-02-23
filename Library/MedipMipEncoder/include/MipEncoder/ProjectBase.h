#pragma once
#include "defineMipEncoder.h"
#include <Qfile>


class ProjectBase
{
public:
	

public:
	ProjectBase();
	virtual ~ProjectBase();
		
	virtual bool LoadProjectFile(QIODevice &file, MIP_ENCODER::ProjectDataInfo* pProjData) = 0;
	virtual bool SaveProjectFile(QIODevice &file, const MIP_ENCODER::ProjectDataInfo* pProjData) = 0;


protected:
	MIP_ENCODER::ProjectHead m_header;

	
	
};

