//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVUnaryOperatorClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 12:04:40  $
//******************************************************************************

#include "stdafx.h"
#include "SVUnaryOperatorClass.h"

SV_IMPLEMENT_CLASS( SVUnaryOperatorClass, SVUnaryOperatorClassGuid )

SVUnaryOperatorClass::SVUnaryOperatorClass( SVObjectClass* POwner, int StringResourceID )
					 :SVOperatorClass( POwner, StringResourceID )
{
	init();
}

void SVUnaryOperatorClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryOperatorObjectType;

	// Register Embedded Object(s)

	// Set Embedded defaults

	// Set default inputs and outputs
	addDefaultInputObjects();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVUnaryOperatorClass.cpp_v  $
 * 
 *    Rev 1.0   09 Aug 2013 12:04:40   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/