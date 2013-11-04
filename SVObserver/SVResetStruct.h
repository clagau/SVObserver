//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResetStruct
//* .File Name       : $Workfile:   SVResetStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:55:12  $
//******************************************************************************

#ifndef SVRESETSTRUCT_H
#define SVRESETSTRUCT_H

enum SVResetStateEnum : unsigned long
{
	SVResetStateEmpty                  = 0x00000000,
	SVResetStateAll                    = 0xFFFFFFFF,
	SVResetStateInitializeOnReset      = 0x00000001,
	SVResetStateArchiveToolCreateFiles = 0x00000002,
	SVResetStateLoadFiles              = 0x00000004,
	SVResetAutoMoveAndResize           = 0x00000008,
};

struct SVResetStruct
{
	unsigned long m_State;

	SVResetStruct() : m_State( SVResetStateEmpty ) {}

	bool IsStateSet( unsigned long p_State ) const { return ( ( m_State & p_State ) == p_State ); }

	void AddState( unsigned long p_State ) { m_State = ( m_State | p_State ); }
	void RemoveState( unsigned long p_State ) { m_State = ( m_State & ~( p_State ) ); }

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVResetStruct.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:55:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Aug 2012 17:07:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with automatic move and resize logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Dec 2010 15:13:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/