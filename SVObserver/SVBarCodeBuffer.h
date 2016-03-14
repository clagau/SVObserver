//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBarCodeBuffer
//* .File Name       : $Workfile:   SVBarCodeBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:40:58  $
//******************************************************************************

#ifndef SVBARCODEBUFFER_H
#define SVBARCODEBUFFER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVGlobalHandles.h"

class SVBarCodeBufferInfoClass
{
public:
	SVBarCodeBufferInfoClass();
	SVBarCodeBufferInfoClass( const SVBarCodeBufferInfoClass& S2 );

	const SVBarCodeBufferInfoClass& operator=( const SVBarCodeBufferInfoClass& S2 );

public:
	SVBarCodeTypesEnum m_lCodeType;
	SVBarCodeBufferHandleStruct HBuffer;
};

#endif	//	SVBARCODEBUFFER_H

