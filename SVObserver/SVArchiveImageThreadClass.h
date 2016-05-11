//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once

#pragma region Inlcudes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <vector>
#include "SVImageClass.h"
#include "SVImageListClass.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "SVMemoryManager.h"
#pragma endregion Includes

class SVArchiveTool;
class SVArchiveRecord;

class SVArchiveImageThreadClass
{
public:
	struct BufferInfo
	{
		// passed into the thread class
		SVMatroxBuffer id;
		CString strFilename;
		long lBufferSize;
		SVImageInfoClass info;
		SVArchiveRecord* pRecord;

		// maintained by the thread class
		SVClock::SVTimeStamp m_Timestamp;
		SVImageObjectClassPtr pImageObject;

		BufferInfo() : lBufferSize(0), m_Timestamp(0), pRecord(nullptr), pImageObject(nullptr) {}
		BufferInfo( const BufferInfo& rhs )
			: id(rhs.id), strFilename(rhs.strFilename), lBufferSize(rhs.lBufferSize), m_Timestamp(rhs.m_Timestamp), info(rhs.info), pImageObject(rhs.pImageObject), pRecord(rhs.pRecord) {}
		BufferInfo( SVMatroxBuffer p_id, CString p_strFilename, long p_lBufferSize, SVImageInfoClass p_info, SVArchiveRecord* p_pRecord )
			: id(p_id), strFilename(p_strFilename), lBufferSize(p_lBufferSize), info(p_info), pRecord(p_pRecord) {}
		const BufferInfo& operator = ( const BufferInfo& rhs )
			{ if ( this != &rhs ) {id = rhs.id; strFilename = rhs.strFilename; lBufferSize = rhs.lBufferSize; m_Timestamp = rhs.m_Timestamp; info = rhs.info; pImageObject = rhs.pImageObject; pRecord = rhs.pRecord; } return *this; }
	};

#pragma region Constructor
	SVArchiveImageThreadClass();
	virtual ~SVArchiveImageThreadClass();
#pragma endregion Constructor

#pragma region Public Methods
	HRESULT GoOnline();
	HRESULT GoOffline();
	HRESULT QueueImage( BufferInfo info );
#pragma endregion Public Methods

private:
#pragma region Private Methods
	static DWORD WINAPI    ThreadEntry( LPVOID lpParam );
	DWORD ThreadFunction( );
	HRESULT PopAndWrite();
#pragma endregion Private Methods

#pragma region Private Members
	HANDLE            m_hThread;
	DWORD             m_dwThreadId;
	CCriticalSection  m_mtxQueue;	// not needed if queue is done in APC
	volatile bool     m_bRunThread;

	typedef std::deque<BufferInfo> QueueType;
	QueueType m_Queue;
	volatile HANDLE    m_hExitEvent;

	typedef std::pair<SVArchiveImageThreadClass*, BufferInfo> APCDataType;
#pragma endregion Private Members
};

typedef TBasicSingletonHolder< SVArchiveImageThreadClass > SVArchiveImageThreadClassSingleton;
inline SVArchiveImageThreadClass& TheSVArchiveImageThreadClass() {return SVArchiveImageThreadClassSingleton::Instance();}
