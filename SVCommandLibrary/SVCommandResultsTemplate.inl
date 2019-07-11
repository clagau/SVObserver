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

