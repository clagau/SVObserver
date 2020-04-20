//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAsynchronousCommandTemplate
//* .File Name       : $Workfile:   SVObjectAsynchronousCommandTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Nov 2014 10:00:04  $
//******************************************************************************

#pragma region Includes
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVObjectThreadCommandTemplate.h"
#pragma endregion Includes

template<typename CommandPtr>
SVObjectAsynchronousCommandTemplate<CommandPtr>::SVObjectAsynchronousCommandTemplate(const SVObjectAsynchronousCommandTemplate& rObject)
: m_ObjectID( rObject.m_ObjectID), m_pCommand(rObject.m_pCommand), m_pWrapper(rObject.m_pWrapper)
{
}

template<typename CommandPtr>
SVObjectAsynchronousCommandTemplate<CommandPtr>::SVObjectAsynchronousCommandTemplate(uint32_t objectID, const CommandPtr& rpCommand)
: m_ObjectID(objectID), m_pCommand(rpCommand), m_pWrapper()
{
}

template<typename CommandPtr>
SVObjectAsynchronousCommandTemplate<CommandPtr>::~SVObjectAsynchronousCommandTemplate()
{
}

template<typename CommandPtr>
HRESULT SVObjectAsynchronousCommandTemplate<CommandPtr>::SubmitCommand()
{
	HRESULT Result{S_OK};

	if(nullptr != m_pCommand )
	{
		m_pWrapper = std::make_shared<SVObjectCommandWrapperTemplate<CommandPtr>>(m_pCommand);

		if(nullptr != m_pWrapper)
		{
			SvOi::ICommandPtr pCommand = std::make_shared<SVObjectThreadCommandTemplate<SVObjectCommandWrapperPtr>>(m_pWrapper);

			if(nullptr != pCommand)
			{
				auto pInspection = dynamic_cast<SvOi::IInspectionProcess*>  (SvOi::getObject(m_ObjectID));
				Result = (nullptr != pInspection) ? pInspection->SubmitCommand(pCommand) : E_POINTER;
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

template<typename CommandPtr>
HRESULT SVObjectAsynchronousCommandTemplate<CommandPtr>::IsCommandComplete()
{
	HRESULT l_Status = S_OK;

	if(nullptr != m_pWrapper)
	{
		l_Status = m_pWrapper->IsRequestComplete();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template<typename CommandPtr>
HRESULT SVObjectAsynchronousCommandTemplate<CommandPtr>::WaitForCommandToComplete( DWORD TimeoutInMilliseconds )
{
	HRESULT l_Status = S_OK;

	if( !( m_pWrapper.empty() ) )
	{
		l_Status = m_pWrapper->WaitForRequest( TimeoutInMilliseconds );

		if( S_OK == l_Status )
		{
			l_Status = m_pWrapper->GetStatus();
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template<typename CommandPtr>
const CommandPtr& SVObjectAsynchronousCommandTemplate<CommandPtr>::GetCommandPtr() const
{
	return m_pCommand;
}

