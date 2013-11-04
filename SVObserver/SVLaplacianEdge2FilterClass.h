//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVLaplacianEdge2FilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:55:10  $
//******************************************************************************

#ifndef SVLAPLACIANEDGE2FILTERCLASS_H
#define SVLAPLACIANEDGE2FILTERCLASS

#include "SVFilterClass.h"

class SVLaplacianEdge2FilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVLaplacianEdge2FilterClass );
public:
	SVLaplacianEdge2FilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVLAPLACIANEDGE2FILTER );

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};

#endif	//	SVLAPLACIANEDGE2FILTERCLASS

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLaplacianEdge2FilterClass.h_v  $
 * 
 *    Rev 1.0   09 Aug 2013 09:55:10   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
