//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once

#pragma region Inlcudes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <vector>
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVLibrary/SVTemplate.h"
#include "TriggerRecordController/IImage.h"
#pragma endregion Includes

namespace SvTo
{

class SVArchiveTool;
class SVArchiveRecord;

class SVArchiveImageThreadClass
{
public:
	struct BufferInfo
	{
		// passed into the thread class
		SvTrc::IImagePtr m_pImageBuffer = nullptr;
		std::string m_FileName;
		SVImageInfoClass info;
		SVArchiveRecord* pRecord = nullptr;

		// maintained by the thread class
		double m_Timestamp = 0;

		BufferInfo() = default;

		BufferInfo(const BufferInfo& rhs) = default;

		BufferInfo(SvTrc::IImagePtr pImageBuffer, const std::string& rFileName, SVImageInfoClass p_info, SVArchiveRecord* p_pRecord )
			: m_pImageBuffer(pImageBuffer), m_FileName(rFileName), info(p_info), pRecord(p_pRecord) {}

		BufferInfo& operator = (const BufferInfo& rhs) = default;
	};

#pragma region Constructor
	SVArchiveImageThreadClass();
	virtual ~SVArchiveImageThreadClass();
#pragma endregion Constructor

#pragma region Public Methods
	HRESULT GoOnline();
	HRESULT GoOffline();
	HRESULT QueueImage( BufferInfo info );
	void setMaxNumberOfBuffer(long maxNumber) { m_MaxNumberOfBuffer = maxNumber; };
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
	std::mutex		  m_mtxQueue;	// not needed if queue is done in APC
	volatile bool     m_bRunThread;

	typedef std::deque<BufferInfo> QueueType;
	QueueType m_Queue;
	volatile HANDLE    m_hExitEvent;
	long m_MaxNumberOfBuffer = 1;

	typedef std::pair<SVArchiveImageThreadClass*, BufferInfo> APCDataType;
#pragma endregion Private Members
};

typedef TBasicSingletonHolder< SVArchiveImageThreadClass > SVArchiveImageThreadClassSingleton;
inline SVArchiveImageThreadClass& TheSVArchiveImageThreadClass() {return SVArchiveImageThreadClassSingleton::Instance();}

} //namespace SvTo
