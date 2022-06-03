//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Tool clipboard
//* .File Name       : $Workfile:   ToolClipboard.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 Jan 2015 10:38:46  $
//* ----------------------------------------------------------------------------
//* This class is used to write and read the selected tool to and from the clipboard
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
#include "ToolClipboard.h"

#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVInspectionTreeParser.h"
#include "SVObjectScriptParser.h"
#include "SVObserver.h"
#include "SVParserProgressDialog.h"
#include "SVToolSet.h"
#include "TextDefinesSvO.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionEngine/SVTaskObjectList.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVFileSystemLibrary/FilepathUtilities.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "SVXMLLibrary/SaxXMLHandler.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVObjectXMLWriter.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma endregion Declarations

#pragma region Public Methods

ToolClipboard::ToolClipboard() : m_baseFilePath(SvStl::GlobalPath::Inst().GetTempPath().c_str())
{
	m_baseFilePath += _T("\\");
	m_baseFilePath += SvO::ClipboardFileName;
	m_zipFilePath = m_baseFilePath + SvO::ZipExtension;
}


HRESULT ToolClipboard::writeXmlToolData(const std::vector<uint32_t>& rToolIds) const
{
	HRESULT result(S_OK);

	try
	{
		if (::OpenClipboard(AfxGetApp()->m_pMainWnd->m_hWnd))
		{
			UINT  ClipboardFormat(::RegisterClipboardFormat(SvO::ToolClipboardFormat));

			if (::EmptyClipboard() && 0 != ClipboardFormat)
			{
				std::string zippedToolDataForAllTools = createToolDefinitionString(rToolIds);

				HGLOBAL ClipboardData = GlobalAlloc(GMEM_MOVEABLE, zippedToolDataForAllTools.size() + 1);

				if (nullptr != ClipboardData)
				{
					memcpy(GlobalLock(ClipboardData), &zippedToolDataForAllTools.at(0), zippedToolDataForAllTools.size());
					GlobalUnlock(ClipboardData);

					if (nullptr == ::SetClipboardData(ClipboardFormat, ClipboardData))
					{
						::CloseClipboard();
						::GlobalFree(ClipboardData);
						result = E_FAIL;
						SvStl::MessageManager e(SvStl::MsgType::Data);
						e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_SetClipboardDataFailed, SvStl::SourceFileParams(StdMessageParams));
						e.Throw();
					}
				}
				else
				{
					::CloseClipboard();
					result = E_FAIL;
					SvStl::MessageManager e(SvStl::MsgType::Data);
					e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardMemoryFailed, SvStl::SourceFileParams(StdMessageParams));
					e.Throw();
				}
			}
			::CloseClipboard();
		}
	}
	catch (const SvStl::MessageContainer& rSvE)
	{
		m_errorMessage.setMessage(rSvE.getMessage());
		result = E_FAIL;
	}

	return result;
}


std::string ToolClipboard::readXmlToolData()
{
	CWaitCursor Wait;

	std::string ClipboardData = clipboardDataToString();
	std::string baseFilePath(SvStl::GlobalPath::Inst().GetTempPath().c_str());

	writeStringToFile(m_zipFilePath, ClipboardData, false);

	SvDef::StringVector containedFilepaths;
	if (!SvUl::unzipAll(m_zipFilePath, SvStl::GlobalPath::Inst().GetTempPath(), containedFilepaths))
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardUnzipFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}
	::DeleteFile(m_zipFilePath.c_str());
	moveDependencyFilesToRunPath(containedFilepaths);

	return readContentFromFileAndDelete(xmlFilePath());
}


std::vector<uint32_t> ToolClipboard::createToolsFromXmlData(const std::string& rXmlData, uint32_t ownerId)
{
	std::string XmlData(rXmlData);

	SVObjectClass* pOwner = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerId, pOwner);
	if (nullptr != pOwner)
	{
		m_pInspection = dynamic_cast<SVInspectionProcess*> (pOwner->GetAncestor(SvPb::SVInspectionObjectType));
	}

	SVTreeType Tree;

	HRESULT Result = convertXmlToTree(XmlData, Tree);
	if (S_OK == Result)
	{
		Result = checkVersion(Tree);
	}

	if (S_OK == Result)
	{
		readTool(XmlData, Tree, ownerId);
		Result = replaceDuplicateToolNames(XmlData, Tree, pOwner);
	}
	if (S_OK == Result)
	{
		Result = replaceUniqueIds(XmlData, Tree);
	}
	if (S_OK == Result)
	{
		Tree.Clear();
		Result = convertXmlToTree(XmlData, Tree);
	}
	if (S_OK == Result)
	{
		auto toolIds = parseTreeToTool(Tree, pOwner);
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
std::string ToolClipboard::createToolDefinitionString(const std::vector<uint32_t>& rToolIds) const
{
	SvDef::StringVector filepaths = streamToolsToXmlFile(rToolIds);

	if (!SvUl::makeZipFile(m_zipFilePath, filepaths, _T(""), false))
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardZipFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}
	for (const auto& XmlFileName : filepaths)
	{
		::DeleteFile(XmlFileName.c_str());
	}

	auto zippedToolDataForAllTools = readContentFromFileAndDelete(m_zipFilePath);

	return zippedToolDataForAllTools;
}

