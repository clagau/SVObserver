// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekDcamAcquisitionProxy.cpp
// * .File Name       : $Workfile:   SVIntekDCamAcquisitionProxy.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:05:22  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIntekDCamAcquisitionProxy.h"
#include "SVAcquisitionClass.h"
#include "SVDigitizerProcessingClass.h"

SVIntekDCamAcquisitionProxy::SVIntekDCamAcquisitionProxy()
{
	m_pOwner = NULL;
}

SVIntekDCamAcquisitionProxy::~SVIntekDCamAcquisitionProxy()
{
}

void SVIntekDCamAcquisitionProxy::SetOwner( LPCTSTR p_szOwnerName )
{
	m_pOwner = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( p_szOwnerName );
}

HRESULT SVIntekDCamAcquisitionProxy::WriteCameraRegister( unsigned long ulAddress, unsigned long ulWriteValue )
{
	return m_pOwner->WriteCameraRegister( ulAddress, ulWriteValue );
}

HRESULT SVIntekDCamAcquisitionProxy::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )
{
	return m_pOwner->ReadCameraRegister( ulAddress, rulValue );
}

HRESULT SVIntekDCamAcquisitionProxy::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->WriteCameraRegisterBlock( ulAddress, ulCount, paulValue );
}

HRESULT SVIntekDCamAcquisitionProxy::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->ReadCameraRegisterBlock( ulAddress, ulCount, paulValue );
}

HRESULT SVIntekDCamAcquisitionProxy::SetDCAMParameter( const SVDeviceParamWrapper& rwParam )
{
	return m_pOwner->SetStandardCameraParameter( rwParam );
}

bool SVIntekDCamAcquisitionProxy::CameraDeviceParamExists( SVDeviceParamEnum e )
{
	return m_pOwner->m_DeviceParams.ParameterExists( e );
}

const SVDeviceParamWrapper& SVIntekDCamAcquisitionProxy::GetCameraDeviceParam( SVDeviceParamEnum e ) const
{
	return m_pOwner->m_DeviceParams.Parameter( e );
}

SVDeviceParamWrapper& SVIntekDCamAcquisitionProxy::GetCameraDeviceParamNonConst( SVDeviceParamEnum e )
{
	return m_pOwner->m_DeviceParams.GetParameter( e );
}

HRESULT SVIntekDCamAcquisitionProxy::SetCameraDeviceParam( const SVDeviceParamWrapper& rw )
{
	return m_pOwner->m_DeviceParams.SetParameter( rw );
}

bool SVIntekDCamAcquisitionProxy::IsCameraOnline() const
{
	return m_pOwner->IsOnline();
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIntekDCamAcquisitionProxy.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:05:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Mar 2011 16:04:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Mar 2011 07:54:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Oct 2009 19:30:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 10:36:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
