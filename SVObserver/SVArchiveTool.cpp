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

#include "stdafx.h"
#include <io.h>
#include "SVArchiveTool.h"

#include "SVDataManagerLibrary/DataManager.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVFileNameManagerClass.h"
#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVGlobal.h"
#include "SVImageClass.h"
#include "SVImageListClass.h"
#include "SVInspectionProcess.h"
#include "SVMemoryManager.h"
#include "SVSVIMStateClass.h"
#include "SVToolSet.h"
#include "SVArchiveHeaderEditDlg.h"
#include "SVVisionProcessorHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const CString ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME = _T("ArchiveToolOnlineAsync");
const CString ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME = _T("ArchiveToolGoOffline");

SVArchiveRecord::SVArchiveRecord()
{
	m_ImageBufferPtr = new SVImageObjectClass;

	m_lCountImages = 1L;
	m_pArchiveTool = NULL;

	m_pImageCircleBuffer.clear();
	m_LastIndexHandle.clear();

	m_lLastIndex = m_lCountImages;
	m_lMaxIndex = 0;
	m_eArchiveMethod = SVArchiveInvalidMethod;
	m_lImageSize = 0;

	BSTR bstrName;
	CString strName;

	// Create a managed index for the input circle buffer
	strName = _T( "Archive Record Image Pool" );
	bstrName = strName.AllocSysString();

	TheSVDataManager.CreateManagedIndexArray( m_pImageCircleBuffer, bstrName, 1 );

	::SysFreeString( bstrName );

	TheSVDataManager.GetNextAvailableBufferIndex( m_pImageCircleBuffer, SV_ARCHIVE, m_LastIndexHandle );
}

SVArchiveRecord::~SVArchiveRecord()
{
	CString sPoolName;
	if ( m_eArchiveMethod != SVArchiveSynchronous && m_eArchiveMethod != SVArchiveInvalidMethod )
	{
		sPoolName = m_eArchiveMethod == SVArchiveGoOffline
				  ? ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME
				  : ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME;
		HRESULT hrAllocate = TheSVMemoryManager().ReleasePoolMemory(sPoolName, this);
	}

	m_aFileNames.clear();
	m_LastIndexHandle.clear();
	m_pImageCircleBuffer.clear();
}

void SVArchiveRecord::InitArchiveRecord( SVArchiveTool* p_pToolArchive, SVObjectReference p_refObject )
{
	ASSERT( p_pToolArchive );
	
	m_pArchiveTool = p_pToolArchive;
	m_eArchiveMethod = m_pArchiveTool->m_eArchiveMethod;

	m_svObjectReference = p_refObject;
	
	if ( dynamic_cast <SVImageClass*> (m_svObjectReference.Object()) )
	{
		BuildFileName();
	}
}

void SVArchiveRecord::BuildFileName()
{
	ASSERT(m_svObjectReference.Object());
	m_csFileNameImage = m_csImageObjectName = m_svObjectReference.Object()->GetCompleteObjectName();
	m_csFileNameImage.Replace( ".", "_" );
}

HRESULT SVArchiveRecord::BuildArchiveImageFilePath(CString& rcsPath)
{
	HRESULT	hr;

	DWORD		dwMaxImages;
	DWORD		dwStopAtMaxCount;

	CString	csPathRoot;
	
	hr = 0;
	dwMaxImages = 0;
	dwStopAtMaxCount = 0;

	//
	// Check what is required when the max image count is reached.
	//
	do
	{
		ASSERT(m_csFileNameImage.GetLength() > 0);

		ASSERT(m_pArchiveTool);

		m_pArchiveTool->m_dwArchiveMaxImagesCount.GetValue(dwMaxImages);
		m_pArchiveTool->m_dwArchiveStopAtMaxImages.GetValue(dwStopAtMaxCount);
	
		if (m_lCountImages > (long)dwMaxImages)
		{
			if (dwStopAtMaxCount)
			{
				hr = -1902;
				break;
			}

			m_lCountImages = 1;        // reset to over write existin image file.
		}
		
		//
		// Insert the image count as part of the image file name.
		//
		CString csImageNameCountAndExtension;
		csImageNameCountAndExtension.Format( _T("%s__%06.6ld.bmp"), m_csFileNameImage, m_lCountImages);

		m_lLastIndex = m_lCountImages;
		m_lMaxIndex = std::max( m_lMaxIndex, m_lLastIndex );

		m_lCountImages++;
		
		SVFileNameClass svFileName;

		if (!m_pArchiveTool->GetImageArchivePath (csPathRoot))
		{
			hr = -1903;
			break;
		}

		svFileName.SetPathName( csPathRoot );
		svFileName.SetFileName( csImageNameCountAndExtension );

		rcsPath = svFileName.GetFullFileName();

		if (   m_eArchiveMethod == SVArchiveAsynchronous
			|| m_eArchiveMethod == SVArchiveGoOffline )
		{
			// dynamically reserve enough space the first time through
			if ( m_aFileNames.size() <= dwMaxImages )
			{
				m_aFileNames.resize( dwMaxImages + 1 );	// index is 1-based, so add one extra
			}

			TheSVDataManager.GetNextAvailableBufferIndex( m_pImageCircleBuffer, SV_ARCHIVE, m_LastIndexHandle );

			m_aFileNames[ m_lLastIndex ] = SVFileNameIndexHandlePair( rcsPath, m_LastIndexHandle );
		}

	} while (false);	// end do
	
	
	return hr;
}

