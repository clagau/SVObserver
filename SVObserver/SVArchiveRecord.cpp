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
#include "InspectionEngine/SVImageObjectClass.h"
#include "SVFileSystemLibrary/SVFileNameClass.h"
#include "TextDefinesSvO.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Constructor
SVArchiveRecord::SVArchiveRecord()
{
	m_ImageBufferPtr = SVImageObjectClassPtr{ new SVImageObjectClass };

	m_lCountImages = 1L;
	m_pArchiveTool = nullptr;

	m_pImageCircleBuffer.reset();
	m_LastIndexHandle.clear();

	m_lLastIndex = m_lCountImages;
	m_lMaxIndex = 0;
	m_eArchiveMethod = SVArchiveInvalidMethod;
	m_lImageSize = 0;

	_bstr_t bName( _T( "Archive Record Image Pool" ) );
	// Create a managed index for the input circle buffer

	TheSVDataManager.CreateManagedIndexArray( m_pImageCircleBuffer, bName.Detach(), 1 );

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
	m_pImageCircleBuffer.reset();
}
#pragma endregion Constructor

#pragma region Public Methods
void SVArchiveRecord::InitArchiveRecord( SVArchiveTool* p_pToolArchive, SVObjectReference p_refObject )
{
	ASSERT( p_pToolArchive );
	
	m_pArchiveTool = p_pToolArchive;
	m_eArchiveMethod = m_pArchiveTool->m_eArchiveMethod;

	m_svObjectReference = p_refObject;
	
	if ( dynamic_cast <SVImageClass*> (m_svObjectReference.getObject()) )
	{
		BuildFileName();
	}
}

void SVArchiveRecord::BuildFileName()
{
	ASSERT(m_svObjectReference.getObject());
	m_FileNameImage = m_ImageObjectName = m_svObjectReference.getObject()->GetCompleteName();
	SvUl::searchAndReplace( m_FileNameImage,  _T("."), _T("_") );
}

