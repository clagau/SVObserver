//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
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
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary\ZipHelper.h"
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
HRESULT ToolClipboard::writeToClipboard(uint32_t toolId) const
{
	HRESULT result( S_OK);

	try
	{
		if( ::OpenClipboard( AfxGetMainWnd()->m_hWnd ) )
		{    
			UINT  ClipboardFormat( ::RegisterClipboardFormat( SvO::ToolClipboardFormat ) );

			if( ::EmptyClipboard() && 0 != ClipboardFormat )
			{
				std::string FileName( SvStl::GlobalPath::Inst().GetTempPath().c_str() );

				FileName += _T("\\");
				FileName += SvO::ClipboardFileName;
				result = streamToolToZip( FileName, toolId);

				if( S_OK == result)
				{
					std::string FileData;

					FileName += SvO::ZipExtension;
					readFileToString( FileName, FileData );
					::DeleteFile( FileName.c_str() );
					HGLOBAL ClipboardData = GlobalAlloc( GMEM_MOVEABLE,	FileData.size() + 1 );

					if( nullptr != ClipboardData )
					{
						memcpy( GlobalLock( ClipboardData ), &FileData.at( 0 ), FileData.size() );
						GlobalUnlock( ClipboardData );

						if( nullptr == ::SetClipboardData( ClipboardFormat, ClipboardData ) )
						{
							::CloseClipboard();
							::GlobalFree(ClipboardData);
							result = E_FAIL;
							SvStl::MessageManager e( SvStl::MsgType::Data);
							e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_SetClipboardDataFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25000_SetClipboardData );
							e.Throw();
						}
					}
					else
					{
						::CloseClipboard();
						result = E_FAIL;
						SvStl::MessageManager e( SvStl::MsgType::Data);
						e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardMemoryFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25001_ClipboardMemory );
						e.Throw();
					}
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

HRESULT ToolClipboard::readFromClipboard(uint32_t postId, uint32_t ownerId, uint32_t& rToolId)
{
	HRESULT Result( S_OK );

	try
	{
		CWaitCursor Wait;
		std::string ClipboardData;
		std::string XmlData;

		Result = convertClipboardDataToString( ClipboardData );
		if( S_OK == Result )
		{
			std::string FileName( SvStl::GlobalPath::Inst().GetTempPath().c_str()  );

			FileName += _T("\\");
			FileName += SvO::ClipboardFileName;
			FileName += SvO::ZipExtension;
			writeStringToFile( FileName, ClipboardData, false );

			SvDef::StringVector ZippedFiles;
			if(!SvUl::unzipAll(FileName, SvStl::GlobalPath::Inst().GetTempPath(), ZippedFiles))
			{
				SvStl::MessageManager e(SvStl::MsgType::Data);
				e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardUnzipFailed, SvStl::SourceFileParams(StdMessageParams));
				e.Throw();
			}
			::DeleteFile( FileName.c_str() );
			updateDependencyFiles( ZippedFiles );
			
			//Same file name just different extensions
			SvUl::searchAndReplace( FileName, SvO::ZipExtension, SvO::XmlExtension );
			readFileToString( FileName, XmlData );
			XmlData.append( _T("\0") );
			::DeleteFile( FileName.c_str() );
		}

		SVObjectClass* pOwner = nullptr;
		SVObjectManagerClass::Instance().GetObjectByIdentifier(ownerId, pOwner);
		if(nullptr != pOwner)
		{
			m_pInspection = dynamic_cast<SVInspectionProcess*> (pOwner->GetAncestor(SvPb::SVInspectionObjectType));
		}

		SVTreeType Tree;

		if( S_OK == Result )
		{
			Result = convertXmlToTree( XmlData, Tree );
		}
		if( S_OK == Result )
		{
			Result = checkVersion( Tree );
		}

		if( S_OK == Result )
		{
			Result = readTool( XmlData, Tree, postId, ownerId);
		}
		if( S_OK == Result )
		{
			Result = replaceToolName( XmlData, Tree, pOwner );
		}
		if( S_OK == Result )
		{
			Result = replaceUniqueIds( XmlData, Tree );
		}
		if( S_OK == Result )
		{
			Tree.Clear();
			Result = convertXmlToTree( XmlData, Tree );
		}
		if( S_OK == Result )
		{
			Result = parseTreeToTool( Tree, pOwner, rToolId);
		}
	}
	catch( const SvStl::MessageContainer& rSvE )
	{
		SvStl::MessageManager e(SvStl::MsgType::Log | SvStl::MsgType::Display );
		e.setMessage( rSvE.getMessage() );
	}

	return Result;
}

bool ToolClipboard::isClipboardDataValid()
{
	bool Result( true);

	try
	{
		std::string ClipboardData;
		convertClipboardDataToString( ClipboardData );
	}
	catch(  const SvStl::MessageContainer& )
	{
		//Exception means data not correct format
		Result = false;
	}

	return Result;
}

#pragma endregion Public Methods

#pragma region Protected Methods
HRESULT ToolClipboard::streamToolToZip( const std::string& rFileName, uint32_t toolId) const
{
	HRESULT Result{S_OK};

	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject(toolId));

	if( nullptr == pTool)
	{
		Result = E_FAIL;
		SvStl::MessageManager e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25002_ToolInvalid );
		e.Throw();
	}
	else
	{
		m_pInspection = dynamic_cast<SVInspectionProcess*> (pTool->GetAncestor(SvPb::SVInspectionObjectType));

		std::ostringstream MemoryStream;
		SvXml::SVObjectXMLWriter XmlWriter( MemoryStream );

		std::string rootNodeName( SvXml::ToolCopyTag );
		XmlWriter.WriteRootElement( rootNodeName.c_str() );
		XmlWriter.WriteSchema();
		writeBaseAndEnvironmentNodes( XmlWriter );

		XmlWriter.StartElement(SvXml::ToolsTag );
		writeSourceIds( XmlWriter, *pTool );
		pTool->Persist( XmlWriter );

		XmlWriter.EndAllElements();

		SvDef::StringVector FileNames;
		findDependencyFiles( MemoryStream.str(), FileNames );

		std::string XmlFileName( rFileName );
		XmlFileName += SvO::XmlExtension;
		FileNames.emplace_back(XmlFileName);
		writeStringToFile( XmlFileName, MemoryStream.str(), true );

		std::string ZipFileName( rFileName );
		ZipFileName += SvO::ZipExtension;

		if (!SvUl::makeZipFile(ZipFileName, FileNames, _T(""), false))
		{
			Result = E_FAIL;
			SvStl::MessageManager e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardZipFailed, SvStl::SourceFileParams(StdMessageParams));
			e.Throw();
		}
		::DeleteFile(XmlFileName.c_str());
	}

	return Result;
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
	Value = TheSVObserverApp.getCurrentVersion();

	rWriter.StartElement( SvXml::CTAG_ENVIRONMENT  );
	rWriter.WriteAttribute( SvXml::CTAG_VERSION_NUMBER, Value );
	rWriter.EndElement();
}

