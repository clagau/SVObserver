//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ModuleController.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Controls all modules which are in a configuration.
//******************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include "SVConfigurationObject.h"
#include "ModuleController.h"
#include "TextDefinesSvO.h"
#include "ToolClipboard.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "Tools/GroupTool.h"
#include "Tools/ModuleTool.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "FilesystemUtilities/FilepathUtilities.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion Includes

namespace
{
std::string getModulePath(const std::string& rModuleName)
{
	return SvStl::GlobalPath::Inst().GetRunPath((rModuleName + SvO::ModuleExtension).c_str());
}

void createModuleLFile(SvTo::ModuleTool& rModuleTool, const std::string& rModuleName, const std::string& rXmlFilePath, const std::string& rFullOwnerPath)
{
	//write Module to file
	std::string oldName = rModuleTool.GetName();
	rModuleTool.SetName(rModuleName.c_str()); //Set name to Module name to have the name in xmlFile
	auto dependencyFiles = streamToolsToXmlFile({rModuleTool.getObjectId()}, rXmlFilePath);
	rModuleTool.SetName(oldName.c_str());

	//Rename all old names to new name in XML-File
	auto xmlData = SvFs::readContentFromFile(rXmlFilePath);
	std::string fullToolNameStr = rFullOwnerPath + _T(".") + oldName;
	std::string fullToolNameNewStr = rFullOwnerPath + _T(".") + rModuleName;
	SvUl::searchAndReplace(xmlData, fullToolNameStr.c_str(), fullToolNameNewStr.c_str());
	SvFs::writeStringToFile(rXmlFilePath, xmlData, true);

	const auto moduleFilePath = getModulePath(rModuleName);
	if (!SvUl::makeZipFile(moduleFilePath, dependencyFiles, _T(""), false))
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ClipboardZipFailed, SvStl::SourceFileParams(StdMessageParams), rModuleTool.getObjectId());
		throw msg;
	}
	::DeleteFile(rXmlFilePath.c_str());

	AddFileToConfig(moduleFilePath.c_str());
}

std::vector<std::pair<time_t, std::string>> readHistory(SVTreeType& rTree, SVTreeType::SVBranchHandle hModule)
{
	std::vector<std::pair<time_t, std::string>> historyVector;
	SVTreeType::SVBranchHandle hHistory(nullptr);
	SvXml::SVNavigateTree::GetItemBranch(rTree, scHistoryTag, hModule, hHistory);
	SvXml::SVXMLMaterialsTree::SVBranchHandle hInput {rTree.getFirstBranch(hHistory)};
	do
	{
		if (hHistory)
		{
			std::string branchName = rTree.getBranchName(hInput);
			try
			{

				time_t date = std::stoll(branchName);
				_variant_t comment;
				SvXml::SVNavigateTree::GetItem(rTree, scCommentTag, hInput, comment);

				if (comment.vt == VT_BSTR)
				{
					historyVector.emplace_back(date, SvUl::createStdString(comment));
				}
			}
			catch (...)
			{
				Log_Assert(false);
			}

			hHistory = rTree.getNextBranch(hHistory, hInput);
		}
	} while (hHistory);
	return historyVector;
}
}

ModuleController::ModuleController()
	: m_xmlFilePath {SvStl::GlobalPath::Inst().GetTempPath() + _T("\\Module.xml")}
{
}

void ModuleController::loadModules(SVTreeType& rTree)
{
	m_moduleList.clear();
	SVTreeType::SVBranchHandle hModuleList(nullptr);
	bool bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_MODULE_LIST, nullptr, hModuleList);

	if (!bOk)
	{	//no module list found (normal for old configurations)
		return;
	}

	SVTreeType::SVBranchHandle hModule = rTree.getFirstBranch(hModuleList);

	while (nullptr != hModule)
	{
		std::string moduleName = rTree.getBranchName(hModule);
		
		SVGUID guid;
		std::string comment;
		_variant_t Value;
		if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, hModule, Value))
		{
			guid = SVGUID{Value};
		}
		else
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleParameterLoadFailed, {moduleName}, SvStl::SourceFileParams(StdMessageParams));
			throw msg;
		}

		if (SvXml::SVNavigateTree::GetItem(rTree, scCommentTag, hModule, Value))
		{
			comment = SvUl::createStdString(Value);
		}
		else
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleParameterLoadFailed, {moduleName}, SvStl::SourceFileParams(StdMessageParams));
			throw msg;
		}

		auto historyVector = readHistory(rTree, hModule);

		if (false == std::filesystem::exists(getModulePath(moduleName)))
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleFileLoadFailed, {getModulePath(moduleName)}, SvStl::SourceFileParams(StdMessageParams));
			throw msg;
		}
	
		m_moduleList.emplace_back(moduleName, std::move(historyVector), comment, guid);
		hModule = rTree.getNextBranch(hModuleList, hModule);
	}
}

