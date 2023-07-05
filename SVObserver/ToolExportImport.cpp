//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Tool export import
//* .File Name       : $Workfile:   ToolExportImport.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 Jan 2015 10:38:46  $
//* ----------------------------------------------------------------------------
//* This class is used to write and read the selected tool to and from the clipboard
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
#include "ToolExportImport.h"

#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVInspectionTreeParser.h"
#include "SVObjectScriptParser.h"
#include "SVObserver.h"
#include "SVParserProgressDialog.h"
#include "TextDefinesSvO.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionEngine/SVTaskObjectList.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "FilesystemUtilities/FilepathUtilities.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "SVXMLLibrary/SaxXMLHandler.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVObjectXMLWriter.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "Tools/ModuleTool.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma endregion Declarations

namespace
{
void checkToolDepth(SvXml::SVXMLMaterialsTree& rTree, uint32_t ownerId)
{
	SvXml::SVXMLMaterialsTree::SVLeafHandle ToolDepthItem;
	if (SvXml::SVNavigateTree::GetItemLeaf(rTree, SvXml::ToolDepthTag, nullptr, ToolDepthItem))
	{
		variant_t value;
		rTree.getLeafData(ToolDepthItem, value);
		int pastDepth = value;
		auto* pOwner = dynamic_cast<SvIe::SVTaskObjectListClass*>(SVObjectManagerClass::Instance().GetObject(ownerId));
		if (nullptr != pOwner)
		{
			int currentDepth = pOwner->getToolDepth(true);
			if (SvDef::c_maxLoopGroupDepth < pastDepth + currentDepth)
			{
				SvDef::StringVector messageList;
				messageList.push_back(std::to_string(pastDepth + currentDepth));
				messageList.push_back(std::to_string(SvDef::c_maxLoopGroupDepth));
				SvStl::MessageManager e(SvStl::MsgType::Data);
				e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_LoopToolInsertLoopToolFailed, messageList, SvStl::SourceFileParams(StdMessageParams));
				e.Throw();
			}
		}
	}
}

void writeToolParameter(SvOi::IObjectWriter& rWriter, SvTo::SVToolClass& rTool, int pos)
{
	std::string toolTag {std::format(SvXml::FullToolNameTag, pos)};
	std::string tmpString = rTool.GetObjectNameToObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType);
	_variant_t Value = tmpString.c_str();
	rWriter.WriteAttribute(toolTag.c_str(), Value);
}

///  This method finds the dependency files in the tool Xml string
/// \param rToolXmlString [in] Reference to the tool XML string to search
/// \param inserter SvDef::StringVector vector of dependency filepaths
void findDependencyFiles(const std::string& rToolXmlString, std::back_insert_iterator<SvDef::StringVector> inserter)
{
	size_t StartPos(0);
	size_t EndPos(0);
	std::string SearchString(SvStl::GlobalPath::Inst().GetRunPath().c_str());
	SearchString += _T("\\");

	std::string ToolXmlString(rToolXmlString); // working on copy of the xml string keeps changes local to this function
	StartPos = ToolXmlString.find("\\\\");
	while (std::string::npos != StartPos)
	{
		ToolXmlString.replace(StartPos, sizeof("\\\\") - 1, "\\"); // this makes the search string work 
			// regardless of whether single or double backslashes are present in file paths 
			// and ensures dependency filepaths contain single backslashes
			// (fixes SVO-3548)
		StartPos = ToolXmlString.find("\\\\");
	}

	StartPos = ToolXmlString.find(SearchString.c_str(), EndPos);

	while (std::string::npos != StartPos)
	{
		EndPos = ToolXmlString.find(SvXml::DataTag, StartPos);
		if (std::string::npos != EndPos)
		{
			inserter = ToolXmlString.substr(StartPos, EndPos - StartPos);
			StartPos = ToolXmlString.find(SearchString.c_str(), EndPos);
		}
		else
		{
			StartPos = std::string::npos;
		}
	}
}