void ToolClipboard::writeSourceIds(SvOi::IObjectWriter& rWriter, SvTo::SVToolClass& rTool ) const
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
	rWriter.WriteAttribute(SvXml::FullToolNameTag, Value);

	std::set<uint32_t> imageIdVector;
	SvOl::SVInObjectInfoStruct* pImageInfo = nullptr;
	SvOl::SVInObjectInfoStruct* pLastImageInfo = nullptr;
	while( nullptr == pImageInfo && S_OK ==  rTool.FindNextInputImageInfo( pImageInfo, pLastImageInfo ) )
	{
		if( nullptr != pImageInfo )
		{
			SVObjectClass* pImage = pImageInfo->GetInputObjectInfo().getFinalObject();
			if( pImageInfo->IsConnected() && nullptr !=  pImage)
			{
				SVObjectClass* pTool = pImage->GetAncestor(SvPb::SVObjectTypeEnum::SVToolObjectType, true);
				//Add input image only if not from the tool being copied
				if(nullptr == pTool || pTool->getObjectId() != rTool.getObjectId())
				{
					imageIdVector.insert(pImageInfo->GetInputObjectInfo().getObjectId());
				}
			}
		}
		pLastImageInfo = pImageInfo;
		pImageInfo = nullptr;
	}

	int imageIndex {0};
	for(auto imageId : imageIdVector)
	{
		std::string inputImageName {SvUl::Format(SvXml::InputImageTag, imageIndex)};
		rWriter.WriteAttribute(inputImageName.c_str(), convertObjectIdToVariant(imageId));
		imageIndex++;
	}
}

