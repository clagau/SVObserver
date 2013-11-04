//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVUnaryOperatorClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:30:54  $
//******************************************************************************

#ifndef SVUNARYOPERATORCLASS_H
#define SVUNARYOPERATORCLASS_H

#include "SVOperator.h"
#include "SVObjectLibrary/SVObjectLibrary.h"

class SVUnaryOperatorClass : public SVOperatorClass
{
	SV_DECLARE_CLASS( SVUnaryOperatorClass );
public:
	SVUnaryOperatorClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVUNARYOPERATOR );
	
private:
	void init();
};

#endif	//	SVUNARYOPERATORCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVUnaryOperatorClass.h_v  $
 * 
 *    Rev 1.0   12 Aug 2013 16:30:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
