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
	DoubleSortValueObject( LPCTSTR ObjectName );
	DoubleSortValueObject( SVObjectClass* POwner = nullptr , int StringResourceID = IDS_CLASSNAME_SVDOUBLEVALUEOBJECT );
	DoubleSortValueObject( const DoubleSortValueObject& rhs );
	const DoubleSortValueObject& operator = (const DoubleSortValueObject& rhs);

	virtual ~DoubleSortValueObject();
#pragma endregion Constructor

#pragma region Public Methods
public:
	size_t getSortContainerSize() const;
	size_t getSortContainerCapacity() const;
	
	HRESULT setSortContainerPtr(spValueObjectSortContainer sortMap);
	HRESULT setSortContainerDummy(const DummySortContainer& rDummy);
	virtual HRESULT SetValue(const double& rValue, int Index) override;
	virtual HRESULT GetValue(double& rValue, int Index = -1) const override;
	virtual HRESULT getValues(std::vector<_variant_t>&  rValues) const override;
	virtual bool isArray() const override { return true; };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	/// Return the result size. 
	/// ATTENTION: Do not use m_aiResultSize, this must be on ArraySize because otherwise ValidateIndexes can failed even if index valid.
	/// \returns int Result size
	virtual int getResultSize() const override;
	virtual HRESULT GetArrayValues( std::vector<double>& rValues) const override;

	//! Copies the value object to the memory block
	//! \param pMemoryBlock [in] Pointer to the byte address of the memory block
	//! \param MemByteSize [in] The memory block byte size
	//! \returns the number of bytes copied
	virtual long CopyToMemoryBlock(BYTE* pMemoryBlock, long MemByteSize) const override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void LocalInitialize();
#pragma endregion Private Methods

#pragma region Member Variables
private:

	spValueObjectSortContainer m_spSortContainer {nullptr};
	DummySortContainer m_DummySortContainer;
#pragma endregion Member Variables
};

typedef std::shared_ptr<DoubleSortValueObject> DoubleSortValuePtr;

} //namespace SvVol
