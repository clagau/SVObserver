//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCallbackStruct
//* .File Name       : $Workfile:   SVMatroxGigeCallbackStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 15:22:46  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <deque>
#pragma endregion Includes

struct SVMatroxGigeCallbackStruct
{
public:
	SVMatroxGigeCallbackStruct();
	virtual ~SVMatroxGigeCallbackStruct();

	unsigned long m_ulDigitizerHandle;
	unsigned long m_ulBufferIndex;

	double m_StartFrameTimeStamp;

};

typedef std::deque<SVMatroxGigeCallbackStruct*> SVMatroxGigeCallbackStructPtrArray;

