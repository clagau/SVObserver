//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Y Rotation Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

namespace SvOp
{

class SVEvaluateRotationYClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateRotationYClass )
public:
	SVEvaluateRotationYClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_ROTATION_Y );
	virtual ~SVEvaluateRotationYClass();
};

} //namespace SvOp
