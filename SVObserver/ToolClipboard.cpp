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


ToolClipboard::ToolClipboard(): m_baseFilePath(SvStl::GlobalPath::Inst().GetTempPath().c_str())
{
	m_baseFilePath += _T("\\");
	m_baseFilePath += SvO::ClipboardFileName;
	m_zipFilePath = m_baseFilePath + SvO::ZipExtension;
}


HRESULT ToolClipboard::writeXmlToolData(const std::vector<uint32_t>& rToolIds) const
{
	HRESULT result( S_OK);

	try
	{
		if( ::OpenClipboard(AfxGetApp()->m_pMainWnd->m_hWnd) )
		{    
			UINT  ClipboardFormat( ::RegisterClipboardFormat( SvO::ToolClipboardFormat ) );

			if (::EmptyClipboard() && 0 != ClipboardFormat)
			{
				std::string zippedToolDataForAllTools = createToolDefinitionString(rToolIds);

				HGLOBAL ClipboardData = GlobalAlloc( GMEM_MOVEABLE,	zippedToolDataForAllTools.size() + 1 );

				if( nullptr != ClipboardData )
				{
					memcpy( GlobalLock( ClipboardData ), &zippedToolDataForAllTools.at( 0 ), zippedToolDataForAllTools.size() );
					GlobalUnlock( ClipboardData );

					if( nullptr == ::SetClipboardData( ClipboardFormat, ClipboardData ) )
					{
						::CloseClipboard();
						::GlobalFree(ClipboardData);
						result = E_FAIL;
						SvStl::MessageManager e( SvStl::MsgType::Data);
						e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_SetClipboardDataFailed, SvStl::SourceFileParams(StdMessageParams));
						e.Throw();
					}
				}
				else
				{
					::CloseClipboard();
					result = E_FAIL;
					SvStl::MessageManager e( SvStl::MsgType::Data);
					e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardMemoryFailed, SvStl::SourceFileParams(StdMessageParams));
					e.Throw();
				}
			}
			::CloseClipboard();
		}
	}
	catch( const SvStl::MessageContainer& rSvE )
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
	std::string baseFilePath( SvStl::GlobalPath::Inst().GetTempPath().c_str()  );
			
	writeStringToFile(m_zipFilePath, ClipboardData, false );

	SvDef::StringVector containedFilepaths;
	if(!SvUl::unzipAll(m_zipFilePath, SvStl::GlobalPath::Inst().GetTempPath(), containedFilepaths))
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardUnzipFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}
	::DeleteFile(m_zipFilePath.c_str() );
	moveDependencyFilesToRunPath( containedFilepaths );

	return readContentFromFileAndDelete(xmlFilePath());
}


std::vector<uint32_t> ToolClipboard::createToolsFromXmlData(const std::string& rXmlData, uint32_t postId, uint32_t ownerId)
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
		Result = readTool(XmlData, Tree, postId, ownerId);
	}
	if (S_OK == Result)
	{
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
	for (const auto& XmlFileName: filepaths)
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

	uint32_t toolIndex = 0;
	for (auto toolId : rToolIds)
	{
		if (SvDef::InvalidObjectId != toolId)
		{
			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(toolId));

			if (nullptr == pTool)
			{
				SvStl::MessageManager e(SvStl::MsgType::Data);
				e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25002_ToolInvalid);
				e.Throw();
			}

			m_pInspection = dynamic_cast<SVInspectionProcess*> (pTool->GetAncestor(SvPb::SVInspectionObjectType));

			writeSourceIds(XmlWriter, *pTool, toolIndex++);
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

	xmlnsValue.SetString( SvO::SvrNameSpace );

	Value.SetString( SvO::SV_BaseNode );

	rWriter.StartElement(SvXml::BaseTag );
	rWriter.ElementAttribute( SvO::XmlNameSpace, xmlnsValue );
	rWriter.ElementAttribute(SvXml::TypeTag, Value );

	Value.Clear();
	Value = TheSVObserverApp().getCurrentVersion();

	rWriter.StartElement( SvXml::CTAG_ENVIRONMENT  );
	rWriter.WriteAttribute( SvXml::CTAG_VERSION_NUMBER, Value );
	rWriter.EndElement();
}