void ToolClipboard::findDependencyFiles( const std::string& rToolXmlString, SvDef::StringVector& rDependencyFiles ) const
{
	size_t StartPos( 0 );
	size_t EndPos( 0 );
	std::string SearchString( SvStl::GlobalPath::Inst().GetRunPath().c_str() );
	SearchString += _T("\\");

	StartPos = rToolXmlString.find( SearchString.c_str(), EndPos );
	while( std::string::npos != StartPos )
	{
		EndPos = rToolXmlString.find(SvXml::DataTag, StartPos );
		if( std::string::npos != EndPos )
		{
			std::string FileName;
			FileName = rToolXmlString.substr( StartPos, EndPos-StartPos );
			rDependencyFiles.emplace_back( FileName );

			StartPos = rToolXmlString.find( SearchString.c_str(), EndPos );
		}
		else
		{
			StartPos = std::string::npos;
		}
	}
}

void ToolClipboard::updateDependencyFiles( const SvDef::StringVector& rDependencyFiles ) const
{
	std::string XmlFileName( SvO::ClipboardFileName );
	XmlFileName += SvO::XmlExtension;

	for(const auto& rEntry : rDependencyFiles)
	{
		//We need to ignore the XML file it is not a dependency file
		if( std::string::npos == rEntry.find( XmlFileName.c_str() ) )
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

HRESULT ToolClipboard::convertClipboardDataToString( std::string& rClipboardData )
{
	HRESULT Result( S_OK );

	if( ::OpenClipboard( AfxGetMainWnd()->GetSafeHwnd() ) )
	{
		UINT  ClipboardFormat( ::RegisterClipboardFormat( SvO::ToolClipboardFormat ) );
		HGLOBAL ClipboardMemData = nullptr;

		if( nullptr == (ClipboardMemData = ::GetClipboardData( ClipboardFormat )) )
		{
			::CloseClipboard();

			Result = E_FAIL;
			SvStl::MessageManager e( SvStl::MsgType::Data);
			e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_GetClipboardDataFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25003_GetClipboardData );
			e.Throw();
		}
		else
		{
			size_t DataSize = ::GlobalSize( ClipboardMemData );
			rClipboardData.resize( DataSize );
			char* pClipboardData( nullptr );
			pClipboardData = static_cast<char*> ( ::GlobalLock( ClipboardMemData ) );
			memcpy( &rClipboardData.at(0), pClipboardData, DataSize );
			::GlobalUnlock( ClipboardMemData );
			::CloseClipboard();
		}
	}

	return Result;
}

void ToolClipboard::writeStringToFile( const std::string& rFileName, const std::string& rFileData, bool Text ) const
{
	std::ofstream FileStream;

	if( Text )
	{
		FileStream.open( rFileName.c_str() );
	}
	else
	{
		FileStream.open( rFileName.c_str(), std::ofstream::binary );
	}
	if( FileStream.is_open() )
	{
		FileStream.write( &rFileData.at( 0 ), rFileData.size() );
		FileStream.close();
	}
}

void ToolClipboard::readFileToString( const std::string& rFileName, std::string& rFileData ) const
{
	std::ifstream FileStream;

	FileStream.open( rFileName.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate );
	if( FileStream.is_open() )
	{
		size_t FileSize( 0 );
		FileSize = static_cast<size_t> (FileStream.tellg());
		rFileData.resize( FileSize );
		FileStream.seekg ( 0, std::ios::beg );
		FileStream.read ( &rFileData.at( 0 ), FileSize );
		FileStream.close();
	}
}

HRESULT ToolClipboard::convertXmlToTree( const std::string& rXmlData, SVTreeType& rTree ) const
{
	SvXml::SaxXMLHandler<SVTreeType>  SaxHandler;
	HRESULT	Result =SaxHandler.BuildFromXMLString (&rTree, _variant_t(rXmlData.c_str()));
	if( false == SUCCEEDED(Result))
	{
		SvStl::MessageManager e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25004_ClipboardDataConversion );
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
		if ( VT_UI4 == ClipboardVersion.vt && TheSVObserverApp.getCurrentVersion() == ClipboardVersion.ulVal )
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
		SvStl::MessageManager e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25008_ClipboardDataConversion );
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
			e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_LoopToolInsertLoopToolFailed, messageList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25006_ColorToolInsert);
			e.Throw();
		}
	}

	SVIPDoc* pDoc = TheSVObserverApp.GetIPDoc(m_pInspection->getObjectId());
	SVToolSet* pToolSet = m_pInspection->GetToolSet();
	bool isColorCamera = m_pInspection->IsColorCamera();
	SvTo::SVToolClass* pPostTool = dynamic_cast<SvTo::SVToolClass*>(SVObjectManagerClass::Instance().GetObject(postId));
	if (nullptr != pDoc && nullptr != pToolSet)
	{
		//Color tool can not be inserted into a IPD without color images
		if (SvPb::ColorToolClassId == toolClassId && !pDoc->isImageAvailable(SvPb::SVImageColorType))
		{
			result = E_FAIL;
			SvStl::MessageManager e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ColorToolInsertFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25006_ColorToolInsert);
			e.Throw();
		}
		//Only color tools are allowed to be the first tool in a color system
		else if (SvPb::ColorToolClassId != toolClassId && isColorCamera && ((nullptr != pPostTool && pPostTool->getToolPosition() == 1) || (nullptr != pToolSet && 0 == pToolSet->GetSize())))
		{
			result = E_FAIL;
			SvStl::MessageManager e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_NonColorToolInsertFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25007_NonColorToolInsert);
			e.Throw();
		}
		else
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

							if ((nullptr == pPostTool || toolPositionOfImage < pPostTool->getToolPosition()) && inspectionId == m_pInspection->getObjectId())
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
							if (SvPb::ColorToolClassId == toolClassId)
							{
								useToolSetImage = true;
							}
							else
							{
								SVObjectClass* pBand0Image = dynamic_cast<SVObjectClass*> (pToolSet->getBand0Image());
								if (nullptr != pBand0Image)
								{
									DefaultImageId = pBand0Image->getObjectId();
								}
							}
						}
						else
						{
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
						SvUl::searchAndReplace(rXmlData, convertObjectIdToString(inputImageId).c_str(), convertObjectIdToString(DefaultImageId).c_str());
					}
				}
			}
		}
	}
	return result;
}

