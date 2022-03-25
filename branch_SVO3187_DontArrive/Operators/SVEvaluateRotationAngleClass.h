//******************************************************************************
/// COPYRIGHT (c) 2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Class for Rotation Angle Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

namespace SvOp
{

class SVEvaluateRotationAngleClass : public SVEvaluate
{
	SV_DECLARE_CLASS

public:
	SVEvaluateRotationAngleClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_ROTATION_ANGLE );
	virtual ~SVEvaluateRotationAngleClass();
};

} //namespace SvOp
