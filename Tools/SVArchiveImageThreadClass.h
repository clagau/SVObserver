//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved 
/// \file SVArchiveImageThreadClass.h
//*****************************************************************************

#pragma once

#pragma region Inlcudes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <vector>
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVLibrary/SVTemplate.h"
#include "ObjectInterfaces/ITRCImage.h"
#include "Tools/ArchiveMethodEnum.h"
#include "SVFileSystemLibrary/ImageFileFormats.h"

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
		SvOi::ITRCImagePtr m_pImageBuffer = nullptr;
		std::string m_FileName;
		ImageFileFormat m_format = ImageFileFormat::invalid;
		std::string m_ImageDirectoryPath;
		SVImageInfoClass info;
		int m_toolPos= -1;

		// maintained by the thread class
		double m_Timestamp = 0;

		BufferInfo() = default;

		BufferInfo(const BufferInfo& rhs) = default;

		BufferInfo(SvOi::ITRCImagePtr pImageBuffer, const std::string& rFileName, ImageFileFormat fileFormat, const std::string& rImageDirectoryPath, SVImageInfoClass p_info, int pos)
			: m_pImageBuffer(pImageBuffer), m_FileName(rFileName), m_format(fileFormat),m_ImageDirectoryPath(rImageDirectoryPath), info(p_info), m_toolPos(pos) {}

		BufferInfo& operator = (const BufferInfo& rhs) = default;
	};

#pragma region Constructor
	SVArchiveImageThreadClass();
	virtual ~SVArchiveImageThreadClass();
#pragma endregion Constructor

#pragma region Public Methods
	static SVArchiveImageThreadClass& Instance();

	HRESULT GoOnline();
	HRESULT GoOffline();
	long QueueImage( BufferInfo info ); // returns current queue length (-1 on error)
	void setMaxNumberOfBuffer(int toolPos, long maxNumber) { m_maxBufferNumber[toolPos] = maxNumber; };
#pragma endregion Public Methods

private:
#pragma region Private Methods
	static DWORD WINAPI    ThreadEntry( LPVOID lpParam );
	DWORD ThreadFunction( );
	void PopAndWrite();
#pragma endregion Private Methods

#pragma region Private Members
	HANDLE            m_hThread;
	HANDLE			  m_hAppThread;
	DWORD             m_dwThreadId;
	std::mutex		  m_mtxQueue;	// not needed if queue is done in APC
	volatile bool     m_bRunThread;

	std::deque<BufferInfo> m_Queue;
	volatile HANDLE    m_hExitEvent;
	std::unordered_map<int, int> m_maxBufferNumber; //@TODO[MEC][10.20][14.03.2022] max buffer number per tool psition differnt inspections?
	std::unordered_map<int, int> m_currentBufferNumber;
	long m_MaxNumberOfBuffer = 1;
#pragma endregion Private Members
};

} //namespace SvTo
