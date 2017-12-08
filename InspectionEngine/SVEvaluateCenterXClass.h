//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Center X Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

class SVEvaluateCenterXClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateCenterXClass )

public:
	SVEvaluateCenterXClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_CENTER_X );
	virtual ~SVEvaluateCenterXClass();
};
