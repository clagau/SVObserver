//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInt64ValueObjectClass
//* .File Name       : $Workfile:   SVInt64ValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 12:24:32  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <sstream>
#include "SVInt64ValueObjectClass.h"
#include "Definitions/StringTypeDef.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(SVInt64ValueObjectClass, SvPb::Int64ValueClassId);

SVInt64ValueObjectClass::SVInt64ValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<__int64>( ObjectName )
{
	LocalInitialize();
}

SVInt64ValueObjectClass::SVInt64ValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<__int64>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVInt64ValueObjectClass::~SVInt64ValueObjectClass()
{
}

const SVInt64ValueObjectClass& SVInt64ValueObjectClass::operator = (const SVInt64ValueObjectClass& rhs)
{
	__super::operator = (rhs);
	return *this;
}

__int64 SVInt64ValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	std::string Digits = SvUl::ValidateString( rValue, _T("0123456789- ") );
	if( Digits == rValue )
	{
		return _atoi64( Digits.c_str() );
	}

	SvDef::StringVector msgList;
	msgList.push_back( rValue );
	msgList.push_back( GetName() );
	SvStl::MessageManager Exception(SvStl::MsgType::Log );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
	Exception.Throw();
}

void SVInt64ValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter) const
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;
	_variant_t Value;
	Value.ChangeType(VT_I8);

	// for all elements in the array
	for (int32_t i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		SVInt64ValueObjectClass::GetValue(Value.llVal, i);
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVInt64ValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter) const
{
	_variant_t Value(GetDefaultValue());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVInt64ValueObjectClass::LocalInitialize()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVInt64ValueObjectType;
	DefaultValue() = 0;
	SetTypeName( _T("Integer64") );

	setStandardFormatString();
	init();
}

std::string SVInt64ValueObjectClass::getFixedWidthFormatString(uint32_t totalWidth, uint32_t /*decimals: ignored*/)
{
	std::ostringstream formatStream;
	formatStream << "{:" << totalWidth << "}";
	return formatStream.str();
}



void SVInt64ValueObjectClass::setStandardFormatString()
{
	setOutputFormat(_T("{}"));
}


} //namespace SvVol
