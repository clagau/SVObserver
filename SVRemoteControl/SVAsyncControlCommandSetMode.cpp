//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncControlCommandSetMode
//* .File Name       : $Workfile:   SVAsyncControlCommandSetMode.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:24:38  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVAsyncControlCommandSetMode.h"
#include "SVControlCommands.h"
#pragma endregion Includes

SVAsyncControlCommandSetMode::SVAsyncControlCommandSetMode( long p_NewMode )
: SVCommandTemplate(), m_NewMode( p_NewMode )
{
}

SVAsyncControlCommandSetMode::~SVAsyncControlCommandSetMode()
{
}

HRESULT SVAsyncControlCommandSetMode::Execute(CommandDispatcher disp)
{
	return disp->SetMode( m_NewMode, m_Status );
}

HRESULT SVAsyncControlCommandSetMode::Cancel()
{
	return CO_E_CANCEL_DISABLED;
}
