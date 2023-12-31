//*****************************************************************************
/// \copyright (c) 2022,2022 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "ToolExportImport.h"
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
#include "SVXMLLibrary/SaxXMLHandler.h"
#include "Definitions/TextDefinesSvDef.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion Includes

namespace
{
std::string getModulePath(const std::string& rModuleName)
{
	return SvStl::GlobalPath::Inst().GetRunPath((rModuleName + SvO::ModuleExtension).c_str());
}

std::string getModuleTempPath(const std::string& rModuleName)
{
	return SvStl::GlobalPath::Inst().GetTempPath((rModuleName + SvO::ModuleExtension).c_str());
}

void writeModule(SvOi::IObjectWriter& rWriter, const ModuleData& rData)
{
	rWriter.StartElement(rData.m_name.c_str());
	_variant_t svValue {rData.m_guid.ToString().c_str()};
	rWriter.WriteAttribute(SvXml::CTAG_UNIQUE_REFERENCE_ID, svValue);
	std::string temp = rData.m_comment;
	SvUl::AddEscapeSpecialCharacters(temp, true);
	svValue.SetString(temp.c_str());
	rWriter.WriteAttribute(scCommentTag, svValue);
	rWriter.StartElement(scHistoryTag);
	for (const auto& rValue : rData.m_historyList)
	{
		rWriter.StartElement(std::to_string(rValue.first).c_str());
		temp = rValue.second;
		SvUl::AddEscapeSpecialCharacters(temp, true);
		svValue.SetString(temp.c_str());
		rWriter.WriteAttribute(scCommentTag, svValue);
		rWriter.EndElement();
	}
	rWriter.EndElement();
	rWriter.EndElement(); //End of Module
}

void createModuleFile(SvTo::ModuleTool& rModuleTool, const std::string& rModuleName, const std::string& rXmlFileName)
{
	//write Module to file
	std::string oldName = rModuleTool.GetName();
	rModuleTool.SetName(rModuleName.c_str()); //Set name to Module name to have the name in xmlFile
	if (nullptr != rModuleTool.GetInspection())
	{
		rModuleTool.GetInspection()->OnObjectRenamed(rModuleTool, oldName);
	}

	ToolExportImport toolExport;
	const auto moduleFilePath = getModulePath(rModuleName);
	toolExport.exportTools({rModuleTool.getObjectId()}, moduleFilePath.c_str(), rXmlFileName.c_str(), false);
	rModuleTool.SetName(oldName.c_str());
	if (nullptr != rModuleTool.GetInspection())
	{
		rModuleTool.GetInspection()->OnObjectRenamed(rModuleTool, rModuleName);
	}

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
		if (hInput)
		{
			std::string branchName = rTree.getBranchName(hInput);
			try
			{

				time_t date = std::stoll(branchName);
				_variant_t comment;
				SvXml::SVNavigateTree::GetItem(rTree, scCommentTag, hInput, comment);

				if (comment.vt == VT_BSTR)
				{
					auto commentStr = SvUl::createStdString(comment);
					SvUl::RemoveEscapedSpecialCharacters(commentStr, true);
					historyVector.emplace_back(date, commentStr);
				}
			}
			catch (...)
			{
				Log_Assert(false);
			}

			hInput = rTree.getNextBranch(hHistory, hInput);
		}
	} while (hInput);
	return historyVector;
}

std::string readModuleComment(SVTreeType& rTree, SVTreeType::SVBranchHandle hModule)
{
	SVTreeType::SVBranchHandle hEmbedded(nullptr);
	SvXml::SVNavigateTree::GetItemBranch(rTree, scEmbeddedsTag, hModule, hEmbedded);
	if (nullptr == hEmbedded)
	{
		return {};
	}

	SVTreeType::SVBranchHandle hComment(nullptr);
	SvXml::SVNavigateTree::GetItemBranch(rTree, scModuleCommentTag, hEmbedded, hComment);
	if (nullptr == hComment)
	{
		return {};
	}

	SVTreeType::SVBranchHandle hArray(nullptr);
	SvXml::SVNavigateTree::GetItemBranch(rTree, SvDef::cArrayTag, hComment, hArray);
	if (nullptr == hArray)
	{
		return {};
	}
	
	_variant_t comment;
	SvXml::SVNavigateTree::GetItem(rTree, scElementTag, hArray, comment);
	auto commentStr = SvUl::createStdString(comment);
	SvUl::RemoveEscapedSpecialCharacters(commentStr, true);
	return commentStr;
}

