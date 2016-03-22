// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekCameraStruct
// * .File Name       : $Workfile:   SVIntekCameraStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 14:55:56  $
// ******************************************************************************

#pragma once

#include "SVContainerLibrary/SVVector.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVCCamera.h"
#include "SVIntekTimeStruct.h"
#include "SVIntekExports.h"
#include "SVIntekBufferStruct.h"

#include <map>

class SVAcquisitionBufferInterface;

/*
This union is defined to describe the data in the Intek header.
*/
union SVIntekCameraIdUnion
{
	//This attribute holds the identification number for the camera.
	unsigned __int64 m_ui64CameraId;

	struct
	{
		//This attribute holds the first part of the identification number for a camera.
		unsigned long m_ul040Ch;
		//This attribute holds the second part of the identification number for a camera.
		unsigned long m_ul0410h;
	};
};

/*
This class is the data class for each camera available for use.
*/
struct SVIntekCameraStruct  
{
public:
	SVIntekCameraStruct();
	SVIntekCameraStruct( const SVIntekCameraStruct &p_rValue );
	virtual ~SVIntekCameraStruct();

	const SVIntekCameraStruct &operator=( const SVIntekCameraStruct &p_rValue );

	unsigned __int64 GetVenderID();
	unsigned __int64 GetSerialNumber();

	bool IsAcquisitionTriggered() const;
	void SetAcquisitionTriggered(bool bAcquisitionTriggered);
	void SetLineState(bool bState);
	bool GetLineState() const;

	const SVCallbackStruct& GetTriggerCallback() const;
	void SetTriggerCallback(SVCallbackStruct& callback);
	void ClearTriggerCallback();

	//This attribute holds the pointer to the Intek camera class.
	CCamera *m_pCamera;

	// This attribute provides the interface to the buffer interface of the host application.
	SVAcquisitionBufferInterface* m_pBufferInterface;

	// This attribute holds the time stamp in milliseconds used to collate the acquisition buffer to the correct product data.
	SVClock::SVTimeStamp m_StartFrameTimeStamp;

	//This attribute holds the pointer to the digitizer name for this camera.
	CString m_csName;
	//This attribute holds the pointer to the full digitizer name for this camera.
	CString m_csFullName;

	//This attribute holds the current width of the image in pixels.
	unsigned long m_ulWidth;
	//This attribute holds the current height of the image in pixels.
	unsigned long m_ulHeight;

	//This attribute holds the current video format for the camera.
	unsigned char m_ucVideoFormat;
	//This attribute holds the current video mode for the camera.
	unsigned char m_ucVideoMode;
	//This attribute holds the current frame rate for the camera.
	unsigned char m_ucFrameRate;
	//This attribute holds the current color format used in setting the Format 7 parameters.
	unsigned char m_ucColorFormat;
	//This attribute holds the current horizontal(x) offset used in setting the Format 7 parameters.
	unsigned short m_usXPos;
	//This attribute holds the current vertical(y) offset used in setting the Format 7 parameters.
	unsigned short m_usYPos;
	//This attribute holds the current packet size used in setting the Format 7 parameters.
	unsigned short m_usPktSize;

	//This attribute holds the current format of the image.
	int m_iFormat;

	//This attribute holds the active state of the digitizer.
	long m_lIsStarted;

	//This attribute holds the index to the next copy buffer.
	long m_NextCopyBuffer;
	//This attribute holds the index to the next process buffer.
	long m_NextProcessBuffer;

	//This attribute contains the acquisition buffers associated with this digitizer.
	SVIntekBufferStructCArray m_caBuffers;

	//This attribute holds the current register address for getting and settting custom parameters.
	unsigned long m_ulRegisterAddress;

	//This attribute holds the connected camera information.
	SVIntekCameraIdUnion m_svCameraId;

	//This attribute holds the handle to the acquisition processing thread shutdown event.
	HANDLE mhShutdown;
	//This attribute holds the handle to the acquistion processing thread.
	HANDLE mhThread;

	//This attribute holds the software trigger methodology identifier.
	unsigned long m_DCamTriggerMode;
	//This attribute holds the IEEE1394 register address the software trigger.
	unsigned long m_ulSoftwareTriggerRegisterAddress;
	//This attribute holds the value required to activate the software trigger.
	unsigned long m_ulSoftwareTriggerValue;

	bool m_bAcquisitionTriggered;
	bool m_lineState; // if we can get this value...

	SVCallbackStruct m_triggerCallback;
};

/*
This class is the data class for each camera available for use.
*/
typedef SVVector< SVIntekCameraStruct > SVIntekCameraStructCArray;