void ToolClipboard::writeSourceIds(SvOi::IObjectWriter& rWriter, SvTo::SVToolClass& rTool, uint32_t toolIndex) const
{
	_variant_t Value;

	Value.Clear();
	if(nullptr != m_pInspection)
	{
		Value = convertObjectIdToVariant(m_pInspection->getObjectId());
		rWriter.WriteAttribute(SvXml::CTAG_INSPECTION_PROCESS, Value);
		Value.Clear();
		Value.SetString(m_pInspection->GetName());
		rWriter.WriteAttribute(SvXml::InspectionNameTag, Value);
	}

	Value.Clear();
	Value = rTool.GetClassID();
	rWriter.WriteAttribute(SvXml::ToolTypeTag, Value );

	Value.Clear();
	std::string tmpString = rTool.GetObjectNameToObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType);
	Value = tmpString.c_str();
	auto numberedFullToolNameTag = SvXml::FullToolNameTag + SvUl::AsString(toolIndex);
	rWriter.WriteAttribute(numberedFullToolNameTag.c_str(), Value);

	std::set<uint32_t> imageIdVector;

	std::vector<SvOl::InputObject*> inputList;
	rTool.getInputs(std::back_inserter(inputList));
	for (const auto* pInput : inputList)
	{
		if (nullptr != pInput && SvPb::SVImageObjectType == pInput->GetInputObjectInfo().m_ObjectTypeInfo.m_ObjectType && SvPb::noAttributes != pInput->ObjectAttributesAllowed()) 
		{
			SVObjectClass* pImage = pInput->GetInputObjectInfo().getObject();
			if(pInput->IsConnected() && nullptr !=  pImage)
			{
				SVObjectClass* pOwner = pImage->GetParent();
				bool addImage = true;
				while (nullptr != pOwner)
				{
					if (pOwner->GetObjectType() == SvPb::SVObjectTypeEnum::SVToolObjectType && pOwner->getObjectId() == rTool.getObjectId())
					{
						addImage = false;
						break;
					}
					pOwner = pOwner->GetParent();
				}
				//Add input image only if not from the tool being copied
				if (addImage)
				{
					imageIdVector.insert(pInput->GetInputObjectInfo().getObjectId());
				}
			}
		}
	}

	int imageIndex {0};
	for(auto imageId : imageIdVector)
	{
		std::string inputImageName {SvUl::Format(SvXml::InputImageTag, imageIndex)};
		rWriter.WriteAttribute(inputImageName.c_str(), convertObjectIdToVariant(imageId));
		imageIndex++;
	}
}

SvDef::StringVector ToolClipboard::findDependencyFiles(const std::string& rToolXmlString) const
{
	size_t StartPos( 0 );
	size_t EndPos( 0 );
	std::string SearchString( SvStl::GlobalPath::Inst().GetRunPath().c_str() );
	SearchString += _T("\\");

	std::string ToolXmlString(rToolXmlString); // working on copy of the xml string keeps changes local to this function
	StartPos = ToolXmlString.find("\\\\");
	while (std::string::npos != StartPos)
	{
		ToolXmlString.replace(StartPos, sizeof("\\\\") - 1, "\\"); // this makes the search string work 
			// regardless of whether single or double backslashes are present in file paths 
			// and ensures dependecy filepaths contain single backslashes
			// (fixes SVO-3548)
		StartPos = ToolXmlString.find("\\\\");
	}

	StartPos = ToolXmlString.find( SearchString.c_str(), EndPos );

	SvDef::StringVector DependencyFilepaths;

	while( std::string::npos != StartPos )
	{
		EndPos = ToolXmlString.find(SvXml::DataTag, StartPos );
		if( std::string::npos != EndPos )
		{
			std::string FileName;
			FileName = ToolXmlString.substr( StartPos, EndPos-StartPos );

			DependencyFilepaths.emplace_back( FileName );

			StartPos = ToolXmlString.find( SearchString.c_str(), EndPos );
		}
		else
		{
			StartPos = std::string::npos;
		}
	}

	return DependencyFilepaths;
}