void changeModuleName(std::string& xmlData, const std::string& newName)
{
	SvXml::SVXMLMaterialsTree rTree;
	SvXml::convertXmlToTree(xmlData, rTree);
	SvXml::SVXMLMaterialsTree::SVBranchHandle ToolsItem(nullptr);
	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::ToolsTag, nullptr, ToolsItem))
	{
		SvXml::SVXMLMaterialsTree::SVBranchHandle ToolItem = rTree.getFirstBranch(ToolsItem);

		int toolIndex = 0;
		while (rTree.isValidBranch(ToolItem))
		{
			//Get names from tree
			std::string fullToolNameTag {std::format(SvXml::FullToolNameTag, toolIndex)};
			SvXml::SVXMLMaterialsTree::SVLeafHandle fullToolNameHandle;
			SvXml::SVNavigateTree::GetItemLeaf(rTree, fullToolNameTag.c_str(), ToolsItem, fullToolNameHandle);
			variant_t value;
			rTree.getLeafData(fullToolNameHandle, value);
			_variant_t ObjectName;
			SvXml::SVNavigateTree::GetItem(rTree, scObjectNameTag, ToolItem, ObjectName);
			auto oldToolName = SvUl::createStdString(ObjectName);
			auto fullOldToolName = SvUl::createStdString(value);
			auto fullNewToolName = fullOldToolName;
			auto pos = fullNewToolName.rfind(oldToolName);
			fullNewToolName.replace(pos, oldToolName.size(), newName);

			replaceToolName(xmlData, oldToolName, newName, fullOldToolName, fullNewToolName);

			//rename FullToolName part in XML.
			constexpr auto FullNameFormatStr = _T("<DATA Name=\"FullToolName{}\" Type=\"VT_BSTR\">{}</DATA>");
			SvUl::searchAndReplace(xmlData, std::format(FullNameFormatStr, toolIndex, fullOldToolName).c_str(), std::format(FullNameFormatStr, toolIndex, fullNewToolName).c_str());

			ToolItem = rTree.getNextBranch(ToolsItem, ToolItem);
			++toolIndex;
		}
	}
}

ModuleData getModuleData(const auto& rXMLString)
{
	ModuleData retVal;
	SvXml::SVXMLMaterialsTree tree;
	SvXml::convertXmlToTree(rXMLString, tree);
	SvXml::SVXMLMaterialsTree::SVBranchHandle ToolsItem(nullptr);
	if (SvXml::SVNavigateTree::GetItemBranch(tree, SvXml::ToolsTag, nullptr, ToolsItem))
	{
		SvXml::SVXMLMaterialsTree::SVBranchHandle ToolItem = tree.getFirstBranch(ToolsItem);
		if (tree.isValidBranch(ToolItem))
		{
			//Get names from tree
			std::string fullToolNameTag {std::format(SvXml::FullToolNameTag, 0)};
			SvXml::SVXMLMaterialsTree::SVLeafHandle fullToolNameHandle;
			SvXml::SVNavigateTree::GetItemLeaf(tree, fullToolNameTag.c_str(), ToolsItem, fullToolNameHandle);
			variant_t value;
			SvXml::SVNavigateTree::GetItem(tree, scObjectNameTag, ToolItem, value);
			retVal.m_name = SvUl::createStdString(value);
			SvXml::SVNavigateTree::GetItem(tree, scModuleGuidTag, ToolItem, value);
			retVal.m_guid = SVGUID {SvUl::createStdString(value)};

			retVal.m_historyList = readHistory(tree, ToolItem);
			retVal.m_comment = readModuleComment(tree, ToolItem);
		}
		else
		{
			SvStl::MessageManager e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolsUnzipFailed, SvStl::SourceFileParams(StdMessageParams));
			e.Throw();
		}
	}
	else
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolsUnzipFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}
	return retVal;
}

