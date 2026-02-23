#include "stdafx.h"
#include "MacroArgumentParser_Omniverse.h"

MacroArgumentParser_Omniverse::MacroArgumentParser_Omniverse()
{
}

bool MacroArgumentParser_Omniverse::Parse(MacroParameter* pOutParameter, QString macro, QStringListIterator it, QString* pOutErrorMessage)
{
	if (macro == MACRO_OMNIVERSE_UPLOAD_USD)
	{
		return parseUploadUsd(pOutParameter, it, pOutErrorMessage);
	}
	else if (macro == MACRO_OMNIVERSE_CREATE_USD_MODEL)
	{
		return parseCreateUsdModel(pOutParameter, it, pOutErrorMessage);
	}
	else if (macro == MACRO_OMNIVERSE_CREATE_MESH_STL)
	{
		return parseCreateMeshSTL(pOutParameter, it, pOutErrorMessage);
	}
	else
	{
		qWarning() << "not supported macro : " << macro;
		return false;
	}
}

bool MacroArgumentParser_Omniverse::parseUploadUsd(MacroParameter* pOutParameter, QStringListIterator it, QString* pOutErrorMessage)
{
	if (it.hasNext() == false)
	{
		qWarning() << "ip address not exist";
		return false;
	}
	pOutParameter->OmniverseUploadUsd.IPAddress = it.next();

	if (it.hasNext() == false)
	{
		qWarning() << "upload load file path not exist";
		return false;
	}
	pOutParameter->OmniverseUploadUsd.LocalFilePath = it.next();

	if (it.hasNext() == false)
	{
		qWarning() << "server url not exist";
		return false;
	}
	pOutParameter->OmniverseUploadUsd.ServerDirectoryPath = it.next();

	return true;
}

bool MacroArgumentParser_Omniverse::parseCreateUsdModel(MacroParameter* pOutParameter, QStringListIterator it, QString* pOutErrorMessage)
{
	if (it.hasNext() == false)
	{
		qWarning() << "ip address not exist";
		return false;
	}
	pOutParameter->OmniverseCreateUsdModelUsd.IPAddress = it.next();

	if (it.hasNext() == false)
	{
		qWarning() << "usd file path not exist";
		return false;
	}
	pOutParameter->OmniverseCreateUsdModelUsd.UsdFilePath = it.next();

	return true;
}

bool MacroArgumentParser_Omniverse::parseCreateMeshSTL(MacroParameter* pOutParameter, QStringListIterator it, QString* pOutErrorMessage)
{
	if (it.hasNext() == false)
	{
		qWarning() << "ip address not exist";
		return false;
	}
	pOutParameter->OmniverseCreateMeshSTL.IPAddress = it.next();

	if (it.hasNext() == false)
	{
		qWarning() << "usd file path not exist";
		return false;
	}
	pOutParameter->OmniverseCreateMeshSTL.UsdFilePath = it.next();

	if (it.hasNext() == false)
	{
		qWarning() << "local stl file not exist";
		return false;
	}
	pOutParameter->OmniverseCreateMeshSTL.LocalSTLFilePath = it.next();

	if (it.hasNext() == false)
	{
		qWarning() << "mesh layer name not exist";
		return false;
	}
	pOutParameter->OmniverseCreateMeshSTL.MeshLayerName = it.next();

	return true;
}

