//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointYResult
//* .File Name       : $Workfile:   SVDPointYResult.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:21:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDPointYResult.h"
#include "SVRange.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVDPointYResultClass, SVDPointYResultClassGuid );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVDPointYResultClass::SVDPointYResultClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					:SVResultClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVResultDPointYObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType( SVDPointValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "DPointYResult" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &y, SVDYObjectGuid, IDS_OBJECTNAME_DY, false, SVResetItemNone );

	// Set Embedded defaults
	y.SetDefaultValue( 0.0, TRUE );

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SVDoubleValueObjectType;
	interfaceInfo.EmbeddedID = SVDYObjectGuid;
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

SVDPointYResultClass::~SVDPointYResultClass()
{

}

BOOL SVDPointYResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVResultClass::CreateObject( PCreateStructure ) )
	{
		bOk = nullptr != getInputPoint();
	}

	// Set / Reset Printable Flag
	y.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

BOOL SVDPointYResultClass::CloseObject()
{
	return SVResultClass::CloseObject();
}

SVDPointValueObjectClass* SVDPointYResultClass::getInputPoint()
{
	if( m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().PObject )
		return static_cast<SVDPointValueObjectClass*>(m_inputObjectInfo.GetInputObjectInfo().PObject);
	
	return nullptr;
}

bool SVDPointYResultClass::onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( RRunStatus, pErrorMessages ) )
	{
		SVDPointValueObjectClass* pPoint = getInputPoint();
		ASSERT( pPoint );

		SVDPointClass dp;
		pPoint->GetValue( dp );

		// Set X
		y.SetValue( RRunStatus.m_lResultDataIndex, dp.y );
	
		return true;
	}
	RRunStatus.SetInvalid();
	return false;
}

