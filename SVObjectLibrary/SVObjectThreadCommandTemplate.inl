//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectThreadCommandTemplate
//* .File Name       : $Workfile:   SVObjectThreadCommandTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:01:14  $
//******************************************************************************

#ifndef SVOBJECTTHREADCOMMANDTEMPLATE_INL
#define SVOBJECTTHREADCOMMANDTEMPLATE_INL

#include "SVObjectThreadCommandTemplate.h"

template< typename SVCommandPtr >
SVObjectThreadCommandTemplate< SVCommandPtr >::SVObjectThreadCommandTemplate( const SVCommandPtr& p_rCommandPtr )
: SVCommandTemplate( 1 ), m_CommandPtr( p_rCommandPtr )
{
}

template< typename SVCommandPtr >
SVObjectThreadCommandTemplate< SVCommandPtr >::~SVObjectThreadCommandTemplate()
{
}

template< typename SVCommandPtr >
HRESULT SVObjectThreadCommandTemplate< SVCommandPtr >::Execute()
{
	HRESULT l_Status = S_OK;

	if( !( m_CommandPtr.empty() ) )
	{
		m_CommandPtr->Execute();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectThreadCommandTemplate.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 17:01:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Aug 2012 11:35:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/





