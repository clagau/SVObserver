//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandConcreteTemplate
//* .File Name       : $Workfile:   SVCommandConcreteTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:26  $
//******************************************************************************

#ifndef SVCOMMANDCONCRETETEMPLATE_INL
#define SVCOMMANDCONCRETETEMPLATE_INL

#include "SVCommandConcreteTemplate.h"

template< unsigned long p_CommandType >
unsigned long SVCommandConcreteTemplate< p_CommandType >::GetStaticCommandType()
{
	return p_CommandType;
}

template< unsigned long p_CommandType >
SVCommandConcreteTemplate< p_CommandType >::SVCommandConcreteTemplate( SVCommandExecuteFunctor p_ExecuteFunctor )
: SVCommandTemplate( p_CommandType )
, SVCommandAttributes()
, SVCommandResults()
, m_ExecuteFunctor( p_ExecuteFunctor )
{
}

template< unsigned long p_CommandType >
SVCommandConcreteTemplate< p_CommandType >::~SVCommandConcreteTemplate()
{
}

template< unsigned long p_CommandType >
HRESULT SVCommandConcreteTemplate< p_CommandType >::Execute()
{
	return m_ExecuteFunctor( m_Attributes, m_Results );
}

#endif

