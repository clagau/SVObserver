//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestGigeAcquisitionClass.h
//* .File Name       : $Workfile:   SVTestGigeAcquisitionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:10:10  $
//******************************************************************************

#ifndef SVTESTGIGEACQUISITIONCLASS_H
#define SVTESTGIGEACQUISITIONCLASS_H

#include "SVTestAcquisitionClass.h"
#include "SVTestGigeCameraProxy.h"

class SVTestGigeAcquisitionClass : public SVTestAcquisitionClass
{
private:
	SVTestGigeCameraProxy m_gigeCameraProxy;

	HRESULT SetGigeFeatureOverrides(const SVString& featureOverrides);

public:
	SVTestGigeAcquisitionClass(SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer);
	virtual ~SVTestGigeAcquisitionClass();

	virtual HRESULT ReadCameraFile( const CString& sFile );

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams );
	virtual bool CameraMatchesCameraFile();
	
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw );

	virtual bool StartAcquire(SVCameraPage& p_rDisplay);
	virtual void StopAcquire();
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVTestGigeAcquisitionClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:10:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jun 2010 08:07:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jul 2009 15:53:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 18:17:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