void ModuleController::saveModules(SvOi::IObjectWriter& rWriter) const
{
	rWriter.StartElement(SvXml::CTAG_MODULE_LIST);

	for (const auto& rData : m_moduleList)
	{
		rWriter.StartElement(rData.m_name.c_str());
		_variant_t svValue {rData.m_guid.ToString().c_str()};
		rWriter.WriteAttribute(SvXml::CTAG_UNIQUE_REFERENCE_ID, svValue);
		svValue = rData.m_comment.c_str();
		rWriter.WriteAttribute(scCommentTag, svValue);
		rWriter.StartElement(scHistoryTag);
		for (const auto& rValue : rData.m_historyList)
		{
			rWriter.StartElement(std::to_string(rValue.first).c_str());
			rWriter.WriteAttribute(scCommentTag, rValue.second.c_str());
			rWriter.EndElement();
		}
		rWriter.EndElement();
		rWriter.EndElement(); //End of Module
	}
	
	rWriter.EndElement(); //End of Module List
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
			pModuleTool->moveObjectToThis(*pGroupTool);
			pModuleTool->setModuleComment(pGroupTool->getComment());
			pOwner->DestroyChildObject(pGroupTool, SvDef::SVMFSetDefaultInputs);
			pOwner->CreateChildObject(*pModuleTool, SvDef::SVMFSetDefaultInputs);
		}
		else
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NotCreated, SvStl::SourceFileParams(StdMessageParams), toolId);
			throw msg;
		}

		//Renew ModulGuid if already used
		auto guid = pModuleTool->getModuleGuid();
		while (std::ranges::any_of(m_moduleList, [guid](const auto& rEntry) {return rEntry.m_guid == guid; }))
		{
			guid = pModuleTool->renewModuleGuid();
		}

		createModuleLFile(*pModuleTool, rName, m_xmlFilePath, pOwner->GetObjectNameToObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType));

		m_moduleList.emplace_back(rName, pModuleTool->getHistory(), pModuleTool->getModuleComment(), pModuleTool->getModuleGuid(), std::vector<uint32_t>{pModuleTool->getObjectId()});
	}
	else
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ObjectNotFound, {std::to_string(toolId)}, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}
}

void ModuleController::convertModuleInstance(uint32_t toolId)
{
	auto* pModuleTool = dynamic_cast<SvTo::ModuleTool*>(SVObjectManagerClass::Instance().GetObject(toolId));
	if (nullptr != pModuleTool)
	{
		auto* pOwner = dynamic_cast<SvIe::SVTaskObjectListClass*>(pModuleTool->GetParent());
		if (nullptr == pOwner)
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidOwner, SvStl::SourceFileParams(StdMessageParams), toolId);
			throw msg;
		}

		//create GroupTool and delete ModuleTool
		auto* pGroupTool = new SvTo::GroupTool(pOwner);
		pOwner->InsertBefore(pModuleTool->getObjectId(), pGroupTool);
		if (pGroupTool && !pGroupTool->is_Created())
		{
			unregisterInstance(pModuleTool->getModuleGuid(), pModuleTool->getObjectId());
			pGroupTool->moveObjectToThis(*pModuleTool);
			pGroupTool->setComment(pModuleTool->getModuleComment());
			pOwner->DestroyChildObject(pModuleTool, SvDef::SVMFSetDefaultInputs);
			pOwner->CreateChildObject(*pGroupTool, SvDef::SVMFResetInspection);
		}
		else
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NotCreated, SvStl::SourceFileParams(StdMessageParams), toolId);
			throw msg;
		}
	}
	else
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ObjectNotFound, {std::to_string(toolId)}, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}
}

void ModuleController::registerInstance(SVGUID guid, uint32_t instanceId, const std::string& rComment, const SvOi::HistoryList& rHistoryList)
{
	auto iter = std::ranges::find_if(m_moduleList, [guid](const auto& rEntry) { return rEntry.m_guid == guid; });
	if (m_moduleList.end() != iter)
	{
		if (iter->m_comment != rComment)
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleCommentDifferent, SvStl::SourceFileParams(StdMessageParams), instanceId);
			throw msg;
		}

		if (iter->m_historyList != rHistoryList)
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleHistoryDifferent, SvStl::SourceFileParams(StdMessageParams), instanceId);
			throw msg;
		}

		if (iter->m_moduleInstanceIds.end() == std::ranges::find(iter->m_moduleInstanceIds, instanceId))
		{
			iter->m_moduleInstanceIds.push_back(instanceId);
		}
	}
	else
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleNotFound, SvStl::SourceFileParams(StdMessageParams), instanceId);
		throw msg;
	}
}

