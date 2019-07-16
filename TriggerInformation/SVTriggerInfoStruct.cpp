//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file SVTriggerInfoStruct.cpp
/// All Rights Reserved
//*****************************************************************************
  
/// Contains the struct SVTriggerInfoStruct 
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTriggerInfoStruct.h"
#pragma endregion Includes

namespace SvTi
{
	void SVTriggerInfoStruct::Reset()
	{
		bValid			= false;
		lTriggerCount	= 0;
		m_ToggleState = false;
		m_BeginProcess	= 0;
		m_ToggleTimeStamp = 0;
		m_PreviousTrigger	= 0;
		m_Data.clear();
		m_Inputs.clear();
	}// end Reset
} //namespace SvTi
