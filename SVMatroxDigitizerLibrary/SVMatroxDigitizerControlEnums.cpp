//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerControlEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerControlEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:45:26  $
//******************************************************************************
#include "stdafx.h"
#include "SVMatroxDigitizerControlEnums.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <mil.h>

// Assign mappings for Control Camera Types
SVMatroxDigitizerControl::SVCameraEnumMap SVMatroxDigitizerControl::m_CameraEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerControl::SVSourceOffsetX,			M_SOURCE_OFFSET_X)
(SVMatroxDigitizerControl::SVSourceOffsetY,			M_SOURCE_OFFSET_Y)
(SVMatroxDigitizerControl::SVSourceSizeX,			M_SOURCE_SIZE_X)
(SVMatroxDigitizerControl::SVSourceSizeY,			M_SOURCE_SIZE_Y);

// Assign mappings for Control UserBit Types
SVMatroxDigitizerControl::SVUserBitEnumMap SVMatroxDigitizerControl::m_UserBitEnumConvertor = boost::assign::map_list_of< SVMatroxDigitizerControl::SVUserBitEnum, SVMatroxIdentifier >
(SVMatroxDigitizerControl::SVUserBitInterruptMode,	M_USER_BIT_INTERRUPT_MODE)
(SVMatroxDigitizerControl::SVUserBitInterruptState,	M_USER_BIT_INTERRUPT_STATE)
(SVMatroxDigitizerControl::SVUserBitMode,			M_USER_BIT_MODE)
(SVMatroxDigitizerControl::SVUserBitValue,			M_USER_BIT_VALUE)
(SVMatroxDigitizerControl::SVUserBitSource,			M_USER_BIT_SOURCE);

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxDigitizerLibrary\SVMatroxDigitizerControlEnums.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 10:45:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 14:27:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
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
 *    Rev 1.0   29 Jul 2009 16:12:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:28:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
