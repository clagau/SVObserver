//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeBuffer
//* .File Name       : $Workfile:   SVBarCodeBuffer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:40:50  $
//******************************************************************************

#include "stdafx.h"
#include "SVBarCodeBuffer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVBarCodeBufferInfoClass::SVBarCodeBufferInfoClass()
{
	m_lCodeType = SVDataMatrix;
}


SVBarCodeBufferInfoClass::SVBarCodeBufferInfoClass( const SVBarCodeBufferInfoClass& S2 )
{
	m_lCodeType = SVDataMatrix;

	*this = S2;
}

//******************************************************************************
// Operator(s):
//******************************************************************************

const SVBarCodeBufferInfoClass &SVBarCodeBufferInfoClass::operator=( const SVBarCodeBufferInfoClass& S2 )
{
	m_lCodeType = S2.m_lCodeType;
	HBuffer	= S2.HBuffer;

	return ( *this );
}

