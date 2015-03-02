// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekCameraStruct
// * .File Name       : $Workfile:   SVIntekCameraStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:52:00  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIntekCameraStruct.h"

#include "SVHBitmapUtilitiesLibrary\SVImageFormatEnum.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
This constructor initializes all local attributes to desired defaults.
*/
SVIntekCameraStruct::SVIntekCameraStruct()
: m_pCamera(NULL) 
, m_pBufferInterface(NULL)
, m_StartFrameTimeStamp(0)
, m_ulWidth(0)
, m_ulHeight(0)
, m_ucVideoFormat(0)
, m_ucVideoMode(0)
, m_ucFrameRate(0)
, m_ucColorFormat(0)
, m_usXPos(0)
, m_usYPos(0)
, m_usPktSize(0)
, m_iFormat(SVImageFormatUnknown)
, m_lIsStarted(0)
, m_ulRegisterAddress(0xF0F00800)
, m_DCamTriggerMode(TM_ONESHOT)
, m_ulSoftwareTriggerRegisterAddress(0x62C)
, m_ulSoftwareTriggerValue(0x80000000)
, m_NextCopyBuffer(0)
, m_NextProcessBuffer(0)
, mhThread(NULL)
, m_bAcquisitionTriggered(false)
, m_lineState(false)
{
	m_svCameraId.m_ui64CameraId = 0;
	mhShutdown = CreateEvent( NULL, TRUE, FALSE, NULL );
}

/*
This copy constructor uses the operator= method to copy the attributes from the provided object into the current one.
*/
SVIntekCameraStruct::SVIntekCameraStruct( const SVIntekCameraStruct &p_rValue)
{
	*this = p_rValue;

	mhShutdown = CreateEvent( NULL, TRUE, FALSE, NULL );

	mhThread = NULL;
}

/*
This destructor is a placeholder and performs no actions at this time.
*/
SVIntekCameraStruct::~SVIntekCameraStruct()
{
	if( mhShutdown != NULL )
	{
		::SetEvent( mhShutdown );
		::CloseHandle( mhShutdown );
		mhShutdown = NULL;
	}

	if( mhThread != NULL )
	{
		::CloseHandle( mhThread );
		mhThread = NULL;
	}
}

/*
This operator copies all of the internal attributes from the provided object into the current one.
*/
const SVIntekCameraStruct &SVIntekCameraStruct::operator=( const SVIntekCameraStruct &p_rValue )
{
	m_pCamera = p_rValue.m_pCamera;

	m_pBufferInterface = p_rValue.m_pBufferInterface;
	m_StartFrameTimeStamp = p_rValue.m_StartFrameTimeStamp;

	m_csName = (LPCTSTR)p_rValue.m_csName;
	m_csFullName = (LPCTSTR)p_rValue.m_csFullName;

	m_ulWidth = p_rValue.m_ulWidth;
	m_ulHeight = p_rValue.m_ulHeight;

	m_ucVideoFormat = p_rValue.m_ucVideoFormat;
	m_ucVideoMode = p_rValue.m_ucVideoMode;
	m_ucFrameRate = p_rValue.m_ucFrameRate;
	m_ucColorFormat = p_rValue.m_ucColorFormat;
	m_usXPos = p_rValue.m_usXPos;
	m_usYPos = p_rValue.m_usYPos;
	m_usPktSize = p_rValue.m_usPktSize;

	m_iFormat = p_rValue.m_iFormat;

	m_lIsStarted = p_rValue.m_lIsStarted;

	m_NextCopyBuffer = p_rValue.m_NextCopyBuffer;
	m_NextProcessBuffer = p_rValue.m_NextProcessBuffer;

	m_caBuffers.RemoveAll();
	m_caBuffers.Copy( p_rValue.m_caBuffers );

	m_ulRegisterAddress = p_rValue.m_ulRegisterAddress;

	m_svCameraId = p_rValue.m_svCameraId;

	m_DCamTriggerMode = p_rValue.m_DCamTriggerMode;
	m_ulSoftwareTriggerRegisterAddress = p_rValue.m_ulSoftwareTriggerRegisterAddress;
	m_ulSoftwareTriggerValue = p_rValue.m_ulSoftwareTriggerValue;

	m_bAcquisitionTriggered = p_rValue.m_bAcquisitionTriggered;
	m_lineState = p_rValue.m_lineState;

	return *this;
}

/*
This method is used to extract the vender identification number from the Camera Identification attribute.
*/
unsigned __int64 SVIntekCameraStruct::GetVenderID()
{
	unsigned __int64 l_ui64VenderId = m_svCameraId.m_ui64CameraId >> 40;

	return l_ui64VenderId;
}

/*
This method is used to extract the serial number from the Camera Identification attribute.
*/
unsigned __int64 SVIntekCameraStruct::GetSerialNumber()
{
	unsigned __int64 l_ui64SerialNumber = m_svCameraId.m_ui64CameraId & 0x000000FFFFFFFFFF;

	return l_ui64SerialNumber;
}

bool SVIntekCameraStruct::IsAcquisitionTriggered() const
{
	return m_bAcquisitionTriggered;
}

void SVIntekCameraStruct::SetAcquisitionTriggered(bool bAcquisitionTriggered)
{
	m_bAcquisitionTriggered = bAcquisitionTriggered;
}

bool SVIntekCameraStruct::GetLineState() const
{
	return m_lineState;
}

void SVIntekCameraStruct::SetLineState(bool bState)
{
	m_lineState = bState;
}

const SVCallbackStruct& SVIntekCameraStruct::GetTriggerCallback() const
{
	return m_triggerCallback;
}

void SVIntekCameraStruct::SetTriggerCallback(SVCallbackStruct& callback)
{
	m_triggerCallback = callback;
}

void SVIntekCameraStruct::ClearTriggerCallback()
{
	m_triggerCallback.m_pCallback = NULL;
	m_triggerCallback.m_pOwner = NULL;
	m_triggerCallback.m_pData = NULL;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIntek\SVIntekCameraStruct.cpp_v  $
 * 
 *    Rev 1.0   26 Apr 2013 14:52:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   21 Jan 2013 09:50:54   sjones
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
 *    Rev 1.8   15 Dec 2010 09:42:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix a copy construction initialization bug.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Aug 2010 10:44:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  699
 * SCR Title:  Fix problem with AVT Guppy and Software Triggers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to specify DCAM software trigger or DCAM one shot functionality for software trigger.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   15 Feb 2010 10:23:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Dec 2009 12:14:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Oct 2009 10:35:10   sjones
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
 *    Rev 1.2   28 Nov 2006 17:41:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed references to SVDriverPropertyStructStdMap
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