SvDef::StringVector ToolClipboard::streamToolsToXmlFile(const std::vector<uint32_t>& rToolIds) const
{
	SvDef::StringVector filepaths;

	std::ostringstream MemoryStream;
	SvXml::SVObjectXMLWriter XmlWriter(MemoryStream);

	std::string rootNodeName(SvXml::ToolCopyTag);
	XmlWriter.WriteRootElement(rootNodeName.c_str());
	XmlWriter.WriteSchema();
	writeBaseAndEnvironmentNodes(XmlWriter);
	XmlWriter.StartElement(SvXml::ToolsTag);

	for (int i=0; rToolIds.size() > i; ++i)
	{
		if (SvDef::InvalidObjectId != rToolIds[i])
		{
			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(rToolIds[i]));

			if (nullptr == pTool)
			{
				SvStl::MessageManager e(SvStl::MsgType::Data);
				e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25002_ToolInvalid);
				e.Throw();
			}

			m_pInspection = dynamic_cast<SVInspectionProcess*> (pTool->GetAncestor(SvPb::SVInspectionObjectType));

			writeToolParameter(XmlWriter, *pTool, i);
			pTool->Persist(XmlWriter);
		}
	}

	XmlWriter.EndAllElements();

	auto DependencyFilepaths = findDependencyFiles(MemoryStream.str());

	filepaths.insert(filepaths.end(), DependencyFilepaths.begin(), DependencyFilepaths.end());

	writeStringToFile(xmlFilePath(), MemoryStream.str(), true);

	filepaths.emplace_back(xmlFilePath());

	return filepaths;
}

void ToolClipboard::writeBaseAndEnvironmentNodes(SvOi::IObjectWriter& rWriter) const
{
	_variant_t xmlnsValue;
	_variant_t Value;

	xmlnsValue.SetString(SvO::SvrNameSpace);

	Value.SetString(SvO::SV_BaseNode);

	rWriter.StartElement(SvXml::BaseTag);
	rWriter.ElementAttribute(SvO::XmlNameSpace, xmlnsValue);
	rWriter.ElementAttribute(SvXml::TypeTag, Value);

	Value.Clear();
	Value = TheSVObserverApp().getCurrentVersion();

	rWriter.StartElement(SvXml::CTAG_ENVIRONMENT);
	rWriter.WriteAttribute(SvXml::CTAG_VERSION_NUMBER, Value);
	rWriter.EndElement();
}

void ToolClipboard::writeToolParameter(SvOi::IObjectWriter& rWriter, SvTo::SVToolClass& rTool, int pos) const
{
	std::string toolTag {SvUl::Format(SvXml::FullToolNameTag, pos)};
	std::string tmpString = rTool.GetObjectNameToObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType);
	_variant_t Value = tmpString.c_str();
	rWriter.WriteAttribute(toolTag.c_str(), Value);
}

SvDef::StringVector ToolClipboard::findDependencyFiles(const std::string& rToolXmlString) const
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

	SvDef::StringVector DependencyFilepaths;

	while (std::string::npos != StartPos)
	{
		EndPos = ToolXmlString.find(SvXml::DataTag, StartPos);
		if (std::string::npos != EndPos)
		{
			std::string FileName;
			FileName = ToolXmlString.substr(StartPos, EndPos - StartPos);

			DependencyFilepaths.emplace_back(FileName);

			StartPos = ToolXmlString.find(SearchString.c_str(), EndPos);
		}
		else
		{
			StartPos = std::string::npos;
		}
	}

	return DependencyFilepaths;
}


