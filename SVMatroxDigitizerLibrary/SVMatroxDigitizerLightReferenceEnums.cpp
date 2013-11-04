//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerLightReferenceEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerLightReferenceEnums.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:29:22  $
//******************************************************************************
#include "stdafx.h"
#include "SVMatroxDigitizerLightReferenceEnums.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <mil.h>

// Assign mappings for Light Reference Types
SVMatroxDigitizerLightReference::SVLightReferenceEnumMap SVMatroxDigitizerLightReference::m_LightReferenceEnumConvertor = boost::assign::map_list_of<>
(SVMatroxDigitizerLightReference::SVBrightness,		M_BRIGHTNESS_REF)
(SVMatroxDigitizerLightReference::SVContrast,		M_CONTRAST_REF)
(SVMatroxDigitizerLightReference::SVWhiteBalanceU,	M_WHITE_BALANCE_U)
(SVMatroxDigitizerLightReference::SVWhiteBalanceV,	M_WHITE_BALANCE_V)
(SVMatroxDigitizerLightReference::SVSaturation,		M_SATURATION_REF)
(SVMatroxDigitizerLightReference::SVHue,			M_HUE_REF);

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxDigitizerLibrary\SVMatroxDigitizerLightReferenceEnums.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:29:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Jul 2009 16:13:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:32:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
