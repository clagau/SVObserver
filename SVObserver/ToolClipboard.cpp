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
#include "SVXMLLibrary\SVObjectXMLWriter.h"
#include "SVXMLLibrary\SVNavigateTree.h"

#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary\SVInspectionLevelCreateStruct.h"
#include "SVXMLLibrary\SVConfigurationTags.h"
#include "SVUtilityLibrary\ZipHelper.h"
#include "SVSystemLibrary\SVEncodeDecodeUtilities.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVObserver.h"
#include "SVInspectionProcess.h"
#include "SVObjectScriptParser.h"
#include "SVToolSet.h"
#include "SVTool.h"
#include "SVParserProgressDialog.h"
#include "SVInspectionTreeParser.h"
#include "SVIPDoc.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "TextDefinesSvO.h"
#include "SVXMLLibrary/SaxXMLHandler.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVStatusLibrary/GlobalPath.h"
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
				SVString FileName( SvStl::GlobalPath::Inst().GetTempPath().c_str() );

				FileName += _T("\\");
				FileName += SvO::ClipboardFileName;
				Result = streamToolToZip( FileName, rToolGuid );

				if( S_OK == Result)
				{
					SVString FileData;

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
							Result = S_FALSE;
							SvStl::MessageMgrStd e( SvStl::DataOnly );
							e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvOi::Tid_SetClipboardDataFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25000_SetClipboardData );
							e.Throw();
						}
					}
					else
					{
						Result = S_FALSE;
						SvStl::MessageMgrStd e( SvStl::DataOnly );
						e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvOi::Tid_ClipboardMemoryFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25001_ClipboardMemory );
						e.Throw();
					}
				}
			}
			::CloseClipboard();
		}
	}
	catch( const SvStl::MessageContainer& rSvE )
	{
		SvStl::MessageMgrStd e( SvStl::LogAndDisplay );
		e.setMessage( rSvE.getMessage() );
	}

	return Result;
}

HRESULT ToolClipboard::readFromClipboard( int ToolListindex, SVGUID& rToolGuid )
{
	HRESULT Result( S_OK );

	try
	{
		CWaitCursor Wait;
		SVString ClipboardData;
		SVString XmlData;

		Result = convertClipboardDataToString( ClipboardData );
		if( S_OK == Result )
		{
			SVString FileName( SvStl::GlobalPath::Inst().GetTempPath().c_str()  );

			FileName += _T("\\");
			FileName += SvO::ClipboardFileName;
			FileName += SvO::ZipExtension;
			writeStringToFile( FileName, ClipboardData, false );

			SVStringSet ZippedFiles;
			ZipHelper::unzipAll( FileName, SVString( SvStl::GlobalPath::Inst().GetTempPath().c_str()  ), ZippedFiles );
			::DeleteFile( FileName.c_str() );
			updateDependencyFiles( ZippedFiles );
			
			//Same file name just different extensions
			SvUl_SF::searchAndReplace( FileName, SvO::ZipExtension, SvO::XmlExtension );
			readFileToString( FileName, XmlData );
			XmlData.append( _T("\0") );
			::DeleteFile( FileName.c_str() );
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
			Result = validateGuids( XmlData, Tree, ToolListindex );
		}
		if( S_OK == Result )
		{
			Result = replaceToolName( XmlData, Tree );
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
			Result = parseTreeToTool( Tree, rToolGuid );
		}
	}
	catch( const SvStl::MessageContainer& rSvE )
	{
		SvStl::MessageMgrStd e( SvStl::LogAndDisplay );
		e.setMessage( rSvE.getMessage() );
	}

	return Result;
}

