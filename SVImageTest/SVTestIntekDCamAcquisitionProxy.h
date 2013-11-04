// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestIntekDCamAcquisitionProxy.h
// * .File Name       : $Workfile:   SVTestIntekDCamAcquisitionProxy.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:15:28  $
// ******************************************************************************

#ifndef INCL_SVTESTINTEKDCAMACQUISITIONPROXY_H
#define INCL_SVTESTINTEKDCAMACQUISITIONPROXY_H

#include "SV1394CameraFileLibrary/SVDCamAcquisitionProxy.h"

class SVTestAcquisitionClass;

class SVTestIntekDCamAcquisitionProxy : public SVDCamAcquisitionProxy
{
private:
	SVTestAcquisitionClass* m_pOwner;

public:
	SVTestIntekDCamAcquisitionProxy();
	virtual ~SVTestIntekDCamAcquisitionProxy();
	void SetOwner(SVTestAcquisitionClass * pOwner);

	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue );
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue );
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT SetDCAMParameter( const SVDeviceParamWrapper& rwParam );	// for parameters that are settable through MIL (all standard DCAM params)
	virtual bool CameraDeviceParamExists( SVDeviceParamEnum e );
	virtual const SVDeviceParamWrapper& GetCameraDeviceParam( SVDeviceParamEnum e ) const;
	virtual SVDeviceParamWrapper& GetCameraDeviceParamNonConst( SVDeviceParamEnum e );
	virtual HRESULT SetCameraDeviceParam( const SVDeviceParamWrapper& rw );

	virtual bool IsCameraOnline() const;
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVTestIntekDCamAcquisitionProxy.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:15:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2009 11:43:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
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
