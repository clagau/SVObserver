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

#include "stdafx.h"
#include "SVVariantResultClass.h"

#include "SVRange.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SV_IMPLEMENT_CLASS( SVVariantResultClass, SVVariantResultClassGuid );
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVVariantResultClass::SVVariantResultClass( BOOL BCreateDefaultTaskList , SVObjectClass* POwner  , int StringResourceID  )
{
	// Identify yourself
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType = SVResultVariantObjectType;

	// Identify our input type needs
	m_InputValueObjectInfo.SetInputObjectType( SVVariantValueObjectType );
	//m_InputValueObjectInfo.InputObjectInfo.ObjectTypeInfo.ObjectType = SVVariantValueObjectType;
	m_InputValueObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputValueObjectInfo, _T( "VariantResultValue" ) );


	// Register Embedded Objects
	RegisterEmbeddedObject( &m_Value, SVValueObjectGuid, IDS_OBJECTNAME_VALUE, false, SVResetItemNone );

	// Set Embedded defaults
	_variant_t vt;
	
	m_Value.SetDefaultValue( vt, TRUE );

	// Instantiate Dynamic Objects

	// Construct the SVRangeClass via the ClassInfoStruct
	SVClassInfoStruct rangeClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of the SVRangeClass...
	interfaceInfo.ObjectType = SVVariantValueObjectType;
//	interfaceInfo.ObjectType = SVVariantValueObjectType;
	interfaceInfo.EmbeddedID = SVValueObjectGuid;
	rangeClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Describe the SVRangeClass ...
	rangeClassInfo.m_ObjectTypeInfo.ObjectType = SVRangeObjectType;
	rangeClassInfo.m_ObjectTypeInfo.SubType	= 0;
	rangeClassInfo.m_ClassId = SVRangeClassGuid;
	rangeClassInfo.m_ClassName = SvUl_SF::LoadString( IDS_CLASSNAME_SVRANGE );

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

BOOL SVVariantResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = false;

	if( SVResultClass::CreateObject( PCreateStructure ) )
	{
		bOk = nullptr != GetInputValue();
	}

	m_Value.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}

BOOL SVVariantResultClass::CloseObject()
{
	return SVResultClass::CloseObject();
}

BOOL SVVariantResultClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	if( m_InputValueObjectInfo.IsConnected() &&
		m_InputValueObjectInfo.GetInputObjectInfo().PObject &&
		dynamic_cast <SVValueObjectClass*> (m_InputValueObjectInfo.GetInputObjectInfo().PObject) )
	{
		bRetVal = TRUE;
		bRetVal = SVResultClass::OnValidate() && bRetVal;
	}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

SVValueObjectClass* SVVariantResultClass::GetInputValue()
{
	if( m_InputValueObjectInfo.IsConnected() && m_InputValueObjectInfo.GetInputObjectInfo().PObject )
		return static_cast<SVValueObjectClass*> ( m_InputValueObjectInfo.GetInputObjectInfo().PObject );

	return nullptr;
}

BOOL SVVariantResultClass::onRun( SVRunStatusClass& RRunStatus )
{
	if( SVResultClass::onRun( RRunStatus ) )
	{
		SVValueObjectClass* pValue = GetInputValue();
		ASSERT( pValue );

		_variant_t v;
		pValue->GetValue( v );

		// Set Value
		m_Value.SetValue( RRunStatus.m_lResultDataIndex, v );

		return TRUE;
	}
	RRunStatus.SetInvalid();
	return FALSE;
}