HRESULT ToolClipboard::replaceToolName( std::string& rXmlData, SVTreeType& rTree, const SVObjectClass* pOwner) const
{
	HRESULT Result( E_FAIL );

	SVTreeType::SVBranchHandle ToolsItem( nullptr );

	if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::ToolsTag, nullptr, ToolsItem ) )
	{
		_variant_t fullToolName;
		SvXml::SVNavigateTree::GetItem(rTree, SvXml::FullToolNameTag, ToolsItem, fullToolName);
		std::string fullToolNameStr = SvUl::createStdString(fullToolName.bstrVal);
		SVTreeType::SVBranchHandle ToolItem( nullptr );

		ToolItem = rTree.getFirstBranch(ToolsItem);

		if( rTree.isValidBranch( ToolItem ) )
		{
			_variant_t ObjectName;

			SvXml::SVNavigateTree::GetItem( rTree, scObjectNameTag, ToolItem, ObjectName);
			std::string ToolName = SvUl::createStdString(ObjectName.bstrVal);
			std::string NewName;

			//@Todo[mec] kann ein Colortool owner eines tools sein?
			if (nullptr != pOwner &&( SvPb::SVColorToolObjectType == pOwner->GetObjectSubType() || SvPb::LoopToolObjectType == pOwner->GetObjectSubType() || SvPb::GroupToolObjectType == pOwner->GetObjectSubType()) )
			{
				NewName = static_cast<const SvIe::SVTaskObjectListClass*>(pOwner)->MakeUniqueToolName(ToolName.c_str());
			}
			else
			{
				SVIPDoc* pDoc = (nullptr != m_pInspection) ? TheSVObserverApp.GetIPDoc(m_pInspection->getObjectId()) : nullptr;
				if (nullptr != pDoc)
				{
					// Check to make sure the tool name is unique
					NewName = pDoc->CheckName(ToolName);
				}
			}
			if (NewName != ToolName)
			{
				size_t pos = rXmlData.find(scObjectNameTag);

				if (pos != std::string::npos)
				{
					pos += sizeof(scObjectNameTag);
					pos = rXmlData.find(ToolName.c_str(), pos);
					rXmlData.replace(pos, strlen(ToolName.c_str()), NewName.c_str());
				}
			}
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
			Result = S_OK;
		}
	}

	if( S_OK != Result )
	{
		SvStl::MessageManager e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25009_ClipboardDataConversion );
		e.Throw();
	}

	return Result;
}

