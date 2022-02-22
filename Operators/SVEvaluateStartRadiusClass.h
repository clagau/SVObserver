//******************************************************************************
/// COPYRIGHT (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Class for Start Radius Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

namespace SvOp
{

class SVEvaluateStartRadiusClass : public SVEvaluate
{
	SV_DECLARE_CLASS

public:
	SVEvaluateStartRadiusClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_START_RADIUS );
	virtual ~SVEvaluateStartRadiusClass();
};

} //namespace SvOp
