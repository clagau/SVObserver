// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestIntekAcquisitionClass.h
// * .File Name       : $Workfile:   SVTestIntekAcquisitionClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:13:18  $
// ******************************************************************************

#ifndef INCL_SVTESTINTEKACQUISITIONCLASS_H
#define INCL_SVTESTINTEKACQUISITIONCLASS_H

#include "SVTestAcquisitionClass.h"
#include "SVTestIntekDCamDriverProxy.h"
#include "SVTestIntekDCamAcquisitionProxy.h"

class SVTestIntekAcquisitionClass : public SVTestAcquisitionClass
{
private:
	SVTestIntekDCamDriverProxy m_dcamDriverProxy;
	SVTestIntekDCamAcquisitionProxy m_dcamAcquisitionProxy;
	SVDCamDriver* m_pCameraDriver;

public:
	SVTestIntekAcquisitionClass(SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer);
	virtual ~SVTestIntekAcquisitionClass();

	virtual HRESULT ReadCameraFile( const CString& sFile );

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams );
	virtual bool CameraMatchesCameraFile();
	
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue );
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue );
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw );

	virtual bool StartAcquire( SVCameraPage& p_rDisplay );
	virtual void StopAcquire();

private:
	void DestroyDCamDriver();
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVTestIntekAcquisitionClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:13:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Jul 2009 15:53:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Mar 2008 09:43:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  622
 * SCR Title:  Update SVImageTest Application to Acquire on Multiple Cameras Simultaneously
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added functionality for processing multiple camera acquisitions at the same time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.0   13 Jul 2009 15:40:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update image test program to use the new acquisition methodology for transferring the buffers.
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