uint32_t parseOneToolFromTree(SvXml::SVXMLMaterialsTree& rTree, SVObjectClass& rOwner, SvXml::SVXMLMaterialsTree::SVBranchHandle ToolItem)
{
	_variant_t UniqueID;

	SvXml::SVNavigateTree::GetItem(rTree, scUniqueReferenceIDTag, ToolItem, UniqueID);

	SVParserProgressDialog ParserProgressDialog(SvO::InsertingTool, AfxGetMainWnd());
	unsigned long parserHandle = SVObjectScriptParser::GetParserHandle();

	SVObjectScriptParser* pParser = new SVObjectScriptParser(new SVInspectionTreeParser(rTree, ToolItem, parserHandle,
		rOwner.getObjectId(), &rOwner, &ParserProgressDialog));
	if (nullptr != pParser)
	{
		// Set the Parser Object
		ParserProgressDialog.AddParser(parserHandle, pParser);
		ParserProgressDialog.DoModal();
		return calcObjectId(UniqueID);
	}

	return SvDef::InvalidObjectId;
}

std::string getModuleNameFromPath(const std::string& path)
{
	size_t pos = path.find_last_of("\\");
	if (std::string::npos == pos)
	{
		pos = 0;
	}
	else
	{
		++pos;
	}
	auto name = path.substr(pos, path.size() - pos - strlen(SvO::ModuleExtension));

	//check if name valid and correct it if invalid.
	bool isValidName = false;
	int i = 1;
	do
	{
		auto startName = name;
		auto cmd = SvOi::checkNewModuleName(name);
		if (S_OK != cmd.hresult() && 0 < cmd.errormessage().messages_size())
		{
			if (SvStl::Tid_NameContainsInvalidChars == cmd.errormessage().messages(0).additionaltextid())
			{ //invalid name, use any name
				startName = "NewModuleName";
				name = startName;
			}
			else if (SvStl::Tid_ModuleNameAlreadyUsed == cmd.errormessage().messages(0).additionaltextid())
			{ //Name already used, rename new module.
				name = startName + std::to_string(i++);
			}
		}
		else
		{
			isValidName = true;
		}
	} while (false == isValidName);
	return name;
}

void importModuleAndRemoveFromList(SvDef::StringVector& containedFilepaths)
{
	auto iter = std::ranges::find_if(containedFilepaths, [](const auto& rPath){ return rPath.ends_with(SvO::ModuleExtension); });
	while (containedFilepaths.end() != iter)
	{
		auto data = SvFs::readContentFromFile(*iter, true);
		auto name = getModuleNameFromPath(*iter);

		SvOi::importModule(name, data);

		containedFilepaths.erase(iter);
		iter = std::ranges::find_if(containedFilepaths, [](const auto& rPath){ return rPath.ends_with(SvO::ModuleExtension); });
	}
}

std::string readClipboardData()
{
	std::string ClipboardData;

	if (::OpenClipboard(AfxGetApp()->GetMainWnd()->GetSafeHwnd()))
	{

		UINT  ClipboardFormat(::RegisterClipboardFormat(SvO::ToolClipboardFormat));
		HGLOBAL ClipboardMemData = nullptr;

		if (nullptr == (ClipboardMemData = ::GetClipboardData(ClipboardFormat)))
		{
			::CloseClipboard();

			SvStl::MessageManager e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_GetClipboardDataFailed, SvStl::SourceFileParams(StdMessageParams));
			e.Throw();
		}
		else
		{
			size_t DataSize = ::GlobalSize(ClipboardMemData);
			ClipboardData.resize(DataSize);
			char* pClipboardData(nullptr);
			pClipboardData = static_cast<char*> (::GlobalLock(ClipboardMemData));
			if (nullptr != pClipboardData)
			{
				memcpy(&ClipboardData.at(0), pClipboardData, DataSize);
			}
			::GlobalUnlock(ClipboardMemData);
			::CloseClipboard();
		}
	}

	return ClipboardData;
}

