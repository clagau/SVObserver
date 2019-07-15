//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVArchiveImageThreadClass.h"
#include "SVArchiveRecord.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVSystemLibrary/SVThreadManager.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Constructor
SVArchiveImageThreadClass::SVArchiveImageThreadClass()
{
	m_hThread = nullptr;
	m_hAppThread = nullptr;
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
		HANDLE hProcess = ::GetCurrentProcess();
		DuplicateHandle( hProcess, ::GetCurrentThread(), hProcess, &m_hAppThread, 0, FALSE, DUPLICATE_SAME_ACCESS );
		assert( nullptr == m_hExitEvent );
		m_hExitEvent = ::CreateEvent( nullptr, true, false, nullptr );
		assert( nullptr == m_hThread );

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
		assert( nullptr != m_hExitEvent );
		::SetEvent( m_hExitEvent );
		::CloseHandle( m_hAppThread );
		::CloseHandle( m_hThread );	// can be done on a live thread
		SVThreadManager::Instance().Remove( m_hThread );
		m_dwThreadId = 0;
		m_hAppThread = nullptr;
		m_hThread = nullptr;
	}
	return S_OK;
}

// executes in the Inspection thread 
HRESULT SVArchiveImageThreadClass::QueueImage( BufferInfo p_BufferInfo )
{
	HRESULT Result = S_OK;

	// perform this code in the inspection thread, not the archive tool thread

	{// begin lock scope block
		
		std::lock_guard<std::mutex> lock(m_mtxQueue);

		QueueType::iterator iter;
		for ( iter = m_Queue.begin(); iter != m_Queue.end(); ++iter )
		{
			if (iter->m_FileName == p_BufferInfo.m_FileName)
			{
				break;
			}
		}

		if ( iter != m_Queue.end() )	// found filename
		{
			BufferInfo& rBufferInfo = *iter;
			// must do the copy with the queue locked
			if(nullptr != p_BufferInfo.m_pImageBuffer && !p_BufferInfo.m_pImageBuffer->isEmpty() )
			{
				rBufferInfo.m_pImageBuffer = p_BufferInfo.m_pImageBuffer;

				// update timestamp
				rBufferInfo.m_Timestamp = SvTl::GetTimeStamp();
			}
		}// end if ( iter != m_Queue.end() )	// found filename
		else
		{
			if (m_MaxNumberOfBuffer > m_Queue.size())
			{
				// ** ADD NEW BUFFER TO QUEUE **
				p_BufferInfo.m_Timestamp = SvTl::GetTimeStamp();
				m_Queue.push_back(p_BufferInfo);
			}
			else	// not enough room on queue
			{
				// find oldest entry from source AT
				double iOldest = SvTl::GetTimeStamp();
				QueueType::iterator iterOldest = m_Queue.end();
				for (iter = m_Queue.begin(); iter != m_Queue.end(); ++iter)
				{
					if (iter->pRecord == p_BufferInfo.pRecord && iter->m_Timestamp < iOldest)
					{
						iOldest = iter->m_Timestamp;
						iterOldest = iter;
					}
				}

				if (iterOldest != m_Queue.end())	// if found another entry by the same archive record
				{
					BufferInfo& rBufferInfo = *iterOldest;
					rBufferInfo.m_pImageBuffer = p_BufferInfo.m_pImageBuffer;

					// update timestamp
					rBufferInfo.m_Timestamp = SvTl::GetTimeStamp();
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
	assert( nullptr != m_hExitEvent );
	ahObjects[0] = m_hAppThread;
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
				/*HRESULT hr = */PopAndWrite();
			}
		}
	}

	::CloseHandle( m_hExitEvent );
	m_hExitEvent = nullptr;

	// finish writing buffers
	while ( m_Queue.size() > 0 )
	{
		/*HRESULT hr = */PopAndWrite();
		::Sleep(1);
	}

	return 0;
}

HRESULT SVArchiveImageThreadClass::PopAndWrite()
{
	{// begin lock scope block
		// lock not needed if pushing in APC (WHICH WE'RE NOT DOING)... 
		// lock is still needed because we are pushing in the inspection thread
		std::lock_guard<std::mutex> lock(m_mtxQueue);

		if ( m_Queue.size() > 0 )	// recheck size while within the lock
		{
			BufferInfo info = m_Queue.front();
			m_Queue.pop_front();
			SVMatroxBuffer buf;
			if (nullptr != info.m_pImageBuffer && !info.m_pImageBuffer->isEmpty())
			{
				buf = info.m_pImageBuffer->getHandle()->GetBuffer();
			}
			SVArchiveRecord::WriteImage(buf, info.m_FileName );

		}
	}// end lock scope block

	return S_OK;
}
#pragma endregion Private Methods

} //namespace SvTo
