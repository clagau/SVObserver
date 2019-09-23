//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameValueObjectClass
//* .File Name       : $Workfile:   SVFileNameValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVFileNameValueObjectClass.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
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

SV_IMPLEMENT_CLASS(SVFileNameValueObjectClass, SVFileNameValueObjectClassGuid);

SVFileNameValueObjectClass::SVFileNameValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<std::string>( ObjectName )
{
	LocalInitialize();
}

SVFileNameValueObjectClass::SVFileNameValueObjectClass(	SVObjectClass* pOwner, 	int StringResourceID )
: SVValueObjectClass<std::string>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVFileNameValueObjectClass::SVFileNameValueObjectClass( const SVFileNameValueObjectClass& rhs)
: SVValueObjectClass<std::string>()
{
	LocalInitialize();
	*this = rhs;
}

const SVFileNameValueObjectClass& SVFileNameValueObjectClass::operator = ( const SVFileNameValueObjectClass& rhs )
{
	__super::operator = (rhs);
	return *this;
}

SVFileNameValueObjectClass::~SVFileNameValueObjectClass()
{
	SVFileNameManagerClass::Instance().RemoveItem(&m_FileName);
}

HRESULT SVFileNameValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
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
			SvUl::RemoveEscapedSpecialCharacters(DefaultValue(), false);
		}

		SetDefaultValue(GetDefaultValue().c_str(), false);
	}
	else if (bOk = pDataObject->GetAttributeData(SvDef::cBucketTag, BucketArray, DefaultValue()))
	{
		for (size_t i = 0; i < BucketArray.size(); i++)
		{
			SvUl::RemoveEscapedSpecialCharacters(BucketArray[i][0], false);
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
		//! Note this is required to set the path correctly as it calls the SetValue version of SVFileNameValueObjectClass
		std::string  Value;
		GetValue(Value);
		SetValue(Value);
	}
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData( SvDef::cArrayTag, ValueArray, DefaultValue() ) )
	{
		for (size_t i = 0; i < ValueArray.size(); i++)
		{
			// Remove any escapes
			SvUl::RemoveEscapedSpecialCharacters(ValueArray[i], false);
		}

		SetArraySize( static_cast< int >(ValueArray.size() ) );
		if ( 1 == getArraySize() )
		{
			Value() = ValueArray[0];
		}
		else
		{
			std::swap(__super::ValueArray(), ValueArray);
		}
		//! Note this is required to set the path correctly as it calls the SetValue version of SVFileNameValueObjectClass
		std::string  Value;
		GetValue(Value);
		SetValue(Value);
	}
	else if ( bOk = pDataObject->GetAttributeData(_T("StrDefault"), ValueArray) )
	{
		if ( 0 < ValueArray.size() )
		{
			DefaultValue() = ValueArray[ValueArray.size()-1];
			SvUl::RemoveEscapedSpecialCharacters( DefaultValue(), false );
		}

		SetDefaultValue( GetDefaultValue().c_str(), false );
	}
	else if ( bOk = pDataObject->GetAttributeData( _T("StrArray"), BucketArray, DefaultValue() ) )
	{
		for (size_t i = 0; i < BucketArray.size(); i++)
		{
			SvUl::RemoveEscapedSpecialCharacters(BucketArray[i][0], false);
		}

		if ( 1 == getArraySize() )
		{
			// In configurations the value are placed in bucket 1
			Value() = BucketArray[1][0];
		}
		else
		{
			// In configurations the values are placed in bucket 1
			std::swap(__super::ValueArray(), BucketArray[1]);
		}
		//! Note this is required to set the path correctly as it calls the SetValue version of SVFileNameValueObjectClass
		std::string  Value;
		GetValue(Value);
		SetValue(Value);
	}
	else
	{
		Result = __super::SetObjectValue(pDataObject);
		return Result;
	}
	
	Result = bOk ? S_OK : E_FAIL;
	return Result;
}

HRESULT SVFileNameValueObjectClass::SetValue( const std::string& rValue, int Index /*= -1*/ )
{
	HRESULT Result = ValidateIndex(Index);

	if ( S_OK == Result || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == Result )
	{
		m_FileName.SetFullFileName( rValue.c_str() );
		SVFileNameManagerClass::Instance().LoadItem( &m_FileName );
		Result = __super::SetValue( m_FileName.GetFullFileName(), Index );
	}
	
	return Result;
}

HRESULT SVFileNameValueObjectClass::SetDefaultValue( const std::string& rValue, bool bResetAll )
{
	m_FileName.SetDefaultFullFileName( rValue.c_str() );
	
	return __super::SetDefaultValue( rValue, bResetAll );
}

std::string SVFileNameValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	return rValue;
}

long SVFileNameValueObjectClass::GetByteSize(bool useResultSize) const
{
	long result(0L);

	//Attribute must be set otherwise do not consider for memory requirements
	if (0 != ObjectAttributesAllowed())
	{
		int numberOfElements = useResultSize ? getResultSize() : getArraySize();
		for (int i = 0; i < numberOfElements; ++i)
		{
			std::string valueString;
			SVFileNameValueObjectClass::GetValue(valueString, i);
			//Add place for the ending \0 of the string
			result += static_cast<long> (valueString.size() + 1);
		}
		//If the value object is an array the first value shall contain the result size which is variable
		if (isArray())
		{
			result += sizeof(int);
		}
	}

	return result;
}

long SVFileNameValueObjectClass::CopyToMemoryBlock(BYTE* pMemoryBlock, long MemByteSize) const
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
			for (int i = 0; i < getResultSize(); ++i)
			{
				std::string valueString;
				SVFileNameValueObjectClass::GetValue(valueString, i);
				//Copy also the ending \0 of the string
				memcpy(pMemoryLocation, valueString.c_str(), valueString.size() + 1);
			}
		}
		else
		{
			result = -1L;
		}
	}
	return result;
}

void SVFileNameValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter)
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

	// Get the Data Values (Member Info, Values)
	std::string TempValue(_T(""));
	_variant_t Value;
	Value.Clear();

	// for all elements in the array
	for (int i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		SVFileNameValueObjectClass::GetValue(TempValue, i);
		Value.SetString(TempValue.c_str());
		list.push_back(Value);
		Value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVFileNameValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter)
{
	std::string TempValue(GetDefaultValue());
	_variant_t Value;
	Value.SetString(TempValue.c_str());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVFileNameValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVStringValueObjectType;
	
	SetObjectAttributesAllowed( SvPb::viewable | SvPb::publishable | SvPb::archivable | SvPb::embedable | SvPb::printable | SvPb::dataDefinitionValue, SvOi::SetAttributeType::OverwriteAttribute );

	SetTypeName( _T("FileName") );
	
	init();

	SVFileNameManagerClass::Instance().AddItem(&m_FileName);
}

} //namespace SvVol
