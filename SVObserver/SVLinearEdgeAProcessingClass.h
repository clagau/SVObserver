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

#ifndef _SVLINEAREDGEAPROCESSINGCLASS_H
#define _SVLINEAREDGEAPROCESSINGCLASS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVLinearEdgeProcessingClass.h"

class SVLinearEdgeAProcessingClass : public SVLinearEdgeProcessingClass  
{
	SV_DECLARE_CLASS( SVLinearEdgeAProcessingClass );

public:
	SVLinearEdgeAProcessingClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEAPROCESSING );
	virtual ~SVLinearEdgeAProcessingClass();
};

#endif // _SVLINEAREDGEAPROCESSINGCLASS_H