void writeClipboardData(const std::string& rCliboardData)
{
	if (::OpenClipboard(AfxGetApp()->GetMainWnd()->GetSafeHwnd()))
	{
		UINT  ClipboardFormat(::RegisterClipboardFormat(SvO::ToolClipboardFormat));

		if (::EmptyClipboard() && 0 != ClipboardFormat)
		{
			HGLOBAL ClipboardData = GlobalAlloc(GMEM_MOVEABLE, rCliboardData.size() + 1);

			if (nullptr != ClipboardData)
			{
				memcpy(GlobalLock(ClipboardData), &rCliboardData.at(0), rCliboardData.size());
				GlobalUnlock(ClipboardData);

				if (nullptr == ::SetClipboardData(ClipboardFormat, ClipboardData))
				{
					::CloseClipboard();
					::GlobalFree(ClipboardData);
					SvStl::MessageManager e(SvStl::MsgType::Data);
					e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_SetClipboardDataFailed, SvStl::SourceFileParams(StdMessageParams));
					e.Throw();
				}
				ToolExportImport::SetReloadAfterCopyToolsToClipboard(false);
			}
			else
			{
				::CloseClipboard();
				SvStl::MessageManager e(SvStl::MsgType::Data);
				e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardMemoryFailed, SvStl::SourceFileParams(StdMessageParams));
				e.Throw();
			}
		}
		::CloseClipboard();
	}
}

SvDef::StringVector streamToolsToXmlFile(const std::vector<uint32_t>& rToolIds, const std::string& rXmlFilePath, bool addModuleFiles)
{
	std::ostringstream MemoryStream;
	SvXml::SVObjectXMLWriter XmlWriter(MemoryStream);

	std::string rootNodeName(SvXml::ToolCopyTag);
	XmlWriter.WriteRootElement(rootNodeName.c_str());
	XmlWriter.WriteSchema();
	XmlWriter.WriteStartOfBase();
	XmlWriter.WriteShortEvirmonment(TheSVObserverApp().getCurrentVersion());

	XmlWriter.StartElement(SvXml::ToolsTag);

	int maxToolDepth = 0;
	SvDef::StringVector dependencyFilepaths;

	for (int i = 0; rToolIds.size() > i; ++i)
	{
		if (SvDef::InvalidObjectId != rToolIds[i])
		{
			auto* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(rToolIds[i]));

			if (nullptr == pTool)
			{
				SvStl::MessageManager e(SvStl::MsgType::Data);
				e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolInvalid, SvStl::SourceFileParams(StdMessageParams));
				e.Throw();
			}

			if (addModuleFiles)
			{
				if (auto* pModuleTool = dynamic_cast<SvTo::ModuleTool*>(pTool); pModuleTool)
				{
					std::string moduleFile = SvStl::GlobalPath::Inst().GetRunPath(SvOi::getModuleName(pModuleTool->getModuleGuid()).c_str()) + SvO::ModuleExtension;
					dependencyFilepaths.push_back(moduleFile);
				}
			}

			writeToolParameter(XmlWriter, *pTool, i);
			pTool->Persist(XmlWriter);
			maxToolDepth = std::max(maxToolDepth, pTool->getToolDepth(false));
		}
	}

	XmlWriter.EndElement(); //ToolsTag
	XmlWriter.WriteAttribute(SvXml::ToolDepthTag, maxToolDepth);
	XmlWriter.EndAllElements();
	auto xmlText = SvUl::to_ansi(MemoryStream.str());
	SvFs::writeStringToFile(rXmlFilePath, xmlText, true);

	findDependencyFiles(xmlText, std::back_inserter(dependencyFilepaths));
	dependencyFilepaths.emplace_back(rXmlFilePath);

	return dependencyFilepaths;
}

