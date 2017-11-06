//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVArchiveTool
//* .File Name       : $Workfile:   SVArchiveTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.12  $
//* .Check In Date   : $Date:   17 Dec 2014 11:29:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <io.h>
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVArchiveTool.h"
#include "SVArchiveImageThreadClass.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVSystemLibrary/SVFileNameManagerClass.h"
#include "SVGlobal.h"
#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVImageListClass.h"
#include "SVInspectionProcess.h"
#include "SVMemoryManager.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVToolSet.h"
#include "SVArchiveHeaderEditDlg.h"
#include "SVVisionProcessorHelper.h"
#include "SVValueObjectLibrary/BasicValueObject.h"
#include "TextDefinesSvO.h"
#include "ObjectInterfaces/IValueObject.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma endregion Declarations


SV_IMPLEMENT_CLASS(SVArchiveTool, SVArchiveToolClassGuid);

SVArchiveTool::SVArchiveTool( SVObjectClass* POwner, int StringResourceID )
              : SVToolClass(POwner, StringResourceID)
{
	initializeArchiveTool();
}

void SVArchiveTool::initializeArchiveTool()
{
	m_ArchiveImagePathUsingKW = false;
	m_ImageTranslatedPath = "";

	m_arrayResultsInfoObjectsToArchive.SetArchiveTool( this );
	m_arrayImagesInfoObjectsToArchive.SetArchiveTool( this );

	// Set up your type...
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SVToolArchiveObjectType;
	
	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();
	
	// Register Embedded Objects
	RegisterEmbeddedObject(	
		&m_stringFileArchivePath, 
		SVArchiveFilePathObjectGuid, 
		IDS_OBJECTNAME_ARCHIVE_FILEPATH,
		false, SvOi::SVResetItemTool );
	
	RegisterEmbeddedObject(	
		&m_stringArchiveImageGuids_OBSOLETE, 
		SVArchiveImageGuidsObjectGuid, 
		IDS_OBJECTNAME_ARCHIVE_IMAGE_GUIDS,
		false, SvOi::SVResetItemTool );
	
	RegisterEmbeddedObject(	
		&m_stringArchiveResultGuids_OBSOLETE, 
		SVArchiveResultGuidsObjectGuid, 
		IDS_OBJECTNAME_ARCHIVE_RESULT_GUIDS,
		false, SvOi::SVResetItemTool );
	
	RegisterEmbeddedObject(	
		&m_svoArchiveImageNames, 
		SVArchiveImageNamesObjectGuid, 
		IDS_OBJECTNAME_ARCHIVE_IMAGE_NAMES,
		false, SvOi::SVResetItemTool );
	
	RegisterEmbeddedObject(	
		&m_svoArchiveResultNames, 
		SVArchiveResultNamesObjectGuid, 
		IDS_OBJECTNAME_ARCHIVE_RESULT_NAMES,
		false, SvOi::SVResetItemTool );
	
	RegisterEmbeddedObject(	
		&m_stringImageFileRootPath,
		SVArchiveImageFileRootPathGuid,
		IDS_OBJECTNAME_ARCHIVE_IMAGE_ROOT_PATH,
		false, SvOi::SVResetItemNone );
	
	RegisterEmbeddedObject(	
		&m_dwAppendArchiveFile,
		SVArchiveAppendArchiveFileGuid,
		IDS_OBJECTNAME_ARCHIVE_APPEND_ARCHIVE_FILE,
		false, SvOi::SVResetItemTool );
	m_dwAppendArchiveFile.SetOutputFormat(OutputFormat_int);
	
	RegisterEmbeddedObject(	
		&m_dwArchiveStopAtMaxImages,
		SVArchiveStopAtMaxImagesGuid,
		IDS_OBJECTNAME_ARCHIVE_STOP_AT_MAX_IMAGES,
		false, SvOi::SVResetItemNone );
	m_dwArchiveStopAtMaxImages.SetOutputFormat(OutputFormat_int);
	
	RegisterEmbeddedObject(	
		&m_dwArchiveMaxImagesCount,
		SVArchiveMaxImagesCountGuid,
		IDS_OBJECTNAME_ARCHIVE_MAX_IMAGES_COUNT,
		false, SvOi::SVResetItemNone );
	m_dwArchiveMaxImagesCount.SetOutputFormat(OutputFormat_int);

	RegisterEmbeddedObject(	
		&m_evoArchiveMethod,
		SVArchiveMethodGuid,
		IDS_OBJECTNAME_ARCHIVE_TOOL_METHOD,
		false, SvOi::SVResetItemNone  );

	RegisterEmbeddedObject(
		&m_HeaderLabelNames,
		SVArchiveHeaderLabelGUID,
		IDS_OBJECTNAME_HEADER_LABELS,
		false, SvOi::SVResetItemNone);

	RegisterEmbeddedObject(
		&m_HeaderObjectGUIDs,
		SVArchiveHeaderObjectGUID,
		IDS_OBJECTNAME_HEADER_OBJECT_STRINGS,
		false, SvOi::SVResetItemNone);
	
	RegisterEmbeddedObject(
		&m_bvoUseHeaders,
		SVArchiveUseHeadersGUID,
		IDS_OBJECTNAME_ENABLE_HEADERS,
		false, SvOi::SVResetItemNone);

	// no need to register image buffer
	
//EB???	SetObjectDepth(2);
	
	// Set Embedded defaults
	
	//
	// Use the user changeable tool name as the results archive name.
	//
	SVFileNameClass FileClass;
	FileClass.SetExtension( _T(".txt") );
	FileClass.SetPathName( _T("D:\\TEMP") );
		
	SVString FileName = SvUl_SF::Format( _T("%s__0"), GetName() );
	FileClass.SetFileNameOnly( FileName.c_str() );

	m_stringFileArchivePath.SetDefaultValue( FileClass.GetFullFileName(), true );
	m_stringArchiveImageGuids_OBSOLETE.SetDefaultValue( _T( "" ), true);
	m_stringArchiveResultGuids_OBSOLETE.SetDefaultValue( _T( "" ), true);
	m_svoArchiveImageNames.SetDefaultValue( _T( "" ), true);
	m_svoArchiveResultNames.SetDefaultValue( _T( "" ), true);
	m_stringImageFileRootPath.SetDefaultValue( _T( "D:\\TEMP" ), true);
	m_dwAppendArchiveFile.SetDefaultValue(0, true);
	m_dwArchiveStopAtMaxImages.SetDefaultValue(1, true);
	m_dwArchiveMaxImagesCount.SetDefaultValue(10, true);

	m_evoArchiveMethod.SetDefaultValue( SVArchiveAsynchronous, true);
	SVEnumerateVector vec;
	vec.push_back( SVEnumeratePair(_T("Synchronous"), SVArchiveSynchronous ) );
	vec.push_back( SVEnumeratePair(_T("Change Mode"), SVArchiveGoOffline ) );
	vec.push_back( SVEnumeratePair(_T("Asynchronous"), SVArchiveAsynchronous ) );
	m_evoArchiveMethod.SetEnumTypes(vec);
	
	m_stringArchiveImageGuids_OBSOLETE.SetObjectAttributesAllowed( SvDef::SV_NO_ATTRIBUTES, SvOi::SetAttributeType::OverwriteAttribute );
	m_stringArchiveResultGuids_OBSOLETE.SetObjectAttributesAllowed( SvDef::SV_NO_ATTRIBUTES, SvOi::SetAttributeType::OverwriteAttribute );
	m_svoArchiveImageNames.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::OverwriteAttribute );
	m_svoArchiveResultNames.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::OverwriteAttribute );
	m_HeaderObjectGUIDs.SetObjectAttributesAllowed( SvDef::SV_NO_ATTRIBUTES, SvOi::SetAttributeType::OverwriteAttribute );
	m_bInitializedForRun = false;
	m_eArchiveMethod = SVArchiveInvalidMethod;
	m_uiValidateCount = 0;
}

