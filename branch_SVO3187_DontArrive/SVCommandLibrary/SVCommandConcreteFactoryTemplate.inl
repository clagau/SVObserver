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

template<typename CommandObject>
unsigned long SVCommandConcreteFactoryTemplate<CommandObject>::GetStaticCommandType()
{
	return CommandObject::GetStaticCommandType();
}

template<typename CommandObject>
SVCommandConcreteFactoryTemplate< CommandObject >::SVCommandConcreteFactoryTemplate( SVCommandExecuteFunctor ExecuteFunctor )
: SVCommandFactoryTemplate(CommandObject::GetStaticCommandType()), m_ExecuteFunctor( ExecuteFunctor )
{
}

template<typename CommandObject>
SVCommandConcreteFactoryTemplate<CommandObject>::~SVCommandConcreteFactoryTemplate()
{
}

template<typename CommandObject>
SvOi::ICommandPtr SVCommandConcreteFactoryTemplate<CommandObject>::CreateCommand() const
{
	return std::make_shared<CommandObject>(m_ExecuteFunctor);
}

