//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVByteValueObjectClass
//* .File Name       : $Workfile:   SVByteValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 12:16:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVByteValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Definitions/TextDefineSVDef.h"
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

SV_IMPLEMENT_CLASS(SVByteValueObjectClass, SVByteValueObjectClassGuid);

SVByteValueObjectClass::SVByteValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<BYTE>( ObjectName )
{
	LocalInitialize();
}

SVByteValueObjectClass::SVByteValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<BYTE>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVByteValueObjectClass::SVByteValueObjectClass( const SVByteValueObjectClass& rhs )
: SVValueObjectClass<BYTE>()
{
	LocalInitialize();
	*this = rhs;
}

const SVByteValueObjectClass& SVByteValueObjectClass::operator = ( const SVByteValueObjectClass& rhs )
{
	__super::operator = (rhs);
	return *this;
}

SVByteValueObjectClass::~SVByteValueObjectClass()
{
}

// This override provides the ability to correctly load script data from the legacy SVByteVectorObjectClass
HRESULT SVByteValueObjectClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT Result( E_FAIL );
	
	if ( isLegacyVectorObjectCompatibility() )
	{
		std::vector<ValueType> svArray;	// for default values

		if (   pDataObject->GetAttributeData( _T("pArray"), svArray )
			|| pDataObject->GetAttributeData( SvDef::cBucketTag, svArray ) )
		{
			SetArraySize( static_cast< int >( svArray.size() ) );
			Result = SetArrayValues( svArray.begin(), svArray.end());
			return Result;
		}
	}

	Result = __super::SetObjectValue( pDataObject );
	return Result;
}

HRESULT SVByteValueObjectClass::SetOutputFormat(OutputFormat outputFormat)
{
	HRESULT Result = S_OK;
	switch (outputFormat)
	{
	case OutputFormat_int:
		setOutputFormat( _T("%d") );
		break;
	case OutputFormat_hex:
		setOutputFormat( _T("0x%2.2x") );
		break;
	default:
		Result = E_INVALIDARG;
		break;
	}
	return Result;
}

BYTE SVByteValueObjectClass::ConvertString2Type(const std::string& rValue ) const
{
	std::string Digits = SvUl::ValidateString( rValue, _T("0123456789 .xXabcdefABCDEF") );
	if ( Digits == rValue )
	{
		SvUl::MakeLower( Digits );
		TCHAR* p = nullptr;
		DWORD Value;
		if ( std::string::npos != Digits.find( 'x' ) )
		{
			Value = _tcstoul( Digits.c_str(), &p, 16 );
		}
		else
		{
			Value = _tcstoul( Digits.c_str(), &p, 10 );
		}

		if ( Value <= 255 )
		{
			return static_cast<BYTE> (Value);
		}
	}
	SvDef::StringVector msgList;
	msgList.push_back(std::string(rValue));
	msgList.push_back(GetName());
	SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return 0; //will never be reached, because the exception will throw before. But this line avoids a warning
}

void SVByteValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter)
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

	// for all elements in the array
	for (int i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		_variant_t Value;
		Value.ChangeType(VT_UI1);
		SVByteValueObjectClass::GetValue(Value.bVal, i);
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVByteValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter)
{
	_variant_t Value(GetDefaultValue());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVByteValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVByteValueObjectType;
	DefaultValue() = 0;
	SetTypeName( _T("Integer8") );
	init();

	SetOutputFormat( OutputFormat_hex );
}

} //namespace SvVol