bool ToolClipboard::isClipboardDataValid()
{
	bool Result( true);

	try
	{
		SVString ClipboardData;
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
HRESULT ToolClipboard::streamToolToZip( const SVString rFileName, const SVGUID& rToolGuid ) const
{
	HRESULT Result( S_OK );

	SVToolClass* pTool = dynamic_cast<SVToolClass*> (SVObjectManagerClass::Instance().GetObject( rToolGuid ));

	if( nullptr == pTool)
	{
		Result = S_FALSE;
		SvStl::MessageMgrStd e( SvStl::DataOnly );
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvOi::Tid_ToolInvalid, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25002_ToolInvalid );
		e.Throw();
	}
	else
	{
		std::ostringstream MemoryStream;
		SVObjectXMLWriter XmlWriter( MemoryStream );

		SVString rootNodeName( ToolCopyTag );
		XmlWriter.WriteRootElement( rootNodeName.c_str() );
		XmlWriter.WriteSchema();
		writeBaseAndEnvironmentNodes( XmlWriter );

		XmlWriter.StartElement( ToolsTag );
		writeSourceGuids( XmlWriter, *pTool );
		pTool->Persist( XmlWriter );

		XmlWriter.EndAllElements();

		SVStringSet FileNames;
		findDependencyFiles( MemoryStream.str(), FileNames );

		SVString XmlFileName( rFileName );
		XmlFileName += SvO::XmlExtension;
		FileNames.insert( XmlFileName );
		writeStringToFile( XmlFileName, MemoryStream.str(), true );

		SVString ZipFileName( rFileName );
		ZipFileName += SvO::ZipExtension;
		ZipHelper::makeZipFile( ZipFileName, FileNames, true );
	}

	return Result;
}

void ToolClipboard::writeBaseAndEnvironmentNodes( SVObjectXMLWriter& rXmlWriter ) const
{
	_variant_t xmlnsValue;
	_variant_t Value;

	xmlnsValue.SetString( SvO::SvrNameSpace );

	Value.SetString( SvO::SV_BaseNode );

	rXmlWriter.StartElement( BaseTag );
	rXmlWriter.ElementAttribute( SvO::XmlNameSpace, xmlnsValue );
	rXmlWriter.ElementAttribute(TypeTag, Value );

	Value.Clear();
	Value = TheSVObserverApp.getCurrentVersion();

	rXmlWriter.StartElement( CTAG_ENVIRONMENT  );
	rXmlWriter.WriteAttribute( CTAG_VERSION_NUMBER, Value );
	rXmlWriter.EndElement();
}

void ToolClipboard::writeSourceGuids( SVObjectXMLWriter& rXmlWriter, SVToolClass& rTool ) const
{
	_variant_t Value;

	Value.Clear();
	Value = SVGUID( m_rInspection.GetUniqueObjectID() ).ToVARIANT();
	rXmlWriter.WriteAttribute( m_rInspection.GetObjectName(), Value );

	Value.Clear();
	Value = SVGUID( rTool.GetClassID() ).ToVARIANT();
	rXmlWriter.WriteAttribute( ToolTypeTag, Value );

	SVGUID ToolImageGuid( GUID_NULL );
	SVInObjectInfoStruct* pImageInfo = nullptr;
	SVInObjectInfoStruct* pLastImageInfo = nullptr;

	while( nullptr == pImageInfo && S_OK ==  rTool.FindNextInputImageInfo( pImageInfo, pLastImageInfo ) )
	{
		if( nullptr != pImageInfo )
		{
			if( pImageInfo->IsConnected() && nullptr != pImageInfo->GetInputObjectInfo().PObject )
			{
				ToolImageGuid = pImageInfo->GetInputObjectInfo().PObject->GetUniqueObjectID();
			}
		}
		else
		{
			pLastImageInfo = pImageInfo;
			pImageInfo = nullptr;
		}
	}
	Value.Clear();
	Value = ToolImageGuid.ToVARIANT();
	rXmlWriter.WriteAttribute( ToolImageTag, Value );
}

void ToolClipboard::findDependencyFiles( const std::string& rToolXmlString, SVStringSet& rDependencyFiles ) const
{
	size_t StartPos( 0 );
	size_t EndPos( 0 );
	SVString SearchString( SvStl::GlobalPath::Inst().GetRunPath().c_str() );
	SearchString += _T("\\");

	StartPos = rToolXmlString.find( SearchString.c_str(), EndPos );
	while( std::string::npos != StartPos )
	{
		EndPos = rToolXmlString.find( DataTag, StartPos );
		if( std::string::npos != EndPos )
		{
			SVString FileName;
			FileName = rToolXmlString.substr( StartPos, EndPos-StartPos );
			rDependencyFiles.insert( FileName );

			StartPos = rToolXmlString.find( SearchString.c_str(), EndPos );
		}
		else
		{
			StartPos = std::string::npos;
		}
	}
}

void ToolClipboard::updateDependencyFiles( const SVStringSet& rDependencyFiles ) const
{
	SVString XmlFileName( SvO::ClipboardFileName );
	XmlFileName += SvO::XmlExtension;

	SVStringSet::const_iterator Iter( rDependencyFiles.begin() );
	while( rDependencyFiles.end() != Iter )
	{
		//We need to ignore the XML file it is not a dependency file
		if( SVString::npos == Iter->find( XmlFileName.c_str() ) )
		{
			_TCHAR Name[_MAX_FNAME];
			_TCHAR Extension[_MAX_EXT];
			_splitpath( Iter->c_str(), nullptr, nullptr, Name, Extension );

			SVString DestinationFile( SvStl::GlobalPath::Inst().GetRunPath().c_str());
			DestinationFile += _T("\\");
			DestinationFile += Name;
			DestinationFile += Extension;

			if( 0 == ::_access_s( DestinationFile.c_str(), 0 ) )
			{

				::DeleteFile( Iter->c_str() );
			}
			else
			{
				::MoveFile( Iter->c_str(), DestinationFile.c_str() );
			}
		}
		++Iter;
	}
}

HRESULT ToolClipboard::convertClipboardDataToString( SVString& rClipboardData )
{
	HRESULT Result( S_OK );

	if( ::OpenClipboard( AfxGetMainWnd()->GetSafeHwnd() ) )
	{
		UINT  ClipboardFormat( ::RegisterClipboardFormat( SvO::ToolClipboardFormat ) );
		HGLOBAL ClipboardMemData = nullptr;

		if( nullptr == (ClipboardMemData = ::GetClipboardData( ClipboardFormat )) )
		{
			::CloseClipboard();

			Result = S_FALSE;
			SvStl::MessageMgrStd e( SvStl::DataOnly );
			e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvOi::Tid_GetClipboardDataFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25003_GetClipboardData );
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

void ToolClipboard::writeStringToFile( const SVString& rFileName, const std::string& rFileData, bool Text ) const
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

void ToolClipboard::readFileToString( const SVString& rFileName, SVString& rFileData ) const
{
	std::ifstream FileStream;

	FileStream.open( rFileName.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate );
	if( FileStream.is_open() )
	{
		SVString FileData;
		size_t FileSize( 0 );
		FileSize = static_cast<size_t> (FileStream.tellg());
		rFileData.resize( FileSize );
		FileStream.seekg ( 0, std::ios::beg );
		FileStream.read ( &rFileData.at( 0 ), FileSize );
		FileStream.close();
	}
}

HRESULT ToolClipboard::convertXmlToTree( const SVString& rXmlData, SVTreeType& rTree ) const
{
	SvXml::SaxXMLHandler<SVTreeType>  SaxHandler;
	HRESULT	Result =SaxHandler.BuildFromXMLString (&rTree, _variant_t(rXmlData.c_str()));
	if( false == SUCCEEDED(Result))
	{
		SvStl::MessageMgrStd e( SvStl::DataOnly );
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvOi::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25004_ClipboardDataConversion );
		e.Throw();
	}
	return Result;
}

HRESULT ToolClipboard::checkVersion( SVTreeType& rTree ) const
{
	HRESULT Result( S_FALSE );

	SVTreeType::SVBranchHandle EnvironmentItem = nullptr;

	if( SVNavigateTree::GetItemBranch( rTree, CTAG_ENVIRONMENT, nullptr, EnvironmentItem ) )
	{
		_variant_t ClipboardVersion;

		SVNavigateTree::GetItem( rTree, CTAG_VERSION_NUMBER, EnvironmentItem, ClipboardVersion );

		//Clipboard SVObserver version and current version must be the same
		if ( VT_UI4 == ClipboardVersion.vt && TheSVObserverApp.getCurrentVersion() == ClipboardVersion.ulVal )
		{
			Result = S_OK;
		}
	}

	if( S_OK != Result )
	{
		SvStl::MessageMgrStd e( SvStl::DataOnly );
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvOi::Tid_Clipboard_VersionMismatch, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25005_VersionMismatch );
		e.Throw();
	}
	return Result;
}

HRESULT ToolClipboard::validateGuids( SVString& rXmlData, SVTreeType& rTree, int ToolListindex ) const
{
	HRESULT Result( S_OK );

	SVTreeType::SVBranchHandle ToolsItem = nullptr;

	if( SVNavigateTree::GetItemBranch( rTree, ToolsTag, nullptr, ToolsItem ) )
	{
		_variant_t Inspection;
		_variant_t ToolType;
		_variant_t ToolImage;

		SVNavigateTree::GetItem( rTree, m_rInspection.GetObjectName(), ToolsItem, Inspection );
		SVNavigateTree::GetItem( rTree, ToolTypeTag, ToolsItem, ToolType );
		SVNavigateTree::GetItem( rTree, ToolImageTag, ToolsItem, ToolImage );

		SVGUID InspectionGuid( Inspection );
		SVGUID ToolTypeGuid( ToolType );
		SVGUID ToolImageGuid( ToolImage );

		//Color tool can not be inserted into non color system
		if( SVColorToolClassGuid == ToolTypeGuid && !m_rInspection.IsColorCamera() )
		{
			Result = S_FALSE;
			SvStl::MessageMgrStd e( SvStl::DataOnly );
			e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvOi::Tid_ColorToolInsertFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25006_ColorToolInsert );
			e.Throw();
		}
		//Only color tools are allowed to be the first tool in a color system
		else if( 0 == ToolListindex && SVColorToolClassGuid != ToolTypeGuid && m_rInspection.IsColorCamera() )
		{
			Result = S_FALSE;
			SvStl::MessageMgrStd e( SvStl::DataOnly );
			e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvOi::Tid_NonColorToolInsertFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25007_NonColorToolInsert );
			e.Throw();
		}
		else
		{
			//Color Tool do not use the ToolImage, for Color Tool the ToolImageId should not be changed because the ID are used also from Band0
			if( SVColorToolClassGuid != ToolTypeGuid )
			{
				bool useStandardImage = true;
				SVObjectClass* pImage = SVObjectManagerClass::Instance().GetObject( ToolImageGuid );
				if( nullptr !=  pImage  )
				{
					SVToolClass* pTool = dynamic_cast<SVToolClass*> (pImage->GetAncestor( SVToolObjectType ));
					if( nullptr != pTool && nullptr != m_rInspection.GetToolSet() )
					{
						int ImageToolsetIndex = m_rInspection.GetToolSet()->GetIndex( pTool );
						if(ImageToolsetIndex < ToolListindex && InspectionGuid == m_rInspection.GetUniqueObjectID())
						{
							useStandardImage = false;
						}
					}
				}

				if( useStandardImage && nullptr != m_rInspection.GetToolSetMainImage() )
				{
					SVGUID DefaultImageGuid( GUID_NULL );
					DefaultImageGuid = m_rInspection.GetToolSetMainImage()->GetUniqueObjectID();
					SvUl_SF::searchAndReplace( rXmlData, ToolImageGuid.ToString().c_str(),  DefaultImageGuid.ToString().c_str() );
				}
			}
		}
	}
	else
	{
		Result = S_FALSE;
		SvStl::MessageMgrStd e( SvStl::DataOnly );
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvOi::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25008_ClipboardDataConversion );
		e.Throw();
	}

	return Result;
}