ModuleData getModuleData(SVTreeType& rTree, SVTreeType::SVBranchHandle hModule)
{
	ModuleData data;
	data.m_name = rTree.getBranchName(hModule);

	_variant_t Value;
	if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, hModule, Value))
	{
		data.m_guid = SVGUID {Value};
	}
	else
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleParameterLoadFailed, {data.m_name}, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}

	if (SvXml::SVNavigateTree::GetItem(rTree, scCommentTag, hModule, Value))
	{
		data.m_comment = SvUl::createStdString(Value);
		SvUl::RemoveEscapedSpecialCharacters(data.m_comment, true);
	}
	else
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleParameterLoadFailed, {data.m_name}, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}

	data.m_historyList = readHistory(rTree, hModule);
	return data;
}

SvOi::IObjectClass* createModuleFromFile(const std::string& rName, const std::string& rXmlFilePath, uint32_t parentId)
{
	SvDef::StringVector containedFilepaths;
	if (!SvUl::unzipAll(getModulePath(rName), SvStl::GlobalPath::Inst().GetTempPath(), containedFilepaths))
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolsUnzipFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}
	SvFs::moveFilesToFolder(containedFilepaths, SvStl::GlobalPath::Inst().GetRunPath(), rXmlFilePath);

	auto xmlData = SvFs::readContentFromFile(rXmlFilePath);
	ToolExportImport toolClipboard;
	auto pastedToolIDs = toolClipboard.createToolsFromXmlData(xmlData, parentId, false, false);
	if (0 == pastedToolIDs.size())
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ModuleInstanceConstructFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}

	return SvOi::getObject(pastedToolIDs[0]);
}

SvTo::ModuleTool* switchModuleToAnotherVersion(const std::string& name, const std::string& rXmlFilePath, SvTo::ModuleTool& rModuleTool)
{
	auto* pNewModule = dynamic_cast<SvTo::ModuleTool*>(createModuleFromFile(name, rXmlFilePath, rModuleTool.GetParentID()));
	auto* pParentTaskObjectList = dynamic_cast<SvIe::SVTaskObjectListClass*>(rModuleTool.GetParent());
	if (nullptr == pParentTaskObjectList || nullptr == pNewModule)
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateSFailed, {std::to_string(rModuleTool.getObjectId())}, SvStl::SourceFileParams(StdMessageParams), rModuleTool.getObjectId());
		throw msg;
	}

	pParentTaskObjectList->moveTaskObject(pNewModule->getObjectId(), rModuleTool.getObjectId());
	pNewModule->moveObjectToThis(rModuleTool, false);

	if (!pNewModule->is_Created())
	{
		// And finally try to create the child object...
		if (!dynamic_cast<SvOi::IObjectAppClass*>(pParentTaskObjectList)->CreateChildObject(*pNewModule, SvDef::SVMFResetObject))
		{
			pParentTaskObjectList->Delete(pNewModule);

			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateSFailed, {std::to_string(rModuleTool.getObjectId())}, SvStl::SourceFileParams(StdMessageParams), rModuleTool.getObjectId());
			throw msg;
		}
	}
	pNewModule->finishAddingAndMovingModule(rModuleTool);
	pParentTaskObjectList->DestroyChildObject(&rModuleTool, SvDef::SVMFSetDefaultInputs);
	return pNewModule;
}
}

