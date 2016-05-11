//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Rotation Angle Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

class SVEvaluateRotationAngleClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateRotationAngleClass )

public:
	SVEvaluateRotationAngleClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_ROTATION_ANGLE );
	virtual ~SVEvaluateRotationAngleClass();
};