bool SVArchiveTool::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	if ( bOk )
	{
		long Method = SVArchiveInvalidMethod;
		m_evoArchiveMethod.GetValue( Method );
		m_eArchiveMethod = static_cast<SVArchiveMethodEnum> (Method);

		bOk = true;
	}

	m_HeaderLabelNames.setStatic( true );
	m_HeaderObjectGUIDs.setStatic( true );

	m_stringFileArchivePath.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE | SvDef::SV_REMOTELY_SETABLE & ~SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::AddAttribute );
	m_stringFileArchivePath.SetObjectAttributesAllowed( SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );
	m_stringArchiveImageGuids_OBSOLETE.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_stringArchiveResultGuids_OBSOLETE.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_svoArchiveImageNames.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	m_svoArchiveImageNames.SetObjectAttributesAllowed( SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );
	m_svoArchiveResultNames.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	m_svoArchiveResultNames.SetObjectAttributesAllowed( SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );
	m_stringImageFileRootPath.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE | SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	m_dwAppendArchiveFile.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	m_dwArchiveStopAtMaxImages.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE | SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	m_dwArchiveMaxImagesCount.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE | SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	m_evoArchiveMethod.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE | SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	m_HeaderLabelNames.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE | SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	m_HeaderObjectGUIDs.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_bvoUseHeaders.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	// Override base class exposure of the drawflag
	// This value will not be exposed for the Archive Tool.
	drawToolFlag.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );

	// Override base class exposure of the auxillaryextent variables
	// These values will not be exposed for the Archive Tool.
	m_svUpdateAuxiliaryExtents.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceX.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceY.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceAngle.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliarySourceImageName.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );
	m_svAuxiliaryDrawType.SetObjectAttributesAllowed( SvDef::SV_HIDDEN, SvOi::SetAttributeType::OverwriteAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVArchiveTool::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool result = SVToolClass::ResetObject(pErrorMessages);
	long l_lArchiveMethod = 0;

	m_evoArchiveMethod.GetValue( l_lArchiveMethod );

	m_eArchiveMethod = static_cast<SVArchiveMethodEnum>( l_lArchiveMethod );

	// Put the archive tool text in the thread affinity list.
	if( m_eArchiveMethod == SVArchiveAsynchronous )
	{	// IsAllowed will return S_FALSE if the thread is not found with SVNone.
		if( S_FALSE == SVThreadManager::Instance().IsAllowed(_T( "Archive Tools(Asynchronous)") , SVNone))
		{	// Add thread to list with null handle so the user can set affinity before the thread is created.
			SVThreadManager::Instance().Add( nullptr, _T("Archive Tools(Asynchronous)"), SVAffinityUser );
		}
	}

	SVString Temp;
	m_stringArchiveImageGuids_OBSOLETE.GetValue(Temp);
	if ( Temp.empty() )
	{
		m_arrayImagesInfoObjectsToArchive.InitializeObjects( this, m_svoArchiveImageNames );
		m_arrayImagesInfoObjectsToArchive.ValidateImageObjects();	// makes sure the images are connected as inputs
	}
	else	// pre array value object way
	{
		m_arrayImagesInfoObjectsToArchive.ConvertStringToGuids( this, Temp.c_str() );
	}
	
	m_stringArchiveResultGuids_OBSOLETE.GetValue(Temp);
	if ( Temp.empty() )
	{
		m_arrayResultsInfoObjectsToArchive.InitializeObjects( this, m_svoArchiveResultNames );
		m_arrayResultsInfoObjectsToArchive.ValidateResultsObjects();	// makes sure the results are connected as inputs
	}
	else	// pre array value object way
	{
		m_arrayResultsInfoObjectsToArchive.ConvertStringToGuids( this, Temp.c_str() );
	}

	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());
	if ( pInspection && pInspection->IsResetStateSet( SvDef::SVResetStateArchiveToolCreateFiles ) )
	{
		m_bInitializedForRun = initializeOnRun(pErrorMessages);
		result = result && m_bInitializedForRun;
	}
	else
	{
		m_arrayImagesInfoObjectsToArchive.ResetImageCounts();
		m_arrayImagesInfoObjectsToArchive.ValidateImageObjects();

		result = AllocateImageBuffers(pErrorMessages) && result;
	}

	m_uiValidateCount = 0;

	return ValidateImageSpace(true, pErrorMessages) && result;
}