HRESULT ToolClipboard::replaceUniqueIds( std::string& rXmlData, SVTreeType& rTree ) const
{
	HRESULT Result( E_FAIL );

	SVTreeType::SVBranchHandle ToolsItem = nullptr;

	if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::ToolsTag, nullptr, ToolsItem ) )
	{
		constexpr LPCTSTR cXmlSearch = _T("Type=\"VT_BSTR\">");
		/// This replacement is required to insure rXmlData does not have multiple unique ID values
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
		SvStl::MessageManager e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25010_ClipboardDataConversion );
		e.Throw();
	}

	return Result;
}

HRESULT ToolClipboard::parseTreeToTool(SVTreeType& rTree, SVObjectClass* pOwner, uint32_t& rToolId)
{
	HRESULT Result( E_FAIL );

	SVTreeType::SVBranchHandle ToolsItem( nullptr );

	if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::ToolsTag, nullptr, ToolsItem ) )
	{
		SVTreeType::SVBranchHandle ToolItem( nullptr );

		ToolItem = rTree.getFirstBranch( ToolsItem );

		if ( rTree.isValidBranch( ToolItem ) )
		{
			_variant_t UniqueID;

			SvXml::SVNavigateTree::GetItem( rTree, scUniqueReferenceIDTag, ToolItem, UniqueID);

			SVObjectClass* pOwnerTmp = pOwner;
			if (nullptr == pOwnerTmp)
			{
				assert(false);
				pOwnerTmp = (nullptr != m_pInspection) ? (static_cast<SVInspectionProcess*> (m_pInspection))->GetToolSet() : nullptr;
			}
			
			if( nullptr != pOwnerTmp)
			{
				SVParserProgressDialog ParserProgressDialog( SvO::InsertingTool, AfxGetMainWnd() );
				unsigned long parserHandle = SVObjectScriptParser::GetParserHandle();

				SVObjectScriptParser* pParser = new SVObjectScriptParser(new SVInspectionTreeParser< SVTreeType >(rTree, ToolItem, parserHandle, pOwnerTmp->getObjectId(), pOwnerTmp, &ParserProgressDialog));
				if( nullptr != pParser )
				{
					// Set the Parser Object
					ParserProgressDialog.AddParser(parserHandle, pParser);
					ParserProgressDialog.DoModal();
					rToolId = calcObjectId(UniqueID);
					Result = S_OK;
				}
			}
		}
	}

	if( S_OK != Result )
	{
		SvStl::MessageManager e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25011_ClipboardDataConversion );
		e.Throw();
	}
	return Result;
}
#pragma endregion Protected Methods

