//******************************************************************************
/// COPYRIGHT (c) 2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Class for Start Angle Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

namespace SvOp
{

class SVEvaluateStartAngleClass : public SVEvaluate
{
	SV_DECLARE_CLASS

public:
	SVEvaluateStartAngleClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_START_ANGLE );
	virtual ~SVEvaluateStartAngleClass();
};

} //namespace SvOp
