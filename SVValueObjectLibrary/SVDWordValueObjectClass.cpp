//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDWordValueObjectClass
//* .File Name       : $Workfile:   SVDWordValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Jul 2013 12:28:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVDWordValueObjectClass.h"
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

SV_IMPLEMENT_CLASS(SVDWordValueObjectClass, SvPb::DWordValueClassId);

SVDWordValueObjectClass::SVDWordValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<DWORD>( ObjectName ) 
{
	LocalInitialize();
}


SVDWordValueObjectClass::SVDWordValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<DWORD>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVDWordValueObjectClass::SVDWordValueObjectClass( const SVDWordValueObjectClass& rhs )
: SVValueObjectClass<DWORD>()
{
	LocalInitialize();
	*this = rhs;
}

const SVDWordValueObjectClass& SVDWordValueObjectClass::operator = ( const SVDWordValueObjectClass& rhs )
{
	__super::operator = (rhs);
	return *this;
}

SVDWordValueObjectClass::~SVDWordValueObjectClass()
{
}

HRESULT SVDWordValueObjectClass::SetOutputFormat(OutputFormat outputFormat)
{
	HRESULT Result = S_OK;
	switch (outputFormat)
	{
	case OutputFormat_int:
		setOutputFormat( _T("{}") );
		break;
	case OutputFormat_hex:
		setOutputFormat( _T("{:#010x}") );
		break;
	default:
		Result = E_INVALIDARG;
		break;
	}
	return Result;
}

DWORD SVDWordValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	std::string Digits = SvUl::ValidateString( rValue, _T("0123456789 .xXabcdefABCDEF") );
	if ( Digits == rValue )
	{
		Digits = SvUl::MakeLower(Digits.c_str());
		TCHAR* p = nullptr;
		DWORD Value( 0 );
		if ( std::string::npos != Digits.find( 'x' ) )
		{
			Value = _tcstoul(Digits.c_str(), &p, 16);
		}
		else
		{
			Value = _tcstoul(Digits.c_str(), &p, 10);
		}

		return Value;
	}
	SvDef::StringVector msgList;
	msgList.push_back(std::string(rValue));
	msgList.push_back(GetName());
	SvStl::MessageManager Exception(SvStl::MsgType::Log );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
	Exception.Throw();
	return 0; //will never be reached, because the exception will throw before. But this line avoids a warning
}

void SVDWordValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter) const
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

	// Get the Data Values (Member Info, Values)
	_variant_t Value;
	Value.ChangeType(VT_UI4);

	// for all elements in the array
	for (int32_t i = 0; i < getArraySize(); i++)
	{
		DWORD Temp(0);
		//Make sure this is not a derived virtual method which is called
		SVDWordValueObjectClass::GetValue(Temp, i);
		Value.ulVal = static_cast<ULONG> (Temp);
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVDWordValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter) const
{
	_variant_t Value(GetDefaultValue());
	Value.ChangeType(VT_UI4);
	rWriter.WriteAttribute(scDefaultTag, Value);
}


void SVDWordValueObjectClass::setStandardFormatString()
{
	SetOutputFormat(OutputFormat_hex);
}


void SVDWordValueObjectClass::LocalInitialize()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVDWordValueObjectType;
	DefaultValue() = 0;
	SetTypeName( _T("Integer32Hex") );
	setStandardFormatString();
	init();
	SetMinMaxValues(0, ULONG_MAX);
}

} //namespace SvVol
