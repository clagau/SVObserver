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
#include "SVUtilityLibrary/SVGUID.h"
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

#pragma region Constructor
ToolClipboard::ToolClipboard( SVInspectionProcess& rInspection ) :
	m_rInspection( rInspection )
{
}

ToolClipboard::~ToolClipboard()
{
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT ToolClipboard::writeToClipboard( const SVGUID& rToolGuid ) const
{
	HRESULT Result( S_OK);

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
				Result = streamToolToZip( FileName, rToolGuid );

				if( S_OK == Result)
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
							Result = E_FAIL;
							SvStl::MessageMgrStd e( SvStl::MsgType::Data);
							e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_SetClipboardDataFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25000_SetClipboardData );
							e.Throw();
						}
					}
					else
					{
						::CloseClipboard();
						Result = E_FAIL;
						SvStl::MessageMgrStd e( SvStl::MsgType::Data);
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
		SvStl::MessageMgrStd e(SvStl::MsgType::Log | SvStl::MsgType::Display );
		e.setMessage( rSvE.getMessage() );
	}

	return Result;
}

HRESULT ToolClipboard::readFromClipboard( const SVGUID& rPostGuid, const SVGUID& rOwnerGuid, SVGUID& rToolGuid )
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
				SvStl::MessageMgrStd e(SvStl::MsgType::Data);
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
		SVObjectManagerClass::Instance().GetObjectByIdentifier(rOwnerGuid, pOwner);

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
			Result = readTool( XmlData, Tree, rPostGuid, rOwnerGuid);
		}
		if( S_OK == Result )
		{
			Result = replaceToolName( XmlData, Tree, pOwner );
		}
		if( S_OK == Result )
		{
			Result = replaceUniqueGuids( XmlData, Tree );
		}
		if( S_OK == Result )
		{
			Tree.Clear();
			Result = convertXmlToTree( XmlData, Tree );
		}
		if( S_OK == Result )
		{
			Result = parseTreeToTool( Tree, pOwner, rToolGuid );
		}
	}
	catch( const SvStl::MessageContainer& rSvE )
	{
		SvStl::MessageMgrStd e(SvStl::MsgType::Log | SvStl::MsgType::Display );
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
HRESULT ToolClipboard::streamToolToZip( const std::string& rFileName, const SVGUID& rToolGuid ) const
{
	HRESULT Result( S_OK );

	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (SVObjectManagerClass::Instance().GetObject( rToolGuid ));

	if( nullptr == pTool)
	{
		Result = E_FAIL;
		SvStl::MessageMgrStd e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ToolInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25002_ToolInvalid );
		e.Throw();
	}
	else
	{
		std::ostringstream MemoryStream;
		SvXml::SVObjectXMLWriter XmlWriter( MemoryStream );

		std::string rootNodeName( SvXml::ToolCopyTag );
		XmlWriter.WriteRootElement( rootNodeName.c_str() );
		XmlWriter.WriteSchema();
		writeBaseAndEnvironmentNodes( XmlWriter );

		XmlWriter.StartElement(SvXml::ToolsTag );
		writeSourceGuids( XmlWriter, *pTool );
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
			SvStl::MessageMgrStd e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardZipFailed, SvStl::SourceFileParams(StdMessageParams));
			e.Throw();
			Result = E_FAIL;
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

void ToolClipboard::writeSourceGuids(SvOi::IObjectWriter& rWriter, SvTo::SVToolClass& rTool ) const
{
	_variant_t Value;

	Value.Clear();
	Value = SVGUID( m_rInspection.GetUniqueObjectID() ).ToVARIANT();
	rWriter.WriteAttribute( m_rInspection.GetObjectName(), Value );

	Value.Clear();
	Value = SVGUID( rTool.GetClassID() ).ToVARIANT();
	rWriter.WriteAttribute(SvXml::ToolTypeTag, Value );

	Value.Clear();
	std::string tmpString = rTool.GetObjectNameToObjectType(SvPb::SVObjectTypeEnum::SVToolSetObjectType);
	Value = tmpString.c_str();
	rWriter.WriteAttribute(SvXml::FullToolNameTag, Value);

	std::set<SVGUID> imageGuidVector;
	SvOl::SVInObjectInfoStruct* pImageInfo = nullptr;
	SvOl::SVInObjectInfoStruct* pLastImageInfo = nullptr;
	while( nullptr == pImageInfo && S_OK ==  rTool.FindNextInputImageInfo( pImageInfo, pLastImageInfo ) )
	{
		if( nullptr != pImageInfo )
		{
			SVObjectClass* pImage = pImageInfo->GetInputObjectInfo().getObject();
			if( pImageInfo->IsConnected() && nullptr !=  pImage)
			{
				SVObjectClass* pTool = pImage->GetAncestor(SvPb::SVObjectTypeEnum::SVToolObjectType, true);
				//Add input image only if not from the tool being copied
				if(nullptr == pTool || pTool->GetUniqueObjectID() != rTool.GetUniqueObjectID())
				{
					imageGuidVector.insert(pImageInfo->GetInputObjectInfo().getUniqueObjectID());
				}
			}
		}
		pLastImageInfo = pImageInfo;
		pImageInfo = nullptr;
	}

	int imageIndex {0};
	for(const auto& rImageGuid : imageGuidVector)
	{
		std::string inputImageName {SvUl::Format(SvXml::InputImageTag, imageIndex)};
		rWriter.WriteAttribute(inputImageName.c_str(), rImageGuid.ToVARIANT());
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
			SvStl::MessageMgrStd e( SvStl::MsgType::Data);
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
		SvStl::MessageMgrStd e( SvStl::MsgType::Data);
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
		SvStl::MessageMgrStd e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_Clipboard_VersionMismatch, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25005_VersionMismatch );
		e.Throw();
	}
	return Result;
}