std::vector<uint32_t> parseTreeToTool(SvXml::SVXMLMaterialsTree& rTree, SVObjectClass& rOwner)
{
	SvXml::SVXMLMaterialsTree::SVBranchHandle ToolsItem(nullptr);

	std::vector<uint32_t> toolIds;

	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::ToolsTag, nullptr, ToolsItem))
	{
		SvXml::SVXMLMaterialsTree::SVBranchHandle ToolItem(nullptr);

		ToolItem = rTree.getFirstBranch(ToolsItem);

		while (rTree.isValidBranch(ToolItem))
		{
			toolIds.push_back(parseOneToolFromTree(rTree, rOwner, ToolItem));
			ToolItem = rTree.getNextBranch(ToolsItem, ToolItem);
		}
	}

	bool anyInvalidIds = std::any_of(toolIds.cbegin(), toolIds.cend(), [](const auto& toolID){return SvDef::InvalidObjectId == toolID; });

	if (toolIds.empty() || anyInvalidIds)
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConversionFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}

	return toolIds;
}
}

bool toolClipboardDataPresent()
{
	try
	{
		readClipboardData();
		return true;
	}
	catch (const SvStl::MessageContainer&)
	{
		//Exception means data not correct format
		return false;
	}
}

void replaceToolName(std::string& rXmlData, const std::string& rOldToolName, const std::string& rNewToolName, const std::string& rOldFullToolName, const std::string& rNewFullToolName)
{
	if (rNewToolName != rOldToolName)
	{
		size_t pos = rXmlData.find(scObjectNameTag);

		if (pos != std::string::npos)
		{
			// adding ">" and "<" ensures that only the correct occurrences of the name in the XML string will be replaced
			auto searchString = ">" + rNewToolName + "<";
			auto replacementString = ">" + rOldToolName + "<";

			pos += sizeof(scObjectNameTag);
			pos = rXmlData.find(replacementString.c_str(), pos);
			rXmlData.replace(pos, strlen(replacementString.c_str()), searchString.c_str());
		}
	}

	if (false == rOldFullToolName.empty())
	{ //update the dotted Name
		std::string fullToolNameStr = rOldFullToolName + _T(".");
		auto fullToolNameNewStr = rNewFullToolName + _T(".");

		SvUl::searchAndReplace(rXmlData, fullToolNameStr.c_str(), fullToolNameNewStr.c_str());

		//ConnectedDotname should not be renamed, because it is to display the old connectedDotname if the id invalid. (Change is caused by SVB-743)
		std::string tmpText = "<DATA Name=\"ConnectedDotname\" Type=\"VT_BSTR\">";
		fullToolNameStr = tmpText + fullToolNameStr;
		fullToolNameNewStr = tmpText + fullToolNameNewStr;
		SvUl::searchAndReplace(rXmlData, fullToolNameNewStr.c_str(), fullToolNameStr.c_str());
	}
}

#pragma region Public Methods
bool ToolExportImport::m_isReloadedAfterCopyToolsToClipboard {false};

ToolExportImport::ToolExportImport()
{
}


HRESULT ToolExportImport::writeToolDataToClipboard(const std::vector<uint32_t>& rToolIds) const
{
	HRESULT result(S_OK);

	try
	{
		std::string clipboardZipFile {SvStl::GlobalPath::Inst().GetTempPath() + '\\'};
		clipboardZipFile += SvO::ExportToolsName;
		clipboardZipFile += SvO::ZipExtension;
		createToolsExportFile(rToolIds, clipboardZipFile.c_str(), _T(""), true);
		std::string clipboardData = SvFs::readContentFromFile(clipboardZipFile);

		writeClipboardData(clipboardData);
	}
	catch (const SvStl::MessageContainer& rSvE)
	{
		m_errorMessage.setMessage(rSvE.getMessage());
		result = E_FAIL;
	}

	return result;
}

std::string ToolExportImport::readlToolDataFromClipboard()
{
	std::string ClipboardData = readClipboardData();

	std::string clipboardZipFile {SvStl::GlobalPath::Inst().GetTempPath() + '\\'};
	clipboardZipFile += SvO::ExportToolsName;
	clipboardZipFile += SvO::ZipExtension;
	SvFs::writeStringToFile(clipboardZipFile, ClipboardData, false);

	std::string result = importTools(clipboardZipFile.c_str());
	::DeleteFile(clipboardZipFile.c_str());

	return result;
}

