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
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLongResultClass, SVLongResultClassGuid );

SVLongResultClass::SVLongResultClass( SVObjectClass* POwner, int StringResourceID )
				  :SVResultClass( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVResultLongObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVLongValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, SvO::cInputTag_LongResultValue );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Value, SVValueObjectGuid, IDS_OBJECTNAME_VALUE, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_Value.SetDefaultValue( 0, true );
	m_Value.setSaveValueFlag(false);

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SvDef::SVValueObjectType;
	interfaceInfo.SubType = SvDef::SVLongValueObjectType;
	interfaceInfo.EmbeddedID = SVValueObjectGuid;
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

SVLongResultClass::~SVLongResultClass()
{

}

bool SVLongResultClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResultClass::CreateObject(rCreateStructure) && nullptr != getInput();
	
	m_Value.SetObjectAttributesAllowed(SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

const GUID & SVLongResultClass::GetInputEmbeddedID() const
{
	if (nullptr != getInput() && SvDef::SVLongValueObjectType == getInput()->GetObjectSubType())
	{
		return getInput()->GetEmbeddedID();
	}
	else
	{
		return SV_GUID_NULL;
	}
}

bool SVLongResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		const SVObjectClass* pObject = getInput();
		ASSERT( pObject );

		if( nullptr != pObject )
		{
			double FetchValue(0.0);
			pObject->getValue( FetchValue );
			long SetValue = static_cast<long> (FetchValue);
			m_Value.SetValue(SetValue);
			return true;
		}
	}
	rRunStatus.SetInvalid();
	return false;
}


