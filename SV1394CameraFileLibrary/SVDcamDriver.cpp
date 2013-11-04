//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamDriver
//* .File Name       : $Workfile:   SVDcamDriver.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:36:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVDCAMAcquisitionProxy.h"
#include "SVDCamDriver.h"


SVDCamDriver::SVDCamDriver()
: m_pOwner( NULL )
{
}

SVDCamDriver::~SVDCamDriver()
{
	m_pOwner = NULL;
}

HRESULT SVDCamDriver::SetOwner( SVDCamAcquisitionProxy* pOwner )
{
	ASSERT( pOwner != NULL );
	m_pOwner = pOwner;
	return S_OK;
}

HRESULT SVDCamDriver::WriteCameraRegister( unsigned long ulAddress, unsigned long ulWriteValue )
{
	HRESULT hr = S_OK;

	#ifdef _DEBUG
		HRESULT hrRegister;
		unsigned long ulPreValue = -1;
		hrRegister = ReadCameraRegister( ulAddress, ulPreValue );
	#endif
	
	hr = m_pOwner->WriteCameraRegister( ulAddress, ulWriteValue );

	#ifdef _DEBUG
		unsigned long ulPostValue = -1;
		hrRegister = ReadCameraRegister( ulAddress, ulPostValue );
		hrRegister+=0;
	#endif

	return hr;
}

HRESULT SVDCamDriver::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )
{
	return m_pOwner->ReadCameraRegister( ulAddress, rulValue );
}

HRESULT SVDCamDriver::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->WriteCameraRegisterBlock( ulAddress, ulCount, paulValue );
}

HRESULT SVDCamDriver::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->ReadCameraRegisterBlock( ulAddress, ulCount, paulValue );
}


HRESULT SVDCamDriver::SetDCAMParameter( const SVDeviceParamWrapper& rwParam )	// for parameters that are settable through MIL (all standard DCAM params)
{
	return m_pOwner->SetDCAMParameter( rwParam );
}

bool SVDCamDriver::CameraDeviceParamExists( SVDeviceParamEnum e )
{
	return m_pOwner->CameraDeviceParamExists( e );
}

const SVDeviceParamWrapper& SVDCamDriver::GetCameraDeviceParam( SVDeviceParamEnum e ) const
{
	return m_pOwner->GetCameraDeviceParam( e );
}

SVDeviceParamWrapper& SVDCamDriver::GetCameraDeviceParamNonConst( SVDeviceParamEnum e )
{
	return m_pOwner->GetCameraDeviceParamNonConst( e );
}

HRESULT SVDCamDriver::SetCameraDeviceParam( const SVDeviceParamWrapper& rw )
{
	return m_pOwner->SetCameraDeviceParam( rw );
}

bool SVDCamDriver::IsCameraOnline() const
{
	return m_pOwner->IsCameraOnline();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SV1394CameraFileLibrary\SVDcamDriver.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:36:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Mar 2011 15:36:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construct functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Oct 2009 19:19:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Split Dragonfly and Flea classes apart and fixed issues with functionality and const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 14:56:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Nov 2005 15:42:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   25 May 2004 08:38:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  421
 * SCR Title:  Allow SVObserver to get the Trigger Edge and Strobe Edge from the Camera File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added three new mothods CameraDeviceParamExists, GetCameraDeviceParamNonConst, and SetCameraDeviceParam to allow the driver class to add or update the acquisition class camera parameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Oct 2003 08:22:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CanSoftwareTrigger and block register read/write
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Sep 2003 09:01:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Aug 2003 12:54:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed SetDCAMParameter call to SetStandardCameraParameter
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2003 08:44:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