HRESULT ToolClipboard::readTool( std::string& rXmlData, SVTreeType& rTree, const SVGUID& rPostGuid, const SVGUID& rOwnerGuid ) const
{
	HRESULT Result( S_OK );

	SVTreeType::SVBranchHandle ToolsItem = nullptr;

	if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::ToolsTag, nullptr, ToolsItem ) )
	{
		_variant_t Inspection;
		_variant_t ToolType;
		_variant_t inputImage;
		SVGuidSet InputImages;

		SvXml::SVNavigateTree::GetItem( rTree, m_rInspection.GetObjectName(), ToolsItem, Inspection );
		SvXml::SVNavigateTree::GetItem( rTree, SvXml::ToolTypeTag, ToolsItem, ToolType );

		int imageIndex{0};
		std::string inputImageName {SvUl::Format(SvXml::InputImageTag, imageIndex)};
		SVTreeType::SVLeafHandle inputImageHandle;
		SvXml::SVNavigateTree::GetItemLeaf(rTree, inputImageName.c_str(), ToolsItem, inputImageHandle);
		while(rTree.isValidLeaf(ToolsItem, inputImageHandle))
		{
			rTree.getLeafData(inputImageHandle, inputImage);
			InputImages.insert(SVGUID(inputImage));
			imageIndex++;
			inputImageName = SvUl::Format(SvXml::InputImageTag, imageIndex);
			SvXml::SVNavigateTree::GetItemLeaf(rTree, inputImageName.c_str(), ToolsItem, inputImageHandle);
		}

		SVGUID InspectionGuid( Inspection );
		SVGUID ToolTypeGuid( ToolType );
		Result = validateGuids(rXmlData, rPostGuid, rOwnerGuid, InspectionGuid, ToolTypeGuid, InputImages);
	}
	else
	{
		Result = E_FAIL;
		SvStl::MessageMgrStd e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25008_ClipboardDataConversion );
		e.Throw();
	}

	return Result;
}