void SVArchiveRecord::ConnectInputObject()
{
	if( m_svObjectReference.Guid() != SVInvalidGUID )
	{
		//
		// Get a pointer to the object based on the guid.
		//
		SVObjectClass* l_psvObject = NULL;

		try
		{
			l_psvObject = dynamic_cast<SVObjectClass*>( SVObjectManagerClass::Instance().GetObject( m_svObjectReference.Guid() ) );

			if( l_psvObject != NULL )
			{
				if( dynamic_cast<SVValueObjectClass*>( l_psvObject ) == NULL &&
					dynamic_cast<SVImageClass*>( l_psvObject ) == NULL )
				{
					l_psvObject = NULL;
				}
			}
		}
		catch( ... )
		{
			l_psvObject = NULL;
		}

		if ( l_psvObject != m_svObjectReference.Object() )
		{
			long lArrayIndex = m_svObjectReference.ArrayIndex();
			m_svObjectReference = l_psvObject;
			m_svObjectReference.SetArrayIndex( lArrayIndex );
		}
	}
	
	if( m_svObjectReference.Guid() != SVInvalidGUID)
	{
		ASSERT(m_pArchiveTool);

		SVInObjectInfoStruct InObjectInfo;

		InObjectInfo.PObject                    = m_pArchiveTool;
		InObjectInfo.UniqueObjectID             = m_pArchiveTool->GetUniqueObjectID();
		InObjectInfo.ObjectTypeInfo.ObjectType  = SVToolObjectType;
		
		DWORD_PTR rc = ::SVSendMessage( m_svObjectReference.Guid(), 
		                            SVM_CONNECT_OBJECT_INPUT, 
		                            reinterpret_cast <DWORD_PTR> (&InObjectInfo), 
		                            NULL );

		ASSERT(rc == SVMR_SUCCESS );
	}
}

void SVArchiveRecord::DisconnectInputObject()
{
	if( m_svObjectReference.Guid() != SVInvalidGUID)
	{
		SVInObjectInfoStruct InObjectInfo;
		
		InObjectInfo.PObject                   = m_pArchiveTool;
		InObjectInfo.UniqueObjectID            = m_pArchiveTool->GetUniqueObjectID();
		InObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
		
		DWORD_PTR rc = ::SVSendMessage(	m_svObjectReference.Guid(), 
		                            SVM_DISCONNECT_OBJECT_INPUT, 
		                            reinterpret_cast <DWORD_PTR> (&InObjectInfo), 
		                            NULL );
	}
}

HRESULT SVArchiveRecord::QueueImage( SVMatroxBuffer& buf, CString strFileName )
{
	ASSERT( !buf.empty() );
	HRESULT hr = S_OK;

	if ( m_eArchiveMethod == SVArchiveAsynchronous )
	{
		// the QueueImage function will copy the buffer, so pass in the original here
		SVArchiveImageThreadClass::BufferInfo info( buf, strFileName, m_lImageSize, m_ImageInfo, this );
		TheSVArchiveImageThreadClass().QueueImage( info );
	}
	else
	{
		if( !( m_ImageBufferPtr.empty() ) )
		{
			m_ImageBufferPtr->SetCurrentIndex( m_LastIndexHandle );
			SVSmartHandlePointer handle;
			m_ImageBufferPtr->GetImageHandle( m_LastIndexHandle.GetIndex(), handle );

			if( !( handle.empty() ) )
			{
				SVImageBufferHandleImage l_MilBuffer;
				handle->GetData( l_MilBuffer );

				if( !( l_MilBuffer.empty() ) )
				{
					SVMatroxBufferInterface::SVStatusCode l_Code = SVMatroxBufferInterface::CopyBuffer(l_MilBuffer.GetBuffer(), buf );
					// SafeImageCopyToHandle	// no... this happens in the inspection, therefore not necessary
													// No, because we remove all previous duplicate filenames in the Queue function
													// therefore they are never written out
				}
				else
				{
					hr = E_FAIL;
				}
			}
			else
			{
				hr = E_FAIL;
			}
		}
		else
		{
			hr = E_FAIL;
		}
	}

	return hr;
}

// right now called if method == SVArchiveGoOffline or SVArchiveAsynchronous
HRESULT SVArchiveRecord::AllocateBuffers( long lBufferSize )
{
	HRESULT hr = S_FALSE;

	m_lMaxIndex = 0;
	SVImageClass* pImage = dynamic_cast <SVImageClass*> (m_svObjectReference.Object());
	ASSERT( pImage );
	if ( pImage )
	{
		SVImageInfoClass info = pImage->GetImageInfo();
		m_ImageInfo = info;

		BSTR bstrName;
		CString strName;

		// Create a managed index for the input circle buffer
		strName = _T( "Archive Record Image Pool" );
		bstrName = strName.AllocSysString();

		TheSVDataManager.CreateManagedIndexArray( m_pImageCircleBuffer, bstrName, lBufferSize + 1 );

		::SysFreeString( bstrName );

		TheSVDataManager.GetNextAvailableBufferIndex( m_pImageCircleBuffer, SV_ARCHIVE, m_LastIndexHandle );

		m_lImageSize = SVArchiveTool::CalculateImageMemory( pImage );
		long l_lImageBufferSize = m_lImageSize * lBufferSize;

		// reserve memory
		HRESULT hrAllocate = TheSVMemoryManager().ReleasePoolMemory(ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, this);
		if ( m_eArchiveMethod == SVArchiveGoOffline )
		{
			hrAllocate = TheSVMemoryManager().ReservePoolMemory(ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, this, l_lImageBufferSize );
			hr = hrAllocate;
		}
		else
		{
			hr = S_OK;
		}

		// now ceate buffer if reserve OK
		if ( hr == S_OK && m_eArchiveMethod != SVArchiveAsynchronous )
		{
			if( !( m_ImageBufferPtr.empty() ) )
			{
				m_ImageBufferPtr->SetImageInfo( info );
				m_ImageBufferPtr->resize( lBufferSize+1 );

				hr = m_ImageBufferPtr->ResetObject();
			}
			else
			{
				hr = E_FAIL;
			}
		}
	}

	return hr;
}

HRESULT SVArchiveRecord::WriteImageQueue()
{
	HRESULT hr = S_OK;

	if( !( m_ImageBufferPtr.empty() ) )
	{
		long lCount = m_lMaxIndex;
		for ( long l = 1; l <= lCount; l++ )
		{
			SVSmartHandlePointer handle;
			m_ImageBufferPtr->GetImageHandle( m_aFileNames[l].second.GetIndex(), handle );

			if( !( handle.empty() ) )
			{
				SVImageBufferHandleImage l_MilBuffer;
				handle->GetData( l_MilBuffer );

				if ( !l_MilBuffer.empty() )
				{
					WriteImage( l_MilBuffer.GetBuffer(), m_aFileNames[l].first );
				}
			}
			else
			{
				hr = E_FAIL;
			}
		}
	}
	else
	{
		hr = E_FAIL;
	}

	return hr;
}

