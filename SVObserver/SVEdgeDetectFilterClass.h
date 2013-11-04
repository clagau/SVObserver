//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVEdgeDetectFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:53:14  $
//******************************************************************************

#ifndef SVEDGEDETECTFILTERCLASS_H
#define SVEDGEDETECTFILTERCLASS_H

#include "SVFilterClass.h"

class SVEdgeDetectFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVEdgeDetectFilterClass );
public:
	SVEdgeDetectFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVEDGEDETECTFILTER );

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};

#endif	//	SVEDGEDETECTFILTERCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVEdgeDetectFilterClass.h_v  $
 * 
 *    Rev 1.0   09 Aug 2013 09:53:14   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
