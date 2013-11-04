// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCallbackStruct
// * .File Name       : $Workfile:   SVCallbackStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:01:00  $
// ******************************************************************************

#ifndef _SVCALLBACKSTRUCT_H
#define _SVCALLBACKSTRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif

typedef HRESULT ( CALLBACK *SVCallbackPtr )( void *,   /* Owner */
                                             void * ); /* Data */ 

struct SVCallbackStruct  
{
	SVCallbackStruct();
	SVCallbackStruct( const SVCallbackStruct &p_rOriginal );
	virtual ~SVCallbackStruct();

	const SVCallbackStruct &operator=( const SVCallbackStruct &p_rOriginal );

	SVCallbackPtr m_pCallback;

	void *m_pOwner;
	void *m_pData;
};

bool operator==( const SVCallbackStruct &p_rLeft,
                 const SVCallbackStruct &p_rRight );

#endif // _SVCALLBACKSTRUCT_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVCallbackStruct.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:01:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2003 13:13:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322, 320
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   New files needed to handle callback functionality and parameter functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/