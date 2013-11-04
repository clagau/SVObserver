// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekCameraStruct
// * .File Name       : $Workfile:   SVIntekCameraStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 14:55:56  $
// ******************************************************************************

#ifndef SVINTEKCAMERASTRUCT_H
#define SVINTEKCAMERASTRUCT_H

#include "SVContainerLibrary/SVVector.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVCCamera.h"
#include "SVIntekTimeStruct.h"
#include "SVIntekExports.h"
#include "SVIntekBufferStruct.h"

#include <map>

class SVAcquisitionBufferInterface;

/*
This union is defined to describe the data in the Intek header.
*/
union SVIntekCameraIdUnion
{
	//This attribute holds the identification number for the camera.
	unsigned __int64 m_ui64CameraId;

	struct
	{
		//This attribute holds the first part of the identification number for a camera.
		unsigned long m_ul040Ch;
		//This attribute holds the second part of the identification number for a camera.
		unsigned long m_ul0410h;
	};
};

/*
This class is the data class for each camera available for use.
*/
struct SVIntekCameraStruct  
{
public:
	SVIntekCameraStruct();
	SVIntekCameraStruct( const SVIntekCameraStruct &p_rValue );
	virtual ~SVIntekCameraStruct();

	const SVIntekCameraStruct &operator=( const SVIntekCameraStruct &p_rValue );

	unsigned __int64 GetVenderID();
	unsigned __int64 GetSerialNumber();

	bool IsAcquisitionTriggered() const;
	void SetAcquisitionTriggered(bool bAcquisitionTriggered);
	void SetLineState(bool bState);
	bool GetLineState() const;

	const SVCallbackStruct& GetTriggerCallback() const;
	void SetTriggerCallback(SVCallbackStruct& callback);
	void ClearTriggerCallback();

	//This attribute holds the pointer to the Intek camera class.
	CCamera *m_pCamera;

	// This attribute provides the interface to the buffer interface of the host application.
	SVAcquisitionBufferInterface* m_pBufferInterface;

	// This attribute holds the time stamp in milliseconds used to collate the acquisition buffer to the correct product data.
	SVClock::SVTimeStamp m_StartFrameTimeStamp;

	//This attribute holds the pointer to the digitizer name for this camera.
	CString m_csName;
	//This attribute holds the pointer to the full digitizer name for this camera.
	CString m_csFullName;

	//This attribute holds the current width of the image in pixels.
	unsigned long m_ulWidth;
	//This attribute holds the current height of the image in pixels.
	unsigned long m_ulHeight;

	//This attribute holds the current video format for the camera.
	unsigned char m_ucVideoFormat;
	//This attribute holds the current video mode for the camera.
	unsigned char m_ucVideoMode;
	//This attribute holds the current frame rate for the camera.
	unsigned char m_ucFrameRate;
	//This attribute holds the current color format used in setting the Format 7 parameters.
	unsigned char m_ucColorFormat;
	//This attribute holds the current horizontal(x) offset used in setting the Format 7 parameters.
	unsigned short m_usXPos;
	//This attribute holds the current vertical(y) offset used in setting the Format 7 parameters.
	unsigned short m_usYPos;
	//This attribute holds the current packet size used in setting the Format 7 parameters.
	unsigned short m_usPktSize;

	//This attribute holds the current format of the image.
	int m_iFormat;

	//This attribute holds the active state of the digitizer.
	long m_lIsStarted;

	//This attribute holds the index to the next copy buffer.
	long m_NextCopyBuffer;
	//This attribute holds the index to the next process buffer.
	long m_NextProcessBuffer;

	//This attribute contains the acquisition buffers associated with this digitizer.
	SVIntekBufferStructCArray m_caBuffers;

	//This attribute holds the current register address for getting and settting custom parameters.
	unsigned long m_ulRegisterAddress;

	//This attribute holds the connected camera information.
	SVIntekCameraIdUnion m_svCameraId;

	//This attribute holds the handle to the acquisition processing thread shutdown event.
	HANDLE mhShutdown;
	//This attribute holds the handle to the acquistion processing thread.
	HANDLE mhThread;

	//This attribute holds the software trigger methodology identifier.
	unsigned long m_DCamTriggerMode;
	//This attribute holds the IEEE1394 register address the software trigger.
	unsigned long m_ulSoftwareTriggerRegisterAddress;
	//This attribute holds the value required to activate the software trigger.
	unsigned long m_ulSoftwareTriggerValue;

	bool m_bAcquisitionTriggered;
	bool m_lineState; // if we can get this value...

	SVCallbackStruct m_triggerCallback;
};

/*
This class is the data class for each camera available for use.
*/
typedef SVVector< SVIntekCameraStruct > SVIntekCameraStructCArray;

#endif // _SVINTEKCAMERASTRUCT_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIntek\SVIntekCameraStruct.h_v  $
 * 
 *    Rev 1.1   08 May 2013 14:55:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   29 Apr 2013 12:49:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Apr 2013 14:52:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   21 Jan 2013 09:50:54   sjones
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
 *    Rev 1.7   09 Nov 2010 15:44:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Aug 2010 10:44:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  699
 * SCR Title:  Fix problem with AVT Guppy and Software Triggers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to specify DCAM software trigger or DCAM one shot functionality for software trigger.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Feb 2010 10:23:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Oct 2009 10:35:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  674
 * SCR Title:  Incorrect Strobe timing when using Software Trigger
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added member variables for Software Trigger Register address and value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   14 Jul 2009 10:00:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include the new buffer acquisition methodology and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Nov 2006 17:40:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed refereneces to SVDriverProperryStruct and SVDriverPropertyStructStdMap
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Mar 2006 14:31:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  550
 * SCR Title:  Fix Format7 problem when changing width of image using the Intek driver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add format7 camera parameters and use camera parameters to finalize acquisition definition information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Nov 2005 13:32:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial code to implement Intek 1394 driver for SVObserver and test applications.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
