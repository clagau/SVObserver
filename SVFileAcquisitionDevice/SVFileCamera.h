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
#ifndef SVFILECAMERA_H
#define SVFILECAMERA_H

#include <vector>
#include <set>
#include <iterator>
#include <boost/config.hpp>
#include <boost/function.hpp>
#include "SVFileCameraStruct.h"
#include "SVEventHandler.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFile.h"
#include "SVFileSystemLibrary/SVFileInfo.h"
#include "SVFileSystemLibrary/SVFileInfoComparator.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"
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

	const SVCallbackStruct& GetTriggerCallback() const;
	void SetTriggerCallback(SVCallbackStruct& callback);
	void ClearTriggerCallback();

	SVAcquisitionBufferInterface* m_pBufferInterface;

	SVClock::SVTimeStamp m_StartTimeStamp;
	SVCallbackStruct m_triggerCallback;
};

typedef std::vector<SVFileCamera> SVFileCameraList;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVFileAcquisitionDevice\SVFileCamera.h_v  $
 * 
 *    Rev 1.2   01 Oct 2013 06:14:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 20:04:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Apr 2013 12:30:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 09:28:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jan 2013 09:47:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for trigger interface
 * Revised to support Acquisition Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Jan 2010 14:52:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Sep 2009 08:45:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Jul 2009 15:00:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Dec 2008 15:23:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Loading modes for File Acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2008 07:58:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
