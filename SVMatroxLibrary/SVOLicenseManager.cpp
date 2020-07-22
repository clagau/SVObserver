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
#include "SVMatroxLicenseInterface.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
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
	m_svStrErrorList.clear();

	SVMatroxLicenseInterface svMatroxLicense; 

	HRESULT hrRet = svMatroxLicense.InitMatroxLicense();

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


void SVOLicenseManager::AddLicenseErrorToList(const std::string& objectName)
{
	m_svStrErrorList.insert(objectName);
}

void SVOLicenseManager::RemoveLicenseErrorFromList(const std::string& objectName)
{
	auto iter = m_svStrErrorList.find(objectName);

	if (iter != m_svStrErrorList.end())
	{
		m_svStrErrorList.erase(objectName);
	}

	//update the dialog
	ShowLicenseManagerErrors();
}

void SVOLicenseManager::ClearLicenseErrors()
{
	if ( HasToolErrors() )
	{
		m_svStrErrorList.clear();

		if ( nullptr != m_hCheckEvent )
		{
			::CloseHandle( m_hCheckEvent );
			m_hCheckEvent = nullptr;
			SvOi::destroyLicenseManagerDialog();
		}
	} 
}

void SVOLicenseManager::ShowLicenseManagerErrors()
{
	if( nullptr != m_hCheckEvent )
	{
		::CloseHandle( m_hCheckEvent );
		m_hCheckEvent = nullptr;
		SvOi::destroyLicenseManagerDialog();

	}
	if ( HasToolErrors() )
	{
		m_hCheckEvent = ::CreateEvent( nullptr, true, false, nullptr );

		if ( nullptr != m_hCheckEvent )
		{
			switch( ::WaitForSingleObject( m_hCheckEvent, 0 ) )
			{
				case WAIT_TIMEOUT:
				{
					if( ::SetEvent( m_hCheckEvent ) )
					{
						std::string Temp( _T("The following tools are invalid because no Matrox Identification License was found") );
						SvOi::showLicenseManagerDialog(Temp, m_svStrErrorList, m_hCheckEvent);
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
	return !m_svStrErrorList.empty();
}
