// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCIBufferStruct
// * .File Name       : $Workfile:   SVCIBufferStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:02  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCIBufferStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
This constructor initializes all local attributes to desired defaults.
*/
SVCIBufferStruct::SVCIBufferStruct()
{
	m_hCorServer = CORHANDLE_NULL;

	m_ulWidth = 640;
	m_ulHeight = 480;
	m_ulFormat = CORACQ_VAL_OUTPUT_FORMAT_MONO8;
	m_ulType = CORBUFFER_VAL_TYPE_SCATTER_GATHER;

	m_pAddress = NULL;
	m_Pitch = 0;

	m_hCorBuffer = CORHANDLE_NULL;

	m_StartTimeStamp = 0;
	m_EndTimeStamp = 0;

}

/*
This copy constructor uses the operator= method to copy the attributes from the provided object into the current one.
*/
SVCIBufferStruct::SVCIBufferStruct( const SVCIBufferStruct &p_rValue )
{
	*this = p_rValue;
}

/*
This destructor is a placeholder and performs no actions at this time.
*/
SVCIBufferStruct::~SVCIBufferStruct()
{
}

/*
This operator copies all of the internal attributes from the provided object into the current one.
*/
const SVCIBufferStruct &SVCIBufferStruct::operator=( const SVCIBufferStruct &p_rValue )
{
	m_hCorServer = p_rValue.m_hCorServer;

	m_ulWidth = p_rValue.m_ulWidth;
	m_ulHeight = p_rValue.m_ulHeight;
	m_ulFormat = p_rValue.m_ulFormat;
	m_ulType = p_rValue.m_ulType;

	m_pAddress = p_rValue.m_pAddress;
	m_Pitch = p_rValue.m_Pitch;

	m_hCorBuffer = p_rValue.m_hCorBuffer;

	m_StartTimeStamp = p_rValue.m_StartTimeStamp;
	m_EndTimeStamp = p_rValue.m_EndTimeStamp;

	return *this;
}