ModuleController::ModuleController()
	: m_xmlFilePath {SvStl::GlobalPath::Inst().GetTempPath() + _T("\\") + std::string{SvO::ModuleName} + SvO::XmlExtension}
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
		auto data = getModuleData(rTree, hModule);


		if (false == std::filesystem::exists(getModulePath(data.m_name)))
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleFileLoadFailed, {getModulePath(data.m_name)}, SvStl::SourceFileParams(StdMessageParams));
			throw msg;
		}

		m_moduleList.emplace_back(data);
		hModule = rTree.getNextBranch(hModuleList, hModule);
	}
}


void ModuleController::addModules(SVTreeType& rTree, const std::string& zipFilename)
{
	SVTreeType::SVBranchHandle hModuleList(nullptr);
	bool bOk = SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_MODULE_LIST, nullptr, hModuleList);

	if (!bOk)
	{	//no module list found (normal for old configurations)
		return;
	}

	SvDef::StringVector Files;
	if (false == SvUl::unzipAll(zipFilename, SvStl::GlobalPath::Inst().GetTempPath(), Files))
	{
		SvStl::MessageContainer msg(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolsUnzipFailed, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}

	auto oldModuleList = m_moduleList;

	try
	{
		SVTreeType::SVBranchHandle hModule = rTree.getFirstBranch(hModuleList);
		while (nullptr != hModule)
		{
			auto data = getModuleData(rTree, hModule);

			auto modulefilename = getModuleTempPath(data.m_name);

			auto iter = std::ranges::find_if(m_moduleList, [data](const auto& rEntry) { return data.m_guid == rEntry.m_guid; });
			if (m_moduleList.end() != iter)
			{
				//By now if guid exist, module exist, no editing needed. If modules editable, here is a check needed if version is equal.
				::DeleteFile(modulefilename.c_str());
				std::erase(Files, modulefilename);

				hModule = rTree.getNextBranch(hModuleList, hModule);
				continue;
			}

			if (false == std::filesystem::exists(modulefilename))
			{
				SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleFileLoadFailed, {data.m_name}, SvStl::SourceFileParams(StdMessageParams));
				throw msg;
			}

			iter = std::ranges::find_if(m_moduleList, [data](const auto& rEntry) { return data.m_name == rEntry.m_name; });
			if (m_moduleList.end() != iter)
			{
				//Name already used, rename new module.
				int i = 1;
				do
				{
					data.m_name = iter->m_name + std::to_string(i++);
				} while (std::ranges::any_of(m_moduleList, [data](const auto& rEntry) { return data.m_name == rEntry.m_name; }));
				const auto oldName = getModuleTempPath(iter->m_name);
				const auto newName = getModuleTempPath(data.m_name);
				rename(oldName.c_str(), newName.c_str());
				auto fileIter = std::ranges::find(Files, oldName);
				if (Files.end() != fileIter)
				{
					*fileIter = newName;
				}
			}

			AddFileToConfig(getModulePath(data.m_name).c_str());
			m_moduleList.emplace_back(data);
			hModule = rTree.getNextBranch(hModuleList, hModule);
		}
	}
	catch (...)
	{
		m_moduleList = oldModuleList;
		for (const auto rEntry : Files)
		{
			::DeleteFile(rEntry.c_str());
		}
		throw;
	}

	::DeleteFile(zipFilename.c_str());
	SvUl::makeZipFile(zipFilename, Files, _T(""), true);
}

void ModuleController::saveModules(SvOi::IObjectWriter& rWriter) const
{
	rWriter.StartElement(SvXml::CTAG_MODULE_LIST);

	for (const auto& rData : m_moduleList)
	{
		writeModule(rWriter, rData);
	}
	
	rWriter.EndElement(); //End of Module List
}

