//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeEmptySetterFunc
//* .File Name       : $Workfile:   SVGigeEmptySetter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:42  $
//******************************************************************************
#ifndef SVGIGEEMPTYSETTER_H
#define SVGIGEEMPTYSETTER_H

#include <comdef.h>
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerRef.h"
#include "SVGigeFeature.h"

struct SVGigeEmptySetter
{
	// Setter
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeEmptySetter.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:54:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jul 2009 15:32:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:05:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

