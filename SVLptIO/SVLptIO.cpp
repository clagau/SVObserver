//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptIO
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLptIO.h"
#include "SVLPTIO/LPT/SVLptDll.h"
#pragma endregion Includes

#pragma comment(lib, "SVLpt")

static IrqHandler g_IrqHandler;

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
	g_IrqHandler = nullptr;
	return DisableInterrupt();
}

HRESULT SVLptIO::EnableAckInterrupt(const IrqHandler& func)
{
	g_IrqHandler = func;
	return EnableInterrupt(&SVLptIO::OnLPTInterrupt);
}

bool SVLptIO::isIrqHandlerEnabled()  const
{
	return nullptr != g_IrqHandler;
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
	if (nullptr != g_IrqHandler)
	{
		g_IrqHandler();
	}
}