HRESULT SVArchiveRecord::BuildArchiveImageFilePath(std::string& rImageFile)
{
	HRESULT	hr;

	DWORD		dwMaxImages;
	DWORD		dwStopAtMaxCount;

	std::string PathRoot;
	
	hr = 0;
	dwMaxImages = 0;
	dwStopAtMaxCount = 0;

	//
	// Check what is required when the max image count is reached.
	//
	do
	{
		ASSERT(0 < m_FileNameImage.size());

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
		std::string FileName = SvUl::Format( _T("%s__%06.6ld.bmp"), m_FileNameImage.c_str(), m_lCountImages);

		m_lLastIndex = m_lCountImages;
		m_lMaxIndex = std::max( m_lMaxIndex, m_lLastIndex );

		m_lCountImages++;
		
		SVFileNameClass svFileName;

		if(m_pArchiveTool->isImagePathUsingKeywords())
		{
			m_pArchiveTool->getTranslatedImagePath(PathRoot);
		}
		else
		{
			if (!m_pArchiveTool->GetImageArchivePath (PathRoot))
			{
				hr = -1903;
				break;
			}
		}

		svFileName.SetPathName( PathRoot.c_str() );
		svFileName.SetFileName( FileName.c_str() );

		rImageFile = svFileName.GetFullFileName();

		if (   m_eArchiveMethod == SVArchiveAsynchronous
			|| m_eArchiveMethod == SVArchiveGoOffline )
		{
			// dynamically reserve enough space the first time through
			if ( m_aFileNames.size() <= dwMaxImages )
			{
				m_aFileNames.resize( dwMaxImages + 1 );	// index is 1-based, so add one extra
			}

			TheSVDataManager.GetNextAvailableBufferIndex( m_pImageCircleBuffer, SV_ARCHIVE, m_LastIndexHandle );

			m_aFileNames[ m_lLastIndex ] = SVFileNameIndexHandlePair( rImageFile, m_LastIndexHandle );
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
		SVObjectClass* pObject = nullptr;

		try
		{
			pObject = SVObjectManagerClass::Instance().GetObject( m_svObjectReference.Guid() );

			if( nullptr != pObject )
			{
				if( nullptr == dynamic_cast<SvOi::IValueObject*> (pObject) &&
					nullptr == dynamic_cast<SVImageClass*>( pObject ) )
				{
					pObject = nullptr;
				}
			}
		}
		catch( ... )
		{
			pObject = nullptr;
		}

		if ( pObject != m_svObjectReference.getObject() )
		{
			long lArrayIndex = m_svObjectReference.ArrayIndex();
			m_svObjectReference = pObject;
			m_svObjectReference.SetArrayIndex( lArrayIndex );
		}
	}
	
	if( SV_GUID_NULL != m_svObjectReference.Guid() )
	{
		ASSERT(m_pArchiveTool);

		SVInObjectInfoStruct InObjectInfo;

		InObjectInfo.SetObject(m_pArchiveTool);
		InObjectInfo.m_ObjectTypeInfo.ObjectType  = SvDef::SVToolObjectType;
		
		bool rc = SVObjectManagerClass::Instance().ConnectObjectInput( m_svObjectReference.Guid(), &InObjectInfo );

		ASSERT(rc );
	}
}

void SVArchiveRecord::DisconnectInputObject()
{
	if( SV_GUID_NULL != m_svObjectReference.Guid() )
	{
		SVInObjectInfoStruct InObjectInfo;
		
		InObjectInfo.SetObject(m_pArchiveTool);
		InObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVToolObjectType;
		
		SVObjectManagerClass::Instance().DisconnectObjectInput(m_svObjectReference.Guid(), &InObjectInfo);
	}
}

HRESULT SVArchiveRecord::QueueImage( SVMatroxBuffer& buf, const std::string& rFileName )
{
	ASSERT( !buf.empty() );
	HRESULT Result = S_OK;

	if ( m_eArchiveMethod == SVArchiveAsynchronous )
	{
		// the QueueImage function will copy the buffer, so pass in the original here
		SVArchiveImageThreadClass::BufferInfo info( buf, rFileName, m_lImageSize, m_ImageInfo, this );
		TheSVArchiveImageThreadClass().QueueImage( info );
	}
	else
	{
		if(nullptr != m_ImageBufferPtr)
		{
			m_ImageBufferPtr->SetCurrentIndex( m_LastIndexHandle );
			SVImageBufferHandlePtr handle;
			m_ImageBufferPtr->GetImageHandle( m_LastIndexHandle.GetIndex(), handle );

			if(nullptr != handle)
			{
				SVImageBufferHandleImage l_MilBuffer;
				handle->GetData( l_MilBuffer );

				if( !( l_MilBuffer.empty() ) )
				{
					Result = SVMatroxBufferInterface::CopyBuffer(l_MilBuffer.GetBuffer(), buf );
					// SafeImageCopyToHandle	// no... this happens in the inspection, therefore not necessary
													// No, because we remove all previous duplicate filenames in the Queue function
													// therefore they are never written out
				}
				else
				{
					Result = E_FAIL;
				}
			}
			else
			{
				Result = E_FAIL;
			}
		}
		else
		{
			Result = E_FAIL;
		}
	}

	return Result;
}

// right now called if method == SVArchiveGoOffline or SVArchiveAsynchronous
HRESULT SVArchiveRecord::AllocateBuffers( long lBufferSize )
{
	HRESULT hr = S_FALSE;

	m_lMaxIndex = 0;
	SVImageClass* pImage = dynamic_cast <SVImageClass*> (m_svObjectReference.getObject());
	ASSERT( pImage );
	if ( pImage )
	{
		SVImageInfoClass info = pImage->GetImageInfo();
		info.setDibBufferFlag(false);
		m_ImageInfo = info;

		_bstr_t bName( _T( "Archive Record Image Pool" ) );

		TheSVDataManager.CreateManagedIndexArray( m_pImageCircleBuffer, bName.Detach(), lBufferSize + 1 );

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
			if(nullptr != m_ImageBufferPtr)
			{
				m_ImageBufferPtr->SetImageInfo( info );
				m_ImageBufferPtr->resize( lBufferSize+1 );

				SvStl::MessageContainerVector errorMessages;
				m_ImageBufferPtr->ResetObject(&errorMessages);
				if (!errorMessages.empty())
				{
					hr = errorMessages[0].getMessage().m_MessageCode;
				}
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

	if( nullptr != m_ImageBufferPtr)
	{
		long lCount = m_lMaxIndex;
		for ( long l = 1; l <= lCount; l++ )
		{
			SVImageBufferHandlePtr handle;
			m_ImageBufferPtr->GetImageHandle( m_aFileNames[l].second.GetIndex(), handle );

			if( nullptr != handle)
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

	SVImageClass* pImage = dynamic_cast <SVImageClass*> (m_svObjectReference.getObject());
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
		SVImageBufferHandlePtr ImageHandle;
		bOk = pImage->GetImageHandle( ImageHandle ) && (nullptr != ImageHandle);
		if ( bOk )
		{
			HRESULT l_Code;

			SVImageBufferHandleImage l_MilBuffer;
			ImageHandle->GetData( l_MilBuffer );

			SVMatroxBuffer milBuffer = l_MilBuffer.GetBuffer();
			std::string ImageFile;

			//
			// Write the MIL image to a file in BMP form.
			//
			hr = BuildArchiveImageFilePath( ImageFile );
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

						l_Code = SVMatroxBufferInterface::Export(milBuffer, 
							ImageFile, 
							SVFileBitmap );

					}
					catch (CException& )
					{
						hr = E_FAIL;
					}
					catch (...)
					{
						hr = E_FAIL;
					}
				}
				else	// SVArchiveGoOffline or SVArchiveAsynchronous
				{
					QueueImage( milBuffer, ImageFile );
				}
			}
		}
	}


	return hr;
}
/*static*/HRESULT SVArchiveRecord::WriteImage( SVMatroxBuffer& milBuffer, const std::string& rFileName )
{
	HRESULT Result = S_OK;
	
	if ( !milBuffer.empty() )
	{
		try
		{
			Result = SVMatroxBufferInterface::Export( milBuffer, rFileName, SVFileBitmap);

		}
		catch (CException& )
		{
			Result = E_FAIL;
		}
		catch (...)
		{
			Result = E_FAIL;
		}
	}// end if ( bOk )

	return Result;
}

void SVArchiveRecord::Init( SVArchiveTool* pTool )
{
	m_pArchiveTool = pTool;
	m_eArchiveMethod = pTool->m_eArchiveMethod;
}


SVImageClass* SVArchiveRecord::GetImage()
{
	SVImageClass* l_psvImage = nullptr;

	if( nullptr != m_svObjectReference.getObject() )
	{
		GUID guid = m_svObjectReference.Guid();
	
		l_psvImage = dynamic_cast<SVImageClass*>( SVObjectManagerClass::Instance().GetObject(guid) );
	}

	return l_psvImage;
}
#pragma endregion Public Methods