void ModuleController::saveModulesForIP(SvOi::IObjectWriter& rWriter, uint32_t inspectionId) const
{
	rWriter.StartElement(SvXml::CTAG_MODULE_LIST);

	for (const auto& rData : m_moduleList)
	{
		bool isUsedInIp = std::ranges::any_of(rData.m_moduleInstanceIds, [inspectionId](uint32_t toolId) 
		{
			auto* pGroupTool = dynamic_cast<SvTo::GroupTool*>(SVObjectManagerClass::Instance().GetObject(toolId));
			if (pGroupTool)
			{
				return (nullptr != pGroupTool->GetInspection() && inspectionId == pGroupTool->GetInspection()->getObjectId());
			}
			return false;
		});
		if (isUsedInIp)
		{
			writeModule(rWriter, rData);
		}
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

		std::vector<SvOi::IObjectClass*> list;
		pGroupTool->fillObjectList(std::back_inserter(list), SvDef::SVObjectTypeInfoStruct{SvPb::SVToolObjectType, SvPb::ModuleToolObjectType}, true);
		if (false == list.empty())
		{
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleToolInModuleInvalid, SvStl::SourceFileParams(StdMessageParams), toolId);
			throw msg;
		}

		//create ModulTool and delete GroupTool
		auto* pModuleTool = new SvTo::ModuleTool(pOwner);
		pOwner->InsertBefore(pGroupTool->getObjectId(), pModuleTool);
		if (pModuleTool && !pModuleTool->is_Created())
		{
			pOwner->CreateChildObject(*pModuleTool, SvDef::SVMFSetDefaultInputs);
			pModuleTool->moveObjectToThis(*pGroupTool);
			pModuleTool->setModuleComment(pGroupTool->getComment());
			pOwner->DestroyChildObject(pGroupTool, SvDef::SVMFSetDefaultInputs);
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

		createModuleFile(*pModuleTool, rName, m_xmlFilePath);

		m_moduleList.emplace_back(rName, pModuleTool->getHistory(), pModuleTool->getModuleComment(), pModuleTool->getModuleGuid(), std::vector<uint32_t>{pModuleTool->getObjectId()});

		if (pModuleTool->GetInspection())
		{
			//This is needed that the images is set correctly in TRC
			pModuleTool->GetInspection()->resetAllObjects();
			dynamic_cast<SVInspectionProcess*>(pModuleTool->GetInspection())->RunOnce(); //This is needed to update the overlays
		}
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
			pOwner->CreateChildObject(*pGroupTool, SvDef::SVMFSetDefaultInputs);
			unregisterInstance(pModuleTool->getModuleGuid(), pModuleTool->getObjectId());
			pGroupTool->moveObjectToThis(*pModuleTool);
			pGroupTool->setComment(pModuleTool->getModuleComment());
			pOwner->DestroyChildObject(pModuleTool, SvDef::SVMFResetInspection);
			dynamic_cast<SVInspectionProcess*>(pOwner->GetInspection())->RunOnce(); //This is needed to update the overlays
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

void ModuleController::startEditModule(uint32_t toolId)
{
	auto* pModuleTool = dynamic_cast<SvTo::ModuleTool*>(SVObjectManagerClass::Instance().GetObject(toolId));
	if (nullptr != pModuleTool)
	{
		pModuleTool->setEditFlag(true);
		if (auto* pInsp = pModuleTool->GetInspection(); pInsp)
		{
			pInsp->resetAllObjects();
		}
	}
	else
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ObjectNotFound, {std::to_string(toolId)}, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}
}

void ModuleController::saveEditModule(uint32_t toolId, const std::string& rChangeText)
{
	auto* pModuleTool = dynamic_cast<SvTo::ModuleTool*>(SVObjectManagerClass::Instance().GetObject(toolId));
	if (nullptr != pModuleTool)
	{
		pModuleTool->setEditFlag(false);
		auto guid = pModuleTool->getModuleGuid();
		auto iter = std::ranges::find_if(m_moduleList, [guid](const auto& rEntry) { return rEntry.m_guid == guid; });
		if (m_moduleList.end() != iter)
		{
			iter->m_historyList.emplace_back(std::time(nullptr), rChangeText);
			iter->m_comment = pModuleTool->getModuleComment();
			pModuleTool->setHistory(iter->m_historyList);
			createModuleFile(*pModuleTool, iter->m_name, m_xmlFilePath);
			auto list = iter->m_moduleInstanceIds;
			std::erase(list, toolId);
			switchListOfModuleInstancesToAnotherVersion(list, iter->m_name);
		}		
	}
	else
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ObjectNotFound, {std::to_string(toolId)}, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}
}

