//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVArchiveRecord.h"
#include "SVArchiveTool.h"
#include "SVGlobal.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVMemoryManager.h"
#include "ArchiveToolHelper.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVArchiveImageThreadClass.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"
#include "SVImageObjectClass.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#pragma region Constructor
SVArchiveRecord::SVArchiveRecord()
{
	m_ImageBufferPtr = new SVImageObjectClass;

	m_lCountImages = 1L;
	m_pArchiveTool = nullptr;

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
	LPCTSTR sPoolName;
	if ( m_eArchiveMethod != SVArchiveSynchronous && m_eArchiveMethod != SVArchiveInvalidMethod )
	{
		sPoolName = m_eArchiveMethod == SVArchiveGoOffline
				  ? SvO::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME
				  : SvO::ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME;
		HRESULT hrAllocate = TheSVMemoryManager().ReleasePoolMemory(sPoolName, this);
	}

	m_aFileNames.clear();
	m_LastIndexHandle.clear();
	m_pImageCircleBuffer.clear();
}
#pragma endregion Constructor

#pragma region Public Methods
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

			m_lCountImages = 1;        // reset to overwrite existing image file.
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

		if(m_pArchiveTool->isImagePathUsingKeywords())
		{
			m_pArchiveTool->getTranslatedImagePath(csPathRoot);
		}
		else
		{
			if (!m_pArchiveTool->GetImageArchivePath (csPathRoot))
			{
				hr = -1903;
				break;
			}
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
	if( SV_GUID_NULL != m_svObjectReference.Guid() )
	{
		//
		// Get a pointer to the object based on the guid.
		//
		SVObjectClass* l_psvObject = nullptr;

		try
		{
			l_psvObject = dynamic_cast<SVObjectClass*>( SVObjectManagerClass::Instance().GetObject( m_svObjectReference.Guid() ) );

			if( nullptr != l_psvObject )
			{
				if( nullptr == dynamic_cast<SVValueObjectClass*>( l_psvObject ) &&
					nullptr == dynamic_cast<SVImageClass*>( l_psvObject ) )
				{
					l_psvObject = nullptr;
				}
			}
		}
		catch( ... )
		{
			l_psvObject = nullptr;
		}

		if ( l_psvObject != m_svObjectReference.Object() )
		{
			long lArrayIndex = m_svObjectReference.ArrayIndex();
			m_svObjectReference = l_psvObject;
			m_svObjectReference.SetArrayIndex( lArrayIndex );
		}
	}
	
	if( SV_GUID_NULL != m_svObjectReference.Guid() )
	{
		ASSERT(m_pArchiveTool);

		SVInObjectInfoStruct InObjectInfo;

		InObjectInfo.PObject                    = m_pArchiveTool;
		InObjectInfo.UniqueObjectID             = m_pArchiveTool->GetUniqueObjectID();
		InObjectInfo.ObjectTypeInfo.ObjectType  = SVToolObjectType;
		
		bool rc = SVObjectManagerClass::Instance().ConnectObjectInput( m_svObjectReference.Guid(), &InObjectInfo );

		ASSERT(rc );
	}
}

void SVArchiveRecord::DisconnectInputObject()
{
	if( SV_GUID_NULL != m_svObjectReference.Guid() )
	{
		SVInObjectInfoStruct InObjectInfo;
		
		InObjectInfo.PObject                   = m_pArchiveTool;
		InObjectInfo.UniqueObjectID            = m_pArchiveTool->GetUniqueObjectID();
		InObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
		
		SVObjectManagerClass::Instance().DisconnectObjectInput(m_svObjectReference.Guid(), &InObjectInfo);
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
		info.setDibBufferFlag(false);
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
		__int64 l_lImageBufferSize = m_lImageSize * lBufferSize;
		
		// reserve memory
		HRESULT hrAllocate = TheSVMemoryManager().ReleasePoolMemory(SvO::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, this);
		if ( m_eArchiveMethod == SVArchiveGoOffline )
		{
			hrAllocate = TheSVMemoryManager().ReservePoolMemory(SvO::ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME, this, l_lImageBufferSize );
			hr = hrAllocate;
		}
		else
		{
			hr = S_OK;
		}

		// now ceate buffer if reserve OK
		if ( S_OK == hr && SVArchiveAsynchronous != m_eArchiveMethod )
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
	bOk = nullptr != pImage;
	
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
						SVString l_strPathImageFile = csPathImageFile;

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
	SVString l_strPath = sFileName;

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
	SVImageClass* l_psvImage = nullptr;

	if( nullptr != m_svObjectReference.Object() )
	{
		GUID guid = m_svObjectReference.Guid();
	
		l_psvImage = dynamic_cast<SVImageClass*>( SVObjectManagerClass::Instance().GetObject(guid) );
	}

	return l_psvImage;
}
#pragma endregion Public Methods