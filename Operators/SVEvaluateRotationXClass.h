//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for X Rotation Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

namespace SvOp
{

class SVEvaluateRotationXClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateRotationXClass )

public:
	SVEvaluateRotationXClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_ROTATION_X );
	virtual ~SVEvaluateRotationXClass();
};

} //namespace SvOp
