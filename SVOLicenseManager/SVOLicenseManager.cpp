//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOLicenseManager
//* .File Name       : $Workfile:   SVOLicenseManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 May 2013 14:15:46  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVOLicenseManager.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMatroxLibrary/SVMatroxLicenseInterface.h"
#include "SVObserver/SVLicenseMgrModelessDlg.h"
#pragma endregion Includes

SVOLicenseManager& SVOLicenseManager::Instance()
{
	static SVOLicenseManager l_licenseMgr;
	return l_licenseMgr;
}

SVOLicenseManager::SVOLicenseManager()
:m_bMatroxImageLicense(false)
, m_bMatroxIdentificationLicense(false)
, m_bMatroxGigELicense(false)
, m_hCheckEvent(nullptr)
{
}

SVOLicenseManager::~SVOLicenseManager()
{
	if( nullptr != m_hCheckEvent )
	{
		::CloseHandle( m_hCheckEvent );
		m_hCheckEvent = nullptr;
	}
}

HRESULT SVOLicenseManager::InitLicenseManager()
{
	HRESULT hrRet = S_OK;

	m_svErrorList.clear();

	SVMatroxLicenseInterface svMatroxLicense; 

	hrRet = svMatroxLicense.InitMatroxLicense();

	if ( S_OK == hrRet )
	{
		m_bMatroxImageLicense = svMatroxLicense.HasMatroxLicense();
		m_bMatroxIdentificationLicense  = svMatroxLicense.HasMatroxIdentificationLicense();
		m_bMatroxGigELicense = svMatroxLicense.HasMatroxGigELicense();
	}

	return hrRet;
}

bool SVOLicenseManager::HasMatroxGigELicense() const
{
	return m_bMatroxGigELicense;
}

bool SVOLicenseManager::HasMatroxIdentificationLicense()  const
{
	return m_bMatroxIdentificationLicense;
}

bool SVOLicenseManager::HasMatroxLicense()  const
{
	return m_bMatroxImageLicense;
}

void SVOLicenseManager::AddLicenseErrorToList(const SVGUID& svGuid)
{
	m_svErrorList.insert(svGuid);
}

void SVOLicenseManager::RemoveLicenseErrorFromList(const SVGUID& svGuid)
{
	SVLicenseErrorListSet::iterator iter;

	iter = m_svErrorList.find(svGuid);

	if ( iter != m_svErrorList.end() )
	{
		m_svErrorList.erase(svGuid);
	}

	//update the dialog
	ShowLicenseManagerErrors();
}

void SVOLicenseManager::ClearLicenseErrors()
{
	if ( HasToolErrors() )
	{
		m_svErrorList.clear();

		if ( nullptr != m_hCheckEvent )
		{
			::CloseHandle( m_hCheckEvent );
			m_hCheckEvent = nullptr;
			SVLicenseMgrModelessDlg::Destroy();
		}
	} 
}

void SVOLicenseManager::ShowLicenseManagerErrors()
{
	if( nullptr != m_hCheckEvent )
	{
		::CloseHandle( m_hCheckEvent );
		m_hCheckEvent = nullptr;
		SVLicenseMgrModelessDlg::Destroy();

	}
	if ( HasToolErrors() )
	{
		m_hCheckEvent = ::CreateEvent( nullptr, true, false, nullptr );

		CString sTmp = "The following tools are invalid because no Matrox Identification License was found";

		if ( nullptr != m_hCheckEvent )
		{
			switch( ::WaitForSingleObject( m_hCheckEvent, 0 ) )
			{
				case WAIT_TIMEOUT:
				{
					if( ::SetEvent( m_hCheckEvent ) )
					{
						SVLicenseMgrModelessDlg::Show(sTmp,m_svErrorList,m_hCheckEvent);
					}
				}
				case WAIT_OBJECT_0:
				{
					break;
				}
				default:
				{
					break;
				}
			}
		}
	} 
}

bool SVOLicenseManager::HasToolErrors() const
{
	return !m_svErrorList.empty();
}
