//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Center Y Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

namespace SvOp
{

class SVEvaluateCenterYClass : public SVEvaluate
{
	SV_DECLARE_CLASS( SVEvaluateCenterYClass )

public:
	SVEvaluateCenterYClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_CENTER_Y );
	virtual ~SVEvaluateCenterYClass();
};

} //namespace SvOp