SVArchiveTool::~SVArchiveTool()
{
	TheSVArchiveImageThreadClass().GoOffline();
}

// Should be overridden and must be called in derived classes...
// Sets Depth of Array for historical purposes 
// (i.e. to get images/results N events ago)
void SVArchiveTool::SetObjectDepth( int NewObjectDepth )
{
	// Set object depth of members here...
	DWORD dwMaxImages;
	m_dwArchiveMaxImagesCount.GetValue( dwMaxImages );

	SVToolClass::SetObjectDepth( NewObjectDepth );
}

// Should be overridden and must be called in derived classes...
// Sets Depth of Array for historical purposes 
// (i.e. to get images/results N events ago)
void SVArchiveTool::SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex )
{
	// Set object depth of members here...
	DWORD dwMaxImages;
	m_dwArchiveMaxImagesCount.GetValue( dwMaxImages );

	SVToolClass::SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
}


// Create a file to store selected results in text format.
//
bool SVArchiveTool::CreateTextArchiveFile(SvStl::MessageContainerVector *pErrorMessages)
{
	bool bValidFile = true;
	//
	// CFile object.
	//
	SVString FileArchivePath;
	GetFileArchive( FileArchivePath );

	if( FileArchivePath.empty() )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_PathEmpty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() ); 
			pErrorMessages->push_back(Msg);
		}
		return false;
	}

	ArchiveToolHelper athFile;
	athFile.Init( FileArchivePath );

	if (athFile.isUsingKeywords())
	{
		if(athFile.isTokensValid())
		{
			FileArchivePath = athFile.TranslatePath( FileArchivePath);
		}
		else
		{
			bValidFile = false;
		}
	}
	
	//
	// Make sure the file is closed. Before creating/opening a new one.
	//
	if (m_fileArchive.m_hFile != CFile::hFileNull)
	{
		m_fileArchive.Close();
	}
	
	if ( ! FileArchivePath.empty() )
	{
		SVFileNameClass svFileName( FileArchivePath.c_str() );

		if ( _access( svFileName.GetPathName().c_str(), 0 ) != 0 )
		{
			if (!SVFileNameManagerClass::Instance().CreatePath( svFileName.GetPathName().c_str() ) )
			{
				if (nullptr != pErrorMessages)
				{
					SVStringVector msgList;
					msgList.push_back( FileArchivePath );
					SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_CreateFileFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10036, GetUniqueObjectID() ); 
					pErrorMessages->push_back(Msg);
				}
				return false;
			}
		}
	}

	//
	// Check for append option selected.
	//
	DWORD appendOption;
	m_dwAppendArchiveFile.GetValue(appendOption);
	
	UINT uOpenFlags = CFile::modeCreate |
	                  CFile::modeReadWrite | 
	                  CFile::shareDenyNone;
	
	if ( appendOption )
	{
		uOpenFlags |= CFile::modeNoTruncate;
	}
	
	BOOL bResult = m_fileArchive.Open( FileArchivePath.c_str(), uOpenFlags, nullptr );

	if(!bResult)
	{
		if (nullptr != pErrorMessages)
		{
			SVStringVector msgList;
			msgList.push_back( FileArchivePath );
			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_CreateFileFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10036, GetUniqueObjectID() ); 
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	
	//
	// Write a date and time stamp to the archive file.
	//
	CTime  timeCurrent;
	timeCurrent = CTime::GetCurrentTime();
	//
	// Format date and time as DD/MM/YYYY HH:MM:SS
	//
	SVString TimeStamp = timeCurrent.Format(_T("%d/%m/%Y %H:%M:%S\r\n"));
	
	//
	// Write the result to the archive file.
	//
	ASSERT(m_fileArchive.m_hFile != CFile::hFileNull);
	try
	{
		//
		// 'Seek' to the end of the file.
		//
		m_fileArchive.SeekToEnd();
		
		m_fileArchive.Write( TimeStamp.c_str(), static_cast<int> (TimeStamp.size()) );

		BOOL bUseHeaders = false;
		HRESULT hr = m_bvoUseHeaders.GetValue( bUseHeaders );
		if( S_OK == hr && bUseHeaders )
		{
			// Write Header
			SVStringVector HeaderVector;
			m_HeaderLabelNames.GetArrayValues( HeaderVector );
			SVString strHeader;
			for( SVStringVector::iterator it = HeaderVector.begin() ; it != HeaderVector.end() ; ++it)
			{
				strHeader += *it;
				if( it+1 != HeaderVector.end())
				{
					strHeader+= _T(",");
				}
			}
			if( !strHeader.empty() )
			{
				strHeader += _T("\r\n");
				m_fileArchive.Write( strHeader.c_str(), static_cast<UINT> (strHeader.size()) );
			}
		}
	} 
	catch (const CException& )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_WriteToFileFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() ); 
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	catch (...)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_WriteToFileFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() ); 
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
		
	return true;
}

