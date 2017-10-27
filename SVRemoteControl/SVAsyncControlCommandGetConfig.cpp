//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncControlCommandGetConfig
//* .File Name       : $Workfile:   SVAsyncControlCommandGetConfig.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:24:38  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVAsyncControlCommandGetConfig.h"
#include "SVControlCommands.h"
#pragma endregion Includes

SVAsyncControlCommandGetConfig::SVAsyncControlCommandGetConfig( const _bstr_t& p_rFileName )
: SVCommandTemplate(), m_FileName( p_rFileName ), m_Cancel( 0 )
{
}

SVAsyncControlCommandGetConfig::~SVAsyncControlCommandGetConfig()
{
}

HRESULT SVAsyncControlCommandGetConfig::Execute(CommandDispatcher disp)
{
	return disp->GetConfig( m_FileName, m_Cancel, false, m_Status );
}

HRESULT SVAsyncControlCommandGetConfig::Cancel()
{
	m_Cancel = 1;

	return S_OK;
}