HRESULT ToolExportImport::exportTools(const std::vector<uint32_t>& rToolIds, LPCTSTR exportfileName, LPCTSTR xmlFileName, bool includeModuleFiles) const
{
	HRESULT result(S_OK);

	try
	{
		createToolsExportFile(rToolIds, exportfileName, xmlFileName, includeModuleFiles);
	}
	catch (const SvStl::MessageContainer& rSvE)
	{
		m_errorMessage.setMessage(rSvE.getMessage());
		result = E_FAIL;
	}

	return result;
}

std::string ToolExportImport::importTools(LPCTSTR fileName) const
{
	SvDef::StringVector containedFilepaths;
	if (SvUl::unzipAll(fileName, SvStl::GlobalPath::Inst().GetTempPath(), containedFilepaths) == false)
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolsUnzipFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}
	std::string xmlExtension{SvO::XmlExtension};
	auto iter = std::find_if(containedFilepaths.cbegin(), containedFilepaths.cend(), [xmlExtension](const auto& rEntry) { return rEntry.find(xmlExtension) != std::string::npos; });
	std::string xmlFile;
	if (containedFilepaths.cend() != iter)
	{
		xmlFile = *iter;
	}

	importModuleAndRemoveFromList(containedFilepaths);

	SvFs::moveFilesToFolder(containedFilepaths, SvStl::GlobalPath::Inst().GetRunPath(), xmlFile);

	return SvFs::readContentFromFile(xmlFile);
}

std::vector<uint32_t> ToolExportImport::createToolsFromXmlData(const std::string& rXmlData, uint32_t ownerId, bool onlySameVersionValid /*= true*/, bool useExplorerStyle /*= true*/)
{
	std::string XmlData(rXmlData);

	SVObjectClass* pOwner = nullptr;
	SVIPDoc* pDoc = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerId, pOwner);
	if (nullptr != pOwner)
	{
		auto* pInspection = dynamic_cast<SVInspectionProcess*> (pOwner->GetAncestor(SvPb::SVInspectionObjectType));
		if (nullptr == pInspection)
		{
			SvStl::MessageManager e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Invalid_Inspection, SvStl::SourceFileParams(StdMessageParams));
			e.Throw();
		}
		pDoc = GetIPDocByInspectionID(pInspection->getObjectId());
		if (nullptr == pDoc)
		{
			SvStl::MessageManager e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Invalid_Inspection, SvStl::SourceFileParams(StdMessageParams));
			e.Throw();
		}
	}
	else
	{
		SvStl::MessageContainer e(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidOwner, SvStl::SourceFileParams(StdMessageParams));
		throw e;
	}

	SvXml::SVXMLMaterialsTree Tree;
	SvXml::convertXmlToTree(XmlData, Tree);
	checkVersion(Tree, onlySameVersionValid);

	checkToolDepth(Tree, ownerId);
	SvXml::SVXMLMaterialsTree::SVBranchHandle ToolsItem = nullptr;
	if (false == SvXml::SVNavigateTree::GetItemBranch(Tree, SvXml::ToolsTag, nullptr, ToolsItem))
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConversionFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}

	auto Result = updateAllToolNames(XmlData, Tree, *pOwner, *pDoc, useExplorerStyle);
	if (S_OK == Result)
	{
		Result = replaceUniqueIds(XmlData, Tree);
	}
	if (S_OK == Result)
	{
		Tree.Clear();
		SvXml::convertXmlToTree(XmlData, Tree);
		auto toolIds = parseTreeToTool(Tree, *pOwner);
#if defined (TRACE_THEM_ALL) || defined (TRACE_TOOLSET)
		std::stringstream str;
		str << "Pasting " << toolId << " after " << postToolId << ", owner = " << ownerId << "\n";
		::OutputDebugString(str.str().c_str());
#endif
		return toolIds;
	}

	return {{}};
}
#pragma endregion Public Methods