void local_remove_items( SVStringVector& rVec, SVStringValueObjectClass& rvo )
{
	SVStringVector NameVector;
	SVStringVector::iterator iterRemoved;

	if ( !rVec.empty() )
	{
		rvo.GetArrayValues( NameVector );

		for ( iterRemoved = rVec.begin(); iterRemoved != rVec.end(); ++iterRemoved )
		{
			SVStringVector::iterator iterName;
			iterName = std::find(NameVector.begin(), NameVector.end(), *iterRemoved );
			if ( iterName != NameVector.end() )
			{
				NameVector.erase( iterName );
			}
		}

		rvo.SetArraySize( static_cast< int >( NameVector.size() ) );
		rvo.SetArrayValues( NameVector );
	}
}


bool SVArchiveTool::initializeOnRun(SvStl::MessageContainerVector *pErrorMessages)
{
	long Method( SVArchiveInvalidMethod );
	m_evoArchiveMethod.GetValue( Method );
	m_eArchiveMethod = static_cast<SVArchiveMethodEnum> (Method);

	SVString Temp;
	GetImageArchivePath( Temp );
	ArchiveToolHelper athImage;
	athImage.Init( Temp );
	
	if (athImage.isUsingKeywords() && athImage.isTokensValid())
	{
		Temp = athImage.TranslatePath( Temp );
	}
	

	if ( !Temp.empty() )
	{
		if ( _access( Temp.c_str(), 0 ) != 0 )
		{
			if (!SVFileNameManagerClass::Instance().CreatePath( Temp.c_str() ))
			{
				if (nullptr != pErrorMessages)
				{
					SVStringVector msgList;
					msgList.push_back( Temp );
					SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_CreatePathFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() ); 
					pErrorMessages->push_back(Msg);
				}
				return false;
			}
		}
	}

	m_arrayImagesInfoObjectsToArchive.SetArchiveTool( this );
	//
	// Make sure the image 'index' for image file creation is reset.
	// 04 Jan 2000 - frb.
	//
	m_arrayImagesInfoObjectsToArchive.ResetImageCounts();
	
	//
	// Validate the image objects to be archived.
	//
	m_arrayImagesInfoObjectsToArchive.ValidateImageObjects();
	
	if ( m_eArchiveMethod == SVArchiveGoOffline || m_eArchiveMethod == SVArchiveAsynchronous )
	{
		if ( !AllocateImageBuffers(pErrorMessages) )
		{
			return false;
		}

		if ( m_eArchiveMethod == SVArchiveAsynchronous )
		{
			HRESULT hrThreadOnline = TheSVArchiveImageThreadClass().GoOnline();
		}
	}

	//
	// Validate the results objects to be archived in text format.
	//
	int nCountResults = m_arrayResultsInfoObjectsToArchive.ValidateResultsObjects();
	
	m_ArchiveStringBuffer.clear();

	//
	// Don't create/open the results archive file if no results to
	// archive.
	//
	if ( ! nCountResults )
	{
		return true;
	}
	
	//
	// Create a file to store results in text format.
	//
	if ( m_fileArchive.m_hFile != CFile::hFileNull )
	{
		return true;
	}

	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());
	if ( pInspection && pInspection->IsResetStateSet(SvDef::SVResetStateArchiveToolCreateFiles ) )	// If going online
	{
		//
		// Create and open the results to text Archive file.
		//
		return CreateTextArchiveFile(pErrorMessages);
	}
	
	return true;
}

