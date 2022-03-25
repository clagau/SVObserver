//******************************************************************************
/// COPYRIGHT (c) 2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Class for X Translation Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

namespace SvOp
{

class SVEvaluateTranslationXClass : public SVEvaluate
{
	SV_DECLARE_CLASS

public:
	SVEvaluateTranslationXClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_TRANSLATION_X );
	virtual ~SVEvaluateTranslationXClass();
};

} //namespace SvOp