HRESULT SVArchiveRecord::WriteImage( )
{
	HRESULT hr = S_OK;
	bool bOk;

	SVImageClass* pImage = dynamic_cast <SVImageClass*> (m_svObjectReference.Object());
	bOk = pImage != NULL;
	
	if ( bOk )
	{
		//
		// An image in our list is archivable.
		//
		//
		// Create a file and convert the image to a .bmp type 
		// file.
		//
		SVSmartHandlePointer ImageHandle;
		bOk = pImage->GetImageHandle( ImageHandle ) && !( ImageHandle.empty() );
		if ( bOk )
		{
			SVMatroxBufferInterface::SVStatusCode l_Code;

			SVImageBufferHandleImage l_MilBuffer;
			ImageHandle->GetData( l_MilBuffer );

			SVMatroxBuffer milBuffer = l_MilBuffer.GetBuffer();
			CString csPathImageFile;

			//
			// Write the MIL image to a file in BMP form.
			//
			hr = BuildArchiveImageFilePath( csPathImageFile );
			if (hr & 0xc000)
			{
				bOk = FALSE;
			}
			else
			{
				bOk = TRUE;
			}

			if ( bOk && !milBuffer.empty() )
			{
				if ( m_eArchiveMethod == SVArchiveSynchronous )
				{
					try
					{
						SVMatroxString l_strPathImageFile = csPathImageFile;

						l_Code = SVMatroxBufferInterface::Export(milBuffer, 
							l_strPathImageFile, 
							SVFileBitmap );

					}
					catch (CException& )
					{
						hr = SV_FALSE;
					}
					catch (...)
					{
						hr = SV_FALSE;
					}
				}
				else	// SVArchiveGoOffline or SVArchiveAsynchronous
				{
					QueueImage( milBuffer, csPathImageFile );
				}
			}
		}
	}


	return hr;
}
/*static*/HRESULT SVArchiveRecord::WriteImage( SVMatroxBuffer& milBuffer, CString sFileName )
{
	HRESULT hr = S_OK;
	
	SVMatroxBufferInterface::SVStatusCode l_Code;
	SVMatroxString l_strPath = sFileName;

	if ( !milBuffer.empty() )
	{
		try
		{
			l_Code = SVMatroxBufferInterface::Export( milBuffer, l_strPath, SVFileBitmap);

		}
		catch (CException& )
		{
			hr = SV_FALSE;
		}
		catch (...)
		{
			hr = SV_FALSE;
		}
	}// end if ( bOk )

	return hr;
}

void SVArchiveRecord::Init( SVArchiveTool* pTool )
{
	m_pArchiveTool = pTool;
	m_eArchiveMethod = pTool->m_eArchiveMethod;
}


SVImageClass* SVArchiveRecord::GetImage()
{
	SVImageClass* l_psvImage = NULL;

	if( m_svObjectReference.Object() != NULL )
	{
		GUID guid = m_svObjectReference.Guid();
	
		l_psvImage = dynamic_cast<SVImageClass*>( SVObjectManagerClass::Instance().GetObject(guid) );
	}

	return l_psvImage;
}

SVArchiveRecordsArray::SVArchiveRecordsArray()
{
	m_pArchiveTool = NULL;
}

SVArchiveRecordsArray::~SVArchiveRecordsArray()
{
	ClearArray();
}

void SVArchiveRecordsArray::ResetImageCounts()
{
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pRecord = m_vecRecords.at(i);
		if (pRecord)
		{
			//
			// Reset the first image 'index' to 1.
			//
			pRecord->m_lCountImages = 1L;
		}
	}
}

void SVArchiveRecordsArray::ClearArray()
{
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pRecord = m_vecRecords.at(i);
		if (pRecord)
		{
			//
			// Make sure object is 'disconnected' as registered input.
			//
			pRecord->DisconnectInputObject();
			delete pRecord;
		}
	}
	m_vecRecords.clear();
}


HRESULT SVArchiveRecordsArray::InitializeObjects(SVArchiveTool* p_pToolArchive, SVStringValueObjectClass& p_svoObjects )	// use array capability of string vo
{
	HRESULT hr = S_OK;
	ASSERT( m_pArchiveTool != NULL );
	ClearArray();
	SVInspectionProcess* pInspection = m_pArchiveTool->GetInspection();
	ASSERT( pInspection );

	int iSize = p_svoObjects.GetResultSize();
	for ( int i = 0; i < iSize; i++ )
	{
		CString sName;
		long l_lIndex = p_svoObjects.GetLastSetIndex();
		p_svoObjects.GetValue( l_lIndex, i, sName );
		if ( !sName.IsEmpty() )
		{
			SVArchiveRecord* pArchiveRecord = new SVArchiveRecord;
			SVObjectReference ref;
			int iPos = sName.Find('.',0);
			if( iPos > -1)	// This assumes that the first part of the dotted name is the inspection.
			{				// Build the object name with the current inspection name.
				CString sNewName = p_pToolArchive->GetInspection()->GetName();
				sNewName += sName.Mid(iPos); 
				HRESULT hrGetObject = SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( sNewName ), ref );
				if( hrGetObject == S_OK && sNewName != sName)
				{			// Set value with new inspection name.
					p_svoObjects.SetValue(l_lIndex, i, sNewName);
				}
			}
			else
			{				// We should always find a dotted name here.
				ASSERT(FALSE);
			}

			if ( ref.Object() == NULL )
			{
				TRACE( _T( "SVArchiveRecordsArray::InitializeObjects-ToolName=%s-ObjectName=%s\n" ), m_pArchiveTool->GetCompleteObjectName(), sName );
			}
			else
			{
				pArchiveRecord->InitArchiveRecord( p_pToolArchive, ref );
			}
			m_vecRecords.push_back(pArchiveRecord);
		}
	}
	return hr;
}

