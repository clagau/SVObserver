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
#include "SVFileNameManagerClass.h"
#include "SVGlobal.h"
#include "SVImageClass.h"
#include "SVImageListClass.h"
#include "SVInspectionProcess.h"
#include "SVMemoryManager.h"
#include "SVSVIMStateClass.h"
#include "SVToolSet.h"
#include "SVArchiveHeaderEditDlg.h"
#include "SVVisionProcessorHelper.h"
#include "BasicValueObject.h"
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
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVToolArchiveObjectType;
	
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
	
	RegisterEmbeddedObject(	
		&m_dwArchiveStopAtMaxImages,
		SVArchiveStopAtMaxImagesGuid,
		IDS_OBJECTNAME_ARCHIVE_STOP_AT_MAX_IMAGES,
		false, SVResetItemNone );
	
	RegisterEmbeddedObject(	
		&m_dwArchiveMaxImagesCount,
		SVArchiveMaxImagesCountGuid,
		IDS_OBJECTNAME_ARCHIVE_MAX_IMAGES_COUNT,
		false, SVResetItemNone );

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
	SVFileNameClass svFileName;
	svFileName.SetExtension( ".txt" );
	svFileName.SetPathName( "D:\\TEMP" );
		
	CString csFileName;
	csFileName.Format("%s__0",GetName() );
	svFileName.SetFileNameOnly( csFileName );

	m_stringFileArchivePath.SetDefaultValue( svFileName.GetFullFileName(), TRUE );
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

	isCreated = bOk;

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

	CString csTemp;
	m_stringArchiveImageGuids_OBSOLETE.GetValue(csTemp);
	if ( csTemp.IsEmpty() )
	{
		m_arrayImagesInfoObjectsToArchive.InitializeObjects( this, m_svoArchiveImageNames );
		m_arrayImagesInfoObjectsToArchive.ValidateImageObjects();	// makes sure the images are connected as inputs
	}
	else	// pre array value object way
	{
		m_arrayImagesInfoObjectsToArchive.ConvertStringToGuids( this, (LPCTSTR) csTemp );
	}
	
	m_stringArchiveResultGuids_OBSOLETE.GetValue(csTemp);
	if ( csTemp.IsEmpty() )
	{
		m_arrayResultsInfoObjectsToArchive.InitializeObjects( this, m_svoArchiveResultNames );
		m_arrayResultsInfoObjectsToArchive.ValidateResultsObjects();	// makes sure the results are connected as inputs
	}
	else	// pre array value object way
	{
		m_arrayResultsInfoObjectsToArchive.ConvertStringToGuids( this, (LPCTSTR) csTemp );
	}

	if ( GetInspection() != NULL && GetInspection()->IsResetStateSet( SVResetStateArchiveToolCreateFiles ) )
	{
		HRESULT hrInitialize = initializeOnRun();
		m_bInitializedForRun = hrInitialize == S_OK;
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
// (i.e. to get images/results N events ago) SEJ
//
BOOL SVArchiveTool::SetObjectDepth( int NewObjectDepth )
{
	// Set object depth of members here...
	DWORD dwMaxImages;
	m_dwArchiveMaxImagesCount.GetValue( dwMaxImages );

	return SVToolClass::SetObjectDepth( NewObjectDepth );
}

// Should be overridden and must be called in derived classes...
// Sets Depth of Array for historical purposes 
// (i.e. to get images/results N events ago) SEJ
//
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
	CString csFileArchivePath;
	GetFileArchive(csFileArchivePath);

	if (csFileArchivePath.GetLength() == 0)
	{
		return FALSE;
	}

	ArchiveToolHelper athFile;
	athFile.Init(SVString(csFileArchivePath));

	if (athFile.isUsingKeywords())
	{
		if(athFile.isTokensValid())
		{
			csFileArchivePath = athFile.TranslatePath(SVString(csFileArchivePath)).c_str();
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
	
	if ( ! csFileArchivePath.IsEmpty() )
	{
		SVFileNameClass svFileName( csFileArchivePath );

		if ( _access( svFileName.GetPathName(), 0 ) != 0 )
		{
			SVFileNameManagerClass svFileManager;

			if(!svFileManager.CreatePath(svFileName.GetPathName() ) )return FALSE;
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
	
	BOOL bResult = m_fileArchive.Open( (LPCTSTR)csFileArchivePath,
	                                   uOpenFlags,
	                                   NULL );

	if(!bResult)
	{
		SVString s = SvUl_SF::Format(SvO::Error_ArchiveTool_CreateFileFailed,	(LPCTSTR)csFileArchivePath);
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, s.c_str(), StdMessageParams, SvOi::Err_10036 ); 
		return FALSE;
	}
	
	//
	// Write a date and time stamp to the archive file.
	//
	CString csTimeStamp;
	CTime  timeCurrent;
	timeCurrent = CTime::GetCurrentTime();
	//
	// Format date and time as DD/MM/YYYY HH:MM:SS
	//
	csTimeStamp = timeCurrent.Format(_T("%d/%m/%Y %H:%M:%S\r\n"));
	
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
		
		m_fileArchive.Write( (LPCTSTR)csTimeStamp, csTimeStamp.GetLength() );

		bool bUseHeaders = false;
		HRESULT hr = m_bvoUseHeaders.GetValue( bUseHeaders );
		if( hr == S_OK && bUseHeaders )
		{
			// Write Header
			std::vector<CString> astrHeaders;
			m_HeaderLabelNames.GetValues( astrHeaders );
			CString strHeader;
			for( std::vector<CString>::iterator it = astrHeaders.begin() ; it != astrHeaders.end() ; ++it)
			{
				strHeader += *it;
				if( it+1 != astrHeaders.end())
				{
					strHeader+= _T(",");
				}
			}
			if( !strHeader.IsEmpty() )
			{
				strHeader += _T("\r\n");
				m_fileArchive.Write( (LPCTSTR) strHeader, strHeader.GetLength() );
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

	CString csImagePath;

	GetImageArchivePath( csImagePath );

	ArchiveToolHelper athImage;
	athImage.Init(SVString(csImagePath));
	if (athImage.isUsingKeywords() && athImage.isTokensValid())
	{
		csImagePath = athImage.TranslatePath(SVString(csImagePath)).c_str();
		m_ImageTranslatedPath = csImagePath;
		m_ArchiveImagePathUsingKW = true;
	}

	bOk = ! csImagePath.IsEmpty();
	if ( bOk )
	{
		bOk = _access( csImagePath, 0 ) == 0;

		if ( !bOk )
		{
			SVFileNameManagerClass svFileManager;

			bOk = svFileManager.CreatePath( csImagePath );
		}
	}

	if (m_bDriveError)
	{	
		//Display message that the Archive Tool path is full
		SVString sMessage;
		sMessage = SvUl_SF::Format(" %s - drive is full.", csImagePath.GetString());
		//GetUniqueObjectID()
		SVVisionProcessorHelper::Instance().AddToolError(GetUniqueObjectID(),sMessage);
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
				CString csImagePath;
				GetImageArchivePath( csImagePath );

				if ( !SVSVIMStateClass::CheckState(SV_STATE_RUNNING) )
				{
					ArchiveToolHelper athImage;
					athImage.Init(SVString(csImagePath));

					if (athImage.isUsingKeywords() && athImage.isTokensValid())
					{
						csImagePath = athImage.TranslatePath(SVString(csImagePath)).c_str();
						m_ImageTranslatedPath = csImagePath;
						m_ArchiveImagePathUsingKW = true;

						//since it is using keywords verify that the path exists
						if ( _access( csImagePath, 0 ) != 0 )
						{
							//create the new path
							SVFileNameManagerClass svFileManager;
							svFileManager.CreatePath( csImagePath );
						}
					}
				}
				else
				{
					if (m_ArchiveImagePathUsingKW)
					{
						csImagePath = m_ImageTranslatedPath;
					}
				}

				//
				// Check the available space for storing image archive files.
				//
				
				ULARGE_INTEGER lFreeBytesAvailableToCaller;
				ULARGE_INTEGER lTotalNumberOfBytes;
				ULARGE_INTEGER lTotalNumberOfFreeBytes;
				
				bOk = ::GetDiskFreeSpaceEx( (LPCTSTR)csImagePath,         // pointer to the directory name
											&lFreeBytesAvailableToCaller, // receives the number of bytes on
																			// disk available to the caller
											&lTotalNumberOfBytes,         // receives the number of bytes on disk
											&lTotalNumberOfFreeBytes );   // receives the free bytes on disk
				
				if(!bOk)  
				{
					msvError.msvlErrorCd = (DWORD)(-(long)(GetLastError()));

					if ( msvError.msvlErrorCd == -3 )
					{ //should not ever get here since the path is validated above
						SVString temp = SvUl_SF::Format (SvO::Error_PathFileNotFound, csImagePath);
						SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
						Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, temp.c_str(), StdMessageParams, SvOi::Err_10037 ); 

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



void local_remove_items( std::vector<CString>& rVec, SVStringValueObjectClass& rvo )//Arvid 2015-01-13 made this a free function to avoid Cppcheck warning
{
	std::vector<CString> vecNames;
	std::vector<CString>::iterator iterRemoved;

	if ( !rVec.empty() )
	{
		rvo.GetValues( vecNames );

		for ( iterRemoved = rVec.begin(); iterRemoved != rVec.end(); ++iterRemoved )
		{
			std::vector<CString>::iterator iterName;
			iterName = std::find(vecNames.begin(), vecNames.end(), *iterRemoved );
			if ( iterName != vecNames.end() )
				vecNames.erase( iterName );
		}

		rvo.SetArraySize( static_cast< int >( vecNames.size() ) );
		rvo.SetArrayValues(1, vecNames );
	}
}


/////////////////////////////////////////////////////////////////////////////
//
DWORD_PTR SVArchiveTool::processMessage( DWORD dwMessageID, 
                                     DWORD_PTR dwMessageValue, 
                                     DWORD_PTR dwMessageContext )
{
	//BOOL bResult;
	DWORD_PTR dwResult = NULL;
	// Try to process message by yourself...
	DWORD dwPureMessageID = dwMessageID & SVM_PURE_MESSAGE;
	switch (dwPureMessageID)
	{
		case SVMSGID_GOING_ONLINE:
		{
			return SVMR_SUCCESS;
		}
		break;
				
		// is sent from SVObserver.cpp.
		case SVMSGID_GOING_OFFLINE:
		{
			if ( m_eArchiveMethod == SVArchiveAsynchronous )
			{
				TheSVArchiveImageThreadClass().GoOffline();
			}

			if ( m_eArchiveMethod == SVArchiveGoOffline )
			{
				WriteBuffers();
			}

			//
			// Close the text to archive file if necessary, i.e. is open.
			//
			if(m_fileArchive.m_hFile != CFile::hFileNull)
			{
				m_fileArchive.Close();
				return SVMR_SUCCESS;
			}
		}
		break;
				
		// This Message occurs for two scenarios
		// 1. Some Object is using our outputs and they are no longer needed.
		// 2. We are using some Object's outputs and the outputs are no longer available
		case SVMSGID_DISCONNECT_OBJECT_INPUT:
		{
			// ...use second message parameter ( DwMessageValue ) 
			//   as pointer to InObjectInfo ( SVInObjectInfoStruct* )
			// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
			SVInObjectInfoStruct* pInObjectInfo = reinterpret_cast <SVInObjectInfoStruct*> (dwMessageValue);

			std::vector<CString> vecRemovedImage  = m_arrayImagesInfoObjectsToArchive. RemoveDisconnectedObject( pInObjectInfo->GetInputObjectInfo() );
			std::vector<CString> vecRemovedResult = m_arrayResultsInfoObjectsToArchive.RemoveDisconnectedObject( pInObjectInfo->GetInputObjectInfo() );

			local_remove_items ( vecRemovedImage, m_svoArchiveImageNames );
			local_remove_items ( vecRemovedResult, m_svoArchiveResultNames );

			return SVMR_SUCCESS;
		}
		break;

		case SVMSGID_OBJECT_RENAMED:
		{
			SVObjectClass* pObject = reinterpret_cast <SVObjectClass*> (dwMessageValue); // Object with new name
			LPCTSTR orgName = ( LPCTSTR )dwMessageContext;

			if( renameToolSetSymbol(pObject, orgName ) )
			{
				dwResult = SVMR_SUCCESS;
			}
		}
		break;
		
		default:
		{
			break;
		}
	}	
	return( SVToolClass::processMessage( dwMessageID, dwMessageValue, dwMessageContext ) | dwResult );
}

HRESULT SVArchiveTool::initializeOnRun()
{
	CString csTemp;

	DWORD dwMethod=0;
	m_evoArchiveMethod.GetValue( dwMethod );
	m_eArchiveMethod = static_cast<SVArchiveMethodEnum>( dwMethod );

	GetImageArchivePath( csTemp );
	ArchiveToolHelper athImage;
	athImage.Init(SVString(csTemp));
	
	if (athImage.isUsingKeywords() && athImage.isTokensValid())
	{
		csTemp = athImage.TranslatePath(SVString(csTemp)).c_str();
	}
	

	if ( ! csTemp.IsEmpty() )
	{
		if ( _access( csTemp, 0 ) != 0 )
		{
			SVFileNameManagerClass svFileManager;

			if(!svFileManager.CreatePath( csTemp ))
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
		if ( hrAllocate != S_OK )
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
	
	std::vector<CString>().swap(m_aArchiveStringBuffer);	// the correct way to clear a vector according to More Exceptional C++ p 52.

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

	if ( GetInspection()->IsResetStateSet( SVResetStateArchiveToolCreateFiles ) )	// If going online
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
		ASSERT( hrAllocate == S_OK );
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
			CString csArchive; 
			csArchive = m_arrayResultsInfoObjectsToArchive.BuildResultsArchiveString();

			if ( csArchive.GetLength() )
			{
				csArchive += _T("\r\n");
				

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
							m_fileArchive.Write( (LPCTSTR)csArchive, csArchive.GetLength() );
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
					QueueArchiveString( csArchive );
				}
			}

			TRACE( _T( "SVArchiveTool::onRun-WriteArchiveImageFiles-Name=%s\n" ), GetCompleteObjectName() );

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

HRESULT SVArchiveTool::QueueArchiveString( CString strArchiveString )
{
	HRESULT hr = S_OK;
	m_aArchiveStringBuffer.push_back( strArchiveString );
	return hr;
}

// Called by SVToolAdjustmentArchivePage::OnInitDialog().
//
void SVArchiveTool::UpdateTaskObjectOutputList()
{
	SVToolSetClass* pToolSet = GetInspection()->GetToolSet();
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
	SVToolSetClass* pToolSet = GetInspection()->GetToolSet();

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

		m_svoArchiveResultNames.SetValue(1 , i, ref.GetCompleteObjectName() );
	}

	m_stringArchiveResultGuids_OBSOLETE.SetValue(1, CString(_T("")));
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
	SVErrorClass msvError;
	msvError.ClearLastErrorCd ();
	
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

		if( ( pImage != NULL ) && ( pImage->ObjectAttributesSet() & SV_ARCHIVABLE_IMAGE ) )
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

		m_svoArchiveImageNames.SetValue( 1, i, ref.GetCompleteObjectName() );
	}

	m_stringArchiveImageGuids_OBSOLETE.SetValue(1, CString(_T("")) );
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
			CString csImage;
			csImage = pImage->GetCompleteObjectName();
			
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
					CString csImage2;
					csImage2 = pImage2->GetCompleteObjectName();
					
					int nResult = csImage2.CompareNoCase(csImage);  // 22 Nov 1999 - frb.
					if (nResult == 0)          // Equal?
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

/////////////////////////////////////////////////////////////////////////////
//
//
void SVArchiveTool::UpdateImagePointerInImageArray(	SVArchiveRecord* pImageRecord )
{
	CString csImageDottedName = pImageRecord->GetImageObjectName();
	pImageRecord->GetObjectReference() = NULL;           
	
	//
	// Get a pointer to the toolset
	//
	SVToolSetClass* pToolSet = GetInspection()->GetToolSet();
	
	SVErrorClass msvError;
	msvError.ClearLastErrorCd ();
	
	SVObjectTypeInfoStruct info;

	info.ObjectType = SVImageObjectType;
	info.SubType = SVNotSetSubObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetInspection()->GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		if (pImage)
		{
			CString csDottedNameToCheck = pImage->GetCompleteObjectName();
			if (csImageDottedName == csDottedNameToCheck)
			{
				pImageRecord->GetObjectReference() = pImage->GetObjectInfo().GetObjectReference();
				return;
			}
		}
	}
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


BOOL SVArchiveTool::GetFileArchive( CString& rcsName )
{
	return m_stringFileArchivePath.GetValue( rcsName ) == S_OK;
}


BOOL SVArchiveTool::GetImageArchivePath( CString& rcsName )
{
	return m_stringImageFileRootPath.GetValue( rcsName ) == S_OK;
}


BOOL SVArchiveTool::SetFileArchive( LPCTSTR lpszName )
{
	return m_stringFileArchivePath.SetValue( 1, lpszName ) == S_OK;
}


BOOL SVArchiveTool::SetImageArchivePath( LPCTSTR lpszName )
{
	return m_stringImageFileRootPath.SetValue( 1, lpszName ) == S_OK;
}


HRESULT SVArchiveTool::WriteBuffers()
{
	HRESULT hr = S_OK;

	// write text
	if (m_fileArchive.m_hFile != CFile::hFileNull)
	{
		try
		{
			std::vector<CString>::iterator iter;
			for ( iter = m_aArchiveStringBuffer.begin(); iter != m_aArchiveStringBuffer.end(); ++iter )
			{
				m_fileArchive.Write( (LPCTSTR)*iter, iter->GetLength() );
			}
		} 
		catch (CException* e)
		{
			e->Delete();
			hr = SV_FALSE;
		} 
		catch (...)
		{
			hr = SV_FALSE;
		}
	}

	// write images

	HRESULT hrImages = m_arrayImagesInfoObjectsToArchive.WriteImageQueue();
	if ( hr == S_OK )
		hr = hrImages;

	return hr;
}

long SVArchiveTool::TotalImageMemoryUsage()
{
	long lTotalMemory = 0;

	int iSize = m_arrayImagesInfoObjectsToArchive.GetSize();
	for ( int i = 0; i < iSize; i++ )
	{
		SVArchiveRecord* pRecord = m_arrayImagesInfoObjectsToArchive.GetAt(i);
		lTotalMemory += pRecord->GetImageMemorySize();
	}

	return lTotalMemory;
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


BOOL SVArchiveTool::renameToolSetSymbol( const SVObjectClass* pObject, LPCTSTR originalName )
{
	bool Result( false );
	
	if( nullptr != pObject )
	{
		SVString newPrefix;
		SVString oldPrefix;

		if( const SVInspectionProcess* l_pInspection = dynamic_cast<const SVInspectionProcess*> (pObject) )
		{
			newPrefix = l_pInspection->GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType ) + _T( "." );
		}// end if
		else if( const BasicValueObject* pBasicValueObject = dynamic_cast<const BasicValueObject*> (pObject) )
		{
			newPrefix = pBasicValueObject->GetCompleteObjectNameToObjectType( NULL, SVRootObjectType );
		}
		else
		{
			newPrefix = pObject->GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType ) + _T( "." );
		}// end else
		oldPrefix = newPrefix;
		SvUl_SF::searchAndReplace( oldPrefix, pObject->GetName(), originalName );

		int iSize = m_svoArchiveResultNames.GetResultSize();
		int iLastSet = m_svoArchiveResultNames.GetLastSetIndex();

		for (int i = 0; i < iSize; i++ )
		{
			CString sName;
			m_svoArchiveResultNames.GetValue(iLastSet,i,sName);
			if ( sName.Replace( oldPrefix.c_str(), newPrefix.c_str() ) > 0 )
			{
				Result = true;
			}
			m_svoArchiveResultNames.SetValue(iLastSet,i,sName);
			m_svoArchiveResultNames.GetValue(iLastSet,i,sName);
		}
	}

	return Result;
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

void SVArchiveTool::getTranslatedImagePath(CString &ImagePath)
{
	ImagePath = m_ImageTranslatedPath;
}

