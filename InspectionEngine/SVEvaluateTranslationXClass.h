//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for X Translation Evaluation
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVEvaluate.h"
#pragma endregion Includes

class SVEvaluateTranslationXClass : public SVEvaluateClass
{
	SV_DECLARE_CLASS( SVEvaluateTranslationXClass )

public:
	SVEvaluateTranslationXClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVEVALUATE_TRANSLATION_X );
	virtual ~SVEvaluateTranslationXClass();
};