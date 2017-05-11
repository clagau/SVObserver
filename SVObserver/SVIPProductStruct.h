//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPProductStruct
//* .File Name       : $Workfile:   SVIPProductStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:17:34  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVIPImageDataElement.h"
#include "SVOCore/SVIPResultData.h"
#pragma endregion Includes

struct SVIPProductStruct
{
	unsigned long m_TriggerCount;
	SVIPImageDataElementMap m_ImageData;
	SVIPResultData m_ResultData;

	SVIPProductStruct() : m_TriggerCount( 0 ), m_ImageData(), m_ResultData() {}

	void clear() { m_TriggerCount = 0; m_ImageData.clear(); m_ResultData.clear(); }
};


