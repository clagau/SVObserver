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

template<unsigned long CommandType>
unsigned long SVCommandConcreteTemplate<CommandType>::GetStaticCommandType()
{
	return CommandType;
}

template<unsigned long CommandType>
SVCommandConcreteTemplate<CommandType>::SVCommandConcreteTemplate( SVCommandExecuteFunctor ExecuteFunctor ) : 
	SvOi::ICommand()
, SVCommandAttributes()
, SVCommandResults()
, m_CommandType{CommandType}
, m_ExecuteFunctor( ExecuteFunctor )
{
}

template<unsigned long CommandType>
SVCommandConcreteTemplate<CommandType>::~SVCommandConcreteTemplate()
{
}

template<unsigned long CommandType>
HRESULT SVCommandConcreteTemplate<CommandType>::Execute()
{
	return m_ExecuteFunctor(m_Attributes, m_Results);
}


