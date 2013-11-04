//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerHookEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerHookEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:27:46  $
//******************************************************************************
#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include "SVMatroxDigitizerHookEnums.h"
#include "SVMatroxLibrary/SVMatroxImagingLibrary.h" // for SV_CURRENT_MIL_VERSION define

// Assign mappings for Hook Types
SVMatroxDigitizerHook::SVHookTypeEnumMap SVMatroxDigitizerHook::m_HookTypeEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerHook::SVCameraPresent,		M_CAMERA_PRESENT)
(SVMatroxDigitizerHook::SVFieldStart,			M_FIELD_START)
(SVMatroxDigitizerHook::SVFieldStartEven,		M_FIELD_START_EVEN)
(SVMatroxDigitizerHook::SVFieldStartOdd,		M_FIELD_START_ODD)
(SVMatroxDigitizerHook::SVFrameStart,			M_FRAME_START)
(SVMatroxDigitizerHook::SVGrabEnd,				M_GRAB_END)
(SVMatroxDigitizerHook::SVGrabFieldEnd,			M_GRAB_FIELD_END)
(SVMatroxDigitizerHook::SVGrabFieldEndEven,		M_GRAB_FIELD_END_EVEN)
(SVMatroxDigitizerHook::SVGrabFieldEndOdd,		M_GRAB_FIELD_END_ODD)
(SVMatroxDigitizerHook::SVGrabFrameEnd,			M_GRAB_FRAME_END)
(SVMatroxDigitizerHook::SVGrabFrameStart,		M_GRAB_FRAME_START)
(SVMatroxDigitizerHook::SVGrabStart,			M_GRAB_START)
(SVMatroxDigitizerHook::SVUserBitChange,		M_USER_BIT_CHANGE)
#if SV_CURRENT_MIL_VERSION == 0x0900
(SVMatroxDigitizerHook::SVGigeEvent,			M_GC_EVENT)
(SVMatroxDigitizerHook::SVLineRisingEdgeEvent,	M_LINE_RISING_EDGE)
(SVMatroxDigitizerHook::SVLineFallingEdgeEvent,	M_LINE_FALLING_EDGE)
(SVMatroxDigitizerHook::SVLineAnyEdgeEvent,		M_LINE_ANY_EDGE)
#endif
;
// Assign mappings for Hook Info Types
SVMatroxDigitizerHook::SVHookInfoEnumMap SVMatroxDigitizerHook::m_HookInfoEnumConvertor = boost::assign::map_list_of<SVMatroxDigitizerHook::SVHookInfoEnum,long>
(SVMatroxDigitizerHook::SVUserBit,			M_USER_BIT)
(SVMatroxDigitizerHook::SVUserBitState,		M_USER_BIT_STATE)
(SVMatroxDigitizerHook::SVGigeCameraTimeStamp,	M_GC_CAMERA_TIME_STAMP)
#if SV_CURRENT_MIL_VERSION == 0x0900
(SVMatroxDigitizerHook::SVGigeEventType,		M_GC_EVENT_TYPE)
#endif
;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxDigitizerLibrary\SVMatroxDigitizerHookEnums.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:27:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Aug 2012 15:36:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include statements to allow the new MIL 9.0 functionality to be available.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Jul 2012 12:27:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow gige to be compiled with both MIL 8.0 and 9.0.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Jun 2012 16:38:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Jan 2012 14:46:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated assignment definitions for matrox digitatizer features.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Jul 2009 16:12:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:30:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/