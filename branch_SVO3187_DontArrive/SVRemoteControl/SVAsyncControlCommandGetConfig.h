//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncControlCommandGetConfig
//* .File Name       : $Workfile:   SVAsyncControlCommandGetConfig.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:24:38  $
//******************************************************************************
#pragma once
#pragma region Includes
#include "SVCommandTemplate.h"
#pragma endregion Includes

class SVAsyncControlCommandGetConfig : public SVCommandTemplate
{
public:
	SVAsyncControlCommandGetConfig( const _bstr_t& p_rFileName );

	virtual ~SVAsyncControlCommandGetConfig();
	virtual HRESULT Execute(CommandDispatcher disp);
	virtual HRESULT Cancel();

protected:
	_bstr_t m_FileName;

	long m_Cancel;
};
