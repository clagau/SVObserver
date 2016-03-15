// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekBufferStruct
// * .File Name       : $Workfile:   SVIntekBufferStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:50:58  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIntekBufferStruct.h"
#include "SVHBitmapUtilitiesLibrary\SVImageFormatEnum.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
This constructor initializes all local attributes to desired defaults.
*/
SVIntekBufferStruct::SVIntekBufferStruct()
{
	m_ulWidth = 640;
	m_ulHeight = 480;

	m_iFormat = SVImageFormatUnknown;

	m_pBitmapInfo = NULL;
  m_pucImage = NULL;

	m_bIsLocked = false;

	m_CameraIndex = -1;

	m_StartTimeStamp = 0;
	m_EndTimeStamp = 0;
}

/*
This copy constructor uses the operator= method to copy the attributes from the provided object into the current one.
*/
SVIntekBufferStruct::SVIntekBufferStruct( const SVIntekBufferStruct &p_rValue )
{
	*this = p_rValue;
}

/*
This destructor is a placeholder and performs no actions at this time.
*/
SVIntekBufferStruct::~SVIntekBufferStruct()
{
}

/*
This operator copies all of the internal attributes from the provided object into the current one.
*/
const SVIntekBufferStruct &SVIntekBufferStruct::operator=( const SVIntekBufferStruct &p_rValue )
{
	m_ulWidth = p_rValue.m_ulWidth;
	m_ulHeight = p_rValue.m_ulHeight;

	m_iFormat = p_rValue.m_iFormat;

	m_pBitmapInfo = p_rValue.m_pBitmapInfo;
  m_pucImage = p_rValue.m_pucImage;

	m_bIsLocked = p_rValue.m_bIsLocked;

	m_CameraIndex = p_rValue.m_CameraIndex;

	m_StartTimeStamp = p_rValue.m_StartTimeStamp;
	m_EndTimeStamp = p_rValue.m_EndTimeStamp;

	return *this;
}

