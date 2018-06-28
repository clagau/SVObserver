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

template<typename SVAbstractFactory>
SVCommandAbstractFactorySingleton<SVAbstractFactory>::~SVCommandAbstractFactorySingleton()
{
}

template<typename SVAbstractFactory>
SVCommandAbstractFactorySingleton<SVAbstractFactory>& SVCommandAbstractFactorySingleton< SVAbstractFactory >::Instance()
{
	static SVCommandAbstractFactorySingleton l_Object;

	return l_Object;
}

template<typename SVAbstractFactory>
SVCommandAbstractFactoryTemplatePtr SVCommandAbstractFactorySingleton<SVAbstractFactory>::GetAbstractFactory() const
{
	return m_pAbstractFactory;
}

template<typename SVAbstractFactory>
bool SVCommandAbstractFactorySingleton<SVAbstractFactory>::IsValidCommandType(unsigned long CommandType) const
{
	bool l_Valid = true;

	if (m_pAbstractFactory.empty())
	{
		l_bValid = false;
	}
	else
	{
		l_bValid = m_pAbstractFactory->IsValidCommandType(CommandType);
	}

	return l_bValid;
}

template< typename SVAbstractFactory >
SvOi::ICommandPtr SVCommandAbstractFactorySingleton<SVAbstractFactory>::CreateCommand(unsigned long CommandType) const
{
	SvOi::ICommandPtr pCommand;

	if (nullptr != m_pAbstractFactory)
	{
		pCommand = m_pAbstractFactory->CreateCommand(CommandType);
	}

	return pCommand;
}

template< typename SVAbstractFactory >
SVCommandAbstractFactorySingleton<SVAbstractFactory>::SVCommandAbstractFactorySingleton()
	: m_pAbstractFactory()
{
	m_pAbstractFactory = SVCommandAbstractFactoryTemplatePtr {new SVAbstractFactory};
}

