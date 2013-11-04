//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVSharpen2FilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:29:22  $
//******************************************************************************

#ifndef SVSHARPEN2FILTERCLASS_H
#define SVSHARPEN2FILTERCLASS_H

#include "SVFilterClass.h"

class SVSharpen2FilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVSharpen2FilterClass );
public:
	SVSharpen2FilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVSHARPEN2FILTER );

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
};

#endif	//	SVSHARPEN2FILTERCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVSharpen2FilterClass.h_v  $
 * 
 *    Rev 1.0   12 Aug 2013 16:29:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