void ToolClipboard::moveDependencyFilesToRunPath( const SvDef::StringVector& rDependencyFilepaths ) const
{
	for(const auto& rEntry : rDependencyFilepaths)
	{
		//ignore the tool XML files themself: they are not dependency files
		if( std::string::npos == rEntry.find(m_baseFilePath))
		{
			_TCHAR Name[_MAX_FNAME];
			_TCHAR Extension[_MAX_EXT];
			_splitpath(rEntry.c_str(), nullptr, nullptr, Name, Extension );

			std::string DestinationFile( SvStl::GlobalPath::Inst().GetRunPath().c_str());
			DestinationFile += _T("\\");
			DestinationFile += Name;
			DestinationFile += Extension;

			if( 0 == ::_access_s( DestinationFile.c_str(), 0 ) )
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

HRESULT ToolClipboard::convertXmlToTree( const std::string& rXmlData, SVTreeType& rTree ) const
{
	SvXml::SaxXMLHandler<SVTreeType>  SaxHandler;
	HRESULT	Result =SaxHandler.BuildFromXMLString (&rTree, _variant_t(rXmlData.c_str()));
	if( false == SUCCEEDED(Result))
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConversionFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}
	return Result;
}

HRESULT ToolClipboard::checkVersion( SVTreeType& rTree ) const
{
	HRESULT Result( E_FAIL );

	SVTreeType::SVBranchHandle EnvironmentItem = nullptr;

	if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_ENVIRONMENT, nullptr, EnvironmentItem ) )
	{
		_variant_t ClipboardVersion;

		SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_VERSION_NUMBER, EnvironmentItem, ClipboardVersion );

		//Clipboard SVObserver version and current version must be the same
		if ( VT_UI4 == ClipboardVersion.vt && TheSVObserverApp().getCurrentVersion() == ClipboardVersion.ulVal )
		{
			Result = S_OK;
		}
	}

	if( S_OK != Result )
	{
		SvStl::MessageManager e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_Clipboard_VersionMismatch, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25005_VersionMismatch );
		e.Throw();
	}
	return Result;
}

HRESULT ToolClipboard::readTool(std::string& rXmlData, SVTreeType& rTree, uint32_t postId, uint32_t ownerId) const
{
	HRESULT Result( S_OK );

	SVTreeType::SVBranchHandle ToolsItem = nullptr;

	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::ToolsTag, nullptr, ToolsItem))
	{
		_variant_t Value;
		std::set<uint32_t> InputImages;

		SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_INSPECTION_PROCESS, ToolsItem, Value);
		uint32_t orgInspectionID = calcObjectId(Value);
		SvXml::SVNavigateTree::GetItem(rTree, SvXml::InspectionNameTag, ToolsItem, Value);
		std::string orgInspectionName = SvUl::createStdString(Value) + '.';
		SvXml::SVNavigateTree::GetItem(rTree, SvXml::ToolTypeTag, ToolsItem, Value);
		SvPb::ClassIdEnum classId = calcClassId(Value);

		//Rename all dotted names starting with the inspection name
		if (nullptr != m_pInspection)
		{
			std::string inspectionName{ m_pInspection->GetName() };
			inspectionName += '.';
			SvUl::searchAndReplace(rXmlData, orgInspectionName.c_str(), inspectionName.c_str());
		}

		int imageIndex{ 0 };
		std::string inputImageName{ SvUl::Format(SvXml::InputImageTag, imageIndex) };
		SVTreeType::SVLeafHandle inputImageHandle;
		SvXml::SVNavigateTree::GetItemLeaf(rTree, inputImageName.c_str(), ToolsItem, inputImageHandle);
		while (rTree.isValidLeaf(ToolsItem, inputImageHandle))
		{
			Value.Clear();
			rTree.getLeafData(inputImageHandle, Value);
			InputImages.insert(calcObjectId(Value));
			imageIndex++;
			inputImageName = SvUl::Format(SvXml::InputImageTag, imageIndex);
			SvXml::SVNavigateTree::GetItemLeaf(rTree, inputImageName.c_str(), ToolsItem, inputImageHandle);
		}
		Result = validateIds(rXmlData, postId, ownerId, orgInspectionID, classId, InputImages);
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

