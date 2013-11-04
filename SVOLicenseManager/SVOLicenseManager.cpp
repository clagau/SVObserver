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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLicenseManager\SVOLicenseManager.cpp_v  $
 * 
 *    Rev 1.1   06 May 2013 14:15:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Apr 2013 14:24:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   - added the methods for the Fast OCR License
 * - new method RemoveLicenseErrorFromList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 12:40:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Mar 2013 12:37:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added the methods - AddLicenseErrorToList, HasToolErros and ClearLicenseErrors, added the use of a modeless dialog to display the license errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Mar 2013 08:44:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the error string text
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Feb 2013 09:56:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
 