HRESULT ToolClipboard::replaceToolName( SVString& rXmlData, SVTreeType& rTree ) const
{
	HRESULT Result( S_FALSE );

	SVTreeType::SVBranchHandle ToolsItem( nullptr );

	if( SVNavigateTree::GetItemBranch( rTree, ToolsTag, nullptr, ToolsItem ) )
	{
		SVTreeType::SVBranchHandle ToolItem( nullptr );

		ToolItem = rTree.getFirstBranch( ToolsItem );

		if( rTree.isValidBranch( ToolItem ) )
		{
			_variant_t ObjectName;

			SVNavigateTree::GetItem( rTree, scObjectNameTag, ToolItem, ObjectName);
			SVString ToolName = SvUl_SF::createSVString( ObjectName.bstrVal );
			SVString NewName;

			SVIPDoc* pDoc = TheSVObserverApp.GetIPDoc( m_rInspection.GetUniqueObjectID() );
			if( nullptr != pDoc )
			{
				// Check to make sure the tool name is unique
				NewName = pDoc->CheckName( ToolName );
			}
			if( NewName != ToolName )
			{
				SvUl_SF::searchAndReplace( rXmlData, ToolName.c_str(), NewName.c_str() );
			}
			Result = S_OK;
		}
	}

	if( S_OK != Result )
	{
		SvStl::MessageMgrStd e( SvStl::DataOnly );
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvOi::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25009_ClipboardDataConversion );
		e.Throw();
	}

	return Result;
}