void ModuleController::unregisterInstance(SVGUID guid, uint32_t instanceId)
{
	auto iter = std::ranges::find_if(m_moduleList, [guid](const auto& rEntry) { return rEntry.m_guid == guid; });
	if (m_moduleList.end() != iter)
	{
		std::erase(iter->m_moduleInstanceIds, instanceId);
	}
}

SvOi::IObjectClass* ModuleController::constructAndAddModuleInstance(int index, uint32_t parentId)
{
	if (0 <= index && m_moduleList.size() > index && std::filesystem::exists(getModulePath(m_moduleList[index].m_name)))
	{
		SvDef::StringVector containedFilepaths;
		if (!SvUl::unzipAll(getModulePath(m_moduleList[index].m_name), SvStl::GlobalPath::Inst().GetTempPath(), containedFilepaths))
		{
			SvStl::MessageManager e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardUnzipFailed, SvStl::SourceFileParams(StdMessageParams));
			e.Throw();
		}
		SvFs::moveFilesToFolder(containedFilepaths, SvStl::GlobalPath::Inst().GetRunPath(), m_xmlFilePath);

		auto xmlData = SvFs::readContentFromFile(m_xmlFilePath);
		ToolClipboard toolClipboard;
		auto pastedToolIDs = toolClipboard.createToolsFromXmlData(xmlData, parentId, false, false);
		if (0 == pastedToolIDs.size())
		{
			SvStl::MessageManager e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ModuleInstanceConstructFailed, SvStl::SourceFileParams(StdMessageParams));
			e.Throw();
		}

		auto* pGroupTool = dynamic_cast<SvTo::GroupTool*>(SvOi::getObject(pastedToolIDs[0]));
		if (pGroupTool)
		{
			pGroupTool->setInputsToDefault();
		}
		return pGroupTool;
	}
	SvStl::MessageManager e(SvStl::MsgType::Data);
	e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ModuleNotFound, SvStl::SourceFileParams(StdMessageParams));
	e.Throw();
}

SvPb::InspectionCmdResponse ModuleController::getModuleListResp()
{
	SvPb::InspectionCmdResponse cmdResp;
	auto* pResp = cmdResp.mutable_getmodulelistresponse();
	for (const auto& rEntry : m_moduleList)
	{
		auto* pModule = pResp->add_list();
		pModule->set_guid(rEntry.m_guid.ToString());
		pModule->set_name(rEntry.m_name);
		for (const auto& rHist : rEntry.m_historyList)
		{
			auto* pHist = pModule->add_historylist();
			pHist->set_time(rHist.first);
			pHist->set_comment(rHist.second);
		}
		pModule->set_comment(rEntry.m_comment);
		pModule->set_number_of_instances(static_cast<int>(rEntry.m_moduleInstanceIds.size()));
	}
	return cmdResp;
}

SvPb::InspectionCmdResponse ModuleController::deleteModule(SVGUID guid)
{
	SvPb::InspectionCmdResponse cmdResp;
	auto iter = std::ranges::find_if(m_moduleList, [guid](const auto& rEntry) { return rEntry.m_guid == guid; });
	if (m_moduleList.end() != iter)
	{
		if (0 == iter->m_moduleInstanceIds.size())
		{
			const auto& moduleFilePath = getModulePath(iter->m_name);
			::DeleteFile(moduleFilePath.c_str());
			RemoveFileFromConfig(moduleFilePath.c_str());
			m_moduleList.erase(iter);
		}
		else
		{
			cmdResp.set_hresult(E_FAIL);
			cmdResp.mutable_errormessage()->CopyFrom(SvPb::createErrorMessages(SvDef::InvalidObjectId, SvStl::SourceFileParams(StdMessageParams), SvStl::Tid_DeleteFailedModulesHasInstance));
		}
	}
	else
	{
		cmdResp.set_hresult(E_FAIL);
		cmdResp.mutable_errormessage()->CopyFrom(SvPb::createErrorMessages(SvDef::InvalidObjectId, SvStl::SourceFileParams(StdMessageParams), SvStl::Tid_ModuleNotFound));
	}
	
	return cmdResp;
}

std::string ModuleController::getModuleName(SVGUID guid)
{
	SvPb::InspectionCmdResponse cmdResp;
	auto iter = std::ranges::find_if(m_moduleList, [guid](const auto& rEntry) { return rEntry.m_guid == guid; });
	if (m_moduleList.end() != iter)
	{
		return iter->m_name;
	}
	return {};
}