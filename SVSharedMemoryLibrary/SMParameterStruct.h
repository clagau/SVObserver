//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file CreateParam.h
/// All Rights Reserved 
//*****************************************************************************
/// struct encapsulate Configuration Paramater for Create Shared Memory
//******************************************************************************
#pragma once
namespace SvSml 
{
	struct  SMParameterStruct
	{
		SMParameterStruct(int TimeOut, int WaitTime)
		{
			
			if (TimeOut > 0 && WaitTime > 0)
			{
				SMCreateTimeout = TimeOut;
				SMCreateWaitTime = WaitTime;
			}
			else
			{
				SMCreateTimeout = 0;
				SMCreateWaitTime = 1;
			}

		}
		int SMCreateTimeout;
		int SMCreateWaitTime;
	};
}
