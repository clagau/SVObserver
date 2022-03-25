//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncControlCommandPutConfig
//* .File Name       : $Workfile:   SVAsyncControlCommandPutConfig.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:24:38  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVCommandTemplate.h"
#pragma endregion Includes

class SVAsyncControlCommandPutConfig : public SVCommandTemplate
{
public:
	SVAsyncControlCommandPutConfig( const _bstr_t& p_rFileName, const _bstr_t& p_rProductName );
	virtual ~SVAsyncControlCommandPutConfig();

	virtual HRESULT Execute(CommandDispatcher disp);
	virtual HRESULT Cancel();

protected:
	_bstr_t m_FileName;
	_bstr_t m_ProductName;

	long m_Cancel;
};
