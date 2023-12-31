//******************************************************************************
//* COPYRIGHT (c) 2009 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeBufferStruct
//* .File Name       : $Workfile:   SVMatroxGigeBufferStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:41:22  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <memory>
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#pragma endregion Includes

typedef std::shared_ptr<SVMatroxBuffer> SVMatroxBufferRef;

struct SVMatroxGigeBufferStruct
{
public:
	SVMatroxGigeBufferStruct();
	SVMatroxGigeBufferStruct( const SVMatroxGigeBufferStruct& p_rValue );
	virtual ~SVMatroxGigeBufferStruct();

	const SVMatroxGigeBufferStruct& operator=( const SVMatroxGigeBufferStruct& p_rValue );

	bool IsValid() const;

	unsigned long m_ulWidth;
	unsigned long m_ulHeight;
	unsigned long m_ulFormat;
	unsigned long m_ulAddress;
	bool m_bIsLocked;

	SVMatroxBufferRef m_Buffer;
};

typedef std::deque<SVMatroxGigeBufferStruct> SVMatroxGigeBufferStructArray;

