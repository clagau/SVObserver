//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerLightReferenceEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerLightReferenceEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:29:28  $
//******************************************************************************

#ifndef SVMATROXDIGITIZERLIGHTREFERENCEENUMS_H
#define SVMATROXDIGITIZERLIGHTREFERENCEENUMS_H

#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxEnumConvertor.h"


/**
@SVObjectName Matrox Digitizer Light Reference Enum

@SVObjectOverview This enum is used to get/set the Light Reference parameters of the Digitizer.

@SVObjectOperations None

*/
struct SVMatroxDigitizerLightReference
{
	enum SVLightReferenceEnum
	{
		SVBrightness,		// M_BRIGHTNESS_REF
		SVContrast,			// M_CONTRAST_REF
		SVWhiteBalanceU,	// M_WHITE_BALANCE_U
		SVWhiteBalanceV,	// M_WHITE_BALANCE_V
		SVSaturation,		// M_SATURATION_REF (Color only)
		SVHue,				// M_HUE_REF (Color only)
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVLightReferenceEnum, long> SVLightReferenceEnumMap;
	static SVLightReferenceEnumMap m_LightReferenceEnumConvertor;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxDigitizerLibrary\SVMatroxDigitizerLightReferenceEnums.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:29:28   bWalter
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
 *    Rev 1.0   02 Jun 2009 15:32:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/