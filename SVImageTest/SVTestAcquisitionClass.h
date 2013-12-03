// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestAcquisitionClass.h
// * .File Name       : $Workfile:   SVTestAcquisitionClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   01 Oct 2013 08:25:48  $
// ******************************************************************************

#ifndef INCL_SVTESTACQUISITIONCLASS_H
#define INCL_SVTESTACQUISITIONCLASS_H

#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVTimerLibrary/SVClock.h"

class SVCameraPage;
class SVTestAcquisitionSubsystem;

class SVTestAcquisitionClass : public SVAcquisitionBufferInterface
{

public:
	SVTestAcquisitionSubsystem& m_rSubsystem;
	SVDeviceParamCollection m_DeviceParams;
	SVDeviceParamCollection m_CameraFileDeviceParams;
	SVCameraPage* m_pDisplay;

	SVTestAcquisitionClass( SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer );
	virtual ~SVTestAcquisitionClass();

	virtual SVClock::SVTimeStamp GetTimeStamp() const;
	
	virtual unsigned long GetBufferWidth() const;
	virtual unsigned long GetBufferHeight() const;
	virtual int GetBufferFormat() const;

	virtual HRESULT GetNextBuffer( SVImageBufferInterface& p_rBuffer );
	virtual HRESULT UpdateWithCompletedBuffer( const SVImageBufferInterface& p_rBuffer );

	virtual HRESULT ReadCameraFile( const CString& sFile );

	virtual HRESULT GetDeviceParameters( SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT GetCameraFileParameters( SVDeviceParamCollection& rDeviceParams );

	virtual bool CameraMatchesCameraFile();
	
	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue );
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue );
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw );

	virtual HRESULT InitializeDevice( const SVDeviceParamCollection& rParams );
	
	virtual bool IsOnline();
	virtual bool StartAcquire(SVCameraPage& p_rDisplay);
	virtual void StopAcquire();
	
	void FireSoftwareTrigger();

protected:
	unsigned long m_hDigitizer;
	bool m_bOnline;
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVTestAcquisitionClass.h_v  $
 * 
 *    Rev 1.2   01 Oct 2013 08:25:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add X64 Platform
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 14:51:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Apr 2013 12:40:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 11:10:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Dec 2009 11:43:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jul 2009 15:53:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Sep 2008 14:03:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added FireSoftwareTrigger method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Mar 2008 09:43:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  622
 * SCR Title:  Update SVImageTest Application to Acquire on Multiple Cameras Simultaneously
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added functionality for processing multiple camera acquisitions at the same time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.0   13 Jul 2009 15:40:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update image test program to use the new acquisition methodology for transferring the buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 10:43:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