HRESULT ToolClipboard::validateIds(std::string& rXmlData, uint32_t postId, uint32_t ownerId, uint32_t inspectionId, SvPb::ClassIdEnum toolClassId, std::set<uint32_t> rInputImages) const
{
	HRESULT result{S_OK};

	if(nullptr == m_pInspection)
	{
		return E_POINTER;
	}

	SVObjectClass* pOwner = SVObjectManagerClass::Instance().GetObject(ownerId);
	if (nullptr != pOwner)
	{
		SvStl::MessageTextEnum firstTId = (SvPb::LoopToolClassId == toolClassId) ? SvStl::Tid_LoopTool : (SvPb::GroupToolClassId == toolClassId) ? SvStl::Tid_GroupTool : SvStl::Tid_Empty;
		SvStl::MessageTextEnum secondTId = (SvPb::LoopToolObjectType == pOwner->GetObjectSubType()) ? SvStl::Tid_LoopTool : (SvPb::GroupToolObjectType == pOwner->GetObjectSubType()) ? SvStl::Tid_GroupTool : SvStl::Tid_Empty;
		if (SvStl::Tid_Empty != firstTId && SvStl::Tid_Empty != secondTId)
		{
			result = E_FAIL;
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
	bool isColorCamera = m_pInspection->IsColorCamera();
	SvTo::SVToolClass* pPostTool = dynamic_cast<SvTo::SVToolClass*>(SVObjectManagerClass::Instance().GetObject(postId));
	if (nullptr != pDoc && nullptr != pToolSet)
	{
		if (!pDoc->isImageAvailable(SvPb::SVImageColorType))
		{
			const std::string colorToolTypeString = "<DATA Name=\"ClassID\" Type=\"VT_INT\">35</DATA>";
			bool isColorToolInto = (std::string::npos != rXmlData.find(colorToolTypeString));
			//Color tool can not be inserted into a IPD without color images
			if (isColorToolInto)
			{
				result = E_FAIL;
				SvStl::MessageManager e(SvStl::MsgType::Data);
				e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ColorToolInsertFailed, SvStl::SourceFileParams(StdMessageParams));
				e.Throw();
			}
		}
		
		if (S_OK == result)
		{
			for (auto inputImageId : rInputImages)
			{
				if (SvDef::InvalidObjectId != inputImageId)
				{
					bool useStandardImage = true;
					SVObjectClass* pImage = SVObjectManagerClass::Instance().GetObject(inputImageId);
					if (nullptr != pImage)
					{
						SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (pImage->GetAncestor(SvPb::SVToolObjectType));
						if (nullptr != pTool)
						{
							int toolPositionOfImage = pTool->getToolPosition();

							if ((nullptr == pPostTool || toolPositionOfImage < pPostTool->getToolPosition() || SvPb::ImageToolClassId == toolClassId) && inspectionId == m_pInspection->getObjectId())
							{
								useStandardImage = false;
							}
						}
					}

					if (useStandardImage)
					{
						uint32_t DefaultImageId{ SvDef::InvalidObjectId };
						bool useToolSetImage {false};
						if (isColorCamera)
						{
							constexpr std::array<int, 14> nonMonoImageTools {SvPb::ColorToolClassId, SvPb::LoadImageToolClassId, SvPb::ArchiveToolClassId, SvPb::MathToolClassId, SvPb::AcquisitionToolClassId, 
								SvPb::StatisticsToolClassId, SvPb::ExternalToolClassId, SvPb::RingBufferToolClassId, SvPb::TableToolClassId, SvPb::TableAnalyzerToolClassId, SvPb::ShiftToolClassId,
								SvPb::DrawToolClassId, SvPb::LoopToolClassId, SvPb::GroupToolClassId};
							if (nonMonoImageTools.end() != std::find(nonMonoImageTools.begin(), nonMonoImageTools.end(), toolClassId))
							{
								useToolSetImage = true;
							}
							else
							{
								SVObjectClass* pBand0Image = dynamic_cast<SVObjectClass*> (pToolSet->getBand0Image());
								bool hasMonoImage = pDoc->isImageAvailable(SvPb::SVImageMonoType) && nullptr != pBand0Image;
								if(hasMonoImage)
								{
									DefaultImageId = pBand0Image->getObjectId();
								}
								else
								{
									result = E_FAIL;
									SvStl::MessageManager e(SvStl::MsgType::Data);
									e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_MonoToolInsertFailed, SvStl::SourceFileParams(StdMessageParams));
									e.Throw();
								}
							}
						}
						else
						{
							pDoc->isImageAvailable(SvPb::SVImageMonoType);
							//For mono IPD the toolset image can be used for all tools except color tool
							if (nullptr != m_pInspection->GetToolSetMainImage() && SvPb::ColorToolClassId != toolClassId)
							{
								useToolSetImage = true;
							}
						}

						if (useToolSetImage)
						{
							DefaultImageId = m_pInspection->GetToolSetMainImage()->getObjectId();
						}
						//Replace only ID's which start with ConnectedID which means it is a source image
						constexpr LPCTSTR cSourceImage = R"(Name="ConnectedID" Type="VT_BSTR">)";
						std::string inputImage{ cSourceImage };
						inputImage += convertObjectIdToString(inputImageId);
						std::string defaultImage{ cSourceImage };
						defaultImage += convertObjectIdToString(DefaultImageId);

						SvUl::searchAndReplace(rXmlData, inputImage.c_str(), defaultImage.c_str());
					}
				}
			}
		}
	}
	return result;
}

