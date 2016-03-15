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

