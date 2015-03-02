//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVFilterClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:54:04  $
//******************************************************************************

#ifndef SVFILTERCLASS_H
#define SVFILTERCLASS_H

#include "ObjectInterfaces\ISVFilter.h"
#include "SVUnaryImageOperatorClass.h"

class SVFilterClass : virtual public SvOi::ISVFilter
	,public SVUnaryImageOperatorClass
{
	SV_DECLARE_CLASS( SVFilterClass );
public:
	SVFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVFILTER );

#pragma region virtual methods (IFilter)
	virtual bool shouldResetInspection() const override { return false; };
#pragma endregion virtual methods (IFilter)

private:
	void init();

};

#endif	//	SVFILTERCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFilterClass.h_v  $
 * 
 *    Rev 1.0   09 Aug 2013 09:54:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
