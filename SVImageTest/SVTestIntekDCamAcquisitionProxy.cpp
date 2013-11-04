// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestIntekDcamAcquisitionProxy.cpp
// * .File Name       : $Workfile:   SVTestIntekDCamAcquisitionProxy.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:15:24  $
// ******************************************************************************

#include "stdafx.h"
#include "SVTestIntekDCamAcquisitionProxy.h"
#include "SVTestAcquisitionClass.h"

SVTestIntekDCamAcquisitionProxy::SVTestIntekDCamAcquisitionProxy()
{
	m_pOwner = NULL;
}

SVTestIntekDCamAcquisitionProxy::~SVTestIntekDCamAcquisitionProxy()
{
}

void SVTestIntekDCamAcquisitionProxy::SetOwner(SVTestAcquisitionClass* pOwner)
{
	ASSERT( pOwner != NULL );
	m_pOwner = pOwner;
}

HRESULT SVTestIntekDCamAcquisitionProxy::WriteCameraRegister( unsigned long ulAddress, unsigned long ulWriteValue )
{
	return m_pOwner->WriteCameraRegister( ulAddress, ulWriteValue );
}

HRESULT SVTestIntekDCamAcquisitionProxy::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )
{
	return m_pOwner->ReadCameraRegister( ulAddress, rulValue );
}

HRESULT SVTestIntekDCamAcquisitionProxy::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->WriteCameraRegisterBlock( ulAddress, ulCount, paulValue );
}

HRESULT SVTestIntekDCamAcquisitionProxy::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->ReadCameraRegisterBlock( ulAddress, ulCount, paulValue );
}

HRESULT SVTestIntekDCamAcquisitionProxy::SetDCAMParameter( const SVDeviceParamWrapper& rwParam )
{
	return m_pOwner->SetStandardCameraParameter( rwParam );
}

bool SVTestIntekDCamAcquisitionProxy::CameraDeviceParamExists( SVDeviceParamEnum e )
{
	return m_pOwner->m_DeviceParams.ParameterExists( e );
}

const SVDeviceParamWrapper& SVTestIntekDCamAcquisitionProxy::GetCameraDeviceParam( SVDeviceParamEnum e ) const
{
	return m_pOwner->m_DeviceParams.Parameter( e );
}

SVDeviceParamWrapper& SVTestIntekDCamAcquisitionProxy::GetCameraDeviceParamNonConst( SVDeviceParamEnum e )
{
	return m_pOwner->m_DeviceParams.GetParameter( e );
}

HRESULT SVTestIntekDCamAcquisitionProxy::SetCameraDeviceParam( const SVDeviceParamWrapper& rw )
{
	return m_pOwner->m_DeviceParams.SetParameter( rw );
}

bool SVTestIntekDCamAcquisitionProxy::IsCameraOnline() const
{
	return m_pOwner->IsOnline();
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVTestIntekDCamAcquisitionProxy.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:15:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Oct 2009 19:24:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated to handle const correctness issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 10:43:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
