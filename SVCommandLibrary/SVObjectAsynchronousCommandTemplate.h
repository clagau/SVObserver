//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAsynchronousCommandTemplate
//* .File Name       : $Workfile:   SVObjectAsynchronousCommandTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Nov 2014 10:00:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectCommandWrapperTemplate.h"
#include "SVObjectAsynchronousCommandTemplate.h"
#pragma endregion Includes

template<typename CommandPtr>
class SVObjectAsynchronousCommandTemplate
{
public:
	SVObjectAsynchronousCommandTemplate(const SVObjectAsynchronousCommandTemplate& rObject);
	SVObjectAsynchronousCommandTemplate(uint32_t objectID, const CommandPtr& rpCommand);

	virtual ~SVObjectAsynchronousCommandTemplate();

	HRESULT SubmitCommand();
	HRESULT IsCommandComplete();
	HRESULT WaitForCommandToComplete( DWORD p_TimeoutInMilliseconds );

	const CommandPtr& GetCommandPtr() const;

private:
	typedef typename SVObjectCommandWrapperTemplate<CommandPtr>::SVObjectCommandWrapperPtr SVObjectCommandWrapperPtr;

	uint32_t m_ObjectID;
	CommandPtr m_pCommand;
	SVObjectCommandWrapperPtr m_pWrapper;

};

#include "SVObjectAsynchronousCommandTemplate.inl"