void ModuleController::cancelEditModule(uint32_t toolId)
{
	auto* pModuleTool = dynamic_cast<SvTo::ModuleTool*>(SVObjectManagerClass::Instance().GetObject(toolId));
	if (nullptr != pModuleTool)
	{
		auto* pInsp = pModuleTool->GetInspection();
		pModuleTool->setEditFlag(false);
		auto guid = pModuleTool->getModuleGuid();
		auto iter = std::ranges::find_if(m_moduleList, [guid](const auto& rEntry) { return rEntry.m_guid == guid; });
		if (m_moduleList.end() != iter)
		{
			auto oldComment = pModuleTool->getComment();
			pModuleTool = switchModuleToAnotherVersion(iter->m_name, m_xmlFilePath, *pModuleTool);
			Log_Assert(pModuleTool);
			if (pModuleTool)
			{
				pModuleTool->setComment(oldComment);
			}
			if (pInsp)
			{
				//This is needed that the images is set correctly in TRC
				pInsp->resetAllObjects();
				pInsp->resetAllObjects();
				dynamic_cast<SVInspectionProcess*>(pInsp)->RunOnce(); //This is needed to update the overlays
			}
		}
	}
	else
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ObjectNotFound, {std::to_string(toolId)}, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}
}

void ModuleController::registerInstance(SVGUID guid, uint32_t instanceId, const std::string& rComment, const SvOi::HistoryList& rHistoryList, bool checkComment)
{
	auto iter = std::ranges::find_if(m_moduleList, [guid](const auto& rEntry) { return rEntry.m_guid == guid; });
	if (m_moduleList.end() != iter)
	{
		if (checkComment && iter->m_comment != rComment)
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
		return createModuleFromFile(m_moduleList[index].m_name, m_xmlFilePath, parentId);
	}

	SvStl::MessageManager e(SvStl::MsgType::Data);
	e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ModuleNotFound, SvStl::SourceFileParams(StdMessageParams));
	e.Throw();
}

SvPb::InspectionCmdResponse ModuleController::getModuleListResp() const
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

void ModuleController::deleteModule(SVGUID guid)
{
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
			SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DeleteFailedModulesHasInstance, SvStl::SourceFileParams(StdMessageParams));
			throw msg;
		}
	}
	else
	{
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleNotFound, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}
}


void ModuleController::renameModule(SVGUID guid, const std::string& newName)
{
	SvPb::InspectionCmdResponse cmdResp;
	auto iter = std::ranges::find_if(m_moduleList, [guid](const auto& rEntry) { return rEntry.m_guid == guid; });
	if (m_moduleList.end() != iter)
	{
		if (iter->m_name == newName)
		{
			return;
		}

		checkIfNameValid(newName);

		if (std::filesystem::exists(getModulePath(iter->m_name)))
		{
			SvDef::StringVector containedFilepaths;
			if (!SvUl::unzipAll(getModulePath(iter->m_name), SvStl::GlobalPath::Inst().GetTempPath(), containedFilepaths))
			{
				SvStl::MessageManager e(SvStl::MsgType::Data);
				e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolsUnzipFailed, SvStl::SourceFileParams(StdMessageParams));
				e.Throw();
			}

			auto xmlData = SvFs::readContentFromFile(m_xmlFilePath);
			changeModuleName(xmlData, newName);

			SvFs::writeStringToFile(m_xmlFilePath, xmlData, true);
			const auto moduleFilePath = getModulePath(newName);
			if (!SvUl::makeZipFile(moduleFilePath, containedFilepaths, _T(""), false))
			{
				SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ToolsZipFailed, SvStl::SourceFileParams(StdMessageParams));
				throw msg;
			}
			::DeleteFile(getModulePath(iter->m_name).c_str());
			AddFileToConfig(moduleFilePath.c_str());
			RemoveFileFromConfig(getModulePath(iter->m_name).c_str());

			iter->m_name = newName;
			return;
		}
		
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ModuleNotFound, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}
	SvStl::MessageManager e(SvStl::MsgType::Data);
	e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleNotFound, SvStl::SourceFileParams(StdMessageParams));
	e.Throw();
}

