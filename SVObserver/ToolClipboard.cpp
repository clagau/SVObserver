//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Tool clipboard
//* .File Name       : $Workfile:   ToolClipboard.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   19 Dec 2014 04:20:18  $
//* ----------------------------------------------------------------------------
//* This class is used to write and read the selected tool to and from the clipboard
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <sstream>
#include "ToolClipboard.h"
#include "SVObjectLibrary/SVObjectXMLWriter.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVXMLLibrary/SVXML2TreeConverter.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVInspectionLevelCreateStruct.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "SVSystemLibrary/SVEncodeDecodeUtilities.h"
#include "SVObserver.h"
#include "SVInspectionProcess.h"
#include "SVObjectScriptParser.h"
#include "SVToolSet.h"
#include "SVTool.h"
#include "SVParserProgressDialog.h"
#include "SVInspectionTreeParser.h"
#include "SVIPDoc.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const UINT GRA_ErrorBase = 25000;
static const UINT Err_25000 = GRA_ErrorBase + 0;
static const UINT Err_25001 = GRA_ErrorBase + 1;
static const UINT Err_25002 = GRA_ErrorBase + 2;
static const UINT Err_25003 = GRA_ErrorBase + 3;
static const UINT Err_25004 = GRA_ErrorBase + 4;
static const UINT Err_25005 = GRA_ErrorBase + 5;
static const UINT Err_25006 = GRA_ErrorBase + 6;
static const UINT Err_25007 = GRA_ErrorBase + 7;
static const UINT Err_25008 = GRA_ErrorBase + 8;
static const UINT Err_25009 = GRA_ErrorBase + 9;
static const UINT Err_25010 = GRA_ErrorBase + 10;

static const TCHAR ToolClipboardFormat[] = _T("SVO-Tool");
static const TCHAR TempFolder[] = _T("C:\\Temp");
static const TCHAR RunFolder[] = _T("C:\\RUN");
static const TCHAR ClipboardFileName[] = _T("ClipboardFile");
static const TCHAR ZipExtension[] = _T(".zip");
static const TCHAR XmlExtension[] = _T(".xml");
static const TCHAR XmlNameSpace[] = _T("xmlns");
static const TCHAR SvrNameSpace[] = _T("x-schema:#SVR00001");
static const TCHAR SV_BaseNode[] = _T("SV_BASENODE");
static const TCHAR ToolCopyTag[] = _T( "Tool-Copy");
static const TCHAR ToolsTag[] = _T("Tools");
static const TCHAR ToolTypeTag[] = _T("ToolType");
static const TCHAR ToolImageTag[] = _T("ToolImage");
static const TCHAR DependencyFilesTag[] =  _T("DependencyFiles");
static const TCHAR BaseTag[] = _T("Base");
static const TCHAR TypeTag[] = _T("Type");
static const TCHAR ElementTag[] = _T("Element");
static const TCHAR DataTag[] = _T("</DATA>");
static const TCHAR InsertingTool[] = _T("Inserting Tool ...");

