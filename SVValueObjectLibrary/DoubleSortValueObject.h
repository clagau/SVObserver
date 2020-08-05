//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file DoubleSortValueObject.h
/// All Rights Reserved 
//*****************************************************************************
/// This class is a doubleValueObject, but the order of the array will be defined by a map.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/circular_buffer.hpp>
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "SVDoubleValueObjectClass.h"
#pragma endregion Includes

namespace SvVol //<SVValueObjectLibrary
{

#pragma region Declarations
typedef boost::circular_buffer<int> ValueObjectSortContainer;
typedef std::shared_ptr<boost::circular_buffer<int>> spValueObjectSortContainer;

#pragma endregion Declarations
/// For performance reason this is used instead of the sortcontainer, when the order is unchanged 
struct DummySortContainer
{
	explicit DummySortContainer(size_t size) :bIsActive {true}, SimpleSize {size}, Capacity {size} { ; };
	DummySortContainer() = default;
	bool bIsActive {false};
	size_t SimpleSize {0};
	size_t  Capacity {0};
};


class DoubleSortValueObject : public SVDoubleValueObjectClass
{
	SV_DECLARE_CLASS( DoubleSortValueObject );
#pragma region Constructor
public:
	explicit DoubleSortValueObject( LPCTSTR ObjectName );
	explicit DoubleSortValueObject( SVObjectClass* POwner = nullptr , int StringResourceID = IDS_CLASSNAME_SVDOUBLEVALUEOBJECT );
	explicit DoubleSortValueObject( const DoubleSortValueObject& rhs );
	const DoubleSortValueObject& operator = (const DoubleSortValueObject& rhs);

	virtual ~DoubleSortValueObject() = default;
#pragma endregion Constructor

#pragma region Public Methods
public:
	size_t getSortContainerSize() const;
	size_t getSortContainerCapacity() const;
	
	HRESULT setSortContainerPtr(spValueObjectSortContainer sortMap);
	HRESULT setSortContainerDummy(const DummySortContainer& rDummy);
	virtual HRESULT SetValue(const double& rValue, int Index) override;
	virtual HRESULT GetValue(double& rValue, int Index = -1) const override;
	virtual HRESULT SetArrayValues(const ValueVector& rValues) override;
	virtual HRESULT getValues(std::vector<_variant_t>&  rValues) const override;
	virtual HRESULT GetArrayValues(std::vector<double>& rValues) const override;
	virtual bool isArray() const override { return true; };

	virtual void setMemBlockPointer(uint8_t* pMemBlockBase) override;
	virtual void updateMemBlockData() override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	/// Return the result size. 
	/// ATTENTION: Do not use m_ResultSize, this must be on ArraySize because otherwise ValidateIndexes can failed even if index valid.
	/// \returns int Result size
	virtual int32_t getResultSize() const override;

	virtual double* reserveLocalMemory() override;
	virtual void clearMemoryBlockPointer() override { m_pMemBlockData = nullptr; }

	virtual HRESULT ValidateIndex(int ArrayIndex) const;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void LocalInitialize();
#pragma endregion Private Methods

#pragma region Member Variables
private:

	spValueObjectSortContainer m_spSortContainer {nullptr};
	DummySortContainer m_DummySortContainer;

	///DoubleSortValue value objects have specialized data and the allocateDataMemory points to this vector
	std::vector<double> m_doubleData;

	///The memory block pointer where the sorted double are to be copied too
	uint8_t* m_pMemBlockData {nullptr};
#pragma endregion Member Variables
};

typedef std::shared_ptr<DoubleSortValueObject> DoubleSortValuePtr;

} //namespace SvVol
