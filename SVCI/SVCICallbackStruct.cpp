// ******************************************************************************
// * COPYRIGHT (c) 2009 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCICallbackStruct
// * .File Name       : $Workfile:   SVCICallbackStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:59:04  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCICallbackStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
This constructor initializes all local attributes to desired defaults.
*/
SVCICallbackStruct::SVCICallbackStruct()
: m_ulDigitizerIndex( -1 )
, m_ulBufferIndex( -1 )
, m_IsTrash( false )
{
}

/*
This destructor is a placeholder and performs no actions at this time.
*/
SVCICallbackStruct::~SVCICallbackStruct()
{
}

