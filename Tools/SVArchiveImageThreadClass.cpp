//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by K�rber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved 
/// \file SVArchiveImageThreadClass.cpp
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVArchiveImageThreadClass.h"
#include "SVArchiveRecord.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/SVClock.h"
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
SVArchiveImageThreadClass& SVArchiveImageThreadClass::Instance()
{
	static SVArchiveImageThreadClass object;

	return object;
}

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
		Log_Assert( nullptr == m_hExitEvent );
		m_hExitEvent = ::CreateEvent( nullptr, true, false, nullptr );
		Log_Assert( nullptr == m_hThread );

		m_hThread = ::CreateThread( nullptr, 0, SVArchiveImageThreadClass::ThreadEntry, this, 0, &m_dwThreadId );

		::SetThreadPriority( m_hThread, THREAD_PRIORITY_LOWEST );
	}
	return S_OK;
}

HRESULT SVArchiveImageThreadClass::GoOffline()
{
	if ( m_bRunThread )
	{
		m_bRunThread = false;
		Log_Assert( nullptr != m_hExitEvent );
		::SetEvent( m_hExitEvent );
		::CloseHandle( m_hAppThread );
		::CloseHandle( m_hThread );	// can be done on a live thread
		m_dwThreadId = 0;
		m_hAppThread = nullptr;
		m_hThread = nullptr;
	}
	return S_OK;
}

// executes in the inspection thread 
long SVArchiveImageThreadClass::QueueImage( BufferInfo p_BufferInfo )
{
	std::lock_guard<std::mutex> lock(m_mtxQueue);

	auto iter = std::find_if(m_Queue.begin(), m_Queue.end(), [&p_BufferInfo](const auto& rInfo) { return (rInfo.m_FileName == p_BufferInfo.m_FileName); });
	if (iter != m_Queue.end())	// found filename
	{
		// must do the copy with the queue locked
		if (nullptr != p_BufferInfo.m_pImageBuffer && !p_BufferInfo.m_pImageBuffer->isEmpty())
		{
			iter->m_pImageBuffer = p_BufferInfo.m_pImageBuffer;
			iter->m_Timestamp = SvUl::GetTimeStamp();
		}
	}// end if ( iter != m_Queue.end() )	// found filename
	else
	{
		if (m_maxImageBufferCount[p_BufferInfo.m_toolPos] > m_currentBufferNumber[p_BufferInfo.m_toolPos])
		{
			// ** ADD NEW BUFFER TO QUEUE **
			p_BufferInfo.m_Timestamp = SvUl::GetTimeStamp();
			m_Queue.push_back(p_BufferInfo);
			++m_currentBufferNumber[p_BufferInfo.m_toolPos];
		}
		else
		{
			SvDef::StringVector msgList;
			msgList.push_back(p_BufferInfo.m_FileName);
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_ArchiveQueueFull, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}// end if not found filename in queue else

	//return current buffer number for the current Archive tool here, not m_Queue.size(): m_Queue contains image buffers for all Archive Tools
	return static_cast<long>(m_currentBufferNumber[p_BufferInfo.m_toolPos]); 
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
	Log_Assert( nullptr != m_hExitEvent );
	ahObjects[0] = m_hAppThread;
	ahObjects[1] = m_hExitEvent;

	const DWORD dwTimeoutMilliseconds = 10;
	DWORD dwWait;
	while ( (dwWait = ::WaitForMultipleObjects( 2, &(ahObjects[0]), FALSE, dwTimeoutMilliseconds ) ) == WAIT_TIMEOUT )
	{
		while ( m_Queue.size() > 0)
		{
			PopAndWrite();
			std::this_thread::yield();
		}
	}

	::CloseHandle( m_hExitEvent );
	m_hExitEvent = nullptr;

	// finish writing buffers
	while ( m_Queue.size() > 0 )
	{
		PopAndWrite();
		::Sleep(1);
	}

	return 0;
}

void SVArchiveImageThreadClass::PopAndWrite()
{
	BufferInfo info;
	{// begin lock scope block
		std::lock_guard<std::mutex> lock(m_mtxQueue);

		if ( m_Queue.size() > 0 )	// recheck size while within the lock
		{
			info = m_Queue.front();
			m_Queue.pop_front();
		}
	}// end lock scope block

	if (nullptr != info.m_pImageBuffer && !info.m_pImageBuffer->isEmpty())
	{
		if (false == info.m_ImageDirectoryPath.empty())
		{
			std::filesystem::create_directories(info.m_ImageDirectoryPath);
			//@TODO[MEC][10.20][14.03.2022] possible optimisation is to save information if path exist
		}
		const SVMatroxBuffer& buf = info.m_pImageBuffer->getHandle()->GetBuffer();
		SVArchiveRecord::WriteImageToFile(buf, info.m_FileName, info.m_format);
		{
			std::lock_guard<std::mutex> lock(m_mtxQueue);
			--m_currentBufferNumber[info.m_toolPos];
		}
	}
	else
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVO_73_ARCHIVE_MEMORY, SvStl::Tid_ArchiveImageNotFound, SvStl::SourceFileParams(StdMessageParams));
	}
}
#pragma endregion Private Methods

} //namespace SvTo
