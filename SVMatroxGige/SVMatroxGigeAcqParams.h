//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeAcqParams
//* .File Name       : $Workfile:   SVMatroxGigeAcqParams.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:38:50  $
//******************************************************************************
#ifndef SVMATROXGIGEACQPARAMS_H
#define SVMATROXGIGEACQPARAMS_H

#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerGrabEnums.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"

namespace SVMatroxGigeTrigger
{
	enum TriggerSource
	{
		HardwareTrigger = 0,
		SoftwareTrigger
	};
}

struct SVMatroxGigeAcqParams
{
	unsigned long Width;		// X-Dimension
	unsigned long Height;		// Y-Dimension
	unsigned long Format;		// Actual ColorFormat (pixel depth ?)
	unsigned long XOffsetPos;	// X-Offset
	unsigned long YOffsetPos;	// Y-Offset
	UINT8 TriggerMode;			// Triggermode (Bulb mode ?)
	UINT8 BayerPattern;			// Used bayer pattern
	bool bAcquisitionTriggered;	//

	SVMatroxGigeTrigger::TriggerSource TriggerType;				// Trigger Source (Hardware/Software)
	SVMatroxDigitizerGrab::SVGrabTriggerModeEnum TriggerEdge;	// Trigger Edge(Rising/Falling)

	SVMatroxGigeAcqParams()
	: Width(640)
	, Height(480)
	, XOffsetPos(0)
	, YOffsetPos(0)
	, Format(SVImageFormatMono8)
	, TriggerMode(0)
	, BayerPattern(0)
	, TriggerType(SVMatroxGigeTrigger::HardwareTrigger)
	, TriggerEdge(SVMatroxDigitizerGrab::SVEdgeRising)
	, bAcquisitionTriggered(false)
	{}
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeAcqParams.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:38:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Jan 2013 09:59:12   sjones
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
 *    Rev 1.1   30 Jul 2009 07:50:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new buffer management and buffer transfer methodologies.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:18:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
