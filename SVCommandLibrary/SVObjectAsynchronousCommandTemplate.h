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
#include "SVUtilityLibrary/SVGUID.h"
#include "SVObjectCommandWrapperTemplate.h"
#include "SVObjectAsynchronousCommandTemplate.h"
#pragma endregion Includes

template<typename CommandPtr>
class SVObjectAsynchronousCommandTemplate
{
public:
	SVObjectAsynchronousCommandTemplate(const SVObjectAsynchronousCommandTemplate& rObject);
	SVObjectAsynchronousCommandTemplate(const SVGUID& rObjectID, const CommandPtr& rpCommand);

	virtual ~SVObjectAsynchronousCommandTemplate();

	HRESULT SubmitCommand();
	HRESULT IsCommandComplete();
	HRESULT WaitForCommandToComplete( DWORD p_TimeoutInMilliseconds );

	const CommandPtr& GetCommandPtr() const;

private:
	typedef typename SVObjectCommandWrapperTemplate<CommandPtr>::SVObjectCommandWrapperPtr SVObjectCommandWrapperPtr;

	SVGUID m_ObjectID;
	CommandPtr m_pCommand;
	SVObjectCommandWrapperPtr m_pWrapper;

};

#include "SVObjectAsynchronousCommandTemplate.inl"