HRESULT ToolClipboard::replaceUniqueGuids( SVString& rXmlData, SVTreeType& rTree ) const
{
	HRESULT Result( S_FALSE );

	SVTreeType::SVBranchHandle ToolsItem = nullptr;

	if( SVNavigateTree::GetItemBranch( rTree, ToolsTag, nullptr, ToolsItem ) )
	{
		SVStringSet UniqueIDList;

		rTree.getLeafValues( ToolsItem, SVString( scUniqueReferenceIDTag ), UniqueIDList);

		SVStringSet::iterator Iter( UniqueIDList.begin() );
		//Replace each uniqueID with a new GUID
		SVGUID newUniqueID( GUID_NULL );
		while( UniqueIDList.end() != Iter )
		{
			CoCreateGuid( &newUniqueID );
			SvUl_SF::searchAndReplace( rXmlData, Iter->c_str(), newUniqueID.ToString().c_str() );
			++Iter;
		}

		Result = S_OK;
	}
	else
	{
		Result = S_FALSE;
		SvStl::MessageMgrStd e( SvStl::DataOnly );
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvOi::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25010_ClipboardDataConversion );
		e.Throw();
	}

	return Result;
}

HRESULT ToolClipboard::parseTreeToTool( SVTreeType& rTree, SVGUID& rToolGuid )
{
	HRESULT Result( S_FALSE );

	SVTreeType::SVBranchHandle ToolsItem( nullptr );

	if( SVNavigateTree::GetItemBranch( rTree, ToolsTag, nullptr, ToolsItem ) )
	{
		SVTreeType::SVBranchHandle ToolItem( nullptr );

		ToolItem = rTree.getFirstBranch( ToolsItem );

		if ( rTree.isValidBranch( ToolItem ) )
		{
			_variant_t UniqueID;

			SVNavigateTree::GetItem( rTree, scUniqueReferenceIDTag, ToolItem, UniqueID);

			SVToolSetClass* pToolset( m_rInspection.GetToolSet() );
			if( nullptr != pToolset )
			{
				SVParserProgressDialog ParserProgressDialog( SvO::InsertingTool, AfxGetMainWnd() );
				unsigned long parserHandle = SVObjectScriptParserClass::GetParserHandle();

				SVObjectScriptParserClass* pParser = new SVObjectScriptParserClass(new SVInspectionTreeParser< SVTreeType >(rTree, ToolItem, parserHandle, pToolset->GetUniqueObjectID(), pToolset, &ParserProgressDialog));
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
		SvStl::MessageMgrStd e( SvStl::DataOnly );
		e.setMessage( SVMSG_SVO_51_CLIPBOARD_WARNING, SvOi::Tid_ClipboardDataConverionFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25011_ClipboardDataConversion );
		e.Throw();
	}
	return Result;
}
#pragma endregion Protected Methods

