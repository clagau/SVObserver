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

#include "SVLinearEdgeProcessingClass.h"

class SVLinearEdgeAProcessingClass : public SVLinearEdgeProcessingClass  
{
	SV_DECLARE_CLASS( SVLinearEdgeAProcessingClass );

public:
	SVLinearEdgeAProcessingClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEAPROCESSING );
	virtual ~SVLinearEdgeAProcessingClass();
};

