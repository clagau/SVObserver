//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ModuleController.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Controls all modules which are in a configuration.
//******************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include "ModuleController.h"
#include "TextDefinesSvO.h"
#include "ToolClipboard.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "Tools/GroupTool.h"
#include "Tools/ModuleTool.h"
#pragma endregion Includes

ModuleController::ModuleController()
	: m_xmlFilePath {SvStl::GlobalPath::Inst().GetTempPath() + _T("\\Module.xml")}
{
}

void ModuleController::loadModules()
{
	//@TODO[MZA][10.30][23.12.2022] loadModules (load from config)
}

void ModuleController::saveModules()
{
	//@TODO[MZA][10.30][23.12.2022] saveModules (save to config)
}

void ModuleController::checkIfNameValid(const std::string& rName) const
{
	//check if only valid characters are used.
	if (!SvUl::isValidObjectName(rName))
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NameContainsInvalidChars, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}


	//check if name is unique
	if (std::ranges::any_of(m_moduleList, [rName](const auto& rEntry) { return rEntry.m_name == rName; }))
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleNameAlreadyUsed, {rName}, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}
}

void ModuleController::convertGroupTool(uint32_t toolId, const std::string& rName)
{
	checkIfNameValid(rName);

	auto* pGroupTool = dynamic_cast<SvTo::GroupTool*>(SVObjectManagerClass::Instance().GetObject(toolId));
	if (nullptr != pGroupTool)
	{
		auto* pOwner = dynamic_cast<SvIe::SVTaskObjectListClass*>(pGroupTool->GetParent());
		if (nullptr == pOwner)
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidOwner, SvStl::SourceFileParams(StdMessageParams), toolId);
			throw msg;
		}

		//create ModulTool and delete GroupTool
		auto* pModuleTool = new SvTo::ModuleTool(pOwner);
		pOwner->InsertBefore(pGroupTool->getObjectId(), pModuleTool);
		if (pModuleTool && !pModuleTool->is_Created())
		{
			pModuleTool->MoveObjectToThis(*pGroupTool);
			pOwner->DestroyChildObject(pGroupTool, SvDef::SVMFSetDefaultInputs);
			pOwner->CreateChildObject(*pModuleTool, SvDef::SVMFResetInspection);
		}
		else
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NotCreated, SvStl::SourceFileParams(StdMessageParams), toolId);
			throw msg;
		}


		//write Module to file
		auto dependencyFiles = streamToolsToXmlFile({toolId}, m_xmlFilePath);
		if (!SvUl::makeZipFile(SvStl::GlobalPath::Inst().GetRunPath((rName + SvO::ModuleExtension).c_str()), dependencyFiles, _T(""), false))
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ClipboardZipFailed, SvStl::SourceFileParams(StdMessageParams), toolId);
			throw msg;
		}
		::DeleteFile(m_xmlFilePath.c_str());
	}
	else
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ObjectNotFound, {std::to_string(toolId)}, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}
}