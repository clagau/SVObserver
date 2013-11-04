//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVSmoothFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 11:09:04  $
//******************************************************************************

#ifndef SVSMOOTHFILTERCLASS_H
#define SVSMOOTHFILTERCLASS_H

#include "SVFilterClass.h"

class SVSmoothFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVSmoothFilterClass );
public:
	SVSmoothFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVSMOOTHFILTER );

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};

#endif	//	SVSMOOTHFILTERCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVSmoothFilterClass.h_v  $
 * 
 *    Rev 1.0   09 Aug 2013 11:09:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