std::string ModuleController::getModuleName(SVGUID guid) const
{
	SvPb::InspectionCmdResponse cmdResp;
	auto iter = std::ranges::find_if(m_moduleList, [guid](const auto& rEntry) { return rEntry.m_guid == guid; });
	if (m_moduleList.end() != iter)
	{
		return iter->m_name;
	}
	return {};
}

SvPb::InspectionCmdResponse ModuleController::importModule(std::string moduleName, const std::string& moduleContainerStr, bool overwriteAllowed)
{
	SvPb::InspectionCmdResponse cmd;
	auto tmpName = SvStl::GlobalPath::Inst().GetTempPath("Temp.svm");
	::DeleteFile(tmpName.c_str());
	SvFs::writeStringToFile(tmpName, moduleContainerStr, false);
	SvDef::StringVector containedFilepaths;
	if (!SvUl::unzipAll(tmpName, SvStl::GlobalPath::Inst().GetTempPath(), containedFilepaths))
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolsUnzipFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}

	auto xmlData = SvFs::readContentFromFile(m_xmlFilePath, false);

	bool overwriteModule = false;
	ModuleData data = getModuleData(xmlData);
	cmd.mutable_importmoduleresponse()->set_name(moduleName);
	cmd.mutable_importmoduleresponse()->set_guid(data.m_guid.ToString());
	auto iter = std::ranges::find_if(m_moduleList, [data](const auto& rEntry) { return rEntry.m_guid == data.m_guid; });
	if (m_moduleList.end() != iter)
	{
		if (iter->m_historyList != data.m_historyList && overwriteAllowed)
		{
			overwriteModule = true;
			moduleName = iter->m_name;
			data.m_moduleInstanceIds = iter->m_moduleInstanceIds;
		}
		else
		{
			for (const auto& rEntry : containedFilepaths)
			{
				::DeleteFile(rEntry.c_str());
			}
			::DeleteFile(tmpName.c_str());
			if (iter->m_historyList == data.m_historyList)
			{
				return {};
			}

			
			cmd.set_hresult(E_FAIL);
			auto* pExistingMsg = cmd.mutable_importmoduleresponse()->mutable_existing();
			pExistingMsg->set_name(iter->m_name);
			for (const auto& rHist : iter->m_historyList)
			{
				auto* pHist = pExistingMsg->add_oldhistory();
				pHist->set_time(rHist.first);
				pHist->set_comment(rHist.second);
			}
			for (const auto& rHist : data.m_historyList)
			{
				auto* pHist = pExistingMsg->add_newhistory();
				pHist->set_time(rHist.first);
				pHist->set_comment(rHist.second);
			}
			return cmd;
		}
	}
	else
	{
		try
		{
			checkIfNameValid(moduleName);
		}
		catch (const SvStl::MessageContainer& rExp)
		{
			for (const auto& rEntry : containedFilepaths)
			{
				::DeleteFile(rEntry.c_str());
			}
			::DeleteFile(tmpName.c_str());
			SvPb::convertMessageToProtobuf(rExp, cmd.mutable_importmoduleresponse()->mutable_invalidname());
			cmd.set_hresult(E_FAIL);
			return cmd;
		}
	}

	if (moduleName != data.m_name)
	{
		changeModuleName(xmlData, moduleName);

		SvFs::writeStringToFile(m_xmlFilePath, xmlData, true);
		data.m_name = moduleName;
	}

	::DeleteFile(tmpName.c_str());
	const auto moduleFilePath = getModulePath(moduleName);
	if (overwriteModule)
	{
		::DeleteFile(moduleFilePath.c_str());
	}

	if (!SvUl::makeZipFile(moduleFilePath, containedFilepaths, _T(""), true))
	{
		for (const auto& rEntry : containedFilepaths)
		{
			::DeleteFile(rEntry.c_str());
		}
		SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ToolsZipFailed, SvStl::SourceFileParams(StdMessageParams));
		throw msg;
	}

	AddFileToConfig(moduleFilePath.c_str());
	if (overwriteModule)
	{
		*iter = data;
		switchListOfModuleInstancesToAnotherVersion(data.m_moduleInstanceIds, moduleName);
	}
	else
	{
		m_moduleList.push_back(std::move(data));
	}
	return cmd;
}

