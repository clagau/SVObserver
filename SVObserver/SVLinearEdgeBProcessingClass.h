// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearEdgeBProcessingClass
// * .File Name       : $Workfile:   SVLinearEdgeBProcessingClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:53:26  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVLinearEdgeProcessingClass.h"
#pragma endregion Includes

class SVLinearEdgeBProcessingClass : public SVLinearEdgeProcessingClass  
{
	SV_DECLARE_CLASS( SVLinearEdgeBProcessingClass );

public:
	SVLinearEdgeBProcessingClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEBPROCESSING );
	virtual ~SVLinearEdgeBProcessingClass();
};

