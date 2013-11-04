//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVBinaryOperatorClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 11:56:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVBinaryOperatorClass.h"

SV_IMPLEMENT_CLASS( SVBinaryOperatorClass, SVBinaryOperatorClassGuid )

SVBinaryOperatorClass::SVBinaryOperatorClass( SVObjectClass* POwner, int StringResourceID )
					  :SVOperatorClass( POwner, StringResourceID )
{
	init();
}

void SVBinaryOperatorClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVBinaryOperatorObjectType;

	// Register Embedded Object(s)
//	RegisterEmbeddedObject( &outputImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	// Set Embedded defaults

	// Set default inputs and outputs
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVBinaryOperatorClass.cpp_v  $
 * 
 *    Rev 1.0   09 Aug 2013 11:56:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/