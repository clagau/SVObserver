//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCharValueObjectClass
//* .File Name       : $Workfile:   SVCharValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 12:28:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVCharValueObjectClass.h"
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
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVCharValueObjectClass, SvPb::CharValueClassId);

SVCharValueObjectClass::SVCharValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<TCHAR>( ObjectName )
{
	LocalInitialize();
}


SVCharValueObjectClass::SVCharValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<TCHAR>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVCharValueObjectClass::SVCharValueObjectClass( const SVCharValueObjectClass& rhs )
: SVValueObjectClass<TCHAR>()
{
	LocalInitialize();
	*this = rhs;
}

const SVCharValueObjectClass& SVCharValueObjectClass::operator = ( const SVCharValueObjectClass& rhs )
{
	__super::operator = (rhs);
	return *this;
}

SVCharValueObjectClass::~SVCharValueObjectClass()
{
}

TCHAR SVCharValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	if ( 1 == rValue.size() )
	{
		return rValue[0];
	}
	else
	{
		std::string Digits = SvUl::ValidateString( rValue, _T("0123456789 .xXabcdefABCDEF") );
		if ( Digits == rValue )
		{
			Digits = SvUl::MakeLower(Digits.c_str());
			TCHAR* p = nullptr;
			long Value;
			if ( std::string::npos != Digits.find( 'x' ) )
			{
				Value = _tcstol(Digits.c_str(), &p, 16);
			}
			else
			{
				Value = _tcstol(Digits.c_str(), &p, 10);
			}

			if ( 0 <= Value && 255 >= Value )
			{
				return static_cast<TCHAR> (Value);
			}
		}
	}
	SvDef::StringVector msgList;
	msgList.push_back(std::string(rValue));
	msgList.push_back(GetName());
	SvStl::MessageManager Exception(SvStl::MsgType::Log );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
	Exception.Throw();
	return 0; //will never reached, because the exception will throw before. But this line avoids a warning
}

void SVCharValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter)
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

	// for all elements in the array
	_variant_t Value;
	Value.ChangeType(VT_I1);
	for (int32_t i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		SVCharValueObjectClass::GetValue(Value.cVal, i);
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVCharValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter)
{
	_variant_t Value(GetDefaultValue());
	rWriter.WriteAttribute(scDefaultTag, Value);
}


void SVCharValueObjectClass::setStandardFormatString()
{
	setOutputFormat(_T("%c"));
}

void SVCharValueObjectClass::LocalInitialize()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVCharValueObjectType;
	DefaultValue() = 0;
	SetTypeName( _T("Char") );
	setStandardFormatString();
	init();
}

} //namespace SvVol