bool SVArchiveTool::AllocateImageBuffers(SvStl::MessageContainerVector *pErrorMessages)
{
	if ( m_eArchiveMethod == SVArchiveGoOffline || m_eArchiveMethod == SVArchiveAsynchronous )
	{
		// async doesn't really allocate any buffers but it does preparation work (initm_ImageInfo)
		DWORD dwMaxImages;
		m_dwArchiveMaxImagesCount.GetValue( dwMaxImages );
		HRESULT hrAllocate = m_arrayImagesInfoObjectsToArchive.AllocateBuffers( dwMaxImages );
		ASSERT( S_OK == hrAllocate );
		if ( S_OK != hrAllocate )
		{
			if (nullptr != pErrorMessages)
			{
				SVStringVector msgList;
				msgList.push_back( SvUl_SF::Format(_T("%x"), hrAllocate) );
				SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ArchiveTool_AllocImageFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() ); 
				pErrorMessages->push_back(Msg);
			}
			return false;
		}
		
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
bool SVArchiveTool::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	if( ValidateOnRun(pErrorMessages) && __super::onRun( rRunStatus, pErrorMessages ) )
	{
		//
		// If this is not a test mode or run mode (online) no work is required.
		// Usually a condition where user is 'moving' a tool that causes the
		// toolset to 'run'.
		//
		if ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) )
		{
			return true;
		}
		
		if (!m_bInitializedForRun)
		{
			rRunStatus.SetFailed();
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ArchiveTool_InitFlagFalse, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
			return false;
		}

		//
		// Iterate the array of objects to archive and build a string.
		//
		SVString Archive = m_arrayResultsInfoObjectsToArchive.BuildResultsArchiveString();

		if (!Archive.empty())
		{
			Archive += _T("\r\n");


			if (true)	// always do this for now (never buffer text) see FR00100.007.0460.xxx
			{
				//
				// Write the result to the archive file.
				// The file may not be open at this time if a test 'run'
				// is active for one cycle.
				//
				if (m_fileArchive.m_hFile != CFile::hFileNull)
				{
					try
					{
						m_fileArchive.Write(Archive.c_str(), static_cast<int> (Archive.size()));
					}
					catch (CException* e)
					{
						rRunStatus.SetFailed();
						e->Delete();
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
							pErrorMessages->push_back(Msg);
						}
						return false;
					}
					catch (...)
					{
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownException, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
							pErrorMessages->push_back(Msg);
						}
						rRunStatus.SetFailed();
						return false;
					}
				}
			}// end if ( !bBufferText || m_eArchiveMethod != SVArchiveGoOffline )
			else if (m_eArchiveMethod == SVArchiveGoOffline)
			{
				QueueArchiveString(Archive);
			}
		}

#if defined (TRACE_THEM_ALL) || defined (TRACE_ARCHIVE)
		TRACE(_T("SVArchiveTool::onRun-WriteArchiveImageFiles-Name=%s\n"), GetCompleteName());
#endif
		//
		// Iterate the list of images to archive.
		//
		m_arrayImagesInfoObjectsToArchive.WriteArchiveImageFiles();

		rRunStatus.SetPassed();

		return true;
	}
	
	rRunStatus.SetInvalid();
	return false;
}

HRESULT SVArchiveTool::QueueArchiveString( const SVString& rArchiveString )
{
	HRESULT hr = S_OK;
	m_ArchiveStringBuffer.push_back( rArchiveString );
	return hr;
}

// Called by SVToolAdjustmentArchivePage::OnInitDialog().
//
void SVArchiveTool::UpdateTaskObjectOutputList()
{
	SVToolSetClass* pToolSet = dynamic_cast<SVInspectionProcess*>(GetInspection())->GetToolSet();
	ASSERT( pToolSet );
	SVOutputInfoListClass l_ToolSetOutputList;
	SVObjectReferenceVector vecObjects;

	pToolSet->GetOutputList( l_ToolSetOutputList );

	l_ToolSetOutputList.GetObjectReferenceList( vecObjects );

	//
	// Iterate the tool set output list and reset the archivable attribute
	// bits.
	//
	int nCount = static_cast< int >( vecObjects.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVObjectReference ObjectRef = vecObjects.at(i);
		
		//
		// Clear the existing archivable bits that might be set.
		//
		ObjectRef.SetObjectAttributesSet( SvDef::SV_ARCHIVABLE, SvOi::SetAttributeType::RemoveAttribute );
		
	}
	//
	// Iterate the current list of results to archive.
	//
	int nCountResults = m_arrayResultsInfoObjectsToArchive.GetSize();
	for (int i = 0; i < nCountResults; i++)
	{
		// SVObjectInfoStruct InfoItemArchive = 
		//	arrayResultsInfoObjectsToArchive.GetAt(k);
		SVObjectReference ObjectRef = m_arrayResultsInfoObjectsToArchive.GetAt( i )->GetObjectReference();

		if (ObjectRef.ObjectAttributesAllowed() & SvDef::SV_ARCHIVABLE)
		{
			ObjectRef.SetObjectAttributesSet( SvDef::SV_ARCHIVABLE, SvOi::SetAttributeType::AddAttribute );
		}
	}
}

