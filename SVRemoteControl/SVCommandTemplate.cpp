//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandTemplate
//* .File Name       : $Workfile:   SVCommandTemplate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2012 15:37:42  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVCommandTemplate.h"
#pragma endregion Includes

SVCommandTemplate::~SVCommandTemplate()
{
}

const SVCommandStatus& SVCommandTemplate::GetStatus() const
{
	return m_Status;
}

SVCommandTemplate::SVCommandTemplate()
: m_Status()
{
}
