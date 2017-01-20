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

#pragma region Includes
#include "stdafx.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVStdImageOperatorListClass.h"
#include "SVInplaceImageOperatorListClass.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma region Includes

SV_IMPLEMENT_CLASS( SVUnaryImageOperatorClass, SVUnaryImageOperatorClassGuid )

SVUnaryImageOperatorClass::SVUnaryImageOperatorClass( SVObjectClass* POwner, int StringResourceID )
						  :SVOperatorClass( POwner, StringResourceID )
{
	init();
}

SVUnaryImageOperatorClass::~SVUnaryImageOperatorClass()
{
}

void SVUnaryImageOperatorClass::init()
{
	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorObjectType;

	// Identify our input type needs

	// Register Embedded Object(s)

	// Set Embedded defaults
	m_pCurrentUIOPL = nullptr;

	// Set default inputs and outputs
	addDefaultInputObjects();
}

BOOL SVUnaryImageOperatorClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( __super::CreateObject( PCreateStructure ) )
	{
		// Owner can only be : SVUnaryImageOperatorListClass !!!
		if( m_pCurrentUIOPL = dynamic_cast<SVUnaryImageOperatorListClass*>(GetOwner()) )
		{
			bOk = true;
		}
	}

	m_isCreated = bOk;

	return bOk;
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
	m_statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

	// Get Status...
	dwValue = RRunStatus.GetState();
	m_statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );

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
	assert(0);
	return false;
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
	return nullptr;
}

