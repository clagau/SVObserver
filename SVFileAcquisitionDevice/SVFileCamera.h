//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileCamera
//* .File Name       : $Workfile:   SVFileCamera.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 06:14:50  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <iterator>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVFileCameraStruct.h"
#include "SVEventHandler.h"
#include "TriggerHandling/TriggerBasics.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFile.h"
#include "SVFileSystemLibrary/SVFileInfo.h"
#include "SVFileSystemLibrary/SVFileInfoComparator.h"

#include "SVSystemLibrary/SVSequencer.h"
#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "SVTimerLibrary/SVClock.h"

#pragma endregion Includes

class SVAcquisitionBufferInterface;

class SVFileCamera
{
private:
	typedef std::set<SVFileInfo, SVFileInfoComparator> SVFileList;
	typedef SVFileList::const_iterator FileListIterator;
	typedef std::insert_iterator<SVFileList> Insertor;

	typedef void ( CALLBACK *APCSignalHandler )( ULONG_PTR );
	typedef boost::function<void ( bool& )> ThreadSignalHandler;
	
public:
	typedef boost::function<HRESULT (unsigned long)> EventHandler;
	typedef SVEventHandler<EventHandler> SVFrameEventHandler;

private:
	unsigned long m_index;
	SVFileCameraStruct m_fileData;
	std::string m_name;
	SVFileList m_fileList;
	SVSequencer<FileListIterator> m_loadSequence;
	SVImageFile m_bitmap;
	SVAsyncProcedure<APCSignalHandler, ThreadSignalHandler> m_thread;
	SVFrameEventHandler m_startFrameEvent;
	SVFrameEventHandler m_endFrameEvent;
	bool m_lineState;

	static void CALLBACK OnAPCEvent( ULONG_PTR data );
	void OnThreadEvent( bool& p_WaitForEvents );

	std::string GetNextFilename();
	bool ValidImageFormatForCopy() const;

	HRESULT CopySameBitDepthImage(const unsigned char* pSrcBuf, unsigned char* pDstBuf, int bitDepth, const SIZE& imageSize, bool bInvert);
	HRESULT Copy24BitTo32BitImage(const unsigned char* pSrcBuf, unsigned char* pDstBuf, const SIZE& imageSize, bool bInvert);

public:
	long m_lIsStarted;
	bool m_bAcquisitionTriggered;

	SVFileCamera();

	const std::string& GetName() const;
	void SetName(const std::string& name);

	const SIZE& GetImageSize() const;
	void SetImageSize(const SIZE& imageSize);

	SvDef::SVImageFormatEnum GetImageFormat() const;
	void SetImageFormat(SvDef::SVImageFormatEnum imageFormat);
	
	// setup
	LPCTSTR GetDirectory() const;
	void SetDirectory(const std::string& dir);
	LPCTSTR GetFileName() const;
	void SetFileName(const std::string& fileName);

	void SetLoadingMode(SVFileAcquisitonLoadingModeEnum mode);
	SVFileAcquisitonLoadingModeEnum GetLoadingMode() const;
	bool IsSingleFileMode() const;
	bool IsContinuousLoadMode() const;
	bool IsSingleIterationLoadMode() const;
	
	HRESULT Start(const EventHandler& startFrameHandler, const EventHandler& endFrameHandler, unsigned long p_ulIndex);
	void Stop();

	bool IsRunning() const;

	HRESULT DoOneShot();

	HRESULT CopyImage(unsigned char* ulBuffer);

	bool IsAcquisitionTriggered() const;
	void SetAcquisitionTriggered(bool bAcquisitionTriggered);
	void SetLineState(bool bState);
	bool GetLineState() const;

	const SvTh::TriggerDispatcher& GetTriggerDispatcher() const;
	void SetTriggerDispatcher(const SvTh::TriggerDispatcher& rDispatcher);
	void ClearTriggerCallback();

	SVAcquisitionBufferInterface* m_pBufferInterface;

	SvTl::SVTimeStamp m_StartTimeStamp;
	SvTh::TriggerDispatcher m_dispatcher;
};

typedef std::vector<SVFileCamera> SVFileCameraList;