HRESULT ToolClipboard::validateGuids(std::string& rXmlData, const SVGUID& rPostGuid, const SVGUID& rOwnerGuid, const SVGUID& rInspectionId, const SVGUID& rToolTypeGuid, SVGuidSet rInputImages) const
{
	HRESULT Result{S_OK};

	SVObjectClass* pOwner = SVObjectManagerClass::Instance().GetObject(rOwnerGuid);
	if (nullptr != pOwner && LoopToolClassGuid == pOwner->GetClassID() && LoopToolClassGuid == rToolTypeGuid)
	{
		Result = E_FAIL;
		SvStl::MessageMgrStd e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_LoopToolInsertLoopToolFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25006_ColorToolInsert);
		e.Throw();
	}

	SVIPDoc* pDoc = TheSVObserverApp.GetIPDoc(m_rInspection.GetUniqueObjectID());
	SvTo::SVToolClass* pPostTool = dynamic_cast<SvTo::SVToolClass*>(SVObjectManagerClass::Instance().GetObject(rPostGuid));
	if (nullptr != pDoc)
	{
		SVToolSetClass* pToolSet =  m_rInspection.GetToolSet();
		//Color tool can not be inserted into a IPD without color images
		if (SVColorToolClassGuid == rToolTypeGuid && !pDoc->isImageAvailable(SvPb::SVImageColorType))
		{
			Result = E_FAIL;
			SvStl::MessageMgrStd e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ColorToolInsertFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25006_ColorToolInsert);
			e.Throw();
		}
		//Only color tools are allowed to be the first tool in a color system
		else if (SVColorToolClassGuid != rToolTypeGuid && m_rInspection.IsColorCamera() && ((nullptr != pPostTool && pPostTool->getToolPosition() == 1) || (nullptr == pToolSet && 0 == pToolSet->GetSize())))
		{
			Result = E_FAIL;
			SvStl::MessageMgrStd e(SvStl::MsgType::Data);
			e.setMessage(SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_NonColorToolInsertFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25007_NonColorToolInsert);
			e.Throw();
		}
		else
		{
			for (const auto& rInputImageGuid : rInputImages)
			{
				if (GUID_NULL != rInputImageGuid)
				{
					bool useStandardImage = true;
					SVObjectClass* pImage = SVObjectManagerClass::Instance().GetObject(rInputImageGuid);
					if (nullptr != pImage)
					{
						SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (pImage->GetAncestor(SvPb::SVToolObjectType));
						if (nullptr != pTool && nullptr != m_rInspection.GetToolSet())
						{
							int toolPositionOfImage = pTool->getToolPosition();

							if ((nullptr == pPostTool || toolPositionOfImage < pPostTool->getToolPosition()) && rInspectionId == m_rInspection.GetUniqueObjectID())
							{
								useStandardImage = false;
							}
						}
					}

					if (useStandardImage)
					{
						SVGUID DefaultImageGuid(GUID_NULL);
						bool useToolSetImage {false};
						if (m_rInspection.IsColorCamera())
						{
							if (SVColorToolClassGuid == rToolTypeGuid)
							{
								useToolSetImage = true;
							}
							else
							{
								SVObjectClass* pBand0Image = dynamic_cast<SVObjectClass*> (m_rInspection.GetToolSet()->getBand0Image());
								if (nullptr != pBand0Image)
								{
									DefaultImageGuid = pBand0Image->GetUniqueObjectID();
								}
							}
						}
						else
						{
							//For mono IPD the toolset image can be used for all tools except color tool
							if (nullptr != m_rInspection.GetToolSetMainImage() && SVColorToolClassGuid != rToolTypeGuid)
							{
								useToolSetImage = true;
							}
						}

						if (useToolSetImage)
						{
							DefaultImageGuid = m_rInspection.GetToolSetMainImage()->GetUniqueObjectID();
						}
						SvUl::searchAndReplace(rXmlData, rInputImageGuid.ToString().c_str(), DefaultImageGuid.ToString().c_str());
					}
				}
			}
		}
	}
	return Result;
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

			if (nullptr != pOwner && LoopToolClassGuid == pOwner->GetClassID())
			{
				NewName = static_cast<const SvIe::SVTaskObjectListClass*>(pOwner)->MakeUniqueToolName(ToolName.c_str());
			}
			else
			{
				SVIPDoc* pDoc = TheSVObserverApp.GetIPDoc(m_rInspection.GetUniqueObjectID());
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
		SvStl::MessageMgrStd e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25009_ClipboardDataConversion );
		e.Throw();
	}

	return Result;
}

HRESULT ToolClipboard::replaceUniqueGuids( std::string& rXmlData, SVTreeType& rTree ) const
{
	HRESULT Result( E_FAIL );

	SVTreeType::SVBranchHandle ToolsItem = nullptr;

	if( SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::ToolsTag, nullptr, ToolsItem ) )
	{
		SvDef::StringSet UniqueIDList;

		rTree.getLeafValues( ToolsItem, std::string( scUniqueReferenceIDTag ), UniqueIDList);

		SvDef::StringSet::iterator Iter( UniqueIDList.begin() );
		//Replace each uniqueID with a new GUID
		SVGUID newUniqueID( GUID_NULL );
		while( UniqueIDList.end() != Iter )
		{
			CoCreateGuid( &newUniqueID );
			SvUl::searchAndReplace( rXmlData, Iter->c_str(), newUniqueID.ToString().c_str() );
			++Iter;
		}

		Result = S_OK;
	}
	else
	{
		Result = E_FAIL;
		SvStl::MessageMgrStd e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25010_ClipboardDataConversion );
		e.Throw();
	}

	return Result;
}

HRESULT ToolClipboard::parseTreeToTool( SVTreeType& rTree, SVObjectClass* pOwner, SVGUID& rToolGuid)
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
				pOwnerTmp = m_rInspection.GetToolSet();
			}
			
			if( nullptr != pOwnerTmp)
			{
				SVParserProgressDialog ParserProgressDialog( SvO::InsertingTool, AfxGetMainWnd() );
				unsigned long parserHandle = SVObjectScriptParserClass::GetParserHandle();

				SVObjectScriptParserClass* pParser = new SVObjectScriptParserClass(new SVInspectionTreeParser< SVTreeType >(rTree, ToolItem, parserHandle, pOwnerTmp->GetUniqueObjectID(), pOwnerTmp, &ParserProgressDialog));
				if( nullptr != pParser )
				{
					// Set the Parser Object
					ParserProgressDialog.AddParser(parserHandle, pParser);
					ParserProgressDialog.DoModal();

					rToolGuid = SVGUID( UniqueID );

					Result = S_OK;
				}
			}
		}
	}

	if( S_OK != Result )
	{
		SvStl::MessageMgrStd e( SvStl::MsgType::Data);
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvStl::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25011_ClipboardDataConversion );
		e.Throw();
	}
	return Result;
}
#pragma endregion Protected Methods

