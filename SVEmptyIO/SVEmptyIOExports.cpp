// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVEmptyIOExports
// * .File Name       : $Workfile:   SVEmptyIOExports.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 09:13:42  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVEmptyIOExports.h"
#include "Definitions/SVImageFormatEnum.h"
#include "SVIOLibrary\SVIOParameterEnum.h"
#pragma endregion Includes


constexpr char* c_ioBoardName = "IO_Board_1";
constexpr char* c_digitizer = ".Dig_";
constexpr unsigned long cTriggerMaxCount = 4UL;
constexpr unsigned long cInputMaxCount = 8UL;
constexpr unsigned long cOutputMaxCount = 16UL;

static std::string g_boardName{c_ioBoardName};

HRESULT WINAPI SVCreate()
{
	return S_OK;
}

HRESULT WINAPI SVDestroy()
{
	return S_OK;
}

unsigned long WINAPI SVInputGetCount( )
{
	return cInputMaxCount;
}

bool WINAPI SVInputGetValue( unsigned long)
{
	return false;
}

unsigned long WINAPI SVInputGetValues()
{
	return 0UL;
}

unsigned long WINAPI SVOutputGetCount()
{
	return cOutputMaxCount;
}

HRESULT WINAPI SVOutputSetValue(unsigned long, bool)
{
	return S_OK;
}

unsigned long WINAPI SVOutputGetPortCount()
{
	return 0UL;
}

HRESULT WINAPI SVOutputSetPortValue(unsigned long , unsigned long)
{
	return S_OK;
}

unsigned long WINAPI SVTriggerGetCount()
{
	return cTriggerMaxCount;
}

unsigned long WINAPI SVTriggerGetHandle(unsigned long index)
{
	return index + 1;
}

_variant_t WINAPI SVTriggerGetName(unsigned long triggerchannel)
{
	_variant_t result;

	if (0 < triggerchannel && triggerchannel <= cTriggerMaxCount)
	{
		std::string triggerName{g_boardName};
		triggerName += c_digitizer;
		triggerName += std::to_string(triggerchannel - 1);
		result.SetString(triggerName.c_str());
	} 

	return result;
}

HRESULT WINAPI SVTriggerRegister( unsigned long triggerchannel, SvTrig::TriggerCallBack pTriggerCallback)
{
	if (0 < triggerchannel && triggerchannel <= cTriggerMaxCount)
	{
		return S_OK;
	} 

	return E_FAIL;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long triggerchannel)
{
	if (0 < triggerchannel && triggerchannel <= cTriggerMaxCount)
	{
		return S_OK;
	} 

	return E_FAIL;
}

HRESULT WINAPI SVTriggerStart( unsigned long triggerchannel )
{
	HRESULT l_hrOk = E_FAIL;

	if(0 < triggerchannel && triggerchannel <= cTriggerMaxCount)
	{
		l_hrOk = S_OK;
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerStop( unsigned long triggerchannel )
{
	HRESULT l_hrOk = E_FAIL;

	if(0 < triggerchannel && triggerchannel <= cTriggerMaxCount)
	{
		l_hrOk = S_OK;
	} 

	return l_hrOk;
}

unsigned long WINAPI SVTriggerGetParameterCount( unsigned long )
{
	return 0UL;
}

_variant_t WINAPI SVTriggerGetParameterName( unsigned long triggerchannel, unsigned long index)
{
	_variant_t result;

	if (0 < triggerchannel && triggerchannel <= cTriggerMaxCount)
	{
		if (SVIOParameterEnum::SVBoardName == index)
		{
			result.SetString(_T("Board name"));
		}
	}
	return result;
}

_variant_t WINAPI SVTriggerGetParameterValue( unsigned long triggerchannel, unsigned long index)
{
	_variant_t result;

	if (0 < triggerchannel && triggerchannel <= cTriggerMaxCount)
	{
		if (SVIOParameterEnum::SVBoardName == index)
		{
			result.SetString(g_boardName.c_str());
		}
	}

	return result;
}

HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerchannel, unsigned long index, const _variant_t& rValue)
{
	HRESULT result{E_NOTIMPL};

	if (0 < triggerchannel && triggerchannel <= cTriggerMaxCount)
	{
		if (SVIOParameterEnum::SVBoardName == index)
		{
			if (VT_BSTR == rValue.vt)
			{
				g_boardName.assign(_bstr_t{rValue.bstrVal});
				result = S_OK;
			}
		}
		else
		{
			result = S_OK;
		}
	}

	return result;
}

// Digitizer Export Functions
unsigned long WINAPI SVDigitizerGetCount()
{
	return 0UL;
}

unsigned long WINAPI SVDigitizerGetHandle(unsigned long)
{
	return 0UL;
}

_variant_t WINAPI SVDigitizerGetName(unsigned long)
{
	return _variant_t{};
}

unsigned long WINAPI SVDigitizerGetBufferWidth(unsigned long)
{
	return 0UL;
}

unsigned long WINAPI SVDigitizerGetBufferHeight(unsigned long)
{
	return 0UL;
}

int WINAPI SVDigitizerGetBufferFormat(unsigned long)
{
	return SvDef::SVImageFormatUnknown;
}

HRESULT WINAPI SVDigitizerCreateBuffers(unsigned long)
{
	return E_FAIL;
}

HRESULT WINAPI SVDigitizerRegisterBufferInterface(unsigned long , SVAcquisitionBufferInterface*)
{
	return E_FAIL;
}

HRESULT WINAPI SVDigitizerStart(unsigned long)
{
	return E_FAIL;
}

HRESULT WINAPI SVDigitizerStop(unsigned long)
{
	return S_OK;
}

HRESULT WINAPI SVDigitizerUnregisterBufferInterface(unsigned long)
{
	return E_FAIL;
}

HRESULT WINAPI SVDigitizerInternalTriggerEnable(unsigned long)
{
	return S_OK;
}

HRESULT WINAPI SVDigitizerInternalTrigger(unsigned long)
{
	return E_FAIL;
}

HRESULT WINAPI SVDigitizerDestroyBuffers(unsigned long)
{
	return S_OK;
}
