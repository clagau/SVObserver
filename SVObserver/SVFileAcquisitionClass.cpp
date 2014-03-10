//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionClass 
//* .File Name       : $Workfile:   SVFileAcquisitionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   07 Mar 2014 18:14:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVFileAcquisitionClass.h"

#include "SVDataManagerLibrary/DataManager.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVOINIClass.h"

#include "SVDigitizerProcessingClass.h"
#include "SVImageObjectClass.h"
#include "SVImageProcessingClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

SVFileAcquisitionClass::SVFileAcquisitionClass( const SVAcquisitionConstructParams& p_rParams )
: SVAcquisitionClass( p_rParams )
{
	mbIsFilesLoaded = false;
}

SVFileAcquisitionClass::~SVFileAcquisitionClass()
{
	DestroyLocal();
}

bool SVFileAcquisitionClass::IsValid() const
{
	bool bOk = true;

	bOk = m_hDigitizer != NULL && bOk;
	bOk = SVAcquisitionClass::IsValid() && bOk;

	return bOk;
}

bool SVFileAcquisitionClass::IsValidBoard() const
{
	bool bOk = true;
	
	bOk = m_hDigitizer != NULL && bOk;

	return bOk;
}

HRESULT SVFileAcquisitionClass::Destroy()
{
	HRESULT hrOk = DestroyLocal();

	if ( SVAcquisitionClass::Destroy() != S_OK )
	{
		hrOk = S_FALSE;
	}
	return hrOk;
}

HRESULT SVFileAcquisitionClass::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	// Send these to the FileAcquisition device
	if ( IsDigitizerSubsystemValid() )
	{
		hr = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->SetParameters( m_hDigitizer, &rDeviceParams );
	}
	return hr;
}

HRESULT SVFileAcquisitionClass::LoadFiles(SVFileNameArrayClass& rFiles)
{
	HRESULT hrOk = S_OK;
	GetCameraImageInfo( &msvImageInfo );
	return hrOk;
}

HRESULT SVFileAcquisitionClass::UnloadFiles()
{
	HRESULT hrOk = S_OK;
	return hrOk;
}

HRESULT SVFileAcquisitionClass::GetLightReference( SVLightReference& rArray ) const
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::SetLightReference( SVLightReference& rArray, int iBand /* = -1 */)
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::CreateLightReference(int iBands, int iBrightness, int iContrast)
{
	HRESULT hrOk = S_FALSE;
	return hrOk;
}

HRESULT SVFileAcquisitionClass::CreateLightReferenceBand( int iBand, int iAttributes )
{
	HRESULT hrOk = S_FALSE;
	return hrOk;
}

HRESULT SVFileAcquisitionClass::ResetLightReference()
{
	HRESULT hrOk = S_FALSE;
	return hrOk;
}

HRESULT SVFileAcquisitionClass::GetImageInfo(SVImageInfoClass *pImageInfo) const
{
	HRESULT hrOk = S_FALSE;

	if ( pImageInfo != NULL )
	{
		*pImageInfo = msvImageInfo;
		
		hrOk = S_OK;
	}
	return hrOk;
}

HRESULT SVFileAcquisitionClass::DestroyLocal()
{
	HRESULT hrOk = S_OK;

	if ( Stop() != S_OK )
	{
		hrOk = S_FALSE;
	}

	if ( DestroyBuffers() != S_OK )
	{
		hrOk = S_FALSE;
	}

	if ( mbIsFilesLoaded )
	{
		if ( UnloadFiles() != S_OK )
		{
			hrOk = S_FALSE;
		}
	}

	if ( ResetLightReference() != S_OK )
	{
		hrOk = S_FALSE;
	}

	return hrOk;
}

HRESULT SVFileAcquisitionClass::GetLut( SVLut& lut )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::SetLut( const SVLutBand& lutBand )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::SetLut( const SVLut& lut, int iBand )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::ResetLut()
{
	HRESULT hr = S_FALSE;
	return hr;
}	

HRESULT SVFileAcquisitionClass::GetLutImpl( SVLut& lut )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT SVFileAcquisitionClass::GetCameraImageInfo(SVImageInfoClass *pImageInfo)
{
	HRESULT hrOk = S_FALSE;

	unsigned long bufWidth = 640;
	unsigned long bufHeight = 480;
	unsigned long uiBandNumber = 1;
	int iFormat = SVImageFormatUnknown;

	if ( IsValidBoard() && IsDigitizerSubsystemValid() )
	{
		SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetBufferHeight( m_hDigitizer, &bufHeight );
		SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetBufferWidth( m_hDigitizer, &bufWidth );
		SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetBufferFormat( m_hDigitizer, &iFormat );
	}// end if subsystem available
	else
	{
		// get camera format from DeviceParams
		const SVCameraFormatsDeviceParam* pParam = m_DeviceParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
		if ( pParam )
		{
			bufHeight = pParam->options.find( _T("default") )->second.lHeight;
			bufWidth = pParam->options.find( _T("default") )->second.lWidth;
			iFormat = pParam->options.find( _T("default") )->second.eImageType;
		}
	}

	// Band number depends on video type...
	switch( iFormat )
	{
		case SVImageFormatMono8:  // Mono
		{
			uiBandNumber = 1;
			break;
		}
		case SVImageFormatRGB8888:  // RGB
		{
			uiBandNumber = 3;
			break;
		}
	}

	pImageInfo->SetImageProperty( SVImagePropertyFormat, iFormat );
	pImageInfo->SetImageProperty( SVImagePropertyPixelDepth, 8 );
	pImageInfo->SetImageProperty( SVImagePropertyBandNumber, uiBandNumber );
	pImageInfo->SetImageProperty( SVImagePropertyBandLink, 0 );

	pImageInfo->SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0 );
	pImageInfo->SetExtentProperty( SVExtentPropertyWidth, bufWidth );
	pImageInfo->SetExtentProperty( SVExtentPropertyHeight, bufHeight );

	hrOk = S_OK;

	if ( hrOk != S_OK )
	{
		AfxMessageBox( "Failed to get acquisition source information!", MB_ICONEXCLAMATION );
	}

	return hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFileAcquisitionClass.cpp_v  $
 * 
 *    Rev 1.2   07 Mar 2014 18:14:42   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Added DEBUG_NEW.
 *   Made methods const.
 *   Removed methods that did not change base class functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:16:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   29 Apr 2013 14:12:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed header to modeless debug dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:31:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   09 Aug 2012 16:00:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added missing SetLUT over-ride method in file acquiaition class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   16 Sep 2011 15:55:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with extra consistancy checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Mar 2011 07:50:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Mar 2011 13:32:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 Feb 2011 14:39:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Nov 2010 13:35:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Feb 2010 12:29:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Dec 2009 10:24:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Sep 2009 10:07:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Aug 2009 15:07:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added removed SetDeviceParameters method that was remove in error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 11:36:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2008 10:23:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
