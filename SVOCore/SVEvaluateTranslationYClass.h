//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for Y Translation Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

class SVEvaluateTranslationYClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateTranslationYClass )

public:
	SVEvaluateTranslationYClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_TRANSLATION_Y );
	virtual ~SVEvaluateTranslationYClass();
};