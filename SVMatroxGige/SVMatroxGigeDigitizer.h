//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeDigitizer
//* .File Name       : $Workfile:   SVMatroxGigeDigitizer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 15:22:50  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <deque>
#include <stack>
#include <vector>
#include "SVGigeCameraParametersLibrary/SVGigeDeviceParameterStruct.h"
#include "SVMatroxLibrary/SVMatroxSystem.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxBufferArray.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizer.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerRef.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVMatroxGigeCallbackStruct.h"
#include "SVMatroxGigeCameraId.h"
#include "SVMatroxGigeAcqParams.h"
#include "SVMatroxGigeBufferCreator.h"
#include "SVMatroxGigeBufferManager.h"
#pragma endregion Includes

#pragma region Declarations
class SVAcquisitionBufferInterface;
#pragma endregion Declarations

class SVConditionVariable
{
private:
	volatile long m_value;

public:
	void Increment() { ::InterlockedIncrement(&m_value); }
	void clear() { ::InterlockedExchange(&m_value, 0); }
	long get() const { return m_value; }
};

class SVMatroxGigeDigitizer
{
private:
	typedef SVMatroxGigeBufferCreator<SVBufAttGrabImageProcDib> AcqBufferCreator;
	SVMatroxBufferArray m_AcqBuffers;
	SVGigeDeviceParameterMap m_featuresMap;
	unsigned long m_deviceNumber;
	SVMatroxString m_lineInputMoniker;
	SVMatroxString m_lineInputRisingEventName;
	SVMatroxString m_lineInputFallingEventName;
	bool m_lineState;
	SVGigeEventList m_eventList;

public:
#pragma region Constructor
	SVMatroxGigeDigitizer(unsigned long deviceNumber=-1, unsigned char systemHandle=0xFF);
	SVMatroxGigeDigitizer( const SVMatroxGigeDigitizer& p_rValue );
	virtual ~SVMatroxGigeDigitizer();
#pragma endregion Constructor

	const SVMatroxGigeDigitizer& operator=( const SVMatroxGigeDigitizer& p_rValue );

	// Buffer Management
	HRESULT CreateAcquisitionBuffers(const SVMatroxSystem& rSystem, unsigned long width, unsigned long height, unsigned long format);
	HRESULT DestroyAcquistionBuffers();

	HRESULT StartGrabArray(SVMatroxDigitizerInterface::SVGrabProcessFunc func);
	HRESULT StopGrabArray(SVMatroxDigitizerInterface::SVGrabProcessFunc func);

	HRESULT BuildGigeFeaturesMap(BSTR xmlData);
	const SVGigeDeviceParameterStruct& GetFeature(SVGigeParameterEnum paramEnum) const;

	unsigned long GetDeviceNumber() const;

	bool HasLineInput() const;
	void SetLineInputMoniker(const SVString& moniker);
	SVMatroxString GetLineInputRisingEventName() const;
	SVMatroxString GetLineInputFallingEventName() const;

	bool IsAcquisitionTriggered() const;
	void SetAcquisitionTriggered(bool bAcquisitionTriggered);

	bool IsGigeEventListValid() const;
	HRESULT GetGigeEventList();
	long GetLineRisingEvent() const;
	long GetLineFallingEvent() const;

	void SetLineState(bool bState);
	bool GetLineState() const;

	const SVCallbackStruct& GetTriggerCallback() const;
	void SetTriggerCallback(SVCallbackStruct& callback);
	void ClearTriggerCallback();

#pragma region Member Variables
public:
	SVString m_Name;
	SVString m_FullName;

	unsigned char m_SystemHandle;
	unsigned char m_Handle;

	long m_lIsStarted; // long (not bool) because of InterlockedExchange...

	// Image processing buffers in use
	long m_lStartIndex;
	long m_lLastUsedIndex;

	SVConditionVariable m_frameStack;
	SVMatroxDigitizerRef m_Digitizer;
	SVMatroxGigeCameraId m_svCameraId;

	SVClock::SVTimeStamp m_StartFrameTimeStamp;

	SVMatroxGigeAcqParams m_params;

	SVAcquisitionBufferInterface* m_pBufferInterface;

private:
	SVCallbackStruct m_triggerCallback;
#pragma endregion Member Variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVMatroxGige\SVMatroxGigeDigitizer.h_v  $
 * 
 *    Rev 1.1   08 May 2013 15:22:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Apr 2013 13:04:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 14:44:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jan 2013 09:59:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for trigger interface
 * Revised to support Acquisition Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jun 2012 16:43:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jun 2010 08:01:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 07:50:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new buffer management and buffer transfer methodologies.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:19:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/