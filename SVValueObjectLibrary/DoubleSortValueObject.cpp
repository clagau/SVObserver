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
HRESULT DoubleSortValueObject::setSortContainer(const ValueObjectSortContainer& sortMap) 
{
	HRESULT result(E_FAIL);
	if (m_isCreated)
	{
		m_sortContainer = sortMap;
		result = S_OK;
	}
	return result;
}

#pragma endregion Public Methods

#pragma region Protected Methods
HRESULT DoubleSortValueObject::SetValue(const double& rValue, int Index )
{
	if (0 <= Index && m_sortContainer.size() > Index)
	{
		return SVValueObjectClass<double>::SetValue( rValue, m_sortContainer[Index] );
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::GetValue( double& rValue, int Index) const
{
	//! When Value Object is an array and index is -1 then use first index
	if (0 > Index)
	{
		Index = 0;
	}
	rValue = 0.0;
	if(0 <= Index)
	{
		if (m_sortContainer.size() > Index)
		{
			Index = m_sortContainer[Index];
			return SVValueObjectClass<double>::GetValue(rValue, Index);
		}
		else
		{
			return E_BOUNDS;
		}
	}
	return E_FAIL;
}


HRESULT DoubleSortValueObject::CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index /* = -1*/) const
{
	//! When Value Object is an array and index is -1 then use first index
	if (isArray() && 0 > Index)
	{
		Index = 0;
	}
	if (0 <= Index)
	{
		if (m_sortContainer.size() > Index)
		{
			Index = m_sortContainer[Index];
			return SVValueObjectClass<double>::CopyToMemoryBlock(pMemoryBlock, MemByteSize, Index);
		}
		else
		{
			return E_BOUNDS;
		}
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::getValues( std::vector<_variant_t>&  rValues) const
{
	HRESULT Result( S_OK );

	int ResultSize = getResultSize();
	assert( ResultSize <= getArraySize() );
	rValues.resize( ResultSize );
	_variant_t Value;
	for( int i=0; i < ResultSize && S_OK == Result; i++ )
	{
		//must be get once by once, because values can be disorder and not in a row.
		Result = getValue(Value, i);
		rValues[i] = Value;
	}

	return Result;
}

HRESULT DoubleSortValueObject::GetArrayValues(std::vector<double>& rValues) const
{
	HRESULT Result(S_OK);

	int iResultSize = getResultSize();
	assert( iResultSize <= getArraySize() );
	rValues.resize( iResultSize );
	double value = 0;
	for (int i=0; i<iResultSize && S_OK==Result; i++)
	{
		//must be get once by once, because values can be disorder and not in a row.
		Result = GetValue(value, i);
		rValues[i] = value;
	}

	return Result;
}

#pragma endregion Protected Methods

#pragma region Private Methods
void DoubleSortValueObject::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::DoubleSortValueObjectType;
	SetTypeName( _T("Decimal") );

	//normally the DoubleSortValueObject is a result value and should not be print. 
	//If a parameter should be printed, it has to be set after creating.
	SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
}
#pragma endregion Private Methods
