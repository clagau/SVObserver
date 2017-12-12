//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandAbstractFactoryTemplate
//* .File Name       : $Workfile:   SVCommandAbstractFactoryTemplate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:10  $
//******************************************************************************

#include "stdafx.h"
#include "SVCommandAbstractFactoryTemplate.h"

SVCommandAbstractFactoryTemplate::SVCommandAbstractFactoryTemplate( const SVCommandFactoryMap& p_rObject )
: m_Factories( p_rObject )
{
}

SVCommandAbstractFactoryTemplate::~SVCommandAbstractFactoryTemplate()
{
}

bool SVCommandAbstractFactoryTemplate::IsValidCommandType( unsigned long p_CommandType ) const
{
	SVCommandFactoryMap::const_iterator l_Iter = m_Factories.find( p_CommandType );

	return ( l_Iter != m_Factories.end() );
}

SVCommandTemplatePtr SVCommandAbstractFactoryTemplate::CreateCommand( unsigned long p_CommandType ) const
{
	SVCommandTemplatePtr l_pCommand;

	SVCommandFactoryMap::const_iterator l_Iter = m_Factories.find( p_CommandType );

	if( l_Iter != m_Factories.end() )
	{
		if( nullptr != l_Iter->second )
		{
			l_pCommand = l_Iter->second->CreateCommand();
		}
	}

	return l_pCommand;
}

