//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectSynchronousCommandTemplate
//* .File Name       : $Workfile:   SVObjectSynchronousCommandTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:00:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

template<typename CommandPtr>
class SVObjectSynchronousCommandTemplate
{
public:
	SVObjectSynchronousCommandTemplate() = delete;
	SVObjectSynchronousCommandTemplate(uint32_t rObjectID, const CommandPtr& rpCommand);

	virtual ~SVObjectSynchronousCommandTemplate();

	HRESULT Execute( DWORD p_TimeoutInMilliseconds );

private:
	uint32_t m_ObjectID;
	CommandPtr m_pCommand;
};

enum 
{
	TWO_MINUTE_CMD_TIMEOUT = 120000
};

#include "SVObjectSynchronousCommandTemplate.inl"

