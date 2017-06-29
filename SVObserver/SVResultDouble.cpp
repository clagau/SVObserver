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
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVDoubleResultClass, SVDoubleResultClassGuid );

SVDoubleResultClass::SVDoubleResultClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				  :SVResultClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVResultDoubleObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType( SVDoubleValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "DoubleResultValue" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Value, SVValueObjectGuid, IDS_OBJECTNAME_VALUE, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_Value.SetDefaultValue( 0, true );
	m_Value.setSaveValueFlag(false);
	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SVDoubleValueObjectType;
	interfaceInfo.EmbeddedID = SVValueObjectGuid;
	rangeClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Describe the SVRangeClass...
	rangeClassInfo.m_ObjectTypeInfo.ObjectType = SVRangeObjectType;
	rangeClassInfo.m_ObjectTypeInfo.SubType	= 0;
	rangeClassInfo.m_ClassId = SVRangeClassGuid;
	rangeClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_CLASSNAME_SVRANGE );

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

	m_Value.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

BOOL SVDoubleResultClass::CloseObject()
{
	return SVResultClass::CloseObject();
}

bool SVDoubleResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun(rRunStatus, pErrorMessages) )
	{
		const SVObjectClass* pObject = getInput();
		ASSERT(pObject);

		if( nullptr != pObject )
		{
			double Value;
			pObject->getValue(Value);
			m_Value.SetValue(Value);
			return true;
		}
	}
	rRunStatus.SetInvalid();
	return false;
}