#pragma region Protected Methods
void ToolExportImport::createToolsExportFile(const std::vector<uint32_t>& rToolIds, LPCTSTR exportFleName, LPCTSTR xmlFileName, bool includeModuleFiles) const
{
	std::string xmlFilePath {xmlFileName};
	if (xmlFilePath.empty())
	{
		xmlFilePath = SvStl::GlobalPath::Inst().GetTempPath() + _T("\\") + std::string {SvO::ExportToolsName} + SvO::XmlExtension;
	}
	SvDef::StringVector filepaths = streamToolsToXmlFile(rToolIds, xmlFilePath, includeModuleFiles);

	if (!SvUl::makeZipFile(exportFleName, filepaths, _T(""), false))
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolsZipFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}

	::DeleteFile(xmlFilePath.c_str());
}

void ToolExportImport::checkVersion(SvXml::SVXMLMaterialsTree& rTree, bool onlySameVersionValid) const
{
	HRESULT Result(E_FAIL);

	SvXml::SVXMLMaterialsTree::SVBranchHandle EnvironmentItem = nullptr;

	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_ENVIRONMENT, nullptr, EnvironmentItem))
	{
		_variant_t ClipboardVersion;

		SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_VERSION_NUMBER, EnvironmentItem, ClipboardVersion);

		//Clipboard SVObserver version and current version must be the same
		if (VT_UI4 == ClipboardVersion.vt)
		{
			if (TheSVObserverApp().getCurrentVersion() == ClipboardVersion.ulVal || 
				(false == onlySameVersionValid && TheSVObserverApp().getCurrentVersion() > ClipboardVersion.ulVal))
			{
				Result = S_OK;
			}
		}
	}

	if (S_OK != Result)
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_Clipboard_VersionMismatch, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}
}

HRESULT ToolExportImport::updateAllToolNames(std::string& rXmlData, SVTreeType& rTree, const SVObjectClass& rOwner, const SVIPDoc& rDoc, bool useExplorerStyle) const
{
	HRESULT Result(E_FAIL);

	SvXml::SVXMLMaterialsTree::SVBranchHandle ToolsItem(nullptr);

	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::ToolsTag, nullptr, ToolsItem))
	{
		SvXml::SVXMLMaterialsTree::SVBranchHandle ToolItem = rTree.getFirstBranch(ToolsItem);

		int toolIndex = 0;
		while (rTree.isValidBranch(ToolItem))
		{
			std::string fullToolNameTag {std::format(SvXml::FullToolNameTag, toolIndex++)};
			SvXml::SVXMLMaterialsTree::SVLeafHandle fullToolNameHandle;
			SvXml::SVNavigateTree::GetItemLeaf(rTree, fullToolNameTag.c_str(), ToolsItem, fullToolNameHandle);
			variant_t value;
			rTree.getLeafData(fullToolNameHandle, value);

			_variant_t ObjectName;
			SvXml::SVNavigateTree::GetItem(rTree, scObjectNameTag, ToolItem, ObjectName);

			updateToolName(rXmlData, rOwner, rDoc, SvUl::createStdString(ObjectName.bstrVal), SvUl::createStdString(value), useExplorerStyle);
			Result = S_OK;
			ToolItem = rTree.getNextBranch(ToolsItem, ToolItem);
		}
	}

	if (S_OK != Result)
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConversionFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}

	return Result;
}

void ToolExportImport::updateToolName(std::string& rXmlData, const SVObjectClass& rOwner, const SVIPDoc& rDoc, const std::string& toolName, const std::string& rOldFullToolName, bool useExplorerStyle) const
{
	std::string NewToolName = getUniqueToolName(toolName, rOwner, rDoc, useExplorerStyle);

#if defined (TRACE_THEM_ALL) || defined (TRACE_TOOLCLIPBOARD)
	std::stringstream ss;
	ss << _T("ToolClipboard::updateToolName(): ") << ToolName << _T(" -> ") << NewToolName << "\n";
	::OutputDebugString(ss.str().c_str());
#endif
	std::string fullToolNameNewStr = rOwner.GetObjectNameToObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType) + _T(".") + NewToolName;
	replaceToolName(rXmlData, toolName, NewToolName, rOldFullToolName, fullToolNameNewStr);
}

