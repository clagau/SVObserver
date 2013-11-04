// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVProcessFunctor
// * .File Name       : $Workfile:   SVProcessFunctor.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 12:22:22  $
// ******************************************************************************

#ifndef SVPROCESSFUNCTOR_H
#define SVPROCESSFUNCTOR_H

template< typename SVT_CLASS >
class SVProcessFunctor
{
public:
	SVProcessFunctor();
	SVProcessFunctor( SVT_CLASS* p_pObject );
	SVProcessFunctor( const SVProcessFunctor& p_rObject );

	const SVProcessFunctor& operator=( const SVProcessFunctor& p_rObject );

	void operator()( bool& p_WaitForEvents );

protected:
	SVT_CLASS* m_pObject;

};

#include "SVProcessFunctor.inl"

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVProcessFunctor.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 12:22:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Jan 2010 14:55:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Aug 2009 16:43:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated new threading methodology to remove crash when closing a configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
