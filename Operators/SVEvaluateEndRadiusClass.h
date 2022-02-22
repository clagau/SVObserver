//******************************************************************************
/// COPYRIGHT (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Class for End Radius Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

namespace SvOp
{

class SVEvaluateEndRadiusClass : public SVEvaluate
{
	SV_DECLARE_CLASS

public:
	SVEvaluateEndRadiusClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_END_RADIUS );
	virtual ~SVEvaluateEndRadiusClass();
};

} //namespace SvOp