void SVArchiveRecordsArray::ConvertStringToGuids( SVArchiveTool * pToolArchive,
                                                  LPCTSTR pszStrValue )
{
	ClearArray();
	
	CString csTemp = pszStrValue;
	
	BOOL bDone = FALSE;
	while (!bDone)
	{
		CString csGuid;
		int nIndex = csTemp.Find((TCHAR)'\n',0);
		if (nIndex > -1)
		{
			csGuid = csTemp.Left(nIndex);
		}
		else
		{
			csGuid = csTemp;
			bDone = TRUE;             // we are done 
		}
		//
		// Adjust for next iteration.
		//
		csTemp = csTemp.Right(csTemp.GetLength() - nIndex -1);
		
		//
		// Convert string guid to guid structure
		//
		GUID objectGuid;
		// convert the guidStr to a Guid
		AfxGetClassIDFromString(csGuid, &objectGuid);
		
		//
		// The image record has a dotted name that needs to be 
		// associated with a pointer to an image later.
		//
		if ((objectGuid != SVInvalidGUID) && csGuid.GetLength())
		{
			SVArchiveRecord* pArchiveRecord = new SVArchiveRecord;
			SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( objectGuid );
			pArchiveRecord->InitArchiveRecord( pToolArchive, SVObjectReference( pObject ) );
			m_vecRecords.push_back(pArchiveRecord);
		}
	}
}

