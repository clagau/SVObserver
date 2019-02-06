//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for End Angle Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

namespace SvOp
{

class SVEvaluateEndAngleClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateEndAngleClass )

public:
	SVEvaluateEndAngleClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_END_ANGLE );
	virtual ~SVEvaluateEndAngleClass();
};

} //namespace SvOp
