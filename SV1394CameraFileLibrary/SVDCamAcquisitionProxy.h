// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDCamAcquisitionProxy.h
// * .File Name       : $Workfile:   SVDCamAcquisitionProxy.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:32:58  $
// ******************************************************************************

#ifndef INCL_SVDCAMACQUISITIONPROXY_H
#define INCL_SVDCAMACQUISITIONPROXY_H

#include "SVOMFCLibrary/SVDeviceParamCollection.h"

/////////////////////////////////////////////////////////////////////////////////
// This class decouples the SVAcquistion class from the SVDCamDriver classes
// so that SVImageTest can use the SVDCamDrivers and load the camera files
/////////////////////////////////////////////////////////////////////////////////
class SVDCamAcquisitionProxy
{
public:
	SVDCamAcquisitionProxy() {}
	virtual ~SVDCamAcquisitionProxy() {}

	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue )=0;
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )=0;
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )=0;
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )=0;
	virtual HRESULT SetDCAMParameter( const SVDeviceParamWrapper& rwParam )=0;	// for parameters that are settable through MIL (all standard DCAM params)
	virtual bool CameraDeviceParamExists( SVDeviceParamEnum e )=0;
	virtual const SVDeviceParamWrapper& GetCameraDeviceParam( SVDeviceParamEnum e ) const=0;
	virtual SVDeviceParamWrapper& GetCameraDeviceParamNonConst( SVDeviceParamEnum e )=0;
	virtual HRESULT SetCameraDeviceParam( const SVDeviceParamWrapper& rw )=0;

	virtual bool IsCameraOnline() const=0;
};
#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SV1394CameraFileLibrary\SVDCamAcquisitionProxy.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:32:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2009 11:13:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
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
 *    Rev 1.0   29 Nov 2006 14:55:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
