//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandConcreteFactoryTemplate
//* .File Name       : $Workfile:   SVCommandConcreteFactoryTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:14  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVCommandFactoryTemplate.h"
#include "SVCommandConcreteTemplate.h"
#pragma endregion Includes

template< typename SVCommandObject >
class SVCommandConcreteFactoryTemplate : public SVCommandFactoryTemplate
{
public:
	static unsigned long GetStaticCommandType();

	SVCommandConcreteFactoryTemplate( SVCommandExecuteFunctor p_ExecuteFunctor );
	virtual ~SVCommandConcreteFactoryTemplate();

	virtual SVCommandTemplatePtr CreateCommand() const;

protected:
	SVCommandExecuteFunctor m_ExecuteFunctor;

};

#include "SVCommandConcreteFactoryTemplate.inl"

