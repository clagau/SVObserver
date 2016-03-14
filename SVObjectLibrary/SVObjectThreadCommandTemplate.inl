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

