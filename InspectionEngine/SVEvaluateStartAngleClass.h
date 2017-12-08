//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Start Angle Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

class SVEvaluateStartAngleClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateStartAngleClass )

public:
	SVEvaluateStartAngleClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_START_ANGLE );
	virtual ~SVEvaluateStartAngleClass();
};