static const TCHAR SetClipboardDataFailed[] = _T("Setting the data into the clipboard failed.");
static const TCHAR ClipboardMemoryFailed[] = _T("Clipboard memory allocation failed.");
static const TCHAR GetClipboardDataFailed[] = _T("Failed to retrieve the clipboard data.");
static const TCHAR ToolInvalid[] = _T("The tool to be copied is invalid.");
static const TCHAR ClipboardDataConverionFailed[] = _T("The clipboard data conversion failed.");
static const TCHAR VersionMismatch[] = _T("The current SVObserver Version does not match the version coming from the clipboard.");
static const TCHAR ColorToolInsertFailed[] = _T("A color tool cannot be inserted into a none color system.");
static const TCHAR NonColorToolInsertFailed[] = _T("A color tool must always be the first tool in a color system.");
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
			UINT  ClipboardFormat( ::RegisterClipboardFormat( ToolClipboardFormat ) );

			if( ::EmptyClipboard() && 0 != ClipboardFormat )
			{
				SVString FileName( TempFolder );

				FileName += _T("\\");
				FileName += ClipboardFileName;
				Result = streamToolToZip( FileName, rToolGuid );

				if( S_OK == Result)
				{
					SVString FileData;

					FileName += ZipExtension;
					readFileToString( FileName, FileData );
					::DeleteFile( FileName.c_str() );
					HGLOBAL ClipboardData = GlobalAlloc( GMEM_MOVEABLE,	FileData.size() + 1 );

					if( NULL != ClipboardData )
					{
						memcpy( GlobalLock( ClipboardData ), &FileData.at( 0 ), FileData.size() );
						GlobalUnlock( ClipboardData );

						if( NULL == ::SetClipboardData( ClipboardFormat, ClipboardData ) )
						{
							Result = S_FALSE;
							SVException Exception;
							SETEXCEPTION5( Exception, SVMSG_SVO_51_CLIPBOARD_ERROR, Err_25000, SetClipboardDataFailed );
							throw Exception;
						}
					}
					else
					{
						Result = S_FALSE;
						SVException Exception;
						SETEXCEPTION5( Exception, SVMSG_SVO_51_CLIPBOARD_ERROR, Err_25001, ClipboardMemoryFailed );
						throw Exception;
					}
				}
			}
			::CloseClipboard();
		}
	}
	catch( SVException e )
	{
		::MessageBox( AfxGetMainWnd()->GetSafeHwnd(), e.what().c_str(), NULL, MB_ICONEXCLAMATION | MB_OK );
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
			SVString FileName( TempFolder );

			FileName += _T("\\");
			FileName += ClipboardFileName;
			FileName += ZipExtension;
			writeStringToFile( FileName, ClipboardData.ToDataType(), false );

			SVStringSet ZippedFiles;
			ZipHelper::unzipAll( FileName, SVString( TempFolder ), ZippedFiles );
			::DeleteFile( FileName.c_str() );
			updateDependencyFiles( ZippedFiles );
			
			//Same file name just different extensions
			FileName.replace( ZipExtension, XmlExtension );
			readFileToString( FileName, XmlData );
			XmlData.append( _T("\0") );
			::DeleteFile( FileName.c_str() );
		}

		SVMaterialsTree LocalTree;
		SVXMLMaterialsTree Tree( LocalTree );

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
			replaceUniqueGuids( XmlData, Tree );
			Tree.Clear();
			Result = convertXmlToTree( XmlData, Tree );
		}
		if( S_OK == Result )
		{
			Result = parseTreeToTool( Tree, rToolGuid );
		}
	}
	catch( SVException e )
	{
		::MessageBox( AfxGetMainWnd()->GetSafeHwnd(), e.what().c_str(), NULL, MB_ICONEXCLAMATION | MB_OK );
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
		if( ClipboardData.empty() )
		{
			Result = true;
		}
	}
	catch( SVException e )
	{
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
		SVException Exception;
		SETEXCEPTION5( Exception, SVMSG_SVO_51_CLIPBOARD_ERROR, Err_25003, ToolInvalid );
		throw Exception;
	}
	else
	{
		std::ostringstream MemoryStream;
		SVObjectXMLWriter XmlWriter( MemoryStream );

		SVString rootNodeName( ToolCopyTag );
		XmlWriter.WriteRootElement( rootNodeName );
		XmlWriter.WriteSchema();
		writeBaseAndEnvironmentNodes( XmlWriter );

		XmlWriter.StartElement( ToolsTag );
		writeSourceGuids( XmlWriter, *pTool );
		pTool->Persist( XmlWriter );

		XmlWriter.EndAllElements();

		SVStringSet FileNames;
		findDependencyFiles( MemoryStream.str(), FileNames );

		SVString XmlFileName( rFileName );
		XmlFileName += XmlExtension;
		FileNames.insert( XmlFileName );
		writeStringToFile( XmlFileName, MemoryStream.str(), true );

		SVString ZipFileName( rFileName );
		ZipFileName += ZipExtension;
		ZipHelper::makeZipFile( ZipFileName, FileNames, true );
	}

	return Result;
}

