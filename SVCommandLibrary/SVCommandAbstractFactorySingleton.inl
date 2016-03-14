//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandAbstractFactorySingleton
//* .File Name       : $Workfile:   SVCommandAbstractFactorySingleton.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:10  $
//******************************************************************************

#ifndef SVCOMMANDABSTRACTFACTORYSINGLETON_INL
#define SVCOMMANDABSTRACTFACTORYSINGLETON_INL

#include "SVCommandAbstractFactorySingleton.h"

template< typename SVAbstractFactory >
SVCommandAbstractFactorySingleton< SVAbstractFactory >::~SVCommandAbstractFactorySingleton()
{
}

template< typename SVAbstractFactory >
SVCommandAbstractFactorySingleton< SVAbstractFactory >& SVCommandAbstractFactorySingleton< SVAbstractFactory >::Instance()
{
	static SVCommandAbstractFactorySingleton l_Object;

	return l_Object;
}

template< typename SVAbstractFactory >
SVCommandAbstractFactoryTemplatePtr SVCommandAbstractFactorySingleton< SVAbstractFactory >::GetAbstractFactory() const
{
	return m_pAbstractFactory;
}

template< typename SVAbstractFactory >
bool SVCommandAbstractFactorySingleton< SVAbstractFactory >::IsValidCommandType( unsigned long p_CommandType ) const
{
	bool l_Valid = true;

	if( m_pAbstractFactory.empty() )
	{
		l_bValid = false;
	}
	else
	{
		l_bValid = m_pAbstractFactory->IsValidCommandType( p_CommandType );
	}

	return l_bValid;
}

template< typename SVAbstractFactory >
SVCommandTemplatePtr SVCommandAbstractFactorySingleton< SVAbstractFactory >::CreateCommand( unsigned long p_CommandType ) const
{
	SVCommandTemplatePtr l_Command;

	if( ! m_pAbstractFactory.empty() )
	{
		l_Command = m_pAbstractFactory->CreateCommand( p_CommandType );
	}

	return l_Command;
}

template< typename SVAbstractFactory >
SVCommandAbstractFactorySingleton< SVAbstractFactory >::SVCommandAbstractFactorySingleton()
: m_pAbstractFactory()
{
	m_pAbstractFactory = new SVAbstractFactory;
}

#endif

