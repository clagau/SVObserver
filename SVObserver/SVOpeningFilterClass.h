//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVOpeningFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:28:36  $
//******************************************************************************

#ifndef SVOPENINGFILTERCLASS_H
#define SVOPENINGFILTERCLASS_H

#include "SVFilterClass.h"

class SVOpeningFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVOpeningFilterClass );
public:
	SVOpeningFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVOPENINGFILTER );
protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};

#endif	//	SVOPERATOR_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOpeningFilterClass.h_v  $
 * 
 *    Rev 1.0   12 Aug 2013 16:28:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