void ToolClipboard::moveDependencyFilesToRunPath(const SvDef::StringVector& rDependencyFilepaths) const
{
	for (const auto& rEntry : rDependencyFilepaths)
	{
		//ignore the tool XML files themself: they are not dependency files
		if (std::string::npos == rEntry.find(m_baseFilePath))
		{
			_TCHAR Name[_MAX_FNAME];
			_TCHAR Extension[_MAX_EXT];
			_splitpath(rEntry.c_str(), nullptr, nullptr, Name, Extension);

			std::string DestinationFile(SvStl::GlobalPath::Inst().GetRunPath().c_str());
			DestinationFile += _T("\\");
			DestinationFile += Name;
			DestinationFile += Extension;

			if (0 == ::_access_s(DestinationFile.c_str(), 0))
			{
				::DeleteFile(rEntry.c_str());
			}
			else
			{
				::MoveFile(rEntry.c_str(), DestinationFile.c_str());
			}
		}
	}
}

HRESULT ToolClipboard::convertXmlToTree(const std::string& rXmlData, SVTreeType& rTree) const
{
	SvXml::SaxXMLHandler<SVTreeType>  SaxHandler;
	HRESULT	Result = SaxHandler.BuildFromXMLString(&rTree, _variant_t(rXmlData.c_str()));
	if (false == SUCCEEDED(Result))
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConversionFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}
	return Result;
}

HRESULT ToolClipboard::checkVersion(SVTreeType& rTree) const
{
	HRESULT Result(E_FAIL);

	SVTreeType::SVBranchHandle EnvironmentItem = nullptr;

	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_ENVIRONMENT, nullptr, EnvironmentItem))
	{
		_variant_t ClipboardVersion;

		SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_VERSION_NUMBER, EnvironmentItem, ClipboardVersion);

		//Clipboard SVObserver version and current version must be the same
		if (VT_UI4 == ClipboardVersion.vt && TheSVObserverApp().getCurrentVersion() == ClipboardVersion.ulVal)
		{
			Result = S_OK;
		}
	}

	if (S_OK != Result)
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_Clipboard_VersionMismatch, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25005_VersionMismatch);
		e.Throw();
	}
	return Result;
}

void ToolClipboard::readTool(std::string& rXmlData, SVTreeType& rTree, uint32_t ownerId) const
{
	SVTreeType::SVBranchHandle ToolsItem = nullptr;
	if (false == SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::ToolsTag, nullptr, ToolsItem))
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConversionFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}

	std::vector<typename SVTreeType::SVBranchHandle> branchHandles = SvXml::SVNavigateTree::findSubbranches(rTree, ToolsItem); //we are assuming here that all subbranches contain tools

	if (false == branchHandles.empty())
	{
		for (auto handle : branchHandles)
		{
			_variant_t Value;
			if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::ClassIdTag, handle, Value))
			{
				SvPb::ClassIdEnum classId = calcClassId(Value);
				validateIds(rXmlData, ownerId, classId);
			}
		}
	}
}

void ToolClipboard::validateIds(std::string& rXmlData, uint32_t ownerId, SvPb::ClassIdEnum toolClassId) const
{
	if (nullptr == m_pInspection)
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Invalid_Inspection, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}

	SVObjectClass* pOwner = SVObjectManagerClass::Instance().GetObject(ownerId);
	if (nullptr != pOwner)
	{
		SvStl::MessageTextEnum firstTId = (SvPb::LoopToolClassId == toolClassId) ? SvStl::Tid_LoopTool : (SvPb::GroupToolClassId == toolClassId) ? SvStl::Tid_GroupTool : SvStl::Tid_Empty;
		SvStl::MessageTextEnum secondTId = (SvPb::LoopToolObjectType == pOwner->GetObjectSubType()) ? SvStl::Tid_LoopTool : (SvPb::GroupToolObjectType == pOwner->GetObjectSubType()) ? SvStl::Tid_GroupTool : SvStl::Tid_Empty;
		if (SvStl::Tid_Empty != firstTId && SvStl::Tid_Empty != secondTId)
		{
			SvDef::StringVector messageList;
			messageList.push_back(SvStl::MessageData::convertId2AdditionalText(firstTId));
			messageList.push_back(SvStl::MessageData::convertId2AdditionalText(secondTId));
			SvStl::MessageManager e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_LoopToolInsertLoopToolFailed, messageList, SvStl::SourceFileParams(StdMessageParams));
			e.Throw();
		}
	}

	SVIPDoc* pDoc = GetIPDocByInspectionID(m_pInspection->getObjectId());
	SVToolSet* pToolSet = m_pInspection->GetToolSet();
	if (nullptr != pDoc && nullptr != pToolSet)
	{
		if (!pDoc->isImageAvailable(SvPb::SVImageColorType))
		{
			const std::string colorToolTypeString = "<DATA Name=\"ClassID\" Type=\"VT_INT\">35</DATA>";
			bool isColorToolInto = (std::string::npos != rXmlData.find(colorToolTypeString));
			//Color tool can not be inserted into a IPD without color images
			if (isColorToolInto)
			{
				SvStl::MessageManager e(SvStl::MsgType::Data);
				e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ColorToolInsertFailed, SvStl::SourceFileParams(StdMessageParams));
				e.Throw();
			}
		}
	}
}

