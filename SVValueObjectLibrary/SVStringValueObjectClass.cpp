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

namespace SvVol
{

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

long SVStringValueObjectClass::GetByteSize(bool useResultSize) const
{
	long result(0L);

	//Attribute must be set otherwise do not consider for memory requirements
	if (0 != ObjectAttributesAllowed())
	{
		int numberOfElements = useResultSize ? getResultSize() : getArraySize();
		if(0 == m_maxByteSize)
		{
			for (int i = 0; i < numberOfElements; ++i)
			{
				std::string valueString;
				SVStringValueObjectClass::GetValue(valueString, i);
				//Add place for the ending \0 of the string
				result += static_cast<long> (valueString.size() + 1);
			}
		}
		else
		{
			result = m_maxByteSize;
			result *= numberOfElements;
		}
		//If the value object is an array the first value shall contain the result size which is variable
		if (isArray())
		{
			result += sizeof(int);
		}
	}

	return result;
}

long SVStringValueObjectClass::CopyToMemoryBlock(BYTE* pMemoryBlock, long MemByteSize) const
{
	long result {0L};

	//Attribute must be set otherwise do not consider for memory requirements
	if (0 != ObjectAttributesAllowed() && -1 != GetMemOffset())
	{
		result = GetByteSize(false);
		if (result <= MemByteSize)
		{
			BYTE* pMemoryLocation = pMemoryBlock + GetMemOffset();
			if (isArray())
			{
				//For arrays we need to write the result size at the start of the memory as an int
				*(reinterpret_cast<int*> (pMemoryLocation)) = getResultSize();
				pMemoryLocation += sizeof(int);
			}
			for(int i=0; i < getResultSize(); ++i)
			{
				std::string valueString;
				SVStringValueObjectClass::GetValue(valueString, i);
				//Copy also the ending \0 of the string
				memcpy(pMemoryLocation, valueString.c_str(), valueString.size() + 1);
				pMemoryLocation += valueString.size() + 1;
			}
		}
		else
		{
			result = -1L;
		}
	}
	return result;
}

void SVStringValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter)
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

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

} //namespace SvVol
