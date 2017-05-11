//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Start Radius Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

class SVEvaluateStartRadiusClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateStartRadiusClass )

public:
	SVEvaluateStartRadiusClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_START_RADIUS );
	virtual ~SVEvaluateStartRadiusClass();
};