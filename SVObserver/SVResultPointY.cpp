//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultPointY
//* .File Name       : $Workfile:   SVResultPointY.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:57:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVResultPointY.h"
#include "SVRange.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVPointYResultClass, SVPointYResultClassGuid );

SVPointYResultClass::SVPointYResultClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					:SVResultClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVResultPointYObjectType;

	// Identify our input type needs
	
	m_inputObjectInfo.SetInputObjectType( SVPointValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "PointYResultValue" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &y, SVYObjectGuid, IDS_OBJECTNAME_Y, false, SVResetItemNone );

	// Set Embedded defaults
	y.SetDefaultValue( 0, TRUE );

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SVLongValueObjectType;
	interfaceInfo.EmbeddedID = SVYObjectGuid;
	rangeClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Describe the SVRangeClass ...
	rangeClassInfo.m_ObjectTypeInfo.ObjectType = SVRangeObjectType;
	rangeClassInfo.m_ObjectTypeInfo.SubType	= 0;
	rangeClassInfo.m_ClassId = SVRangeClassGuid;
	rangeClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVRANGE );

	// Construct it
	SVRangeClass* pRange = (SVRangeClass* )rangeClassInfo.Construct(); 
	if( pRange )
		Add( pRange );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVPointYResultClass::~SVPointYResultClass()
{

}

BOOL SVPointYResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVResultClass::CreateObject( PCreateStructure ) )
	{
		bOk = nullptr != getInputPoint();
	}

	y.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

BOOL SVPointYResultClass::CloseObject()
{
	return SVResultClass::CloseObject();
}

SVPointValueObjectClass* SVPointYResultClass::getInputPoint()
{
	if( m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().PObject )
		return static_cast<SVPointValueObjectClass*>(m_inputObjectInfo.GetInputObjectInfo().PObject);

	return nullptr;
}

BOOL SVPointYResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVResultClass::onRun( RRunStatus ) )
	{
		SVPointValueObjectClass* pPoint = getInputPoint();
		ASSERT( pPoint );

		POINT p;
		pPoint->GetValue( p );

		// Set X
		y.SetValue( RRunStatus.m_lResultDataIndex, p.y );
	
		return TRUE;
	}
	RRunStatus.SetInvalid();
	return FALSE;
}