std::vector<CString> SVArchiveRecordsArray::RemoveDisconnectedObject( const SVObjectInfoStruct& p_rInfoObject )
{
	std::vector<CString> vecRemoved;

	// Effective STL Item 9 p46: forward iteration / erase
	int nCount = static_cast< int >( m_vecRecords.size() );
	RecordsType::iterator iter;
	for ( iter = m_vecRecords.begin(); iter != m_vecRecords.end();  )
	{
		SVArchiveRecord* pImageRecord = *iter;
		if ( pImageRecord )
		{
			if( p_rInfoObject.UniqueObjectID == pImageRecord->m_svObjectReference.Guid() )
			{
				vecRemoved.push_back( pImageRecord->m_svObjectReference.GetCompleteObjectName() );
				delete pImageRecord;
				iter = m_vecRecords.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	return vecRemoved;
}


/////////////////////////////////////////////////////////////////////////////
//
void SVArchiveRecordsArray::ValidateImageObjects()
{
	//
	// Validate the image objects to be archived.
	//
	// Effective STL Item 9 p46: forward iteration / erase
	int nCount = static_cast< int >( m_vecRecords.size() );
	RecordsType::iterator iter;
	for ( iter = m_vecRecords.begin(); iter != m_vecRecords.end(); )
	{
		SVArchiveRecord* pImageRecord = *iter;
		ASSERT(pImageRecord);
		pImageRecord->DisconnectInputObject();
		
		SVImageClass* pImageObject = pImageRecord->GetImage();
		if (pImageObject)
		{
			pImageRecord->m_svObjectReference = pImageObject;
			pImageRecord->BuildFileName();
			pImageRecord->ConnectInputObject();

			++iter;
		}
		else
		{
			//
			// Remove this image record from our list.
			//
			delete pImageRecord;
			iter = m_vecRecords.erase(iter);
		}
	}
}


void SVArchiveRecordsArray::SetArchiveTool( SVArchiveTool* pTool )
{
	m_pArchiveTool = pTool;
	RecordsType::iterator iter;
	for ( iter = m_vecRecords.begin(); iter != m_vecRecords.end(); ++iter )
	{
		(*iter)->Init(m_pArchiveTool);
	}
}

BOOL SVArchiveRecordsArray::WriteArchiveImageFiles( )
{
	ASSERT( m_pArchiveTool != NULL );
	BOOL bOk = TRUE;
	HRESULT	hr = 0;
	
	int nCount = static_cast< int >( m_vecRecords.size() );
	
	for ( int i = 0; bOk && i < nCount; i++ )
	{
		SVArchiveRecord* pImageRecord = m_vecRecords.at(i);
		bOk = bOk && (pImageRecord->WriteImage( ) == S_OK);
	}// end for ( int i = 0; bOk && i < nCount; i++ )
	
	return bOk;
}

/////////////////////////////////////////////////////////////////////////////
//
//
// Make sure the results objects to archive are valid.  
// 'Connect as input' the good ones and remove bad ones.
//
// Return the final count of results objects to arhive.
//
int SVArchiveRecordsArray::ValidateResultsObjects()
{
	// Effective STL Item 9 p46: forward iterate / erase

	RecordsType::iterator iter;
	for ( iter = m_vecRecords.begin(); iter != m_vecRecords.end(); )
	{
		SVArchiveRecord* pResultRecord = *iter;
		pResultRecord->DisconnectInputObject();
		
		SVValueObjectClass* pValueObject = NULL;

		GUID guid = pResultRecord->m_svObjectReference.Guid();

		pValueObject = dynamic_cast <SVValueObjectClass*> (SVObjectManagerClass::Instance().GetObject(guid));

		bool bRecordOK = false;
		if (pValueObject)
		{
			if ( static_cast <SVObjectClass*> (pValueObject) != pResultRecord->m_svObjectReference.Object() )
			{
				long lArrayIndex = pResultRecord->m_svObjectReference.ArrayIndex();
				pResultRecord->m_svObjectReference = pValueObject;
				pResultRecord->m_svObjectReference.SetArrayIndex( lArrayIndex );
			}

			SVValueObjectReference voref( pResultRecord->m_svObjectReference );

			CString sTemp;
			if ( voref.GetValue(0, sTemp) != SVMSG_SVO_33_OBJECT_INDEX_INVALID )
			{
				bRecordOK = true;
				pResultRecord->ConnectInputObject();
				++iter;
			}
		}

		if ( !bRecordOK )
		{
			pResultRecord->m_svObjectReference = NULL;
			iter = m_vecRecords.erase(iter);
		}
	}
	
	//
	// return the count of objects to arhive.
	//
	return static_cast< int >( m_vecRecords.size() );
}

/////////////////////////////////////////////////////////////////////////////
//
CString SVArchiveRecordsArray::BuildResultsArchiveString()
{
	CString csArchive;
	
	BOOL bFirst = TRUE;	
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pResultRecord = m_vecRecords.at(i);
		ASSERT(pResultRecord);
		//
		// We assume all archivable objects are Value Objects.
		//
		SVValueObjectReference voref( pResultRecord->m_svObjectReference );

		ASSERT( voref.Object() );

		if ( voref.Object() )
		{
			CString csTemp;

			HRESULT hr = voref.GetValue( csTemp );
			if ( hr == S_OK || hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
			{
				if ( bFirst )
				{
					bFirst = FALSE;
				}
				else
				{
					csArchive += _T(", ");
				}

			//
			// Get a string to write to the archive file.
			//
			csArchive += csTemp;
		}
	}
	}

	return csArchive;
}

void SVArchiveRecordsArray::DisconnectAllResultObjects()
{
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pResultRecord = m_vecRecords.at(i);
		if ( pResultRecord )
		{
			pResultRecord->DisconnectInputObject();
		}
	}
}

HRESULT SVArchiveRecordsArray::AllocateBuffers( long lBufferSize )
{
	HRESULT hr = S_OK;
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pResultRecord = m_vecRecords.at(i);
		HRESULT hrRecord = pResultRecord->AllocateBuffers( lBufferSize );
		if ( hr == S_OK )
			hr = hrRecord;
	}
	return hr;
}

HRESULT SVArchiveRecordsArray::WriteImageQueue()
{
	HRESULT hr = S_OK;
	int nCount = static_cast< int >( m_vecRecords.size() );
	for (int i = 0; i < nCount; i++)
	{
		SVArchiveRecord* pResultRecord = m_vecRecords.at(i);
		HRESULT hrRecord = pResultRecord->WriteImageQueue();
		if ( hr == S_OK )
			hr = hrRecord;
	}
	return hr;
}


int SVArchiveRecordsArray::GetSize()
{
	return static_cast< int >( m_vecRecords.size() );
}

SVArchiveRecord* SVArchiveRecordsArray::GetAt(int i)
{
	return m_vecRecords.at(i);
}

int SVArchiveRecordsArray::Add( SVArchiveRecord* pRecord )
{
	m_vecRecords.push_back( pRecord );
	return static_cast< int >( m_vecRecords.size() - 1 );
}

SVArchiveImageThreadClass::SVArchiveImageThreadClass()
{
	m_hThread = NULL;
	m_dwThreadId = 0;
	m_bRunThread = false;
	m_hExitEvent = NULL;
}

SVArchiveImageThreadClass::~SVArchiveImageThreadClass()
{
	GoOffline();
}

HRESULT SVArchiveImageThreadClass::GoOnline()
{
	if ( !m_bRunThread )
	{
		// clear queue
		m_Queue.clear();

		// create thread
		m_bRunThread = true;
		ASSERT( m_hExitEvent == NULL );
		m_hExitEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
		ASSERT( m_hThread == NULL );

		m_hThread = ::CreateThread( NULL, 0, SVArchiveImageThreadClass::ThreadEntry, this, 0, &m_dwThreadId );

		::SetThreadPriority( m_hThread, THREAD_PRIORITY_NORMAL );
		
		SVThreadManager::Instance().Add( m_hThread, "Archive Tools(Asynchronous)", SVAffinityUser );
	}
	return S_OK;
}

HRESULT SVArchiveImageThreadClass::GoOffline()
{
	if ( m_bRunThread )
	{
		m_bRunThread = false;
		ASSERT( m_hExitEvent != NULL );
		::SetEvent( m_hExitEvent );
		::CloseHandle( m_hThread );	// can be done on a live thread
		SVThreadManager::Instance().Remove( m_hThread );
		m_dwThreadId = 0;
	}
	return S_OK;
}

// executes in the Inspection thread 
HRESULT SVArchiveImageThreadClass::QueueImage( BufferInfo p_BufferInfo )
{
	HRESULT hr = S_OK;

	// perform this code in the inspection thread, not the archive tool thread

	{// begin lock scope block
		
		SVMatroxBufferInterface::SVStatusCode l_Code;

		CSingleLock lock( &m_mtxQueue, TRUE );

		QueueType::iterator iter;
		for ( iter = m_Queue.begin(); iter != m_Queue.end(); ++iter )
		{
			if ( iter->strFilename == p_BufferInfo.strFilename )
				break;
		}

		if ( iter != m_Queue.end() )	// found filename
		{
			BufferInfo& rBufferInfo = *iter;
			// must do the copy with the queue locked
			// ** COPY BUFFER **
			SVSmartHandlePointer l_DestHandle;
			rBufferInfo.pImageObject->GetImageHandle( 0, l_DestHandle );

			if( !( l_DestHandle.empty() ) )
			{
				SVImageBufferHandleImage l_MilBuffer;
				l_DestHandle->GetData( l_MilBuffer );

				if ( !l_MilBuffer.empty() )
				{

					l_Code = SVMatroxBufferInterface::CopyBuffer(l_MilBuffer.GetBuffer(), p_BufferInfo.id );
					// at this point, p_BufferInfo.id is the source buffer

					rBufferInfo.id = l_MilBuffer.GetBuffer();	// switch over to copy

					// update timestamp
					rBufferInfo.m_Timestamp = SVClock::GetTimeStamp();
				}
			}
		}// end if ( iter != m_Queue.end() )	// found filename
		else
		{
			HRESULT hrAllocate = TheSVMemoryManager().ReservePoolMemory(ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME, this, p_BufferInfo.lBufferSize );
			if ( hrAllocate == S_OK )	// if enough memory in queue
			{
				lock.Unlock();	// do the least possible amount of work with this locked

				// ** CREATE BUFFER **
				SVImageObjectClassPtr pImageObject = new SVImageObjectClass;
				if( !( pImageObject.empty() ) )
				{
					pImageObject->SetImageInfo( p_BufferInfo.info );
					pImageObject->resize( 1 );

					hr = pImageObject->ResetObject();
				}
				else
				{
					hr = E_FAIL;
				}

				if ( hr == S_OK )
				{
					p_BufferInfo.pImageObject = pImageObject;

					// ** COPY BUFFER **
					SVSmartHandlePointer l_DestHandle;
					pImageObject->GetImageHandle( 0, l_DestHandle );

					if( !( l_DestHandle.empty() ) )
					{
						SVImageBufferHandleImage l_MilBuffer;
						l_DestHandle->GetData( l_MilBuffer );

						if ( !l_MilBuffer.empty() )
						{
							// at this point, p_BufferInfo.id is the source buffer
							SVMatroxBufferInterface::CopyBuffer( l_MilBuffer.GetBuffer(), p_BufferInfo.id );
							p_BufferInfo.id = l_MilBuffer.GetBuffer();	// switch over to copy


							// ** ADD NEW BUFFER TO QUEUE **
							p_BufferInfo.m_Timestamp = SVClock::GetTimeStamp();
							lock.Lock();
							m_Queue.push_back( p_BufferInfo );
							lock.Unlock();
						}
					}
				}

			}// if enough memory in queue
			else	// not enough room on queue
			{
				// find oldest entry from source AT
				SVClock::SVTimeStamp iOldest = SVClock::GetTimeStamp();
				QueueType::iterator iterOldest = m_Queue.end();
				for ( iter = m_Queue.begin(); iter != m_Queue.end(); ++iter )
				{
					if ( iter->pRecord == p_BufferInfo.pRecord && iter->m_Timestamp < iOldest )
					{
						iOldest = iter->m_Timestamp;
						iterOldest = iter;
					}
				}

				if ( iterOldest != m_Queue.end() )	// if found another entry by the same archive record
				{
					// assume if it came from the same archive record that the
					// image size, info, etc. is identical, therefore, just copy over current MIL buffer.
					BufferInfo& rBufferInfo = *iterOldest;

					// must do the copy with the queue locked
					SVSmartHandlePointer l_DestHandle;
					iterOldest->pImageObject->GetImageHandle( 0, l_DestHandle );

					if( !( l_DestHandle.empty() ) )
					{
						SVImageBufferHandleImage l_MilBuffer;
						l_DestHandle->GetData( l_MilBuffer );

						if ( !l_MilBuffer.empty() )
						{
							// at this point, p_BufferInfo.id is the source buffer
							SVMatroxBufferInterface::CopyBuffer( l_MilBuffer.GetBuffer(), p_BufferInfo.id );
							rBufferInfo.id = l_MilBuffer.GetBuffer();	// switch over to copy
							// update timestamp
							rBufferInfo.m_Timestamp = SVClock::GetTimeStamp();
						}
					}
				}
			}// end else not enough room on queue
		}// end if not found filename in queue else

	}// end lock scope block

	return S_OK;
}

DWORD WINAPI SVArchiveImageThreadClass::ThreadEntry( LPVOID lpParam )
{
	SVArchiveImageThreadClass* pThis = reinterpret_cast <SVArchiveImageThreadClass*> (lpParam);

	return pThis->ThreadFunction();
}

DWORD SVArchiveImageThreadClass::ThreadFunction( )
{
	// handle to app thread, handle to exit event
	// timeout 1 ms
	HANDLE ahObjects[2];
	BOOL bDuplicateHandle = ::DuplicateHandle(  ::GetCurrentProcess(),
	                                            AfxGetApp()->m_hThread,
	                                            ::GetCurrentProcess(),
	                                            &(ahObjects[0]),
	                                            0, FALSE, DUPLICATE_SAME_ACCESS );
	ASSERT( m_hExitEvent != NULL );
	ahObjects[1] = m_hExitEvent;

	const DWORD dwTimeoutMilliseconds = 10;
	DWORD dwWait;
	while ( (dwWait = ::WaitForMultipleObjects( 2, &(ahObjects[0]), FALSE, dwTimeoutMilliseconds ) ) == WAIT_TIMEOUT )
	{
		int iNumTimes = 1;
		while ( iNumTimes-- > 0 )
		{
			if ( m_Queue.size() > 0 )
			{
				HRESULT hr = PopAndWrite();
			}
		}
	}

	::CloseHandle( ahObjects[0] );
	::CloseHandle( m_hExitEvent );
	m_hExitEvent = NULL;

	// finish writing buffers
	while ( m_Queue.size() > 0 )
	{
		HRESULT hr = PopAndWrite();
		::Sleep(1);
	}


	m_hThread = NULL;
	return 0;
}

HRESULT SVArchiveImageThreadClass::PopAndWrite()
{
	{// begin lock scope block
		// lock not needed if pushing in APC (WHICH WE'RE NOT DOING)... 
		// lock is still needed because we are pushing in the inspection thread
		CSingleLock lock( &m_mtxQueue, TRUE );

		if ( m_Queue.size() > 0 )	// recheck size while within the lock
		{
			BufferInfo info = m_Queue.front();
			m_Queue.pop_front();
			HRESULT hrAllocate = TheSVMemoryManager().ReleasePoolMemory(ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME, this, info.lBufferSize );
			lock.Unlock();
			SVArchiveRecord::WriteImage( info.id, info.strFilename );

			info.pImageObject.clear();
			//TheSVDataManager.ReleaseBufferIndex( info.lDMBuffer, info.lDMIndex, SV_ARCHIVE );
		}
	}// end lock scope block

	return S_OK;
}

SV_IMPLEMENT_CLASS(SVArchiveTool, SVArchiveToolClassGuid);

SVArchiveTool::SVArchiveTool( BOOL BCreateDefaultTaskList, 
                              SVObjectClass* POwner, 
                              int StringResourceID )
              : SVToolClass(BCreateDefaultTaskList, POwner, StringResourceID)
{
	initializeArchiveTool();
	m_bDriveError = false;
}

void SVArchiveTool::initializeArchiveTool()
{
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
	//
	// CFile object.
	//
	CString csFileArchivePath;
	GetFileArchive(csFileArchivePath);

	if (csFileArchivePath.GetLength() == 0)
	{
		return FALSE;
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
		CString s;
		s.Format("ERROR: Create Archive File: %s Failed",	(LPCTSTR)csFileArchivePath);
		AfxMessageBox(s);
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
		CString sMessage;
		sMessage.Format(" %s - drive is full.", csImagePath.GetString());
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
			if((m_uiValidateCount % 10 == 0) || (m_uiValidateCount == 0))
			{
				CString csImagePath;
				GetImageArchivePath( csImagePath );
				{

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
							CString temp;
							temp.Format ("Path/File not found:  %s", csImagePath);
							AfxMessageBox (temp);

							bOk = FALSE;
						}
					}
				
					//
					// Make sure we have at least 100 Meg bytes space on the drive.
					//
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
				return SVMR_SUCCESS;
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
	// INEFFICIENT N^2 IMPLEMENTATION!!!

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
		
		//
		// Iterate the current list of results to archive.
		//
		int nCountResults = m_arrayResultsInfoObjectsToArchive.GetSize();
		for (int k = 0; k < nCountResults; k++)
		{
			// SVObjectInfoStruct InfoItemArchive = 
			//	arrayResultsInfoObjectsToArchive.GetAt(k);
			SVArchiveRecord* pResultRecord = m_arrayResultsInfoObjectsToArchive.GetAt( k );
			
			//
			// Compare and set the archivable attributes set if a match.
			//
			if ( pResultRecord->GetObjectReference() == ref )
			{
				if (ref.ObjectAttributesAllowed() & SV_ARCHIVABLE)
				{
					ref.ObjectAttributesSetRef() |= SV_ARCHIVABLE;
				}
			}
		}// end for (int k = 0; k < nCountResults; k++)
	}// end for (int i = 0; i < nCount; i++)
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


BOOL SVArchiveTool::renameToolSetSymbol(SVObjectClass* pObject, LPCTSTR orgName)
{
	CString newPrefix;
	CString oldPrefix;

	if( SVInspectionProcess* l_pInspection = dynamic_cast< SVInspectionProcess* >( pObject ) )
	{
		newPrefix = l_pInspection->GetCompleteObjectNameToObjectType( NULL, SVInspectionObjectType ) + _T( "." );
		oldPrefix = newPrefix;
		oldPrefix.Replace( l_pInspection->GetName(), orgName );
	}// end if
	else
	{
		newPrefix = _T( "\"" ) + pObject->GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType ) + _T( "." );
		oldPrefix = newPrefix;
		oldPrefix.Replace( pObject->GetName(), orgName );
	}// end else


	int iSize = m_svoArchiveResultNames.GetResultSize();
	int iLastSet = m_svoArchiveResultNames.GetLastSetIndex();

	bool bReplaced = false;
	for (int i = 0; i < iSize; i++ )
	{
		CString sName;
		m_svoArchiveResultNames.GetValue(iLastSet,i,sName);
		if ( sName.Replace(oldPrefix,newPrefix) > 0 )
			bReplaced = true;
		m_svoArchiveResultNames.SetValue(iLastSet,i,sName);
		m_svoArchiveResultNames.GetValue(iLastSet,i,sName);
	}

	return bReplaced;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVArchiveTool.cpp_v  $
 * 
 *    Rev 1.12   17 Dec 2014 11:29:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added the archive name to thread affinity list before the archive thread is created so the user will see it in the thread list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   17 Dec 2014 07:15:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Enum name for thread attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   09 Dec 2014 13:28:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed the text for the archive tool thread from "SVArchiveTool "to "Archive Tools(Asychronous)".
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Dec 2014 13:06:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   SVThreadManager thread attribute and lables.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Jul 2014 07:41:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed Attribute where Header Lables were not showing up in results picker and SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 May 2014 10:09:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   23 Apr 2014 12:34:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  903
 * SCR Title:  Fix Archive Tool Bug where inputs get lost when inspection name changes.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed un-used code from ResetObject.
 * Modified SVArchiveRecordsArray::InitializeObjects to use the current inspection name for looking up objects with GetObjectByDottedName(.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Feb 2014 10:16:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   11 Nov 2013 07:10:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add functionality for headers in text archive.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Oct 2013 12:05:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 Sep 2013 09:48:52   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  857
 * SCR Title:  Change Archive Tool Default Mode to Asynchronous
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Set default value for ArchiveMethod to asynchronous.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:01:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.78   29 Apr 2013 14:04:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  830
   SCR Title:  Consolidate all Time Stamp and Clock Functionality
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 09:33:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.77   08 Jan 2013 11:50:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  805
   SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.76   01 Nov 2012 07:08:12   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  799
   SCR Title:  Fix Bug  "Archive Tool not working in Regression Test Mode".
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Regression Mode state flag to onRun function.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.75   04 Oct 2012 10:36:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changed from post incrementor to pre incrementor.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.74   01 Aug 2012 08:44:56   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  777
   SCR Title:  Change default drive from E to D
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Change the method initializeArchiveTool to use the "D" drive instead of "E"
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.73   30 Jul 2012 12:53:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated the parameter change for the visitor fucntionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.72   27 Jul 2012 08:10:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed include information and updated overlay collection functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.71   25 Jul 2012 13:59:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removing dead code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.70   09 Jul 2012 13:59:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to change the include for SVIM State.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.69   02 Jul 2012 16:11:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.68   18 Jun 2012 17:48:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.67   28 Feb 2012 09:03:24   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  752
   SCR Title:  Fix configuration print for the Archive Tool
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed the print flag for Archived Result Names and Archived Image Names
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.66   16 Sep 2011 15:38:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated to added additional debug logging.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.65   14 Jul 2011 08:16:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  729
   SCR Title:  Adjust SVObserver to fix perfomance problems due to logging
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated source code to remove logging in places that cause performance issues and added a registry controlled variable to handle the go offline data manager dumps to files.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.64   30 Mar 2011 15:04:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.40 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to use STL min/max functions instead of MFC macros.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.63   25 Feb 2011 12:07:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.62   10 Feb 2011 14:24:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.61   27 Jan 2011 10:47:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.60   08 Dec 2010 07:40:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.59   04 Nov 2010 13:18:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with image processing and display image processing classes and associated includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.58   28 Oct 2010 13:39:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.57   01 Jun 2010 13:39:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.56   15 Dec 2009 14:55:28   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated include information and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.55   06 Nov 2009 16:22:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  633
   SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     Updated issues with archiving and image tracking.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.54   03 Sep 2009 09:59:06   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated files to fix moved file includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.53   30 Jul 2009 11:12:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Merged branced changes into current code branch with appropriate updates.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.52   09 Feb 2009 16:01:50   jhanebach
   Project:  SVObserver
   Change Request (SCR) nbr:  640
   SCR Title:  Update Archive Tool to expose Selected List to remote access.
   Checked in by:  jHanebach;  Jack Hanebach
   Change Description:  
     removed setable on-line attribute
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.51   28 Jan 2009 12:02:00   jhanebach
   Project:  SVObserver
   Change Request (SCR) nbr:  640
   SCR Title:  Update Archive Tool to expose Selected List to remote access.
   Checked in by:  jHanebach;  Jack Hanebach
   Change Description:  
     Chaged archive image names and archive result names value objects' attributes to SV_REMOTELY_SETABLE
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.50   24 Jul 2007 10:49:32   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  597
   SCR Title:  Upgrade Matrox Imaging Library to version 8.0
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Update to Matrox Imaging Library 8.0
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.49   21 Jun 2007 11:51:40   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.48.1.0   14 Jul 2009 11:56:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated to use the new data manager objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.48   12 Dec 2005 09:07:56   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  533
   SCR Title:  Fix Archive Blob Array results
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated code in ConnectInputObject and ValidateResultsObjects to not overwrite the object reference if it hasn't changed (maintain array index)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.47   02 Nov 2005 09:07:42   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  512
   SCR Title:  Add Published Result Images functionality
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented published result images
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.46   20 Oct 2005 10:15:14   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed potential crash caused by C-style casts
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.45   19 Oct 2005 11:22:22   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed problem with inspection name being renamed. also will remove invalid objects during the reset.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.44   13 Oct 2005 13:55:26   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed index 1 from GetValue calls.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.43   12 Oct 2005 13:45:20   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated methods to handle issue when removing all images from the archive tool and the archive mode selection.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.42   12 Oct 2005 10:48:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Update InitArchiveRecord method to remove un-necessary parameter and updated connect methodology to handle the images and value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.41   07 Oct 2005 15:14:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Changed all ObjectReference->GetUniqueId to ObjectReference.Guid to prevent crashes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.40   06 Oct 2005 12:14:26   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Added new method to SVArchiveRecordClass: GetImage.  This method will verify that the archive record has a valid object before trying to get the GUID from that object.  It will then request the object, by GUID, from the object manager.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.39   06 Oct 2005 07:47:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed problem of incorrect Change Mode memory usage reported in Archive page
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.38   27 Sep 2005 10:15:48   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     modified the Asynchronous mode behavior to match the functional requirements - allocate image buffers on the fly.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.37   08 Sep 2005 13:09:42   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed ArchiveMethod from a DWord value object to an Enumerate value object
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   11 Aug 2005 10:11:46   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed input connection / disconnection problems
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   08 Aug 2005 14:00:36   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     renamed SVToolArchive to SVArchiveTool
   implemented usage tracking
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   03 Aug 2005 14:51:58   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed UpdateTaskObjectOutputList to work with SVObjectReference properly
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   29 Jul 2005 11:58:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   28 Jul 2005 13:49:32   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed crash on archive images
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   28 Jul 2005 13:31:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed another path from C:\TEMP to E:\TEMP
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   28 Jul 2005 13:15:32   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed default path from c:\temp to e:\temp
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   27 Jul 2005 16:04:08   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     renamed SVInspectionProcess::FindValueObject to GetValueObject
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   26 Jul 2005 13:38:30   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed file create bugs
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   19 Jul 2005 14:25:38   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     file created when going online, not on normal reset object
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   14 Jul 2005 12:33:44   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     updated to work with array value objects
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   22 Jun 2005 12:33:50   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed compile errors
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   20 Jun 2005 10:06:16   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented correct functionality for async and offline
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   20 May 2005 14:21:00   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed problems with compatability with 4.50 code (async was originally coded for 4.40)
   fixed thread queue check
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   20 May 2005 12:27:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented Asynchronous archive tool methods
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   17 Feb 2005 13:05:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated Create, Reset, and Object functionality based on SVObjectAppClass and new extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   16 Feb 2005 13:06:40   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   13 May 2004 11:32:24   jBrown
   Project:  SVObserver
   Change Request (SCR) nbr:  422
   SCR Title:  Image archiving across directories
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     The following functions were modified....
     SVArchiveRecord::BuildArchiveImageFilePath () 
       Changed return type from BOOL to HRESULT in order to get better error resolution.
       Changed method for retrieving image archive path.
     SVArchiveRecordsArray::WriteArchiveImageFiles ()
       Modified to handle new SVArchiveRecord::BuildArchiveImageFilePath () return type.
     SVArchiveTool::onRun ()
       Changed method for passing image archive path to SVArchiveRecord.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   08 Oct 2003 10:50:08   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed attributes on value objects so that they can be set by the activeX
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   17 Apr 2003 16:45:08   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   29 Jan 2003 10:45:18   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated header information and updated WriteArchiveImageFile method to use the new image circle buffer mthodolodgy.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   09 Dec 2002 18:42:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   19 Nov 2002 10:28:20   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed all references to System
   Cosmetic change: 
   TheObjectManager ->TheSVObjectManager           
   Changed ValueObject semantics to work with buckets
   initializeArchiveTool uses name of tool instead of generated temp name; 
   function CheckForUniqueArchiveFilePath no longer needed
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   04 Oct 2001 13:18:52   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  229
   SCR Title:  Upgrade SVObserver to Version 3.34 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modified source code to remove compiler warning message.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
