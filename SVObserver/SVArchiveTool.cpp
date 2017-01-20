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
#include "SVSVIMStateClass.h"
#include "SVToolSet.h"
#include "SVArchiveHeaderEditDlg.h"
#include "SVVisionProcessorHelper.h"
#include "SVValueObjectLibrary/BasicValueObject.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma endregion Declarations


SV_IMPLEMENT_CLASS(SVArchiveTool, SVArchiveToolClassGuid);

SVArchiveTool::SVArchiveTool( BOOL BCreateDefaultTaskList, 
                              SVObjectClass* POwner, 
                              int StringResourceID )
              : SVToolClass(BCreateDefaultTaskList, POwner, StringResourceID)
{
	initializeArchiveTool();
}

void SVArchiveTool::initializeArchiveTool()
{
	m_ArchiveImagePathUsingKW = false;
	m_bDriveError = false;
	m_ImageTranslatedPath = "";

	m_arrayResultsInfoObjectsToArchive.SetArchiveTool( this );
	m_arrayImagesInfoObjectsToArchive.SetArchiveTool( this );

	// Set up your type...
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVToolArchiveObjectType;
	
	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();
	
	// Register Embedded Objects
	RegisterEmbeddedObject(	
		&m_stringFileArchivePath, 
		SVArchiveFilePathObjectGuid, 
		IDS_OBJECTNAME_ARCHIVE_FILEPATH,
		false, SVResetItemTool );
	
	RegisterEmbeddedObject(	
		&m_stringArchiveImageGuids_OBSOLETE, 
		SVArchiveImageGuidsObjectGuid, 
		IDS_OBJECTNAME_ARCHIVE_IMAGE_GUIDS,
		false, SVResetItemTool );
	
	RegisterEmbeddedObject(	
		&m_stringArchiveResultGuids_OBSOLETE, 
		SVArchiveResultGuidsObjectGuid, 
		IDS_OBJECTNAME_ARCHIVE_RESULT_GUIDS,
		false, SVResetItemTool );
	
	RegisterEmbeddedObject(	
		&m_svoArchiveImageNames, 
		SVArchiveImageNamesObjectGuid, 
		IDS_OBJECTNAME_ARCHIVE_IMAGE_NAMES,
		false, SVResetItemTool );
	
	RegisterEmbeddedObject(	
		&m_svoArchiveResultNames, 
		SVArchiveResultNamesObjectGuid, 
		IDS_OBJECTNAME_ARCHIVE_RESULT_NAMES,
		false, SVResetItemTool );
	
	RegisterEmbeddedObject(	
		&m_stringImageFileRootPath,
		SVArchiveImageFileRootPathGuid,
		IDS_OBJECTNAME_ARCHIVE_IMAGE_ROOT_PATH,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject(	
		&m_dwAppendArchiveFile,
		SVArchiveAppendArchiveFileGuid,
		IDS_OBJECTNAME_ARCHIVE_APPEND_ARCHIVE_FILE,
		false, SVResetItemTool );
	m_dwAppendArchiveFile.SetOutputFormat(OutputFormat_int);
	
	RegisterEmbeddedObject(	
		&m_dwArchiveStopAtMaxImages,
		SVArchiveStopAtMaxImagesGuid,
		IDS_OBJECTNAME_ARCHIVE_STOP_AT_MAX_IMAGES,
		false, SVResetItemNone );
	m_dwArchiveStopAtMaxImages.SetOutputFormat(OutputFormat_int);
	
	RegisterEmbeddedObject(	
		&m_dwArchiveMaxImagesCount,
		SVArchiveMaxImagesCountGuid,
		IDS_OBJECTNAME_ARCHIVE_MAX_IMAGES_COUNT,
		false, SVResetItemNone );
	m_dwArchiveMaxImagesCount.SetOutputFormat(OutputFormat_int);

	RegisterEmbeddedObject(	
		&m_evoArchiveMethod,
		SVArchiveMethodGuid,
		IDS_OBJECTNAME_ARCHIVE_TOOL_METHOD,
		false, SVResetItemNone  );

	RegisterEmbeddedObject(
		&m_HeaderLabelNames,
		SVArchiveHeaderLabelGUID,
		IDS_OBJECTNAME_HEADER_LABELS,
		false, SVResetItemNone);

	RegisterEmbeddedObject(
		&m_HeaderObjectGUIDs,
		SVArchiveHeaderObjectGUID,
		IDS_OBJECTNAME_HEADER_OBJECT_STRINGS,
		false, SVResetItemNone);
	
	RegisterEmbeddedObject(
		&m_bvoUseHeaders,
		SVArchiveUseHeadersGUID,
		IDS_OBJECTNAME_ENABLE_HEADERS,
		false, SVResetItemNone);

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

	m_stringFileArchivePath.SetDefaultValue( FileClass.GetFullFileName(), TRUE );
	m_stringArchiveImageGuids_OBSOLETE.SetDefaultValue( _T( "" ), TRUE );
	m_stringArchiveResultGuids_OBSOLETE.SetDefaultValue( _T( "" ), TRUE );
	m_svoArchiveImageNames.SetDefaultValue( _T( "" ), TRUE );
	m_svoArchiveResultNames.SetDefaultValue( _T( "" ), TRUE );
	m_stringImageFileRootPath.SetDefaultValue( _T( "D:\\TEMP" ), TRUE );
	m_dwAppendArchiveFile.SetDefaultValue(0, TRUE);
	m_dwArchiveStopAtMaxImages.SetDefaultValue(1, TRUE);
	m_dwArchiveMaxImagesCount.SetDefaultValue(10, TRUE);

	m_evoArchiveMethod.SetDefaultValue( SVArchiveAsynchronous, TRUE );
	SVEnumerateVector vec;
	vec.push_back( SVEnumeratePair(_T("Synchronous"), SVArchiveSynchronous ) );
	vec.push_back( SVEnumeratePair(_T("Change Mode"), SVArchiveGoOffline ) );
	vec.push_back( SVEnumeratePair(_T("Asynchronous"), SVArchiveAsynchronous ) );
	m_evoArchiveMethod.SetEnumTypes(vec);
	
	m_stringArchiveImageGuids_OBSOLETE.ObjectAttributesAllowedRef() = SV_NO_ATTRIBUTES;
	m_stringArchiveResultGuids_OBSOLETE.ObjectAttributesAllowedRef() = SV_NO_ATTRIBUTES;
	m_svoArchiveImageNames.ObjectAttributesAllowedRef() =  SV_REMOTELY_SETABLE;
	m_svoArchiveResultNames.ObjectAttributesAllowedRef() = SV_REMOTELY_SETABLE;
	m_HeaderObjectGUIDs.ObjectAttributesAllowedRef() = SV_NO_ATTRIBUTES;
	m_bInitializedForRun = FALSE;
	m_eArchiveMethod = SVArchiveInvalidMethod;
	m_uiValidateCount = 0;
}

BOOL SVArchiveTool::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if ( SVToolClass::CreateObject( PCreateStructure ) )
	{
		DWORD dwMethod = SVArchiveInvalidMethod;
		m_evoArchiveMethod.GetValue( dwMethod );
		m_eArchiveMethod = static_cast<SVArchiveMethodEnum>( dwMethod );

		bOk = TRUE;
	}

	m_stringFileArchivePath.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	m_stringArchiveImageGuids_OBSOLETE.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_stringArchiveResultGuids_OBSOLETE.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_svoArchiveImageNames.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	m_svoArchiveResultNames.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	m_stringImageFileRootPath.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	m_dwAppendArchiveFile.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_dwArchiveStopAtMaxImages.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	m_dwArchiveMaxImagesCount.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	m_evoArchiveMethod.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	m_HeaderLabelNames.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	m_HeaderObjectGUIDs.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_bvoUseHeaders.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	// Override base class exposure of the drawflag
	// This value will not be exposed for the Archive Tool.
	drawToolFlag.ObjectAttributesAllowedRef() = SV_HIDDEN;

	// Override base class exposure of the auxillaryextent variables
	// These values will not be exposed for the Archive Tool.
	m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceX.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceY.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceAngle.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceImageName.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliaryDrawType.ObjectAttributesAllowedRef() = SV_HIDDEN;

	m_isCreated = bOk;

	return bOk;
}

