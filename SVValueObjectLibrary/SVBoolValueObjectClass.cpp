//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVStatusLibrary\MessageManager.h"
#include "Definitions/TextDefinesSvDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVBoolValueObjectClass, SvPb::BoolValueClassId);

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

HRESULT SVBoolValueObjectClass::GetValidTypes( SvDef::StringVector& rTypes ) const
{
	rTypes.push_back( SvDef::cTrue );
	rTypes.push_back( SvDef::cFalse );
	return S_OK;
}

BOOL SVBoolValueObjectClass::ConvertString2Type( const std::string &rValue ) const
{
	bool Result(true);
	std::string Value = SvUl::TrimLeft(rValue.c_str());

	if ( _T("1") == Value  || _T("-1") == Value /*for VB weenies*/ || (SvUl::CompareNoCase( Value, SvDef::cTrue) == 0) )
	{
		Result = true;
	}
	else if ( _T("0") == Value || (SvUl::CompareNoCase( Value, SvDef::cFalse) == 0) )
	{
		Result = false;
	}
	else
	{
		SvDef::StringVector msgList;
		msgList.push_back(Value);
		msgList.push_back(GetName());
		SvStl::MessageManager Exception(SvStl::MsgType::Log );
		Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
		Exception.Throw();
	}

	return Result;
}

void SVBoolValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter) const
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

	// Get the Data Values (Member Info, Values)
	_variant_t Value;
	Value.ChangeType(VT_BOOL);

	// for all elements in the array
	for (int32_t i = 0; i < getArraySize(); i++)
	{
		BOOL Temp(false);
		//Make sure this is not a derived virtual method which is called
		SVBoolValueObjectClass::GetValue(Temp, i);
		Value.boolVal = Temp ? VARIANT_TRUE : VARIANT_FALSE;
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVBoolValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter) const
{
	_variant_t Value;
	Value.ChangeType(VT_BOOL);
	Value.boolVal = GetDefaultValue() ? VARIANT_TRUE : VARIANT_FALSE;
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVBoolValueObjectClass::LocalInitialize()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVBoolValueObjectType;
	DefaultValue() = false;
	SetTypeName( _T("Bool") );
	SetMinMaxValues(FALSE,TRUE);
	init();
}

} //namespace SvVol
