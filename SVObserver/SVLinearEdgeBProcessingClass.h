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

#ifndef _SVLINEAREDGEBPROCESSINGCLASS_H
#define _SVLINEAREDGEBPROCESSINGCLASS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVLinearEdgeProcessingClass.h"

class SVLinearEdgeBProcessingClass : public SVLinearEdgeProcessingClass  
{
	SV_DECLARE_CLASS( SVLinearEdgeBProcessingClass );

public:
	SVLinearEdgeBProcessingClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEAREDGEBPROCESSING );
	virtual ~SVLinearEdgeBProcessingClass();
};

#endif // _SVLINEAREDGEBPROCESSINGCLASS_H

