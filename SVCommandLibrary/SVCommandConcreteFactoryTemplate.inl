//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandFactoryTemplate
//* .File Name       : $Workfile:   SVCommandConcreteFactoryTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:16  $
//******************************************************************************

#ifndef SVCOMMANDFACTORYTEMPLATE_INL
#define SVCOMMANDFACTORYTEMPLATE_INL

#include "SVCommandConcreteFactoryTemplate.h"

template< typename SVCommandObject >
unsigned long SVCommandConcreteFactoryTemplate< SVCommandObject >::GetStaticCommandType()
{
	return SVCommandObject::GetStaticCommandType();
}

template< typename SVCommandObject >
SVCommandConcreteFactoryTemplate< SVCommandObject >::SVCommandConcreteFactoryTemplate( SVCommandExecuteFunctor p_ExecuteFunctor )
: SVCommandFactoryTemplate( SVCommandObject::GetStaticCommandType() ), m_ExecuteFunctor( p_ExecuteFunctor )
{
}

template< typename SVCommandObject >
SVCommandConcreteFactoryTemplate< SVCommandObject >::~SVCommandConcreteFactoryTemplate()
{
}

template< typename SVCommandObject >
SVCommandTemplatePtr SVCommandConcreteFactoryTemplate< SVCommandObject >::CreateCommand() const
{
	return SVCommandTemplatePtr{ new SVCommandObject(m_ExecuteFunctor) };
}

#endif

