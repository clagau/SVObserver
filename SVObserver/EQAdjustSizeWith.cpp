//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Equation class for Formula for Windows Width for ToolsSize Adjust 
//******************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "EQAdjustSizeWith.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( EQAdjustSizeWith, EQAdjustSizeWidthGuid );


EQAdjustSizeWith::EQAdjustSizeWith( SVObjectClass* POwner, int StringResourceID )
	:EQAdjustSize( POwner, StringResourceID )
{
	init();
}

EQAdjustSizeWith::~EQAdjustSizeWith()
{
}

void EQAdjustSizeWith::init()
{
	m_bUseOverlays = false;

	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVEquationObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = EQSizeWidthType;


	RegisterEmbeddedObject( &m_result, EQAdjustSizeWidthResultGuid, IDS_OBJECTNAME_RESULT, false, SVResetItemNone );

	// Set Embedded defaults
	m_result.SetDefaultValue( 100.0, TRUE );

	// Set default inputs and outputs
	addDefaultInputObjects();
}
