//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVLaplacianEdgeFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:55:38  $
//******************************************************************************

#ifndef SVLAPLACIANEDGEFILTERCLASS_H
#define SVLAPLACIANEDGEFILTERCLASS_H

#include "SVFilterClass.h"

class SVLaplacianEdgeFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVLaplacianEdgeFilterClass );
public:
	SVLaplacianEdgeFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVLAPLACIANEDGEFILTER );

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};

#endif	//	SVLAPLACIANEDGEFILTERCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLaplacianEdgeFilterClass.h_v  $
 * 
 *    Rev 1.0   09 Aug 2013 09:55:38   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
