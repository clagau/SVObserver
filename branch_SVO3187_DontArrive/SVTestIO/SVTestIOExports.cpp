// ******************************************************************************
// * COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIONIExports
// * .File Name       : $Workfile:   SVTestIOExports.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   16 Jul 2013 13:22:16  $
// ******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVTestIOApp.h"
#include "SVTestIODlg.h"
#include "SVTestIOExports.h"
#pragma endregion Includes

extern SVTestIOApp theApp;
constexpr unsigned long cInputMaxCount = 8;
constexpr unsigned long cOutputMaxCount = 16;
constexpr unsigned long cMaxTriggers = 4;
constexpr LPCTSTR cTriggerName = "IO_Board_1.Dig_";			///This name must match the name in the SVHardwareManifest

HRESULT WINAPI SVCreate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );


	HRESULT l_hrOk = S_OK;

	return l_hrOk;
}

HRESULT WINAPI SVDestroy()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;
	return l_hrOk;
}

unsigned long WINAPI SVInputGetCount()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return cInputMaxCount;
}

bool WINAPI SVInputGetValue(unsigned long channel)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	bool result{false};

	result = (theApp.m_pTestIODlg->m_lInputs & (1 << channel))== 0;

	return result;
}

unsigned long WINAPI SVInputGetValues()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return theApp.m_pTestIODlg->m_lInputs;
}

unsigned long WINAPI SVOutputGetCount()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return cOutputMaxCount;
}

HRESULT WINAPI SVOutputSetValue( unsigned long p_ulChannel, bool p_bValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	theApp.m_pTestIODlg->SetOutput( p_ulChannel, p_bValue );

	return S_OK;
}

unsigned long WINAPI SVOutputGetPortCount()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return 0UL;
}

HRESULT WINAPI SVOutputSetPortValue( unsigned long , unsigned long  )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return S_OK;
}

unsigned long WINAPI SVTriggerGetCount()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return cMaxTriggers;
}

unsigned long WINAPI SVTriggerGetHandle(unsigned long index)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return index + 1;
}

_variant_t WINAPI SVTriggerGetName( unsigned long triggerIndex)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	_variant_t result;

	if (0 < triggerIndex && triggerIndex <= cMaxTriggers)
	{
		std::string triggerName{ cTriggerName };
		triggerName += std::to_string(triggerIndex - 1);
		result.SetString(triggerName.c_str());
	} 

	return result;
}

HRESULT WINAPI SVTriggerRegister(unsigned long triggerIndex, SvTrig::TriggerCallBack pTriggerCallback)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (0 < triggerIndex && triggerIndex <= cMaxTriggers)
	{
		return theApp.m_pTestIODlg->RegisterCallback(triggerIndex, pTriggerCallback);
	} 

	return E_FAIL;
}

HRESULT WINAPI SVTriggerUnregister(unsigned long triggerIndex)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (0 < triggerIndex && triggerIndex <= cMaxTriggers)
	{
		return theApp.m_pTestIODlg->UnRegisterCallback(triggerIndex);
	} 

	return E_FAIL;
}

HRESULT WINAPI SVTriggerStart(unsigned long triggerIndex)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (0 < triggerIndex && triggerIndex <= cMaxTriggers)
	{
		return theApp.m_pTestIODlg->StartTrigger(triggerIndex);
	} 

	return E_FAIL;
}

HRESULT WINAPI SVTriggerStop(unsigned long triggerIndex)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (0 < triggerIndex && triggerIndex <= cMaxTriggers)
	{
		return theApp.m_pTestIODlg->StopTrigger(triggerIndex);
	} 

	return E_FAIL;
}

unsigned long WINAPI SVTriggerGetParameterCount(unsigned long )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return 0UL;
}

_variant_t WINAPI SVTriggerGetParameterName( unsigned long , unsigned long)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return _variant_t{};
}

_variant_t WINAPI SVTriggerGetParameterValue( unsigned long , unsigned long)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return _variant_t{};
}


HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerIndex, unsigned long, const _variant_t& )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (0 < triggerIndex && triggerIndex <= cMaxTriggers)
	{
		return S_OK;
	}

	return E_INVALIDARG;
}


