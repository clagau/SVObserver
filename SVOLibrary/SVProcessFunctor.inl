// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVProcessFunctor
// * .File Name       : $Workfile:   SVProcessFunctor.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 12:22:38  $
// ******************************************************************************

template< typename SVT_CLASS >
SVProcessFunctor< SVT_CLASS >::SVProcessFunctor()
: m_pObject( NULL )
{
}

template< typename SVT_CLASS >
SVProcessFunctor< SVT_CLASS >::SVProcessFunctor( SVT_CLASS* p_pObject )
: m_pObject( p_pObject )
{
}

template< typename SVT_CLASS >
SVProcessFunctor< SVT_CLASS >::SVProcessFunctor( const SVProcessFunctor& p_rObject )
: m_pObject( p_rObject.m_pObject )
{
}

template< typename SVT_CLASS >
const SVProcessFunctor< SVT_CLASS >& SVProcessFunctor< SVT_CLASS >::operator=( const SVProcessFunctor& p_rObject )
{
	if( this != &p_rObject )
	{
		m_pObject = p_rObject.m_pObject;
	}

	return *this;
}

template< typename SVT_CLASS >
void SVProcessFunctor< SVT_CLASS >::operator()( bool& p_WaitForEvents )
{
	if( m_pObject != NULL )
	{
		m_pObject->Process( p_WaitForEvents );
	}
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVProcessFunctor.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 12:22:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Jan 2010 14:55:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Aug 2009 16:43:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated new threading methodology to remove crash when closing a configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
