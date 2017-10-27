//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncControlCommandPutFile
//* .File Name       : $Workfile:   SVAsyncControlCommandPutFile.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:24:38  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVAsyncControlCommandPutFile.h"
#include "SVControlCommands.h"
#pragma endregion Includes

SVAsyncControlCommandPutFile::SVAsyncControlCommandPutFile( const _bstr_t& p_rSourcePath, const _bstr_t& p_rDestinationPath )
: SVCommandTemplate(), m_SourcePath( p_rSourcePath ), m_DestinationPath( p_rDestinationPath ), m_Cancel( 0 )
{
}

SVAsyncControlCommandPutFile::~SVAsyncControlCommandPutFile()
{
}

HRESULT SVAsyncControlCommandPutFile::Execute(CommandDispatcher disp)
{
	return disp->PutFile( m_SourcePath, m_DestinationPath, m_Cancel, false, m_Status );
}

HRESULT SVAsyncControlCommandPutFile::Cancel()
{
	m_Cancel = 1;

	return S_OK;
}