HRESULT SVArchiveTool::ResetObject()
{
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
	if ( pInspection && pInspection->IsResetStateSet( SVResetStateArchiveToolCreateFiles ) )
	{
		HRESULT hrInitialize = initializeOnRun();
		m_bInitializedForRun = S_OK == hrInitialize;
	}
	else
	{
		m_arrayImagesInfoObjectsToArchive.ResetImageCounts();
		m_arrayImagesInfoObjectsToArchive.ValidateImageObjects();

		HRESULT hrAllocate = AllocateImageBuffers();
	}

	return SVToolClass::ResetObject();
}

SVArchiveTool::~SVArchiveTool()
{
	TheSVArchiveImageThreadClass().GoOffline();
}

// Should be overridden and must be called in derived classes...
// Sets Depth of Array for historical purposes 
// (i.e. to get images/results N events ago)
BOOL SVArchiveTool::SetObjectDepth( int NewObjectDepth )
{
	// Set object depth of members here...
	DWORD dwMaxImages;
	m_dwArchiveMaxImagesCount.GetValue( dwMaxImages );

	return SVToolClass::SetObjectDepth( NewObjectDepth );
}

// Should be overridden and must be called in derived classes...
// Sets Depth of Array for historical purposes 
// (i.e. to get images/results N events ago)
BOOL SVArchiveTool::SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex )
{
	// Set object depth of members here...
	DWORD dwMaxImages;
	m_dwArchiveMaxImagesCount.GetValue( dwMaxImages );

	return SVToolClass::SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
}


