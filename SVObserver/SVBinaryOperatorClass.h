//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVBinaryOperatorClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 09:51:40  $
//******************************************************************************

#ifndef SVBINARYOPERATORCLASS_H
#define SVBINARYOPERATORCLASS_H

#include "SVOperator.h"

class SVBinaryOperatorClass : public SVOperatorClass
{
	SV_DECLARE_CLASS( SVBinaryOperatorClass );
public:
	SVBinaryOperatorClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVBINARYOPERATOR );
	
private:
	void init();
};

#endif	//	SVBINARYOPERATORCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVBinaryOperatorClass.h_v  $
 * 
 *    Rev 1.0   09 Aug 2013 09:51:40   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
