//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeBufferCreator
//* .File Name       : $Workfile:   SVMatroxGigeBufferCreator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:39:06  $
//******************************************************************************

#ifndef SVMATROXGIGEBUFFERCREATOR_H
#define SVMATROXGIGEBUFFERCREATOR_H

#include "SVMatroxLibrary/SVMatroxSystem.h"
#include "SVMatroxLibrary/SVMatroxEnums.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVMatroxGigeBufferStruct.h"

template <SVMatroxBufferAttributeEnum Attribute>
class SVMatroxGigeBufferCreator
{
public:
	SVMatroxGigeBufferCreator();
	~SVMatroxGigeBufferCreator();

	HRESULT Create(const SVMatroxSystem& rSystem, unsigned long p_ulWidth, unsigned long p_ulHeight, unsigned long p_ulFormat, SVMatroxGigeBufferStruct& rBuffer);
	HRESULT Destroy(SVMatroxGigeBufferStruct& rBuffer);

	HRESULT BuildCreateStruct(unsigned long p_ulWidth, unsigned long p_ulHeight, unsigned long p_ulFormat, SVMatroxBufferCreateStruct& rCreateStruct);
};

#include "SVMatroxGigeBufferCreator.inl"

#endif

