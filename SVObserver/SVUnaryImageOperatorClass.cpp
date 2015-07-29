//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVUnaryImageOperatorClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 12:04:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVUnaryImageOperatorList.h"

SV_IMPLEMENT_CLASS( SVUnaryImageOperatorClass, SVUnaryImageOperatorClassGuid )

SVUnaryImageOperatorClass::SVUnaryImageOperatorClass( SVObjectClass* POwner, int StringResourceID )
						  :SVUnaryOperatorClass( POwner, StringResourceID )
{
	init();
}

void SVUnaryImageOperatorClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorObjectType;

	// Identify our input type needs
//	inputImageObjectInfo.InputObjectInfo.ObjectTypeInfo.ObjectType = SVImageObjectType;
//	inputImageObjectInfo.SetObject( GetObjectInfo() );

	// Register Embedded Object(s)
//	RegisterEmbeddedObject( &outputImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	// Set Embedded defaults
	m_pCurrentUIOPL = nullptr;

	// Set default inputs and outputs
	addDefaultInputObjects();

}

BOOL SVUnaryImageOperatorClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVUnaryOperatorClass::CreateObject( PCreateStructure ) )
	{
		// Owner can only be : SVUnaryImageOperatorListClass !!!
		if( m_pCurrentUIOPL = dynamic_cast<SVUnaryImageOperatorListClass*>(GetOwner()) )
		{
			bOk = TRUE;
		}
	}

	isCreated = bOk;

	return bOk;
}

BOOL SVUnaryImageOperatorClass::CloseObject()
{
	return SVUnaryOperatorClass::CloseObject();
}

BOOL SVUnaryImageOperatorClass::OnValidate() 
{
	return SVUnaryOperatorClass::OnValidate();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Run
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
BOOL SVUnaryImageOperatorClass::Run( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{
	// Should call Base Class Run() here but we have different parameters!!!
	
	// Run yourself...
	BOOL bRetVal =  onRun( First, RInputImageHandle, ROutputImageHandle, RRunStatus );

	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	// Get Status...
	dwValue = RRunStatus.GetState();
	statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	return bRetVal;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator. MUST BE OVERRIDDEN !!!
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
BOOL SVUnaryImageOperatorClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{
	ASSERT(0);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : getReferenceImage
// -----------------------------------------------------------------------------
// .Description : Returns reference image object.
//				: The reference object provides for you information about
//				: the target image object for this operator.
//				: 
//				: i.e. use this reference image in CreateObject(...) to determine
//				:	target size, in case you have to support an 
//				:	intermediate proc buffer,
//				:	or pixel depth, etc.
////////////////////////////////////////////////////////////////////////////////
SVImageClass* SVUnaryImageOperatorClass::getReferenceImage()
{
	if( SVStdImageOperatorListClass* pOpL = dynamic_cast<SVStdImageOperatorListClass*>(m_pCurrentUIOPL) )
	{
		// Output image is reference...
		return pOpL->getOutputImage();
	}
	else if( SV_IS_KIND_OF( m_pCurrentUIOPL, SVInPlaceImageOperatorListClass ) )
	{
		// Input image is reference...
		return m_pCurrentUIOPL->getInputImage();
	}

	return NULL;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVUnaryImageOperatorClass.cpp_v  $
 * 
 *    Rev 1.0   09 Aug 2013 12:04:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Created from SVOperator.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/