//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncControlCommandPutFile
//* .File Name       : $Workfile:   SVAsyncControlCommandPutFile.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:24:38  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVCommandTemplate.h"
#pragma endregion Includes

class SVAsyncControlCommandPutFile : public SVCommandTemplate
{
public:
	SVAsyncControlCommandPutFile( const _bstr_t& p_rSourcePath, const _bstr_t& p_rDestinationPath );
	virtual ~SVAsyncControlCommandPutFile();

	virtual HRESULT Execute(CommandDispatcher disp);
	virtual HRESULT Cancel();

protected:
	_bstr_t m_SourcePath;
	_bstr_t m_DestinationPath;

	long m_Cancel;
};
