// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVTestIO.h"
#include "SVTestIODlg.h"
#include "SVTestIOExports.h"
#pragma endregion Includes

extern CSVTestIOApp theApp;
constexpr unsigned long cInputMaxCount = 8;
constexpr unsigned long cOutputMaxCount = 16;

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

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

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
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

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
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName && 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = S_OK;

		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = nullptr;
		}

		CString l_csName;
		
		l_csName.Format( "IO_Board_1.Dig_%d", triggerchannel - 1 );

		*p_pbstrName = l_csName.AllocSysString();
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerRegister( unsigned long triggerchannel, SvTi::TriggerCallBack pTriggerCallback)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (0 < triggerchannel && triggerchannel <= 4 )
	{
		return theApp.m_pTestIODlg->RegisterCallback(triggerchannel, pTriggerCallback);
	} 

	return E_FAIL;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long triggerchannel)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (0 < triggerchannel && triggerchannel <= 4 )
	{
		return theApp.m_pTestIODlg->UnRegisterCallback(triggerchannel);
	} 

	return E_FAIL;
}

HRESULT WINAPI SVTriggerStart( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = theApp.m_pTestIODlg->StartTrigger( triggerchannel );
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerStop( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = theApp.m_pTestIODlg->StopTrigger( triggerchannel );
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

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
			*p_pulCount = 4;
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetParameterName( unsigned long , unsigned long , BSTR *p_pbstrName )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

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
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

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


HRESULT WINAPI SVTriggerSetParameterValue( unsigned long triggerchannel, unsigned long, VARIANT *p_pvarValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel && triggerchannel <= 4 )
	{
		if ( nullptr != p_pvarValue )
		{
			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}