HRESULT ToolClipboard::replaceDuplicateToolNames(std::string& rXmlData, SVTreeType& rTree, const SVObjectClass* pOwner) const
{
	HRESULT Result(E_FAIL);

	SVTreeType::SVBranchHandle ToolsItem(nullptr);

	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::ToolsTag, nullptr, ToolsItem))
	{
		SVTreeType::SVBranchHandle ToolItem = rTree.getFirstBranch(ToolsItem);

		int toolIndex = 0;
		while (rTree.isValidBranch(ToolItem))
		{
			std::string fullToolNameTag {SvUl::Format(SvXml::FullToolNameTag, toolIndex++)};
			SVTreeType::SVLeafHandle fullToolNameHandle;
			SvXml::SVNavigateTree::GetItemLeaf(rTree, fullToolNameTag.c_str(), ToolsItem, fullToolNameHandle);
			variant_t value;
			rTree.getLeafData(fullToolNameHandle, value);

			replaceToolNameIfDuplicate(rXmlData, rTree, pOwner, ToolItem, SvUl::createStdString(value));
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

void ToolClipboard::replaceToolNameIfDuplicate(std::string& rXmlData, SVTreeType& rTree, const SVObjectClass* pOwner, SVTreeType::SVBranchHandle ToolItem, const std::string& rOldFullToolName) const
{
	_variant_t ObjectName;
	SvXml::SVNavigateTree::GetItem(rTree, scObjectNameTag, ToolItem, ObjectName);
	std::string ToolName = SvUl::createStdString(ObjectName.bstrVal);

	std::string NewToolName = getUniqueToolName(ToolName, pOwner);

#if defined (TRACE_THEM_ALL) || defined (TRACE_TOOLCLIPBOARD)
	std::stringstream ss;
	ss << _T("ToolClipboard::replaceToolNameIfDuplicate(): ") << ToolName << _T(" -> ") << NewToolName << "\n";
	::OutputDebugString(ss.str().c_str());
#endif

	if (NewToolName != ToolName)
	{
		// adding ">" and "<" ensures that the correct occurrence of the name in the XML string will be replaced
		auto searchString = ">" + NewToolName + "<";
		auto replacementString = ">" + ToolName + "<";

		size_t pos = rXmlData.find(scObjectNameTag);

		if (pos != std::string::npos)
		{
			pos += sizeof(scObjectNameTag);
			pos = rXmlData.find(replacementString.c_str(), pos);
			rXmlData.replace(pos, strlen(replacementString.c_str()), searchString.c_str());
		}

		if (false == rOldFullToolName.empty())
		{ //replace the dottedName in equations with the new name.
			std::string fullToolNameStr = rOldFullToolName + _T(".");
			std::string fullToolNameNewStr = fullToolNameStr;
			if (nullptr != pOwner)
			{
				fullToolNameNewStr = pOwner->GetObjectNameToObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType) + _T(".") + NewToolName + _T(".");
			}
			else
			{
				SvUl::searchAndReplace(fullToolNameNewStr, ToolName.c_str(), NewToolName.c_str());
			}
			SvUl::searchAndReplace(rXmlData, fullToolNameStr.c_str(), fullToolNameNewStr.c_str());

			//ConnectedDotname should not be renamed, because it is to display the old connectedDotname if the id invalid. (Change is caused by SVB-743)
			std::string tmpText = "<DATA Name=\"ConnectedDotname\" Type=\"VT_BSTR\">";
			fullToolNameStr = tmpText + fullToolNameStr;
			fullToolNameNewStr = tmpText + fullToolNameNewStr;
			SvUl::searchAndReplace(rXmlData, fullToolNameNewStr.c_str(), fullToolNameStr.c_str());
		}
	}
}

std::string ToolClipboard::getUniqueToolName(std::string& rToolName, const SVObjectClass* pOwner) const
{
	std::string uniqueName = rToolName;

	if (nullptr != pOwner && (pOwner->isLoopOrGroupTool()))
	{
		auto pTaskObjectList = static_cast<const SvIe::SVTaskObjectListClass*>(pOwner);
		uniqueName = pTaskObjectList->makeNameUnique(rToolName, m_additionalNames);
	}
	else
	{
		SVIPDoc* pDoc = (nullptr != m_pInspection) ? GetIPDocByInspectionID(m_pInspection->getObjectId()) : nullptr;
		if (nullptr != pDoc)
		{
			uniqueName = pDoc->makeNameUnique(rToolName, m_additionalNames);
		}
	}

	if (uniqueName != rToolName)
	{//i.e. newly created. It's OK to have just one vector for this since a Clipboard will be eine only "in one Place"
		m_additionalNames.push_back(uniqueName);
	}

	return uniqueName;
}


HRESULT ToolClipboard::replaceUniqueIds(std::string& rXmlData, SVTreeType& rTree) const
{
	HRESULT Result(E_FAIL);

	SVTreeType::SVBranchHandle ToolsItem = nullptr;

	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::ToolsTag, nullptr, ToolsItem))
	{
		constexpr LPCTSTR cXmlSearch = _T("Type=\"VT_BSTR\">");
		/// This replacement is required to ensure rXmlData does not have multiple unique ID values
		std::string searchString {cXmlSearch};
		searchString += _T("{#");
		std::string replaceString {cXmlSearch};
		replaceString += _T("($");
		SvUl::searchAndReplace(rXmlData, searchString.c_str(), replaceString.c_str());

		//Replace each uniqueID with a new ID
		SvDef::StringVector UniqueIDVector = rTree.getLeafValues(ToolsItem, std::string(scUniqueReferenceIDTag));
		for (auto rUniqueID : UniqueIDVector)
		{
			uint32_t newId = SVObjectManagerClass::Instance().getNextObjectId();
			std::string newIdString = convertObjectIdToString(newId);
			SvUl::searchAndReplace(rUniqueID, _T("{#"), _T("($"));
			SvUl::searchAndReplace(rXmlData, rUniqueID.c_str(), newIdString.c_str());
		}
		SvUl::searchAndReplace(rXmlData, replaceString.c_str(), searchString.c_str());
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


std::vector<uint32_t> ToolClipboard::parseTreeToTool(SVTreeType& rTree, SVObjectClass* pOwner)
{
	SVTreeType::SVBranchHandle ToolsItem(nullptr);

	std::vector<uint32_t> toolIds;

	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::ToolsTag, nullptr, ToolsItem))
	{
		SVTreeType::SVBranchHandle ToolItem(nullptr);

		ToolItem = rTree.getFirstBranch(ToolsItem);

		while (rTree.isValidBranch(ToolItem))
		{
			toolIds.push_back(parseOneToolFromTree(rTree, pOwner, ToolItem));
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


uint32_t ToolClipboard::parseOneToolFromTree(SVTreeType& rTree, SVObjectClass* pOwner, SVTreeType::SVBranchHandle ToolItem)
{
	_variant_t UniqueID;

	SvXml::SVNavigateTree::GetItem(rTree, scUniqueReferenceIDTag, ToolItem, UniqueID);

	SVObjectClass* pOwnerTmp = pOwner;
	if (nullptr == pOwnerTmp)
	{
		assert(false);
		pOwnerTmp = (nullptr != m_pInspection) ? (static_cast<SVInspectionProcess*> (m_pInspection))->GetToolSet() : nullptr;
	}

	if (nullptr != pOwnerTmp)
	{
		SVParserProgressDialog ParserProgressDialog(SvO::InsertingTool, AfxGetMainWnd());
		unsigned long parserHandle = SVObjectScriptParser::GetParserHandle();

		SVObjectScriptParser* pParser = new SVObjectScriptParser(new SVInspectionTreeParser< SVTreeType >(rTree, ToolItem, parserHandle,
			pOwnerTmp->getObjectId(), pOwnerTmp, &ParserProgressDialog));
		if (nullptr != pParser)
		{
			// Set the Parser Object
			ParserProgressDialog.AddParser(parserHandle, pParser);
			ParserProgressDialog.DoModal();
			return calcObjectId(UniqueID);
		}
	}

	return SvDef::InvalidObjectId;
}


std::string ToolClipboard::xmlFilePath() const
{
	return m_baseFilePath + SvO::XmlExtension;
}

#pragma endregion Protected Methods

bool toolClipboardDataPresent()
{
	try
	{
		clipboardDataToString();
		return true;
	}
	catch (const SvStl::MessageContainer&)
	{
		//Exception means data not correct format
		return false;
	}
}

std::string clipboardDataToString()
{
	std::string ClipboardData;

	if (::OpenClipboard(AfxGetApp()->m_pMainWnd->m_hWnd))
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
			memcpy(&ClipboardData.at(0), pClipboardData, DataSize);
			::GlobalUnlock(ClipboardMemData);
			::CloseClipboard();
		}
	}

	return ClipboardData;
}

