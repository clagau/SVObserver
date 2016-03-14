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

#include "StdAfx.h"
#include "SVOLicenseManager.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMatroxLibrary/SVMatroxLicenseInterface.h"

#include "SVObserver/SVLicenseMgrModelessDlg.h"

SVOLicenseManager& SVOLicenseManager::Instance()
{
	static SVOLicenseManager l_licenseMgr;
	return l_licenseMgr;
}

SVOLicenseManager::SVOLicenseManager(void)
{
	m_bMatroxImageLicense = false;
	m_bMatroxIdentificationLicense = false;
	m_bMatroxGigELicense = false;
	m_hCheckEvent = NULL;
	//Init Fast OCR License
	m_bFastOCR = false;
}

SVOLicenseManager::~SVOLicenseManager(void)
{
	if( m_hCheckEvent != NULL )
	{
		::CloseHandle( m_hCheckEvent );
		m_hCheckEvent = NULL;
	}
}

HRESULT SVOLicenseManager::InitLicenseManager()
{
	HRESULT hrRet = S_OK;

	m_svErrorList.clear();

	SVMatroxLicenseInterface svMatroxLicense; 

	hrRet = svMatroxLicense.InitMatroxLicense();

	if ( hrRet == S_OK )
	{
		m_bMatroxImageLicense = svMatroxLicense.HasMatroxLicense();
		m_bMatroxIdentificationLicense  = svMatroxLicense.HasMatroxIdentificationLicense();
		m_bMatroxGigELicense = svMatroxLicense.HasMatroxGigELicense();
	}

	return hrRet;
}

bool SVOLicenseManager::HasMatroxGigELicense()
{
	return m_bMatroxGigELicense;
}

bool SVOLicenseManager::HasMatroxIdentificationLicense()
{
	return m_bMatroxIdentificationLicense;
}

bool SVOLicenseManager::HasMatroxLicense()
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

		if ( m_hCheckEvent != NULL )
		{
			::CloseHandle( m_hCheckEvent );
			m_hCheckEvent = NULL;
			SVLicenseMgrModelessDlg::Destroy();
		}
	} 
}


void SVOLicenseManager::ShowLicenseManagerErrors()
{
	if( m_hCheckEvent != NULL )
	{
		::CloseHandle( m_hCheckEvent );
		m_hCheckEvent = NULL;
		SVLicenseMgrModelessDlg::Destroy();

	}
	if ( HasToolErrors() )
	{
		m_hCheckEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );

		CString sTmp = "The following tools are invalid because no Matrox Identification License was found";

		if ( m_hCheckEvent != NULL )
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

bool SVOLicenseManager::HasToolErrors()
{
	return !m_svErrorList.empty();
}

void SVOLicenseManager::SetFastOCRLicense(bool bPresent)
{
	m_bFastOCR = bPresent;
}

bool SVOLicenseManager::HasFastOCRLicense()
{
	return m_bFastOCR;
}