SvPb::InspectionCmdResponse ModuleController::exportModule(SVGUID moduleGuid) const
{
	auto iter = std::ranges::find_if(m_moduleList, [moduleGuid](const auto& rEntry) { return rEntry.m_guid == moduleGuid; });
	if (m_moduleList.end() != iter && std::filesystem::exists(getModulePath(iter->m_name)))
	{
		SvPb::InspectionCmdResponse retValue;
		auto* pCmd = retValue.mutable_exportmoduleresponse();
		pCmd->set_datastring(SvFs::readContentFromFile(getModulePath(iter->m_name), false));
		return retValue;
	}
	
	SvStl::MessageContainer msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ModuleNotFound, SvStl::SourceFileParams(StdMessageParams));
	throw msg;
}

void ModuleController::updateModuleInstances(std::vector<uint32_t>& rListOfNewTools, const std::vector<SVGUID>& rModules)
{
	for (auto& id : rListOfNewTools)
	{
		auto* pModuleTool = dynamic_cast<SvTo::ModuleTool*>(SVObjectManagerClass::Instance().GetObject(id));
		if (nullptr != pModuleTool)
		{
			auto guid = pModuleTool->getModuleGuid();
			auto oldComment = pModuleTool->getComment();
			if (std::ranges::any_of(rModules, [guid](const auto& rEntry) { return guid == rEntry; }))
			{
				auto iter = std::ranges::find_if(m_moduleList, [guid](const auto& rEntry) { return rEntry.m_guid == guid; });
				if (m_moduleList.end() != iter)
				{
					pModuleTool = switchModuleToAnotherVersion(iter->m_name, m_xmlFilePath, *pModuleTool);
					if (pModuleTool)
					{
						id = pModuleTool->getObjectId();
						pModuleTool->setComment(oldComment);
					}
				}
				else
				{
					//This case should never happen: The module must be there - otherwise it would not be rejected.
					Log_Assert(false);
				}
			}
		}
	}
}

void ModuleController::switchListOfModuleInstancesToAnotherVersion(const std::vector<uint32_t>& rList, const std::string& rModuleName)
{
	std::set<SVInspectionProcess*> resetIPList;
	for (auto id : rList)
	{
		auto* pModuleTool2 = dynamic_cast<SvTo::ModuleTool*>(SVObjectManagerClass::Instance().GetObject(id));
		if (nullptr != pModuleTool2)
		{
			auto oldComment = pModuleTool2->getComment();
			if (auto* pInsp = dynamic_cast<SVInspectionProcess*>(pModuleTool2->GetInspection()); pInsp)
			{
				resetIPList.insert(pInsp);
			}
			pModuleTool2 = switchModuleToAnotherVersion(rModuleName, m_xmlFilePath, *pModuleTool2);
			Log_Assert(pModuleTool2);
			if (pModuleTool2)
			{
				pModuleTool2->setComment(oldComment);
			}
		}
		else
		{
			Log_Assert(false);
		}
	}

	for (auto* pInsp : resetIPList)
	{
		//This is needed to ensure that the images are set correctly in the TRC
		pInsp->resetAllObjects();
		pInsp->resetAllObjects();
		pInsp->RunOnce(); //This is needed to update the overlays
	}
}