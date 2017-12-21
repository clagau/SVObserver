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
#include "SVUtilityLibrary/StringHelper.h"
#include "SVRange.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVDPointYResultClass, SVDPointYResultClassGuid );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVDPointYResultClass::SVDPointYResultClass( SVObjectClass* POwner, int StringResourceID )
					:SVResultClass( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVResultDPointYObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVDPointValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "DPointYResult" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &y, SVDYObjectGuid, IDS_OBJECTNAME_DY, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	y.SetDefaultValue( 0.0, true);
	y.setSaveValueFlag(false);

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SvDef::SVValueObjectType;
	interfaceInfo.SubType = SvDef::SVDoubleValueObjectType;
	interfaceInfo.EmbeddedID = SVDYObjectGuid;
	rangeClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Describe the SVRangeClass ...
	rangeClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVRangeObjectType;
	rangeClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVNotSetSubObjectType;
	rangeClassInfo.m_ClassId = SVRangeClassGuid;
	rangeClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVRANGE );

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

bool SVDPointYResultClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResultClass::CreateObject(rCreateStructure);
	bOk &= nullptr != getInputPoint();

	// Set / Reset Printable Flag
	y.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

SVDPointValueObjectClass* SVDPointYResultClass::getInputPoint()
{
	if( m_inputObjectInfo.IsConnected() )
	{
		return dynamic_cast<SVDPointValueObjectClass*> (m_inputObjectInfo.GetInputObjectInfo().getObject());
	}
	
	return nullptr;
}

bool SVDPointYResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		SVDPointValueObjectClass* pPoint = getInputPoint();
		ASSERT( pPoint );

		SVDPointClass DPoint;
		pPoint->GetValue( DPoint );

		// Set Y
		y.SetValue(DPoint.y);
	
		return true;
	}
	rRunStatus.SetInvalid();
	return false;
}

