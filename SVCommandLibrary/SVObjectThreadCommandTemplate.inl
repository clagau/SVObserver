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

template<typename CommandPtr>
SVObjectThreadCommandTemplate<CommandPtr>::SVObjectThreadCommandTemplate(const CommandPtr& rpCommand) : SvOi::ICommand()
	,m_pCommand(rpCommand)
{
}

template<typename CommandPtr>
SVObjectThreadCommandTemplate<CommandPtr>::~SVObjectThreadCommandTemplate()
{
}

template<typename CommandPtr>
HRESULT SVObjectThreadCommandTemplate<CommandPtr>::Execute()
{
	HRESULT l_Status = S_OK;

	if(nullptr != m_pCommand )
	{
		m_pCommand->Execute();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

