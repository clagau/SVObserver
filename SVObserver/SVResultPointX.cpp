//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultPointX
//* .File Name       : $Workfile:   SVResultPointX.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:57:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVResultPointX.h"
#include "SVRange.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVPointXResultClass, SVPointXResultClassGuid );

SVPointXResultClass::SVPointXResultClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					:SVResultClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVResultPointXObjectType;

	// Identify our input type needs
	inputObjectInfo.SetInputObjectType( SVPointValueObjectType );
	inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputObjectInfo, _T( "PointXResultValue" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &x, SVXObjectGuid, IDS_OBJECTNAME_X, false, SVResetItemNone );

	// Set Embedded defaults
	x.SetDefaultValue( 0, TRUE );

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SVLongValueObjectType;
	interfaceInfo.EmbeddedID = SVXObjectGuid;
	rangeClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Describe the SVRangeClass ...
	rangeClassInfo.m_ObjectTypeInfo.ObjectType = SVRangeObjectType;
	rangeClassInfo.m_ObjectTypeInfo.SubType	= 0;
	rangeClassInfo.m_ClassId = SVRangeClassGuid;
	rangeClassInfo.m_ClassName = SvUl_SF::LoadString( IDS_CLASSNAME_SVRANGE );

	// Construct it
	SVRangeClass* pRange = (SVRangeClass* )rangeClassInfo.Construct(); 
	if( pRange )
		Add( pRange );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVPointXResultClass::~SVPointXResultClass()
{

}

BOOL SVPointXResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVResultClass::CreateObject( PCreateStructure ) )
	{
		bOk = nullptr != getInputPoint();
	}
	
	x.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

BOOL SVPointXResultClass::CloseObject()
{
	return SVResultClass::CloseObject();
}

SVPointValueObjectClass* SVPointXResultClass::getInputPoint()
{
	if( inputObjectInfo.IsConnected() && inputObjectInfo.GetInputObjectInfo().PObject )
		return static_cast<SVPointValueObjectClass*>(inputObjectInfo.GetInputObjectInfo().PObject);

	return nullptr;
}

BOOL SVPointXResultClass::OnValidate()
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

BOOL SVPointXResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVResultClass::onRun( RRunStatus ) )
	{
		SVPointValueObjectClass* pPoint = getInputPoint();
		ASSERT( pPoint );

		POINT p;
		pPoint->GetValue( p );

		// Set X
		x.SetValue( RRunStatus.m_lResultDataIndex, p.x );

		return TRUE;
	}
	RRunStatus.SetInvalid();
	return FALSE;
}






