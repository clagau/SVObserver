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
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVPointXResultClass, SVPointXResultClassGuid );

SVPointXResultClass::SVPointXResultClass( SVObjectClass* POwner, int StringResourceID )
					:SVResultClass( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVResultPointXObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVPointValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "PointXResultValue" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &m_X, SVXObjectGuid, IDS_OBJECTNAME_X, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_X.SetDefaultValue( 0 );
	m_X.setSaveValueFlag(false);

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SvDef::SVValueObjectType;
	interfaceInfo.SubType = SvDef::SVLongValueObjectType;
	interfaceInfo.EmbeddedID = SVXObjectGuid;
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

SVPointXResultClass::~SVPointXResultClass()
{

}

bool SVPointXResultClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVResultClass::CreateObject(rCreateStructure) && nullptr != getInputPoint();
	
	m_X.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}

SVPointValueObjectClass* SVPointXResultClass::getInputPoint()
{
	if( m_inputObjectInfo.IsConnected() && nullptr != m_inputObjectInfo.GetInputObjectInfo().getObject())
	{
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return static_cast<SVPointValueObjectClass*> (m_inputObjectInfo.GetInputObjectInfo().getObject());
	}

	return nullptr;
}

bool SVPointXResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		const SVPointValueObjectClass* pValueObject = getInputPoint();
		ASSERT( pValueObject );

		if( nullptr != pValueObject )
		{
			SVPoint Value;
			pValueObject->GetValue(Value);
			m_X.SetValue(Value.x());
			return true;
		}
	}
	rRunStatus.SetInvalid();
	return false;
}






