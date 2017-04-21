//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file DoubleSortValueObject.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This class is a doubleValueObject, but the order of the array will be defined by a map.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DoubleSortValueObject.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "SVStatusLibrary\MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(DoubleSortValueObject, DoubleSortValueObjectGuid);
#pragma endregion Declarations

#pragma region Constructor
DoubleSortValueObject::DoubleSortValueObject(LPCTSTR ObjectName)
	: SVDoubleValueObjectClass(ObjectName) 
{
	LocalInitialize();
}

DoubleSortValueObject::DoubleSortValueObject(SVObjectClass* POwner, int StringResourceID)
	: SVDoubleValueObjectClass(POwner, StringResourceID) 
{
	LocalInitialize();
}

DoubleSortValueObject::DoubleSortValueObject( const DoubleSortValueObject& rhs )
	: SVDoubleValueObjectClass()
{
	LocalInitialize();
	*this = rhs;
}

const DoubleSortValueObject& DoubleSortValueObject::operator = (const DoubleSortValueObject& rhs)
{
	__super::operator = (rhs);
	return *this;
}

DoubleSortValueObject::~DoubleSortValueObject()
{
}
#pragma endregion Constructor

#pragma region Public Methods
const ValueObjectSortContainer& DoubleSortValueObject::getSortContainer(int iBucket) const 
{
	assert(iBucket >= 0 && iBucket < getNumberOfBuckets() );
	if (iBucket < 0 || iBucket > getNumberOfBuckets() )
	{
		iBucket = 0;
	}
	
	return m_sortContainerArray[iBucket];
}

HRESULT DoubleSortValueObject::setSortContainer(int iBucket, const ValueObjectSortContainer& sortMap) 
{
	HRESULT result(E_FAIL);
	if (iBucket >= 0 && iBucket < getNumberOfBuckets() && m_isCreated)
	{
		m_sortContainerArray[iBucket] = sortMap;
		result = S_OK;
	}
	return result;
}

HRESULT DoubleSortValueObject::CopyValue(int iSourceBucket, int iDestBucket)
{
	if ((iSourceBucket >= 0 && iSourceBucket < getNumberOfBuckets()) && (iDestBucket >= 0 && iDestBucket < getNumberOfBuckets()))
	{
		m_sortContainerArray[iDestBucket] = m_sortContainerArray[iSourceBucket];
		return __super::CopyValue(iSourceBucket, iDestBucket);
	}

	return S_FALSE;
}
#pragma endregion Public Methods

#pragma region Protected Methods
void DoubleSortValueObject::CreateBuckets( )
{
	__super::CreateBuckets();
	m_sortContainerArray.resize(getNumberOfBuckets());
}

HRESULT DoubleSortValueObject::SetValue(const double& rValue, int Bucket, int Index )
{
	Bucket = (-1 == Bucket) ? GetLastSetIndex() : Bucket;
	if (0 <= Bucket && Bucket < getNumberOfBuckets() && 0 <= Index && m_sortContainerArray[Bucket].size() > Index)
	{
		return SVValueObjectClass<double>::SetValue( rValue, Bucket, m_sortContainerArray[Bucket][Index] );
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::GetValue( double& rValue, int Bucket, int Index ) const
{
	Bucket = (-1 == Bucket) ? GetLastSetIndex() : Bucket;
	if( 0 <= Bucket && Bucket < getNumberOfBuckets() && 0 <= Index )
	{
		if (m_sortContainerArray[Bucket].size() > Index)
		{
			return SVValueObjectClass<double>::GetValue(rValue, Bucket, m_sortContainerArray[Bucket][Index]);
		}
		else
		{
			return E_BOUNDS;
		}
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::getValues( std::vector<_variant_t>&  rValues, int Bucket ) const
{
	HRESULT Result( E_FAIL );

	Bucket = (-1 == Bucket) ? GetLastSetIndex() : Bucket;
	assert( Bucket >= 0 && Bucket < getNumberOfBuckets() );

	if( Bucket >= 0 && Bucket < getNumberOfBuckets() )
	{
		int ResultSize = getResultSize( Bucket );
		assert( ResultSize <= getArraySize() );
		rValues.resize( ResultSize );
		_variant_t Value;
		for( int i=0; i< ResultSize; i++ )
		{
			//must be get once by once, because values can be disorder and not in a row.
			getValue( Value, Bucket, i );
			rValues[i] = Value;
		}
		Result = S_OK;
	}
	return Result;
}

int DoubleSortValueObject::getResultSize(int Bucket) const
{
	Bucket = (-1 == Bucket) ? GetLastSetIndex() : Bucket;
	assert(0 <= Bucket && Bucket < getNumberOfBuckets());
	return static_cast<int> (getSortContainer(Bucket).size());
}

HRESULT DoubleSortValueObject::GetArrayValues( std::vector<double>& rValues, int Bucket ) const
{
	HRESULT hrOk = E_FAIL;
	Bucket = (-1 == Bucket) ? GetLastSetIndex() : Bucket;

	assert( Bucket >= 0 && Bucket < getNumberOfBuckets() );
	if ( 0 <= Bucket && Bucket < getNumberOfBuckets() )
	{
		int iResultSize = getResultSize(Bucket);
		assert( iResultSize <= getArraySize() );
		rValues.resize( iResultSize );
		double value = 0;
		for (int i=0; i<iResultSize; i++)
		{
			//must be get once by once, because values can be disorder and not in a row.
			GetValue( value, Bucket, i );
			rValues[i] = value;
		}
		hrOk = S_OK;
	}
	return hrOk;
}

#pragma endregion Protected Methods

#pragma region Private Methods
void DoubleSortValueObject::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = DoubleSortValueObjectType;
	SetTypeName( _T("Decimal") );
}
#pragma endregion Private Methods
