//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultLong
//* .File Name       : $Workfile:   SVResultLong.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:56:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVResultLong.h"
#include "SVRange.h"
#include "TextDefinesSvO.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLongResultClass, SVLongResultClassGuid );

SVLongResultClass::SVLongResultClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				  :SVResultClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVResultLongObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType( SVLongValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, SvO::cInputTag_LongResultValue );

	// Register Embedded Objects
	RegisterEmbeddedObject( &value, SVValueObjectGuid, IDS_OBJECTNAME_VALUE, false, SVResetItemNone );

	// Set Embedded defaults
	value.SetDefaultValue( 0, TRUE );

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SVLongValueObjectType;
	interfaceInfo.EmbeddedID = SVValueObjectGuid;
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

SVLongResultClass::~SVLongResultClass()
{

}

BOOL SVLongResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
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

BOOL SVLongResultClass::CloseObject()
{
	return SVResultClass::CloseObject();
}

bool SVLongResultClass::onRun( SVRunStatusClass& RRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( RRunStatus, pErrorMessages ) )
	{
		SVLongValueObjectClass* pValue = static_cast <SVLongValueObjectClass*> (getInput());
		ASSERT( pValue );

		long v;
		pValue->GetValue( v );

		// Set Value
		value.SetValue( RRunStatus.m_lResultDataIndex, v );

		return true;
	}
	RRunStatus.SetInvalid();
	return false;
}

SVLongValueObjectClass* SVLongResultClass::getInputLong()
{
	if( m_inputObjectInfo.IsConnected() && m_inputObjectInfo.GetInputObjectInfo().PObject )
		return static_cast<SVLongValueObjectClass*>(m_inputObjectInfo.GetInputObjectInfo().PObject);

	return nullptr;
}


