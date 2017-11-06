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
#include "SVSystemLibrary/SVFileNameManagerClass.h"
#include "SVUtilityLibrary/SVStringConversions.h"
#include "Definitions/TextDefineSVDef.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVFileNameValueObjectClass, SVFileNameValueObjectClassGuid);

SVFileNameValueObjectClass::SVFileNameValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<SVString>( ObjectName )
{
	LocalInitialize();
}

SVFileNameValueObjectClass::SVFileNameValueObjectClass(	SVObjectClass* pOwner, 	int StringResourceID )
: SVValueObjectClass<SVString>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVFileNameValueObjectClass::SVFileNameValueObjectClass( const SVFileNameValueObjectClass& rhs)
: SVValueObjectClass<SVString>()
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

void SVFileNameValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	SVFileNameManagerClass::Instance().SaveItem(&m_FileName);
	
	__super::Persist( rWriter );
}

HRESULT SVFileNameValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT Result( E_FAIL );
	bool	bOk( false );
	
	SvCl::SVObjectArrayClassTemplate<ValueType> ObjectArray;	// for default values
	BucketVector BucketArray;
	ValueVector ReadValueArray;
	
	if (bOk = pDataObject->GetAttributeData(scDefaultTag, ObjectArray))
	{
		if (0 < ObjectArray.GetSize())
		{
			DefaultValue() = ObjectArray[ObjectArray.GetSize() - 1];
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

		if( !isBucketized() )
		{
			if ( 1 == getArraySize() )
			{
				// In configurations the value are placed in bucket 1
				Value() = BucketArray[1][0];
			}
			else
			{
				// In configurations the values are placed in bucket 1
				std::swap( ValueArray(), BucketArray[1] );
			}
		}
		else
		{
			if ( 1 == getArraySize() )
			{
				if(nullptr != getBucket().get())
				{
					getBucket()->at(0) = BucketArray[0][0];
					getBucket()->at(1) = BucketArray[1][0];
				}
			}
			else
			{
				if(nullptr != getBucketArray().get())
				{
					std::swap( *getBucketArray(), BucketArray );
				}
			}
		}
		//! Note this is required to set the path correctly as it calls the SetValue version of SVFileNameValueObjectClass
		SVString  Value;
		GetValue(Value);
		SetValue(Value);
	}
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData( SvDef::cArrayTag, ReadValueArray, DefaultValue() ) )
	{
		for (size_t i = 0; i < ReadValueArray.size(); i++)
		{
			// Remove any escapes
			SvUl::RemoveEscapedSpecialCharacters(ReadValueArray[i], false);
		}

		SetArraySize( static_cast< int >( ReadValueArray.size() ) );
		if( !isBucketized() )
		{
			if ( 1 == getArraySize() )
			{
				Value() = ReadValueArray[0];
			}
			else
			{
				std::swap( ValueArray(), ReadValueArray );
			}
		}
		//! Note this is required to set the path correctly as it calls the SetValue version of SVFileNameValueObjectClass
		SVString  Value;
		GetValue(Value);
		SetValue(Value);
	}
	else if ( bOk = pDataObject->GetAttributeData(_T("StrDefault"), ObjectArray) )
	{
		if ( 0 < ObjectArray.GetSize() )
		{
			DefaultValue() = ObjectArray[ ObjectArray.GetSize()-1 ];
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

		if( !isBucketized() )
		{
			if ( 1 == getArraySize() )
			{
				// In configurations the value are placed in bucket 1
				Value() = BucketArray[1][0];
			}
			else
			{
				// In configurations the values are placed in bucket 1
				std::swap( ValueArray(), BucketArray[1] );
			}
		}
		else
		{
			if ( 1 == getArraySize() )
			{
				if( nullptr != getBucket().get() )
				{
					getBucket()->at(0) = BucketArray[0][0];
					getBucket()->at(1) = BucketArray[1][0];
				}
			}
			else
			{
				if( nullptr != getBucketArray() )
				{
					std::swap( *getBucketArray(), BucketArray );
				}
			}
		}
		//! Note this is required to set the path correctly as it calls the SetValue version of SVFileNameValueObjectClass
		SVString  Value;
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

HRESULT SVFileNameValueObjectClass::SetValue( const SVString& rValue, int Index /*= -1*/ )
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

HRESULT SVFileNameValueObjectClass::SetDefaultValue( const SVString& rValue, bool bResetAll )
{
	m_FileName.SetDefaultFullFileName( rValue.c_str() );
	
	return __super::SetDefaultValue( rValue, bResetAll );
}

SVString SVFileNameValueObjectClass::ConvertString2Type( const SVString& rValue ) const
{
	return rValue;
}

HRESULT SVFileNameValueObjectClass::CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index /* = -1*/) const
{
	HRESULT Result = ValidateMemoryBlockParameters(pMemoryBlock, MemByteSize, Index);

	if (S_OK == Result)
	{
		SVString Value;
		SVFileNameValueObjectClass::GetValue(Value, Index);
		//! For strings the memory hast to first be cleared
		memset(pMemoryBlock, 0, GetByteSize());
		//! Either the whole string or maximum bytes size -1
		size_t Size = std::min(static_cast<size_t> (GetByteSize() - 1), Value.size());
		memcpy(pMemoryBlock, Value.c_str(), Size);
	}

	return Result;
}

void SVFileNameValueObjectClass::WriteValues(SVObjectWriter& rWriter)
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;

	// Get the Data Values (Member Info, Values)
	SVString TempValue(_T(""));
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

void SVFileNameValueObjectClass::WriteDefaultValues(SVObjectWriter& rWriter)
{
	SVString TempValue(GetDefaultValue());
	_variant_t Value;
	Value.SetString(TempValue.c_str());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVFileNameValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVStringValueObjectType;
	
	SetObjectAttributesAllowed( SvDef::SV_VIEWABLE | SvDef::SV_PUBLISHABLE | SvDef::SV_ARCHIVABLE | SvDef::SV_EMBEDABLE | SvDef::SV_PRINTABLE | SvDef::SV_DD_VALUE, SvOi::SetAttributeType::OverwriteAttribute );

	SetTypeName( _T("FileName") );
	
	InitializeBuckets();

	SVFileNameManagerClass::Instance().AddItem(&m_FileName);
}

