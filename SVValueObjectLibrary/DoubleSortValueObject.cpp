//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DoubleSortValueObject.cpp
/// All Rights Reserved 
//*****************************************************************************
/// This class is a doubleValueObject, but the order of the array will be defined by a map.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DoubleSortValueObject.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS(DoubleSortValueObject, SvPb::DoubleSortValueClassId);
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
#pragma endregion Constructor

#pragma region Public Methods
HRESULT DoubleSortValueObject::setSortContainerPtr(spValueObjectSortContainer sortMap)
{
	HRESULT result(E_FAIL);
	if (m_isCreated)
	{
		m_DummySortContainer.bIsActive = false;
		m_spSortContainer.swap(sortMap);
		SetResultSize(static_cast<int32_t> (getSortContainerSize()));
		setHasChanged(true);
		result = S_OK;
	}
	return result;
}
HRESULT DoubleSortValueObject::setSortContainerDummy(const DummySortContainer& rDummy)
{
	HRESULT result(E_FAIL);
	if (m_isCreated)
	{
		m_DummySortContainer = rDummy;
		SetResultSize(static_cast<int32_t> (getSortContainerSize()));
		setHasChanged(true);
		result = S_OK;
	}
	return result;
}



#pragma endregion Public Methods

#pragma region Protected Methods
HRESULT DoubleSortValueObject::SetValue(const double& rValue, int Index )
{

	if (m_DummySortContainer.bIsActive)
	{
		if (0 <= Index && m_DummySortContainer.SimpleSize > Index)
		{
			setHasChanged(true);
			return SVValueObjectClass<double>::SetValue(rValue, Index);
		}
	}
	else if (0 <= Index && m_spSortContainer.get()&& m_spSortContainer->size() > Index)
	{
		setHasChanged(true);
		return SVValueObjectClass<double>::SetValue( rValue, m_spSortContainer->at(Index));
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
		if (m_DummySortContainer.bIsActive)
		{
			if (m_DummySortContainer.SimpleSize > Index)
			{
				return SVValueObjectClass<double>::GetValue(rValue, Index);
			}
		}
		else if (m_spSortContainer.get() && m_spSortContainer->size() > Index)
		{
			Index = m_spSortContainer->at(Index);
			return SVValueObjectClass<double>::GetValue(rValue, Index);
		}
		
		return E_BOUNDS;
	}
	return E_FAIL;
}

HRESULT DoubleSortValueObject::SetArrayValues(const ValueVector& rValues)
{
	HRESULT Result(E_FAIL);

	int32_t Size = static_cast<int32_t> (rValues.size());
	assert(Size <= getArraySize());
 	if (Size <= m_doubleData.size())
	{
		SetResultSize(Size);
		if (0 < Size && m_DummySortContainer.bIsActive)
		{
			std::copy(rValues.begin(), rValues.end(), m_doubleData.begin());
			setHasChanged(true);
		}
		else
		{
			for (int i = 0; i < Size; i++)
			{
				//must get one by one, because values can be disordered
				SetValue(rValues[i], i);
			}
		}
		Result = S_OK;
	}
	else
	{
		Result = SVMSG_SVO_33_OBJECT_INDEX_INVALID;
	}
	return Result;
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

void DoubleSortValueObject::setMemBlockPointer(uint8_t* pMemBlockBase)
{
	int32_t memOffset = getMemOffset();
	if (nullptr != pMemBlockBase && -1 != memOffset)
	{
		///This is always an array
		setResultSizePointer(reinterpret_cast<int32_t*> (pMemBlockBase + memOffset));
		m_pMemBlockData = (pMemBlockBase + memOffset + sizeof(int32_t));
		updateMemBlockData();
	}
}

void DoubleSortValueObject::updateMemBlockData()
{
	///The result size for the mem block array parameter size needs to be set as it may have changed
	SetResultSize(getResultSize());
	///Here we only want the data byte size
	int32_t dataByteSize = getByteSize(true, false);
	///This is to make sure that enough space has been reserved for memory block data and the object has attributes
	if (0 < dataByteSize && dataByteSize <= getMemSizeReserved() && nullptr != m_pMemBlockData)
	{
		if (hasChanged())
		{
			uint8_t* pMemoryLocation = m_pMemBlockData;
			std::vector<double> sortedResult;
			if(S_OK == GetArrayValues(sortedResult) && dataByteSize == static_cast<int32_t> (sortedResult.size() * sizeof(double)))
			{
				memcpy(pMemoryLocation, &sortedResult.at(0), dataByteSize);
			}
			setHasChanged(false);
		}
	}
	else
	{
		if (0 < getMemSizeReserved() && nullptr != m_pMemBlockData)
		{
			///Clear the memory block data
			memset(m_pMemBlockData, 0, getMemSizeReserved());
			setHasChanged(false);
			if(0 < dataByteSize)
			{
				///Memory block reserved for value object is to small. This should not happen!
				assert(false);
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorMemoryBlockDataReservedSize, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			}
		}
	}
}

HRESULT DoubleSortValueObject::GetArrayValues(std::vector<double>& rValues) const
{
	HRESULT Result(S_OK);

	int resultSize = getResultSize();
	assert( resultSize <= getArraySize() );
	rValues.resize( resultSize );
	if (m_DummySortContainer.bIsActive && m_doubleData.size())
	{
		std::copy(m_doubleData.begin(), m_doubleData.begin() + resultSize, rValues.begin());
	}
	else
	{
		double value = 0;
		for (int i=0; i < resultSize && S_OK==Result; i++)
		{
			//must get one by one, because values can be disordered
			Result = GetValue(value, i);
			rValues[i] = value;
		}
	}

	return Result;
}

double* DoubleSortValueObject::reserveLocalMemory()
{
	double* pResult {nullptr};
	if (m_doubleData.size() != static_cast<size_t> (getArraySize()))
	{
		m_doubleData.resize(getArraySize(), GetDefaultValue());
	}
	if (0 < m_doubleData.size())
	{
		pResult = &m_doubleData.at(0);
	}
	return pResult;
}


size_t DoubleSortValueObject::getSortContainerSize() const 
{ 
	if (m_DummySortContainer.bIsActive)
	{
		return m_DummySortContainer.SimpleSize;
	}
	else if (m_spSortContainer)
	{
		return m_spSortContainer->size();
	}
	else
	{
		return 0;
	}
}
size_t  DoubleSortValueObject::getSortContainerCapacity() const
{
	if (m_DummySortContainer.bIsActive)
	{
		return m_DummySortContainer.Capacity;
	}
	else if (m_spSortContainer)
	{
		return m_spSortContainer->capacity();
	}
	else
	{
		return 0;
	}
}
int32_t  DoubleSortValueObject::getResultSize() const
{
	int32_t result{static_cast<int32_t> (getSortContainerSize())};
	return result;
}

HRESULT DoubleSortValueObject::ValidateIndex(int ArrayIndex) const
{
	HRESULT result = __super::ValidateIndex(ArrayIndex);

	///The array index for double sort can be out of range as long as it is smaller than the array size
	///Note the base function already tests that the array index > 0
	if(SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == result && ArrayIndex < getArraySize())
	{
		result = S_OK;
	}

	return result;
}

#pragma endregion Protected Methods

#pragma region Private Methods
void DoubleSortValueObject::LocalInitialize()
{
	m_ObjectTypeInfo.m_SubType = SvPb::DoubleSortValueObjectType;
	SetTypeName( _T("Decimal") );

	//normally the DoubleSortValueObject is a result value and should not be print. 
	//If a parameter should be printed, it has to be set after creating.
	SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
}

#pragma endregion Private Methods



} //namespace SvVol
