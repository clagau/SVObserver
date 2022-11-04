//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPointValueObjectClass
//* .File Name       : $Workfile:   SVPointValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Aug 2013 11:21:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVPointValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(SVPointValueObjectClass, SvPb::PointValueClassId);

SVPointValueObjectClass::SVPointValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<SVPoint<long>>( ObjectName )
{
	LocalInitialize();
}


SVPointValueObjectClass::SVPointValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<SVPoint<long>>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVPointValueObjectClass::SVPointValueObjectClass( const SVPointValueObjectClass& rhs )
: SVValueObjectClass<SVPoint<long>>()
{
	LocalInitialize();
	*this = rhs;
}

const SVPointValueObjectClass& SVPointValueObjectClass::operator = (const SVPointValueObjectClass& rhs)
{
	__super::operator = (rhs);
	return *this;
}

SVPointValueObjectClass::~SVPointValueObjectClass()
{
}

_variant_t SVPointValueObjectClass::ValueType2Variant(const SVPoint<long>* pValue) const
{
	_variant_t Result;

	if(nullptr != pValue)
	{
		Result.SetString( ConvertType2String(*pValue).c_str() );
	}

	return Result;
}

SVPoint<long> SVPointValueObjectClass::Variant2ValueType( const _variant_t& rValue) const
{
	SVPoint<long> Result;

	if( VT_BSTR == rValue.vt )
	{
		try
		{
			std::string Value = SvUl::createStdString( rValue );
			Result = ConvertString2Type( Value );
		}
		catch( const SvStl::MessageContainer& )
		{
		}
	}

	return Result;
}

SVPoint<long> SVPointValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	std::string LegalChars = SvUl::ValidateString( rValue, _T("0123456789()-, ") );	// only integers
	if ( LegalChars == rValue )
	{
		SvUl::RemoveCharacters( LegalChars, _T("()") );
		size_t Pos = LegalChars.find(_T(','));
		if ( std::string::npos != Pos )
		{
			std::string XValue = SvUl::Left( LegalChars, Pos );
			std::string YValue = SvUl::Mid( LegalChars, Pos + 1 );
			return SVPoint<long>( atol(XValue.c_str()), atol(YValue.c_str()) );
		}
	}
	SvDef::StringVector msgList;
	msgList.push_back(std::string(rValue));
	msgList.push_back(GetName());
	SvStl::MessageManager Exception(SvStl::MsgType::Log );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
	Exception.Throw();
}

void SVPointValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter) const
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

	// Get the Data Values (Member Info, Values)
	std::string TempValue;
	_variant_t Value;
	Value.Clear();

	// for all elements in the array
	for (int32_t i = 0; i < getArraySize(); i++)
	{
		SVPoint<long> PointValue;
		//Make sure this is not a derived virtual method which is called
		SVPointValueObjectClass::GetValue(PointValue, i);
		TempValue = SvUl::Format(_T("%d, %d"), PointValue.m_x, PointValue.m_y);
		Value.SetString(TempValue.c_str());
		list.push_back(Value);
		Value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVPointValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter) const
{
	std::string TempValue;
	TempValue = SvUl::Format(_T("%d, %d"), GetDefaultValue().m_x, GetDefaultValue().m_y);
	_variant_t Value;
	Value.SetString(TempValue.c_str());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVPointValueObjectClass::LocalInitialize()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVPointValueObjectType;
	SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::OverwriteAttribute );

	SetTypeName( _T("Point") );
	init();
}

} //namespace SvVol