std::string ToolExportImport::getUniqueToolName(const std::string& rToolName, const SVObjectClass& rOwner, const SVIPDoc& rDoc, bool useExplorerStyle) const
{
	std::string uniqueName;

	if (rOwner.isLoopOrGroupTool())
	{
		auto pTaskObjectList = static_cast<const SvIe::SVTaskObjectListClass*>(&rOwner);
		uniqueName = pTaskObjectList->makeNameUnique(rToolName, m_additionalNames, useExplorerStyle);
	}
	else
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TOOLNAMES)
		if (!m_additionalNames.empty())
		{
			OutputDebugString("(additional):\n");
			for (const auto& rName : m_additionalNames)
			{
				OutputDebugString(std::format("\t{}\n", rName.c_str()).c_str());
			}
		}
#endif

		uniqueName = rDoc.makeNameUnique(rToolName, m_additionalNames, useExplorerStyle);

#if defined (TRACE_THEM_ALL) || defined (TRACE_TOOLNAMES)
		OutputDebugString(std::format("new Unique: {} -> {}\n", rToolName.c_str(), uniqueName.c_str()).c_str());
#endif
	}

	//The unique name _always_ needs to be added to m_additionalNames (and not just if uniqueName != rToolName) because all names that are present 
	//need to be either there or in the "names from the tool group list" that will be checked against further down in pDoc->makeNameUnique()
	m_additionalNames.push_back(uniqueName);//It's OK to have just one vector for this since a clipboard will be used only "in one place"

	return uniqueName;
}


HRESULT ToolExportImport::replaceUniqueIds(std::string& rXmlData, SvXml::SVXMLMaterialsTree& rTree) const
{
	HRESULT Result;

	SvXml::SVXMLMaterialsTree::SVBranchHandle ToolsItem = nullptr;

	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::ToolsTag, nullptr, ToolsItem))
	{
		constexpr LPCTSTR cXmlSearch = _T("Type=\"VT_BSTR\">");
		constexpr LPCTSTR cTmpStart = _T("($");
		/// This replacement is required to ensure rXmlData does not have multiple unique ID values
		std::string searchString {cXmlSearch};
		searchString += _T("{#");
		std::string replaceString {cXmlSearch};
		replaceString += cTmpStart;
		SvUl::searchAndReplace(rXmlData, searchString.c_str(), replaceString.c_str());

		//Replace each uniqueID with a new ID
		SvDef::StringVector UniqueIDVector = rTree.getLeafValues(ToolsItem, std::string(scUniqueReferenceIDTag));
		for (auto rUniqueID : UniqueIDVector)
		{
			uint32_t newId = SVObjectManagerClass::Instance().getNextObjectId();
			std::string newIdString = convertObjectIdToString(newId);
			SvUl::searchAndReplace(rUniqueID, _T("{#"), cTmpStart);
			SvUl::searchAndReplace(rXmlData, rUniqueID.c_str(), newIdString.c_str());
		}
		if (m_isReloadedAfterCopyToolsToClipboard)
		{
			size_t pos = rXmlData.find(cTmpStart, 0);

			while (pos != std::string::npos)
			{
				size_t endPos = rXmlData.find("}", pos + 1);
				if (endPos == std::string::npos)
				{
					Log_Assert(false);
					break;
				}
				rXmlData.replace(pos, endPos - pos, "{#0}");

				pos = rXmlData.find(cTmpStart, pos);
			}
		}
		else
		{
			SvUl::searchAndReplace(rXmlData, replaceString.c_str(), searchString.c_str());
		}
		Result = S_OK;
	}
	else
	{
		Result = E_FAIL;
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConversionFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}

	return Result;
}

#pragma endregion Protected Methods

