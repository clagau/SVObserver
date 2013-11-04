//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIMTypeInfoStruct
//* .File Name       : $Workfile:   SVIMTypeInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:58:58  $
//******************************************************************************

#ifndef SVIMTYPEINFOSTRUCT_H
#define SVIMTYPEINFOSTRUCT_H

struct SVIMTypeInfoStruct
{
	bool m_ColorSystem;
	bool m_Supported;
	short m_MaxDigitizers;
	short m_MaxTriggers;

	SVIMTypeInfoStruct( bool p_ColorSystem, bool p_Supported, short p_MaxDigitizers, short p_MaxTriggers )
	: m_ColorSystem( p_ColorSystem )
	, m_Supported( p_Supported )
	, m_MaxDigitizers( p_MaxDigitizers )
	, m_MaxTriggers( p_MaxTriggers)
	{
	}
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIMTypeInfoStruct.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:58:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Jan 2013 16:04:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Moved hardware tables and look-up fucntionality to the hardware manifest object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Aug 2012 08:30:44   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