//  Called by SVToolAdjustmentArchivePage::OnOK().
//
void SVArchiveTool::RebuildResultsArchiveList()
{
	//
	// Now iterate the output list and build a list for the archive tool.
	// Get a pointer to the toolset
	//
	SVToolSetClass* pToolSet = dynamic_cast<SVInspectionProcess*>(GetInspection())->GetToolSet();

	SVOutputInfoListClass l_ToolSetOutputList;

	pToolSet->GetOutputList( l_ToolSetOutputList );

	//
	// Find all outputs marked as selected for archivable
	// Note:: both Lists must be from the same root object
	// which at this point is SVToolSetClass
	//
	m_arrayResultsInfoObjectsToArchive.DisconnectAllResultObjects();
	m_arrayResultsInfoObjectsToArchive.ClearArray();

	SVObjectReferenceVector vecObjects;

	l_ToolSetOutputList.GetSetAttributesList( SvDef::SV_ARCHIVABLE, vecObjects );
	
	m_svoArchiveResultNames.SetArraySize( static_cast<int> (vecObjects.size()) );

	for ( int i = 0; i < static_cast<int> (vecObjects.size()); i++ )
	{
		const SVObjectReference& rObjectRef = vecObjects[i];

		SVArchiveRecord* pArchiveRecord = new SVArchiveRecord;
		pArchiveRecord->Init( this );
		pArchiveRecord->GetObjectReference() = rObjectRef;
		
		m_arrayResultsInfoObjectsToArchive.Add( pArchiveRecord );
		pArchiveRecord->ConnectInputObject();

		m_svoArchiveResultNames.SetValue( rObjectRef.GetCompleteName(), i );
	}

	m_stringArchiveResultGuids_OBSOLETE.SetValue( SVString() );
}

void SVArchiveTool::AddImageToArray( SVImageClass* PImage )
{
	SVArchiveRecord* pImageRecord = new SVArchiveRecord;
	pImageRecord->InitArchiveRecord(this, SVObjectReference( PImage ) );
	pImageRecord->ConnectInputObject();
	m_arrayImagesInfoObjectsToArchive.Add(pImageRecord);
}

// Called by SVToolAdjustmentArchivePage::OnOK();
//
// Builds a new list of images to archive based on the Attributes set as
// selected to be archived.
//
void SVArchiveTool::RebuildImageArchiveList()
{
	m_arrayImagesInfoObjectsToArchive.ClearArray();
	
	//
	// Get a pointer to the toolset
	//

	
	SVObjectTypeInfoStruct  info;

	info.ObjectType = SVImageObjectType;
	info.SubType = SVNotSetSubObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetInspection()->GetUniqueObjectID() );

	SVObjectReferenceVector vecImages;

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		if( ( nullptr != pImage ) && ( pImage->ObjectAttributesSet() & SvDef::SV_ARCHIVABLE_IMAGE ) )
		{
			AddImageToArray( pImage );
			pImage->SetObjectAttributesSet( SvDef::SV_ARCHIVABLE_IMAGE, SvOi::SetAttributeType::RemoveAttribute );

			vecImages.push_back( pImage );
		}
	}
	
	
	m_svoArchiveImageNames.SetArraySize( static_cast< int >( vecImages.size() ) );

	for ( int i = 0; i < static_cast< int >( vecImages.size() ); i++ )
	{
		const SVObjectReference& rObjectRef = vecImages[i];

		m_svoArchiveImageNames.SetValue( rObjectRef.GetCompleteName(), i  );
	}

	m_stringArchiveImageGuids_OBSOLETE.SetValue( SVString() );
}

// Update the attributes for archivable images from the current list of
// images to archive. The pImageList is a collection of images that will
// be used by the Archive Tool Property Page to display a 'tree' in a
// tree list control.
//
void SVArchiveTool::SetImageAttributesFromArchiveList( SVImageListClass* pImageList )
{
	if (!pImageList)
	{
		return;
	}
	int nCount = static_cast< int >( pImageList->GetSize() );
	for (int i = 0; i < nCount; i++)
	{
		SVImageClass* pImage = pImageList->GetAt(i);
		if (pImage)
		{
			SVString ImageName;
			ImageName = pImage->GetCompleteName();
			
			int nCount2 = m_arrayImagesInfoObjectsToArchive.GetSize();
			for (int k = 0; k < nCount2; k++)
			{
				SVArchiveRecord* pRecordImage = m_arrayImagesInfoObjectsToArchive.GetAt(k);
				ASSERT(pRecordImage);
				SVImageClass* pImage2 = dynamic_cast <SVImageClass*> ( pRecordImage->GetObjectReference().getObject() );
				if (pImage2)
				{
					//
					// Compare pointers here - we need something more solid like
					// a GUID compare.
					//
					SVString ImageName2;
					ImageName2 = pImage2->GetCompleteName();
					
					if ( 0 == SvUl_SF::CompareNoCase( ImageName, ImageName2) )          // Equal?
					{
						if (pImage->ObjectAttributesAllowed() & SvDef::SV_ARCHIVABLE_IMAGE)
						{
							pImage->SetObjectAttributesSet( SvDef::SV_ARCHIVABLE_IMAGE, SvOi::SetAttributeType::AddAttribute );
							//
							// Update the pointer to the image..incase..
							//
							pRecordImage->GetObjectReference() = pImage->GetObjectInfo().GetObjectReference();
						}
					}
				}// end if (pImage2)
			}// end for (int k = 0; k < nCount2; k++)
		}// end if (pImage)
	}// end for (int i = 0; i < nCount; i++)
}

