//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVArchiveImageThreadClass.h"
#include "SVArchiveTool.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMemoryManager.h"
#include "SVSystemLibrary/SVThreadManager.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#pragma region Constructor
SVArchiveImageThreadClass::SVArchiveImageThreadClass()
{
	m_hThread = nullptr;
	m_dwThreadId = 0;
	m_bRunThread = false;
	m_hExitEvent = nullptr;
}

SVArchiveImageThreadClass::~SVArchiveImageThreadClass()
{
	GoOffline();
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT SVArchiveImageThreadClass::GoOnline()
{
	if ( !m_bRunThread )
	{
		// clear queue
		m_Queue.clear();

		// create thread
		m_bRunThread = true;
		ASSERT( nullptr == m_hExitEvent );
		m_hExitEvent = ::CreateEvent( nullptr, true, false, nullptr );
		ASSERT( nullptr == m_hThread );

		m_hThread = ::CreateThread( nullptr, 0, SVArchiveImageThreadClass::ThreadEntry, this, 0, &m_dwThreadId );

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
		ASSERT( nullptr != m_hExitEvent );
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
			HRESULT hrAllocate = TheSVMemoryManager().ReservePoolMemory(SvO::ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME, this, p_BufferInfo.lBufferSize );
			if ( S_OK == hrAllocate )	// if enough memory in queue
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

				if ( S_OK == hr )
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
#pragma endregion Public Methods

#pragma region Private Methods
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
	ASSERT( nullptr != m_hExitEvent );
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
	m_hExitEvent = nullptr;

	// finish writing buffers
	while ( m_Queue.size() > 0 )
	{
		HRESULT hr = PopAndWrite();
		::Sleep(1);
	}


	m_hThread = nullptr;
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
			HRESULT hrAllocate = TheSVMemoryManager().ReleasePoolMemory(SvO::ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME, this, info.lBufferSize );
			lock.Unlock();
			SVArchiveRecord::WriteImage( info.id, info.strFilename );

			info.pImageObject.clear();
			//TheSVDataManager.ReleaseBufferIndex( info.lDMBuffer, info.lDMIndex, SV_ARCHIVE );
		}
	}// end lock scope block

	return S_OK;
}
#pragma endregion Private Methods