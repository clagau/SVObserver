// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekCameraStruct
// * .File Name       : $Workfile:   SVIntekCameraStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:52:00  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIntekCameraStruct.h"

#include "SVHBitmapUtilitiesLibrary\SVImageFormatEnum.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
This constructor initializes all local attributes to desired defaults.
*/
SVIntekCameraStruct::SVIntekCameraStruct()
: m_pCamera(NULL) 
, m_pBufferInterface(NULL)
, m_StartFrameTimeStamp(0)
, m_ulWidth(0)
, m_ulHeight(0)
, m_ucVideoFormat(0)
, m_ucVideoMode(0)
, m_ucFrameRate(0)
, m_ucColorFormat(0)
, m_usXPos(0)
, m_usYPos(0)
, m_usPktSize(0)
, m_iFormat(SVImageFormatUnknown)
, m_lIsStarted(0)
, m_ulRegisterAddress(0xF0F00800)
, m_DCamTriggerMode(TM_ONESHOT)
, m_ulSoftwareTriggerRegisterAddress(0x62C)
, m_ulSoftwareTriggerValue(0x80000000)
, m_NextCopyBuffer(0)
, m_NextProcessBuffer(0)
, mhThread(NULL)
, m_bAcquisitionTriggered(false)
, m_lineState(false)
{
	m_svCameraId.m_ui64CameraId = 0;
	mhShutdown = CreateEvent( NULL, TRUE, FALSE, NULL );
}

/*
This copy constructor uses the operator= method to copy the attributes from the provided object into the current one.
*/
SVIntekCameraStruct::SVIntekCameraStruct( const SVIntekCameraStruct &p_rValue)
{
	*this = p_rValue;

	mhShutdown = CreateEvent( NULL, TRUE, FALSE, NULL );

	mhThread = NULL;
}

/*
This destructor is a placeholder and performs no actions at this time.
*/
SVIntekCameraStruct::~SVIntekCameraStruct()
{
	if( mhShutdown != NULL )
	{
		::SetEvent( mhShutdown );
		::CloseHandle( mhShutdown );
		mhShutdown = NULL;
	}

	if( mhThread != NULL )
	{
		::CloseHandle( mhThread );
		mhThread = NULL;
	}
}

/*
This operator copies all of the internal attributes from the provided object into the current one.
*/
const SVIntekCameraStruct &SVIntekCameraStruct::operator=( const SVIntekCameraStruct &p_rValue )
{
	m_pCamera = p_rValue.m_pCamera;

	m_pBufferInterface = p_rValue.m_pBufferInterface;
	m_StartFrameTimeStamp = p_rValue.m_StartFrameTimeStamp;

	m_csName = (LPCTSTR)p_rValue.m_csName;
	m_csFullName = (LPCTSTR)p_rValue.m_csFullName;

	m_ulWidth = p_rValue.m_ulWidth;
	m_ulHeight = p_rValue.m_ulHeight;

	m_ucVideoFormat = p_rValue.m_ucVideoFormat;
	m_ucVideoMode = p_rValue.m_ucVideoMode;
	m_ucFrameRate = p_rValue.m_ucFrameRate;
	m_ucColorFormat = p_rValue.m_ucColorFormat;
	m_usXPos = p_rValue.m_usXPos;
	m_usYPos = p_rValue.m_usYPos;
	m_usPktSize = p_rValue.m_usPktSize;

	m_iFormat = p_rValue.m_iFormat;

	m_lIsStarted = p_rValue.m_lIsStarted;

	m_NextCopyBuffer = p_rValue.m_NextCopyBuffer;
	m_NextProcessBuffer = p_rValue.m_NextProcessBuffer;

	m_caBuffers.RemoveAll();
	m_caBuffers.Copy( p_rValue.m_caBuffers );

	m_ulRegisterAddress = p_rValue.m_ulRegisterAddress;

	m_svCameraId = p_rValue.m_svCameraId;

	m_DCamTriggerMode = p_rValue.m_DCamTriggerMode;
	m_ulSoftwareTriggerRegisterAddress = p_rValue.m_ulSoftwareTriggerRegisterAddress;
	m_ulSoftwareTriggerValue = p_rValue.m_ulSoftwareTriggerValue;

	m_bAcquisitionTriggered = p_rValue.m_bAcquisitionTriggered;
	m_lineState = p_rValue.m_lineState;

	return *this;
}

/*
This method is used to extract the vender identification number from the Camera Identification attribute.
*/
unsigned __int64 SVIntekCameraStruct::GetVenderID()
{
	unsigned __int64 l_ui64VenderId = m_svCameraId.m_ui64CameraId >> 40;

	return l_ui64VenderId;
}

/*
This method is used to extract the serial number from the Camera Identification attribute.
*/
unsigned __int64 SVIntekCameraStruct::GetSerialNumber()
{
	unsigned __int64 l_ui64SerialNumber = m_svCameraId.m_ui64CameraId & 0x000000FFFFFFFFFF;

	return l_ui64SerialNumber;
}

bool SVIntekCameraStruct::IsAcquisitionTriggered() const
{
	return m_bAcquisitionTriggered;
}

void SVIntekCameraStruct::SetAcquisitionTriggered(bool bAcquisitionTriggered)
{
	m_bAcquisitionTriggered = bAcquisitionTriggered;
}

bool SVIntekCameraStruct::GetLineState() const
{
	return m_lineState;
}

void SVIntekCameraStruct::SetLineState(bool bState)
{
	m_lineState = bState;
}

const SVCallbackStruct& SVIntekCameraStruct::GetTriggerCallback() const
{
	return m_triggerCallback;
}

void SVIntekCameraStruct::SetTriggerCallback(SVCallbackStruct& callback)
{
	m_triggerCallback = callback;
}

void SVIntekCameraStruct::ClearTriggerCallback()
{
	m_triggerCallback.m_pCallback = NULL;
	m_triggerCallback.m_pOwner = NULL;
	m_triggerCallback.m_pData = NULL;
}

