//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVActiveXLockStruct
//* .File Name       : $Workfile:   SVActiveXLockStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:26:50  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

const int NUM_ACTIVEX_LOCKS = 20;

struct SVActiveXLockStruct
{
	SVActiveXLockStruct();
	virtual ~SVActiveXLockStruct();

	SVActiveXLockStruct(const SVActiveXLockStruct &rvalue);
	const SVActiveXLockStruct & operator=(const SVActiveXLockStruct &rvalue);

	void clear();

	BOOL Valid();

	SVString m_Name;
	long m_ProcessCount;

	SVSmartHandlePointer m_ImageHandlePtr;
};

