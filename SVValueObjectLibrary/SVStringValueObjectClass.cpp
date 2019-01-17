//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringValueObjectClass
//* .File Name       : $Workfile:   SVStringValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   02 Oct 2013 08:17:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVStringValueObjectClass.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Definitions/TextDefineSVDef.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVStringValueObjectClass, SVStringValueObjectClassGuid);

SVStringValueObjectClass::SVStringValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<std::string>( ObjectName )
{
	LocalInitialize();
}

SVStringValueObjectClass::SVStringValueObjectClass(	SVObjectClass* pOwner, 	int StringResourceID )
: SVValueObjectClass<std::string>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVStringValueObjectClass::SVStringValueObjectClass( const SVStringValueObjectClass& rhs)
: SVValueObjectClass<std::string>()
{
	LocalInitialize();
	*this = rhs;
}

const SVStringValueObjectClass& SVStringValueObjectClass::operator = (const SVStringValueObjectClass& rhs)
{
	__super::operator = (rhs);
	return *this;
}

SVStringValueObjectClass::~SVStringValueObjectClass()
{
}

HRESULT  SVStringValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT Result( E_FAIL );
	bool	bOk( false );

	std::vector<ValueVector> BucketArray;		//This is for backward compatibility
	ValueVector ValueArray;

	if (bOk = pDataObject->GetAttributeData(scDefaultTag, ValueArray))
	{
		if (0 < ValueArray.size())
		{
			DefaultValue() = ValueArray[ValueArray.size() - 1];
			SvUl::RemoveEscapedSpecialCharacters(DefaultValue(), true);
		}

		SetDefaultValue(GetDefaultValue().c_str(), false);
	}
	else if(bOk = pDataObject->GetAttributeData(SvDef::cBucketTag, BucketArray, DefaultValue()))
	{
		for (size_t i = 0; i < BucketArray.size(); i++)
		{
			SvUl::RemoveEscapedSpecialCharacters(BucketArray[i][0], true);
		}

		if ( 1 == getArraySize() )
		{
			// In configurations the value are placed in bucket 1
			Value() = BucketArray[1][0];
		}
		else
		{
			// In configurations the values are placed in bucket 1
			std::swap( __super::ValueArray(), BucketArray[1] );
		}
	}
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData( SvDef::cArrayTag, ValueArray, DefaultValue() ) )
	{
		for (size_t i = 0; i < ValueArray.size(); i++)
		{
			// Remove any escapes
			SvUl::RemoveEscapedSpecialCharacters(ValueArray[i], true);
		}

		SetArraySize(static_cast<int> (ValueArray.size()));
		if ( 1 == getArraySize() )
		{
			Value() = ValueArray[0];
		}
		else
		{
			std::swap(__super::ValueArray(), ValueArray);
		}
	}
	else if ( bOk = pDataObject->GetAttributeData(_T("StrDefault"), ValueArray) )
	{
		if ( 0 < ValueArray.size() )
		{
			DefaultValue() = ValueArray[ValueArray.size() - 1];
			SvUl::RemoveEscapedSpecialCharacters( DefaultValue(), true );
		}

		SetDefaultValue( GetDefaultValue().c_str(), false );
	}
	else if ( bOk = pDataObject->GetAttributeData( _T("StrArray"), BucketArray, DefaultValue() ) )
	{
		for (size_t i = 0; i < BucketArray.size(); i++)
		{
			SvUl::RemoveEscapedSpecialCharacters(BucketArray[i][0], true);
		}

		if ( 1 == getArraySize() )
		{
			// In configurations the value are placed in bucket 1
			Value() = BucketArray[1][0];
		}
		else
		{
			// In configurations the values are placed in bucket 1
			std::swap(__super::ValueArray(), BucketArray[1] );
		}
	}
	else
	{
		Result = __super::SetObjectValue(pDataObject);
		return Result;
	}

	Result = bOk ? S_OK : E_FAIL;
	return Result;
}

std::string SVStringValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	return rValue;
}

HRESULT SVStringValueObjectClass::CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index /* = -1*/) const
{
	HRESULT Result = ValidateMemoryBlockParameters(pMemoryBlock, MemByteSize, Index);

	if (S_OK == Result)
	{
		std::string Value;
		SVStringValueObjectClass::GetValue(Value, Index);
		//! For strings the memory hast to first be cleared
		memset(pMemoryBlock, 0, GetByteSize());
		//! Either the whole string or maximum bytes size -1
		size_t Size = std::min( static_cast<size_t> (GetByteSize() - 1), Value.size());
		memcpy(pMemoryBlock, Value.c_str(), Size);
	}

	return Result;
}

void SVStringValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter)
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;

	// Get the Data Values (Member Info, Values)
	// Check for DoubleQuotes in variable
	std::string TempValue(_T(""));
	SvUl::AddEscapeSpecialCharacters(TempValue, true);

	_variant_t Value;
	Value.Clear();

	// for all elements in the array
	for (int i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		SVStringValueObjectClass::GetValue(TempValue, i);
		SvUl::AddEscapeSpecialCharacters(TempValue, true);
		Value.SetString(TempValue.c_str());
		list.push_back(Value);
		Value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVStringValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter)
{
	std::string TempValue(GetDefaultValue());
	SvUl::AddEscapeSpecialCharacters(TempValue, true);
	_variant_t Value;
	Value.SetString(TempValue.c_str());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVStringValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVStringValueObjectType;
	
	SetObjectAttributesAllowed( SvPb::viewable | SvPb::publishable | SvPb::archivable | SvPb::embedable | SvPb::printable | SvPb::dataDefinitionValue, SvOi::SetAttributeType::OverwriteAttribute );
	SetTypeName( _T("Text") );

	init();
}
