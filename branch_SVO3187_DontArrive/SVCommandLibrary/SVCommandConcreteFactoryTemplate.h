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

template<typename CommandObject>
class SVCommandConcreteFactoryTemplate : public SVCommandFactoryTemplate
{
public:
	static unsigned long GetStaticCommandType();

	SVCommandConcreteFactoryTemplate(SVCommandExecuteFunctor ExecuteFunctor);
	virtual ~SVCommandConcreteFactoryTemplate();

	virtual SvOi::ICommandPtr CreateCommand() const override;

protected:
	SVCommandExecuteFunctor m_ExecuteFunctor;
};

#include "SVCommandConcreteFactoryTemplate.inl"

