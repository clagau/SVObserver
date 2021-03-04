// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearEdgeAProcessingClass
// * .File Name       : $Workfile:   SVLinearEdgeAProcessingClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:53:06  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVLinearEdgeProcessingClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVLinearEdgeAProcessingClass : public SVLinearEdgeProcessingClass  
{
	SV_DECLARE_CLASS

public:
	SVLinearEdgeAProcessingClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEAPROCESSING );
	virtual ~SVLinearEdgeAProcessingClass();
};

} //namespace SvOp
