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

namespace SvVol
{

#pragma region Declarations
typedef boost::circular_buffer<int> ValueObjectSortContainer;
#pragma endregion Declarations

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
	const ValueObjectSortContainer& getSortContainer() const { return m_sortContainer; };
	HRESULT setSortContainer(const ValueObjectSortContainer& sortMap);
	virtual HRESULT SetValue(const double& rValue, int Index) override;
	virtual HRESULT GetValue(double& rValue, int Index = -1) const override;
	virtual HRESULT getValues(std::vector<_variant_t>&  rValues) const override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	/// Return the result size. 
	/// ATTENTION: Do not use m_aiResultSize, this must be on ArraySize because otherwise ValidateIndexes can failed even if index valid.
	/// \returns int Result size
	virtual int getResultSize() const override { return static_cast<int> (m_sortContainer.size()); };
	virtual HRESULT GetArrayValues( std::vector<double>& rValues) const override;

	//! Copies the value object to the memory block
	//! \param pMemoryBlock [in] Pointer to the byte address of the memory block
	//! \param MemByteSize [in] The memory block byte size
	//! \param Index [in] The index of the array (-1 if no array)
	//! \returns S_OK if successful
	virtual HRESULT CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index = -1) const override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void LocalInitialize();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	ValueObjectSortContainer m_sortContainer;
#pragma endregion Member Variables
};

typedef std::shared_ptr<DoubleSortValueObject> DoubleSortValuePtr;

} //namespace SvVol
