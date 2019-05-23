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
#include "SVObjectLibrary/SVClsIds.h"
#include "SVStatusLibrary/SVRunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVDoubleResultClass, SVDoubleResultClassGuid );

SVDoubleResultClass::SVDoubleResultClass( SVObjectClass* POwner, int StringResourceID )
				  :SVResultClass( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVResultDoubleObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVDoubleValueObjectType );
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "DoubleResultValue" ) );

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Value, SVValueObjectGuid, IDS_OBJECTNAME_VALUE, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	m_Value.SetDefaultValue( 0, true );
	m_Value.setSaveValueFlag(false);
	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SvIe::SVClassInfoStruct rangeClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SvPb::SVValueObjectType;
	interfaceInfo.SubType = SvPb::SVDoubleValueObjectType;
	interfaceInfo.EmbeddedID = SVValueObjectGuid;
	rangeClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Describe the SVRangeClass...
	rangeClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVRangeObjectType;
	rangeClassInfo.m_ObjectTypeInfo.SubType = SvPb::SVNotSetSubObjectType;
	rangeClassInfo.m_ClassId = SVRangeClassGuid;
	rangeClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVRANGE );

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

bool SVDoubleResultClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResultClass::CreateObject(rCreateStructure ) && nullptr != getInput();

	m_Value.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVDoubleResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// All inputs and outputs must be validated first
	if( __super::onRun(rRunStatus, pErrorMessages) )
	{
		const SVObjectClass* pObject = getInput();
		assert(pObject);

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

} //namespace SvOp
