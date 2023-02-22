//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLongValueObjectClass
//* .File Name       : $Workfile:   SVLongValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 12:29:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <sstream>
#include "SVLongValueObjectClass.h"
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

SV_IMPLEMENT_CLASS(SVLongValueObjectClass, SvPb::LongValueClassId);

SVLongValueObjectClass::SVLongValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<long>( ObjectName )
{
	LocalInitialize();
}

SVLongValueObjectClass::SVLongValueObjectClass(SVObjectClass* pOwner, int StringResourceID)
: SVValueObjectClass<long>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVLongValueObjectClass::SVLongValueObjectClass( const SVLongValueObjectClass& rhs )
: SVValueObjectClass<long>()
{
	LocalInitialize();
	*this = rhs;
}

const SVLongValueObjectClass& SVLongValueObjectClass::operator = (const SVLongValueObjectClass& rhs)
{
	__super::operator = (rhs);
	return *this;
}

SVLongValueObjectClass::~SVLongValueObjectClass()
{
}

long SVLongValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	std::string Digits = SvUl::ValidateString( rValue, _T("-0123456789 .xXabcdefABCDEF") );
	if( Digits == rValue )
	{
		Digits = SvUl::MakeLower(Digits.c_str());
		TCHAR* p = nullptr;
		long lValue;
		if ( std::string::npos != Digits.find( 'x' ) )
		{
			lValue = _tcstol(Digits.c_str(), &p, 16);
		}
		else
		{
			lValue = _tcstol(Digits.c_str(), &p, 10);
		}

		return lValue;
	}
	SvDef::StringVector msgList;
	msgList.push_back( rValue );
	msgList.push_back( GetName() );
	SvStl::MessageManager Exception(SvStl::MsgType::Log );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
	Exception.Throw();
	return 0; //will never reached, because the exception will throw before. But this line avoids a warning
}

void SVLongValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter) const
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

	// for all elements in the array
	for (int32_t i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		_variant_t Value;
		Value.ChangeType(VT_I4);
		SVLongValueObjectClass::GetValue(Value.lVal, i);
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVLongValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter) const
{
	_variant_t Value(GetDefaultValue());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVLongValueObjectClass::LocalInitialize()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVLongValueObjectType;
	DefaultValue() = 0;
	SetTypeName( _T("Integer32") );

	setStandardFormatString();
	init();
}


void SVLongValueObjectClass::setStandardFormatString()
{
	setOutputFormat(_T("{:d}"));
}


std::string SVLongValueObjectClass::getFixedWidthFormatString(uint32_t totalWidth, uint32_t /*decimals: ignored*/)
{
	std::ostringstream formatStream;
	formatStream << "{:" << totalWidth << "d}";
	return formatStream.str();
}



} //namespace SvVol
