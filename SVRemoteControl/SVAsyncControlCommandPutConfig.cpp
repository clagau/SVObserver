//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncControlCommandPutConfig
//* .File Name       : $Workfile:   SVAsyncControlCommandPutConfig.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:24:38  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVAsyncControlCommandPutConfig.h"
#include "SVControlCommands.h"
#pragma endregion Includes

SVAsyncControlCommandPutConfig::SVAsyncControlCommandPutConfig( const _bstr_t& p_rFileName, const _bstr_t& p_rProductName )
: SVCommandTemplate(), m_FileName( p_rFileName ), m_ProductName( p_rProductName ), m_Cancel( 0 )
{
}

SVAsyncControlCommandPutConfig::~SVAsyncControlCommandPutConfig()
{
}

HRESULT SVAsyncControlCommandPutConfig::Execute(CommandDispatcher disp)
{
	HRESULT l_Status = S_OK;

	l_Status = disp->PutConfig( m_FileName, m_ProductName, m_Cancel, false, m_Status );

	return l_Status;
}

HRESULT SVAsyncControlCommandPutConfig::Cancel()
{
	m_Cancel = 1;

	return S_OK;
}
