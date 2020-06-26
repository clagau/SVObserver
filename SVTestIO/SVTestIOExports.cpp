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
#include "TriggerHandling/TriggerDispatcher.h"
#pragma endregion Includes

extern CSVTestIOApp theApp;

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

HRESULT WINAPI SVInputGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 8;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputGetValue( unsigned long p_ulChannel, bool *p_pbValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	if ( nullptr != p_pbValue )
	{
		*p_pbValue = (theApp.m_pTestIODlg->m_lInputs & (1 << p_ulChannel))== 0;
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputGetValues( unsigned long *p_pulValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulValue )
	{
		*p_pulValue = theApp.m_pTestIODlg->m_lInputs;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		*p_pulCount = 16;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputSetValue( unsigned long p_ulChannel, bool p_bValue )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	theApp.m_pTestIODlg->SetOutput( p_ulChannel, p_bValue );
	HRESULT l_hrOk = S_OK;;

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetPortCount( unsigned long *p_pulCount )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

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
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_OK;

	return l_hrOk;
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

HRESULT WINAPI SVTriggerRegister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( rDispatcher.hasCallback() && 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = theApp.m_pTestIODlg->AddDispatcher(triggerchannel, rDispatcher);
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( rDispatcher.hasCallback() && 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = theApp.m_pTestIODlg->RemoveDispatcher(triggerchannel, rDispatcher);
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregisterAll( unsigned long triggerchannel )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel && triggerchannel <= 4 )
	{
		l_hrOk = theApp.m_pTestIODlg->RemoveAllDispatchers(triggerchannel);
	} 

	return l_hrOk;
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


