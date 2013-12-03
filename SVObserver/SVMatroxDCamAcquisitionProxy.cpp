// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDCamAcquisitionProxy.cpp
// * .File Name       : $Workfile:   SVMatroxDCamAcquisitionProxy.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:31:12  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxDCamAcquisitionProxy.h"
#include "SVAcquisitionClass.h"

SVMatroxDCamAcquisitionProxy::SVMatroxDCamAcquisitionProxy()
{
	m_pOwner = NULL;
}

SVMatroxDCamAcquisitionProxy::~SVMatroxDCamAcquisitionProxy()
{
}

void SVMatroxDCamAcquisitionProxy::SetOwner(SVAcquisitionClass* pOwner)
{
	ASSERT( pOwner != NULL );
	m_pOwner = pOwner;
}

HRESULT SVMatroxDCamAcquisitionProxy::WriteCameraRegister( unsigned long ulAddress, unsigned long ulWriteValue )
{
	return m_pOwner->WriteCameraRegister( ulAddress, ulWriteValue );
}

HRESULT SVMatroxDCamAcquisitionProxy::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )
{
	return m_pOwner->ReadCameraRegister( ulAddress, rulValue );
}

HRESULT SVMatroxDCamAcquisitionProxy::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->WriteCameraRegisterBlock( ulAddress, ulCount, paulValue );
}

HRESULT SVMatroxDCamAcquisitionProxy::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->ReadCameraRegisterBlock( ulAddress, ulCount, paulValue );
}

HRESULT SVMatroxDCamAcquisitionProxy::SetDCAMParameter( const SVDeviceParamWrapper& rwParam )
{
	return m_pOwner->SetStandardCameraParameter( rwParam );
}

bool SVMatroxDCamAcquisitionProxy::CameraDeviceParamExists( SVDeviceParamEnum e )
{
	return m_pOwner->m_DeviceParams.ParameterExists( e );
}

const SVDeviceParamWrapper& SVMatroxDCamAcquisitionProxy::GetCameraDeviceParam( SVDeviceParamEnum e )
{
	return m_pOwner->m_DeviceParams.Parameter( e );
}

SVDeviceParamWrapper& SVMatroxDCamAcquisitionProxy::GetCameraDeviceParamNonConst( SVDeviceParamEnum e )
{
	return m_pOwner->m_DeviceParams.GetParameter( e );
}

HRESULT SVMatroxDCamAcquisitionProxy::SetCameraDeviceParam( const SVDeviceParamWrapper& rw )
{
	return m_pOwner->m_DeviceParams.SetParameter( rw );
}

bool SVMatroxDCamAcquisitionProxy::IsCameraOnline()
{
	return m_pOwner->IsOnline();
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMatroxDCamAcquisitionProxy.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:31:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 10:36:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/