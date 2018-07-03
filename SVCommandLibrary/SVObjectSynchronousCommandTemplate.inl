//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectSynchronousCommandTemplate
//* .File Name       : $Workfile:   SVObjectSynchronousCommandTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:01:02  $
//******************************************************************************


template<typename CommandPtr>
SVObjectSynchronousCommandTemplate<CommandPtr>::SVObjectSynchronousCommandTemplate(const SVGUID& rObjectID, const CommandPtr& rpCommand)
	: m_ObjectID(rObjectID), m_pCommand(rpCommand)
{
}

template<typename CommandPtr>
SVObjectSynchronousCommandTemplate< CommandPtr >::~SVObjectSynchronousCommandTemplate()
{
}

template<typename CommandPtr>
HRESULT SVObjectSynchronousCommandTemplate<CommandPtr>::Execute(DWORD TimeoutInMilliseconds)
{
	HRESULT Result {S_OK};

	if (GUID_NULL != m_ObjectID && nullptr != m_pCommand)
	{
		typedef SVObjectCommandWrapperTemplate<CommandPtr>::SVObjectCommandWrapperPtr SVObjectCommandWrapperPtr;

		//SVObjectCommandWrapperTemplate<CommandPtr>::SVObjectCommandWrapperPtr pWrapper{new SVObjectCommandWrapperTemplate<CommandPtr>(m_pCommand)};
		SVObjectCommandWrapperPtr pWrapper = std::make_shared<SVObjectCommandWrapperTemplate<CommandPtr>>(m_pCommand);

		if (nullptr != pWrapper)
		{
			//SvOi::ICommandPtr pCommand{new SVObjectThreadCommandTemplate< SVObjectCommandWrapperTemplate<CommandPtr>::SVObjectCommandWrapperPtr >(pWrapper)};

			SvOi::ICommandPtr pCommand = std::make_shared<SVObjectThreadCommandTemplate<SVObjectCommandWrapperPtr>>(pWrapper);

			if (nullptr != pCommand)
			{
				auto pInspection = dynamic_cast<SvOi::IInspectionProcess*>  (SvOi::getObject(m_ObjectID));
				Result = (nullptr != pInspection) ? pInspection->SubmitCommand(pCommand) : E_POINTER;

				if (S_OK == Result)
				{
					Result = pWrapper->WaitForRequest(TimeoutInMilliseconds);
				}

				if (S_OK == Result)
				{
					Result = pWrapper->GetStatus();
				}
			}
			else
			{
				Result = E_FAIL;
			}
		}
		else
		{
			Result = E_FAIL;
		}
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}
