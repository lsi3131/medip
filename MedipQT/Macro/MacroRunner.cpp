#include "stdafx.h"
#include "MacroRunner.h"
#include "ActionManager.h"
#include "Macro/MacroArgumentParser.h"
#include "Omniverse/OmniverseContext.h"
#include "Actions/Omniverse/ActionOmniverseUploadUsd.h"
#include "Actions/Omniverse/ActionOmniverseCreateUsdModel.h"
#include "Actions/Omniverse/ActionOmniverseCreateMeshSTL.h"
#include "WindowManager.h"

bool MacroRunner::RunWithArgments(QStringList args, QString* pOutErrorMessage)
{
	bool success = false;
	args.pop_front();

	QString macroName;
	QStringListIterator it(args);
	MacroParameter macroParameter;
	success = false;
	while (it.hasNext())
	{
		QString arg = it.next();
		if (arg != "-macro")
		{
			break;
		}

		if (it.hasNext() == false)
		{
			qWarning() << "macro name is empty.";
			return false;
		}
		macroName = it.next();

		if (ParseMacroArgumentToParameter(&macroParameter, macroName, it, pOutErrorMessage) == false)
		{
			qCritical() << "fail to parse macro argument";
			return false;
		}
	}

	if (macroParameter.MacroName == MACRO_START_AND_CLOSE)
	{
		ACTION_MANAGER->action_MEDIP_MACRO_Start();
	}
	else if (macroParameter.MacroName == MACRO_EXPORT_MASK_RAW)
	{
		ACTION_MANAGER->action_MEDIP_MACRO_Export_Mask_Raw_Start(macroParameter.InputMIPFilePath, macroParameter.ExportDirectory);
	}
	else if (macroParameter.MacroName == MACRO_EXPORT_MASK_NII)
	{
		ACTION_MANAGER->action_MEDIP_MACRO_Export_Mask_NII_Start(macroParameter.InputMIPFilePath, macroParameter.ExportDirectory);
	}
	else if (macroParameter.MacroName == MACRO_OMNIVERSE_UPLOAD_USD)
	{
		ACTION_MANAGER->action_Omniverse_Upload_USD(
			macroParameter.OmniverseUploadUsd.IPAddress, 
			macroParameter.OmniverseUploadUsd.ServerDirectoryPath, 
			macroParameter.OmniverseUploadUsd.LocalFilePath
			);
	}

	return true;
}

bool MacroRunner::ParseMacroArgumentToParameter(MacroParameter* pOutParameter, QString macro, QStringListIterator it, QString* pOutErrorMessage)
{
	pOutParameter->MacroName = macro;

	if (macro == MACRO_START_AND_CLOSE)
	{
	}
	else if (macro == MACRO_EXPORT_MASK_RAW)
	{
		return ParseMacroArgumentToParameter_Export(pOutParameter, macro, it, pOutErrorMessage);
	}
	else if (macro == MACRO_EXPORT_MASK_NII)
	{
		return ParseMacroArgumentToParameter_Export(pOutParameter, macro, it, pOutErrorMessage);
	}
	else
	{
		*pOutErrorMessage = QString("invalid macro type : %1").arg(macro);
		qCritical() << "invalid command type. ";
		return false;
	}

	return true;
}

bool MacroRunner::ParseMacroArgumentToParameter_Export(MacroParameter* pOutParameter, QString macro, QStringListIterator it, QString* pOutErrorMessage)
{
	QString usage = QString("invalid parameter.\r\nUsage : MEDIP.exe -macro %1 [mip file path] [export directory path]").arg(macro);
	if (it.hasNext() == false)
	{
		*pOutErrorMessage = usage;
		return false;
	}
	pOutParameter->InputMIPFilePath = it.next();

	if (it.hasNext() == false)
	{
		*pOutErrorMessage = usage;
		return false;
	}
	pOutParameter->ExportDirectory = it.next();
	return true;
}


MacroRunner::MacroRunner(OmniverseContext* pOmniverse, WindowManager* pWindowManager) :
	m_pOmniverse(pOmniverse),
	m_pWindowManager(pWindowManager)
{
}

bool MacroRunner::Run(QStringList args, QString* pOutErrorMessage)
{
	MacroParameter parameter;
	if (MacroArgumentParser::Parse(&parameter, args, pOutErrorMessage) == false)
	{
		return false;
	}

	qInfo() << "Run macro : " << parameter.MacroName;
	
	if (parameter.MacroName == MACRO_OMNIVERSE_UPLOAD_USD)
	{
		ActionOmniverseUploadUsd(
			m_pOmniverse,
			parameter.OmniverseUploadUsd.IPAddress,
			parameter.OmniverseUploadUsd.ServerDirectoryPath,
			parameter.OmniverseUploadUsd.LocalFilePath).Run();
	}
	else if (parameter.MacroName == MACRO_OMNIVERSE_CREATE_USD_MODEL)
	{
		ActionOmniverseCreateUsdModel(
			m_pOmniverse,
			parameter.OmniverseCreateUsdModelUsd.IPAddress,
			parameter.OmniverseCreateUsdModelUsd.UsdFilePath).Run();
	}
	else if (parameter.MacroName == MACRO_OMNIVERSE_CREATE_MESH_STL)
	{
		ActionOmniverseCreateMeshSTL(
			m_pOmniverse,
			g_Renderer,
			parameter.OmniverseCreateMeshSTL.IPAddress,
			parameter.OmniverseCreateMeshSTL.UsdFilePath,
			parameter.OmniverseCreateMeshSTL.LocalSTLFilePath,
			parameter.OmniverseCreateMeshSTL.MeshLayerName).Run();
	}


	return true;
}