bool SVArchiveTool::GetFileArchive( SVString& rName )
{
	bool Result = (S_OK == m_stringFileArchivePath.GetValue( rName ));
	return Result;
}

bool SVArchiveTool::GetImageArchivePath( SVString& rName )
{
	return S_OK == m_stringImageFileRootPath.GetValue( rName );
}

bool SVArchiveTool::SetFileArchive( LPCTSTR lpszName )
{
	return S_OK == m_stringFileArchivePath.SetValue( SVString( lpszName ) );
}

bool SVArchiveTool::SetImageArchivePath( LPCTSTR lpszName )
{
	return S_OK == m_stringImageFileRootPath.SetValue( SVString( lpszName ) );
}


HRESULT SVArchiveTool::WriteBuffers()
{
	HRESULT hr = S_OK;

	// write text
	if (m_fileArchive.m_hFile != CFile::hFileNull)
	{
		try
		{
			SVStringVector::iterator iter;
			for ( iter = m_ArchiveStringBuffer.begin(); iter != m_ArchiveStringBuffer.end(); ++iter )
			{
				m_fileArchive.Write( iter->c_str(), static_cast<int> (iter->size()) );
			}
		} 
		catch (CException* e)
		{
			e->Delete();
			hr = E_FAIL;
		} 
		catch (...)
		{
			hr = E_FAIL;
		}
	}

	// write images

	HRESULT hrImages = m_arrayImagesInfoObjectsToArchive.WriteImageQueue();
	if ( S_OK == hr )
	{
		hr = hrImages;
	}
	return hr;
}

long SVArchiveTool::CalculateImageMemory( SVImageClass* p_pImage )
{
	ASSERT( p_pImage );
	long lImageSize = 0;
	if ( p_pImage )
	{
		SVImageInfoClass info = p_pImage->GetImageInfo();
		lImageSize = info.GetBufferSize();
	}
	return lImageSize;
}

long SVArchiveTool::CalculateImageMemory( std::vector<SVImageClass*> p_apImages )
{
	long lTotalMemory = 0;
	for ( size_t i=0; i < p_apImages.size(); i++ )
	{
		SVImageClass* pImage = p_apImages[i];
		lTotalMemory += CalculateImageMemory( pImage );

	}
	return lTotalMemory;
}

bool SVArchiveTool::isImagePathUsingKeywords()
{
	return m_ArchiveImagePathUsingKW;
}

void SVArchiveTool::getTranslatedImagePath(SVString &rImagePath)
{
	rImagePath = m_ImageTranslatedPath;
}

bool SVArchiveTool::DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo )
{
	if (nullptr != pObjectInInfo)
	{
		SVStringVector vecRemovedImage  = m_arrayImagesInfoObjectsToArchive. RemoveDisconnectedObject( pObjectInInfo->GetInputObjectInfo() );
		SVStringVector vecRemovedResult = m_arrayResultsInfoObjectsToArchive.RemoveDisconnectedObject( pObjectInInfo->GetInputObjectInfo() );

		local_remove_items ( vecRemovedImage, m_svoArchiveImageNames );
		local_remove_items ( vecRemovedResult, m_svoArchiveResultNames );

		return true;
	}
	return false;
}

void SVArchiveTool::goingOffline()
{
	if ( m_eArchiveMethod == SVArchiveAsynchronous )
	{
		TheSVArchiveImageThreadClass().GoOffline();
	}

	if ( m_eArchiveMethod == SVArchiveGoOffline )
	{
		WriteBuffers();
	}

	// Close the text to archive file if necessary, i.e. is open.
	if(m_fileArchive.m_hFile != CFile::hFileNull)
	{
		m_fileArchive.Close();
	}
}

