//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValidateReasons
//* .File Name       : $Workfile:   SVPLCValidateReasons.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:29:14  $
//******************************************************************************

#ifndef SVVALIDATEREASONS_H
#define SVVALIDATEREASONS_H

enum SVValidateReasons
{
	ALREADY_USED = -3133,
	OVERLAPPING = -3134,
	TOO_LARGE = -3135,
	BITS_OVERLAP = -3136,
	INPUT_ALREADY_USED = -3137,
	// Different Types
	DIFFERENT_TYPES = -3138,
	// Data is past the end of the data table.
	PAST_THE_END = -3139,
	// Data is overlapping.
	OVERLAPPING_DATA = -3140,
};

#endif // SVVALIDATEREASONS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCValidateReasons.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:29:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:10:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

