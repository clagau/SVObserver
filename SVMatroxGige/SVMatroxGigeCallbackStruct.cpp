//******************************************************************************
//* COPYRIGHT (c) 2009 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCallbackStruct
//* .File Name       : $Workfile:   SVMatroxGigeCallbackStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:26  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVMatroxGigeCallbackStruct.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

SVMatroxGigeCallbackStruct::SVMatroxGigeCallbackStruct()
{
	m_ulDigitizerHandle = static_cast<unsigned long>(-1);
	m_ulBufferIndex = static_cast<unsigned long>(-1);
	m_StartFrameTimeStamp = 0;
}

SVMatroxGigeCallbackStruct::~SVMatroxGigeCallbackStruct()
{
}

