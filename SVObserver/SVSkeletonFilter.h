// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVSkeletonFilter
// * .File Name       : $Workfile:   SVSkeletonFilter.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   13 Aug 2013 10:32:12  $
// ******************************************************************************

#ifndef SVSKELETONFILTERS_H
#define SVSKELETONFILTERS_H

#include "SVFilterClass.h"

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVSkeletonFilterClass
// -----------------------------------------------------------------------------
// .Description : Base class for all kinds of Skeleton filters
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
////////////////////////////////////////////////////////////////////////////////
class SVSkeletonFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVSkeletonFilterClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
// Standard constructor
	SVSkeletonFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVSKELETONFILTER );
	virtual ~SVSkeletonFilterClass();
	
private:

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );

//******************************************************************************
// Operator(s):
//******************************************************************************

public:

////////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:


//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
protected:

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
};

#endif // SVSKELETONFILTERS_H