HRESULT ToolClipboard::replaceDuplicateToolNames(std::string& rXmlData, SVTreeType& rTree, const SVObjectClass* pOwner) const
{
	HRESULT Result( E_FAIL );

	SVTreeType::SVBranchHandle ToolsItem( nullptr );

	if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::ToolsTag, nullptr, ToolsItem ) )
	{
		SVTreeType::SVBranchHandle ToolItem = rTree.getFirstBranch(ToolsItem);

		uint16_t toolIndex = 0;

		std::map<std::string, int> HighestUsedIndexForBaseToolname;

		while( rTree.isValidBranch( ToolItem ) )
		{ 
			_variant_t fullToolName;
			auto numberedFullToolNameTag = SvXml::FullToolNameTag + SvUl::AsString(toolIndex++);
			SvXml::SVNavigateTree::GetItem(rTree, numberedFullToolNameTag.c_str(), ToolsItem, fullToolName);
			std::string fullToolNameStr = SvUl::createStdString(fullToolName.bstrVal);
			replaceOneToolName(rXmlData, rTree, pOwner, ToolItem, fullToolNameStr, &HighestUsedIndexForBaseToolname);
			Result = S_OK;
			ToolItem = rTree.getNextBranch(ToolsItem, ToolItem);
		}
	}

	if( S_OK != Result )
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConversionFailed, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}

	return Result;
}


