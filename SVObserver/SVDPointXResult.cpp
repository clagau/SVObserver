//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointXResult
//* .File Name       : $Workfile:   SVDPointXResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:21:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDPointXResult.h"
#include "SVRange.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVDPointXResultClass, SVDPointXResultClassGuid );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVDPointXResultClass::SVDPointXResultClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					:SVResultClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVResultDPointXObjectType;

	// Identify our input type needs
	inputObjectInfo.SetInputObjectType( SVDPointValueObjectType );
	inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputObjectInfo, _T( "DPointXResult" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &x, SVDXObjectGuid, IDS_OBJECTNAME_DX, false, SVResetItemNone );

	// Set Embedded defaults
	x.SetDefaultValue( 0.0, TRUE );

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SVDoubleValueObjectType;
	interfaceInfo.EmbeddedID = SVDXObjectGuid;
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

SVDPointXResultClass::~SVDPointXResultClass()
{

}

BOOL SVDPointXResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;
	
	if( SVResultClass::CreateObject( PCreateStructure ) )
	{
		bOk = nullptr != getInputPoint();
	}

	// Set / Reset Printable Flag
	x.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	
	m_isCreated = bOk;

	return bOk;
}

BOOL SVDPointXResultClass::CloseObject()
{
	return SVResultClass::CloseObject();
}

SVDPointValueObjectClass* SVDPointXResultClass::getInputPoint()
{
	if( inputObjectInfo.IsConnected() && inputObjectInfo.GetInputObjectInfo().PObject )
		return static_cast<SVDPointValueObjectClass*>(inputObjectInfo.GetInputObjectInfo().PObject);

	return nullptr;
}


BOOL SVDPointXResultClass::OnValidate()
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

BOOL SVDPointXResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVResultClass::onRun( RRunStatus ) )
	{
		SVDPointValueObjectClass* pPoint = getInputPoint();
		ASSERT( pPoint );

		SVDPointClass dp;
		pPoint->GetValue( dp );

		// Set X
		x.SetValue( RRunStatus.m_lResultDataIndex, dp.x );

		return TRUE;
	}
	RRunStatus.SetInvalid();
	return FALSE;
}