void ToolClipboard::writeBaseAndEnvironmentNodes( SVObjectXMLWriter& rXmlWriter ) const
{
	_variant_t xmlnsValue;
	_variant_t Value;

	xmlnsValue.SetString( SvrNameSpace );

	Value.SetString( SV_BaseNode );

	rXmlWriter.StartElement( BaseTag );
	rXmlWriter.ElementAttribute( XmlNameSpace, xmlnsValue );
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
	SVInObjectInfoStruct* pImageInfo = NULL;
	SVInObjectInfoStruct* pLastImageInfo = NULL;

	while( pImageInfo == NULL && rTool.FindNextInputImageInfo( pImageInfo, pLastImageInfo ) == S_OK )
	{
		if( pImageInfo != NULL )
		{
			if( pImageInfo->IsConnected() && nullptr != pImageInfo->GetInputObjectInfo().PObject )
			{
				ToolImageGuid = pImageInfo->GetInputObjectInfo().PObject->GetUniqueObjectID();
			}
		}
		else
		{
			pLastImageInfo = pImageInfo;
			pImageInfo = NULL;
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
	SVString SearchString( RunFolder );
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
	SVString XmlFileName( ClipboardFileName );
	XmlFileName += XmlExtension;

	SVStringSet::const_iterator Iter( rDependencyFiles.begin() );
	while( rDependencyFiles.end() != Iter )
	{
		//We need to ignore the XML file it is not a dependency file
		if( SVString::npos == Iter->find( XmlFileName.c_str() ) )
		{
			_TCHAR Name[_MAX_FNAME];
			_TCHAR Extension[_MAX_EXT];
			_splitpath( Iter->c_str(), NULL, NULL, Name, Extension );

			SVString DestinationFile( RunFolder );
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
		UINT  ClipboardFormat( ::RegisterClipboardFormat( ToolClipboardFormat ) );
		HGLOBAL ClipboardMemData = NULL;

		if( NULL == (ClipboardMemData = ::GetClipboardData( ClipboardFormat )) )
		{
			::CloseClipboard();

			Result = S_FALSE;
			SVException Exception;
			SETEXCEPTION5( Exception, SVMSG_SVO_51_CLIPBOARD_ERROR, Err_25002, GetClipboardDataFailed );
			throw Exception;
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
		FileStream.seekg ( 0, ios::beg );
		FileStream.read ( &rFileData.at( 0 ), FileSize );
		FileStream.close();
	}
}

HRESULT ToolClipboard::convertXmlToTree( const SVString& rXmlData, SVXMLMaterialsTree& rTree ) const
{
	HRESULT Result( S_OK );

	SVXMLClass Xml;

	::CoInitialize(NULL);
	Result = Xml.Initialize( 0, 0 );
	if( S_OK == Result )
	{
		Xml.PreserveWhitespace( true );

		Result = Xml.CopyXMLTextToDOM( rXmlData.ToBSTR() );
		if ( S_OK == Result )
		{
			Result = SVXML2TreeConverter::CopyToTree( Xml, rTree, BaseTag, false );
		}
	}
	::CoUninitialize();

	if( S_OK != Result )
	{
		SVException Exception;
		SETEXCEPTION5( Exception, SVMSG_SVO_51_CLIPBOARD_ERROR, Err_25004, ClipboardDataConverionFailed );
		throw Exception;
	}
	return Result;
}

HRESULT ToolClipboard::checkVersion( SVXMLMaterialsTree& rTree ) const
{
	HRESULT Result( S_FALSE );

	SVTreeType::SVBranchHandle EnvironmentItem = NULL;

	if( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_ENVIRONMENT, NULL, EnvironmentItem ) )
	{
		_variant_t ClipboardVersion;

		SVNavigateTreeClass::GetItem( rTree, CTAG_VERSION_NUMBER, EnvironmentItem, ClipboardVersion );

		//Clipboard SVObserver version and current version must be the same
		if ( VT_UI4 == ClipboardVersion.vt && TheSVObserverApp.getCurrentVersion() == ClipboardVersion.ulVal )
		{
			Result = S_OK;
		}
	}

	if( S_OK != Result )
	{
		SVException Exception;
		SETEXCEPTION5( Exception, SVMSG_SVO_51_CLIPBOARD_ERROR, Err_25005, VersionMismatch );
		throw Exception;
	}
	return Result;
}

HRESULT ToolClipboard::validateGuids( SVString& rXmlData, SVXMLMaterialsTree& rTree, int ToolListindex ) const
{
	HRESULT Result( S_OK );

	SVTreeType::SVBranchHandle ToolsItem = NULL;

	if( SVNavigateTreeClass::GetItemBranch( rTree, ToolsTag, NULL, ToolsItem ) )
	{
		_variant_t Inspection;
		_variant_t ToolType;
		_variant_t ToolImage;

		SVNavigateTreeClass::GetItem( rTree, m_rInspection.GetObjectName(), ToolsItem, Inspection );
		SVNavigateTreeClass::GetItem( rTree, ToolTypeTag, ToolsItem, ToolType );
		SVNavigateTreeClass::GetItem( rTree, ToolImageTag, ToolsItem, ToolImage );

		SVGUID InspectionGuid( StringToGUID( Inspection ) );
		SVGUID ToolTypeGuid( StringToGUID( ToolType ) );
		SVGUID ToolImageGuid( StringToGUID( ToolImage ) );

		//Color tool can not be inserted into non color system
		if( SVColorToolClassGuid == ToolTypeGuid && !TheSVObserverApp.IsColorSVIM() )
		{
			Result = S_FALSE;
			SVException Exception;
			SETEXCEPTION5( Exception, SVMSG_SVO_51_CLIPBOARD_ERROR, Err_25006, ColorToolInsertFailed );
			throw Exception;
		}
		//Only color tools are allowed to be the first tool in a color system
		else if( 0 == ToolListindex && SVColorToolClassGuid != ToolTypeGuid && TheSVObserverApp.IsColorSVIM() )
		{
			Result = S_FALSE;
			SVException Exception;
			SETEXCEPTION5( Exception, SVMSG_SVO_51_CLIPBOARD_ERROR, Err_25007, NonColorToolInsertFailed );
			throw Exception;
		}
		else
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
				rXmlData.replace( ToolImageGuid.ToString().c_str(),  DefaultImageGuid.ToString().c_str() );
			}
		}
	}
	else
	{
		Result = S_FALSE;
		SVException Exception;
		SETEXCEPTION5( Exception, SVMSG_SVO_51_CLIPBOARD_ERROR, Err_25008, ClipboardDataConverionFailed );
		throw Exception;
	}

	return Result;
}

HRESULT ToolClipboard::replaceUniqueGuids( SVString& rXmlData, SVXMLMaterialsTree& rTree ) const
{
	HRESULT Result( S_FALSE );

	SVTreeType::SVBranchHandle ToolsItem = NULL;

	if( SVNavigateTreeClass::GetItemBranch( rTree, ToolsTag, NULL, ToolsItem ) )
	{
		SVStringSet UniqueIDList;

		rTree.getLeafValues( ToolsItem, SVString( scUniqueReferenceIDTag ), UniqueIDList);

		SVStringSet::iterator Iter( UniqueIDList.begin() );
		//Replace each uniqueID with a new GUID
		SVGUID newUniqueID( GUID_NULL );
		while( UniqueIDList.end() != Iter )
		{
			CoCreateGuid( &newUniqueID );
			rXmlData.replace( Iter->c_str(), newUniqueID.ToString().c_str() );
			++Iter;
		}

		Result = S_OK;
	}
	else
	{
		Result = S_FALSE;
		SVException Exception;
		SETEXCEPTION5( Exception, SVMSG_SVO_51_CLIPBOARD_ERROR, Err_25008, ClipboardDataConverionFailed );
		throw Exception;
	}

	return Result;
}

HRESULT ToolClipboard::parseTreeToTool( SVXMLMaterialsTree& rTree, SVGUID& rToolGuid )
{
	HRESULT Result( S_FALSE );

	SVTreeType::SVBranchHandle ToolsItem = NULL;

	if( SVNavigateTreeClass::GetItemBranch( rTree, ToolsTag, NULL, ToolsItem ) )
	{
		SVTreeType::SVBranchHandle ToolItem = NULL;

		rTree.GetFirstBranch( ToolsItem, ToolItem );

		if ( S_OK == rTree.IsValidBranch( ToolItem ) )
		{
			_variant_t ClassID;
			_variant_t ObjectName;
			_variant_t UniqueID;

			SVNavigateTreeClass::GetItem( rTree, scObjectNameTag, ToolItem, ObjectName);
			SVNavigateTreeClass::GetItem( rTree, scClassIDTag, ToolItem, ClassID);
			SVNavigateTreeClass::GetItem( rTree, scUniqueReferenceIDTag, ToolItem, UniqueID);

			SVToolSetClass* pToolset( m_rInspection.GetToolSet() );
			if( nullptr != pToolset )
			{
				SVIPDoc* pDoc = SVObjectManagerClass::Instance().GetIPDoc( m_rInspection.GetUniqueObjectID() );
				SVString NewName;
				if( nullptr != pDoc )
				{
					// Check to make sure the tool name is unique
					NewName = pDoc->CheckName( SVString( ObjectName.bstrVal ).c_str() );
				}
				if( NewName != SVString( ObjectName.bstrVal ) )
				{
					rTree.ReplaceName( ToolItem, ObjectName, _variant_t( NewName.c_str() ));
				}

				SVParserProgressDialog ParserProgressDialog( InsertingTool, AfxGetMainWnd() );
				unsigned long parserHandle = SVObjectScriptParserClass::GetParserHandle();

				SVObjectScriptParserClass* pParser = new SVObjectScriptParserClass(new SVInspectionTreeParser< SVTreeType >(rTree, ToolItem, parserHandle, pToolset->GetUniqueObjectID(), pToolset, &ParserProgressDialog));
				if( pParser != NULL )
				{
					// Set the Parser Object
					ParserProgressDialog.AddParser(parserHandle, pParser);
					ParserProgressDialog.DoModal();

					rToolGuid = StringToGUID( UniqueID );

					Result = S_OK;
				}
			}
		}
	}

	if( S_OK != Result )
	{
		SVException Exception;
		SETEXCEPTION5( Exception, SVMSG_SVO_51_CLIPBOARD_ERROR, Err_25009, ClipboardDataConverionFailed );
		throw Exception;
	}
	return Result;
}
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ToolClipboard.cpp_v  $
 * 
 *    Rev 1.0   19 Dec 2014 04:20:18   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