void ToolClipboard::replaceOneToolName(std::string& rXmlData, SVTreeType& rTree, const SVObjectClass* pOwner, SVTreeType::SVBranchHandle ToolItem, const std::string& rFullToolNameStr, std::map<std::string, int>* pHighestUsedIndexForBaseToolname) const
{
	_variant_t ObjectName;

	SvXml::SVNavigateTree::GetItem(rTree, scObjectNameTag, ToolItem, ObjectName);
	std::string ToolName = SvUl::createStdString(ObjectName.bstrVal);
	std::string NewName;

	if (nullptr != pOwner && (SvPb::LoopToolObjectType == pOwner->GetObjectSubType() || SvPb::GroupToolObjectType == pOwner->GetObjectSubType()))
	{
		NewName = static_cast<const SvIe::SVTaskObjectListClass*>(pOwner)->MakeUniqueToolName(ToolName.c_str());
	}
	else
	{
		SVIPDoc* pDoc = (nullptr != m_pInspection) ? GetIPDocByInspectionID(m_pInspection->getObjectId()) : nullptr;
		if (nullptr != pDoc)
		{
			NewName = pDoc->determineToolnameWithUniqueIndex(ToolName, pHighestUsedIndexForBaseToolname);
		}
	}
	if (NewName != ToolName)
	{
		// adding ">" and "<" enures that the correct occurrence of the Name in the XML string will be replaced
		auto searchString = ">" + NewName + "<"; 
		auto replacementString = ">" + ToolName + "<";

		size_t pos = rXmlData.find(scObjectNameTag);

		if (pos != std::string::npos)
		{
			pos += sizeof(scObjectNameTag);
			pos = rXmlData.find(replacementString.c_str(), pos);
			rXmlData.replace(pos, strlen(replacementString.c_str()), searchString.c_str());
		}
	}

	std::string fullToolNameStr(rFullToolNameStr);
	if (!fullToolNameStr.empty())
	{ //replace the dottedName in Equations with the new name.
		fullToolNameStr += _T(".");
		std::string fullToolNameNewStr = fullToolNameStr;
		if (nullptr != pOwner)
		{
			fullToolNameNewStr = pOwner->GetObjectNameToObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType) + _T(".") + NewName + _T(".");
		}
		else
		{
			SvUl::searchAndReplace(fullToolNameNewStr, ToolName.c_str(), NewName.c_str());
		}
		SvUl::searchAndReplace(rXmlData, fullToolNameStr.c_str(), fullToolNameNewStr.c_str());
	}

}

HRESULT ToolClipboard::replaceUniqueIds( std::string& rXmlData, SVTreeType& rTree ) const
{
	HRESULT Result( E_FAIL );

	SVTreeType::SVBranchHandle ToolsItem = nullptr;

	if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::ToolsTag, nullptr, ToolsItem ) )
	{
		constexpr LPCTSTR cXmlSearch = _T("Type=\"VT_BSTR\">");
		/// This replacement is required to ensure rXmlData does not have multiple unique ID values
		std::string searchString{ cXmlSearch };
		searchString += _T("{#");
		std::string replaceString{ cXmlSearch };
		replaceString += _T("($");
		SvUl::searchAndReplace(rXmlData, searchString.c_str(), replaceString.c_str());

		//Replace each uniqueID with a new ID
		SvDef::StringVector UniqueIDVector = rTree.getLeafValues(ToolsItem, std::string(scUniqueReferenceIDTag));
		for(auto rUniqueID : UniqueIDVector)
		{
			uint32_t newId = SVObjectManagerClass::Instance().getNextObjectId();
			std::string newIdString = convertObjectIdToString(newId);
			SvUl::searchAndReplace(rUniqueID, _T("{#"), _T("($"));
			SvUl::searchAndReplace( rXmlData, rUniqueID.c_str(), newIdString.c_str() );
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
	SVTreeType::SVBranchHandle ToolsItem( nullptr );

	std::vector<uint32_t> toolIds;

	if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::ToolsTag, nullptr, ToolsItem ) )
	{
		SVTreeType::SVBranchHandle ToolItem( nullptr );

		ToolItem = rTree.getFirstBranch( ToolsItem );

		while( rTree.isValidBranch( ToolItem ) )
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

