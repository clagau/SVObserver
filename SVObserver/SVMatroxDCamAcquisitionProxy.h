// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDCamAcquisitionProxy.h
// * .File Name       : $Workfile:   SVMatroxDCamAcquisitionProxy.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:31:22  $
// ******************************************************************************

#ifndef INCL_SVMATROXDCAMACQUISITIONPROXY_H
#define INCL_SVMATROXDCAMACQUISITIONPROXY_H

#include <SVDCamAcquisitionProxy.h>

class SVAcquisitionClass;

class SVMatroxDCamAcquisitionProxy : public SVDCamAcquisitionProxy
{
private:
	SVAcquisitionClass* m_pOwner;

public:
	SVMatroxDCamAcquisitionProxy();
	virtual ~SVMatroxDCamAcquisitionProxy();
	void SetOwner(SVAcquisitionClass* pOwner);

	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue );
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue );
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT SetDCAMParameter( const SVDeviceParamWrapper& rwParam );	// for parameters that are settable through MIL (all standard DCAM params)
	virtual bool CameraDeviceParamExists( SVDeviceParamEnum e );
	virtual const SVDeviceParamWrapper& GetCameraDeviceParam( SVDeviceParamEnum e );
	virtual SVDeviceParamWrapper& GetCameraDeviceParamNonConst( SVDeviceParamEnum e );
	virtual HRESULT SetCameraDeviceParam( const SVDeviceParamWrapper& rw );

	virtual bool IsCameraOnline();
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMatroxDCamAcquisitionProxy.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:31:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
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