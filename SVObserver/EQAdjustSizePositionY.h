//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
///Equation class for Formula for Windows y Position  for ToolsSize Adjust
//******************************************************************************
#pragma once
 
#pragma region Includes
#include "EQAdjustSize.h"
#pragma endregion Includes
 
//! \brief Equation class for Formula for Windows y Position  for ToolsSize Adjust
class EQAdjustSizePositionY : public EQAdjustSize
{

	SV_DECLARE_CLASS( EQAdjustSizePositionY);
public:
	EQAdjustSizePositionY( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_EQ_ADJUSTSIZE_POSITION_Y );
	virtual ~EQAdjustSizePositionY();
protected:
	 void init() ;
	
};

