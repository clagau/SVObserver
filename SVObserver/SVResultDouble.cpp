//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultDouble
//* .File Name       : $Workfile:   SVResultDouble.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:55:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVResultDouble.h"
#include "SVRange.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVDoubleResultClass, SVDoubleResultClassGuid );

SVDoubleResultClass::SVDoubleResultClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				  :SVResultClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVResultDoubleObjectType;

	// Identify our input type needs
	inputObjectInfo.SetInputObjectType( SVDoubleValueObjectType );
	inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputObjectInfo, _T( "DoubleResultValue" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &value, SVValueObjectGuid, IDS_OBJECTNAME_VALUE, false, SVResetItemNone );

	// Set Embedded defaults
	value.SetDefaultValue( 0, TRUE );
	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SVDoubleValueObjectType;
	interfaceInfo.EmbeddedID = SVValueObjectGuid;
	rangeClassInfo.DesiredInputInterface.Add( interfaceInfo );

	// Describe the SVRangeClass...
	rangeClassInfo.ObjectTypeInfo.ObjectType = SVRangeObjectType;
	rangeClassInfo.ObjectTypeInfo.SubType	= 0;
	rangeClassInfo.ClassId = SVRangeClassGuid;
	rangeClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVRANGE );

	//  Construct it
	SVRangeClass* pRange = ( SVRangeClass* )rangeClassInfo.Construct(); 
	if( pRange )
		Add( pRange );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVDoubleResultClass::~SVDoubleResultClass()
{
	CloseObject();
}

BOOL SVDoubleResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVResultClass::CreateObject( PCreateStructure ) )
	{
		bOk = nullptr != getInput();
	}

	value.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

BOOL SVDoubleResultClass::CloseObject()
{
	return SVResultClass::CloseObject();
}

BOOL SVDoubleResultClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( inputObjectInfo.IsConnected() &&
		inputObjectInfo.GetInputObjectInfo().PObject )
	{
		bRetVal = TRUE;
		bRetVal = SVResultClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

BOOL SVDoubleResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVResultClass::onRun(RRunStatus) )
	{
		SVDoubleValueObjectClass* pValue = static_cast <SVDoubleValueObjectClass*> (getInput());
		ASSERT( pValue );

		double v;

		pValue->GetValue(v);

		// Set Value
		value.SetValue(RRunStatus.m_lResultDataIndex, v);

	
		return TRUE;
	}
	RRunStatus.SetInvalid();
	return FALSE;
}


SVDoubleValueObjectClass* SVDoubleResultClass::getInputDouble()
{
	if( inputObjectInfo.IsConnected() && inputObjectInfo.GetInputObjectInfo().PObject )
		return static_cast<SVDoubleValueObjectClass*>(inputObjectInfo.GetInputObjectInfo().PObject);

	return nullptr;
}


