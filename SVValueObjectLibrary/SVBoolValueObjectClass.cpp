//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBoolValueObjectClass
//* .File Name       : $Workfile:   SVBoolValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   30 Oct 2013 15:18:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVBoolValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\TextDefineSvOi.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVBoolValueObjectClass, SVBoolValueObjectClassGuid);

SVBoolValueObjectClass::SVBoolValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<BOOL>( ObjectName )
{
	LocalInitialize();
}

SVBoolValueObjectClass::SVBoolValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<BOOL>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVBoolValueObjectClass::SVBoolValueObjectClass( const SVBoolValueObjectClass& rhs )
: SVValueObjectClass<BOOL>()
{
	LocalInitialize();
	*this = rhs;
}

SVBoolValueObjectClass::~SVBoolValueObjectClass()
{
}

const SVBoolValueObjectClass& SVBoolValueObjectClass::operator = (const SVBoolValueObjectClass& rhs)
{
	__super::operator = (rhs);
	return *this;
}

void SVBoolValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	_variant_t Value;
	Value.ChangeType(VT_BOOL);
	Value.boolVal = GetDefaultValue() ? VARIANT_TRUE : VARIANT_FALSE;

	rWriter.WriteAttribute(scDefaultTag, Value);

	rWriter.StartElement(scArrayElementsTag);
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array
	for( int i = 0; i < getArraySize(); i++ )
	{
		BOOL Temp(false);
		//Make sure this is not a derived virtual method which is called
		SVBoolValueObjectClass::GetValue( Temp, GetLastSetIndex(), i);
		Value.boolVal = Temp ? VARIANT_TRUE : VARIANT_FALSE;
		list.push_back( Value );
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVBoolValueObjectClass::GetValidTypes( SVStringVector& rTypes ) const
{
	rTypes.push_back( SvOi::cTrue );
	rTypes.push_back( SvOi::cFalse );
	return S_OK;
}

BOOL SVBoolValueObjectClass::ConvertString2Type( const SVString &rValue ) const
{
	bool Result(true);
	SVString Value = rValue;

	Value = SvUl_SF::TrimLeft( Value );
	Value = SvUl_SF::TrimRight( Value );
	if ( _T("1") == Value  || _T("-1") == Value /*for VB weenies*/ || (SvUl_SF::CompareNoCase( Value, SvOi::cTrue) == 0) )
	{
		Result = true;
	}
	else if ( _T("0") == Value || (SvUl_SF::CompareNoCase( Value, SvOi::cFalse) == 0) )
	{
		Result = false;
	}
	else
	{
		SVStringVector msgList;
		msgList.push_back(Value);
		msgList.push_back(GetName());
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		Exception.Throw();
	}

	return Result;
}

void SVBoolValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVBoolValueObjectType;
	DefaultValue() = false;
	if ( m_sLegacyScriptDefaultName.empty() )
	{
		m_sLegacyScriptDefaultName = "bDefault";
		m_sLegacyScriptArrayName = "pBArray";
	}
	SetTypeName( _T("Bool") );

	InitializeBuckets();
}


