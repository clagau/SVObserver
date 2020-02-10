//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVariantResultClass
//* .File Name       : $Workfile:   SVVariantResultClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:19:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVVariantResultClass.h"
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

SV_IMPLEMENT_CLASS( SVVariantResultClass, SvPb::VariantResultClassId);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVVariantResultClass::SVVariantResultClass( BOOL BCreateDefaultTaskList , SVObjectClass* POwner  , int StringResourceID  )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVResultVariantObjectType;

	// Identify our input type needs
	m_inputObjectInfo.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVVariantValueObjectType);
	m_inputObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inputObjectInfo, _T( "VariantResultValue" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Value, SVValueObjectGuid, IDS_OBJECTNAME_VALUE, false, SvOi::SVResetItemNone );

	// Set Embedded defaults
	_variant_t vt;
	
	m_Value.SetDefaultValue( vt );
	m_Value.setSaveValueFlag(false);

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SvIe::SVClassInfoStruct rangeClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SvPb::SVValueObjectType;
	interfaceInfo.SubType = SvPb::SVVariantValueObjectType;
	interfaceInfo.EmbeddedID = SVValueObjectGuid;
	rangeClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Describe the SVRangeClass ...
	rangeClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVRangeObjectType;
	rangeClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVNotSetSubObjectType;
	rangeClassInfo.m_ClassId = SvPb::RangeClassId;
	rangeClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVRANGE );

	// Construct it
	SVRangeClass* pRange = dynamic_cast<SVRangeClass*> (rangeClassInfo.Construct()); 
	if( pRange )
		Add( pRange );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();

}

SVVariantResultClass::~SVVariantResultClass()
{

}

bool SVVariantResultClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVResultClass::CreateObject(rCreateStructure);
	bOk &= nullptr != GetInputValue();

	m_Value.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

SVObjectClass* SVVariantResultClass::GetInputValue()
{
	if( m_inputObjectInfo.IsConnected() && nullptr != m_inputObjectInfo.GetInputObjectInfo().getObject())
	{
		return m_inputObjectInfo.GetInputObjectInfo().getObject();
	}

	return nullptr;
}

bool SVVariantResultClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	if( __super::onRun( rRunStatus, pErrorMessages ) )
	{
		SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (GetInputValue());
		assert( pValueObject );
		
		if( nullptr != pValueObject )
		{
			_variant_t Value;
			pValueObject->getValue(Value);
			m_Value.SetValue(Value);
			return true;
		}
	}
	rRunStatus.SetInvalid();
	return false;
}

} //namespace SvOp
