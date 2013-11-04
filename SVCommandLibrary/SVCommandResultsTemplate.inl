//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandResultsTemplate
//* .File Name       : $Workfile:   SVCommandResultsTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:31:42  $
//******************************************************************************

#ifndef SVCOMMANDRESULTSTEMPLATE_INL
#define SVCOMMANDRESULTSTEMPLATE_INL

#include "SVCommandResultsTemplate.h"

template< typename SVResultCollection >
SVCommandResultsTemplate< SVResultCollection >::SVCommandResultsTemplate()
: m_Results()
{
}

template< typename SVResultCollection >
SVCommandResultsTemplate< SVResultCollection >::SVCommandResultsTemplate( const SVCommandResultsTemplate& p_rObject )
: m_Results( p_rObject.m_Results )
{
}

template< typename SVResultCollection >
SVCommandResultsTemplate< SVResultCollection >::~SVCommandResultsTemplate()
{
}

template< typename SVResultCollection >
const SVCommandResultsTemplate< SVResultCollection >& SVCommandResultsTemplate< SVResultCollection >::operator=( const SVCommandResultsTemplate& p_rObject )
{
	if( this != &m_Results )
	{
		m_Results = p_rObject.m_Results;
	}

	return *this;
}

template< typename SVResultCollection >
HRESULT SVCommandResultsTemplate< SVResultCollection >::GetResults( SVResultCollection& p_rResults ) const
{
	p_rResults = m_Results;

	return S_OK;
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandResultsTemplate.inl_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:31:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:32:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
