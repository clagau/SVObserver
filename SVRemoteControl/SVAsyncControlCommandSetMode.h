//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncControlCommandSetMode
//* .File Name       : $Workfile:   SVAsyncControlCommandSetMode.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   14 May 2013 17:24:36  $
//******************************************************************************
#pragma once
#pragma region Includes
#include "SVCommandTemplate.h"
#pragma endregion Includes

class SVAsyncControlCommandSetMode : public SVCommandTemplate
{
public:
	SVAsyncControlCommandSetMode( long p_NewMode );
	virtual ~SVAsyncControlCommandSetMode();

	virtual HRESULT Execute(CommandDispatcher disp);
	virtual HRESULT Cancel();

protected:
	long m_NewMode;
};
