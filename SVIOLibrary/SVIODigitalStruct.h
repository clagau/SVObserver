// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIODigitalStruct
// * .File Name       : $Workfile:   SVIODigitalStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:46:52  $
// ******************************************************************************

#ifndef _SVIODIGITALSTRUCT_H
#define _SVIODIGITALSTRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct SVIODigitalStruct  
{
	SVIODigitalStruct();
	SVIODigitalStruct( const SVIODigitalStruct &p_rOriginal );
	virtual ~SVIODigitalStruct();

	const SVIODigitalStruct &operator=( const SVIODigitalStruct &p_rOriginal );

	bool m_bIsInverted;
	bool m_bIsForced;
	bool m_bForcedValue;
	bool m_bValue;
};

#endif // _SVIODIGITALSTRUCT_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOLibrary\SVIODigitalStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:46:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 13:21:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added I/O objects to new I/O configuration interface library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/