// Create a file to store selected results in text format.
//
BOOL SVArchiveTool::CreateTextArchiveFile()
{
	bool bValidFile = true;
	//
	// CFile object.
	//
	SVString FileArchivePath;
	GetFileArchive( FileArchivePath );

	if( FileArchivePath.empty() )
	{
		return FALSE;
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
		SVStringVector msgList;
		msgList.push_back( FileArchivePath );
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ArchiveTool_CreateFileFailed, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10036 ); 
		return FALSE;
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

		bool bUseHeaders = false;
		HRESULT hr = m_bvoUseHeaders.GetValue( bUseHeaders );
		if( S_OK == hr && bUseHeaders )
		{
			// Write Header
			SVStringVector astrHeaders;
			m_HeaderLabelNames.GetValues( astrHeaders );
			SVString strHeader;
			for( SVStringVector::iterator it = astrHeaders.begin() ; it != astrHeaders.end() ; ++it)
			{
				strHeader += *it;
				if( it+1 != astrHeaders.end())
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
	catch (CException*)
	{
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
		
	return TRUE;
}

BOOL SVArchiveTool::Validate()	// called once when going online
{
	BOOL bOk = FALSE;

	SVString ImagePath;

	GetImageArchivePath( ImagePath );

	ArchiveToolHelper athImage;
	athImage.Init( ImagePath );
	if (athImage.isUsingKeywords() && athImage.isTokensValid())
	{
		ImagePath = athImage.TranslatePath( ImagePath );
		m_ImageTranslatedPath = ImagePath.c_str();
		m_ArchiveImagePathUsingKW = true;
	}

	bOk = ! ImagePath.empty();
	if ( bOk )
	{
		bOk = _access( ImagePath.c_str(), 0 ) == 0;

		if ( !bOk )
		{
			bOk = SVFileNameManagerClass::Instance().CreatePath( ImagePath.c_str() );
		}
	}

	if (m_bDriveError)
	{	
		SVStringVector msgList;
		msgList.push_back( ImagePath );
		SvStl::MessageContainer Msg( 0L, SvOi::Tid_Drive_Full, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		addTaskMessage( Msg );
	}
	m_uiValidateCount = 0;

	bOk = SVToolClass::Validate();

	return bOk;
}

BOOL SVArchiveTool::OnValidate()	// called each onRun
{
	BOOL bOk = SVToolClass::OnValidate();

	if ( bOk )
	{
		int iSize = m_arrayImagesInfoObjectsToArchive.GetSize();

		//Only need to verify space if there are images to be archived.  If no images are checked we do not need to 
		//run through the checking of disk space.
		if (iSize > 0)
		{
			if (m_uiValidateCount % 10 == 0)
			{
				SVString ImagePath;
				GetImageArchivePath( ImagePath );

				if ( !SVSVIMStateClass::CheckState(SV_STATE_RUNNING) )
				{
					ArchiveToolHelper athImage;
					athImage.Init( ImagePath );

					if (athImage.isUsingKeywords() && athImage.isTokensValid())
					{
						ImagePath = athImage.TranslatePath(SVString(ImagePath)).c_str();
						m_ImageTranslatedPath = ImagePath.c_str();
						m_ArchiveImagePathUsingKW = true;

						//since it is using keywords verify that the path exists
						if ( _access( ImagePath.c_str(), 0 ) != 0 )
						{
							//create the new path
							SVFileNameManagerClass::Instance().CreatePath( ImagePath.c_str() );
						}
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
				
				bOk = ::GetDiskFreeSpaceEx( ImagePath.c_str(),         // pointer to the directory name
											&lFreeBytesAvailableToCaller, // receives the number of bytes on
																			// disk available to the caller
											&lTotalNumberOfBytes,         // receives the number of bytes on disk
											&lTotalNumberOfFreeBytes );   // receives the free bytes on disk
				
				if(!bOk)  
				{
					DWORD  ErrorCd =  GetLastError();
					if ( ErrorCd == ERROR_PATH_NOT_FOUND )
					{ //should not ever get here since the path is validated above
						SVStringVector msgList;
						msgList.push_back( ImagePath );
						SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_PathFileNotFound, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10037 ); 

						bOk = FALSE;
					}
				}
				
				//
				// Make sure we have at least 100 Meg bytes space on the drive.
				//
				//For systems wtih 16GB of memory the amount of memory will be 300Meg
				if ( bOk )
				{
					bOk = ((__int64)100000000) < lFreeBytesAvailableToCaller.QuadPart;
					if (!bOk)
					{
						m_bDriveError = true;
					}
					else
					{
						m_bDriveError = false;
					}
				}

			}
		}
		else
		{
			//don't worry about drive space because no images are selected for archiving.
			m_bDriveError = false;
		}
		m_uiValidateCount++;

	}

	if ( ! bOk )
	{
		SetInvalid();
	}

	return bOk;
}



void local_remove_items( SVStringVector& rVec, SVStringValueObjectClass& rvo )
{
	SVStringVector vecNames;
	SVStringVector::iterator iterRemoved;

	if ( !rVec.empty() )
	{
		rvo.GetValues( vecNames );

		for ( iterRemoved = rVec.begin(); iterRemoved != rVec.end(); ++iterRemoved )
		{
			SVStringVector::iterator iterName;
			iterName = std::find(vecNames.begin(), vecNames.end(), *iterRemoved );
			if ( iterName != vecNames.end() )
			{
				vecNames.erase( iterName );
			}
		}

		rvo.SetArraySize( static_cast< int >( vecNames.size() ) );
		rvo.SetArrayValues(1, vecNames );
	}
}


HRESULT SVArchiveTool::initializeOnRun()
{
	DWORD dwMethod=0;
	m_evoArchiveMethod.GetValue( dwMethod );
	m_eArchiveMethod = static_cast<SVArchiveMethodEnum>( dwMethod );

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
				return S_FALSE;
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
		HRESULT hrAllocate = AllocateImageBuffers();
		if ( S_OK != hrAllocate )
			return hrAllocate;

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
		return S_OK;
	}
	
	//
	// Create a file to store results in text format.
	//
	if ( m_fileArchive.m_hFile != CFile::hFileNull )
	{
		return S_OK;
	}

	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());
	if ( pInspection && pInspection->IsResetStateSet( SVResetStateArchiveToolCreateFiles ) )	// If going online
	{
		//
		// Create and open the results to text Archive file.
		//
		BOOL bResult = CreateTextArchiveFile();
		if (!bResult)
		{
			 return S_FALSE;
		}
	}
	
	return S_OK;
}

HRESULT SVArchiveTool::AllocateImageBuffers()
{
	if ( m_eArchiveMethod == SVArchiveGoOffline || m_eArchiveMethod == SVArchiveAsynchronous )
	{
		// async doesn't really allocate any buffers but it does preparation work (initm_ImageInfo)
		DWORD dwMaxImages;
		m_dwArchiveMaxImagesCount.GetValue( dwMaxImages );
		HRESULT hrAllocate = m_arrayImagesInfoObjectsToArchive.AllocateBuffers( dwMaxImages );
		ASSERT( S_OK == hrAllocate );
		return hrAllocate;
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//
BOOL SVArchiveTool::onRun( SVRunStatusClass& RRunStatus )
{
	if( SVToolClass::onRun( RRunStatus ) )
	{
		//
		// If this is not a test mode or run mode (online) no work is required.
		// Usually a condition where user is 'moving' a tool that causes the
		// toolset to 'run'.
		//
		if ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION) )
		{
			return TRUE;
		}
		
		if ( ! RRunStatus.IsDisabled() && ! RRunStatus.IsDisabledByCondition() )
		{
			if ( !m_bInitializedForRun )
			{
				RRunStatus.SetFailed();
				return FALSE;
			}
			
			//
			// Iterate the array of objects to archive and build a string.
			//
			SVString Archive = m_arrayResultsInfoObjectsToArchive.BuildResultsArchiveString();

			if ( !Archive.empty() )
			{
				Archive += _T("\r\n");
				

				if ( true )	// always do this for now (never buffer text) see FR00100.007.0460.xxx
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
							m_fileArchive.Write( Archive.c_str(), static_cast<int> (Archive.size()) );
						} 
						catch (CException* e)
						{
							RRunStatus.SetFailed();
							e->Delete();
							return FALSE;
						} 
						catch (...)
						{
							RRunStatus.SetFailed();
							return FALSE;
						}
					}
				}// end if ( !bBufferText || m_eArchiveMethod != SVArchiveGoOffline )
				else if ( m_eArchiveMethod == SVArchiveGoOffline )
				{
					QueueArchiveString( Archive );
				}
			}

#if defined (TRACE_THEM_ALL) || defined (TRACE_ARCHIVE)
			TRACE( _T( "SVArchiveTool::onRun-WriteArchiveImageFiles-Name=%s\n" ), GetCompleteName() );
#endif
			//
			// Iterate the list of images to archive.
			//
			m_arrayImagesInfoObjectsToArchive.WriteArchiveImageFiles( );

			RRunStatus.SetPassed();
		}

		return TRUE;
	}
	
	RRunStatus.SetInvalid();
	return FALSE;
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
		SVObjectReference ref = vecObjects.at(i);
		
		//
		// Clear the existing archivable bits that might be set.
		//
		ref.ObjectAttributesSetRef() &= ~SV_ARCHIVABLE;
		
	}
	//
	// Iterate the current list of results to archive.
	//
	int nCountResults = m_arrayResultsInfoObjectsToArchive.GetSize();
	for (int i = 0; i < nCountResults; i++)
	{
		// SVObjectInfoStruct InfoItemArchive = 
		//	arrayResultsInfoObjectsToArchive.GetAt(k);
		SVObjectReference ref = m_arrayResultsInfoObjectsToArchive.GetAt( i )->GetObjectReference();

		if (ref.ObjectAttributesAllowed() & SV_ARCHIVABLE)
		{
			ref.ObjectAttributesSetRef() |= SV_ARCHIVABLE;
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

	l_ToolSetOutputList.GetSetAttributesList( SV_ARCHIVABLE, vecObjects );
	
	m_svoArchiveResultNames.SetArraySize( static_cast< int >( vecObjects.size() ) );

	for ( int i = 0; i < static_cast< int >( vecObjects.size() ); i++ )
	{
		SVObjectReference ref = vecObjects[i];

		SVArchiveRecord* pArchiveRecord = new SVArchiveRecord;
		pArchiveRecord->Init( this );
		pArchiveRecord->GetObjectReference() = ref;
		
		m_arrayResultsInfoObjectsToArchive.Add( pArchiveRecord );
		pArchiveRecord->ConnectInputObject();

		m_svoArchiveResultNames.SetValue(1 , i, ref.GetCompleteName() );
	}

	m_stringArchiveResultGuids_OBSOLETE.SetValue( 1, SVString(_T("")) );
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

		if( ( nullptr != pImage ) && ( pImage->ObjectAttributesSet() & SV_ARCHIVABLE_IMAGE ) )
		{
			AddImageToArray( pImage );
			pImage->ObjectAttributesSetRef() &= ~SV_ARCHIVABLE_IMAGE;	// WHY??

			vecImages.push_back( pImage );
		}
	}
	
	
	m_svoArchiveImageNames.SetArraySize( static_cast< int >( vecImages.size() ) );

	for ( int i = 0; i < static_cast< int >( vecImages.size() ); i++ )
	{
		SVObjectReference ref = vecImages[i];

		m_svoArchiveImageNames.SetValue( 1, i, ref.GetCompleteName() );
	}

	m_stringArchiveImageGuids_OBSOLETE.SetValue(1, SVString(_T("")) );
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
				SVImageClass* pImage2 = dynamic_cast <SVImageClass*> ( pRecordImage->GetObjectReference().Object() );
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
						if (pImage->ObjectAttributesAllowed() & SV_ARCHIVABLE_IMAGE)
						{
							pImage->ObjectAttributesSetRef() |= SV_ARCHIVABLE_IMAGE;
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

// Check for duplicate archive result file path in some other tool or 
// toolset in the system.
//
// Return TRUE if archive file path is unique.
// Return FALSE if another file path is found that is the same.
//
BOOL SVArchiveTool::CheckForUniqueArchiveFilePath( LPCTSTR pszArchiveFilePathToTest )
{
	return TRUE;
}

BOOL SVArchiveTool::GetFileArchive( SVString& rName )
{
	bool Result = (S_OK == m_stringFileArchivePath.GetValue( rName ));
	return Result;
}

BOOL SVArchiveTool::GetImageArchivePath( SVString& rName )
{
	return S_OK == m_stringImageFileRootPath.GetValue( rName );
}

BOOL SVArchiveTool::SetFileArchive( LPCTSTR lpszName )
{
	return S_OK == m_stringFileArchivePath.SetValue( 1, lpszName );
}

BOOL SVArchiveTool::SetImageArchivePath( LPCTSTR lpszName )
{
	return S_OK == m_stringImageFileRootPath.SetValue( 1, lpszName );
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

HRESULT SVArchiveTool::ValidateArchiveTool()
{
	HRESULT hRet = S_OK;

	//reset m_uiValidateCount back to 0 so it will check the drive space
	m_uiValidateCount = 0;

	if (!OnValidate())
	{
		hRet = S_FALSE;
	}
	
	return hRet;
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
	}// end if
	else if( const BasicValueObject* pBasicValueObject = dynamic_cast<const BasicValueObject*> (&rRenamedObject) )
	{
		newPrefix = pBasicValueObject->GetCompleteObjectNameToObjectType( nullptr, SVRootObjectType );
	}
	else if( const SVValueObjectClass* pValueObject = dynamic_cast<const SVValueObjectClass*> (&rRenamedObject) )
	{
		newPrefix = pValueObject->GetCompleteObjectNameToObjectType( nullptr, SVInspectionObjectType );
	}
	else
	{
		newPrefix = rRenamedObject.GetCompleteObjectNameToObjectType( nullptr, SVToolSetObjectType ) + _T( "." );
	}// end else
	oldPrefix = newPrefix;
	SvUl_SF::searchAndReplace( oldPrefix, rRenamedObject.GetName(), rOldName.c_str() );

	int iSize = m_svoArchiveResultNames.GetResultSize();
	int iLastSet = m_svoArchiveResultNames.GetLastSetIndex();

	for (int i = 0; i < iSize; i++ )
	{
		SVString sName;
		m_svoArchiveResultNames.GetValue(iLastSet,i,sName);

		if ('.' == oldPrefix[oldPrefix.size()-1])
		{	//check if part of the name (ends with '.') is to replace
			SvUl_SF::searchAndReplace( sName, oldPrefix.c_str(), newPrefix.c_str() );
		}
		else
		{
			SVString indirectTmp = sName;
			size_t pos = indirectTmp.find('[');
			if (SVString::npos != pos)
			{	//if array ("[x]") in the name, remove it for the check
				indirectTmp = indirectTmp.substr(0, pos);
			}
			//only replace the name if it is the fully name. Do NOT replace parts of the name, because then it this a other object with similar name.
			if (oldPrefix == indirectTmp)
			{
				SvUl_SF::searchAndReplace( sName, oldPrefix.c_str(), newPrefix.c_str() );
			}
		}
		m_svoArchiveResultNames.SetValue(iLastSet,i, sName);
		m_svoArchiveResultNames.GetValue(iLastSet,i, sName);
	}

	__super::OnObjectRenamed(rRenamedObject, rOldName);
}
