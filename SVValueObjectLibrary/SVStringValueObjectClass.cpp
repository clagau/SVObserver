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
#include "SVUtilityLibrary/SVStringConversions.h"
#include "ObjectInterfaces/TextDefineSvOi.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVStringValueObjectClass, SVStringValueObjectClassGuid);

SVStringValueObjectClass::SVStringValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<SVString>( ObjectName ) 
{
	LocalInitialize();
}

SVStringValueObjectClass::SVStringValueObjectClass(	SVObjectClass* pOwner, 	int StringResourceID )
: SVValueObjectClass<SVString>( pOwner, StringResourceID ) 
{
	LocalInitialize();
}

SVStringValueObjectClass::SVStringValueObjectClass( const SVStringValueObjectClass& rhs)
: SVValueObjectClass<SVString>()
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

void SVStringValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the ClassID, ObjectID, Attrributes
	SVValueObjectClass::Persist(rWriter);

	// Get the Data Values (Member Info, Values)
	// Check for DoubleQuotes in variable
	SVString TempValue( GetDefaultValue() );
	SvUl::AddEscapeSpecialCharacters( TempValue, true );
	
	_variant_t Value;
	Value.SetString(TempValue.c_str());
	rWriter.WriteAttribute(scDefaultTag, Value);
	Value.Clear();

	rWriter.StartElement(scArrayElementsTag);

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;
	
	// for all elements in the array
	for (int i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		SVStringValueObjectClass::GetValue( TempValue, GetLastSetIndex(), i );
		SvUl::AddEscapeSpecialCharacters( TempValue, true );
		Value.SetString( TempValue.c_str() );
		list.push_back( Value );
		Value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT  SVStringValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT Result( E_FAIL );
	BOOL	bOk( false );

	SvCl::SVObjectArrayClassTemplate<ValueType> ObjectArray;	// for default values
	BucketVector BucketArray;
	ValueVector ReadValueArray;

	if ( bOk = pDataObject->GetAttributeData( SvOi::cDefaultTag, ObjectArray ) )
	{
		if ( 0 < ObjectArray.GetSize() )
		{
			DefaultValue() = ObjectArray[ ObjectArray.GetSize()-1 ];
			SvUl::RemoveEscapedSpecialCharacters( DefaultValue(), false );
		}

		SetDefaultValue( GetDefaultValue().c_str(), false );
	}
	else if ( bOk = pDataObject->GetAttributeData( SvOi::cBucketTag, BucketArray, DefaultValue() ) )
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
		if ( 1 < getNumberOfBuckets() )
		{
			setLastSetIndex( 1 );
		}

		SVString  Value;
		GetValue( Value );
		SetValue( Value );
	}
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData( SvOi::cArrayTag, ReadValueArray, DefaultValue() ) )
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
		setLastSetIndex( 1 );

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
				if( nullptr != getBucket() )
				{
					getBucket()->at(0) = BucketArray[0][0];
					getBucket()->at(1) = BucketArray[0][0];
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
		if ( 1 < getNumberOfBuckets() )
		{
			setLastSetIndex( 1 );
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

SVString SVStringValueObjectClass::ConvertString2Type( const SVString& rValue ) const
{
	return rValue;
}

void SVStringValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVStringValueObjectType;
	
	SetObjectAttributesAllowed( SV_VIEWABLE | SV_PUBLISHABLE | SV_ARCHIVABLE | SV_EMBEDABLE | SV_PRINTABLE | SV_DD_VALUE, SvOi::SetAttributeType::OverwriteAttribute );
	SetTypeName( _T("Text") );

	InitializeBuckets();
}
