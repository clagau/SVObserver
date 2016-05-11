//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for End Radius Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

class SVEvaluateEndRadiusClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateEndRadiusClass )

public:
	SVEvaluateEndRadiusClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_END_RADIUS );
	virtual ~SVEvaluateEndRadiusClass();
};