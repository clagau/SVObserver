//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once

#pragma region Inlcudes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <vector>
#include "InspectionEngine/SVImageClass.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "SVLibrary\SVTemplate.h"

#pragma endregion Includes

class SVArchiveTool;
class SVArchiveRecord;

class SVArchiveImageThreadClass
{
public:
	struct BufferInfo
	{
		// passed into the thread class
		SvTrc::IImagePtr m_pImageBuffer;
		std::string m_FileName;
		long m_MaxNumberOfBuffer4Async;
		SVImageInfoClass info;
		SVArchiveRecord* pRecord;

		// maintained by the thread class
		SvTl::SVTimeStamp m_Timestamp;

		BufferInfo() : m_MaxNumberOfBuffer4Async(0), m_Timestamp(0), pRecord(nullptr) {}
		BufferInfo( const BufferInfo& rhs )
			: m_pImageBuffer(rhs.m_pImageBuffer), m_FileName(rhs.m_FileName), m_MaxNumberOfBuffer4Async(rhs.m_MaxNumberOfBuffer4Async), m_Timestamp(rhs.m_Timestamp), info(rhs.info), pRecord(rhs.pRecord) {}
		BufferInfo(SvTrc::IImagePtr pImageBuffer, const std::string& rFileName, long maxNumberOfBuffer4Async, SVImageInfoClass p_info, SVArchiveRecord* p_pRecord )
			: m_pImageBuffer(pImageBuffer), m_FileName(rFileName), m_MaxNumberOfBuffer4Async(maxNumberOfBuffer4Async), info(p_info), pRecord(p_pRecord) {}
		const BufferInfo& operator = ( const BufferInfo& rhs )
			{ if ( this != &rhs ) { m_pImageBuffer = rhs.m_pImageBuffer; m_FileName = rhs.m_FileName; m_MaxNumberOfBuffer4Async = rhs.m_MaxNumberOfBuffer4Async; m_Timestamp = rhs.m_Timestamp; info = rhs.info; pRecord = rhs.pRecord; } return *this; }
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
	HANDLE			  m_hAppThread;
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
