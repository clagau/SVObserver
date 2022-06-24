//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAsyncProcedure
//* .File Name       : $Workfile:   SVAsyncProcedure.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:59:04  $
//******************************************************************************

#pragma once

#include "SVThread.h"

namespace SvSyl
{
class SVAsyncProcedure
{
private:
	SVThread m_thread;
	PAPCFUNC m_apcHandler;
	std::string m_tag;

	// The APCSignal handler must implement this
	//static void CALLBACK APCProc( DWORD dwParam );
	
public:
	SVAsyncProcedure() = default;
	~SVAsyncProcedure();

	HRESULT Create(PAPCFUNC apcHandler, LPCTSTR tag);
	void Destroy();

	int GetPriority() const;
	void SetPriority(int priority);

	bool IsActive() const;

	HRESULT Signal(const void* const pData);
};
} //namespace SvSyl