void SVArchiveTool::OnObjectRenamed( const SVObjectClass& rRenamedObject, const SVString& rOldName )
{
	SVString newPrefix;
	SVString oldPrefix;

	if( const SVInspectionProcess* l_pInspection = dynamic_cast<const SVInspectionProcess*> (&rRenamedObject) )
	{
		newPrefix = l_pInspection->GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType ) + _T( "." );
	}
	else if( nullptr != dynamic_cast<const BasicValueObject*> (&rRenamedObject) )
	{
		newPrefix = rRenamedObject.GetCompleteObjectNameToObjectType( nullptr, SVRootObjectType );
	}
	else if( nullptr != dynamic_cast<const SvOi::IValueObject*> (&rRenamedObject) )
	{
		newPrefix = rRenamedObject.GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType );
	}
	else
	{
		newPrefix = rRenamedObject.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType ) + _T( "." );
	}// end else
	oldPrefix = newPrefix;
	SvUl_SF::searchAndReplace( oldPrefix, rRenamedObject.GetName(), rOldName.c_str() );

	int iSize = m_svoArchiveResultNames.getResultSize();

	for (int i = 0; i < iSize; i++ )
	{
		SVString Name;
		m_svoArchiveResultNames.GetValue(Name, i);

		if ('.' == oldPrefix[oldPrefix.size()-1])
		{	//check if part of the name (ends with '.') is to replace
			SvUl_SF::searchAndReplace( Name, oldPrefix.c_str(), newPrefix.c_str() );
		}
		else
		{
			SVString indirectTmp = Name;
			size_t pos = indirectTmp.find('[');
			if (SVString::npos != pos)
			{	//if array ("[x]") in the name, remove it for the check
				indirectTmp = indirectTmp.substr(0, pos);
			}
			//only replace the name if it is the fully name. Do NOT replace parts of the name, because then it this a other object with similar name.
			if (oldPrefix == indirectTmp)
			{
				SvUl_SF::searchAndReplace( Name, oldPrefix.c_str(), newPrefix.c_str() );
			}
		}
		m_svoArchiveResultNames.SetValue( Name, i );
	}

	__super::OnObjectRenamed(rRenamedObject, rOldName);
}

bool SVArchiveTool::ValidateImageSpace( bool shouldFullCheck, SvStl::MessageContainerVector *pErrorMessages )
{
	int iSize = m_arrayImagesInfoObjectsToArchive.GetSize();

	//Only need to verify space if there are images to be archived.  If no images are checked we do not need to 
	//run through the checking of disk space.
	if (iSize > 0)
	{
		SVString ImagePath;
		GetImageArchivePath( ImagePath );

		if ( shouldFullCheck )
		{
			ArchiveToolHelper athImage;
			athImage.Init( ImagePath );
			m_ImageTranslatedPath = ImagePath;
			if (athImage.isUsingKeywords() && athImage.isTokensValid())
			{
				ImagePath = athImage.TranslatePath( ImagePath.c_str() );
				m_ImageTranslatedPath = ImagePath;
				m_ArchiveImagePathUsingKW = true;
			}
			if ( _access( ImagePath.c_str(), 0 ) != 0 )
			{
				//create the new path
				SVFileNameManagerClass::Instance().CreatePath( ImagePath.c_str() );
			}
		}
		else
		{
			if (m_ArchiveImagePathUsingKW)
			{
				ImagePath = m_ImageTranslatedPath;
			}
		}

		//
		// Check the available space for storing image archive files.
		//

		ULARGE_INTEGER lFreeBytesAvailableToCaller;
		ULARGE_INTEGER lTotalNumberOfBytes;
		ULARGE_INTEGER lTotalNumberOfFreeBytes;

		BOOL bOk = ::GetDiskFreeSpaceEx( ImagePath.c_str(),         // pointer to the directory name
			&lFreeBytesAvailableToCaller, // receives the number of bytes on
			// disk available to the caller
			&lTotalNumberOfBytes,         // receives the number of bytes on disk
			&lTotalNumberOfFreeBytes );   // receives the free bytes on disk

		if(!bOk)
		{
			if (nullptr != pErrorMessages)  
			{
				DWORD  ErrorCd =  GetLastError();
				if ( ErrorCd == ERROR_PATH_NOT_FOUND )
				{ //should not ever get here since the path is validated above
					SVStringVector msgList;
					msgList.push_back( ImagePath );
					SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_PathFileNotFound, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10037, GetUniqueObjectID() ); 
					pErrorMessages->push_back(Msg);
				}
			}
			return false;
		}

		//
		// Make sure we have at least 100 Meg bytes space on the drive.
		//
		//For systems wtih 16GB of memory the amount of memory will be 300Meg
		if (((__int64)100000000) > lFreeBytesAvailableToCaller.QuadPart)
		{
			if (nullptr != pErrorMessages)  
			{
				SVStringVector msgList;
				msgList.push_back( SVString( m_ImageTranslatedPath ) );
				SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Drive_Full, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
			return false;
		}
		
	}	
	return true;
}

bool SVArchiveTool::ValidateOnRun(SvStl::MessageContainerVector *pErrorMessages)	// called each onRun
{
	bool bOk = true;

	if ( (0 == m_uiValidateCount % 10))
	{
		bool shouldFullCheck = !SVSVIMStateClass::CheckState(SV_STATE_RUNNING);
		bOk = ValidateImageSpace(shouldFullCheck, pErrorMessages);
	}
	m_uiValidateCount++;

	return bOk;
}
