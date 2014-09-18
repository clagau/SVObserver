//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptIO
//* .File Name       : $Workfile:   SVLptIO.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Sep 2014 15:57:32  $
//******************************************************************************
#include "stdafx.h"
#include "SVLptIO.h"
#include "LPT/SVLptDll.h"

#pragma comment(lib, "SVLpt")

static IrqHandler g_irqHandler;

SVLptIO::SVLptIO()
{
}

SVLptIO::~SVLptIO()
{
	Destroy();
}

HRESULT SVLptIO::GetDataPort(UCHAR& val) const
{
	val = 0;
	return ReadDataPort(val);
}

HRESULT SVLptIO::SetDataPort(UCHAR value) 
{
	return WriteDataPort(value);
}

HRESULT SVLptIO::GetStatusPort(UCHAR& val) const
{
	val = 0;
	return ReadStatusPort(val);
}

HRESULT SVLptIO::GetControlPort(UCHAR& val)  const
{
	val = 0;
	return ReadControlPort(val);
}

HRESULT SVLptIO::SetControlPort(UCHAR value) 
{
	return WriteControlPort(value);
}

HRESULT SVLptIO::IsBidirectionalSupported(bool& biDirectional) const
{
	return SupportsBidirectional(biDirectional);
}

HRESULT SVLptIO::ActivateReadMode()
{
	return SetReadMode();
}

HRESULT SVLptIO::ActivateWriteMode()
{
	return SetWriteMode();
}

bool SVLptIO::IsActive() const
{
	return IsDriverActive();
}

HRESULT SVLptIO::SetActive(bool bActivate) 
{
	HRESULT hr = S_OK;

	if (bActivate)
	{
		hr = Open();
	}
	else
	{
		hr = Close();
	}
	return hr;
}

HRESULT SVLptIO::DisableAckInterrupt() 
{
	return DisableInterrupt();
}

HRESULT SVLptIO::EnableAckInterrupt(IrqHandler func) 
{
	g_irqHandler = func;
	return EnableInterrupt(&SVLptIO::OnLPTInterrupt);
}

void SVLptIO::Destroy()
{
	if (IsActive())
	{
		Close();
	}
}

void SVLptIO::OnLPTInterrupt()
{
	if (!g_irqHandler.empty())
	{
		g_irqHandler();
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLptIO\SVLptIO.cpp_v  $
 * 
 *    Rev 1.0   17 Sep 2014 15:57:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  943
 * SCR Title:  Parallel Port I/O for 64 Bit and Windows 7
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
