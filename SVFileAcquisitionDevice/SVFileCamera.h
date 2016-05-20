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
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <iterator>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVFileCameraStruct.h"
#include "SVEventHandler.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFile.h"
#include "SVFileSystemLibrary/SVFileInfo.h"
#include "SVFileSystemLibrary/SVFileInfoComparator.h"
#include "TriggerHandling/SVCallbackStruct.h"
#include "SVSystemLibrary/SVSequencer.h"
#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVString.h"

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
	SVString m_name;
	SVFileList m_fileList;
	SVSequencer<FileListIterator> m_loadSequence;
	SVImageFile m_bitmap;
	SVAsyncProcedure<APCSignalHandler, ThreadSignalHandler> m_thread;
	SVFrameEventHandler m_startFrameEvent;
	SVFrameEventHandler m_endFrameEvent;
	bool m_lineState;

	static void CALLBACK OnAPCEvent( ULONG_PTR data );
	void OnThreadEvent( bool& p_WaitForEvents );

	SVString GetNextFilename();
	bool ValidImageFormatForCopy() const;

	HRESULT CopySameBitDepthImage(const unsigned char* pSrcBuf, unsigned char* pDstBuf, int bitDepth, const SIZE& imageSize, bool bInvert);
	HRESULT Copy24BitTo32BitImage(const unsigned char* pSrcBuf, unsigned char* pDstBuf, const SIZE& imageSize, bool bInvert);

public:
	long m_lIsStarted;
	bool m_bAcquisitionTriggered;

	SVFileCamera();

	const SVString& GetName() const;
	void SetName(const SVString& name);

	const SIZE& GetImageSize() const;
	void SetImageSize(const SIZE& imageSize);

	SVImageFormatEnum GetImageFormat() const;
	void SetImageFormat(SVImageFormatEnum imageFormat);
	
	// setup
	LPCTSTR GetDirectory() const;
	void SetDirectory(const SVString& dir);
	LPCTSTR GetFileName() const;
	void SetFileName(const SVString& fileName);

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

	const SvTh::SVCallbackStruct& GetTriggerCallback() const;
	void SetTriggerCallback(SvTh::SVCallbackStruct& callback);
	void ClearTriggerCallback();

	SVAcquisitionBufferInterface* m_pBufferInterface;

	SVClock::SVTimeStamp m_StartTimeStamp;
	SvTh::SVCallbackStruct m_triggerCallback;
};

typedef std::vector<SVFileCamera> SVFileCameraList;

