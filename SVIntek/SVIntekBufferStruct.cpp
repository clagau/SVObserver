// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekBufferStruct
// * .File Name       : $Workfile:   SVIntekBufferStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:50:58  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIntekBufferStruct.h"
#include "SVImageLibrary/SVImageEnums.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
This constructor initializes all local attributes to desired defaults.
*/
SVIntekBufferStruct::SVIntekBufferStruct()
{
	m_ulWidth = 640;
	m_ulHeight = 480;

	m_iFormat = SVImageFormatUnknown;

	m_pBitmapInfo = NULL;
  m_pucImage = NULL;

	m_bIsLocked = false;

	m_CameraIndex = -1;

	m_StartTimeStamp = 0;
	m_EndTimeStamp = 0;
}

/*
This copy constructor uses the operator= method to copy the attributes from the provided object into the current one.
*/
SVIntekBufferStruct::SVIntekBufferStruct( const SVIntekBufferStruct &p_rValue )
{
	*this = p_rValue;
}

/*
This destructor is a placeholder and performs no actions at this time.
*/
SVIntekBufferStruct::~SVIntekBufferStruct()
{
}

/*
This operator copies all of the internal attributes from the provided object into the current one.
*/
const SVIntekBufferStruct &SVIntekBufferStruct::operator=( const SVIntekBufferStruct &p_rValue )
{
	m_ulWidth = p_rValue.m_ulWidth;
	m_ulHeight = p_rValue.m_ulHeight;

	m_iFormat = p_rValue.m_iFormat;

	m_pBitmapInfo = p_rValue.m_pBitmapInfo;
  m_pucImage = p_rValue.m_pucImage;

	m_bIsLocked = p_rValue.m_bIsLocked;

	m_CameraIndex = p_rValue.m_CameraIndex;

	m_StartTimeStamp = p_rValue.m_StartTimeStamp;
	m_EndTimeStamp = p_rValue.m_EndTimeStamp;

	return *this;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIntek\SVIntekBufferStruct.cpp_v  $
 * 
 *    Rev 1.0   26 Apr 2013 14:50:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Feb 2010 10:23:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2009 12:14:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jul 2009 10:00:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include the new buffer acquisition methodology and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Nov 2005 12:29:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial code to implement Intek 1394 driver for SVObserver and test applications.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
