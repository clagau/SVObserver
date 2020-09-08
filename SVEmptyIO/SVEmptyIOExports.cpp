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

static std::string g_boardName{c_ioBoardName};


HRESULT WINAPI SVCreate()
{
	return S_OK;
}

HRESULT WINAPI SVDestroy()
{
	return S_OK;
}

HRESULT WINAPI SVInputGetCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 8;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputGetValue( unsigned long , bool* p_pbValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbValue )
	{
		*p_pbValue = false;
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputGetValues( unsigned long *p_pulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulValue )
	{
		*p_pulValue = 0;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 16;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputSetValue( unsigned long , bool  )
{
	return S_OK;
}

HRESULT WINAPI SVOutputGetPortCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 0;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputSetPortValue( unsigned long , unsigned long  )
{
	return S_OK;
}

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 4;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetHandle( unsigned long *pTriggerchannel, unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != pTriggerchannel )
	{
		*pTriggerchannel = p_ulIndex + 1;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetName( unsigned long triggerchannel, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName && 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = S_OK;

		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}

		std::string triggerName{g_boardName};
		triggerName += c_digitizer;
		triggerName += std::to_string(triggerchannel - 1);
		_bstr_t name{triggerName.c_str()};
		*p_pbstrName = name.Detach();
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerRegister( unsigned long triggerchannel, SvTi::TriggerCallBack pTriggerCallback)
{
	if (0 < triggerchannel && triggerchannel <= 4 )
	{
		return S_OK;
	} 

	return E_FAIL;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long triggerchannel)
{
	if (0 < triggerchannel && triggerchannel <= 4 )
	{
		return S_OK;
	} 

	return E_FAIL;
}

HRESULT WINAPI SVTriggerStart( unsigned long triggerchannel )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = S_OK;
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerStop( unsigned long triggerchannel )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = S_OK;
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		if ( 0 < triggerchannel && triggerchannel <= 4 )
		{
			*p_pulCount = 0;

			l_hrOk = S_OK;
		}
		else
		{
			*p_pulCount = 0;
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterName( unsigned long , unsigned long , BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterValue( unsigned long triggerchannel, unsigned long , VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pvarValue )
	{
		if ( S_OK == ::VariantClear( p_pvarValue ) )
		{
			if ( 0 < triggerchannel && triggerchannel <= 4 )
			{
				l_hrOk = S_OK;
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerchannel, unsigned long ulIndex, VARIANT *pvarValue )
{
	HRESULT result{S_FALSE};

	if ( 0 < triggerchannel && triggerchannel <= 4 )
	{
		if ( nullptr != pvarValue )
		{

			if (SVIOParameterEnum::SVBoardName == ulIndex)
			{
				if (VT_BSTR == pvarValue->vt)
				{
					g_boardName.assign(_bstr_t{pvarValue->bstrVal});
					result = S_OK;
				}
			}
			else
			{
				result = S_OK;
			}
		}
	}

	return result;
}

// Digitizer Export Functions
HRESULT WINAPI SVDigitizerGetCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 0;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetHandle( unsigned long *pTriggerchannel, unsigned long  )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != pTriggerchannel )
	{
		*pTriggerchannel = 0;
	} 

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetName( unsigned long , BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		l_hrOk = S_OK;

		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}
	} 

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferWidth( unsigned long , unsigned long *p_pulWidth )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulWidth )
	{
		*p_pulWidth = 0;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferHeight( unsigned long , unsigned long *p_pulHeight )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulHeight )
	{
		*p_pulHeight = 0;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerGetBufferFormat( unsigned long , int *p_piFormat )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_piFormat )
	{
		*p_piFormat = SvDef::SVImageFormatUnknown;
	}

	return l_hrOk;
}

HRESULT WINAPI SVDigitizerCreateBuffers( unsigned long  )
{
	return S_FALSE;
}

HRESULT WINAPI SVDigitizerRegisterBufferInterface( unsigned long , SVAcquisitionBufferInterface*  )
{
	return S_FALSE;
}

HRESULT WINAPI SVDigitizerStart( unsigned long  )
{
	return S_FALSE;
}

HRESULT WINAPI SVDigitizerStop( unsigned long  )
{
	return S_OK;
}

HRESULT WINAPI SVDigitizerUnregisterBufferInterface( unsigned long  )
{
	return S_FALSE;
}

HRESULT WINAPI SVDigitizerInternalTriggerEnable( unsigned long  )
{
	return S_OK;
}

HRESULT WINAPI SVDigitizerInternalTrigger( unsigned long  )
{
	return S_FALSE;
}

HRESULT WINAPI SVDigitizerDestroyBuffers( unsigned long  )
{
	return S_OK;
}

HRESULT WINAPI SVDigitizerSetParameters( unsigned long , const SVDeviceParamCollection*  )
{
	return S_OK;
}

HRESULT WINAPI SVDigitizerSetParameter( unsigned long , const SVDeviceParamWrapper*  )
{
	return S_OK;
}

HRESULT WINAPI SVDigitizerGetParameter( unsigned long , SVDeviceParamEnum , SVDeviceParamWrapper**  )
{
	return S_OK;
}
